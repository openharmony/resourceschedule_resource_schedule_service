/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#include <string>
#include <unistd.h>
#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "res_sched_log.h"
#include "res_sched_errors.h"
#include "system_ability_definition.h"
#include "res_type.h"
#include "string_ex.h"
#include "res_sched_client.h"

namespace OHOS {
namespace ResourceSchedule {
ResSchedClient& ResSchedClient::GetInstance()
{
    static ResSchedClient instance;
    return instance;
}

void ResSchedClient::ReportData(uint32_t resType, int64_t value,
                                const std::unordered_map<std::string, std::string>& mapPayload)
{
    RESSCHED_LOGD("ResSchedClient::ReportData receive resType = %{public}u, value = %{public}lld.",
                  resType, (long long)value);
    if (TryConnect() != ERR_OK) {
        return;
    }
    Json::Value payload;
    if (resType == ResType::RES_TYPE_THREAD_CHANGE) {
        for (auto it = mapPayload.begin(); it != mapPayload.end(); ++it) {
            Json::Value temp;
            temp["tid"] = it->first;
            temp["qos"] = it->second;
            payload["amendThreadList"].append(temp);
        }
        payload["clientPid"] = std::to_string(getpid());
        if (!payload.isMember("pid")) {
            payload["pid"] = std::to_string(getpid());
        }
    } else {
        for (auto it = mapPayload.begin(); it != mapPayload.end(); ++it) {
            payload[it->first] = it->second;
        }
    }
    rss_->ReportData(resType, value, payload);
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

    remoteObject_ = systemManager->GetSystemAbility(RES_SCHED_SYS_ABILITY_ID);
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
} // namespace ResourceSchedule
} // namespace OHOS
