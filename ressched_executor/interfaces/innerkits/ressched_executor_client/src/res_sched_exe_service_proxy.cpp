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
#include "res_sched_exe_service_proxy.h"

#include "ipc_types.h"
#include "iremote_object.h"

#include "ipc_util.h"
#include "res_exe_type.h"
#include "res_sched_exe_common_utils.h"
#include "res_sched_exe_constants.h"
#include "res_sched_exe_log.h"

namespace OHOS {
namespace ResourceSchedule {
int32_t ResSchedExeServiceProxy::SendRequestSync(uint32_t resType, int64_t value,
    const nlohmann::json& context, nlohmann::json& reply)
{
    RSSEXE_LOGD("SendRequestSync start.");
    MessageOption option = { MessageOption::TF_SYNC };
    if (resType == ResExeType::RES_TYPE_DEBUG) {
        SendDebugCommand(option);
        return ResErrCode::RSSEXE_NO_ERR;
    }
    MessageParcel data;
    MakeUpParcel(data, resType, value, context);
    MessageParcel response;
    int32_t error = Remote()->SendRequest(ResIpcType::REQUEST_SEND_SYNC, data, response, option);
    if (error != NO_ERROR) {
        RSSEXE_LOGE("Send request error: %{public}d.", error);
        return ResIpcErrCode::RSSEXE_SEND_REQUEST_FAIL;
    }
    RSSEXE_LOGD("SendRequestSync success.");
    int32_t ret = response.ReadInt32();
    if (ret == ResErrCode::RSSEXE_NO_ERR) {
        ResSchedExeCommonUtils::StringToJson(response.ReadString(), reply);
    }
    return ret;
}

void ResSchedExeServiceProxy::SendRequestAsync(uint32_t resType, int64_t value, const nlohmann::json& context)
{
    RSSEXE_LOGD("SendRequestAsync start.");
    MessageOption option = { MessageOption::TF_ASYNC };
    if (resType == ResExeType::RES_TYPE_DEBUG) {
        SendDebugCommand(option);
        return;
    }
    MessageParcel data;
    MakeUpParcel(data, resType, value, context);
    MessageParcel response;
    int32_t error = Remote()->SendRequest(ResIpcType::REQUEST_SEND_ASYNC, data, response, option);
    if (error != NO_ERROR) {
        RSSEXE_LOGE("Send request error: %{public}d.", error);
        return;
    }
    RSSEXE_LOGD("SendRequestAsync success.");
}

int32_t ResSchedExeServiceProxy::KillProcess(pid_t pid)
{
    RSSEXE_LOGD("KillProcess start.");
    MessageOption option = { MessageOption::TF_SYNC };
    MessageParcel data;
    WRITE_PARCEL(data, InterfaceToken, ResSchedExeServiceProxy::GetDescriptor(),
        ResIpcErrCode::RSSEXE_DATA_ERROR, ResSchedExeServiceProxy);
    WRITE_PARCEL(data, Int32, pid, ResIpcErrCode::RSSEXE_DATA_ERROR, ResSchedExeServiceProxy);
    MessageParcel response;
    int32_t error = Remote()->SendRequest(ResIpcType::REQUEST_KILL_PROCESS, data, response, option);
    if (error != NO_ERROR) {
        RSSEXE_LOGE("Send request error: %{public}d.", error);
        return ResIpcErrCode::RSSEXE_SEND_REQUEST_FAIL;
    }
    RSSEXE_LOGD("KillProcess success.");
    return response.ReadInt32();
}

int32_t ResSchedExeServiceProxy::MakeUpParcel(MessageParcel& data,
    uint32_t resType, int64_t value, const nlohmann::json& context)
{
    WRITE_PARCEL(data, InterfaceToken, ResSchedExeServiceProxy::GetDescriptor(),
        ResIpcErrCode::RSSEXE_DATA_ERROR, ResSchedExeServiceProxy);
    WRITE_PARCEL(data, Uint32, resType, ResIpcErrCode::RSSEXE_DATA_ERROR, ResSchedExeServiceProxy);
    WRITE_PARCEL(data, Int64, value, ResIpcErrCode::RSSEXE_DATA_ERROR, ResSchedExeServiceProxy);
    WRITE_PARCEL(data, String, context.dump(-1, ' ', false, nlohmann::detail::error_handler_t::replace),
        ResIpcErrCode::RSSEXE_DATA_ERROR, ResSchedExeServiceProxy);
    return ResErrCode::RSSEXE_NO_ERR;
}

int32_t ResSchedExeServiceProxy::SendDebugCommand(MessageOption& option)
{
    RSSEXE_LOGD("SendDebugCommand start.");
    MessageParcel data;
    WRITE_PARCEL(data, InterfaceToken, ResSchedExeServiceProxy::GetDescriptor(),
        ResIpcErrCode::RSSEXE_DATA_ERROR, ResSchedExeServiceProxy);
    WRITE_PARCEL(data, Uint32, ResExeType::RES_TYPE_DEBUG, ResIpcErrCode::RSSEXE_DATA_ERROR, ResSchedExeServiceProxy);
    uint64_t curr = ResSchedExeCommonUtils::GetCurrentTimestampUs();
    WRITE_PARCEL(data, Uint64, curr, ResIpcErrCode::RSSEXE_DATA_ERROR, ResSchedExeServiceProxy);
    RSSEXE_LOGD("IPC debug: client send request, current timestamp is %{public}lld.", (long long)curr);

    MessageParcel response;
    int32_t error = Remote()->SendRequest(ResIpcType::REQUEST_SEND_DEBUG, data, response, option);
    if (error != NO_ERROR) {
        RSSEXE_LOGE("Send request error: %{public}d.", error);
        return ResIpcErrCode::RSSEXE_SEND_REQUEST_FAIL;
    }
    RSSEXE_LOGD("SendDebugCommand success.");
    return ResErrCode::RSSEXE_NO_ERR;
}

} // namespace ResourceSchedule
} // namespace OHOS
