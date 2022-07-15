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

namespace OHOS {
namespace ResourceSchedule {
namespace ResType {
enum : uint32_t {
    // screen status, value 0 means screen off, value 1 means screen on, else are invalid.
    RES_TYPE_SCREEN_STATUS,
    // app state change event; value means app state; payload:pid,uid,bundleName
    RES_TYPE_APP_STATE_CHANGE,
    // ability state change event; value means ability state; payload:pid,uid,bundleName
    RES_TYPE_ABILITY_STATE_CHANGE,
    // extension state change event; value means extension state; payload:pid,uid,bundleName
    RES_TYPE_EXTENSION_STATE_CHANGE,
    // process state event; value 0:created, 1:died; payload:pid,uid,bundleName
    RES_TYPE_PROCESS_STATE_CHANGE,
    // window focused event; value 0:focused, 1:unfocused; payload:pid,uid,bundleName,windowId,windowType,displayId
    RES_TYPE_WINDOW_FOCUS,
    // transient task event; value 0:start, 1:finish; payload:pid,uid,bundleName
    RES_TYPE_TRANSIENT_TASK,
    // continuous task event; value 0:start, 1:finish; payload:pid,uid,abilityName
    RES_TYPE_CONTINUOUS_TASK,
    /* cgroup change event;
     * value 0:set thread group sched; 1:set thread sched
     * payload: pid,uid,name,oldGroup,newGroup
     */
    RES_TYPE_CGROUP_ADJUSTER,
    // ace gestures click_recognizer; value means nothing
    RES_TYPE_CLICK_RECOGNIZE,
    // ace pipeline_context.cpp::PushPage(); value means nothing
    RES_TYPE_PUSH_PAGE,
    // ace slide event recognize; value 1: slide on; 0: slide off
    RES_TYPE_SLIDE_RECOGNIZE,
    // window visibility change, value 1: isVisible, 0: not isVisible, payload:pid,uid,windowId,windowType
    RES_TYPE_WINDOW_VISIBILITY_CHANGE,
    // report mmi_service service; value mmi_service tid; payload:uid,pid
    RES_TYPE_REPORT_MMI_PROCESS,
    // report render thread; value render tid; payload:uid,pid
    RES_TYPE_REPORT_RENDER_THREAD,
    // app install and uninstall event; value 0: uninstall, 1: install; payload:uid,bundleName
    RES_TYPE_APP_INSTALL_UNINSTALL,
    // net connect state; value 0:unknow, 1:idle, 2:connecting, 3:connected, 4:disconnecting, 5:disconnected
    RES_TYPE_WIFI_CONNECT_STATE_CHANGE,
    // user switch, value means the userId which switch to.
    RES_TYPE_USER_SWITCH,
    // user remove, value means the userId which is removed.
    RES_TYPE_USER_REMOVE,
    // screen unlock, value 0 means unlock, value 1 means locked.
    RES_TYPE_SCREEN_LOCK,
    // bluetooth a2dp connect state, value 1: connected, 3: disconnected.
    RES_TYPE_BLUETOOTH_A2DP_CONNECT_STATE_CHANGE,
    // network latency request, value 0: low latency, 1: normal latency, payload: identity
    RES_TYPE_NETWORK_LATENCY_REQUEST,
    // call state update; value 0 : active, value 1 : holding, value 2 : dialing, value 3 : alerting,
    // value 4 : incoming, value 5 : waiting, value 6 : disconnected, value 7 : disconnecting, value 8 : idle
    RES_TYPE_CALL_STATE_UPDATE,
    // Other process modify the thread Qos, value for client name id, payload: tid : qos , clientpid, pid
    RES_TYPE_THREAD_QOS_CHANGE,
    // audio render state change; value -1 : RENDERER_INVALID, value 0 : RENDERER_NEW, value 1 : RENDERER_PREPARED,
    // value 2 : RENDERER_RUNNING, value 3 : RENDERER_STOPPED, value 4 : RENDERER_RELEASED, value 5 : RENDERER_PAUSED
    // payload:uid
    RES_TYPE_AUDIO_RENDER_STATE_CHANGE,
    // audio ring mode change; value -1 : RINGER_MODE_NORMAL, value -1 : RINGER_MODE_SILENT, value -1 : RINGER_MODE_VIBRATE
    RES_TYPE_AUDIO_RING_MODE_CHANGE,
    // audio volume key change; value -1 : STREAM_DEFAULT, value 0 : STREAM_VOICE_CALL, value 1 : STREAM_MUSIC,
    // value 2 : STREAM_RING, value 3 : STREAM_MEDIA, value 4 : STREAM_VOICE_ASSISTANT,  value 5 : STREAM_SYSTEM,
    // value 6 : STREAM_ALARM, value 7 : STREAM_NOTIFICATION, value 8 : STREAM_BLUETOOTH_SCO, value 9 : STREAM_ENFORCED_AUDIBLE,
    // value 10 : STREAM_DTMF, value 11 : STREAM_TTS, value 12 : STREAM_ACCESSIBILITY, value 13 : STREAM_RECORDING,
    // payload:volumeKey
    RES_TYPE_AUDIO_VOLUME_KEY_CHANGE,
};
} // namespace ResType
} // namespace ResourceSchedule
} // namespace OHOS

#endif // RESSCHED_INTERFACES_INNERKITS_RESSCHED_CLIENT_INCLUDE_RES_TYPE_H

