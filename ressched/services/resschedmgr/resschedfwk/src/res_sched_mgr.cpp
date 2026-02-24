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

#include "res_sched_mgr.h"

#include <cinttypes>
#include <map>

#include "app_mgr_interface.h"
#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "notifier_mgr.h"
#include "res_exe_type.h"
#include "res_ipc_init.h"
#include "res_sched_exe_client.h"
#include "res_sched_log.h"
#include "res_type.h"
#include "plugin_mgr.h"
#include "hitrace_meter.h"
#include "scene_recognizer_mgr.h"
#include "system_ability_definition.h"
#include "res_sched_hitrace_chain.h"
#include "res_data.h"
#include "res_sched_signature_validator.h"
#include "res_bundle_mgr_helper.h"

#ifdef OBSERVER_EVENT_TEST
#define WEAK_FUNC __attribute__((weak))
#else
#define WEAK_FUNC
#endif
 
namespace OHOS {
namespace ResourceSchedule {
using namespace AppExecFwk;
namespace {
    static const std::map<uint32_t, std::string> resTypeToStr = {
        { ResType::SYNC_RES_TYPE_THAW_ONE_APP, "SYNC_RES_TYPE_THAW_ONE_APP"},
        { ResType::SYNC_RES_TYPE_GET_APP_TYPE, "SYNC_RES_TYPE_GET_APP_TYPE"},
        { ResType::SYNC_RES_TYPE_GET_ALL_SUSPEND_STATE, "SYNC_RES_TYPE_GET_ALL_SUSPEND_STATE"},
        { ResType::SYNC_RES_TYPE_GET_THERMAL_DATA, "SYNC_RES_TYPE_GET_THERMAL_DATA"},
        { ResType::RES_TYPE_SCREEN_STATUS, "SCREEN_STATUS" },
        { ResType::RES_TYPE_APP_STATE_CHANGE, "APP_STATE_CHANGE" },
        { ResType::RES_TYPE_ABILITY_STATE_CHANGE, "ABILITY_STATE_CHANGE" },
        { ResType::RES_TYPE_EXTENSION_STATE_CHANGE, "EXTENSION_STATE_CHANGE" },
        { ResType::RES_TYPE_PROCESS_STATE_CHANGE, "PROCESS_STATE_CHANGE" },
        { ResType::RES_TYPE_WINDOW_FOCUS, "WINDOW_FOCUS" },
        { ResType::RES_TYPE_TRANSIENT_TASK, "TRANSIENT_TASK" },
        { ResType::RES_TYPE_CONTINUOUS_TASK, "CONTINUOUS_TASK" },
        { ResType::RES_TYPE_CGROUP_ADJUSTER, "CGROUP_ADJUSTER" },
        { ResType::RES_TYPE_CLICK_RECOGNIZE, "CLICK_RECOGNIZE" },
        { ResType::RES_TYPE_PUSH_PAGE, "PUSH_PAGE" },
        { ResType::RES_TYPE_SLIDE_RECOGNIZE, "SLIDE_RECOGNIZE" },
        { ResType::RES_TYPE_WINDOW_VISIBILITY_CHANGE, "WINDOW_VISIBILITY_CHANGE" },
        { ResType::RES_TYPE_REPORT_MMI_PROCESS, "REPORT_MMI_PROCESS" },
        { ResType::RES_TYPE_REPORT_RENDER_THREAD, "REPORT_RENDER_THREAD" },
        { ResType::RES_TYPE_APP_INSTALL_UNINSTALL, "APP_INSTALL_UNINSTALL" },
        { ResType::RES_TYPE_WIFI_CONNECT_STATE_CHANGE, "WIFI_CONNECT_STATE_CHANGE" },
        { ResType::RES_TYPE_USER_SWITCH, "USER_SWITCH" },
        { ResType::RES_TYPE_USER_REMOVE, "USER_REMOVE" },
        { ResType::RES_TYPE_SCREEN_LOCK, "SCREEN_LOCK" },
        { ResType::RES_TYPE_BLUETOOTH_A2DP_CONNECT_STATE_CHANGE, "BLUETOOTH_A2DP_CONNECT_STATE_CHANGE" },
        { ResType::RES_TYPE_NETWORK_LATENCY_REQUEST, "NETWORK_LATENCY_REQUEST" },
        { ResType::RES_TYPE_CALL_STATE_UPDATE, "CALL_STATE_UPDATE" },
        { ResType::RES_TYPE_THREAD_QOS_CHANGE, "THREAD_QOS_CHANGE" },
        { ResType::RES_TYPE_AUDIO_RENDER_STATE_CHANGE, "AUDIO_RENDER_STATE_CHANGE" },
        { ResType::RES_TYPE_AUDIO_RING_MODE_CHANGE, "AUDIO_RING_MODE_CHANGE" },
        { ResType::RES_TYPE_AUDIO_VOLUME_KEY_CHANGE, "AUDIO_VOLUME_KEY_CHANGE" },
        { ResType::RES_TYPE_APP_ABILITY_START, "APP_ABILITY_START" },
        { ResType::RES_TYPE_PRELOAD_APPLICATION, "RES_TYPE_PRELOAD_APPLICATION" },
        { ResType::RES_TYPE_POP_PAGE, "POP_PAGE" },
        { ResType::RES_TYPE_WEB_GESTURE, "WEB_GESTURE" },
        { ResType::RES_TYPE_DEVICE_STILL_STATE_CHANGE, "DEVICE_STILL_STATE_CHANGE" },
        { ResType::RES_TYPE_RESIZE_WINDOW, "RES_TYPE_RESIZE_WINDOW" },
        { ResType::RES_TYPE_MOVE_WINDOW, "RES_TYPE_MOVE_WINDOW" },
        { ResType::RES_TYPE_SHOW_REMOTE_ANIMATION, "RES_TYPE_SHOW_REMOTE_ANIMATION" },
        { ResType::RES_TYPE_LOAD_PAGE, "RES_TYPE_LOAD_PAGE" },
        { ResType::RES_TYPE_REPORT_CAMERA_STATE, "RES_TYPE_REPORT_CAMERA_STATE" },
        { ResType::RES_TYPE_RUNNINGLOCK_STATE, "RES_TYPE_RUNNINGLOCK_STATE" },
        { ResType::RES_TYPE_DRAG_STATUS_BAR, "RES_TYPE_DRAG_STATUS_BAR" },
        { ResType::RES_TYPE_REPORT_SCENE_BOARD, "RES_TYPE_REPORT_SCENE_BOARD" },
        { ResType::RES_TYPE_REPORT_KEY_THREAD, "RES_TYPE_REPORT_KEY_THREAD" },
        { ResType::RES_TYPE_REPORT_WINDOW_STATE, "RES_TYPE_REPORT_WINDOW_STATE" },
        { ResType::RES_TYPE_REPORT_SCENE_SCHED, "RES_TYPE_REPORT_SCENE_SCHED" },
        { ResType::RES_TYPE_WEB_GESTURE_MOVE, "RES_TYPE_WEB_GESTURE_MOVE" },
        { ResType::RES_TYPE_WEB_SLIDE_NORMAL, "RES_TYPE_WEB_SLIDE_NORMAL" },
        { ResType::RES_TYPE_LOAD_URL, "RES_TYPE_LOAD_URL" },
        { ResType::RES_TYPE_MOUSEWHEEL, "RES_TYPE_MOUSEWHEEL" },
        { ResType::RES_TYPE_WEBVIEW_AUDIO_STATUS_CHANGE, "RES_TYPE_WEBVIEW_AUDIO_STATUS_CHANGE" },
        { ResType::RES_TYPE_MMI_INPUT_STATE, "RES_TYPE_MMI_INPUT_STATE" },
        { ResType::RES_TYPE_ANCO_CUST, "RES_TYPE_ANCO_CUST" },
        { ResType::RES_TYPE_TIMEZONE_CHANGED, "RES_TYPE_TIMEZONE_CHANGED" },
        { ResType::RES_TYPE_TIME_CHANGED, "RES_TYPE_TIME_CHANGED" },
        { ResType::RES_TYPE_NITZ_TIME_CHANGED, "RES_TYPE_NITZ_TIME_CHANGED" },
        { ResType::RES_TYPE_NITZ_TIMEZONE_CHANGED, "RES_TYPE_NITZ_TIMEZONE_CHANGED" },
        { ResType::RES_TYPE_CHARGING_DISCHARGING, "RES_TYPE_CHARGING_DISCHARGING" },
        { ResType::RES_TYPE_USB_DEVICE, "RES_TYPE_USB_DEVICE" },
        { ResType::RES_TYPE_CALL_STATE_CHANGED, "RES_TYPE_CALL_STATE_CHANGED" },
        { ResType::RES_TYPE_WIFI_P2P_STATE_CHANGED, "RES_TYPE_WIFI_P2P_STATE_CHANGED" },
        { ResType::RES_TYPE_CONNECTION_OBSERVER, "RES_TYPE_CONNECTION_OBSERVER" },
        { ResType::RES_TYPE_AV_SESSION_ON_SESSION_CREATE, "RES_TYPE_AV_SESSION_ON_SESSION_CREATE" },
        { ResType::RES_TYPE_AV_SESSION_ON_SESSION_RELEASE, "RES_TYPE_AV_SESSION_ON_SESSION_RELEASE" },
        { ResType::RES_TYPE_AV_SESSION_ON_TOP_SESSION_CHANGE, "RES_TYPE_AV_SESSION_ON_TOP_SESSION_CHANGE" },
        { ResType::RES_TYPE_ON_APP_STATE_CHANGED, "RES_TYPE_ON_APP_STATE_CHANGED" },
        { ResType::RES_TYPE_PROCESS_PRE_FOREGROUND_CHANGE, "RES_TYPE_PROCESS_PRE_FOREGROUND_CHANGE" },
        { ResType::RES_TYPE_EFFICIENCY_RESOURCES_STATE_CHANGED, "RES_TYPE_EFFICIENCY_RESOURCES_STATE_CHANGED" },
        { ResType::RES_TYPE_AV_CODEC_STATE, "RES_TYPE_AV_CODEC_STATE" },
        { ResType::RES_TYPE_APP_ASSOCIATED_START, "RES_TYPE_APP_ASSOCIATED_START" },
        { ResType::RES_TYPE_THERMAL_STATE, "RES_TYPE_THERMAL_STATE"},
        { ResType::RES_TYPE_SOCPERF_CUST_EVENT_BEGIN, "RES_TYPE_SOCPERF_CUST_EVENT_BEGIN" },
        { ResType::RES_TYPE_SOCPERF_CUST_EVENT_END, "RES_TYPE_SOCPERF_CUST_EVENT_END" },
        { ResType::RES_TYPE_WINDOW_DRAWING_CONTENT_CHANGE, "RES_TYPE_WINDOW_DRAWING_CONTENT_CHANGE" },
        { ResType::RES_TYPE_REPORT_SCREEN_CAPTURE, "RES_TYPE_REPORT_SCREEN_CAPTURE" },
        { ResType::RES_TYPE_LONG_FRAME, "RES_TYPE_LONG_FRAME" },
        { ResType::RES_TYPE_KEY_PERF_SCENE, "RES_TYPE_KEY_PERF_SCENE" },
        { ResType::RES_TYPE_SUPER_LAUNCHER, "RES_TYPE_SUPER_LAUNCHER" },
        { ResType::RES_TYPE_CAST_SCREEN, "RES_TYPE_CAST_SCREEN" },
        { ResType::RES_TYPR_SCREEN_COLLABROATION, "RES_TYPR_SCREEN_COLLABROATION" },
        { ResType::RES_TYPE_SA_CONTROL_APP_EVENT, "RES_TYPE_SA_CONTROL_APP_EVENT" },
        { ResType::RES_TYPE_SYSTEM_CPU_LOAD, "RES_TYPE_SYSTEM_CPU_LOAD" },
        { ResType::RES_TYPE_UPLOAD_DOWNLOAD, "RES_TYPE_UPLOAD_DOWNLOAD" },
        { ResType::RES_TYPE_SPLIT_SCREEN, "RES_TYPE_SPLIT_SCREEN" },
        { ResType::RES_TYPE_FLOATING_WINDOW, "RES_TYPE_FLOATING_WINDOW" },
        { ResType::RES_TYPE_REPORT_DISTRIBUTE_TID, "RES_TYPE_REPORT_DISTRIBUTE_TID"},
        { ResType::RES_TYPE_FRAME_RATE_REPORT, "RES_TYPE_FRAME_RATE_REPORT"},
        { ResType::RES_TYPE_WEB_DRAG_RESIZE, "RES_TYPE_WEB_DRAG_RESIZE"},
        { ResType::RES_TYPE_WEBVIEW_SCREEN_CAPTURE, "RES_TYPE_WEBVIEW_SCREEN_CAPTURE"},
        { ResType::RES_TYPE_WEBVIEW_VIDEO_STATUS_CHANGE, "RES_TYPE_WEBVIEW_VIDEO_STATUS_CHANGE"},
        { ResType::RES_TYPE_LOCATION_STATUS_CHANGE, "RES_TYPE_LOCATION_STATUS_CHANGE"},
        { ResType::RES_TYPE_DEVICE_MODE_STATUS, "RES_TYPE_DEVICE_MODE_STATUS"},
        { ResType::RES_TYPE_REPORT_DISTRIBUTE_COMPONENT_CHANGE, "RES_TYPE_REPORT_DISTRIBUTE_COMPONENT_CHANGE"},
        { ResType::RES_TYPE_FORM_STATE_CHANGE_EVENT, "RES_TYPE_FORM_STATE_CHANGE_EVENT"},
        { ResType::RES_TYPE_POWER_MODE_CHANGED, "RES_TYPE_POWER_MODE_CHANGED" },
        { ResType::RES_TYPE_THERMAL_SCENARIO_REPORT, "RES_TYPE_THERMAL_SCENARIO_REPORT" },
        { ResType::SYNC_RES_TYPE_SHOULD_FORCE_KILL_PROCESS, "SYNC_RES_TYPE_SHOULD_FORCE_KILL_PROCESS" },
        { ResType::RES_TYPE_BOOT_COMPLETED, "RES_TYPE_BOOT_COMPLETED" },
        { ResType::RES_TYPE_CONTINUOUS_STARTUP, "RES_TYPE_CONTINUOUS_STARTUP" },
        { ResType::RES_TYPE_AUDIO_RENDERER_SILENT_PLAYBACK, "RES_TYPE_AUDIO_RENDERER_SILENT_PLAYBACK"},
        { ResType::RES_TYPE_REPORT_GAME_SCHED, "RES_TYPE_REPORT_GAME_SCHED" },
        { ResType::RES_TYPE_REPORT_VSYNC_TID, "RES_TYPE_REPORT_VSYNC_TID" },
        { ResType::RES_TYPE_BT_SERVICE_EVENT, "RES_TYPE_BT_SERVICE_EVENT"},
        { ResType::RES_TYPE_APP_FRAME_DROP, "RES_TYPE_APP_FRAME_DROP"},
        { ResType::RES_TYPE_CLOUD_CONFIG_UPDATE, "RES_TYPE_CLOUD_CONFIG_UPDATE" },
        { ResType::RES_TYPE_GAME_INFO_NOTIFY, "RES_TYPE_GAME_INFO_NOTIFY"},
        { ResType::RES_TYPE_RSS_CLOUD_CONFIG_UPDATE, "RES_TYPE_RSS_CLOUD_CONFIG_UPDATE" },
        { ResType::RES_TYPE_APP_GAME_BOOST_EVENT, "RES_TYPE_APP_GAME_BOOST_EVENT" },
        { ResType::RES_TYPE_GESTURE_ANIMATION, "RES_TYPE_GESTURE_ANIMATION" },
        { ResType::RES_TYPE_AUDIO_RENDERER_STANDBY, "RES_TYPE_AUDIO_RENDERER_STANDBY" },
        { ResType::RES_TYPE_AXIS_EVENT, "RES_TYPE_AXIS_EVENT" },
        { ResType::RES_TYPE_INTENT_CTRL_APP, "RES_TYPE_INTENT_CTRL_APP" },
        { ResType::RES_TYPE_DISPLAY_MULTI_SCREEN, "RES_TYPE_DISPLAY_MULTI_SCREEN" },
        { ResType::RES_TYPE_CROWN_ROTATION_STATUS, "RES_TYPE_CROWN_ROTATION_STATUS" },
        { ResType::SYNC_RES_TYPE_APP_IS_IN_CLICK_REPORT_EXT_LIST, "SYNC_RES_TYPE_APP_IS_IN_CLICK_REPORT_EXT_LIST" },
        { ResType::RES_TYPE_RED_ENVELOPE, "RES_TYPE_RED_ENVELOPE"},
        { ResType::RES_TYPE_GET_GAME_SCENE_INFO, "RES_TYPE_GET_GAME_SCENE_INFO"},
        { ResType::RES_TYPE_RECV_ABC_LOAD_COMPLETED, "RES_TYPE_RECV_ABC_LOAD_COMPLETED"},
        { ResType::RES_TYPE_RAISE_WORKER_THREAD_PRIORITY, "RES_TYPE_RAISE_WORKER_THREAD_PRIORITY" },
        { ResType::SYNC_RES_TYPE_GET_SUSPEND_STATE_BY_UID, "SYNC_RES_TYPE_GET_SUSPEND_STATE_BY_UID" },
        { ResType::SYNC_RES_TYPE_GET_SUSPEND_STATE_BY_PID, "SYNC_RES_TYPE_GET_SUSPEND_STATE_BY_PID" },
        { ResType::RES_TYPE_CAMERA_RESET_PRIORITY, "RES_TYPE_CAMERA_RESET_PRIORITY" },
        { ResType::RES_TYPE_LOAD_KILL_REASON_CONFIG, "RES_TYPE_LOAD_KILL_REASON_CONFIG" },
#ifdef RESSCHED_RESOURCESCHEDULE_FILE_COPY_SOC_PERF_ENABLE
        { ResType::RES_TYPE_FILE_COPY_STATUS, "RES_TYPE_FILE_COPY_STATUS" },
#endif
        { ResType::RES_TYPE_CHECK_APP_IS_IN_SCHEDULE_LIST, "RES_TYPE_CHECK_APP_IS_IN_SCHEDULE_LIST" },
        { ResType::RES_TYPE_PAGE_TRANSITION, "RES_TYPE_PAGE_TRANSITION" },
        { ResType::RES_TYPE_DYNAMICALLY_SET_SUSPEND_EXEMPT, "RES_TYPE_DYNAMICALLY_SET_SUSPEND_EXEMPT" },
        { ResType::RES_TYPE_WEB_SLIDE_SCROLL, "RES_TYPE_WEB_SLIDE_SCROLL" },
        { ResType::RES_TYPE_PARAM_UPDATE_EVENT, "RES_TYPE_PARAM_UPDATE_EVENT" },
        { ResType::RES_TYPE_WIFI_POWER_STATE_CHANGE, "RES_TYPE_WIFI_POWER_STATE_CHANGE" },
        { ResType::RES_TYPE_OVERLAY_EVENT, "RES_TYPE_OVERLAY_EVENT" },
        { ResType::RES_TYPE_APP_OPT_FROM_RECENT, "RES_TYPE_APP_OPT_FROM_RECENT" },
        { ResType::RES_TYPE_SWIPE_DIRECTION_UP, "RES_TYPE_SWIPE_DIRECTION_UP" },
        { ResType::RES_TYPE_WINDOW_PANEL, "RES_TYPE_WINDOW_PANEL" },
        { ResType::RES_TYPE_WINDOW_SPLIT_SCREEN, "RES_TYPE_WINDOW_SPLIT_SCREEN" },
        { ResType::RES_TYPE_SUPPLY_LEVEL, "RES_TYPE_SUPPLY_LEVEL" },
        { ResType::RES_TYPE_GC_THREAD_QOS_STATUS_CHANGE, "RES_TYPE_GC_THREAD_QOS_STATUS_CHANGE"},
        { ResType::RES_TYPE_DEVICE_IDLE_CHANGED, "RES_TYPE_DEVICE_IDLE_CHANGED"},
        { ResType::RES_TYPE_USER_SLEEP_STATE_CHANGED, "RES_TYPE_USER_SLEEP_STATE_CHANGED"},
        { ResType::RES_TYPE_ADJUST_PROTECTLRU_RECLAIM_RATIO, "RES_TYPE_ADJUST_PROTECTLRU_RECLAIM_RATIO"},
        { ResType::RES_TYPE_BACKGROUND_STATUS, "RES_TYPE_BACKGROUND_STATUS"},
        { ResType::SYNC_RES_TYPE_QUERY_TOPN_BUNDLE_USAGE_INFO, "SYNC_RES_TYPE_QUERY_TOPN_BUNDLE_USAGE_INFO" },
        { ResType::RES_TYPE_MEM_OFF_PEAK, "RES_TYPE_MEM_OFF_PEAK" },
        { ResType::RES_TYPE_NAP_MODE, "RES_TYPE_NAP_MODE" },
        { ResType::RES_TYPE_BACKPRESSED_EVENT, "RES_TYPE_BACKPRESSED_EVENT"},
        { ResType::SYNC_RES_TYPE_GET_SMART_GC_SCENE_INFO, "SYNC_RES_TYPE_GET_SMART_GC_SCENE_INFO"},
        { ResType::RES_TYPE_LIVE_VIEW_EVENT, "RES_TYPE_LIVE_VIEW_EVENT"},
        { ResType::RES_TYPE_IME_QOS_CHANGE, "RES_TYPE_IME_QOS_CHANGE" },
        { ResType::RES_TYPE_WEB_SUBWIN_TASK, "RES_TYPE_WEB_SUBWIN_TASK"},
    };
    OHOS::sptr<OHOS::AppExecFwk::IAppMgr> GetAppManagerInstance()
    {
        OHOS::sptr<OHOS::ISystemAbilityManager> systemAbilityManager =
            OHOS::SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        if (!systemAbilityManager) {
            RESSCHED_LOGE("%{public}s : systemAbilityManager nullptr!", __func__);
            return nullptr;
        }
        OHOS::sptr<OHOS::IRemoteObject> object = systemAbilityManager->GetSystemAbility(OHOS::APP_MGR_SERVICE_ID);
        return OHOS::iface_cast<OHOS::AppExecFwk::IAppMgr>(object);
    }
}

IMPLEMENT_SINGLE_INSTANCE(ResSchedMgr);

void ResSchedMgr::Init()
{
    std::shared_ptr<ResourceSchedule::ResBundleMgrHelper> resBundleMgr =
        std::make_shared<ResourceSchedule::ResBundleMgrHelper>();
    ResSchedSignatureValidator::GetInstance().InitSignatureDependencyInterface(resBundleMgr);
    PluginMgr::GetInstance().Init();
    PluginMgr::GetInstance().SetResTypeStrMap(resTypeToStr);

    if (!killProcess_) {
        killProcess_ = std::make_shared<KillProcess>();
    }

    InitExecutorPlugin(true);
}

void ResSchedMgr::Stop()
{
    PluginMgr::GetInstance().Stop();
}

WEAK_FUNC void ResSchedMgr::ReportData(uint32_t resType, int64_t value, const nlohmann::json& payload)
{
    ResSchedHiTraceChain traceChain(__func__);
    ReportDataInner(resType, value, payload);
}

void ResSchedMgr::ReportDataInner(uint32_t resType, int64_t value, const nlohmann::json& payload)
{
    RESSCHED_LOGD("%{public}s, receive resType = %{public}u, value = %{public}lld.", __func__,
        resType, (long long)value);
    std::string trace_str(__func__);
    trace_str.append(",resType[").append(std::to_string(resType)).append("]");
    trace_str.append(",value[").append(std::to_string(value)).append("]");
    StartTrace(HITRACE_TAG_OHOS, trace_str, -1);
    auto resData = std::make_shared<ResData>(resType, value, payload);
    PluginMgr::GetInstance().DispatchResource(resData);
    SceneRecognizerMgr::GetInstance().DispatchResource(resData);
    FinishTrace(HITRACE_TAG_OHOS);
}

int32_t ResSchedMgr::KillProcessByClient(const nlohmann::json& payload)
{
    return killProcess_->KillProcessByPidWithClient(payload);
}

void ResSchedMgr::InitExecutorPlugin(bool isProcessInit)
{
    std::vector<std::string> configStrs = PluginMgr::GetInstance().GetConfigReaderStr();
    std::vector<std::string> switchStrs = PluginMgr::GetInstance().GetPluginSwitchStr();

    nlohmann::json payload;
    nlohmann::json context;
    payload["config"] = configStrs;
    payload["switch"] = switchStrs;
    RESSCHED_LOGI("ResSchedMgr start InitExecutorPlugin");
    ResSchedExeClient::GetInstance().SendRequestSync(ResExeType::RES_TYPE_EXECUTOR_PLUGIN_INIT, 0, payload, context);

    if (isProcessInit) {
        PluginMgr::GetInstance().ParseConfigReader(configStrs);
        PluginMgr::GetInstance().ParsePluginSwitch(switchStrs);
    }
    ResSchedIpcThread::GetInstance().SetInitFlag(true);
}

void ResSchedMgr::OnApplicationStateChange(int32_t state, int32_t pid)
{
    RESSCHED_LOGD("OnApplicationStateChange called, state: %{public}d, pid : %{public}d .", state, pid);
    std::lock_guard<std::mutex> autoLock(foregroundPidsMutex_);

    if (state == static_cast<int32_t>(ApplicationState::APP_STATE_FOREGROUND)) {
        if (foregroundPids_.find(pid) == foregroundPids_.end()) {
            foregroundPids_.emplace(pid);
        }
    }
    if (state == static_cast<int32_t>(ApplicationState::APP_STATE_BACKGROUND)
        || state == static_cast<int32_t>(ApplicationState::APP_STATE_TERMINATED)
        || state == static_cast<int32_t>(ApplicationState::APP_STATE_END)) {
        auto item = foregroundPids_.find(pid);
        if (item != foregroundPids_.end()) {
            foregroundPids_.erase(item);
        }
    }
}

bool ResSchedMgr::IsForegroundApp(int32_t pid)
{
    std::lock_guard<std::mutex> autoLock(foregroundPidsMutex_);
    auto item = foregroundPids_.find(pid);
    return item != foregroundPids_.end();
}

void ResSchedMgr::InitForegroundAppInfo()
{
    sptr<OHOS::AppExecFwk::IAppMgr> appManager = GetAppManagerInstance();
    if (!appManager) {
        RESSCHED_LOGE("%{public}s app manager nullptr!", __func__);
        return;
    }
    std::vector<AppExecFwk::AppStateData> fgapplist;
    int32_t err = appManager->GetForegroundApplications(fgapplist);
    if (err != ERR_OK) {
        RESSCHED_LOGE("%{public}s GetForegroundApplications failed. err:%{public}d", __func__, err);
        return;
    }
    std::lock_guard<std::mutex> autoLock(foregroundPidsMutex_);
    for (const auto& item : fgapplist) {
        if (foregroundPids_.find(item.pid) == foregroundPids_.end()) {
            foregroundPids_.emplace(item.pid);
        }
    }
    RESSCHED_LOGI("%{public}s succeed", __func__);
}

void ResSchedMgr::ReportAppStateInProcess(int32_t state, int32_t pid)
{
    NotifierMgr::GetInstance().OnApplicationStateChange(state, pid);
    ResSchedMgr::GetInstance().OnApplicationStateChange(state, pid);
}

void ResSchedMgr::ReportProcessStateInProcess(int32_t state, int32_t pid)
{
    ResSchedMgr::GetInstance().OnApplicationStateChange(state, pid);
}

std::unordered_set<uint32_t>& ResSchedMgr::GetAllowSCBReportResExt()
{
    return allowSCBReportResExt_;
}

std::unordered_set<uint32_t>& ResSchedMgr::GetAllowAllSAReportResExt()
{
    return allowAllSAReportResExt_;
}

std::unordered_map<uint32_t, std::unordered_set<int32_t>>& ResSchedMgr::GetAllowSomeSAReportResExt()
{
    return allowSomeSAReportResExt_;
}

std::unordered_set<uint32_t>& ResSchedMgr::GetAllowAllAppReportResExt()
{
    return allowAllAppReportResExt_;
}

std::unordered_set<uint32_t>& ResSchedMgr::GetAllowFgAppReportResExt()
{
    return allowFgAppReportResExt_;
}

void ResSchedMgr::SetAllowSCBReportResExt(const std::unordered_set<uint32_t>& allowSCBReportResExt)
{
    allowSCBReportResExt_ = allowSCBReportResExt;
}


void ResSchedMgr::SetAllowAllSAReportResExt(const std::unordered_set<uint32_t>& allowAllSAReportResExt)
{
    allowAllSAReportResExt_ = allowAllSAReportResExt;
}


void ResSchedMgr::SetAllowSomeSAReportResExt(const std::unordered_map<uint32_t, std::unordered_set<int32_t>>&
    allowSomeSAReportResExt)
{
    allowSomeSAReportResExt_ = allowSomeSAReportResExt;
}

void ResSchedMgr::SetAllowAllAppReportResExt(const std::unordered_set<uint32_t>& allowAllAppReportResExt)
{
    allowAllAppReportResExt_ = allowAllAppReportResExt;
}

void ResSchedMgr::SetAllowFgAppReportResExt(const std::unordered_set<uint32_t>& allowFgAppReportResExt)
{
    allowFgAppReportResExt_ = allowFgAppReportResExt;
}

extern "C" void SetAllowSCBReportResExt(const std::unordered_set<uint32_t>& allowSCBReportResExt)
{
    ResSchedMgr::GetInstance().SetAllowSCBReportResExt(allowSCBReportResExt);
}

extern "C" void SetAllowAllSAReportResExt(const std::unordered_set<uint32_t>& allowAllSAReportResExt)
{
    ResSchedMgr::GetInstance().SetAllowAllSAReportResExt(allowAllSAReportResExt);
}

extern "C" void SetAllowSomeSAReportResExt(const std::unordered_map<uint32_t, std::unordered_set<int32_t>>&
    allowSomeSAReportResExt)
{
    ResSchedMgr::GetInstance().SetAllowSomeSAReportResExt(allowSomeSAReportResExt);
}

extern "C" void SetAllowAllAppReportResExt(const std::unordered_set<uint32_t>& allowAllAppReportResExt)
{
    ResSchedMgr::GetInstance().SetAllowAllAppReportResExt(allowAllAppReportResExt);
}

extern "C" void SetAllowFgAppReportResExt(const std::unordered_set<uint32_t>& allowFgAppReportResExt)
{
    ResSchedMgr::GetInstance().SetAllowFgAppReportResExt(allowFgAppReportResExt);
}

extern "C" void ReportDataInProcess(uint32_t resType, int64_t value, const nlohmann::json& payload)
{
    ResSchedMgr::GetInstance().ReportData(resType, value, payload);
}

extern "C" int32_t KillProcessInProcess(const nlohmann::json& payload)
{
    return ResSchedMgr::GetInstance().KillProcessByClient(payload);
}

extern "C" int32_t ReportSyncEventInProcess(uint32_t type, int64_t value,
    const nlohmann::json& payload, nlohmann::json& reply)
{
    return PluginMgr::GetInstance().DeliverResource(
        std::make_shared<ResData>(type, value, payload, reply));
}
} // namespace ResourceSchedule
} // namespace OHOS
