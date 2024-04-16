/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "notifier_mgr.h"

#include <vector>

#include "accesstoken_kit.h"
#include "ipc_skeleton.h"
#include "app_mgr_constants.h"
#include "parameters.h"
#include "res_sched_log.h"
#include "res_sched_notifier_death_recipient.h"
#include "res_sched_systemload_notifier_proxy.h"

namespace OHOS {
namespace ResourceSchedule {
using OHOS::AppExecFwk::ApplicationState;

static const std::string SYSTEMLOAD_CHANGE = "systemLoadChange";

static std::map<ResType::DeviceStatus, std::string> g_DeviceStatusType = {
    { ResType::DeviceStatus::SYSTEMLOAD_LEVEL, SYSTEMLOAD_CHANGE }
};

static std::vector<std::pair<std::string, ResType::SystemloadLevel>> g_systemloadPair = {
    { "LOW", ResType::SystemloadLevel::LOW },
    { "NORMAL", ResType::SystemloadLevel::NORMAL },
    { "MEDIUM", ResType::SystemloadLevel::MEDIUM },
    { "HIGH", ResType::SystemloadLevel::HIGH },
    { "OVERHEATED", ResType::SystemloadLevel::OVERHEATED },
    { "WARNING", ResType::SystemloadLevel::WARNING },
    { "EMERGENCY", ResType::SystemloadLevel::EMERGENCY },
    { "ESCAPE", ResType::SystemloadLevel::ESCAPE }
};

static const std::string SYSTEMLOAD_PARAMETER = "resourceschedule.systemload.level";

NotifierMgr& NotifierMgr::GetInstance()
{
    static NotifierMgr notifierMgr;
    return notifierMgr;
}

NotifierMgr::~NotifierMgr()
{
    std::lock_guard<std::mutex> autoLock(notifierMutex_);
    notifierMap_.clear();
}

void NotifierMgr::Init()
{
    if (initialized_) {
        RESSCHED_LOGE("NotifierMgr has initialized");
        return;
    }
    notifierDeathRecipient_ = sptr<IRemoteObject::DeathRecipient>(
        new (std::nothrow) ResSchedNotifierDeathRecipient(
        std::bind(&NotifierMgr::OnRemoteNotifierDied, this, std::placeholders::_1)));
    notifierHandler_ = std::make_shared<ffrt::queue>("DeviceNotifyQueue");
    std::string systemloadParamDef;
    std::string systemloadParam = OHOS::system::GetParameter(SYSTEMLOAD_PARAMETER, systemloadParamDef);
    if (!systemloadParam.empty()) {
        for (auto& vec : g_systemloadPair) {
            if (vec.first == systemloadParam) {
                systemloadLevel_ = vec.second;
            }
        }
    }
    initialized_ = true;
}

void NotifierMgr::RegisterNotifier(int32_t pid, const sptr<IRemoteObject>& notifier)
{
    RESSCHED_LOGD("RegisterNotifier called, pid = %{public}d.", pid);
    if (notifier == nullptr) {
        RESSCHED_LOGE("RegisterNotifier notifier is null");
        return;
    }
    if (notifierDeathRecipient_ == nullptr) {
        RESSCHED_LOGE("RegisterNotifier error due to notifierDeathRecipient null");
        return;
    }
    std::lock_guard<std::mutex> autoLock(notifierMutex_);
    auto iter = notifierMap_.find(pid);
    if (iter == notifierMap_.end()) {
        NotifierInfo info;
        info.notifier = notifier;
        info.hapApp = IsHapApp();
        notifierMap_[pid] = info;
        notifier->AddDeathRecipient(notifierDeathRecipient_);
    }
}

void NotifierMgr::UnRegisterNotifier(int32_t pid)
{
    RESSCHED_LOGD("UnRegisterNotifier called");
    std::lock_guard<std::mutex> autoLock(notifierMutex_);
    auto iter = notifierMap_.find(pid);
    if (iter != notifierMap_.end()) {
        iter->second.notifier->RemoveDeathRecipient(notifierDeathRecipient_);
        notifierMap_.erase(pid);
    }
}

void NotifierMgr::OnRemoteNotifierDied(const sptr<IRemoteObject>& notifier)
{
    RESSCHED_LOGD("OnRemoteNotifierDied called");
    if (notifier == nullptr) {
        RESSCHED_LOGW("remote notifier null");
        return;
    }
    RemoveNotifierLock(notifier);
}

void NotifierMgr::OnDeviceLevelChanged(int32_t type, int32_t level)
{
    RESSCHED_LOGD("OnDeviceLevelChanged called");
    auto cbType = static_cast<ResType::DeviceStatus>(type);
    if (g_DeviceStatusType.count(cbType) == 0) {
        RESSCHED_LOGW("OnDeviceLevelChanged, no type matched");
        return;
    }

    systemloadLevel_ = static_cast<ResType::SystemloadLevel>(level);
    for (auto& vec : g_systemloadPair) {
        if (systemloadLevel_ == vec.second) {
            OHOS::system::SetParameter(SYSTEMLOAD_PARAMETER, vec.first);
        }
    }

    if (notifierHandler_ == nullptr) {
        RESSCHED_LOGE("OnDeviceLevelChanged error due to notifierHandler null.");
        return;
    }

    OnDeviceLevelChangedLock(level);
}

void NotifierMgr::OnApplicationStateChange(int32_t state, int32_t pid)
{
    RESSCHED_LOGD("OnApplicationStateChange called, state: %{public}d, pid : %{public}d .", state, pid);
    std::lock_guard<std::mutex> autoLock(notifierMutex_);
    auto iter = notifierMap_.find(pid);
    if (iter == notifierMap_.end()) {
        return;
    }
    auto& info = iter->second;
    if (!info.hapApp) {
        RESSCHED_LOGW("OnApplicationStateChange called, not app.");
        return;
    }
    if (state == static_cast<int32_t>(ApplicationState::APP_STATE_FOREGROUND)) {
        info.foreground = true;
        if (systemloadLevel_ != info.level) {
            info.level = systemloadLevel_;
            std::vector<sptr<IRemoteObject>> vec{ info.notifier };
            HandleDeviceLevelChange(vec, systemloadLevel_);
        }
    }
    if (state == static_cast<int32_t>(ApplicationState::APP_STATE_BACKGROUND)
        || state == static_cast<int32_t>(ApplicationState::APP_STATE_TERMINATED)
        || state == static_cast<int32_t>(ApplicationState::APP_STATE_END)) {
        info.foreground = false;
    }
}

int32_t NotifierMgr::GetSystemloadLevel()
{
    RESSCHED_LOGD("GetSystemloadLevel called");
    return static_cast<int32_t>(systemloadLevel_);
}

void NotifierMgr::RemoveNotifierLock(const sptr<IRemoteObject>& notifier)
{
    RESSCHED_LOGD("RemoveNotifierLock called");
    std::lock_guard<std::mutex> autoLock(notifierMutex_);
    for (auto& notifiers : notifierMap_) {
        if (notifiers.second.notifier != notifier) {
            continue;
        }
        notifiers.second.notifier->RemoveDeathRecipient(notifierDeathRecipient_);
        notifierMap_.erase(notifiers.first);
        return;
    }
}

void NotifierMgr::OnDeviceLevelChangedLock(int32_t level)
{
    std::vector<sptr<IRemoteObject>> notifierArray;
    {
        std::lock_guard<std::mutex> autoLock(notifierMutex_);
        for (auto& notifiers : notifierMap_) {
            auto pid = notifiers.first;
            auto& info = notifiers.second;
            if (info.hapApp && !info.foreground) {
                RESSCHED_LOGD("app on background, pid = %{public}d .", pid);
                continue;
            }
            info.level = level;
            notifierArray.push_back(info.notifier);
        }
    }
    HandleDeviceLevelChange(notifierArray, level);
}

bool NotifierMgr::IsHapApp()
{
    Security::AccessToken::AccessTokenID tokenId = OHOS::IPCSkeleton::GetCallingTokenID();
    auto tokenType = Security::AccessToken::AccessTokenKit::GetTokenTypeFlag(tokenId);
    if (tokenType != Security::AccessToken::ATokenTypeEnum::TOKEN_HAP) {
        RESSCHED_LOGE("not hap app");
        return false;
    }
    return true;
}

std::vector<std::pair<int32_t, bool>> NotifierMgr::DumpRegisterInfo()
{
    std::vector<std::pair<int32_t, bool>> vec;
    {
        std::lock_guard<std::mutex> autoLock(notifierMutex_);
        for (auto& notifiers : notifierMap_) {
            vec.push_back({ notifiers.first, notifiers.second.hapApp });
        }
    }
    return vec;
}

void NotifierMgr::HandleDeviceLevelChange(std::vector<sptr<IRemoteObject>>& notifierVec, int32_t level)
{
    auto func = [notifierVec, level] () {
        for (auto& notifier : notifierVec) {
            if (notifier == nullptr) {
                continue;
            }
            auto proxy = std::make_unique<ResSchedSystemloadNotifierProxy>(notifier);
            proxy->OnSystemloadLevel(level);
        }
    };
    notifierHandler_->submit(func);
}
} // ResourceSchedule
} // OHOS