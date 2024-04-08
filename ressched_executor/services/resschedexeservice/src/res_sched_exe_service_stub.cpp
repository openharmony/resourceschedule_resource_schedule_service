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

int32_t ResSchedExeServiceStub::ReportRequestInner(MessageParcel& data, MessageParcel& reply, int32_t uid)
{
    uint32_t resType = 0;
    int64_t value = 0;
    nlohmann::json context;
    nlohmann::json result;
    if (!ParseParcel(data, resType, value, context)) {
        result["errorNo"] = std::to_string(ResIpcErrCode::RSSEXE_DATA_ERROR);
        reply.WriteString(result.dump(-1, ' ', false, nlohmann::detail::error_handler_t::replace));
        reply.WriteInt32(ResIpcErrCode::RSSEXE_DATA_ERROR);
        return ResIpcErrCode::RSSEXE_DATA_ERROR;
    }
    int32_t clientPid = IPCSkeleton::GetCallingPid();
    RSSEXE_LOGD("receive data from ipc resType: %{public}u, value: %{public}lld, pid: %{public}d",
        resType, (long long)value, clientPid);

    int32_t ret = 0;
    if (context.size() <= PAYLOAD_MAX_SIZE) {
        context["callingUid"] = std::to_string(uid);
        context["clientPid"] = std::to_string(clientPid);
        if (IsTypeSync(resType)) {
            ret = SendResRequest(resType, value, context, result);
        } else {
            ReportData(resType, value, context);
            ret = ResErrCode::RSSEXE_NO_ERR;
        }
    } else {
        RSSEXE_LOGE("The payload is too long. DoS.");
        ret = ResIpcErrCode::RSSEXE_DATA_ERROR;
    }
    result["errorNo"] = std::to_string(ret);
    reply.WriteString(result.dump(-1, ' ', false, nlohmann::detail::error_handler_t::replace));
    reply.WriteInt32(ret);
    return ret;
}

int32_t ResSchedExeServiceStub::ReportDebugInner(MessageParcel& data, MessageParcel& reply)
{
    if (!IsValidToken(data)) {
        RSSEXE_LOGE("token is invalid");
        return ResIpcErrCode::RSSEXE_DATA_ERROR;
    }

    uint32_t resType = 0;
    READ_PARCEL(data, Uint32, resType, ResIpcErrCode::RSSEXE_DATA_ERROR, ResSchedExeServiceStub);
    if (!IsTypeDebug(resType)) {
        return ResIpcErrCode::RSSEXE_DATA_ERROR;
    }

    uint64_t curr = ResSchedExeCommonUtils::GetCurrentTimestampUs();
    uint64_t start;
    READ_PARCEL(data, Uint64, start, ResIpcErrCode::RSSEXE_DATA_ERROR, ResSchedExeServiceStub);
    RSSEXE_LOGD("IPC debug: server recieve request, current timestamp is %{public}lld.", (long long)curr);
    RSSEXE_LOGD("IPC debug: server recieve request, cost tome is %{public}lld.", (long long)(curr - start));
    return ResErrCode::RSSEXE_NO_ERR;
}

int32_t ResSchedExeServiceStub::OnRemoteRequest(uint32_t code, MessageParcel &data,
    MessageParcel &reply, MessageOption &option)
{
    auto uid = IPCSkeleton::GetCallingUid();
    RSSEXE_LOGD("code = %{public}u, flags = %{public}d, uid = %{public}d.", code, option.GetFlags(), uid);

    switch (code) {
        case ResIpcType::RES_REQUEST:
            return ReportRequestInner(data, reply, uid);
        case ResIpcType::REPORT_DATA:
            return ReportRequestInner(data, reply, uid);
        case ResIpcType::REQUEST_DEBUG:
            return ReportDebugInner(data, reply);
        default:
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
}

bool ResSchedExeServiceStub::ParseParcel(MessageParcel& data,
    uint32_t& resType, int64_t& value, nlohmann::json& context)
{
    if (!IsValidToken(data)) {
        RSSEXE_LOGE("token is invalid");
        return false;
    }

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

