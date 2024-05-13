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

#ifndef RESSCHED_INTERFACES_INNERKITS_RESSCHED_CLIENT_INCLUDE_RES_TYPE_H
#define RESSCHED_INTERFACES_INNERKITS_RESSCHED_CLIENT_INCLUDE_RES_TYPE_H

#include <cstdint>
#include <vector>

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
    // report silent playback
    RES_TYPE_AUDIO_SILENT_PLAYBACK = 86,
    // report display mode event, value status, payload mode.
    RES_TYPE_DEVICE_MODE_STATUS = 87,
    // report systemload level
    RES_TYPE_SYSTEMLOAD_LEVEL = 88,
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
    //last resType
    RES_TYPE_LAST,
};

/**
 * @brief Screen status
 */
enum ScreenStatus : int64_t {
    SCREEN_OFF = 0,
    SCREEN_ON = 1,
};

/**
 * @brief App install status
 */
enum AppInstallStatus : int64_t {
    APP_UNINSTALL = 0,
    APP_INSTALL = 1,
    APP_CHANGED = 2,
    APP_REPLACED = 3,
    APP_FULLY_REMOVED = 4,
    BUNDLE_REMOVED = 5
};

/**
 * @brief Screen lock status
 */
enum ScreenLockStatus : int64_t {
    SCREEN_UNLOCK = 0,
    SCREEN_LOCK = 1,
};

/**
 * @brief Process status
 */
enum ProcessStatus : int64_t {
    PROCESS_CREATED = 0,
    PROCESS_READY,
    PROCESS_FOREGROUND,
    PROCESS_FOCOUS,
    PROCESS_BACKGROUND,
    PROCESS_DIED,
};

/**
 * @brief Transient task status
 */
enum TransientTaskStatus : int64_t {
    TRANSIENT_TASK_START = 0,
    TRANSIENT_TASK_END = 1,
};

/**
 * @brief Continuous task status
 */
enum ContinuousTaskStatus : int64_t {
    CONTINUOUS_TASK_START = 0,
    CONTINUOUS_TASK_END = 1,
    CONTINUOUS_TASK_UPDATE = 2,
};

/**
 * @brief efficiency resources status
 */
enum EfficiencyResourcesStatus : int64_t {
    APP_EFFICIENCY_RESOURCES_APPLY,
    APP_EFFICIENCY_RESOURCES_RESET,
    PROC_EFFICIENCY_RESOURCES_APPLY,
    PROC_EFFICIENCY_RESOURCES_RESET,
};

/**
 * @brief Window focus status
 */
enum WindowFocusStatus : int64_t {
    WINDOW_FOCUS = 0,
    WINDOW_UNFOCUS = 1,
};

/**
 * @brief Window visibility status
 */
enum WindowVisibilityStatus : int64_t {
    INVISIBLE = 0,
    VISIBLE = 1,
};

/**
 * @brief Slide event status
 */
enum SlideEventStatus : int64_t {
    SLIDE_EVENT_OFF = 0,
    SLIDE_EVENT_ON = 1,
    SLIDE_NORMAL_BEGIN = 3,
    SLIDE_NORMAL_END = 4,
};

/**
 * @brief Click event type
 */
enum ClickEventType : int64_t {
    INVALID_EVENT = 0,
    // touch down event
    TOUCH_EVENT_DOWN = 1,
    CLICK_EVENT = 2,
    TOUCH_EVENT_UP = 3,
};

/**
 * @brief PushPage event type
 */
enum PushPageType : int64_t {
    PUSH_PAGE_START = 0,
    PUSH_PAGE_COMPLETE = 1,
};

/**
 * @brief App Start type
 */
enum AppStartType : int64_t {
    APP_WARM_START = 0,
    APP_COLD_START = 1,
};

/**
 * @brief Window resize type
 */
enum WindowResizeType : int64_t {
    WINDOW_RESIZING = 0,
    WINDOW_RESIZE_STOP = 1,
};

/**
 * @brief Window move type
 */
enum WindowMoveType : int64_t {
    WINDOW_MOVING = 0,
    WINDOW_MOVE_STOP = 1,
};

/**
 * @brief Animation Change Status
 */
enum ShowRemoteAnimationStatus : int64_t {
    ANIMATION_BEGIN = 0,
    ANIMATION_END = 1,
    ANIMATION_UNLOCK_BEGIN = 2,
    ANIMATION_UNLOCK_END = 3,
};

/**
 * @brief LoadPage event type
 */
