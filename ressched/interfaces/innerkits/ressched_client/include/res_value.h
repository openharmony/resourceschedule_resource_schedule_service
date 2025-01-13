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

enum EventType : uint32_t {
    EVENT_START = 0, EVENT_DRAW_FRAME_REPORT, EVENT_MUTEX_STATUS, EVENT_DDR_BOUND_CHANGE_REPORT = 3,
    EVENT_MUTEX_STATUS_BEFORE_START = 4, EVENT_APP_STATE_BACKGROUND_FOREGROUND = 5, EVENT_END,
};

enum EventValue : uint32_t {
    EVENT_VALUE_START = 0,
    EVENT_VALUE_DRAW_FRAME_REPORT_START,
    EVENT_VALUE_DRAW_FRAME_REPORT_STOP,
    EVENT_VALUE_DDR_BOUND_REPORT_START,
    EVENT_VALUE_DDR_BOUND_REPORT_STOP,
    EVENT_VALUE_END,
};

enum EventListenerGroup : uint32_t {
    LISTENER_GROUP_BEGIN = 0,
    LISTENER_GROUP_COMMON = 1,
    LISTENER_GROUP_LLM = 2,
    LISTENER_GROUP_GAME = 3,
    LISTENER_GROUP_CAMERA = 4,
    LISTENER_GROUP_DH = 5,
    LISTENER_GROUP_END
};

enum AppFrameDropType : int32_t {
    JANK_FRAME_APP = 1,    // 通用丢帧
    INTERACTION_APP_JANK,  // 动效丢帧
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
 * @brief app associated start type
 */
enum AssociatedStartType : int64_t {
    SCB_START_ABILITY = 0,
    EXTENSION_START_ABILITY = 1,
    MISSION_LIST_START_ABILITY = 2,
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
 * @brief av_codec state
*/
enum AvCodecState : int64_t {
    CODEC_START_INFO = 0,
    CODEC_STOP_INFO = 1,
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
 * @brief ui sensitive extension
 */
const std::vector<int32_t > UI_SENSITIVE_EXTENSION = {
    2, // INPUT_EXTENSION_TYPE
    255, // UNSPECIFIED_EXTENSION_TYPE
    256, // UI_EXTENSION_TYPE
    257, // HMS_ACCOUNT
    500 // SYS_COMMON_UI_TYPE
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
 * @brief location state
 */
enum LocationStatus : int64_t {
    APP_LOCATION_STATUE_CHANGE = 0,
    LOCATION_SWTICH_CHANGE = 1,
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
 * @brief Key Upload Or Download Scene status
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
 * @brief Device mode status
 */
enum DeviceModeStatus : int64_t {
    MODE_ENTER = 0,
    MODE_QUIT = 1,
};

/**
 * @brief Key Floating Window Scene status
 */
enum KeyFloatingWindowStatus : int64_t {
    ENTER_FLOATING_WINDOW_SCENE = 0,
    EXIT_FLOATING_WINDOW_SCENE = 1,
};

/**
 * @brief web screen capture start
 */
enum WebScreenCapture : int64_t {
    WEB_SCREEN_CAPTURE_START = 0,
    WEB_SCREEN_CAPTURE_STOP = 1,
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
 * @brief web video state
 */
enum WebVideoState : int64_t {
    WEB_VIDEO_PLAYING_START = 0,
    WEB_VIDEO_PLAYING_STOP = 1,
};

/**
 * @brief form create state
 */
enum FormCreateStatus : int64_t {
    FormCreateStart = 0,
    FormCreateEnd = 1,
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
 * @brief background perceivable status
 */
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
    SPP_CONNECT_STATE = 2
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
    GAME_BOOST_START = 0,
    GAME_BOOST_END = 1,
};

/**
 * @brief CosmicCube Status
 */
enum CosmicCubeState : int64_t {
    APPLICATION_ABOUT_TO_APPEAR = 0,
    APPLICATION_ABOUT_TO_HIDE = 1,
    APPLICATION_ABOUT_TO_RECOVER = 2,
};
} // namespace ResType
} // namespace ResourceSchedule
} // namespace OHOS

#endif // RESSCHED_INTERFACES_INNERKITS_RESSCHED_CLIENT_INCLUDE_RES_VALUE_H