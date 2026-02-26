/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef RESSCHED_INTERFACES_INNERKITS_RESSCHED_CLIENT_INCLUDE_RES_VALUE_H
#define RESSCHED_INTERFACES_INNERKITS_RESSCHED_CLIENT_INCLUDE_RES_VALUE_H

#include <cstdint>
#include <vector>

namespace OHOS {
namespace ResourceSchedule {
namespace ResType {
/**
* @brief SystemAbility status
*/
enum SystemAbilityStatus : int64_t {
    SA_REMOVE = 0,
    SA_ADD = 1,
};

enum EventType : uint32_t {
    EVENT_START = 0,
    EVENT_DRAW_FRAME_REPORT = 1,
    EVENT_MUTEX_STATUS = 2,
    EVENT_DDR_BOUND_CHANGE_REPORT = 3,
    EVENT_MUTEX_STATUS_BEFORE_START = 4,
    EVENT_APP_STATE_BACKGROUND_FOREGROUND = 5,
    EVENT_RED_ENVELOPE = 6,
    EVENT_FRAME_RATE_STATISTICS = 7,
    EVENT_MUTEX_CASE_SMALL_WINDOW_TO_BACKGROUND = 8,
    EVENT_MUTEX_CASE_FREE_MULTI_WINDOW_TO_BACKGROUND = 9,
    EVENT_MUTEX_TOPN_BUNDLE_USAGE_INFO = 10,
    EVENT_REPORT_NATIVE_PROC_SUSPEND_STATE = 11,
    EVENT_REPORT_HFLS_LIVE_SCENE_CHANGED = 12,
    EVENT_LIMITED_EXECUTION = 14,
    EVENT_RECOVER_EXECUTION = 15,
    EVENT_SMART_GC_EVENT = 16,
    EVENT_CONTINUOUS_TASK_DETECT_STATE = 17,
    EVENT_UPDATE_KILL_REASON_CONFIG = 18,
    EVENT_END,
};

enum EventValue : uint32_t {
    EVENT_VALUE_START = 0,
    EVENT_VALUE_DRAW_FRAME_REPORT_START,
    EVENT_VALUE_DRAW_FRAME_REPORT_STOP,
    EVENT_VALUE_DDR_BOUND_REPORT_START,
    EVENT_VALUE_DDR_BOUND_REPORT_STOP,
    EVENT_VALUE_FRAME_RATE_STATISTICS_START,
    EVENT_VALUE_FRAME_RATE_STATISTICS_END,
    EVENT_VALUE_FRAME_RATE_STATISTICS_BREAK,
    EVENT_VALUE_HFLS_BEGIN = 8,
    EVENT_VALUE_HFLS_END = 9,
    EVENT_VALUE_END,
};

enum EventListenerGroup : uint32_t {
    LISTENER_GROUP_BEGIN = 0,
    LISTENER_GROUP_COMMON = 1,
    LISTENER_GROUP_LLM = 2,
    LISTENER_GROUP_GAME = 3,
    LISTENER_GROUP_CAMERA = 4,
    LISTENER_GROUP_DH = 5,
    LISTENER_GROUP_KIDDO = 6,
    LISTENER_GROUP_END
};

enum ContinuousTaskState : uint32_t {
    AUDIO_RESUME = 0,
    AUDIO_DETECT_FAIL,
};

enum AppFrameDropType : int32_t {
    JANK_FRAME_APP = 1,    // 通用丢帧
    INTERACTION_APP_JANK,  // 动效丢帧
};

enum WindowPanelType : int32_t {
    MINIMIZE = 0,
    MAXIMIZE,
    RECOVERY,
};

enum OptFromRecentType : int32_t {
    START_APP = 0,
    CLOSE_APP,
};

enum SwipeDirectionUpType : int32_t {
    SWIPE_DIRECTION_ENTER_TASK_CENTER = 0,
    SWIPE_DIRECTION_EXIT_TASK_CENTER,
};

enum GcEventType : int64_t {
    GC_START = 0,
    GC_STOP,
};

/**
* @brief Screen status
*/
enum ScreenStatus : int64_t {
    SCREEN_OFF = 0,
    SCREEN_ON = 1,
};

/**
* @brief WakeUp Screen status
*/
enum WakeUpStatus : int64_t {
    WAKE_UP_TIMEOUT = 0,
    WAKE_UP_START = 1,
};

/**
* @brief App install status
*/
enum AppInstallStatus : int64_t {
    APP_UNINSTALL = 0,
    APP_INSTALL_END = 1,
    APP_CHANGED = 2,
    APP_REPLACED = 3,
    APP_FULLY_REMOVED = 4,
    BUNDLE_REMOVED = 5,
    APP_INSTALL_START = 6
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
    TRANSIENT_TASK_ERR = 2,
    APP_TRANSIENT_TASK_START = 3,
    APP_TRANSIENT_TASK_END = 4,
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
    SLIDE_EVENT_DETECTING = 2,
    SLIDE_NORMAL_BEGIN = 3,
    SLIDE_NORMAL_END = 4,
    AUTO_PLAY_ON = 5,
    AUTO_PLAY_OFF = 6,
    MOVE_EVENT_ON = 7,
    SWIPER_FLING_END = 8,
};

/**
* @brief Axis event status
*/
enum AxisEventStatus : int64_t {
    AXIS_EVENT_BEGIN = 0,
    AXIS_EVENT_END = 1,
    AXIS_EVENT_UPDATE = 2,
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
    TOUCH_EVENT_PULL_UP = 4,
    TOUCH_EVENT_DOWN_MMI = 5,
};

/**
* @brief key event type
*/
enum KeyEventType : int64_t {
    // touch down event
    KEY_EVENT_DOWN = 1,
    KEY_EVENT_UP = 2,
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
    LOAD_PAGE_NO_REQUEST_FRAME = 2,
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
    WEB_SCENE_SCAN = 1009,
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
* @brief Web subwin call status
*/
enum WebSubwinCall : int64_t {
    WEB_SUBWIN_CALL_START = 0,
    WEB_SUBWIN_CALL_STOP = 1,
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
    EXTENSION_SUSPENDED,
    EXTENSION_RESUMED,
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
    USB_DEVICE_STATE = 2,
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
* @brief short term load status
*/
enum ShortTermLoadStatus : int64_t {
    START_LOAD = 0,
    FINISH_LOAD = 1,
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
inline const std::vector<int32_t > UI_SENSITIVE_EXTENSION = {
    2, // INPUT_EXTENSION_TYPE
    255, // UNSPECIFIED_EXTENSION_TYPE
    256, // UI_EXTENSION_TYPE
    257, // HMS_ACCOUNT
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
    DEVICE_IDLE = 5,
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

/**
* @brief account activating status
*/
enum AccountActivatingStatus : int64_t {
    ACCOUNT_ACTIVATING_START = 0,
};

/**
* @brief continuous install status
*/
enum ContinuousInstallStatus : int64_t {
    START_CONTINUOUS_INSTALL = 0,
    STOP_CONTINUOUS_INSTALL = 1,
};

/**
* @brief crown rotation status
*/
enum CrownRotationStatus : int64_t {
    CROWN_ROTATION_START = 0,
    CROWN_ROTATION_END = 2,
};

enum BackgroundPerceivableStatus : int64_t {
    PERCEIVABLE_START = 0,
    PERCEIVABLE_STOP = 1,
};

enum UserInteractionScene : int64_t {
    ENTER_USER_INTERACTION_SCENE = 0,
    EXIT_USER_INTERACTION_SCENE = 1,
};

/**
* @brief game sched status
*/
enum GameSchedStatus : int64_t {
    GAME_SCHED_START = 0,
    GAME_SCHED_STOP = 1,
};

/**
* @brief device idle status
*/
enum DeviceIdleStatus : int64_t {
    DEVICE_IDLE_START = 0,
};

/**
* @brief bt service status
*/
enum BtServiceEvent : int64_t {
    GATT_CONNECT_STATE = 0,
    GATT_APP_REGISTER = 1,
    SPP_CONNECT_STATE = 2,
    GATT_DATA_TRANSFER = 3
};

/**
* @brief LargeModelScene
*/
enum LargeModelScene : int64_t {
    ENTER_LARGE_MODEL_SCENE = 0,
    EXIT_LARGE_MODEL_SCENE = 1,
};

/**
* @brief Bmm status
*/
enum BmmMoniterStatus : int64_t {
    BMM_CLOSE = 0,
    BMM_FOREGROUND = 1,
    BMM_BACKGROUND = 2
};

/**
* @brief Heavy Load Mutex Scene
*/
enum HeavyLoadMutexAddScene : int64_t {
    MUTEX_STATUS_REQUIRE = 0,
    MUTEX_STATUS_RELEASE = 1,
};

/**
* @brief Heavy Load Mutex Reasons
*/
enum HeavyLoadMutexAddReasons : int64_t {
    HeavyLoadMutexStatusAddSucc = 0,
    HeavyLoadMutexStatusAddFailByAdded = 1,
    HeavyLoadMutexStatusAddFailByMutex = 2,
    HeavyLoadMutexStatusAddFailByParams = 3,
    HeavyLoadMutexStatusCloseFailByUnopened = 4,
    HeavyLoadMutexStatusAddSuccWithLimit = 5,
    HeavyLoadMutexStatusCloseSucc = 0,
};

/**
* @brief Heavy Load Mutex Boardcast Scene
*/
enum HeavyLoadMutexSceneBoardcastScene : int64_t {
    HeavyLoadMutexBoardcastNeedProactiveAvoidance = 0,
    HeavyLoadMutexBoardcastNeedProactiveDownspeeding = 1,
    HeavyLoadMutexBoardcastNeedCancelDownspeeding = 2,
};

/**
 * @brief game boost state
 */
enum GameBoostState : int64_t {
    GAME_BOOST_LEVEL0 = 0,
    GAME_BOOST_LEVEL1 = 1,
    GAME_BOOST_LEVEL2 = 2,
    GAME_BOOST_LEVEL3 = 3,
    GAME_BOOST_END = 4,
};

/**
* @brief CosmicCube Status
*/
enum CosmicCubeState : int64_t {
    APPLICATION_ABOUT_TO_APPEAR = 0,
    APPLICATION_ABOUT_TO_HIDE = 1,
    APPLICATION_ABOUT_TO_RECOVER = 2,
};

/**
* @brief game scene Id
*/
enum GameSceneId : int64_t {
    MSG_GAME_STATE_START = 0,
    MSG_GAME_STATE_END = 1,
    MSG_GAME_ENTER_PVP_BATTLE = 2,
    MSG_GAME_EXIT_PVP_BATTLE = 3,
    MSG_GAME_STATE_FOREGROUND = 4,
    MSG_GAME_STATE_BACKGROUND = 5,
};

/**
* @brief raise worker Thread Priority
*/
enum WorkerEventState : int64_t {
    WORKER_EVENT_BEGIN = 0,
    WORKER_EVENT_END = 1,
};

/**
* @brief Wifi Connection State
*/
enum WifiConnectionState : int64_t {
    WIFI_STATE_UNKNOWN = 0,
    WIFI_STATE_IDLE = 1,
    WIFI_STATE_SCAN = 2,
    WIFI_STATE_CONNECTED = 3,
    WIFI_STATE_DISCONNECTING = 4,
    WIFI_STATE_DISCONNECTED = 5,
};

/**
* @brief Frame Report from Rs
*/
enum FrameRateReportState : int64_t {
    FRAME_RATE_COMMON_REPORT = 0,
};

/**
* @brief File copy status
*/
enum CopyStatus : uint64_t {
    COPY_STOP = 0,
    COPY_START = 1,
    COPY_STATUS_MAX,
};

/**
* @brief High Frequence Load Scene State
*/
enum HighFrequenceLoadSceneState : uint64_t {
    HFLS_SCENE_IN = 0,
    HFLS_SCENE_OUT = 1,
};

/**
* @brief Camera Status Change
*/
enum CameraUsingStatus : uint64_t {
    CAMERA_STATUS_APPEAR = 0,
    CAMERA_STATUS_DISAPPEAR = 1,
    CAMERA_STATUS_AVAILABLE = 2,
    CAMERA_STATUS_UNAVAILABLE = 3,
    CAMERA_SERVER_UNAVAILABLE = 4,
};

/**
* @brief Game State Change
*/
enum GameState : uint64_t {
    GAME_FOREGROUND_STATE = 1,
    GAME_BACKGROUND_STATE = 2,
    GAME_EXIT_STATE = 3,
    GAME_GEE_FOCUS_STATE = 4,
    GAME_LOST_FOCUS_STATE = 5,
};

/**
* @brief Game Env Type
*/
enum GameEnvType : uint64_t {
    GAME_NORMAL = 0,
    GAME_CUST = 1,
};

/**
* @brief Recent Build Status
*/
enum RecentBuildStatus : int64_t {
    RECENT_BUILD_START = 0,
    RECENT_BUILD_STOP = 1,
};

/**
* @brief Overlay Status
*/
enum OverlayStatus : int64_t {
    OVERLAY_ADD = 0,
    OVERLAY_REMOVE = 1,
};

/**
* @brief Inner Audio State
*/
enum InnerAudioState : int64_t {
    AUDIO_STATE_RUNNING = 0,
    AUDIO_STATE_STOP = 1,
};

/**
 * @brief Sched mode state
 */
enum SchedMode : int32_t {
    UNKNOWN_MODE = -1,
    NORMAL_MODE = 0,
    PERF_MODE = 1,
};

/**
 * @brief game suspend mode state
 */
enum GameSuspendModeState : int64_t {
    SET_GAME_SUSPEND_MODE,
    CANCEL_GAME_SUSPEND_MODE,
};

/**
* @brief Turbo Mode state
*/
enum TurboModeState : int64_t {
    TURBO_MODE_OFF = 0,
    TURBO_MODE_ON = 1,
};

/**
* @brief Native process state
*/
enum NativeSuspendState : int32_t {
    NATIVE_THAW = 0,
    NATIVE_FREEZE = 1,
};

/**
* @brief audio capture state
*/
enum AudioCaptureState : int32_t {
    AUDIO_CAPTURE_BEGIN = 0,
    AUDIO_CAPTURE_END = 1,
    AUDIO_CAPTURE_CHANGE = 2,
};

/**
* @brief codec encode state
*/
enum CodecEncodeState : int32_t {
    CODEC_ENCODE_BEGIN = 0,
    CODEC_ENCODE_END = 1,
};

/**
* @brief camera len state
*/
enum CameraLensState : int32_t {
    CAMERA_LEN_OPENED = 0,
    CAMERA_LEN_CLOSED = 1,
};

/**
* @brief inputmethod qos state
*/
enum ImeQosState : int64_t {
    IME_START_UP = 0,
    IME_START_FINISH = 1,
};

/**
* @brief live view state
*/
enum LiveViewState : int32_t {
    LIVE_VIEW_EXIT = 0,
    LIVE_VIEW_ENTER = 1,
};

/**
* @brief system ability observe manager status change
*/
enum SystemAbilitySign : int64_t {
    ADD_SYSTEM_ABILITY  = 0,
    REMOVE_SYSTEM_ABILITY  = 1,
};

/**
* @brief data clone state
*/
enum DataCloneState : int64_t {
    CLONE_END = 0,
    NEW_DEVICE_CLONE_START = 1,
    OLD_DEVICE_CLONE_START = 2,
};
} // namespace ResType
} // namespace ResourceSchedule
} // namespace OHOS

#endif // RESSCHED_INTERFACES_INNERKITS_RESSCHED_CLIENT_INCLUDE_RES_VALUE_H