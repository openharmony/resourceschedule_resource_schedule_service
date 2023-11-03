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

#include "res_sched_service_stub.h"
#include <cstdint>
#include "res_sched_errors.h"
#include "res_sched_log.h"
#include "res_sched_ipc_interface_code.h"
#include "res_type.h"
#include "accesstoken_kit.h"
#include "ipc_skeleton.h"
#include "ipc_util.h"

namespace OHOS {
namespace ResourceSchedule {
namespace {
    #define PAYLOAD_MAX_SIZE 4096
    constexpr int32_t FOUNDATION_UID = 5523;
    constexpr int32_t INPUT_UID = 6696;
    constexpr int32_t DHARDWARE_UID = 3056;
    constexpr int32_t SAMGR_UID = 5555;

    bool IsValidToken(MessageParcel& data)
    {
        std::u16string descriptor = ResSchedServiceStub::GetDescriptor();
        std::u16string remoteDescriptor = data.ReadInterfaceToken();
        return descriptor == remoteDescriptor;
    }
}

ResSchedServiceStub::ResSchedServiceStub()
{
    Init();
}

ResSchedServiceStub::~ResSchedServiceStub()
{
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
    int32_t uid = IPCSkeleton::GetCallingUid();
    Security::AccessToken::ATokenTypeEnum tokenTypeFlag = Security::AccessToken::AccessTokenKit::GetTokenTypeFlag(accessToken);
    if (uid != SAMGR_UID || tokenTypeFlag != Security::AccessToken::ATokenTypeEnum::TOKEN_NATIVE) {
        RESSCHED_LOGE("no permission， kill process fail");
        return RES_SCHED_KILL_PROCESS_FAIL;
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
            return ReportDataInner(data, reply);
        case static_cast<uint32_t>(ResourceScheduleInterfaceCode::KILL_PROCESS):
            return KillProcessInner(data, reply);
        default:
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
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

void ResSchedServiceStub::Init()
{
    resource_in_process = {
        ResType::RES_TYPE_CGROUP_ADJUSTER,
        ResType::RES_TYPE_APP_STATE_CHANGE,
        ResType::RES_TYPE_ABILITY_STATE_CHANGE,
        ResType::RES_TYPE_EXTENSION_STATE_CHANGE,
        ResType::RES_TYPE_PROCESS_STATE_CHANGE,
        ResType::RES_TYPE_WINDOW_FOCUS,
        ResType::RES_TYPE_WINDOW_VISIBILITY_CHANGE,
        ResType::RES_TYPE_TRANSIENT_TASK,
        ResType::RES_TYPE_WIFI_CONNECT_STATE_CHANGE,
        ResType::RES_TYPE_SCREEN_STATUS,
        ResType::RES_TYPE_SCREEN_LOCK,
        ResType::RES_TYPE_APP_INSTALL_UNINSTALL,
        ResType::RES_TYPE_USER_SWITCH,
        ResType::RES_TYPE_USER_REMOVE,
        ResType::RES_TYPE_CALL_STATE_UPDATE,
        ResType::RES_TYPE_AUDIO_RENDER_STATE_CHANGE,
        ResType::RES_TYPE_AUDIO_RING_MODE_CHANGE,
        ResType::RES_TYPE_AUDIO_VOLUME_KEY_CHANGE,
        ResType::RES_TYPE_DEVICE_STILL_STATE_CHANGE,
        ResType::RES_TYPE_CONTINUOUS_TASK,
    };
    resource_uid_other_process = {
        { ResType::RES_TYPE_APP_ABILITY_START, FOUNDATION_UID },
        { ResType::RES_TYPE_REPORT_MMI_PROCESS, INPUT_UID },
        { ResType::RES_TYPE_NETWORK_LATENCY_REQUEST, DHARDWARE_UID },
    };
}
} // namespace ResourceSchedule
} // namespace OHOS
