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

#include "res_sched_exe_service_stub.h"

#include <string_ex.h>

#include "ipc_skeleton.h"

#include "ipc_util.h"
#include "res_exe_type.h"
#include "res_sched_exe_common_utils.h"
#include "res_sched_exe_constants.h"
#include "res_sched_exe_log.h"

namespace OHOS {
namespace ResourceSchedule {
namespace {
    constexpr int32_t PAYLOAD_MAX_SIZE = 4096;
    constexpr int32_t KILL_PROCESS_FAILED = -1;
    constexpr int32_t RSS_UID = 0;
    const std::string RES_TYPE_EXT = "extType";

    bool IsValidToken(MessageParcel& data)
    {
        std::u16string descriptor = ResSchedExeServiceStub::GetDescriptor();
        std::u16string remoteDescriptor = data.ReadInterfaceToken();
        return descriptor == remoteDescriptor;
    }

    bool IsTypeVaild(uint32_t type)
    {
        return type >= ResExeType::RES_TYPE_FIRST && type < ResExeType::RES_TYPE_LAST;
    }

    bool IsTypeSync(uint32_t type)
    {
        return IsTypeVaild(type) && type < ResExeType::RES_TYPE_SYNC_END;
    }

    bool IsTypeDebug(uint32_t type)
    {
        return type == ResExeType::RES_TYPE_DEBUG;
    }

    bool GetExtResType(uint32_t& resType, const nlohmann::json& context)
    {
        if (resType != ResExeType::RES_TYPE_COMMON_SYNC && resType != ResExeType::RES_TYPE_COMMON_ASYNC) {
            return true;
        }
        int type = 0;
        if (!context.contains(RES_TYPE_EXT) || !context[RES_TYPE_EXT].is_string()
            || !StrToInt(context[RES_TYPE_EXT], type)) {
            RSSEXE_LOGE("use extend resType, but not send resTypeExt with payload");
            return false;
        }
        resType = (uint32_t)type;
        RSSEXE_LOGD("use extend resType = %{public}d.", resType);
        return true;
    }

