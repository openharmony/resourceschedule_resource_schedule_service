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

#include "res_common_util.h"
#include "res_exe_type.h"
#include "res_json_type.h"
#include "res_sched_exe_constants.h"
#include "res_sched_exe_log.h"

namespace OHOS {
namespace ResourceSchedule {
const int32_t MAX_IPC_MESSAGE_SIZE = 200000;
const int32_t IPC_BATCHES_SIZE = 180000;

ResSchedExeClient& ResSchedExeClient::GetInstance()
{
    static ResSchedExeClient instance;
    return instance;
}

ResSchedExeClient::~ResSchedExeClient()
{
    StopRemoteObject();
}

int32_t ResSchedExeClient::SendRequestSync(uint32_t resType, int64_t value,
    const nlohmann::json& context, nlohmann::json& reply)
{
    if (resType == ResExeType::RES_TYPE_EXECUTOR_PLUGIN_INIT) {
        std::vector<nlohmann::json> splitContext;
        if (ProcessJson(context, splitContext, resType)) {
            return SendRequestBatches(true, resType, splitContext, value, reply);
        }
    }
    return SendRequestInner(true, resType, value, context, reply);
}

void ResSchedExeClient::SendRequestAsync(uint32_t resType, int64_t value,
    const nlohmann::json& context)
{
    nlohmann::json reply;
    if (resType == ResExeType::RES_TYPE_EXECUTOR_PLUGIN_INIT) {
        std::vector<nlohmann::json> splitContext;
        if (ProcessJson(context, splitContext, resType)) {
            SendRequestBatches(false, resType, splitContext, value, reply);
            return;
        }
    }
    SendRequestInner(false, resType, value, context, reply);
}

int32_t ResSchedExeClient::SendRequestBatches(bool isSync, uint32_t resType,
    const std::vector<nlohmann::json>& splitContext, int64_t value, nlohmann::json& reply)
{
    int32_t ret = ResErrCode::RSSEXE_NO_ERR;
    RSSEXE_LOGI("ipc messages exceed the limit and are transmitted in batches.");
    for (const nlohmann::json& jsonMsg : splitContext) {
        RSSEXE_LOGD("ipc messages index: %{public}d", jsonMsg["MESSAGE_INDEX"].get<int>());
        ret = SendRequestInner(isSync, resType, value, jsonMsg, reply);
        if (ret != ResErrCode::RSSEXE_NO_ERR) {
            RSSEXE_LOGE("SendRequestInner failed.");
            break;
        }
    }
    return ret;
}

int32_t ResSchedExeClient::KillProcess(pid_t pid)
{
    RSSEXE_LOGD("kill process receive pid = %{public}d", pid);
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        RSSEXE_LOGE("fail to get resource schedule executor");
        return ResIpcErrCode::RSSEXE_CONNECT_FAIL;
    }
    int32_t funcResult = ResIpcErrCode::RSSEXE_GET_SERVICE_FAIL;
    proxy->KillProcess(static_cast<uint32_t>(pid), funcResult);
    return funcResult;
}

int32_t ResSchedExeClient::SendRequestInner(bool isSync, uint32_t resType, int64_t value,
    const nlohmann::json& context, nlohmann::json& reply)
{
    RSSEXE_LOGD("receive resType = %{public}u, value = %{public}lld.", resType, (long long)value);
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        RSSEXE_LOGE("fail to get resource schedule executor.");
        return ResIpcErrCode::RSSEXE_CONNECT_FAIL;
    }

    ResJsonType jsonTypeContext;
    jsonTypeContext.jsonContext = context;
    if (isSync) {
        RSSEXE_LOGD("ResSchedExeClient send sync request, resType = %{public}u", resType);
        ResJsonType jsonTypeReply;
        int32_t funcResult;
        proxy->SendRequestSync(resType, value, jsonTypeContext, jsonTypeReply, funcResult);
        reply = jsonTypeReply.jsonContext;
        return funcResult;
    } else {
        RSSEXE_LOGD("ResSchedExeClient send async request, resType = %{public}u", resType);
        proxy->SendRequestAsync(resType, value, jsonTypeContext);
        return ResErrCode::RSSEXE_NO_ERR;
    }
}

bool ResSchedExeClient::ProcessJson(const nlohmann::json& context, std::vector<nlohmann::json>& splitContext,
    int32_t resType)
{
    std::string dumpJsonStr = context.dump(-1, ' ', false, nlohmann::detail::error_handler_t::replace);
    size_t dumpJsonStrSize = dumpJsonStr.size();
    if (dumpJsonStrSize > MAX_IPC_MESSAGE_SIZE) {
        RSSEXE_LOGI("ResSchedExeClient need to split the json in restype %{public}d.", resType);
        std::vector<std::string> messageValue;
        for (size_t idx = 0; idx < dumpJsonStrSize; idx += IPC_BATCHES_SIZE) {
            size_t end = std::min(idx + IPC_BATCHES_SIZE, dumpJsonStrSize);
            messageValue.push_back(dumpJsonStr.substr(idx, end - idx));
        }

        int messageIndex = 0;
        for (std::string msg : messageValue) {
            nlohmann::json jsonIpc = {
                {"IPC_MESSAGE", msg},
                {"MESSAGE_INDEX", messageIndex},
                {"MESSAGE_NUMBER", static_cast<int>(messageValue.size())}
            };
            splitContext.push_back(jsonIpc);
            messageIndex++;
        }
        return true;
    }
    return false;
}

void ResSchedExeClient::SendDebugCommand(bool isSync)
{
    nlohmann::json tempJson;
    SendRequestInner(isSync, ResExeType::RES_TYPE_DEBUG, 0, tempJson, tempJson);
}

sptr<IResSchedExeService> ResSchedExeClient::GetProxy()
{
    if (TryConnect() == ResErrCode::RSSEXE_NO_ERR) {
        std::lock_guard<std::mutex> lock(mutex_);
        return resSchedExe_;
    }
    return nullptr;
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

    remoteObject_ = systemManager->CheckSystemAbility(RES_SCHED_EXE_ABILITY_ID);
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

ResSchedExeClient::ResSchedExeDeathRecipient::ResSchedExeDeathRecipient(ResSchedExeClient& resSchedExeClient)
    : resSchedExeClient_(resSchedExeClient) {}

ResSchedExeClient::ResSchedExeDeathRecipient::~ResSchedExeDeathRecipient() {}

void ResSchedExeClient::ResSchedExeDeathRecipient::OnRemoteDied(const wptr<IRemoteObject>& remote)
{
    resSchedExeClient_.StopRemoteObject();
}
} // namespace ResourceSchedule
} // namespace OHOS
