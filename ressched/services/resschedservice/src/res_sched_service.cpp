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

#include <cstdint>
#include <dlfcn.h>
#include "res_sched_service.h"
#include <file_ex.h>
#include <parameters.h>
#include <string_ex.h>
#include "accesstoken_kit.h"
#include "ipc_skeleton.h"
#include "notifier_mgr.h"
#include "plugin_mgr.h"
#include "res_sched_errors.h"
#include "res_sched_exe_client.h"
#include "res_sched_log.h"
#include "res_sched_mgr.h"
#include "tokenid_kit.h"
#include "event_listener_mgr.h"
#include "hisysevent.h"
#include "res_common_util.h"

namespace OHOS {
namespace ResourceSchedule {
using namespace OHOS::Security;
namespace {
    #define PAYLOAD_MAX_SIZE 4096
    constexpr int32_t DUMP_OPTION = 0;
    constexpr int32_t DUMP_PARAM_INDEX = 1;
    const int32_t ENG_MODE = OHOS::system::GetIntParameter("const.debuggable", 0);
    const std::string APP_PRELOAD_PLIGIN_NAME = "libapp_preload_plugin.z.so";
    const std::string NEEDED_PERMISSION = "ohos.permission.REPORT_RESOURCE_SCHEDULE_EVENT";
    const std::string SCENEBOARD_BUNDLE_NAME = "com.ohos.sceneboard";
    constexpr int32_t MEMMGR_UID = 1111;
    constexpr int32_t SAMGR_UID = 5555;
    constexpr int32_t FOUNDATION_UID = 5523;
    constexpr int32_t GRAPHIC_UID = 1003;
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
        ResType::SYNC_RES_TYPE_CHECK_MUTEX_BEFORE_START,
        ResType::RES_TYPE_COSMIC_CUBE_STATE_CHANGE,
        ResType::RES_TYPE_GESTURE_ANIMATION,
        ResType::RES_TYPE_RAISE_WORKER_THREAD_PRIORITY,
    };
    static const std::unordered_set<uint32_t> thirdPartRes_ = {
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
        ResType::RES_TYPE_REPORT_VSYNC_TID,
        ResType::RES_TYPE_WEB_DRAG_RESIZE,
        ResType::RES_TYPE_WEBVIEW_SCREEN_CAPTURE,
        ResType::RES_TYPE_WEBVIEW_VIDEO_STATUS_CHANGE,
        ResType::RES_TYPE_BT_SERVICE_EVENT,
        ResType::RES_TYPE_APP_FRAME_DROP,
        ResType::RES_TYPE_REPORT_DISTRIBUTE_TID,
        ResType::RES_TYPE_AXIS_EVENT,
        ResType::RES_TYPE_CROWN_ROTATION_STATUS,
    };
    static const std::unordered_set<uint32_t> saRes_ = {
        ResType::SYNC_RES_TYPE_THAW_ONE_APP,
        ResType::SYNC_RES_TYPE_GET_ALL_SUSPEND_STATE,
        ResType::SYNC_RES_TYPE_GET_THERMAL_DATA,
        ResType::RES_TYPE_CLICK_RECOGNIZE,
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
        ResType::RES_TYPE_AUDIO_RENDERER_SILENT_PLAYBACK,
        ResType::RES_TYPE_REPORT_GAME_SCHED,
        ResType::RES_TYPE_SEND_FRAME_EVENT,
        ResType::RES_TYPE_CLOUD_CONFIG_UPDATE,
        ResType::RES_TYPE_DEVICE_IDLE,
        ResType::RES_TYPE_BT_SERVICE_EVENT,
        ResType::RES_TYPE_BMM_MONITER_CHANGE_EVENT,
        ResType::RES_TYPE_GAME_INFO_NOTIFY,
        ResType::SYNC_RES_TYPE_REQUEST_MUTEX_STATUS,
        ResType::SYNC_RES_TYPE_SHOULD_FORCE_KILL_PROCESS,
        ResType::RES_TYPE_CLICK_RECOGNIZE,
        ResType::RES_TYPE_RSS_CLOUD_CONFIG_UPDATE,
        ResType::SYNC_RES_TYPE_GET_NWEB_PRELOAD_SET,
        ResType::RES_TYPE_AUDIO_RENDERER_STANDBY,
        ResType::RES_TYPE_APP_GAME_BOOST_EVENT,
        ResType::RES_TYPE_DISPLAY_MULTI_SCREEN,
        ResType::RES_TYPE_INTENT_CTRL_APP,
        ResType::RES_TYPE_GET_GAME_SCENE_INFO,
    };

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
        RESSCHED_LOGE("%{public}s: str=%{public}s is not jsonObj.", __func__, str.c_str());
        return jsonObj;
    }
    return jsonTmp;
}

