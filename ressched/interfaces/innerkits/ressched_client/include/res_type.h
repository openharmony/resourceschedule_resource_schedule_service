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

#ifndef RESSCHED_INTERFACES_INNERKITS_RESSCHED_CLIENT_INCLUDE_RES_TYPE_H
#define RESSCHED_INTERFACES_INNERKITS_RESSCHED_CLIENT_INCLUDE_RES_TYPE_H

#include "res_value.h"

namespace OHOS {
namespace ResourceSchedule {
namespace ResType {
enum : uint32_t {
    // first resType
    RES_TYPE_FIRST = 0,
    // screen status, value 0 means screen off, value 1 means screen on, else are invalid.
    RES_TYPE_SCREEN_STATUS = RES_TYPE_FIRST,
    // app state change event; value means app state; payload:pid,uid,bundleName
    RES_TYPE_APP_STATE_CHANGE = 1,
    // ability state change event; value means ability state; payload:pid,uid,bundleName
    RES_TYPE_ABILITY_STATE_CHANGE = 2,
    // extension state change event; value means extension state; payload:pid,uid,bundleName
    RES_TYPE_EXTENSION_STATE_CHANGE = 3,
    // process state event; value 0:created, 1:died; payload:pid,uid,bundleName
    RES_TYPE_PROCESS_STATE_CHANGE = 4,
    // window focused event; value 0:focused, 1:unfocused; payload:pid,uid,bundleName,windowId,windowType,displayId
    RES_TYPE_WINDOW_FOCUS = 5,
    // transient task event; value 0:start, 1:finish; payload:pid,uid,bundleName
    RES_TYPE_TRANSIENT_TASK = 6,
    // continuous task event; value 0:start, 1:finish; payload:pid,uid,abilityName
    RES_TYPE_CONTINUOUS_TASK = 7,
    /* cgroup change event;
     * value 0:set thread group sched; 1:set thread sched
     * payload: pid,uid,name,oldGroup,newGroup
     */
    RES_TYPE_CGROUP_ADJUSTER = 8,
    // ace gestures click_recognizer; value 1 touch event, value 2 click event
    RES_TYPE_CLICK_RECOGNIZE = 9,
    // ace pipeline_context.cpp::PushPage(); value 0: push_page_start, 1: push_page_complete
    RES_TYPE_PUSH_PAGE = 10,
    // ace slide event recognize; value 1: list fling on; 0: list fling off; 3: slide normal begin, 4: slide normal end.
    RES_TYPE_SLIDE_RECOGNIZE = 11,
    // window visibility change, value 1: isVisible, 0: not isVisible, payload:pid,uid,windowId,windowType
    RES_TYPE_WINDOW_VISIBILITY_CHANGE = 12,
    // report mmi_service service; value mmi_service tid; payload:uid,pid
    RES_TYPE_REPORT_MMI_PROCESS = 13,
    // report render thread; value render tid; payload:uid,pid
    RES_TYPE_REPORT_RENDER_THREAD = 14,
    // app install and uninstall event; value 0: uninstall, 1: install; payload:uid,bundleName
    RES_TYPE_APP_INSTALL_UNINSTALL = 15,
    // net connect state; value 0:unknow, 1:idle, 2:connecting, 3:connected, 4:disconnecting, 5:disconnected
    RES_TYPE_WIFI_CONNECT_STATE_CHANGE = 16,
    // user switch, value means the userId which switch to.
    RES_TYPE_USER_SWITCH = 17,
    // user remove, value means the userId which is removed.
    RES_TYPE_USER_REMOVE = 18,
    // screen unlock, value 0 means unlock, value 1 means locked.
    RES_TYPE_SCREEN_LOCK = 19,
    // bluetooth a2dp connect state, value 1: connected, 3: disconnected.
    RES_TYPE_BLUETOOTH_A2DP_CONNECT_STATE_CHANGE = 20,
    // network latency request, value 0: low latency, 1: normal latency, payload: identity
    RES_TYPE_NETWORK_LATENCY_REQUEST = 21,
    // call state update; value 0 : active, value 1 : holding, value 2 : dialing, value 3 : alerting,
    // value 4 : incoming, value 5 : waiting, value 6 : disconnected, value 7 : disconnecting, value 8 : idle
    RES_TYPE_CALL_STATE_UPDATE = 22,
    // Other process modify the thread Qos, value for client name id, payload: tid : qos , clientpid, pid
    RES_TYPE_THREAD_QOS_CHANGE = 23,
    // audio render state change; value -1 : RENDERER_INVALID, value 0 : RENDERER_NEW, value 1 : RENDERER_PREPARED,
    // value 2 : RENDERER_RUNNING, value 3 : RENDERER_STOPPED, value 4 : RENDERER_RELEASED, value 5 : RENDERER_PAUSED
    // payload:uid,sessionId
    RES_TYPE_AUDIO_RENDER_STATE_CHANGE = 24,
    // audio ring mode change; value 0 : RINGER_MODE_NORMAL, value 1 : RINGER_MODE_SILENT
    // value 2 : RINGER_MODE_VIBRATE
    RES_TYPE_AUDIO_RING_MODE_CHANGE = 25,
    RES_TYPE_AUDIO_VOLUME_KEY_CHANGE = 26,
    // app_ability_start_event : a very early app startup stub, when the app process has not been pulled up.
    // stub position : ability_runtime ability_manager_service.cpp::StartAbilityInner().
    // specifically : ability type is PAGE and ability launchMode is not SPECIFIED.
    RES_TYPE_APP_ABILITY_START = 27,
    // ace pipeline_context.cpp::PopPage(); value means nothing
    RES_TYPE_POP_PAGE = 28,
    // ace web gesture event recognize; value means nothing
    RES_TYPE_WEB_GESTURE = 29,
    // info from msdp, means device still state change(enter or exit)
    RES_TYPE_DEVICE_STILL_STATE_CHANGE = 30,
    // window resize event; value 0: resize window , value 1: stop resizing window.
    RES_TYPE_RESIZE_WINDOW = 31,
    // window move event; value 0: move window, value 1: stop moving window.
    RES_TYPE_MOVE_WINDOW = 32,
    // animation transition event; value 0: animation begin, value 1: animation end.
    RES_TYPE_SHOW_REMOTE_ANIMATION = 33,
    // load page; value 0: load page begin, value 1: load page end.
    RES_TYPE_LOAD_PAGE = 34,
    // report camera state, value 0: camera connect; value 1: camera disconnect
    RES_TYPE_REPORT_CAMERA_STATE = 35,
    // report runningLock state, value 0: runningLock disable; value 1: runningLock enable;
    // value 2: runningLock is proxied; value 3: runningLock is not proxied
    RES_TYPE_RUNNINGLOCK_STATE = 36,
    // drag status bar event; value 0: start drag, value 1: stop drag.
    RES_TYPE_DRAG_STATUS_BAR = 37,
    // report SceneBoard service, value pid; payload:uid, main_tid, bundleName
    RES_TYPE_REPORT_SCENE_BOARD = 38,
    // report key thread for render, value 0 add key Thread, 1 remove key thread, payload: uid, pid, tid, role
    RES_TYPE_REPORT_KEY_THREAD = 39,
    // report window state, value 0 add 1 remove, payload: uid, pid, windowId, serialNum state: 0 active 1 inactive
    RES_TYPE_REPORT_WINDOW_STATE = 40,
    // report scene sched, value 0 scene on 1 scene off, payload: uid, sceneId
    RES_TYPE_REPORT_SCENE_SCHED = 41,
    // web gesture move event; value 0: web gesture move start, value 1: web gesture move end.
    RES_TYPE_WEB_GESTURE_MOVE = 42,
    // web slide normal event; value 0: web slide normal start, value 1: web slide normal end.
    RES_TYPE_WEB_SLIDE_NORMAL = 43,
    // load url event; value means nothing.
    RES_TYPE_LOAD_URL = 44,
    // mousewheel event; value means nothing.
    RES_TYPE_MOUSEWHEEL = 45,
    // report webview audio status, value 0 start 1 stop, payload: uid, pid, tid
    RES_TYPE_WEBVIEW_AUDIO_STATUS_CHANGE = 46,
    // report entering the multimodalinput state, value means subscribed multimodalinput state.
    RES_TYPE_MMI_INPUT_STATE = 47,
    // report anco load protect file config
    RES_TYPE_ANCO_CUST = 48,
    // report start uiextension proc
    RES_TYPE_START_UIEXTENSION_PROC = 49,
    // report time zone changed
    RES_TYPE_TIMEZONE_CHANGED = 50,
    // report connection status
    RES_TYPE_CONNECTION_OBSERVER = 51,
    // report av session create event
    RES_TYPE_AV_SESSION_ON_SESSION_CREATE = 52,
    // report av session release event
    RES_TYPE_AV_SESSION_ON_SESSION_RELEASE = 53,
    // report av session change event
    RES_TYPE_AV_SESSION_ON_TOP_SESSION_CHANGE = 54,
    // report OnAppStateChanged event
    RES_TYPE_ON_APP_STATE_CHANGED = 55,
    // report efficiency resources event
    RES_TYPE_EFFICIENCY_RESOURCES_STATE_CHANGED = 56,
    // report av_codec state, value 0:codec start info, value 1:codec stop info.
    RES_TYPE_AV_CODEC_STATE = 57,
    // 58-64 value do not change
    RES_TYPE_NITZ_TIME_CHANGED = 58,
    RES_TYPE_TIME_CHANGED = 59,
    RES_TYPE_NITZ_TIMEZONE_CHANGED = 60,
    RES_TYPE_CHARGING_DISCHARGING = 61,
    RES_TYPE_USB_DEVICE = 62,
    RES_TYPE_CALL_STATE_CHANGED = 63,
    RES_TYPE_WIFI_P2P_STATE_CHANGED = 64,

