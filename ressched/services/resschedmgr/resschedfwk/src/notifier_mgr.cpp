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

#include "app_mgr_constants.h"
#include "parameters.h"
#include "res_sched_log.h"
#include "res_sched_notifier_death_recipient.h"
#include "res_sched_systemload_notifier_proxy.h"

namespace OHOS {
namespace ResourceSchedule {
using OHOS::AppExecFwk::ApplicationState;

static std::map<ResType::DeviceStatus, std::string> g_DeviceStatusType = {
    { ResType::DeviceStatus::SYSTEMLOAD_LEVEL, "systemLoadChange" }
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

static const std::string SYSTEMLOAD_PARAMETER = "resourceschedule.systemload";

NotifierMgr& NotifierMgr::GetInstance()
{
    static NotifierMgr notifierMgr;
    return notifierMgr;
}

void NotifierMgr::Init()
{
    if (initialized) {
        RESSCHED_LOGE("NotifierMgr has initialized");
        return;
    }
    initialized = true;
    notifierDeathRecipient_ = sptr<IRemoteObject::DeathRecipient>(
        new (std::nothrow) ResSchedNotifierDeathRecipient(
        std::bind(&NotifierMgr::RemoteNotifierDied, this, std::placeholders::_1)));
    notifierHandler_ = std::make_shared<ffrt::queue>("OnDeviceLevelChanged");
    std::string systemloadParamDef;
    std::string systemloadParam = OHOS::system::GetParameter(SYSTEMLOAD_PARAMETER, systemloadParamDef);
    if (!systemloadParam.empty()) {
        for (auto& vec : g_systemloadPair) {
            if (vec.first == systemloadParam) {
                systemloadLevel_ = vec.second;
            }
        }
    }
}

void NotifierMgr::RegisterNotifier(int32_t pid, const std::string& cbType, const sptr<IRemoteObject>& notifier)
{
    RESSCHED_LOGD("RegisterNotifier called, pid = %{public}d, callback type = %{public}s.",
        pid, cbType.c_str());
    if (notifierDeathRecipient_ == nullptr) {
        RESSCHED_LOGE("RegisterNotifier error due to notifierDeathRecipient null");
        return;
    }
    std::lock_guard<std::mutex> autoLock(notifierMutex_);
    if (notifierMap_.find(pid) == notifierMap_.end()) {
        notifierMap_[pid] = std::map<std::string, sptr<IRemoteObject>>();
        notifierMap_[pid][cbType] = notifier;
        notifier->AddDeathRecipient(notifierDeathRecipient_);
    } else {
        auto& notifiers = notifierMap_[pid];
        if (notifiers.find(cbType) == notifiers.end()) {
            notifiers[cbType] = notifier;
            notifier->AddDeathRecipient(notifierDeathRecipient_);
        }
    }
}

void NotifierMgr::UnRegisterNotifier(int32_t pid, const std::string& cbType)
{
    RESSCHED_LOGD("UnRegisterNotifier called");
    std::lock_guard<std::mutex> autoLock(notifierMutex_);
    if (notifierMap_.find(pid) != notifierMap_.end()) {
        auto& notifiers = notifierMap_[pid];
        if (notifiers.find(cbType) != notifiers.end()) {
            notifiers[cbType]->RemoveDeathRecipient(notifierDeathRecipient_);
            notifiers.erase(cbType);
        }
        if (notifiers.empty()) {
            notifierMap_.erase(pid);
        }
    }
}

void NotifierMgr::RemoteNotifierDied(const sptr<IRemoteObject>& notifier)
{
    RESSCHED_LOGD("RemoteNotifierDied called");
    if (notifier == nullptr) {
        RESSCHED_LOGW("remote notifier null");
        return;
    }
    RemoveNotifierLock(notifier);
}

void NotifierMgr::OnDeviceLevelChanged(int32_t type, int32_t level, std::string& action)
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

    OnDeviceLevelChangedLock(g_DeviceStatusType[cbType], level, action);
}

void NotifierMgr::OnApplicationStateChange(int32_t state, int32_t pid)
{
    RESSCHED_LOGD("OnApplicationStateChange called, state: %{public}d, pid : %{public}d .", state, pid);
    {
        std::lock_guard<std::mutex> autoLock(notifierMutex_);
        if (notifierMap_.count(pid) == 0) {
            return;
        }
    }

    std::lock_guard<std::mutex> autoLock(pidSetMutex);
    if (state == static_cast<int32_t>(ApplicationState::APP_STATE_FOREGROUND)) {
        fgPidSet_.insert(pid);
    }
    if (state == static_cast<int32_t>(ApplicationState::APP_STATE_BACKGROUND)
        || state == static_cast<int32_t>(ApplicationState::APP_STATE_TERMINATED)
        || state == static_cast<int32_t>(ApplicationState::APP_STATE_END)) {
        fgPidSet_.erase(pid);
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
        for (auto& iter : notifiers.second) {
            if (iter.second != notifier) {
                continue;
            }
            iter.second->RemoveDeathRecipient(notifierDeathRecipient_);
            notifiers.second.erase(iter.first);
            if (notifiers.second.empty()) {
                notifierMap_.erase(notifiers.first);
            }
            return;
        }
    }
}

void NotifierMgr::OnDeviceLevelChangedLock(const std::string& cbType, int32_t level, std::string& action)
{
    std::vector<sptr<IRemoteObject>> notifierArray;
    {
        std::lock_guard<std::mutex> autoLock(notifierMutex_);
        for (auto& notifiers : notifierMap_) {
            if (fgPidSet_.count(notifiers.first) == 0) {
                RESSCHED_LOGD("app on background, pid = %{public}d .", notifiers.first);
                continue;
            }
            for (auto& iter : notifiers.second) {
                if (iter.first == cbType) {
                    notifierArray.push_back(iter.second);
                }
            }
        }
    }
    auto func = [notifierArray, level] () {
        for (auto& notifier : notifierArray) {
            auto proxy = std::make_unique<ResSchedSystemloadNotifierProxy>(notifier);
            proxy->OnSystemloadLevel(level);
        }
    };
    notifierHandler_->submit(func);
}

} // OHOS
} // ResourceSchedule