ErrCode ResSchedService::ReportData(uint32_t resType, int64_t value, const std::string& payload)
{
    int32_t checkResult = RemoteRequestCheck();
    if (checkResult != ERR_OK) {
        RESSCHED_LOGE("check remote request fail.");
        return checkResult;
    }

    int32_t ret = CheckReportDataParcel(resType, value, payload);
    if (ret != ERR_OK) {
        RESSCHED_LOGE("%{public}s: check report data parcel fail ret=%{public}d, type=%{public}u.",
            __func__, ret, resType);
        return ret;
    }

    int32_t clientPid = IPCSkeleton::GetCallingPid();
    RESSCHED_LOGD("ResSchedService receive data from ipc resType: %{public}u, value: %{public}lld, pid: %{public}d",
                  resType, (long long)value, clientPid);
    nlohmann::json reportDataPayload = StringToJsonObj(payload);
    int32_t callingUid = IPCSkeleton::GetCallingUid();
    reportDataPayload["callingUid"] = std::to_string(callingUid);
    reportDataPayload["clientPid"] = std::to_string(clientPid);
    ResSchedMgr::GetInstance().ReportData(resType, value, reportDataPayload);
    return ERR_OK;
}

ErrCode ResSchedService::ReportSyncEvent(const uint32_t resType, const int64_t value, const std::string& payload,
    std::string& reply, int32_t& resultValue)
{
    int32_t checkResult = RemoteRequestCheck();
    if (checkResult != ERR_OK) {
        RESSCHED_LOGE("check remote request fail.");
        return checkResult;
    }

    int32_t ret = CheckReportDataParcel(resType, value, payload);
    if (ret != ERR_OK) {
        RESSCHED_LOGE("%{public}s: check report data parcel fail ret=%{public}d, type=%{public}u.",
            __func__, ret, resType);
        return ret;
    }
    nlohmann::json payloadJsonValue;
    nlohmann::json replyValue;
    payloadJsonValue = StringToJsonObj(payload);
    int32_t clientPid = IPCSkeleton::GetCallingPid();
    int32_t callingUid = IPCSkeleton::GetCallingUid();
    payloadJsonValue["clientPid"] = std::to_string(clientPid);
    payloadJsonValue["callingUid"] = std::to_string(callingUid);
    resultValue = PluginMgr::GetInstance().DeliverResource(
        std::make_shared<ResData>(resType, value, payloadJsonValue, replyValue));
    reply = replyValue.dump(-1, ' ', false, nlohmann::detail::error_handler_t::replace);
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
        return RES_SCHED_KILL_PROCESS_FAIL;
    }

    if (payload.size() > PAYLOAD_MAX_SIZE) {
        RESSCHED_LOGE("The payload is too long. DoS.");
        return RES_SCHED_KILL_PROCESS_FAIL;
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
    resultValue = NotifierMgr::GetInstance().GetSystemloadLevel();
    return ERR_OK;
}

void ResSchedService::OnDeviceLevelChanged(int32_t type, int32_t level)
{
    NotifierMgr::GetInstance().OnDeviceLevelChanged(type, level);
}

