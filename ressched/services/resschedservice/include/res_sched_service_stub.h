/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#ifndef RESSCHED_SERVICES_RESSCHEDSERVICE_INCLUDE_RES_SCHED_SERVICE_STUB_H
#define RESSCHED_SERVICES_RESSCHEDSERVICE_INCLUDE_RES_SCHED_SERVICE_STUB_H

#include <cstdint>
#include <map>
#include <unordered_set>

#include "iremote_stub.h"
#include "ires_sched_service.h"

namespace OHOS {
namespace ResourceSchedule {
class ResSchedServiceStub : public IRemoteStub<IResSchedService> {
public:
    ResSchedServiceStub();
    ~ResSchedServiceStub();
    DISALLOW_COPY_AND_MOVE(ResSchedServiceStub);
    int32_t OnRemoteRequest(
        uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;

private:
    int32_t ReportDataInner(MessageParcel& data, MessageParcel& reply);
    int32_t KillProcessInner(MessageParcel& data, MessageParcel& reply);
    nlohmann::json StringToJsonObj(const std::string& str);

    void Init();

    using RequestFuncType = std::function<int32_t (MessageParcel& data, MessageParcel& reply)>;
    std::map<uint32_t, RequestFuncType> funcMap_;
    std::unordered_set<uint32_t> thirdPartRes_ = {
        ResType::RES_TYPE_CLICK_RECOGNIZE,
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
    };
    std::unordered_set<uint32_t> saRes_ = {
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
        ResType::RES_TYPE_REPORT_RENDER_THREAD,
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
        ResType::RES_TYPE_ANCO_APP_FRONT,
        ResType::RES_TYPE_TIMEZONE_CHANGED,
    };
};
} // namespace ResourceSchedule
} // namespace OHOS

#endif // RESSCHED_SERVICES_RESSCHEDSERVICE_INCLUDE_RES_SCHED_SERVICE_STUB_H
