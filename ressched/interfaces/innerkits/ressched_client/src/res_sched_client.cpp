/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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
#include "res_sched_client.h"
#include <string>                       // for to_string
#include <unistd.h>                     // for getpid
#include <new>                          // for nothrow, operator new
#include <unordered_map>                // for unordered_map, __hash_map_con...
#include <utility>                      // for pair
#include "if_system_ability_manager.h"  // for ISystemAbilityManager
#include "iremote_broker.h"             // for iface_cast
#include "iservice_registry.h"          // for SystemAbilityManagerClient
#include "res_sched_errors.h"           // for GET_RES_SCHED_SERVICE_FAILED
#include "res_sched_log.h"              // for RESSCHED_LOGE, RESSCHED_LOGD
#include "system_ability_definition.h"  // for RES_SCHED_SYS_ABILITY_ID

namespace OHOS {
namespace ResourceSchedule {
ResSchedClient& ResSchedClient::GetInstance()
{
    static ResSchedClient instance;
    return instance;
}
ResSchedClient::~ResSchedClient()
{
    StopRemoteObject();
}

void ResSchedClient::ReportData(uint32_t resType, int64_t value,
                                const std::unordered_map<std::string, std::string>& mapPayload)
{
    if (TryConnect() != ERR_OK) {
        return;
    }
    RESSCHED_LOGD("ResSchedClient::ReportData receive resType = %{public}u, value = %{public}lld.",
        resType, (long long)value);
    nlohmann::json payload;
    for (auto it = mapPayload.begin(); it != mapPayload.end(); ++it) {
        payload[it->first] = it->second;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    if (rss_ == nullptr) {
        RESSCHED_LOGD("ResSchedClient::ReportData fail to get resource schedule service.");
        return;
    }
    rss_->ReportData(resType, value, payload);
}

int32_t ResSchedClient::KillProcess(const std::unordered_map<std::string, std::string>& mapPayload)
{
    if (TryConnect() != ERR_OK) {
        return RES_SCHED_CONNECT_FAIL;
    }
    RESSCHED_LOGD("ResSchedClient::KillProcess receive mission.");
    nlohmann::json payload;
    for (auto it = mapPayload.begin(); it != mapPayload.end(); ++it) {
        payload[it->first] = it->second;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    if (rss_ == nullptr) {
        RESSCHED_LOGD("ResSchedClient::KillProcess fail to get resource schedule service.");
        return RES_SCHED_KILL_PROCESS_FAIL;
    }
    return rss_->KillProcess(payload);
}

void ResSchedClient::RegisterSystemloadNotifier(const sptr<ResSchedSystemloadNotifierClient>& callbackObj)
{
    RESSCHED_LOGD("ResSchedClient::RegisterSystemloadNotifier receive mission.");
    std::lock_guard<std::mutex> lock(mutex_);
    if (InitSystemloadListenersLocked() != ERR_OK) {
        RESSCHED_LOGE("ResSchedClient::RegisterSystemloadNotifier init listener failed.");
        return;
    }
    systemloadLevelListener_->RegisterSystemloadLevelCb(callbackObj);
    if (!systemloadCbRegistered_ && !systemloadLevelListener_->IsSystemloadCbArrayEmpty() && rss_) {
        rss_->RegisterSystemloadNotifier(systemloadLevelListener_);
        systemloadCbRegistered_ = true;
    }
    AddResSaListenerLocked();
}

void ResSchedClient::UnRegisterSystemloadNotifier(const sptr<ResSchedSystemloadNotifierClient>& callbackObj)
{
    RESSCHED_LOGD("ResSchedClient::UnRegisterSystemloadNotifier receive mission.");
    std::lock_guard<std::mutex> lock(mutex_);
    if (systemloadLevelListener_ == nullptr) {
        RESSCHED_LOGE("ResSchedClient::UnRegisterSystemloadNotifier systemloadLevelListener is null.");
        return;
    }
    systemloadLevelListener_->UnRegisterSystemloadLevelCb(callbackObj);
    UnRegisterSystemloadListenersLocked();
}

int32_t ResSchedClient::GetSystemloadLevel()
{
    if (TryConnect() != ERR_OK) {
        return RES_SCHED_CONNECT_FAIL;
    }
    RESSCHED_LOGD("ResSchedClient::GetSystemloadLevel receive mission.");

    std::lock_guard<std::mutex> lock(mutex_);
    if (rss_ == nullptr) {
        RESSCHED_LOGE("ResSchedClient::GetSystemloadLevel fail to get resource schedule service.");
        return RES_SCHED_CONNECT_FAIL;
    }
    return rss_->GetSystemloadLevel();
}

bool ResSchedClient::IsAllowedAppPreload(const std::string& bundleName, int32_t preloadMode)
{
    if (TryConnect() != ERR_OK) {
        return false;
    }

    std::lock_guard<std::mutex> lock(mutex_);
    if (rss_ == nullptr) {
        RESSCHED_LOGE("ResSchedClient::IsAllowedAppPreload fail to get resource schedule service.");
        return false;
    }

    RESSCHED_LOGD("App preload bundleName %{public}s, preloadMode %{public}d", bundleName.c_str(), preloadMode);
    return rss_->IsAllowedAppPreload(bundleName, preloadMode);
}

ErrCode ResSchedClient::TryConnect()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (rss_) {
        return ERR_OK;
    }

    sptr<ISystemAbilityManager> systemManager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (!systemManager) {
        RESSCHED_LOGE("ResSchedClient::Fail to get registry.");
        return GET_RES_SCHED_SERVICE_FAILED;
    }

    remoteObject_ = systemManager->CheckSystemAbility(RES_SCHED_SYS_ABILITY_ID);
    if (!remoteObject_) {
        RESSCHED_LOGE("ResSchedClient::Fail to connect resource schedule service.");
        return GET_RES_SCHED_SERVICE_FAILED;
    }

    rss_ = iface_cast<IResSchedService>(remoteObject_);
    if (!rss_) {
        return GET_RES_SCHED_SERVICE_FAILED;
    }
    recipient_ = new (std::nothrow) ResSchedDeathRecipient(*this);
    if (!recipient_) {
        RESSCHED_LOGE("ResSchedClient::New ResSchedDeathRecipient failed.");
        return GET_RES_SCHED_SERVICE_FAILED;
    }
    rss_->AsObject()->AddDeathRecipient(recipient_);
    AddResSaListenerLocked();
    RESSCHED_LOGD("ResSchedClient::Connect resource schedule service success.");
    return ERR_OK;
}

void ResSchedClient::StopRemoteObject()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (rss_ && rss_->AsObject()) {
        rss_->AsObject()->RemoveDeathRecipient(recipient_);
    }
    rss_ = nullptr;
    systemloadCbRegistered_ = false;
}

void ResSchedClient::AddResSaListenerLocked()
{
    if (resSchedSvcStatusListener_ != nullptr) {
        return;
    }
    sptr<ISystemAbilityManager> systemManager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (!systemManager) {
        RESSCHED_LOGE("ResSchedClient::Fail to get sa mgr client.");
        return;
    }
    resSchedSvcStatusListener_ = new (std::nothrow) ResSchedSvcStatusChange;
    if (resSchedSvcStatusListener_ == nullptr) {
        RESSCHED_LOGE("ResSchedClient::Fail to new res svc listener.");
        return;
    }
    int32_t ret = systemManager->SubscribeSystemAbility(RES_SCHED_SYS_ABILITY_ID, resSchedSvcStatusListener_);
    if (ret != ERR_OK) {
        RESSCHED_LOGE("ResSchedClient::Register sa status change failed.");
        resSchedSvcStatusListener_ = nullptr;
    }
}

void ResSchedClient::OnAddSystemAbility(int32_t systemAbilityId, const std::string& deviceId)
{
    if (systemAbilityId != RES_SCHED_SYS_ABILITY_ID) {
        RESSCHED_LOGE("ResSchedClient::OnAddSystemAbility is not res sa id.");
        return;
    }
    if (TryConnect() != ERR_OK) {
        return;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    if (InitSystemloadListenersLocked() != ERR_OK) {
        RESSCHED_LOGE("ResSchedClient::OnAddSystemAbility init listener failed.");
        return;
    }
    if (!systemloadCbRegistered_ && !systemloadLevelListener_->IsSystemloadCbArrayEmpty() && rss_) {
        rss_->RegisterSystemloadNotifier(systemloadLevelListener_);
        systemloadCbRegistered_ = true;
    }
}

int32_t ResSchedClient::InitSystemloadListenersLocked()
{
    if (systemloadLevelListener_ != nullptr) {
        return ERR_OK;
    }
    systemloadLevelListener_ = new (std::nothrow) SystemloadLevelListener;
    if (systemloadLevelListener_ == nullptr) {
        RESSCHED_LOGW("ResSchedClient::InitSystemloadListenersLocked new listener error.");
        return RES_SCHED_DATA_ERROR;
    }
    return ERR_OK;
}

void ResSchedClient::UnRegisterSystemloadListenersLocked()
{
    if (systemloadLevelListener_->IsSystemloadCbArrayEmpty() && rss_) {
        rss_->UnRegisterSystemloadNotifier();
        systemloadCbRegistered_ = false;
    }
}

ResSchedClient::ResSchedDeathRecipient::ResSchedDeathRecipient(ResSchedClient &resSchedClient)
    : resSchedClient_(resSchedClient) {}

ResSchedClient::ResSchedDeathRecipient::~ResSchedDeathRecipient() {}

void ResSchedClient::ResSchedDeathRecipient::OnRemoteDied(const wptr<IRemoteObject> &remote)
{
    resSchedClient_.StopRemoteObject();
}

ResSchedClient::SystemloadLevelListener::~SystemloadLevelListener()
{
    std::lock_guard<std::mutex> lock(listMutex_);
    systemloadLevelCbs_.clear();
}

void ResSchedClient::SystemloadLevelListener::RegisterSystemloadLevelCb(
    const sptr<ResSchedSystemloadNotifierClient>& callbackObj)
{
    std::lock_guard<std::mutex> lock(listMutex_);
    if (callbackObj != nullptr) {
        for (auto& iter : systemloadLevelCbs_) {
            if (iter == callbackObj) {
                RESSCHED_LOGE("ResSchedClient register an exist callback object.");
                return;
            }
        }
        systemloadLevelCbs_.emplace_back(callbackObj);
    }
    RESSCHED_LOGD("Client has registered %{public}d listeners.", static_cast<int32_t>(systemloadLevelCbs_.size()));
}

void ResSchedClient::SystemloadLevelListener::UnRegisterSystemloadLevelCb(
    const sptr<ResSchedSystemloadNotifierClient>& callbackObj)
{
    std::lock_guard<std::mutex> lock(listMutex_);
    systemloadLevelCbs_.remove(callbackObj);
    RESSCHED_LOGD(
        "Client left %{public}d systemload level listeners.", static_cast<int32_t>(systemloadLevelCbs_.size()));
}

bool ResSchedClient::SystemloadLevelListener::IsSystemloadCbArrayEmpty()
{
    std::lock_guard<std::mutex> lock(listMutex_);
    return systemloadLevelCbs_.empty();
}

void ResSchedClient::SystemloadLevelListener::OnSystemloadLevel(int32_t level)
{
    std::list<sptr<ResSchedSystemloadNotifierClient>> notifyList;
    {
        std::lock_guard<std::mutex> lock(listMutex_);
        for (auto& iter : systemloadLevelCbs_) {
            notifyList.push_back(iter);
        }
    }
    // copy notifiers from systemloadLevelCbs_ to revent dead lock
    for (auto& notifier : notifyList) {
        if (notifier != nullptr) {
            notifier->OnSystemloadLevel(level);
        }
    }
}

void ResSchedSvcStatusChange::OnAddSystemAbility(int32_t systemAbilityId, const std::string& deviceId)
{
    RESSCHED_LOGD("ResSchedSvcStatusChange::OnAddSystemAbility called, said : %{public}d.", systemAbilityId);
    ResSchedClient::GetInstance().OnAddSystemAbility(systemAbilityId, deviceId);
}

void ResSchedSvcStatusChange::OnRemoveSystemAbility(int32_t systemAbilityId, const std::string& deviceId)
{
    RESSCHED_LOGD("ResSchedSvcStatusChange::OnRemoveSystemAbility called.");
}

extern "C" void ReportData(uint32_t resType, int64_t value,
                           const std::unordered_map<std::string, std::string>& mapPayload)
{
    ResSchedClient::GetInstance().ReportData(resType, value, mapPayload);
}

extern "C" void KillProcess(const std::unordered_map<std::string, std::string>& mapPayload)
{
    ResSchedClient::GetInstance().KillProcess(mapPayload);
}
} // namespace ResourceSchedule
} // namespace OHOS