    bool IsCallingClientRss()
    {
        int32_t clientUid = IPCSkeleton::GetCallingUid();
        RSSEXE_LOGD("calling client uid is %{public}d, allowed uid is %{public}d", clientUid, RSS_UID);
        return RSS_UID == clientUid;
    }
}

ResSchedExeServiceStub::ResSchedExeServiceStub()
{
    Init();
}

ResSchedExeServiceStub::~ResSchedExeServiceStub()
{
}

void ResSchedExeServiceStub::Init()
{
}

int32_t ResSchedExeServiceStub::ReportRequestInner(MessageParcel& data, MessageParcel& reply)
{
    uint32_t resType = 0;
    int64_t value = 0;
    nlohmann::json context;
    if (!ParseParcel(data, resType, value, context)) {
        WRITE_PARCEL(reply, Int32, ResIpcErrCode::RSSEXE_DATA_ERROR,
            ResIpcErrCode::RSSEXE_DATA_ERROR, ResSchedExeServiceStub);
        return ResIpcErrCode::RSSEXE_DATA_ERROR;
    }

    int32_t uid = IPCSkeleton::GetCallingUid();
    int32_t clientPid = IPCSkeleton::GetCallingPid();
    RSSEXE_LOGD("receive data from ipc resType: %{public}u, value: %{public}lld, uid: %{public}d, pid: %{public}d",
        resType, (long long)value, uid, clientPid);

    if (context.size() > PAYLOAD_MAX_SIZE) {
        RSSEXE_LOGE("The payload is too long. DoS.");
        WRITE_PARCEL(reply, Int32, ResIpcErrCode::RSSEXE_DATA_ERROR,
            ResIpcErrCode::RSSEXE_DATA_ERROR, ResSchedExeServiceStub);
        return ResIpcErrCode::RSSEXE_DATA_ERROR;
    }

    bool isSync = IsTypeSync(resType);
    if (!GetExtResType(resType, context)) {
        WRITE_PARCEL(reply, Int32, ResIpcErrCode::RSSEXE_DATA_ERROR,
            ResIpcErrCode::RSSEXE_DATA_ERROR, ResSchedExeServiceStub);
        return ResIpcErrCode::RSSEXE_DATA_ERROR;
    }
    context["callingUid"] = std::to_string(uid);
    context["clientPid"] = std::to_string(clientPid);

    int32_t ret = ResErrCode::RSSEXE_NO_ERR;
    nlohmann::json result;
    if (isSync) {
        ret = SendRequestSync(resType, value, context, result);
    } else {
        SendRequestAsync(resType, value, context);
    }
    WRITE_PARCEL(reply, Int32, ret, ResIpcErrCode::RSSEXE_DATA_ERROR, ResSchedExeServiceStub);
    WRITE_PARCEL(reply, String, result.dump(-1, ' ', false, nlohmann::detail::error_handler_t::replace),
        ResIpcErrCode::RSSEXE_DATA_ERROR, ResSchedExeServiceStub);
    return ret;
}

int32_t ResSchedExeServiceStub::KillProcessInner(MessageParcel& data, MessageParcel& reply)
{
    pid_t pid = -1;
    READ_PARCEL(data, Int32, pid, false, ResSchedExeServiceStub);
    if (pid <= 0) {
        WRITE_PARCEL(reply, Int32, KILL_PROCESS_FAILED, ResIpcErrCode::RSSEXE_DATA_ERROR, ResSchedExeServiceStub);
        return ResIpcErrCode::RSSEXE_DATA_ERROR;
    }

    int32_t ret = KillProcess(pid);
    WRITE_PARCEL(reply, Int32, ret, ResIpcErrCode::RSSEXE_DATA_ERROR, ResSchedExeServiceStub);
    return 0;
}

int32_t ResSchedExeServiceStub::ReportDebugInner(MessageParcel& data)
{
    uint32_t resType = 0;
    READ_PARCEL(data, Uint32, resType, ResIpcErrCode::RSSEXE_DATA_ERROR, ResSchedExeServiceStub);
    if (!IsTypeDebug(resType)) {
        return ResIpcErrCode::RSSEXE_DATA_ERROR;
    }

    uint64_t curr = ResSchedExeCommonUtils::GetCurrentTimestampUs();
    uint64_t start;
    READ_PARCEL(data, Uint64, start, ResIpcErrCode::RSSEXE_DATA_ERROR, ResSchedExeServiceStub);
    RSSEXE_LOGD("IPC debug: server recieve request, current timestamp is %{public}lld.", (long long)curr);
    RSSEXE_LOGD("IPC debug: server recieve request, cost time is %{public}lld.", (long long)(curr - start));
    return ResErrCode::RSSEXE_NO_ERR;
}

int32_t ResSchedExeServiceStub::OnRemoteRequest(uint32_t code, MessageParcel &data,
    MessageParcel &reply, MessageOption &option)
{
    if (!IsCallingClientRss()) {
        RSSEXE_LOGE("calling process has no permission!");
        WRITE_PARCEL(reply, Int32, ResErrCode::RSSEXE_PERMISSION_DENIED,
            ResIpcErrCode::RSSEXE_DATA_ERROR, ResSchedExeServiceStub);
        return ResErrCode::RSSEXE_PERMISSION_DENIED;
    }

    if (!IsValidToken(data)) {
        RSSEXE_LOGE("token is invalid");
        WRITE_PARCEL(reply, Int32, ResIpcErrCode::RSSEXE_DATA_ERROR,
            ResIpcErrCode::RSSEXE_DATA_ERROR, ResSchedExeServiceStub);
        return ResIpcErrCode::RSSEXE_DATA_ERROR;
    }

    RSSEXE_LOGD("code = %{public}u, flags = %{public}d.", code, option.GetFlags());

    switch (code) {
        case ResIpcType::REQUEST_SEND_SYNC:
            return ReportRequestInner(data, reply);
        case ResIpcType::REQUEST_SEND_ASYNC:
            return ReportRequestInner(data, reply);
        case ResIpcType::REQUEST_KILL_PROCESS:
            return KillProcessInner(data, reply);
        case ResIpcType::REQUEST_SEND_DEBUG:
            return ReportDebugInner(data);
        default:
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
}

bool ResSchedExeServiceStub::ParseParcel(MessageParcel& data,
    uint32_t& resType, int64_t& value, nlohmann::json& context)
{
    READ_PARCEL(data, Uint32, resType, false, ResSchedExeServiceStub);
    if (!IsTypeVaild(resType)) {
        RSSEXE_LOGE("type:%{public}d is invalid", resType);
        return false;
    }

    READ_PARCEL(data, Int64, value, false, ResSchedExeServiceStub);

    std::string payload;
    READ_PARCEL(data, String, payload, false, ResSchedExeServiceStub);
    ResSchedExeCommonUtils::StringToJson(payload, context);
    return true;
}
} // namespace ResourceSchedule
} // namespace OHOS

