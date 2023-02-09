/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#include <map>
#include <string>

namespace OHOS {
namespace ResourceSchedule {
namespace ResType {
enum : uint32_t {
    // screen status, value 0 means screen off, value 1 means screen on, else are invalid.
    RES_TYPE_SCREEN_STATUS = 0,
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
    // ace slide event recognize; value 1: slide on; 0: slide off
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
    // audio volume key change; value volume, payload volumeType eg.
    // type -1 : STREAM_DEFAULT, type 0 : STREAM_VOICE_CALL, type 1 : STREAM_MUSIC, type 2 : STREAM_RING,
    // type 3 : STREAM_MEDIA, type 4 : STREAM_VOICE_ASSISTANT,  type 5 : STREAM_SYSTEM, type 6 : STREAM_ALARM,
    // type 7 : STREAM_NOTIFICATION, type 8 : STREAM_BLUETOOTH_SCO, type 9 : STREAM_ENFORCED_AUDIBLE,
    // type 10 : STREAM_DTMF, type 11 : STREAM_TTS, type 12 : STREAM_ACCESSIBILITY, type 13 : STREAM_RECORDING
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
    // slide normal event; value 0: slide normal begin, value 1: slide normal end.
    RES_TYPE_SLIDE_NORMAL = 34,
};

static const std::map<uint32_t, std::string> resTypeToStr = {
    { RES_TYPE_SCREEN_STATUS, "SCREEN_STATUS" },
    { RES_TYPE_APP_STATE_CHANGE, "APP_STATE_CHANGE" },
    { RES_TYPE_ABILITY_STATE_CHANGE, "ABILITY_STATE_CHANGE" },
    { RES_TYPE_EXTENSION_STATE_CHANGE, "EXTENSION_STATE_CHANGE" },
    { RES_TYPE_PROCESS_STATE_CHANGE, "PROCESS_STATE_CHANGE" },
    { RES_TYPE_WINDOW_FOCUS, "WINDOW_FOCUS" },
    { RES_TYPE_TRANSIENT_TASK, "TRANSIENT_TASK" },
    { RES_TYPE_CONTINUOUS_TASK, "CONTINUOUS_TASK" },
    { RES_TYPE_CGROUP_ADJUSTER, "CGROUP_ADJUSTER" },
    { RES_TYPE_CLICK_RECOGNIZE, "CLICK_RECOGNIZE" },
    { RES_TYPE_PUSH_PAGE, "PUSH_PAGE" },
    { RES_TYPE_SLIDE_RECOGNIZE, "SLIDE_RECOGNIZE" },
    { RES_TYPE_WINDOW_VISIBILITY_CHANGE, "WINDOW_VISIBILITY_CHANGE" },
    { RES_TYPE_REPORT_MMI_PROCESS, "REPORT_MMI_PROCESS" },
    { RES_TYPE_REPORT_RENDER_THREAD, "REPORT_RENDER_THREAD" },
    { RES_TYPE_APP_INSTALL_UNINSTALL, "APP_INSTALL_UNINSTALL" },
    { RES_TYPE_WIFI_CONNECT_STATE_CHANGE, "WIFI_CONNECT_STATE_CHANGE" },
    { RES_TYPE_USER_SWITCH, "USER_SWITCH" },
    { RES_TYPE_USER_REMOVE, "USER_REMOVE" },
    { RES_TYPE_SCREEN_LOCK, "SCREEN_LOCK" },
    { RES_TYPE_BLUETOOTH_A2DP_CONNECT_STATE_CHANGE, "BLUETOOTH_A2DP_CONNECT_STATE_CHANGE" },
    { RES_TYPE_NETWORK_LATENCY_REQUEST, "NETWORK_LATENCY_REQUEST" },
    { RES_TYPE_CALL_STATE_UPDATE, "CALL_STATE_UPDATE" },
    { RES_TYPE_THREAD_QOS_CHANGE, "THREAD_QOS_CHANGE" },
    { RES_TYPE_AUDIO_RENDER_STATE_CHANGE, "AUDIO_RENDER_STATE_CHANGE" },
    { RES_TYPE_AUDIO_RING_MODE_CHANGE, "AUDIO_RING_MODE_CHANGE" },
    { RES_TYPE_AUDIO_VOLUME_KEY_CHANGE, "AUDIO_VOLUME_KEY_CHANGE" },
    { RES_TYPE_APP_ABILITY_START, "APP_ABILITY_START" },
    { RES_TYPE_POP_PAGE, "POP_PAGE" },
    { RES_TYPE_WEB_GESTURE, "WEB_GESTURE" },
    { RES_TYPE_DEVICE_STILL_STATE_CHANGE, "DEVICE_STILL_STATE_CHANGE" },
    { RES_TYPE_RESIZE_WINDOW, "RES_TYPE_RESIZE_WINDOW" },
    { RES_TYPE_MOVE_WINDOW, "RES_TYPE_MOVE_WINDOW" },
    { RES_TYPE_SHOW_REMOTE_ANIMATION, "RES_TYPE_SHOW_REMOTE_ANIMATION" },
    { RES_TYPE_SLIDE_NORMAL, "RES_TYPE_SLIDE_NORMAL" }
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
    PROCESS_DIED = 1,
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
};

/**
 * @brief Click event type
 */
enum ClickEventType : int64_t {
    INVALID_EVENT = 0,
    TOUCH_EVENT = 1,
    CLICK_EVENT = 2,
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
};

/**
 * @brief Slide normal Status
 */
enum SlideNormalStatus : int64_t {
    SLIDE_NORMAL_BEGIN = 0,
    SLIDE_NORMAL_END = 1,
};
} // namespace ResType
} // namespace ResourceSchedule
} // namespace OHOS

#endif // RESSCHED_INTERFACES_INNERKITS_RESSCHED_CLIENT_INCLUDE_RES_TYPE_H
