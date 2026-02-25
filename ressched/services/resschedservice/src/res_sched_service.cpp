/*
 * Copyright (c) 2022-2025 Huawei Device Co., Ltd.
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
#include <dlfcn.h>
#include "res_sched_service.h"
#include <file_ex.h>
#include <parameters.h>
#include <string_ex.h>
#include "ipc_skeleton.h"
#include "notifier_mgr.h"
#include "plugin_mgr.h"
#include "res_sched_errors.h"
#include "res_sched_exe_client.h"
#include "res_sched_log.h"
#include "res_sched_mgr.h"
#include "event_listener_mgr.h"
#include "hisysevent.h"
#include "res_common_util.h"
#include "res_ipc_init.h"
#include "res_sched_hitrace_chain.h"

namespace OHOS {
namespace ResourceSchedule {
namespace {
    #define PAYLOAD_MAX_SIZE 4096
    static constexpr int32_t DUMP_OPTION = 0;
    static constexpr int32_t DUMP_PARAM_INDEX = 1;
    static const int32_t ENG_MODE = OHOS::system::GetIntParameter("const.debuggable", 0);
    static const char* APP_PRELOAD_PLIGIN_NAME = "libapp_preload_plugin.z.so";
    static const char* NEEDED_PERMISSION = "ohos.permission.REPORT_RESOURCE_SCHEDULE_EVENT";
    static const char* SCENEBOARD_BUNDLE_NAME = "com.ohos.sceneboard";
    static constexpr int32_t MEMMGR_UID = 1111;
    static constexpr int32_t SAMGR_UID = 5555;
    static constexpr int32_t FOUNDATION_UID = 5523;
    static constexpr int32_t GRAPHIC_UID = 1003;
    static constexpr int32_t HIVIEW_UID = 1201;
    static constexpr int32_t SINGLE_UID_REQUEST_LIMIT_COUNT = 250;
    static constexpr int32_t ALL_UID_REQUEST_LIMIT_COUNT = 650;
    static constexpr int32_t LIMIT_REQUEST_TIME = 1000;
    static constexpr int64_t FOUR_HOUR_TIME = 4 * 60 * 60 * 1000;
#ifdef RESOURCE_SCHEDULE_SERVICE_WITH_EXT_RES_ENABLE
    static const int32_t DEFAULT_VALUE = -1;
    static const char* EXT_RES_KEY = "extType";
#endif
    static const std::unordered_set<uint32_t> SCB_RES = {
        ResType::SYNC_RES_TYPE_THAW_ONE_APP,
        ResType::RES_TYPE_REPORT_SCENE_BOARD,
        ResType::RES_TYPE_SHOW_REMOTE_ANIMATION,
        ResType::RES_TYPE_KEY_PERF_SCENE,
        ResType::RES_TYPE_MOVE_WINDOW,
        ResType::RES_TYPE_RESIZE_WINDOW,
        ResType::RES_TYPE_ONLY_PERF_APP_COLD_START,
        ResType::RES_TYPE_SCENE_ROTATION,
        ResType::SYNC_RES_TYPE_CHECK_MUTEX_BEFORE_START,
        ResType::RES_TYPE_COSMIC_CUBE_STATE_CHANGE,
        ResType::RES_TYPE_GESTURE_ANIMATION,
        ResType::RES_TYPE_RAISE_WORKER_THREAD_PRIORITY,
        ResType::RES_TYPE_RECENT_BUILD,
        ResType::RES_TYPE_APP_OPT_FROM_RECENT,
        ResType::RES_TYPE_SWIPE_DIRECTION_UP,
        ResType::RES_TYPE_WINDOW_PANEL,
        ResType::RES_TYPE_WINDOW_SPLIT_SCREEN,
    };
    static const std::unordered_set<uint32_t> THIRDPART_RES = {
        ResType::RES_TYPE_CLICK_RECOGNIZE,
        ResType::RES_TYPE_KEY_EVENT,
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
        ResType::RES_TYPE_WEBVIEW_SCREEN_CAPTURE,
        ResType::RES_TYPE_WEBVIEW_VIDEO_STATUS_CHANGE,
        ResType::RES_TYPE_BT_SERVICE_EVENT,
        ResType::RES_TYPE_APP_FRAME_DROP,
        ResType::RES_TYPE_GC_EVENT,
        ResType::RES_TYPE_REPORT_DISTRIBUTE_TID,
        ResType::RES_TYPE_AXIS_EVENT,
        ResType::RES_TYPE_CROWN_ROTATION_STATUS,
        ResType::RES_TYPE_RED_ENVELOPE,
        ResType::RES_TYPE_WEB_DRAG_RESIZE,
        ResType::RES_TYPE_RECV_ABC_LOAD_COMPLETED,
        ResType::RES_TYPE_SET_BACKGROUND_PROCESS_PRIORITY,
        ResType::RES_TYPE_SHORT_TERM_LOAD,
        ResType::RES_TYPE_CHECK_APP_IS_IN_SCHEDULE_LIST,
        ResType::RES_TYPE_PAGE_TRANSITION,
        ResType::RES_TYPE_VOICE_RECOGNIZE_WAKE,
        ResType::RES_TYPE_WEB_SLIDE_SCROLL,
        ResType::RES_TYPE_ABILITY_OR_PAGE_SWITCH,
        ResType::RES_TYPE_OVERLAY_EVENT,
        ResType::RES_TYPE_BACKGROUND_STATUS,
        ResType::RES_TYPE_BACKPRESSED_EVENT,
        ResType::SYNC_RES_TYPE_GET_SMART_GC_SCENE_INFO,
        ResType::RES_TYPE_WEB_SUBWIN_TASK,
        ResType::SYNC_RES_TYPE_APP_IS_IN_CLICK_REPORT_EXT_LIST,
    };
    static const std::unordered_set<uint32_t> FG_THIRDPART_RES = {
        ResType::RES_TYPE_CLICK_RECOGNIZE,
        ResType::RES_TYPE_KEY_EVENT,
        ResType::RES_TYPE_SLIDE_RECOGNIZE,
        ResType::RES_TYPE_LOAD_URL,
        ResType::RES_TYPE_MOUSEWHEEL,
        ResType::RES_TYPE_LONG_FRAME,
        ResType::RES_TYPE_WEB_DRAG_RESIZE,
        ResType::RES_TYPE_APP_FRAME_DROP,
        ResType::RES_TYPE_AXIS_EVENT,
        ResType::RES_TYPE_WEB_SLIDE_SCROLL,
        ResType::RES_TYPE_OVERLAY_EVENT,
    };
    static const std::unordered_set<uint32_t> SA_RES = {
        ResType::SYNC_RES_TYPE_THAW_ONE_APP,
        ResType::SYNC_RES_TYPE_GET_ALL_SUSPEND_STATE,
        ResType::SYNC_RES_TYPE_GET_THERMAL_DATA,
        ResType::RES_TYPE_BLUETOOTH_A2DP_CONNECT_STATE_CHANGE,
        ResType::RES_TYPE_NETWORK_LATENCY_REQUEST,
        ResType::RES_TYPE_THREAD_QOS_CHANGE,
        ResType::RES_TYPE_MOVE_WINDOW,
        ResType::RES_TYPE_ANCO_CUST,
        ResType::RES_TYPE_SOCPERF_CUST_ACTION,
        ResType::RES_TYPE_SOCPERF_CUST_EVENT_BEGIN,
        ResType::RES_TYPE_SOCPERF_CUST_EVENT_END,
        ResType::RES_TYPE_SA_CONTROL_APP_EVENT,
        ResType::RES_TYPE_REPORT_DISTRIBUTE_COMPONENT_CHANGE,
        ResType::RES_TYPE_THERMAL_SCENARIO_REPORT,
        ResType::RES_TYPE_AUDIO_RENDERER_SILENT_PLAYBACK,
        ResType::RES_TYPE_CLOUD_CONFIG_UPDATE,
        ResType::RES_TYPE_BT_SERVICE_EVENT,
        ResType::RES_TYPE_REPORT_BOKER_GATT_CONNECT,
        ResType::SYNC_RES_TYPE_REQUEST_MUTEX_STATUS,
        ResType::SYNC_RES_TYPE_GET_NWEB_PRELOAD_SET,
        ResType::RES_TYPE_AUDIO_RENDERER_STANDBY,
        ResType::RES_TYPE_INTENT_CTRL_APP,
        ResType::RES_TYPE_DISPLAY_MULTI_SCREEN,
        ResType::RES_TYPE_SHORT_TERM_LOAD,
        ResType::SYNC_RES_TYPE_GET_SUSPEND_STATE_BY_UID,
        ResType::SYNC_RES_TYPE_GET_SUSPEND_STATE_BY_PID,
#ifdef RESSCHED_RESOURCESCHEDULE_FILE_COPY_SOC_PERF_ENABLE
        ResType::RES_TYPE_FILE_COPY_STATUS,
#endif
        ResType::RES_TYPE_DYNAMICALLY_SET_SUSPEND_EXEMPT,
        ResType::RES_TYPE_CAMERA_PRELAUNCH,
        ResType::RES_TYPE_CAMERA_RESET_PRIORITY,
        ResType::RES_TYPE_DEVICE_MODE_STATUS,
        ResType::RES_TYPE_PARAM_UPDATE_EVENT,
        ResType::RES_TYPE_TDP_TURBO,
        ResType::RES_TYPE_BACKGROUND_STATUS,
        ResType::SYNC_RES_TYPE_QUERY_TOPN_BUNDLE_USAGE_INFO,
        ResType::RES_TYPE_CAMERA_STATUS_CHANGED,
        ResType::RES_TYPE_NAP_MODE,
        ResType::RES_TYPE_CODEC_ENCODE_STATUS_CHANGED,
        ResType::RES_TYPE_CAMERA_LENS_STATUS_CHANGED,
        ResType::RES_TYPE_LOAD_KILL_REASON_CONFIG,
        ResType::RES_TYPE_SA_PULL_APP_IDENTIFIER,
        ResType::RES_TYPE_START_INPUT_METHOD_PROCESS,
        ResType::RES_TYPE_START_UIEXTENSION_PROC,
        ResType::RES_TYPE_SCHED_MODE_CHANGE,
        ResType::RES_TYPE_HARDWARE_DECODING_RESOURCES,
        ResType::RES_TYPE_FIRST_FRAME_DRAWN,
    };
    static const std::unordered_map<uint32_t, std::unordered_set<int32_t>> ALLOW_SOME_UID_REPORT_RES = {
        { ResType::RES_TYPE_MODEM_PA_HIGH_POWER_ABNORMAL, { 1201 } },
        { ResType::RES_TYPE_APP_HIGH_POWER_CONSUMPTION, { 1201 } },
        { ResType::RES_TYPE_APP_ABILITY_START, { 5523 } },
        { ResType::RES_TYPE_PRELOAD_APPLICATION, { 5523 } },
        { ResType::RES_TYPE_APP_ASSOCIATED_START, { 5523 } },
        { ResType::RES_TYPE_REPORT_SCREEN_CAPTURE, { 1013 } },
        { ResType::RES_TYPE_LOCATION_STATUS_CHANGE, { 1021 } },
        { ResType::RES_TYPE_SYSTEM_CPU_LOAD, { 1201 } },
        { ResType::RES_TYPE_FRAME_RATE_REPORT, { 1003 } },
        { ResType::RES_TYPE_FORM_STATE_CHANGE_EVENT, { 5523 } },
        { ResType::RES_TYPE_FORM_RENDER_EVENT, { 5523 } },
        { ResType::RES_TYPE_SEND_FRAME_EVENT, { 1003 } },
        { ResType::RES_TYPE_CLICK_RECOGNIZE, { 6696 } },
        { ResType::RES_TYPE_REPORT_GAME_SCHED, { 7800 } },
        { ResType::RES_TYPE_GAME_INFO_NOTIFY, { 7011 } },
        { ResType::SYNC_RES_TYPE_SHOULD_FORCE_KILL_PROCESS, { 5523 } },
        { ResType::RES_TYPE_GET_GAME_SCENE_INFO, { 7800 } },
        { ResType::RES_TYPE_APP_GAME_BOOST_EVENT, { 7800 } },
        { ResType::RES_TYPE_FRAME_RATE_REPORT_FROM_RS, { 1003 } },
        { ResType::RES_TRPE_GAME_SUSPEND_MODE, { 7800 } },
        { ResType::RES_TYPE_ADJUST_PROTECTLRU_RECLAIM_RATIO, { 1111 } },
        { ResType::RES_TYPE_STANDBY_FREEZE_FAILED, { 1201 } },
        { ResType::RES_TYPE_IME_QOS_CHANGE, { 3820 } },
        { ResType::RES_TYPE_HARDWARE_DECODING_RESOURCES, { 1013 }},
    };
    enum SYSTEM_LOAD_LEVEL_DEBUG_DUMP_SIGNAL : int32_t {
        DEBUG_LEVEL_MINIMUM = 0,
        DEBUG_LEVEL_MAXIMUM = 7,
    };
}

bool ResSchedService::IsThirdPartType(const uint32_t type)
{
    if (allowAllAppReportRes_.find(type) == allowAllAppReportRes_.end()) {
        RESSCHED_LOGD("resType:%{public}d not hap app report", type);
        return false;
     }
    if (allowFgAppReportRes_.find(type) != allowFgAppReportRes_.end() &&
        !ResSchedMgr::GetInstance().IsForegroundApp(IPCSkeleton::GetCallingPid())) {
        RESSCHED_LOGD("not foreground app");
        return false;
    }
    AccessToken::AccessTokenID tokenId = OHOS::IPCSkeleton::GetCallingTokenID();
    auto tokenType = Security::AccessToken::AccessTokenKit::GetTokenTypeFlag(tokenId);
    if (tokenType != Security::AccessToken::ATokenTypeEnum::TOKEN_HAP) {
        RESSCHED_LOGD("not hap app");
        return false;
    }
    return true;
}

bool ResSchedService::IsSBDResType(uint32_t type)
{
    if (allowSCBReportRes_.find(type) == allowSCBReportRes_.end()) {
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
    RESSCHED_LOGD("%{public}s is not sceneboard bundle name", callingTokenInfo.bundleName.c_str());
    return false;
}

bool ResSchedService::IsHasPermission(const uint32_t type, int32_t uid)
{
    const auto& item = allowSomeSAReportRes_.find(type);
    if (item != allowSomeSAReportRes_.end()) {
        if (item->second.find(uid) == item->second.end()) {
            RESSCHED_LOGE("resType:%{public}d not allow uid:%{public}d report", type, uid);
             return false;
        }
    } else if (allowAllSAReportRes_.find(type) == allowAllSAReportRes_.end()) {
        RESSCHED_LOGE("resType:%{public}d not sa report", type);
        return false;
    }
    AccessToken::AccessTokenID tokenId = OHOS::IPCSkeleton::GetCallingTokenID();
    auto tokenType = Security::AccessToken::AccessTokenKit::GetTokenTypeFlag(tokenId);
    if (tokenType != Security::AccessToken::ATokenTypeEnum::TOKEN_NATIVE) {
        RESSCHED_LOGD("not native sa");
        return false;
    }
    int32_t hasPermission = -1;
    std::lock_guard<std::mutex> lock(permissionCacheMutex_);
    if (permissionCache_.get(tokenId, hasPermission)) {
        return hasPermission == 0;
    }
    hasPermission = AccessToken::AccessTokenKit::VerifyAccessToken(tokenId, NEEDED_PERMISSION);
    permissionCache_.put(tokenId, hasPermission);
    if (hasPermission != 0) {
        RESSCHED_LOGE("not have permission");
        return false;
    }
    return true;
}

nlohmann::json ResSchedService::StringToJsonObj(const std::string& str)
{
    nlohmann::json jsonObj = nlohmann::json::object();
    if (str.empty()) {
        return jsonObj;
    }
    nlohmann::json jsonTmp = nlohmann::json::parse(str, nullptr, false);
    if (jsonTmp.is_discarded()) {
        RESSCHED_LOGE("%{public}s: parse fail, str=%{public}s.", __func__, str.c_str());
        return jsonObj;
    }
    if (!jsonTmp.is_object()) {
        RESSCHED_LOGD("%{public}s: str=%{public}s is not jsonObj.", __func__, str.c_str());
        return jsonObj;
    }
    return jsonTmp;
}

#ifdef RESOURCE_SCHEDULE_SERVICE_WITH_EXT_RES_ENABLE
int32_t ResSchedService::GetExtTypeByResPayload(const std::string& payload)
{
    auto jsonPayload = StringToJsonObj(payload);
    if (!jsonPayload.contains(EXT_RES_KEY) || !jsonPayload[EXT_RES_KEY].is_string()) {
        return DEFAULT_VALUE;
    }
    int type = DEFAULT_VALUE;
    if (StrToInt(jsonPayload[EXT_RES_KEY], type)) {
        return type;
    } else {
        return DEFAULT_VALUE;
    }
}
#endif

ErrCode ResSchedService::ReportData(uint32_t resType, int64_t value, const std::string& payload)
{
    ResSchedHiTraceChain traceChain(__func__);
    int32_t checkResult = RemoteRequestCheck();
    if (checkResult != ERR_OK) {
        RESSCHED_LOGD("check remote request fail.");
        return checkResult;
    }
#ifdef RESOURCE_SCHEDULE_SERVICE_WITH_EXT_RES_ENABLE
    if (resType == ResType::RES_TYPE_KEY_PERF_SCENE) {
        int32_t extType = GetExtTypeByResPayload(payload);
        if (extType != DEFAULT_VALUE) {
            resType = (uint32_t)extType;
        }
    }
#endif
    int32_t callingUid = IPCSkeleton::GetCallingUid();
    int32_t ret = CheckReportDataParcel(resType, value, payload, callingUid);
    if (ret != ERR_OK) {
        RESSCHED_LOGE("%{public}s: check report data parcel fail ret=%{public}d, type=%{public}u.",
            __func__, ret, resType);
        return ret;
    }

    int32_t clientPid = IPCSkeleton::GetCallingPid();
    RESSCHED_LOGD("ResSchedService receive data from ipc resType: %{public}u, value: %{public}lld, pid: %{public}d",
                  resType, (long long)value, clientPid);
    nlohmann::json reportDataPayload = StringToJsonObj(payload);
    reportDataPayload["callingUid"] = std::to_string(callingUid);
    reportDataPayload["clientPid"] = std::to_string(clientPid);
    ResSchedMgr::GetInstance().ReportData(resType, value, reportDataPayload);
    ResSchedIpcThread::GetInstance().SetQos(clientPid);
    return ERR_OK;
}

ErrCode ResSchedService::ReportSyncEvent(const uint32_t resType, const int64_t value, const std::string& payload,
    std::string& reply, int32_t& resultValue)
{
    ResSchedHiTraceChain traceChain(__func__);
    int32_t checkResult = RemoteRequestCheck();
    if (checkResult != ERR_OK) {
        RESSCHED_LOGD("check remote request fail.");
        resultValue = checkResult;
        return ERR_OK;
    }
    int32_t callingUid = IPCSkeleton::GetCallingUid();
    int32_t ret = CheckReportDataParcel(resType, value, payload, callingUid);
    if (ret != ERR_OK) {
        RESSCHED_LOGE("%{public}s: check report data parcel fail ret=%{public}d, type=%{public}u.",
            __func__, ret, resType);
        resultValue = ret;
        return ERR_OK;
    }
    nlohmann::json payloadJsonValue;
    nlohmann::json replyValue;
    payloadJsonValue = StringToJsonObj(payload);
    int32_t clientPid = IPCSkeleton::GetCallingPid();
    payloadJsonValue["clientPid"] = std::to_string(clientPid);
    payloadJsonValue["callingUid"] = std::to_string(callingUid);
    resultValue = PluginMgr::GetInstance().DeliverResource(
        std::make_shared<ResData>(resType, value, payloadJsonValue, replyValue));
    reply = replyValue.dump(-1, ' ', false, nlohmann::detail::error_handler_t::replace);
    ResSchedIpcThread::GetInstance().SetQos(clientPid);
    return ERR_OK;
}

ErrCode ResSchedService::KillProcess(const std::string& payload, int32_t& resultValue)
{
    uint32_t accessToken = IPCSkeleton::GetCallingTokenID();
    int32_t uid = IPCSkeleton::GetCallingUid();
    Security::AccessToken::ATokenTypeEnum tokenTypeFlag =
        Security::AccessToken::AccessTokenKit::GetTokenTypeFlag(accessToken);
    if ((uid != MEMMGR_UID && uid != SAMGR_UID && uid != HIVIEW_UID && uid != GRAPHIC_UID)
        || tokenTypeFlag != Security::AccessToken::ATokenTypeEnum::TOKEN_NATIVE) {
        RESSCHED_LOGE("no permission, kill process fail");
        resultValue = RES_SCHED_KILL_PROCESS_FAIL;
        return ERR_OK;
    }

    if (payload.size() > PAYLOAD_MAX_SIZE) {
        RESSCHED_LOGE("The payload is too long. DoS.");
        resultValue = RES_SCHED_KILL_PROCESS_FAIL;
        return ERR_OK;
    }

    resultValue = ResSchedMgr::GetInstance().KillProcessByClient(StringToJsonObj(payload));
    return ERR_OK;
}

ErrCode ResSchedService::RegisterSystemloadNotifier(const sptr<IRemoteObject>& notifier)
{
    if (notifier == nullptr) {
        RESSCHED_LOGE("ResSchedService notifier is nullptr.");
        return RES_SCHED_DATA_ERROR;
    }

    NotifierMgr::GetInstance().RegisterNotifier(IPCSkeleton::GetCallingPid(), notifier);
    return ERR_OK;
}

ErrCode ResSchedService::UnRegisterSystemloadNotifier()
{
    NotifierMgr::GetInstance().UnRegisterNotifier(IPCSkeleton::GetCallingPid());
    return ERR_OK;
}

ErrCode ResSchedService::RegisterEventListener(const sptr<IRemoteObject>& eventListener, uint32_t eventType,
    uint32_t listenerGroup)
{
    EventListenerMgr::GetInstance().RegisterEventListener(IPCSkeleton::GetCallingPid(), eventListener, eventType,
        listenerGroup);
    return ERR_OK;
}

ErrCode ResSchedService::UnRegisterEventListener(uint32_t eventType, uint32_t listenerGroup)
{
    EventListenerMgr::GetInstance().UnRegisterEventListener(IPCSkeleton::GetCallingPid(), eventType, listenerGroup);
    return ERR_OK;
}

ErrCode ResSchedService::GetSystemloadLevel(int32_t& resultValue)
{
    if (systemLoadLevelDebugEnable_) {
        resultValue = debugSystemLoadLevel_;
    } else {
        resultValue = NotifierMgr::GetInstance().GetSystemloadLevel();
    }
    return ERR_OK;
}

ErrCode ResSchedService::GetResTypeList(std::set<uint32_t>& resTypeList)
{
    PluginMgr::GetInstance().GetResTypeList(resTypeList);
    return ERR_OK;
}

void ResSchedService::OnDeviceLevelChanged(int32_t type, int32_t level, bool debugReport)
{
    if (type == static_cast<int32_t>(ResType::DeviceStatus::SYSTEMLOAD_LEVEL)) {
        if (!debugReport) {
            actualSystemLoadLevel_ = level;
        }
        if (systemLoadLevelDebugEnable_) {
            level = debugSystemLoadLevel_;
        }
    }
    NotifierMgr::GetInstance().OnDeviceLevelChanged(type, level);
}

ErrCode ResSchedService::IsAllowedAppPreload(const std::string& bundleName, int32_t preloadMode, bool& resultValue)
{
    AccessToken::AccessTokenID tokenId = IPCSkeleton::GetCallingTokenID();
    int32_t uid = IPCSkeleton::GetCallingUid();
    AccessToken::ATokenTypeEnum tokenTypeFlag = AccessToken::AccessTokenKit::GetTokenTypeFlag(tokenId);
    if (uid != FOUNDATION_UID || tokenTypeFlag != AccessToken::ATokenTypeEnum::TOKEN_NATIVE) {
        RESSCHED_LOGE("Invalid calling token");
        resultValue = false;
        return ERR_OK;
    }

    LoadAppPreloadPlugin();
    if (!appPreloadFunc_) {
        RESSCHED_LOGE("%{public}s, no allow AppPreload !", __func__);
        resultValue = false;
        return ERR_OK;
    }
    resultValue = appPreloadFunc_(bundleName, preloadMode);
    return ERR_OK;
}

void ResSchedService::LoadAppPreloadPlugin()
{
    std::shared_ptr<PluginLib> libInfoPtr = PluginMgr::GetInstance().GetPluginLib(APP_PRELOAD_PLIGIN_NAME);
    if (libInfoPtr == nullptr) {
        RESSCHED_LOGE("ResSchedService::LoadAppPreloadPlugin libInfoPtr nullptr");
        isLoadAppPreloadPlugin_ = false;
        return;
    }

    if (isLoadAppPreloadPlugin_) {
        return;
    }

    appPreloadFunc_ = reinterpret_cast<OnIsAllowedAppPreloadFunc>(dlsym(libInfoPtr->handle.get(),
        "OnIsAllowedAppPreload"));
    isLoadAppPreloadPlugin_ = true;
}

ErrCode ResSchedService::IsAllowedLinkJump(bool isAllowedLinkJump, int32_t& resultValue)
{
    AccessToken::AccessTokenID tokenId = IPCSkeleton::GetCallingTokenID();
    AccessToken::ATokenTypeEnum tokenTypeFlag = AccessToken::AccessTokenKit::GetTokenTypeFlag(tokenId);
    if (tokenTypeFlag != AccessToken::ATokenTypeEnum::TOKEN_HAP) {
        RESSCHED_LOGE("Invalid calling token");
        resultValue = RES_SCHED_ACCESS_TOKEN_FAIL;
        return ERR_OK;
    }

    AccessToken::HapTokenInfo callingTokenInfo;
    AccessToken::AccessTokenKit::GetHapTokenInfo(tokenId, callingTokenInfo);
    std::string callingBundleName = callingTokenInfo.bundleName;
    if (!PluginMgr::GetInstance().GetLinkJumpOptConfig(callingBundleName, isAllowedLinkJump)) {
        RESSCHED_LOGE("Invalid callingBundleName");
        resultValue = ERR_RES_SCHED_INVALID_PARAM;
        return ERR_OK;
    }
    resultValue = ERR_OK;
    return ERR_OK;
}

bool ResSchedService::CheckDumpPermission()
{
    Security::AccessToken::AccessTokenID tokenId = IPCSkeleton::GetCallingTokenID();
    int32_t ret = Security::AccessToken::AccessTokenKit::VerifyAccessToken(tokenId, "ohos.permission.DUMP");
    if (ret != Security::AccessToken::PermissionState::PERMISSION_GRANTED) {
        RESSCHED_LOGE("CheckPermission failed");
        return false;
    }
    return true;
}

bool ResSchedService::CheckENGMode()
{
    if (ENG_MODE == 0) {
        RESSCHED_LOGE("Not eng mode");
        return false;
    }
    return true;
}

int32_t ResSchedService::Dump(int32_t fd, const std::vector<std::u16string>& args)
{
    std::vector<std::string> argsInStr;
    std::transform(args.begin(), args.end(), std::back_inserter(argsInStr),
        [](const std::u16string &arg) {
        std::string ret = Str16ToStr8(arg);
        RESSCHED_LOGI("%{public}s arg: %{public}s.", __func__, ret.c_str());
        return ret;
    });
    if (DumpPreloadSwitch(fd, argsInStr) == ERR_OK) {
        return ERR_OK;
    }
#ifdef SET_SYSTEM_LOAD_LEVEL_2D_ENABLE
    if (!CheckDumpPermission() || (!CheckENGMode() && argsInStr.size() > 0 &&
        (argsInStr[DUMP_OPTION] != "setSystemLoadLevel" &&
            argsInStr[DUMP_OPTION] != "getSystemloadInfo" &&
            argsInStr[DUMP_OPTION] != "-h"))) {
        return ERR_RES_SCHED_PERMISSION_DENIED;
    }
#else
    if (!CheckDumpPermission() || !CheckENGMode()) {
        return ERR_RES_SCHED_PERMISSION_DENIED;
    }
#endif // SET_SYSTEM_LOAD_LEVEL_2D_ENABLE
    RESSCHED_LOGI("%{public}s Dump service.", __func__);
    std::string result;
    if (argsInStr.size() == 0) {
        // hidumper -s said '-h'
#ifdef SET_SYSTEM_LOAD_LEVEL_2D_ENABLE
        DumpUsage2D(result);
#else
        DumpUsage(result);
#endif //SET_SYSTEM_LOAD_LEVEL_2D_ENABLE
    } else if (argsInStr.size() == DUMP_OPTION + 1) {
        // hidumper -s said '-h' or hidumper -s said '-a'
        DumpExt(argsInStr, result);
    } else if (argsInStr.size() >= DUMP_PARAM_INDEX + 1) {
        if (argsInStr[DUMP_OPTION] == "-p") {
            std::vector<std::string> argsInStrToPlugin;
            argsInStrToPlugin.assign(argsInStr.begin() + DUMP_PARAM_INDEX + 1, argsInStr.end());
            PluginMgr::GetInstance().DumpOnePlugin(result, argsInStr[DUMP_PARAM_INDEX], argsInStrToPlugin);
        } else if (argsInStr[DUMP_OPTION] == "sendDebugToExecutor") {
            DumpExecutorDebugCommand(argsInStr, result);
        } else if (argsInStr[DUMP_OPTION] == "setSystemLoadLevel") {
            DumpSetSystemLoad(argsInStr, result);
        }
    }

    if (!SaveStringToFd(fd, result)) {
        RESSCHED_LOGE("%{public}s save to fd failed.", __func__);
    }
    return ERR_OK;
}

int32_t ResSchedService::DumpPreloadSwitch(int32_t fd, std::vector<std::string>& argsInStr)
{
    if (!CheckDumpPermission()) {
        return ERR_RES_SCHED_PERMISSION_DENIED;
    }
    if (argsInStr.size() < 0) {
        return ERR_RES_SCHED_PERMISSION_DENIED;
    }
    std::string result;
    if (argsInStr.size() == 0) {
        if (CheckENGMode()) {
            return ERR_RES_SCHED_PERMISSION_DENIED;
        }
        // hidumper -s 1901
        DumpUserUsage(result);
        if (!SaveStringToFd(fd, result)) {
            RESSCHED_LOGE("PreloadSwitch %{public}s save to fd failed", __func__);
        }
        return ERR_OK;
    }

    if (argsInStr[DUMP_OPTION] == "setPreloadSwitch" || argsInStr[DUMP_OPTION] == "getPreloadSwitch") {
        // hidumper -s -a "setPreloadSwitch (value)"
        // hidumper -s -a "getPreloadSwitch"
        std::string result;
        PluginMgr::GetInstance().DumpOnePlugin(result, "libapp_preload_plugin.z.so", argsInStr);
        if (!SaveStringToFd(fd, result)) {
            RESSCHED_LOGE("PreloadSwitch %{public}s save to fd failed", __func__);
        }
        return ERR_OK;
    }
    return ERR_RES_SCHED_PERMISSION_DENIED;
}

void ResSchedService::DumpExt(const std::vector<std::string>& argsInStr, std::string &result)
{
    if (argsInStr[DUMP_OPTION] == "-h") {
#ifdef SET_SYSTEM_LOAD_LEVEL_2D_ENABLE
        DumpUsage2D(result);
#else
        DumpUsage(result);
#endif //SET_SYSTEM_LOAD_LEVEL_2D_ENABLE
    } else if (argsInStr[DUMP_OPTION] == "-a") {
        DumpAllInfo(result);
    } else if (argsInStr[DUMP_OPTION] == "-p") {
        PluginMgr::GetInstance().DumpAllPlugin(result);
    } else if (argsInStr[DUMP_OPTION] == "getSystemloadInfo") {
        DumpSystemLoadInfo(result);
    } else if (argsInStr[DUMP_OPTION] == "sendDebugToExecutor") {
        DumpExecutorDebugCommand(argsInStr, result);
    } else if (argsInStr[DUMP_OPTION] == "PluginConfig") {
        DumpAllPluginConfig(result);
    } else {
        result.append("Error params.");
    }
}

void ResSchedService::DumpSystemLoadInfo(std::string &result)
{
    result.append("systemloadLevel:")
        .append(ToString(NotifierMgr::GetInstance().GetSystemloadLevel()))
        .append("\n");
    auto notifierInfo = NotifierMgr::GetInstance().DumpRegisterInfo();
    std::string native("natives:");
    std::string hap("apps:");
    for (auto& info : notifierInfo) {
        std::string str = ToString(info.first).append(" ");
        if (info.second) {
            hap.append(str);
        } else {
            native.append(str);
        }
    }
    hap.append("\n");
    native.append("\n");
    result.append(native).append(hap);
}

void ResSchedService::DumpUsage(std::string &result)
{
    result.append("usage: resource schedule service dump [<options>]\n")
        .append("    -h: show the help.\n")
        .append("    -a: show all info.\n")
        .append("    -p: show the all plugin info.\n")
        .append("    -p (plugin name): show one plugin info.\n")
        .append("    setSystemLoadLevel (LevelNum/reset): set system load level. LevelNum range: 0-7\n")
        .append("    getSystemloadInfo: get system load info.\n");
    PluginMgr::GetInstance().DumpHelpFromPlugin(result);
}

void ResSchedService::DumpUserUsage(std::string &result)
{
    result.append("usage: resource schedule service dump [<options>]\n")
        .append("setPreloadSwitch      |setPreloadSwitch [value], 1:open, 0:close.\n")
        .append("getPreloadSwitch      |getPreloadSwitch.\n")
#ifdef SET_SYSTEM_LOAD_LEVEL_2D_ENABLE
        .append("    setSystemLoadLevel (LevelNum/reset): set system load level. LevelNum range: 0-7\n")
        .append("    getSystemloadInfo: get system load info.\n")
#endif //SET_SYSTEM_LOAD_LEVEL_2D_ENABLE
        ;
}

#ifdef SET_SYSTEM_LOAD_LEVEL_2D_ENABLE
void ResSchedService::DumpUsage2D(std::string &result)
{
    result.append("    setSystemLoadLevel (LevelNum/reset): set system load level. LevelNum range: 0-7\n")
        .append("    getSystemloadInfo: get system load info.\n");
}
#endif //SET_SYSTEM_LOAD_LEVEL_2D_ENABLE

void ResSchedService::DumpAllInfo(std::string &result)
{
    result.append("================Resource Schedule Service Infos================\n");
    PluginMgr::GetInstance().DumpAllPlugin(result);
}

void ResSchedService::DumpExecutorDebugCommand(const std::vector<std::string>& args, std::string& result)
{
    // hidumper -s said 'sendDebugToExecutor [isSync times]' isSync - 0/1(default 0), times - 1~...(default 1)
    result.append("Send debug command to resource_schedule_executor.\n");
    bool isSync = true;
    int times = 1;
    if (args.size() > DUMP_PARAM_INDEX + 1) {
        int arg = atoi(args[DUMP_PARAM_INDEX + 1].c_str());
        times = arg > 0 ? arg : times;
    }
    if (args.size() > DUMP_PARAM_INDEX) {
        isSync = atoi(args[DUMP_PARAM_INDEX].c_str()) == 0;
    }
    uint32_t internal = 200;
    for (int i = 0; i < times; i++) {
        ResSchedExeClient::GetInstance().SendDebugCommand(isSync);
        usleep(internal);
    }
}

void ResSchedService::DumpSetSystemLoad(const std::vector<std::string>& args, std::string& result)
{
    //hidumper -s 1901 -a "setSystemLoadLevel (LevelNum/reset)"
    if (args.size() == DUMP_PARAM_INDEX + 1) {
        const std::string& switchStr = args[DUMP_PARAM_INDEX];
        if (switchStr == "reset") {
            systemLoadLevelDebugEnable_ = false;
            debugSystemLoadLevel_ = 0;
            result.append("Set SystemLoad Close\n");
            OnDeviceLevelChanged(ResType::DeviceStatus::SYSTEMLOAD_LEVEL, actualSystemLoadLevel_, false);
            return;
        }
        int32_t switchInfo;
        if (!StrToInt(switchStr, switchInfo)) {
            result.append("Err setSystemLoadLevel param. Please insert 0-7 to start debug, \"reset\" to close debug");
            return;
        }
        if (switchInfo <= SYSTEM_LOAD_LEVEL_DEBUG_DUMP_SIGNAL::DEBUG_LEVEL_MAXIMUM &&
            switchInfo >= SYSTEM_LOAD_LEVEL_DEBUG_DUMP_SIGNAL::DEBUG_LEVEL_MINIMUM) {
            systemLoadLevelDebugEnable_ = true;
            debugSystemLoadLevel_ = switchInfo;
            result.append("setSystemLoadLevel Debug On with Level: ").append(ToString(switchInfo)).append("\n");
            OnDeviceLevelChanged(ResType::DeviceStatus::SYSTEMLOAD_LEVEL, debugSystemLoadLevel_, true);
        } else {
            result.append("Err setSystemLoadLevel param. Please insert 0-7 to start debug, \"reset\" to close debug");
        }
        return;
    }
    result.append("Err setSystemLoadLevel param num. Please insert one param after \"setSystemLoadLevel\"");
}

void ResSchedService::DumpAllPluginConfig(std::string &result)
{
    result.append("================Resource Schedule Plugin Config================\n");
    PluginMgr::GetInstance().DumpAllPluginConfig(result);
}

int32_t ResSchedService::CheckReportDataParcel(const uint32_t& type, const int64_t& value,
    const std::string& payload, int32_t uid)
{
    if (!IsSBDResType(type) && !IsThirdPartType(type) && !IsHasPermission(type, uid)) {
        RESSCHED_LOGD("type:%{public}u, no permission", type);
        return ERR_RES_SCHED_PERMISSION_DENIED;
    }

    if (payload.size() > PAYLOAD_MAX_SIZE) {
        RESSCHED_LOGE("too long payload.size:%{public}u", (uint32_t)payload.size());
        return ERR_RES_SCHED_PARCEL_ERROR;
    }
    return ERR_OK;
}

void ResSchedService::PrintLimitLog(int32_t uid)
{
    if (isPrintLimitLog_.load()) {
        isPrintLimitLog_.store(false);
        RESSCHED_LOGI("request limit, allRequestCount_:%{public}d, cur report uid:%{public}d",
            allRequestCount_.load(), uid);
    }
}

void ResSchedService::ReportBigData()
{
    if (!isReportBigData_.load()) {
        return;
    }
    if (ResCommonUtil::GetNowMillTime(true) < nextReportBigDataTime_) {
        return;
    }
    HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::RSS, "SERVICE_REQUEST_LIMIT",
                    HiviewDFX::HiSysEvent::EventType::FAULT, "REQUEST_LIMIT_COUNT", bigDataReportCount_.load());
    isReportBigData_.store(false);
    bigDataReportCount_.store(0);
}

void ResSchedService::InreaseBigDataCount()
{
    if (!isReportBigData_.load()) {
        isReportBigData_.store(true);
        nextReportBigDataTime_ = ResCommonUtil::GetNowMillTime(true) + FOUR_HOUR_TIME;
    }
    bigDataReportCount_.fetch_add(1, std::memory_order_relaxed);
}

bool ResSchedService::IsLimitRequest(int32_t uid)
{
    std::lock_guard<std::mutex> lock(mutex_);
    int64_t nowTime = ResCommonUtil::GetNowMillTime(true);
    CheckAndUpdateLimitData(nowTime);
    if (allRequestCount_.load() >= ALL_UID_REQUEST_LIMIT_COUNT) {
        RESSCHED_LOGD("all uid request is limit, %{public}d request fail", uid);
        return true;
    }
    auto iter = appRequestCountMap_.find(uid);
    if (iter == appRequestCountMap_.end()) {
        appRequestCountMap_[uid] = 1;
        allRequestCount_.fetch_add(1, std::memory_order_relaxed);
        return false;
    }
    if (appRequestCountMap_[uid] >= SINGLE_UID_REQUEST_LIMIT_COUNT) {
        RESSCHED_LOGD("uid:%{public}d request is limit, request fail", uid);
        return true;
    }
    appRequestCountMap_[uid] ++;
    allRequestCount_.fetch_add(1, std::memory_order_relaxed);
    return false;
}

void ResSchedService::CheckAndUpdateLimitData(int64_t nowTime)
{
    if (nowTime - nextCheckTime_.load() >= 0) {
        nextCheckTime_.store(nowTime + LIMIT_REQUEST_TIME);
        appRequestCountMap_.clear();
        allRequestCount_.store(0);
        isPrintLimitLog_.store(true);
    }
}

int32_t ResSchedService::RemoteRequestCheck()
{
    ReportBigData();
    auto uid = IPCSkeleton::GetCallingUid();
    if (IsLimitRequest(uid)) {
        RESSCHED_LOGD("%{public}d is limit request, cur request fail", uid);
        InreaseBigDataCount();
        PrintLimitLog(uid);
        return RES_SCHED_REQUEST_FAIL;
    }
    return ERR_OK;
}

void ResSchedService::InitAllowIpcReportRes()
{
    AddSCBRes(SCB_RES);
    AddAllSARes(SA_RES);
    AddSomeSARes(ALLOW_SOME_UID_REPORT_RES);
    AddAllAppRes(THIRDPART_RES);
    AddFgAppRes(FG_THIRDPART_RES);
    AddSCBRes(ResSchedMgr::GetInstance().GetAllowSCBReportResExt());
    AddAllSARes(ResSchedMgr::GetInstance().GetAllowAllSAReportResExt());
    AddSomeSARes(ResSchedMgr::GetInstance().GetAllowSomeSAReportResExt());
    AddAllAppRes(ResSchedMgr::GetInstance().GetAllowAllAppReportResExt());
    AddFgAppRes(ResSchedMgr::GetInstance().GetAllowFgAppReportResExt());
}

template <typename T>
inline void AddAll(T& to, const T& from)
{
    if (!from.empty()) {
        to.insert(from.begin(), from.end());
    }
}

void ResSchedService::AddSCBRes(const std::unordered_set<uint32_t>& allowSCBReportRes)
{
    AddAll(allowSCBReportRes_, allowSCBReportRes);
}

void ResSchedService::AddAllSARes(const std::unordered_set<uint32_t>& allowAllSAReportRes)
{
    AddAll(allowAllSAReportRes_, allowAllSAReportRes);
}

void ResSchedService::AddSomeSARes(const std::unordered_map<uint32_t, std::unordered_set<int32_t>>&
    allowSomeSAReportRes)
{
    AddAll(allowSomeSAReportRes_, allowSomeSAReportRes);
}
void ResSchedService::AddAllAppRes(const std::unordered_set<uint32_t>& allowAllAppReportRes)
{
    AddAll(allowAllAppReportRes_, allowAllAppReportRes);
}
void ResSchedService::AddFgAppRes(const std::unordered_set<uint32_t>& allowFgAppReportRes)
{
    AddAll(allowFgAppReportRes_, allowFgAppReportRes);
}
} // namespace ResourceSchedule
} // namespace OHOS