    // last resType
    //report app associated start to performance radar
    RES_TYPE_APP_ASSOCIATED_START = 65,
    //report thermal state event
    RES_TYPE_THERMAL_STATE = 66,
    // socperf event begin
    RES_TYPE_SOCPERF_CUST_EVENT_BEGIN = 67,
    // socperf event end
    RES_TYPE_SOCPERF_CUST_EVENT_END = 68,
    // window drawing content change event
    RES_TYPE_WINDOW_DRAWING_CONTENT_CHANGE = 69,

    // report vediuo capture start or stop
    RES_TYPE_REPORT_SCREEN_CAPTURE = 70,
    // report long frame, value 0 scene on 1 scene off.
    RES_TYPE_LONG_FRAME = 71,
    // report key perf scene, value 0 enter scene 1 exit scene.
    RES_TYPE_KEY_PERF_SCENE = 72,
    // report super launcher state
    RES_TYPE_SUPER_LAUNCHER = 73,
    // report cast screen state
    RES_TYPE_CAST_SCREEN = 74,
    // report screen collabroation state
    RES_TYPR_SCREEN_COLLABROATION = 75,
    // report sa control app event, eg: start app or stop app
    RES_TYPE_SA_CONTROL_APP_EVENT = 76,
    // report CPU load of the entire machine，payload:cpuPercent
    RES_TYPE_SYSTEM_CPU_LOAD = 77,
    // report download or upload scene,value 0 enter scene 1 exit scene.
    RES_TYPE_UPLOAD_DOWNLOAD = 78,
    // report main screen split，value 0 enter scene 1 exit scene.
    RES_TYPE_SPLIT_SCREEN = 79,
    // report main screen floating window，value 0 enter scene 1 exit scene.
    RES_TYPE_FLOATING_WINDOW = 80,
    // report event distribute tid, value tid, payload uid, pid.
    RES_TYPE_REPORT_DISTRIBUTE_TID = 81,
    // report frame rate
    RES_TYPE_FRAME_RATE_REPORT = 82,

