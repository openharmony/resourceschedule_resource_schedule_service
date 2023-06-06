/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#include <cstdint>
#include "res_sched_service_stub.h"
#include "res_sched_log.h"
#include "res_sched_errors.h"
#include "ipc_skeleton.h"
#include "ipc_util.h"

namespace OHOS {
namespace ResourceSchedule {
namespace {
    #define PAYLOAD_MAX_SIZE 4096

    bool IsValidToken(MessageParcel& data)
    {
        std::u16string descriptor = ResSchedServiceStub::GetDescriptor();
        std::u16string remoteDescriptor = data.ReadInterfaceToken();
        return descriptor == remoteDescriptor;
    }
}

int32_t ResSchedServiceStub::ReportDataInner(MessageParcel& data, [[maybe_unused]] MessageParcel& reply)
{
    if (!IsValidToken(data)) {
        return ERR_RES_SCHED_PARCEL_ERROR;
    }
    uint32_t type = 0;
    READ_PARCEL(data, Uint32, type, ERR_RES_SCHED_PARCEL_ERROR, ResSchedServiceStub);

    int64_t value = 0;
    READ_PARCEL(data, Int64, value, ERR_RES_SCHED_PARCEL_ERROR, ResSchedServiceStub);

    std::string payload;
    READ_PARCEL(data, String, payload, ERR_RES_SCHED_PARCEL_ERROR, ResSchedServiceStub);

    if (payload.size() <= PAYLOAD_MAX_SIZE) {
        ReportData(type, value, StringToJsonObj(payload));
    } else {
        RESSCHED_LOGE("The payload is too long. DoS.");
    }
    return ERR_OK;
}

int32_t ResSchedServiceStub::KillProcessInner(MessageParcel& data, MessageParcel& reply)
{
    if (!IsValidToken(data)) {
        return ERR_RES_SCHED_PARCEL_ERROR;
    }
    std::string payload;
    READ_PARCEL(data, String, payload, ERR_RES_SCHED_PARCEL_ERROR, ResSchedServiceStub);
    if (payload.size() <= PAYLOAD_MAX_SIZE) {
        int32_t status = KillProcess(StringToJsonObj(payload));
        reply.WriteInt32(status);
    } else {
        reply.WriteInt32(RES_SCHED_DATA_ERROR);
        RESSCHED_LOGE("The payload is too long. DoS.");
    }
    return ERR_OK;
}

int32_t ResSchedServiceStub::OnRemoteRequest(uint32_t code, MessageParcel &data,
    MessageParcel &reply, MessageOption &option)
{
    auto uid = IPCSkeleton::GetCallingUid();
    RESSCHED_LOGD("ResSchedServiceStub::OnRemoteRequest, code = %{public}u, flags = %{public}d,"
        " uid = %{public}d.", code, option.GetFlags(), uid);

    switch (code) {
        case static_cast<uint32_t>(ResourceScheduleInterfaceCode::REPORT_DATA):
            ReportDataInner(data, reply);
            break;
        case static_cast<uint32_t>(ResourceScheduleInterfaceCode::KILL_PROCESS):
            KillProcessInner(data, reply);
            break;
        default:
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
    return ERR_OK;
}

nlohmann::json ResSchedServiceStub::StringToJsonObj(const std::string& payload)
{
    nlohmann::json jsonObj = nlohmann::json::object();
    if (payload.empty()) {
        return jsonObj;
    }
    nlohmann::json jsonTmp = nlohmann::json::parse(payload, nullptr, false);
    if (jsonTmp.is_discarded()) {
        RESSCHED_LOGE("%{public}s parse payload to json failed: %{public}s.", __func__, payload.c_str());
        return jsonObj;
    }
    if (!jsonTmp.is_object()) {
        RESSCHED_LOGE("%{public}s payload converted result is not a jsonObj: %{public}s.", __func__, payload.c_str());
        return jsonObj;
    }
    return jsonTmp;
}
} // namespace ResourceSchedule
} // namespace OHOS
