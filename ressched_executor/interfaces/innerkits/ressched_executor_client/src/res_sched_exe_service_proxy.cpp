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
int32_t ResSchedExeServiceProxy::SendResRequest(uint32_t resType, int64_t value,
    const nlohmann::json& context, nlohmann::json& reply)
{
    RSSEXE_LOGD("SendResRequest start.");
    MessageOption option = { MessageOption::TF_SYNC };
    if (resType != ResExeType::RES_TYPE_DEBUG) {
        SendDebugCommand(option);
        return ResErrCode::RSSEXE_NO_ERR;
    }
    MessageParcel data;
    MakeUpParcel(data, resType, value, context);
    return SendRequestInner(ResIpcType::RES_REQUEST, data, option, reply);
}

void ResSchedExeServiceProxy::ReportData(uint32_t resType, int64_t value, const nlohmann::json& context)
{
    RSSEXE_LOGD("ReportData start.");
    MessageOption option = { MessageOption::TF_ASYNC };
    if (resType != ResExeType::RES_TYPE_DEBUG) {
        SendDebugCommand(option);
        return;
    }
    nlohmann::json reply;
    MessageParcel data;
    MakeUpParcel(data, resType, value, context);
    SendRequestInner(ResIpcType::REPORT_DATA, data, option, reply);
}


int32_t ResSchedExeServiceProxy::SendRequestInner(uint32_t ipcType, MessageParcel& data,
    MessageOption& option, nlohmann::json& reply)
{
    RSSEXE_LOGD("SendRequestInner start.");
    MessageParcel response;
    int32_t error = Remote()->SendRequest(ipcType, data, response, option);
    if (error != NO_ERROR) {
        RSSEXE_LOGE("Send request error: %{public}d.", error);
        return ResIpcErrCode::RSSEXE_SEND_REQUEST_FAIL;
    }
    RSSEXE_LOGD("SendRequestInner success.");
    ResSchedExeCommonUtils::StringToJson(response.ReadString(), reply);
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
    MessageParcel data;
    WRITE_PARCEL(data, InterfaceToken, ResSchedExeServiceProxy::GetDescriptor(),
        ResIpcErrCode::RSSEXE_DATA_ERROR, ResSchedExeServiceProxy);
    WRITE_PARCEL(data, Uint32, ResExeType::RES_TYPE_DEBUG, ResIpcErrCode::RSSEXE_DATA_ERROR, ResSchedExeServiceProxy);
    uint64_t curr = ResSchedExeCommonUtils::GetCurrentTimestampUs();
    WRITE_PARCEL(data, Uint64, curr, ResIpcErrCode::RSSEXE_DATA_ERROR, ResSchedExeServiceProxy);
    RSSEXE_LOGD("IPC debug: client send request, current timestamp is %{public}lld.", (long long)curr);

    nlohmann::json reply;
    SendRequestInner(ResIpcType::REQUEST_DEBUG, data, option, reply);
    return ResErrCode::RSSEXE_NO_ERR;
}

} // namespace ResourceSchedule
} // namespace OHOS
