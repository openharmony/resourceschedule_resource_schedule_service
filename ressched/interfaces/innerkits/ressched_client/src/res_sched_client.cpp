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

void ResSchedClient::RegisterSystemloadNotifier(const sptr<IRemoteObject>& notifier)
{
    if (TryConnect() != ERR_OK) {
        return;
    }
    RESSCHED_LOGD("ResSchedClient::RegisterSystemloadNotifier receive mission.");

    std::lock_guard<std::mutex> lock(mutex_);
    if (rss_ == nullptr) {
        RESSCHED_LOGE("ResSchedClient::RegisterSystemloadNotifier fail to get resource schedule service.");
        return;
    }
    rss_->RegisterSystemloadNotifier(notifier);
}

void ResSchedClient::UnRegisterSystemloadNotifier()
{
    if (TryConnect() != ERR_OK) {
        return;
    }
    RESSCHED_LOGD("ResSchedClient::UnRegisterSystemloadNotifier receive mission.");

    std::lock_guard<std::mutex> lock(mutex_);
    if (rss_ == nullptr) {
        RESSCHED_LOGE("ResSchedClient::UnRegisterSystemloadNotifier fail to get resource schedule service.");
        return;
    }
    rss_->UnRegisterSystemloadNotifier();
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
}

ResSchedClient::ResSchedDeathRecipient::ResSchedDeathRecipient(ResSchedClient &resSchedClient)
    : resSchedClient_(resSchedClient) {}

ResSchedClient::ResSchedDeathRecipient::~ResSchedDeathRecipient() {}

void ResSchedClient::ResSchedDeathRecipient::OnRemoteDied(const wptr<IRemoteObject> &remote)
{
    resSchedClient_.StopRemoteObject();
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