enum LoadPageType : int64_t {
    LOAD_PAGE_START = 0,
    LOAD_PAGE_COMPLETE = 1,
};

/**
 * @brief Animation change status
 */
enum StatusBarDragStatus : int64_t {
    DRAG_START = 0,
    DRAG_END = 1,
};

/**
 * @brief Report change status
 */
enum ReportChangeStatus : int64_t {
    CREATE = 0,
    REMOVE = 1,
};

/**
 * @brief Window State
 */
enum WindowStates : int64_t {
    ACTIVE = 0,
    INACTIVE = 1,
};

/**
 * @brief Thread role
 */
enum ThreadRole : int64_t {
    USER_INTERACT = 0,
    NORMAL_DISPLAY = 1,
    IMPORTANT_DISPLAY = 2,
    NORMAL_AUDIO = 3,
    IMPORTANT_AUDIO = 4,
    IMAGE_DECODE = 5,
};

/**
 * @brief scene control
 */
enum SceneControl : int64_t {
    SCENE_IN = 0,
    SCENE_OUT = 1,
};

/**
 * @brief audio status
 */
enum AudioStatus : int64_t {
    START = 0,
    STOP = 1,
    RENDERER_RUNNING  = 2,
    RENDERER_STOPPED  = 3,
    RENDERER_RELEASED = 4,
    RENDERER_PAUSED = 5,
};

/**
 * @brief web scene
 */
enum WebScene : int64_t {
    WEB_SCENE_LOAD_URL = 1001,
    WEB_SCENE_CLICK = 1002,
    WEB_SCENE_SLIDE = 1003,
    WEB_SCENE_RESIZE = 1004,
    WEB_SCENE_VISIBLE = 1005,
    WEB_SCENE_KEYBOARD_CLICK = 1006,
    WEB_SCENE_KEY_TASK = 1007,
    WEB_SCENE_IMAGE_DECODE = 1008,
};

/**
 * @brief Web gesture status
 */
enum WebGesture : int64_t {
    WEB_GESTURE_START = 0,
    WEB_GESTURE_END = 1,
};

/**
 * @brief Web gesture move status
 */
enum WebGestureMove : int64_t {
    WEB_GESTURE_MOVE_START = 0,
    WEB_GESTURE_MOVE_END = 1,
};

/**
 * @brief Web slide normal status
 */
enum WebSlideNormal : int64_t {
    WEB_SLIDE_NORMAL_START = 0,
    WEB_SLIDE_NORMAL_END = 1,
};

/**
 * @brief camera state
 */
enum CameraState : int64_t {
    CAMERA_CONNECT = 0,
    CAMERA_DISCONNECT = 1,
};

/**
 * @brief Runninglock State
 */
enum RunninglockState : int64_t {
    RUNNINGLOCK_STATE_DISABLE = 0,
    RUNNINGLOCK_STATE_ENABLE = 1,
    RUNNINGLOCK_STATE_PROXIED = 2,
    RUNNINGLOCK_STATE_UNPROXIED_RESTORE = 3,
};

/**
 * @brief anco cust config
*/
enum AncoConfig : int64_t {
    PROTECT_LRU = 0,
    DEFAULT = 1,
};

/**
 * @brief connection State
 */
enum ConnectionObserverStatus : int64_t {
    EXTENSION_CONNECTED = 0,
    EXTENSION_DISCONNECTED,
    DLP_ABILITY_OPENED,
    DLP_ABILITY_CLOSED,
};


/**
 * @brief charge status
 */
enum ChargeStatus : int64_t {
    EVENT_CHARGING = 0,
    EVENT_DISCHARGING = 1,
};

/**
 * @brief Screen lock status
 */
enum UsbDeviceStatus : int64_t {
    USB_DEVICE_ATTACHED = 0,
    USB_DEVICE_DETACHED = 1,
};

/**
 * @brief av_codec state
*/
enum AvCodecState : int64_t {
    CODEC_START_INFO = 0,
    CODEC_STOP_INFO = 1,
};

/**
 * @brief app associated start type
 */
enum AssociatedStartType : int64_t {
    SCB_START_ABILITY = 0,
    EXTENSION_START_ABILITY = 1,
    MISSION_LIST_START_ABILITY = 2,
};

/**
 * @brief thermal state type
 */
enum ThermalStateType : int64_t {
    THERMAL_LEVEL_STATE = 0,
    THERMAL_ACTION_STATE = 1,
};

