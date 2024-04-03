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
#include "res_sched_exe_client.h"

#include <new>
#include <string>
#include <unistd.h>
#include <unordered_map>
#include <utility>

#include "if_system_ability_manager.h"
#include "iremote_broker.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"

#include "res_exe_type.h"
#include "res_sched_exe_constants.h"
#include "res_sched_exe_log.h"

namespace OHOS {
namespace ResourceSchedule {
ResSchedExeClient& ResSchedExeClient::GetInstance()
{
    static ResSchedExeClient instance;
    return instance;
}

ResSchedExeClient::~ResSchedExeClient()
{
    StopRemoteObject();
}

int32_t ResSchedExeClient::SendResRequest(uint32_t resType, int64_t value,
    const std::unordered_map<std::string, std::string>& context, std::string reply)
{
    return SendRequestInner(true, resType, value, context, reply);
}

void ResSchedExeClient::ReportData(uint32_t resType, int64_t value,
    const std::unordered_map<std::string, std::string>& context)
{
    std::string reply;
    SendRequestInner(false, resType, value, context, reply);
}

int32_t ResSchedExeClient::SendRequestInner(bool isSync, uint32_t resType, int64_t value,
    const std::unordered_map<std::string, std::string>& context, std::string reply)
{
    if (TryConnect() != ResErrCode::RSSEXE_NO_ERR) {
        return ResIpcErrCode::RSSEXE_CONNECT_FAIL;
    }
    RSSEXE_LOGD("SendRequestInner receive resType = %{public}u, value = %{public}ld.", resType, value);

    std::lock_guard<std::mutex> lock(mutex_);
    if (resSchedExe_ == nullptr) {
        RSSEXE_LOGD("fail to get resource schedule executor.");
        return ResIpcErrCode::RSSEXE_REQUEST_FAIL;
    }

    RSSEXE_LOGD("send request.");
    nlohmann::json payload = ConvertMapTojson(context);
    nlohmann::json result;
    if (isSync) {
        int32_t ret = resSchedExe_->SendResRequest(resType, value, payload, result);
        reply.append(result.dump(-1, ' ', false, nlohmann::detail::error_handler_t::replace));
        return ret;
    } else {
        resSchedExe_->ReportData(resType, value, payload);
        return ResErrCode::RSSEXE_NO_ERR;
    }
}

nlohmann::json ResSchedExeClient::ConvertMapTojson(const std::unordered_map<std::string, std::string>& context)
{
    nlohmann::json payload;
    for (auto it = context.begin(); it != context.end(); ++it) {
        payload[it->first] = it->second;
    }
    return payload;
}

void ResSchedExeClient::SendDebugCommand(bool isSync)
{
    std::unordered_map<std::string, std::string> context;
    std::string reply;
    SendRequestInner(isSync, ResExeType::RES_TYPE_DEBUG, 0, context, reply);
}

ErrCode ResSchedExeClient::TryConnect()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (resSchedExe_) {
        return ResErrCode::RSSEXE_NO_ERR;
    }

    sptr<ISystemAbilityManager> systemManager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (!systemManager) {
        RSSEXE_LOGE("Fail to get registry.");
        return ResIpcErrCode::RSSEXE_GET_SERVICE_FAIL;
    }

    remoteObject_ = systemManager->CheckSystemAbility(RES_SCHED_SYS_ABILITY_ID);
    if (!remoteObject_) {
        RSSEXE_LOGE("Fail to connect resource schedule executor.");
        return ResIpcErrCode::RSSEXE_GET_SERVICE_FAIL;
    }

    resSchedExe_ = iface_cast<IResSchedExeService>(remoteObject_);
    if (!resSchedExe_) {
        return ResIpcErrCode::RSSEXE_GET_SERVICE_FAIL;
    }
    recipient_ = new (std::nothrow) ResSchedExeDeathRecipient(*this);
    if (!recipient_) {
        RSSEXE_LOGE("New ResSchedDeathRecipient failed.");
        return ResIpcErrCode::RSSEXE_GET_SERVICE_FAIL;
    }
    resSchedExe_->AsObject()->AddDeathRecipient(recipient_);
    RSSEXE_LOGD("Connect resource schedule executor success.");
    return ResErrCode::RSSEXE_NO_ERR;
}

void ResSchedExeClient::StopRemoteObject()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (resSchedExe_ && resSchedExe_->AsObject()) {
        resSchedExe_->AsObject()->RemoveDeathRecipient(recipient_);
    }
    resSchedExe_ = nullptr;
}

ResSchedExeClient::ResSchedExeDeathRecipient::ResSchedExeDeathRecipient(ResSchedExeClient &resSchedExeClient)
    : resSchedExeClient_(resSchedExeClient) {}

ResSchedExeClient::ResSchedExeDeathRecipient::~ResSchedExeDeathRecipient() {}

void ResSchedExeClient::ResSchedExeDeathRecipient::OnRemoteDied(const wptr<IRemoteObject> &remote)
{
    resSchedExeClient_.StopRemoteObject();
}
} // namespace ResourceSchedule
} // namespace OHOS
