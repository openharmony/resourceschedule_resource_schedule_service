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
    // cgroup change event; value means nothing; payload:pid,uid,name,oldGroup,newGroup
    RES_TYPE_CGROUP_ADJUSTER,
    // ace gestures click_recognizer; value means nothing
    RES_TYPE_CLICK_RECOGNIZE,
    // ace pipeline_context.cpp::PushPage(); value means nothing
    RES_TYPE_PUSH_PAGE,
    // ace slide event recognize; value 1: slide on; 0: slide off
    RES_TYPE_SLIDE_RECOGNIZE,
    // window visibility change, value 1: isVisible, 0: not isVisible, payload:pid,uid,windowId
    RES_TYPE_WINDOW_VISIBILITY_CHANGE,
    // window update state change; value means WindowUpdateType; payload:currentWindowType
    RES_TYPE_WINDOW_UPDATE_STATE_CHANGE,
    // report mmi_service service; value mmi_service tid; payload:uid,pid
    RES_TYPE_REPORT_MMI_PROCESS,
    // report render thread; value render tid; payload:uid,pid
    RES_TYPE_REPORT_RENDER_THREAD,
    // app install and uninstall event; value 0: install, 1: uninstall; payload:uid,bundleName
    RES_TYPE_APP_INSTALL,
    // net connect state; value 0:unknow, 1:idle, 2:connecting, 3:connected, 4:disconnecting, 5:disconnected
    RES_TYPE_NET_CONNECT_STATE_CHANGE,
    // connected net type; value 0:BEARER_CELLULAR, 1:BEARER_WIFI, 2:BEARER_BLUETOOTH
    // 3:BEARER_ETHERNET, 4:BEARER_VPN, 5:BEARER_WIFI_AWARE, -1:unknow
    RSE_TYPE_NET_BEAR_TYPE,
};
} // namespace ResType
} // namespace ResourceSchedule
} // namespace OHOS

#endif // RESSCHED_INTERFACES_INNERKITS_RESSCHED_CLIENT_INCLUDE_RES_TYPE_H
