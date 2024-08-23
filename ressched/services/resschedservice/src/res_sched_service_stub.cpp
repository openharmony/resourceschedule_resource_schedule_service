/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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
#include "res_sched_service_utils.h"
#include "hisysevent.h"

namespace OHOS {
namespace ResourceSchedule {
using namespace OHOS::Security;
namespace {
    #define PAYLOAD_MAX_SIZE 4096
    constexpr int32_t MEMMGR_UID = 1111;
    constexpr int32_t SAMGR_UID = 5555;
    constexpr int32_t FOUNDATION_UID = 5523;
    constexpr int32_t HIVIEW_UID = 1201;
    constexpr int32_t SINGLE_UID_REQUEST_LIMIT_COUNT = 250;
    constexpr int32_t ALL_UID_REQUEST_LIMIT_COUNT = 650;
    constexpr int32_t LIMIT_REQUEST_TIME = 1000;
    constexpr int64_t FOUR_HOUR_TIME = 4 * 60 * 60 * 1000;
    static const std::unordered_set<uint32_t> scbRes = {
        ResType::SYNC_RES_TYPE_THAW_ONE_APP,
        ResType::RES_TYPE_REPORT_SCENE_BOARD,
        ResType::RES_TYPE_SHOW_REMOTE_ANIMATION,
        ResType::RES_TYPE_KEY_PERF_SCENE,
        ResType::RES_TYPE_MOVE_WINDOW,
        ResType::RES_TYPE_RESIZE_WINDOW,
        ResType::RES_TYPE_ONLY_PERF_APP_COLD_START,
        ResType::RES_TYPE_SCENE_ROTATION,
    };
    static const std::unordered_set<uint32_t> thirdPartRes_ = {
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
        ResType::RES_TYPE_REPORT_RENDER_THREAD,
        ResType::RES_TYPE_LONG_FRAME,
        ResType::RES_TYPE_REPORT_DISTRIBUTE_TID,
        ResType::RES_TYPE_REPORT_VSYNC_TID,
        ResType::RES_TYPE_WEB_DRAG_RESIZE,
        ResType::RES_TYPE_WEBVIEW_SCREEN_CAPTURE,
        ResType::RES_TYPE_WEBVIEW_VIDEO_STATUS_CHANGE,
        ResType::RES_TYPE_AUDIO_SILENT_PLAYBACK,
    };
    static const std::unordered_set<uint32_t> saRes_ = {
        ResType::SYNC_RES_TYPE_THAW_ONE_APP,
        ResType::SYNC_RES_TYPE_GET_ALL_SUSPEND_STATE,
        ResType::RES_TYPE_SCREEN_STATUS,
        ResType::RES_TYPE_APP_STATE_CHANGE,
        ResType::RES_TYPE_ABILITY_STATE_CHANGE,
        ResType::RES_TYPE_EXTENSION_STATE_CHANGE,
        ResType::RES_TYPE_PROCESS_STATE_CHANGE,
        ResType::RES_TYPE_WINDOW_FOCUS,
        ResType::RES_TYPE_TRANSIENT_TASK,
        ResType::RES_TYPE_CONTINUOUS_TASK,
        ResType::RES_TYPE_CGROUP_ADJUSTER,
        ResType::RES_TYPE_WINDOW_VISIBILITY_CHANGE,
        ResType::RES_TYPE_REPORT_MMI_PROCESS,
        ResType::RES_TYPE_APP_INSTALL_UNINSTALL,
        ResType::RES_TYPE_WIFI_CONNECT_STATE_CHANGE,
        ResType::RES_TYPE_USER_SWITCH,
        ResType::RES_TYPE_USER_REMOVE,
        ResType::RES_TYPE_SCREEN_LOCK,
        ResType::RES_TYPE_BLUETOOTH_A2DP_CONNECT_STATE_CHANGE,
        ResType::RES_TYPE_NETWORK_LATENCY_REQUEST,
        ResType::RES_TYPE_CALL_STATE_UPDATE,
        ResType::RES_TYPE_THREAD_QOS_CHANGE,
        ResType::RES_TYPE_AUDIO_RENDER_STATE_CHANGE,
        ResType::RES_TYPE_AUDIO_RING_MODE_CHANGE,
        ResType::RES_TYPE_AUDIO_VOLUME_KEY_CHANGE,
        ResType::RES_TYPE_APP_ABILITY_START,
        ResType::RES_TYPE_DEVICE_STILL_STATE_CHANGE,
        ResType::RES_TYPE_RESIZE_WINDOW,
        ResType::RES_TYPE_MOVE_WINDOW,
        ResType::RES_TYPE_SHOW_REMOTE_ANIMATION,
        ResType::RES_TYPE_REPORT_CAMERA_STATE,
        ResType::RES_TYPE_RUNNINGLOCK_STATE,
        ResType::RES_TYPE_DRAG_STATUS_BAR,
        ResType::RES_TYPE_REPORT_SCENE_BOARD,
        ResType::RES_TYPE_MMI_INPUT_STATE,
        ResType::RES_TYPE_ANCO_CUST,
        ResType::RES_TYPE_TIMEZONE_CHANGED,
        ResType::RES_TYPE_APP_ASSOCIATED_START,
        ResType::RES_TYPE_THERMAL_STATE,
        ResType::RES_TYPE_REPORT_SCREEN_CAPTURE,
        ResType::RES_TYPE_KEY_PERF_SCENE,
        ResType::RES_TYPE_SUPER_LAUNCHER,
        ResType::RES_TYPE_CAST_SCREEN,
        ResType::RES_TYPR_SCREEN_COLLABROATION,
        ResType::RES_TYPE_SA_CONTROL_APP_EVENT,
        ResType::RES_TYPE_SYSTEM_CPU_LOAD,
        ResType::RES_TYPE_UPLOAD_DOWNLOAD,
        ResType::RES_TYPE_SPLIT_SCREEN,
        ResType::RES_TYPE_FLOATING_WINDOW,
        ResType::RES_TYPE_FRAME_RATE_REPORT,
        ResType::RES_TYPE_LOCATION_STATUS_CHANGE,
        ResType::RES_TYPE_REPORT_DISTRIBUTE_COMPONENT_CHANGE,
        ResType::RES_TYPE_FORM_STATE_CHANGE_EVENT,
        ResType::RES_TYPE_THERMAL_SCENARIO_REPORT,
        ResType::RES_TYPE_BOOT_COMPLETED,
        ResType::RES_TYPE_CONTINUOUS_STARTUP,
        ResType::RES_TYPE_AUDIO_SILENT_PLAYBACK,
        ResType::RES_TYPE_AUDIO_RENDERER_SILENT_PLAYBACK,
        ResType::RES_TYPE_REPORT_GAME_SCHED,
        ResType::RES_TYPE_CLOUD_CONFIG_UPDATE,
    };
    const std::string NEEDED_PERMISSION = "ohos.permission.REPORT_RESOURCE_SCHEDULE_EVENT";
    const std::string SCENEBOARD_BUNDLE_NAME = "com.ohos.sceneboard";

