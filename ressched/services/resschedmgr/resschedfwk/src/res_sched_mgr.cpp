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

#include "res_sched_mgr.h"

#include <cinttypes>
#include <map>

#include "cgroup_sched.h"
#include "notifier_mgr.h"
#include "res_sched_log.h"
#include "res_type.h"
#include "plugin_mgr.h"
#include "hitrace_meter.h"

namespace OHOS {
namespace ResourceSchedule {
using namespace AppExecFwk;
namespace {
    const std::map<uint32_t, std::string> resTypeToStr = {
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
        { ResType::RES_TYPE_AUDIO_SILENT_PLAYBACK, "RES_TYPE_AUDIO_SILENT_PLAYBACK" },
        { ResType::RES_TYPE_DEVICE_MODE_STATUS, "RES_TYPE_DEVICE_MODE_STATUS"},
        { ResType::RES_TYPE_SYSTEMLOAD_LEVEL, "RES_TYPE_SYSTEMLOAD_LEVEL"},
        { ResType::RES_TYPE_REPORT_DISTRIBUTE_COMPONENT_CHANGE, "RES_TYPE_REPORT_DISTRIBUTE_COMPONENT_CHANGE"},
        { ResType::RES_TYPE_FORM_STATE_CHANGE_EVENT, "RES_TYPE_FORM_STATE_CHANGE_EVENT"},
        { ResType::RES_TYPE_POWER_MODE_CHANGED, "RES_TYPE_POWER_MODE_CHANGED" },
        { ResType::RES_TYPE_THERMAL_SCENARIO_REPORT, "RES_TYPE_THERMAL_SCENARIO_REPORT" },
        { ResType::RES_TYPE_BOOT_COMPLETED, "RES_TYPE_BOOT_COMPLETED" },
        { ResType::RES_TYPE_CONTINUOUS_STARTUP, "RES_TYPE_CONTINUOUS_STARTUP" },
    };
}

IMPLEMENT_SINGLE_INSTANCE(ResSchedMgr);

void ResSchedMgr::Init()
{
    PluginMgr::GetInstance().Init();
    PluginMgr::GetInstance().SetResTypeStrMap(resTypeToStr);

    if (!killProcess_) {
        killProcess_ = std::make_shared<KillProcess>();
    }
}

void ResSchedMgr::Stop()
{
    PluginMgr::GetInstance().Stop();
    PluginMgr::GetInstance().ClearResTypeStrMap();
}

void ResSchedMgr::ReportData(uint32_t resType, int64_t value, const nlohmann::json& payload)
{
    ReportDataInner(resType, value, payload);
    DispatchResourceInner(resType, value, payload);
}

void ResSchedMgr::ReportDataInner(uint32_t resType, int64_t value, const nlohmann::json& payload)
{
    RESSCHED_LOGD("%{public}s, receive resType = %{public}u, value = %{public}lld.", __func__,
        resType, (long long)value);
    std::string trace_str(__func__);
    trace_str.append(",resType[").append(std::to_string(resType)).append("]");
    trace_str.append(",value[").append(std::to_string(value)).append("]");
    StartTrace(HITRACE_TAG_OHOS, trace_str, -1);
    PluginMgr::GetInstance().DispatchResource(std::make_shared<ResData>(resType, value, payload));
    FinishTrace(HITRACE_TAG_OHOS);
}

int32_t ResSchedMgr::KillProcessByClient(const nlohmann::json& payload)
{
    return killProcess_->KillProcessByPidWithClient(payload);
}

void ResSchedMgr::DispatchResourceInner(uint32_t resType, int64_t value, const nlohmann::json& payload)
{
    CgroupSchedDispatch(resType, value, payload);
}

extern "C" void ReportDataInProcess(uint32_t resType, int64_t value, const nlohmann::json& payload)
{
    ResSchedMgr::GetInstance().ReportData(resType, value, payload);
}

extern "C" void ReportAppStateInProcess(int32_t state, int32_t pid)
{
    NotifierMgr::GetInstance().OnApplicationStateChange(state, pid);
}
} // namespace ResourceSchedule
} // namespace OHOS