    // report web screen capture, value 0 start, value 1 stop
    RES_TYPE_WEBVIEW_SCREEN_CAPTURE = 83,
    // report video state. value 0 start playing, value 1 stop playing
    RES_TYPE_WEBVIEW_VIDEO_STATUS_CHANGE = 84,
    // report location status. value 0 app status, value 1 switch status
    RES_TYPE_LOCATION_STATUS_CHANGE = 85,
    // report formCard render start/end
    RES_TYPE_FORM_RENDER_EVENT = 86,
    // report display mode event, value status, payload mode.
    RES_TYPE_DEVICE_MODE_STATUS = 88,
    // report distribute component change
    RES_TYPE_REPORT_DISTRIBUTE_COMPONENT_CHANGE = 89,
    // report web drag resize
    RES_TYPE_WEB_DRAG_RESIZE = 90,
    // report formCard create start/end
    RES_TYPE_FORM_STATE_CHANGE_EVENT = 91,
    // report power mode changed
    RES_TYPE_POWER_MODE_CHANGED = 92,
    // report thermal scenario changed
    RES_TYPE_THERMAL_SCENARIO_REPORT = 93,
    // report boot completed
    RES_TYPE_BOOT_COMPLETED = 94,
    // report continuous application startup
    RES_TYPE_CONTINUOUS_STARTUP = 95,
    // report account activating
    RES_TYPE_ACCOUNT_ACTIVATING = 96,
    // report silent playback in renderer
    RES_TYPE_AUDIO_RENDERER_SILENT_PLAYBACK = 97,
    // report continuous application install
    RES_TYPE_CONTINUOUS_INSTALL = 98,
    // report system updated
    RES_TYPE_FIRST_BOOT_AFTER_SYSTEM_UPGRADE = 99,
    // report background perceivable scene
    RES_TYPE_BACKGROUND_PERCEIVABLE_SCENE = 100,
    // user interaction scene
    RES_TYPE_USER_INTERACTION_SCENE = 101,
    // report game sched, value 0 start, value 1 stop
    RES_TYPE_REPORT_GAME_SCHED = 102,
    // report send frame event
    RES_TYPE_SEND_FRAME_EVENT = 103,
    // report app cold start only perf, value 0 start, value 1 stop
    RES_TYPE_ONLY_PERF_APP_COLD_START = 104,
    // report scene rotation, value 0 start, value 1 stop
    RES_TYPE_SCENE_ROTATION = 105,
    // report event vsync tid, value tid, payload pid, tid, uid.
    RES_TYPE_REPORT_VSYNC_TID = 106,
    // report cloud config update
    RES_TYPE_CLOUD_CONFIG_UPDATE = 107,
    // system ability status change
    RES_TYPE_SYSTEM_ABILITY_STATUS_CHANGE = 108,
    // mmi status change
    RES_TYPE_MMI_STATUS_CHANGE = 109,
    // report media output device change
    RES_TYPE_OUTPUT_DEVICE_CHANGE = 110,
    // report device idle
    RES_TYPE_DEVICE_IDLE = 111,
    // report media output device change
    RES_TYPE_BT_SERVICE_EVENT = 112,
    // report to hisi, let bmm_report begin change
    RES_TYPE_BMM_MONITER_CHANGE_EVENT = 113,
    // app frame drop event
    RES_TYPE_APP_FRAME_DROP = 114,
    // report rss cloud config update
    RES_TYPE_RSS_CLOUD_CONFIG_UPDATE = 115,
    // report game info
    RES_TYPE_GAME_INFO_NOTIFY = 116,
    // report power key down
    RES_TYPE_MMI_INPUT_POWER_KEY = 117,
    // report application stopped
    RES_TYPE_APP_STOPPED = 118,
    // report audio session enter standby
    RES_TYPE_AUDIO_RENDERER_STANDBY = 119,
    // game boost event
    RES_TYPE_APP_GAME_BOOST_EVENT = 120,
    // show gesture animation from scb
    RES_TYPE_GESTURE_ANIMATION = 121,
    // report key touch event
    RES_TYPE_KEY_EVENT = 122,
    // report axis event, value 0: start, value 1: stop value 2: update
    RES_TYPE_AXIS_EVENT = 123,
    // get game scene information
    RES_TYPE_GET_GAME_SCENE_INFO = 124,
    // report multi screen event
    RES_TYPE_DISPLAY_MULTI_SCREEN = 125,
    // intent start app
    RES_TYPE_INTENT_CTRL_APP = 126,
    // report web slide scroll
    RES_TYPE_WEB_SLIDE_SCROLL = 127,
    // crown rotation value 0:start, value 2:end
    RES_TYPE_CROWN_ROTATION_STATUS = 129,
    // report red envelope scene
    RES_TYPE_RED_ENVELOPE = 130,
    // report Date Share scene
    RES_TYPE_DATE_SHARE = 131,
    // report HiCar scene
    RES_TYPE_HICAR = 132,
    // Receving abc load event completed
    RES_TYPE_RECV_ABC_LOAD_COMPLETED = 133,
    // restype use by BackgroundManager API to set or reset background process priority
    RES_TYPE_SET_BACKGROUND_PROCESS_PRIORITY = 134,
    // report pip start 1 is enter 0 is exit
    RES_TRPE_PIP_STATUS = 135,
    // frame report from render service
    RES_TYPE_FRAME_RATE_REPORT_FROM_RS = 136,
    // report short term load scene 0 is start 1 is end
    RES_TYPE_SHORT_TERM_LOAD = 137,
    // report file copy status
    RES_TYPE_FILE_COPY_STATUS = 139,
    // report page transition
    RES_TYPE_PAGE_TRANSITION = 140,
    // report audio scene
    RES_TYPE_AUDIO_SCENE_CHANGE = 141,
    // report voice recognize wake scene
    RES_TYPE_VOICE_RECOGNIZE_WAKE = 142,
    // report modem high power abnormal
    RES_TYPE_MODEM_PA_HIGH_POWER_ABNORMAL = 143,
    // report param update event
    RES_TYPE_PARAM_UPDATE_EVENT = 144,
    // [phased out soon] report camera prelaunch event
    RES_TYPE_CAMERA_PRELAUNCH = 145,
    // report high frequence load scene without aggregation
    RES_TYPE_HIGH_FREQUENCE_LOAD_SCENE_WITHOUT_AGGREGATION = 146,
    // camera status changed
    RES_TYPE_CAMERA_STATUS_CHANGE = 147,
    // report game state change
    RES_TYPE_REPORT_GAME_STATE_CHANGE = 148,
    // cust action
    RES_TYPE_SOCPERF_CUST_ACTION = 149,
    // recent bulid resType
    RES_TYPE_RECENT_BUILD = 150,
    // overlay event
    RES_TYPE_OVERLAY_EVENT = 151,
    // report game state change
    RES_TYPE_DEVICE_ORIENTATION_STATUS = 152,
    // report wifi power state
    RES_TYPE_WIFI_POWER_STATE_CHANGE = 153,
    // report inner audio state
    RES_TYPE_INNER_AUDIO_STATE = 154,
    // report battery status change
    RES_TYPE_REPORT_BATTERY_STATUS_CHANGE = 155,
    // ability or page switch event
    RES_TYPE_ABILITY_OR_PAGE_SWITCH = 156,
    // game with suspend mode
    RES_TRPE_GAME_SUSPEND_MODE = 157,
    // report task center start app or close app, value 0: start app, value 1: close app
    RES_TYPE_APP_OPT_FROM_RECENT = 158,
    // report three finger swipe direction up, value 0: enter task center, value 1:back to the desktop
    RES_TYPE_SWIPE_DIRECTION_UP = 159,
    // report window panel, value 0: minimize, value 1: maxmize, value 2: recovery
    RES_TYPE_WINDOW_PANEL = 160,
    // report boker gatt connect
    RES_TYPE_REPORT_BOKER_GATT_CONNECT = 161,
    // report main screen split，value 0 enter scene 1 exit scene.
    RES_TYPE_WINDOW_SPLIT_SCREEN = 162,
    // report sched mode change
    RES_TYPE_SCHED_MODE_CHANGE = 163,
    // report supply level
    RES_TYPE_SUPPLY_LEVEL = 164,
    // reset camera rss priority
    RES_TYPE_CAMERA_RESET_PRIORITY = 165,
    // report gc qos status change
    RES_TYPE_GC_THREAD_QOS_STATUS_CHANGE = 166,
    //report device idle status changed
    RES_TYPE_DEVICE_IDLE_CHANGED = 167,
    //report user sleep status changed
    RES_TYPE_USER_SLEEP_STATE_CHANGED = 168,
    // report user not care charge sleep
    RES_TYPE_USER_NOT_CARE_CHARGE_SLEEP = 169,
    // report app high power consumption
    RES_TYPE_APP_HIGH_POWER_CONSUMPTION = 170,
    // set protectlru reclaim ratio
    RES_TYPE_ADJUST_PROTECTLRU_RECLAIM_RATIO = 171,
    // report kernel standby failed
    RES_TYPE_STANDBY_FREEZE_FAILED = 172,
    // report process pre foreground state change
    RES_TYPE_PROCESS_PRE_FOREGROUND_CHANGE = 173,
    // Turbo Mode
    RES_TYPE_TDP_TURBO = 174,
    // report background system analyze status
    RES_TYPE_BACKGROUND_STATUS = 175,
    // report camera status [mutex used]
    RES_TYPE_CAMERA_STATUS_CHANGED = 176,
    // report to change camera status
    RES_TYPE_MEM_OFF_PEAK = 177,
    // nap mode
    RES_TYPE_NAP_MODE = 178,
    // report audio capturer status
    RES_TYPE_AUDIO_CAPTURE_STATUS_CHANGED = 179,
    // report codec encode status
    RES_TYPE_CODEC_ENCODE_STATUS_CHANGED = 180,
    // report camera lens status [HFLS used]
    RES_TYPE_CAMERA_LENS_STATUS_CHANGED = 181,
    // sa pull app identifier
    RES_TYPE_SA_PULL_APP_IDENTIFIER = 182,
    // swiperfling end exception flag
    RES_TYPE_SWIPER_FLING_END_EXCEPTION_FLAG = 183,
    // report backpressed event
    RES_TYPE_BACKPRESSED_EVENT = 184,
    // gc start or stop
    RES_TYPE_GC_EVENT = 185,
    // report start input method process
    RES_TYPE_START_INPUT_METHOD_PROCESS = 186,
    // report page change
    RES_TYPE_PAGE_CHANGE = 187,
    // media ctrl playback event
    RES_TYPE_MEDIA_CTRL_EVENT = 189,
    // report inputmethod qos change
    RES_TYPE_IME_QOS_CHANGE = 190,
    // report live view event, value 1 enter live view, value 0 exit live view, payload uid, eventName.
    RES_TYPE_LIVE_VIEW_EVENT = 191,
    // report high frequence load scene with aggregation to fix the MultiInstance issue of application
    RES_TYPE_HIGH_FREQUENCE_LOAD_SCENE_WITH_AGGREGATION = 192,
    // audio focus change event
    RES_TYPE_AUDIO_FOCUS_CHANGE_EVENT  = 193,
    // observe manager status change
    RES_TYPE_OBSERVER_MANAGER_STATUS_CHANGE  = 194,
    // web subwin call start or stop
    RES_TYPE_WEB_SUBWIN_TASK  = 195,
    // report data clone state
    RES_TYPE_DATA_CLONE_STATE = 196,
    // last async resType
    ASYNC_RES_TYPE_LAST,
    // first sync resType
    SYNC_RES_TYPE_FIRST = ASYNC_RES_TYPE_LAST,
    // thaw one application
    SYNC_RES_TYPE_THAW_ONE_APP,
    // get all suspend state
    SYNC_RES_TYPE_GET_ALL_SUSPEND_STATE,
    // get thermal data
    SYNC_RES_TYPE_GET_THERMAL_DATA,
    // check shoul force kill process,
    SYNC_RES_TYPE_SHOULD_FORCE_KILL_PROCESS,
    // get nweb preload process set
    SYNC_RES_TYPE_GET_NWEB_PRELOAD_SET,
    // report preload application
    RES_TYPE_PRELOAD_APPLICATION,
    // get the applicaiton type
    SYNC_RES_TYPE_GET_APP_TYPE,
    // report to request mutex status
    SYNC_RES_TYPE_REQUEST_MUTEX_STATUS = 500,
    // report to check mutex before start
    SYNC_RES_TYPE_CHECK_MUTEX_BEFORE_START = 501,
    // report cosmic cube info
    RES_TYPE_COSMIC_CUBE_STATE_CHANGE = 502,
    // report SceneBoard raise worker thread priority. value 0: begin, value 1: end.
    RES_TYPE_RAISE_WORKER_THREAD_PRIORITY = 503,
    // report to check app is in app inner schedule list
    RES_TYPE_CHECK_APP_IS_IN_SCHEDULE_LIST,
    // report dynamically set suspend exempt info
    RES_TYPE_DYNAMICALLY_SET_SUSPEND_EXEMPT = 505,
    // get suspend state by uid
    SYNC_RES_TYPE_GET_SUSPEND_STATE_BY_UID = 506,
    // get suspend state by pid
    SYNC_RES_TYPE_GET_SUSPEND_STATE_BY_PID = 507,
    // query topn bundle usage info
    SYNC_RES_TYPE_QUERY_TOPN_BUNDLE_USAGE_INFO = 508,
    // report display power wake up, earlier than the screen-on event
    RES_TYPE_DISPLAY_POWER_WAKE_UP = 509,
    // get smart gc scene info
    SYNC_RES_TYPE_GET_SMART_GC_SCENE_INFO = 510,
    // get click ext report
    SYNC_RES_TYPE_APP_IS_IN_CLICK_REPORT_EXT_LIST = 511,
    // last sync resType
    SYNC_RES_TYPE_LAST,
    // get first frame drawn
    RES_TYPE_FIRST_FRAME_DRAWN,
    // report hardware decoding resources
    RES_TYPE_HARDWARE_DECODING_RESOURCES,
    // last resType
    RES_TYPE_LAST = SYNC_RES_TYPE_LAST,
    // user unlocked
    RES_TYPE_UESER_UNLOCKED,
    // usage topn
    RES_TYPE_TOPN_USAGE_INFO,
    // load kill reason config
    RES_TYPE_LOAD_KILL_REASON_CONFIG = 600,
};
} // namespace ResType
} // namespace ResourceSchedule
} // namespace OHOS

#endif // RESSCHED_INTERFACES_INNERKITS_RESSCHED_CLIENT_INCLUDE_RES_TYPE_H