    bool IsValidToken(MessageParcel& data)
    {
        std::u16string descriptor = ResSchedServiceStub::GetDescriptor();
        std::u16string remoteDescriptor = data.ReadInterfaceToken();
        return descriptor == remoteDescriptor;
    }

    bool IsHasPermission(const uint32_t type, const std::unordered_set<uint32_t>& saRes)
    {
        if (saRes.find(type) == saRes.end()) {
            RESSCHED_LOGE("resType:%{public}d not sa report", type);
            return false;
        }
        AccessToken::AccessTokenID tokenId = OHOS::IPCSkeleton::GetCallingTokenID();
        auto tokenType = Security::AccessToken::AccessTokenKit::GetTokenTypeFlag(tokenId);
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

    bool IsTypeVaild(const uint32_t type)
    {
        return type >= ResType::RES_TYPE_FIRST && type < ResType::RES_TYPE_LAST;
    }

    bool IsThirdPartType(const uint32_t type, const std::unordered_set<uint32_t>& thirdPartRes)
    {
        if (thirdPartRes.find(type) == thirdPartRes.end()) {
            RESSCHED_LOGD("resType:%{public}d not hap app report", type);
            return false;
        }
        AccessToken::AccessTokenID tokenId = OHOS::IPCSkeleton::GetCallingTokenID();
        auto tokenType = Security::AccessToken::AccessTokenKit::GetTokenTypeFlag(tokenId);
        if (tokenType != Security::AccessToken::ATokenTypeEnum::TOKEN_HAP) {
            RESSCHED_LOGE("not hap app");
            return false;
        }

        return true;
    }

    bool IsSBDResType(uint32_t type)
    {
        if (scbRes.find(type) == scbRes.end()) {
            return false;
        }
        AccessToken::AccessTokenID tokenId = OHOS::IPCSkeleton::GetCallingTokenID();
        auto tokenType = Security::AccessToken::AccessTokenKit::GetTokenTypeFlag(tokenId);
        if (tokenType != Security::AccessToken::ATokenTypeEnum::TOKEN_HAP) {
            return false;
        }
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
    uint32_t type = 0;
    int64_t value = 0;
    std::string payload;
    int32_t ret = ParseAndCheckReportDataParcel(data, type, value, payload);
    if (ret != ERR_OK) {
        RESSCHED_LOGE("%{public}s: parse fail=%{public}d type=%{public}u", __func__, ret, type);
        return ret;
    }

    ReportData(type, value, StringToJsonObj(payload));
    return ERR_OK;
}

int32_t ResSchedServiceStub::ParseAndCheckReportDataParcel(MessageParcel& data, uint32_t& type, int64_t& value,
    std::string& payload)
{
    if (!IsValidToken(data)) {
        return ERR_RES_SCHED_PARCEL_ERROR;
    }

    READ_PARCEL(data, Uint32, type, ERR_RES_SCHED_PARCEL_ERROR, ResSchedServiceStub);
    if (!IsTypeVaild(type)) {
        RESSCHED_LOGE("type:%{public}u is invalid", type);
        return ERR_RES_SCHED_PARCEL_ERROR;
    }
    if (!IsSBDResType(type) && !IsThirdPartType(type, thirdPartRes_) && !IsHasPermission(type, saRes_)) {
        RESSCHED_LOGE("type:%{public}u, no permission", type);
        return ERR_RES_SCHED_PERMISSION_DENIED;
    }

    READ_PARCEL(data, Int64, value, ERR_RES_SCHED_PARCEL_ERROR, ResSchedServiceStub);
    READ_PARCEL(data, String, payload, ERR_RES_SCHED_PARCEL_ERROR, ResSchedServiceStub);
    if (payload.size() > PAYLOAD_MAX_SIZE) {
        RESSCHED_LOGE("too long payload.size:%{public}u", (uint32_t)payload.size());
        return ERR_RES_SCHED_PARCEL_ERROR;
    }
    return ERR_OK;
}

int32_t ResSchedServiceStub::ReportSyncEventInner(MessageParcel& input, MessageParcel& output)
{
    uint32_t type = 0;
    int64_t value = 0;
    std::string payloadStr;
    int32_t ret = ParseAndCheckReportDataParcel(input, type, value, payloadStr);
    if (ret != ERR_OK) {
        RESSCHED_LOGE("%{public}s: parse fail=%{public}d type=%{public}u", __func__, ret, type);
        return ret;
    }

    nlohmann::json payload = StringToJsonObj(payloadStr);
    int32_t clientPid = IPCSkeleton::GetCallingPid();
    int32_t callingUid = IPCSkeleton::GetCallingUid();
    payload["clientPid"] = std::to_string(clientPid);
    payload["callingUid"] = std::to_string(callingUid);
    nlohmann::json reply;
    ret = ReportSyncEvent(type, value, payload, reply);
    RESSCHED_LOGD("%{public}s: clientPid=%{public}d, callingUid=%{public}d, type=%{public}u, value=%{public}lld, "
                  "ret=%{public}d", __func__, clientPid, callingUid, type, (long long)value, ret);
    WRITE_PARCEL(output, Int32, ret, ERR_RES_SCHED_PARCEL_ERROR, ResSchedServiceStub);
    WRITE_PARCEL(output, String, reply.dump(-1, ' ', false, nlohmann::detail::error_handler_t::replace),
        ERR_RES_SCHED_PARCEL_ERROR, ResSchedServiceStub);
    return ret;
}

int32_t ResSchedServiceStub::KillProcessInner(MessageParcel& data, MessageParcel& reply)
{
    if (!IsValidToken(data)) {
        return ERR_RES_SCHED_PARCEL_ERROR;
    }
    uint32_t accessToken = IPCSkeleton::GetCallingTokenID();
    int32_t uid = IPCSkeleton::GetCallingUid();
    Security::AccessToken::ATokenTypeEnum tokenTypeFlag = Security::AccessToken::AccessTokenKit::GetTokenTypeFlag(accessToken);
    if ((uid != MEMMGR_UID && uid != SAMGR_UID && uid != HIVIEW_UID)
        || tokenTypeFlag != Security::AccessToken::ATokenTypeEnum::TOKEN_NATIVE) {
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

void ResSchedServiceStub::RegisterSystemloadNotifierInner(MessageParcel& data,
                                                          [[maybe_unused]] MessageParcel& reply)
{
    if (!IsValidToken(data)) {
        RESSCHED_LOGE("Register invalid token.");
        return;
    }
    sptr<IRemoteObject> notifier = data.ReadRemoteObject();
    if (notifier == nullptr) {
        RESSCHED_LOGE("ResSchedServiceStub Read notifier fail.");
        return;
    }
    RegisterSystemloadNotifier(notifier);
}

void ResSchedServiceStub::UnRegisterSystemloadNotifierInner(MessageParcel& data,
                                                            [[maybe_unused]] MessageParcel& reply)
{
    if (!IsValidToken(data)) {
        RESSCHED_LOGE("UnRegister invalid token.");
        return;
    }
    UnRegisterSystemloadNotifier();
}

int32_t ResSchedServiceStub::GetSystemloadLevelInner(MessageParcel& data, MessageParcel& reply)
{
    if (!IsValidToken(data)) {
        RESSCHED_LOGE("GetSystemload level invalid token.");
        return ERR_RES_SCHED_PARCEL_ERROR;
    }
    int32_t level = GetSystemloadLevel();
    if (!reply.WriteInt32(level)) {
        RESSCHED_LOGE("GetSystemload level write reply failed.");
        return ERR_RES_SCHED_PARCEL_ERROR;
    }
    return ERR_OK;
}

bool ResSchedServiceStub::IsAllowedAppPreloadInner(MessageParcel& data, MessageParcel& reply)
{
    if (!IsValidToken(data)) {
        return false;
    }

    AccessToken::AccessTokenID tokenId = IPCSkeleton::GetCallingTokenID();
    int32_t uid = IPCSkeleton::GetCallingUid();
    AccessToken::ATokenTypeEnum tokenTypeFlag = AccessToken::AccessTokenKit::GetTokenTypeFlag(tokenId);
    if (uid != FOUNDATION_UID || tokenTypeFlag != AccessToken::ATokenTypeEnum::TOKEN_NATIVE) {
        RESSCHED_LOGE("Invalid calling token");
        return false;
    }

    std::string bundleName {""};
    int32_t preloadMode {0};
    if (!data.ReadString(bundleName) || !data.ReadInt32(preloadMode)) {
        RESSCHED_LOGE("IsAllowedAppPreloadInner ReadParcelable failed");
        return false;
    }

    bool isAllowedPreload = IsAllowedAppPreload(bundleName, preloadMode);
    if (!reply.WriteBool(isAllowedPreload)) {
        RESSCHED_LOGE("IsAllowedAppPreloadInner write isAllowedPreload failed");
        return false;
    }
    return true;
}

bool ResSchedServiceStub::IsLimitRequest(int32_t uid)
{
    std::lock_guard<std::mutex> lock(mutex_);
    int64_t nowTime = ResSchedUtil::GetNowMillTime();
    CheckAndUpdateLimitData(nowTime);
    if (allRequestCount_.load() >= ALL_UID_REQUEST_LIMIT_COUNT) {
        RESSCHED_LOGD("all uid request is limit, %{public}d request fail", uid);
        return true;
    }
    auto iter = appRequestCountMap_.find(uid);
    if (iter == appRequestCountMap_.end()) {
        appRequestCountMap_[uid] = 1;
        allRequestCount_ ++;
        return false;
    }
    if (appRequestCountMap_[uid] >= SINGLE_UID_REQUEST_LIMIT_COUNT) {
        RESSCHED_LOGD("uid:%{public}d request is limit, request fail", uid);
        return true;
    }
    appRequestCountMap_[uid] ++;
    allRequestCount_ ++;
    return false;
}

void ResSchedServiceStub::CheckAndUpdateLimitData(int64_t nowTime)
{
    if (nowTime - nextCheckTime_.load() > LIMIT_REQUEST_TIME) {
        nextCheckTime_.store(nowTime + LIMIT_REQUEST_TIME);
        appRequestCountMap_.clear();
        allRequestCount_.store(0);
        isPrintLimitLog_.store(true);
    }
}

void ResSchedServiceStub::PrintLimitLog(int32_t uid)
{
    if (isPrintLimitLog_.load()) {
        isPrintLimitLog_.store(false);
        RESSCHED_LOGI("request limit, allRequestCount_:%{public}d, cur report uid:%{public}d",
            allRequestCount_.load(), uid);
    }
}

void ResSchedServiceStub::ReportBigData()
{
    if (!isReportBigData_.load()) {
        return;
    }
    if (ResSchedUtils::GetNowMillTime() < nextReportBigDataTime_) {
        return;
    }
    HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::RSS, "SERVICE_REQUEST_LIMIT",
                    HiviewDFX::HiSysEvent::EventType::FAULT, "REQUEST_LIMIT_COUNT", bigDataReportCount_.load());
    isReportBigData_.store(false);
    bigDataReportCount_.store(0);
}

void ResSchedServiceStub::InreaseBigDataCount()
{
    if (!isReportBigData_.load()) {
        isReportBigData_.store(true);
        nextReportBigDataTime_ = ResSchedUtils::GetNowMillTime() + FOUR_HOUR_TIME;
    }
    bigDataReportCount_ ++;
}

int32_t ResSchedServiceStub::OnRemoteRequest(uint32_t code, MessageParcel &data,
    MessageParcel &reply, MessageOption &option)
{
    ReportBigData();
    auto uid = IPCSkeleton::GetCallingUid();
    RESSCHED_LOGD("ResSchedServiceStub::OnRemoteRequest, code = %{public}u, flags = %{public}d,"
        " uid = %{public}d.", code, option.GetFlags(), uid);
    if (IsLimitRequest(uid)) {
        RESSCHED_LOGD("%{public}d is limit request, cur request fail", uid);
        InreaseBigDataCount();
        PrintLimitLog(uid);
        return RES_SCHED_REQUEST_FAIL;
    }
    switch (code) {
        case static_cast<uint32_t>(ResourceScheduleInterfaceCode::REPORT_DATA):
            return ReportDataInner(data, reply);
        case static_cast<uint32_t>(ResourceScheduleInterfaceCode::REPORT_SYNC_EVENT):
            return ReportSyncEventInner(data, reply);
        case static_cast<uint32_t>(ResourceScheduleInterfaceCode::KILL_PROCESS):
            return KillProcessInner(data, reply);
        case static_cast<uint32_t>(ResourceScheduleInterfaceCode::REGISTER_SYSTEMLOAD_NOTIFIER):
            RegisterSystemloadNotifierInner(data, reply);
            return ERR_OK;
        case static_cast<uint32_t>(ResourceScheduleInterfaceCode::UNREGISTER_SYSTEMLOAD_NOTIFIER):
            UnRegisterSystemloadNotifierInner(data, reply);
            return ERR_OK;
        case static_cast<uint32_t>(ResourceScheduleInterfaceCode::GET_SYSTEMLOAD_LEVEL):
            return GetSystemloadLevelInner(data, reply);
        case static_cast<uint32_t>(ResourceScheduleInterfaceCode::TOUCH_DOWN_APP_PRELOAD):
            return IsAllowedAppPreloadInner(data, reply);
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
        RESSCHED_LOGD("%{public}s payload converted result is not a jsonObj: %{public}s.", __func__, payload.c_str());
        return jsonObj;
    }
    return jsonTmp;
}

void ResSchedServiceStub::Init()
{
}
} // namespace ResourceSchedule
} // namespace OHOS
