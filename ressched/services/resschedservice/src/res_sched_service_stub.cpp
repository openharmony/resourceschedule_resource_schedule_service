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
#include "accesstoken_kit.h"

namespace OHOS {
namespace ResourceSchedule {
using namespace OHOS::Security;
namespace {
    #define PAYLOAD_MAX_SIZE 4096
    constexpr int32_t SAMGR_UID = 5555;
    const std::string NEEDED_PERMISSION = "ohos.permission.REPORT_RESOURCE_SCHEDULE_EVENT";
    const std::string SCENEBOARD_BUNDLE_NAME = "com.ohos.sceneboard";

    bool IsValidToken(MessageParcel& data)
    {
        std::u16string descriptor = ResSchedServiceStub::GetDescriptor();
        std::u16string remoteDescriptor = data.ReadInterfaceToken();
        return descriptor == remoteDescriptor;
    }

    bool IsHasPermission() {
        AccessToken::AccessTokenID tokenId = OHOS::IPCSkeleton::GetCallingTokenID();
        auto tokenType = Security::AccessToken::AccessTokenKit::GetTokenFlag(tokenId);
        if (tokenType != Security::AccessToken::ATokenTypeEnum::TOKEN_NATIVE) {
            RESSCHED_LOGE("not native sa");
            return false;
        }
        int32_t hasPermission = AccessToken::AccessTokenKit::VerifyAccessToken(tokenId, NEEDED_PERMISSION);
        if (hasPermission != 0) {
            RESSCHED_LOGE("not have permission");
            return false;
        }
        return true;
    }

    bool IsTypeVaild(uint32_t type) {
        return type >= ResType::RES_TYPE_FIRST && type < ResType::RES_TYPE_LAST;
    }

    bool IsThirdPartType(uint32_t type, std::unordered_set<uint32_t> thirdPartRes) {
        if (thirdPartRes.find(type) != thirdPartRes.end()) {
            return true;
        }
        return false;
    }

    bool IsSBDResType(uint32_t type) {
        if (type != ResType::RES_TYPE_REPORT_SCENE_BOARD) {
            return false;
        }
        AccessToken::AccessTokenID tokenId = OHOS::IPCSkeleton::GetCallingTokenID();
        AccessToken::HapTokenInfo callingTokenInfo;
        AccessToken::AccessTokenKit::GetHapTokenInfo(tokenId, callingTokenInfo);
        if (callingTokenInfo.bundleName == SCENEBOARD_BUNDLE_NAME) {
            return true;
        }
        RESSCHED_LOGE("%{public}s is not sceneboard bundle name", callingTokenInfo.bundleName.c_str());
        return false;
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
    if (!IsTypeVaild(type)) {
        RESSCHED_LOGE("type:%{public}d is invalid", type);
        return ERR_RES_SCHED_PARCEL_ERROR;
    }
    if (!IsSBDResType(type) && !IsThirdPartType(type, thirdPartRes_) && !isHasPermission()) {
        return ERR_RES_SCHED_PERMISSION_DENIED;
    }
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
    uint32_t accessToken = IPCSkeleton::GetCallingTokenID();
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
    thirdPartRes_ = {
        ResType::RES_TYPE_CLICK_RECOGNIZE,
        ResType::RES_TYPE_PUSH_PAGE,
        ResType::RES_TYPE_SLIDE_RECOGNIZE,
        ResType::RES_TYPE_POP_PAGE,
        ResType::RES_TYPE_LOAD_PAGE,
        ResType::RES_TYPE_WEB_GESTURE,
        ResType::RES_TYPE_REPORT_KEY_THREAD,
        ResType::RES_TYPE_REPORT_WINDOW_STATE,
        ResType::RES_TYPE_REPORT_SCENE_SCHED,
        ResType::RES_TYPE_WEB_GESTURE_MOVE,
        ResType::RES_TYPE_WEB_SLIDE_NORMAL,
        ResType::RES_TYPE_LOAD_URL,
        ResType::RES_TYPE_MOUSEWHEEL,
        ResType::RES_TYPE_WEBVIEW_AUDIO_STATUS_CHANGE,
    };
}
} // namespace ResourceSchedule
} // namespace OHOS