/**
 * @brief Window drawing status
 */
enum WindowDrawingStatus : int64_t {
    NotDrawing = 0,
    Drawing = 1,
};

/**
 * @brief Window drawing status
 */
enum ScreenCaptureStatus : int64_t {
    START_SCREEN_CAPTURE = 0,
    STOP_SCREEN_CAPTURE = 1,
};

/**
 * @brief Key Perf Scene status
 */
enum KeyPerfStatus : int64_t {
    ENTER_SCENE = 0,
    EXIT_SCENE = 1,
};

/**
 * @brief Collabroation Service status
 */
enum CollabroationServiceStatus : int64_t {
    UNKNOWN_STATE = 0,
    IDLE = 1,
    PREPARE = 2,
    CONNECTING = 3,
    CONNECT_SUCC = 4,
};

/**
 * @brief sa control app status
 */
enum SaControlAppStatus : int64_t {
    SA_START_APP,
    SA_STOP_APP,
};

/**
 * @brief Key Download Scene status
 */
enum KeyUploadOrDownloadStatus : int64_t {
    ENTER_UPLOAD_DOWNLOAD_SCENE = 0,
    EXIT_UPLOAD_DOWNLOAD_SCENE = 1,
};


/**
 * @brief Key Split Screen Scene status
 */
enum KeySplitScreenStatus : int64_t {
    ENTER_SPLIT_SCREEN_SCENE = 0,
    EXIT_SPLIT_SCREEN_SCENE = 1,
};

/**
 * @brief Key Floating Window Scene status
 */
enum KeyFloatingWindowStatus : int64_t {
    ENTER_FLOATING_WINDOW_SCENE = 0,
    EXIT_FLOATING_WINDOW_SCENE = 1,
};

/**
 * @brief ui sensitive extension
 */
const std::vector<int32_t > UI_SENSITIVE_EXTENSION = {
    2, // INPUT_EXTENSION_TYPE
    255, // UNSPECIFIED_EXTENSION_TYPE
    256, // UI_EXTENSION_TYPE
    500 // SYS_COMMON_UI_TYPE
};

/**
 * @brief systemload level
 */
enum SystemloadLevel : int64_t {
    LOW = 0,
    NORMAL = 1,
    MEDIUM = 2,
    HIGH = 3,
    OVERHEATED = 4,
    WARNING = 5,
    EMERGENCY = 6,
    ESCAPE = 7,
};

/**
 * @brief Device status
 */
enum DeviceStatus : int64_t {
    SYSTEMLOAD_LEVEL = 0,
    POWER_FOOTAGE = 1,
    THERMAL = 2,
    MEMORY_LEVEL = 3,
    HIGH_LOAD = 4,
};

/**
 * @brief Device mode status
 */
enum DeviceModeStatus : int64_t {
    MODE_ENTER = 0,
    MODE_QUIT = 1,
};

/**
 * @brief web screen capture start
 */
enum WebScreenCapture : int64_t {
    WEB_SCREEN_CAPTURE_START = 0,
    WEB_SCREEN_CAPTURE_STOP = 1,
};

/**
 * @brief web video state
 */
enum WebVideoState : int64_t {
    WEB_VIDEO_PLAYING_START = 0,
    WEB_VIDEO_PLAYING_STOP = 1,
};

/**
 * @brief location state
 */
enum LocationStatus : int64_t {
    APP_LOCATION_STATUE_CHANGE = 0,
    LOCATION_SWTICH_CHANGE = 1,
};

/**
 * @brief form create state
 */
enum FormCreateStatus : int64_t {
    FormCreateStart = 0,
    FormCreateEnd = 1,
};

/**
 * @brief drag web window resize status
 */
enum WebDragResizeStatus : int64_t {
    WEB_DRAG_START = 0,
    WEB_DRAG_END = 1,
};

/**
 * @brief boot completed status
 */
enum BootCompletedStatus : int64_t {
    START_BOOT_COMPLETED = 0,
    STOP_BOOT_COMPLETED = 1,
};

/**
 * @brief continuous startUp status
 */
enum ContinuousStartupStatus : int64_t {
    START_CONTINUOUS_STARTUP = 0,
    STOP_CONTINUOUS_STARTUP = 1,
};

} // namespace ResType
} // namespace ResourceSchedule
} // namespace OHOS

#endif // RESSCHED_INTERFACES_INNERKITS_RESSCHED_CLIENT_INCLUDE_RES_TYPE_H