ErrCode ResSchedService::IsAllowedAppPreload(const std::string& bundleName, int32_t preloadMode, bool& resultValue)
{
    AccessToken::AccessTokenID tokenId = IPCSkeleton::GetCallingTokenID();
    int32_t uid = IPCSkeleton::GetCallingUid();
    AccessToken::ATokenTypeEnum tokenTypeFlag = AccessToken::AccessTokenKit::GetTokenTypeFlag(tokenId);
    if (uid != FOUNDATION_UID || tokenTypeFlag != AccessToken::ATokenTypeEnum::TOKEN_NATIVE) {
        RESSCHED_LOGE("Invalid calling token");
        return RES_SCHED_DATA_ERROR;
    }

    LoadAppPreloadPlugin();
    if (!appPreloadFunc_) {
        RESSCHED_LOGE("%{public}s, no allow AppPreload !", __func__, errno);
        resultValue = false;
        return RES_SCHED_DATA_ERROR;
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
        RESSCHED_LOGI("ResSchedService::LoadAppPreloadPlugin, already loaded AppPreloadPlugin");
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
        return RES_SCHED_ACCESS_TOKEN_FAIL;
    }

    AccessToken::HapTokenInfo callingTokenInfo;
    AccessToken::AccessTokenKit::GetHapTokenInfo(tokenId, callingTokenInfo);
    std::string callingBundleName = callingTokenInfo.bundleName;
    if (!PluginMgr::GetInstance().GetLinkJumpOptConfig(callingBundleName, isAllowedLinkJump)) {
        RESSCHED_LOGE("Invalid callingBundleName");
        resultValue = ERR_RES_SCHED_INVALID_PARAM;
        return ERR_RES_SCHED_INVALID_PARAM;
    }
    resultValue = ERR_OK;
    return ERR_OK;
}

bool ResSchedService::AllowDump()
{
    if (ENG_MODE == 0) {
        RESSCHED_LOGE("Not eng mode");
        return false;
    }
    Security::AccessToken::AccessTokenID tokenId = IPCSkeleton::GetCallingTokenID();
    int32_t ret = Security::AccessToken::AccessTokenKit::VerifyAccessToken(tokenId, "ohos.permission.DUMP");
    if (ret != Security::AccessToken::PermissionState::PERMISSION_GRANTED) {
        RESSCHED_LOGE("CheckPermission failed");
        return false;
    }
    return true;
}

int32_t ResSchedService::Dump(int32_t fd, const std::vector<std::u16string>& args)
{
    if (!AllowDump()) {
        return ERR_RES_SCHED_PERMISSION_DENIED;
    }
    RESSCHED_LOGI("%{public}s Dump service.", __func__);
    std::vector<std::string> argsInStr;
    std::transform(args.begin(), args.end(), std::back_inserter(argsInStr),
        [](const std::u16string &arg) {
        std::string ret = Str16ToStr8(arg);
        RESSCHED_LOGI("%{public}s arg: %{public}s.", __func__, ret.c_str());
        return ret;
    });
    std::string result;
    if (argsInStr.size() == 0) {
        // hidumper -s said '-h'
        DumpUsage(result);
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
        }
    }

    if (!SaveStringToFd(fd, result)) {
        RESSCHED_LOGE("%{public}s save to fd failed.", __func__);
    }
    return ERR_OK;
}


void ResSchedService::DumpExt(const std::vector<std::string>& argsInStr, std::string &result)
{
    if (argsInStr[DUMP_OPTION] == "-h") {
        DumpUsage(result);
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
        .append("    -p (plugin name): show one plugin info.\n");
    PluginMgr::GetInstance().DumpHelpFromPlugin(result);
}

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

void ResSchedService::DumpAllPluginConfig(std::string &result)
{
    result.append("================Resource Schedule Plugin Config================\n");
    PluginMgr::GetInstance().DumpAllPluginConfig(result);
}

int32_t ResSchedService::CheckReportDataParcel(const uint32_t& type, const int64_t& value,
    const std::string& payload)
{
    if (!IsTypeVaild(type)) {
        RESSCHED_LOGE("type:%{public}u is invalid", type);
        return ERR_RES_SCHED_PARCEL_ERROR;
    }
    if (!IsSBDResType(type) && !IsThirdPartType(type, thirdPartRes_) && !IsHasPermission(type, saRes_)) {
        RESSCHED_LOGE("type:%{public}u, no permission", type);
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
    if (ResCommonUtil::GetNowMillTime() < nextReportBigDataTime_) {
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
        nextReportBigDataTime_ = ResCommonUtil::GetNowMillTime() + FOUR_HOUR_TIME;
    }
    bigDataReportCount_.fetch_add(1, std::memory_order_relaxed);
}

bool ResSchedService::IsLimitRequest(int32_t uid)
{
    std::lock_guard<std::mutex> lock(mutex_);
    int64_t nowTime = ResCommonUtil::GetNowMillTime();
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
    if (nowTime - nextCheckTime_.load() > LIMIT_REQUEST_TIME) {
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

} // namespace ResourceSchedule
} // namespace OHOS

