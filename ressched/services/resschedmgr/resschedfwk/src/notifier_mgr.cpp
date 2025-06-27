/*
 * Copyright (c) 2024-2025 Huawei Device Co., Ltd.
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
#include "file_ex.h"
#include "res_sched_log.h"
#include "res_sched_common_death_recipient.h"
#include "res_sched_systemload_notifier_proxy.h"
#include "res_sched_event_reporter.h"
#include "hisysevent.h"

namespace OHOS {
namespace ResourceSchedule {
using OHOS::AppExecFwk::ApplicationState;

static constexpr size_t MAX_REPORT_SIZE = 100;
static constexpr int64_t REPORT_INTERVAL_MS = static_cast<int64_t>(2) * 60 * 60 * 1000 * 1000;
static constexpr int64_t FIRST_REPORT_DELAY_MS = 10 * 1000 * 1000;
static const char* SYSTEMLOAD_CHANGE = "systemLoadChange";

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

static const char* SYSTEMLOAD_FILE = "/data/service/el1/public/ressched/systemload";

NotifierMgr& NotifierMgr::GetInstance()
{
    static NotifierMgr notifierMgr;
    return notifierMgr;
}

NotifierMgr::~NotifierMgr()
{
    // 先锁notifierMutex_后锁hisyseventMutex_，防止与RegisterNotifier死锁
    std::lock_guard<std::mutex> autoLock(notifierMutex_);
    notifierMap_.clear();
    std::lock_guard<ffrt::mutex> hisyseventLock(hisyseventMutex_);
    hisyseventBundleNames_.clear();
}

void NotifierMgr::Init()
{
    if (initialized_) {
        RESSCHED_LOGE("NotifierMgr has initialized");
        return;
    }
    notifierDeathRecipient_ = sptr<IRemoteObject::DeathRecipient>(
        new (std::nothrow) ResSchedCommonDeathRecipient([](const sptr<IRemoteObject>& notifier) {
            NotifierMgr::GetInstance().OnRemoteNotifierDied(notifier);
        }));
    notifierHandler_ = std::make_shared<ffrt::queue>("DeviceNotifyQueue");
    std::string systemload;
    if (LoadStringFromFile(SYSTEMLOAD_FILE, systemload)) {
        for (auto& vec : g_systemloadPair) {
            if (vec.first == systemload) {
                systemloadLevel_ = vec.second;
                RESSCHED_LOGI("load systemload from file, value:%{public}d", (int)systemloadLevel_);
            }
        }
    }
    initialized_ = true;
    ResschedEventReporter::GetInstance().ReportFileSizeEvent(SYSTEMLOAD_FILE);
}

void NotifierMgr::Deinit()
{
    notifierHandler_.reset();
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
    // 加{}控制notifierMutex_锁作用域
    {
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
    NotifierMgr::GetInstance().ReportPidToHisysevent(pid);
}

void NotifierMgr::ReportPidToHisysevent(const int32_t pid)
{
    std::string bundleName = NotifierMgr::GetInstance().GetBundleNameByPid(pid);
    if (bundleName.empty()) {
        RESSCHED_LOGW("pid %{public}d get bundleName error, not report", pid);
        return;
    }
    std::lock_guard<ffrt::mutex> autoLock(hisyseventMutex_);
    hisyseventBundleNames_.insert(bundleName);
    if (isTaskSubmit_) {
        NotifierMgr::GetInstance().NotifierEventReportDelay(FIRST_REPORT_DELAY_MS);
        isTaskSubmit_ = true;
    }
    size_t size = hisyseventBundleNames_.size();
    if (size > MAX_REPORT_SIZE) {
        NotifierMgr::GetInstance().NotifierEventReport();
    }
}

void NotifierMgr::NotifierEventReportDelay(int64_t delay)
{
    ffrt::submit([](){
         NotifierMgr::GetInstance().NotifierEventReportPeriod();
    }, ffrt::task_attr().delay(delay));    
}

void NotifierMgr::NotifierEventReportPeriod()
{
    std::lock_guard<ffrt::mutex> autoLock(hisyseventMutex_);
    NotifierMgr::GetInstance().NotifierEventReport();
    NotifierEventReportDelay(REPORT_INTERVAL_MS);
}

void NotifierMgr::NotifierEventReport()
{
    if (!hisyseventBundleNames_.empty()) {
        std::vector<std::string> bundleNames;
        for (const auto& item : hisyseventBundleNames_) {
            bundleNames.emplace_back(item);
        }
        std::int32_t size = hisyseventBundleNames_.size();
        HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::RSS, "NOTIFIER",
            HiviewDFX::HiSysEvent::EventType::STATISTIC, "BUNDLE_NAMES", bundleNames, "BUNDLE_SIZE", size);
    }
    hisyseventBundleNames_.clear();
}

std::string NotifierMgr::GetBundleNameByPid(int32_t pid)
{
    std::string bundleName= "";
    int32_t uid = 0;
    if (appMgrClient_ == nullptr) {
        appMgrClient_ = std::make_shared<AppExecFwk::AppMgrClient>();

    }
    std::string identity = IPCSkeleton::ResetCallingIdentity();
    int32_t ret = static_cast<int32_t>(appMgrClient_->GetBundleNameByPid(pid, bundleName, uid));
    IPCSkeleton::SetCallingIdentity(identity);
    return bundleName;
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
            if (SaveStringToFile(SYSTEMLOAD_FILE, vec.first)) {
                RESSCHED_LOGI("save systemload succeed,systemload is %{public}d", (int)vec.second);
            } else {
                RESSCHED_LOGW("save systemload failed,systemload is %{public}d", (int)vec.second);
            }
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