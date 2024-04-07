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

#ifdef RESSCHED_RESOURCESCHEDULE_SOC_PERF_ENABLE
#include "socperf_plugin.h"
#include "app_mgr_constants.h"
#include "config_info.h"
#include "plugin_mgr.h"
#include "res_type.h"
#include "socperf_log.h"

namespace OHOS {
namespace ResourceSchedule {
using namespace ResType;
namespace {
    const std::string LIB_NAME = "libsocperf_plugin.z.so";
    const std::string PLUGIN_NAME = "SOCPERF";
    const std::string CONFIG_NAME_SOCPERF_FEATURE_SWITCH = "socperfFeatureSwitch";
    const std::string SUB_ITEM_KEY_NAME_SOCPERF_ON_DEMAND = "socperf_on_demand";
    const std::string EXTENSION_TYPE_KEY = "extensionType";
    const std::string DEVICE_MODE_PAYMODE_NAME = "deviceMode";
    const int32_t INVALID_VALUE                             = -1;
    const int32_t PERF_REQUEST_CMD_ID_APP_START             = 10000;
    const int32_t PERF_REQUEST_CMD_ID_WARM_START            = 10001;
    const int32_t PERF_REQUEST_CMD_ID_WINDOW_SWITCH         = 10002;
    const int32_t PERF_REQUEST_CMD_ID_EVENT_CLICK           = 10006;
    const int32_t PERF_REQUEST_CMD_ID_LOAD_PAGE_START       = 10007;
    const int32_t PERF_REQUEST_CMD_ID_EVENT_SLIDE           = 10008;
    const int32_t PERF_REQUEST_CMD_ID_EVENT_SLIDE_OVER      = 10009;
    const int32_t PERF_REQUEST_CMD_ID_EVENT_TOUCH           = 10010;
    const int32_t PERF_REQUEST_CMD_ID_LOAD_PAGE_COMPLETE    = 10011;
    const int32_t PERF_REQUEST_CMD_ID_EVENT_WEB_GESTURE     = 10012;
    const int32_t PERF_REQUEST_CMD_ID_POP_PAGE              = 10016;
    const int32_t PERF_REQUEST_CMD_ID_RESIZE_WINDOW         = 10018;
    const int32_t PERF_REQUEST_CMD_ID_MOVE_WINDOW           = 10019;
    const int32_t PERF_REQUEST_CMD_ID_WEB_GESTURE_MOVE      = 10020;
    const int32_t PERF_REQUEST_CMD_ID_WEB_SLIDE_NORMAL      = 10025;
    const int32_t PERF_REQUEST_CMD_ID_REMOTE_ANIMATION      = 10030;
    const int32_t PERF_REQUEST_CMD_ID_DRAG_STATUS_BAR       = 10034;
    const int32_t PERF_REQUEST_CMD_ID_LOAD_URL              = 10070;
    const int32_t PERF_REQUEST_CMD_ID_MOUSEWHEEL            = 10071;
    const int32_t PERF_REQUEST_CMD_ID_WEB_DRAG_RESIZE       = 10073;
}
IMPLEMENT_SINGLE_INSTANCE(SocPerfPlugin)

void SocPerfPlugin::Init()
{
    InitFunctionMap();
    InitResTypes();
    for (auto resType : resTypes) {
        PluginMgr::GetInstance().SubscribeResource(LIB_NAME, resType);
    }
    SOC_PERF_LOGI("SocPerfPlugin::Init success");
}

void SocPerfPlugin::InitFunctionMap()
{
    functionMap = {
        { RES_TYPE_WINDOW_FOCUS,
            [this](const std::shared_ptr<ResData>& data) { HandleWindowFocus(data); } },
        { RES_TYPE_CLICK_RECOGNIZE,
            [this](const std::shared_ptr<ResData>& data) { HandleEventClick(data); } },
        { RES_TYPE_LOAD_PAGE,
            [this](const std::shared_ptr<ResData>& data) { HandleLoadPage(data); } },
        { RES_TYPE_SLIDE_RECOGNIZE,
            [this](const std::shared_ptr<ResData>& data) { HandleEventSlide(data); } },
        { RES_TYPE_WEB_GESTURE,
            [this](const std::shared_ptr<ResData>& data) { HandleEventWebGesture(data); } },
        { RES_TYPE_POP_PAGE,
            [this](const std::shared_ptr<ResData>& data) { HandlePopPage(data); } },
        { RES_TYPE_APP_ABILITY_START,
            [this](const std::shared_ptr<ResData>& data) { HandleAppAbilityStart(data); } },
        { RES_TYPE_RESIZE_WINDOW,
            [this](const std::shared_ptr<ResData>& data) { HandleResizeWindow(data); } },
        { RES_TYPE_MOVE_WINDOW,
            [this](const std::shared_ptr<ResData>& data) { HandleMoveWindow(data); } },
        { RES_TYPE_SHOW_REMOTE_ANIMATION,
                [this](const std::shared_ptr<ResData>& data) { HandleRemoteAnimation(data); } },
        { RES_TYPE_DRAG_STATUS_BAR,
            [this](const std::shared_ptr<ResData>& data) { HandleDragStatusBar(data); } },
        { RES_TYPE_WEB_GESTURE_MOVE,
            [this](const std::shared_ptr<ResData>& data) { HandleWebGestureMove(data); } },
        { RES_TYPE_WEB_SLIDE_NORMAL,
            [this](const std::shared_ptr<ResData>& data) { HandleWebSlideNormal(data); } },
        { RES_TYPE_LOAD_URL,
            [this](const std::shared_ptr<ResData>& data) { HandleLoadUrl(data); } },
        { RES_TYPE_MOUSEWHEEL,
            [this](const std::shared_ptr<ResData>& data) { HandleMousewheel(data); } },
        { RES_TYPE_APP_STATE_CHANGE,
            [this](const std::shared_ptr<ResData>& data) { HandleAppStateChange(data); } },
        { RES_TYPE_DEVICE_MODE_STATUS,
            [this](const std::shared_ptr<ResData>& data) { HandleDeviceModeStatusChange(data); } },
        { RES_TYPE_WEB_DRAG_RESIZE,
            [this](const std::shared_ptr<ResData>& data) { HandleWebDragResize(data); } },
    };
}

void SocPerfPlugin::InitResTypes()
{
    resTypes = {
        RES_TYPE_WINDOW_FOCUS,
        RES_TYPE_CLICK_RECOGNIZE,
        RES_TYPE_LOAD_PAGE,
        RES_TYPE_SLIDE_RECOGNIZE,
        RES_TYPE_WEB_GESTURE,
        RES_TYPE_POP_PAGE,
        RES_TYPE_APP_ABILITY_START,
        RES_TYPE_RESIZE_WINDOW,
        RES_TYPE_MOVE_WINDOW,
        RES_TYPE_SHOW_REMOTE_ANIMATION,
        RES_TYPE_DRAG_STATUS_BAR,
        RES_TYPE_WEB_GESTURE_MOVE,
        RES_TYPE_WEB_SLIDE_NORMAL,
        RES_TYPE_LOAD_URL,
        RES_TYPE_MOUSEWHEEL,
        RES_TYPE_APP_STATE_CHANGE,
        RES_TYPE_DEVICE_MODE_STATUS,
        RES_TYPE_WEB_DRAG_RESIZE,
    };
}

void SocPerfPlugin::Disable()
{
    functionMap.clear();
    for (auto resType : resTypes) {
        PluginMgr::GetInstance().UnSubscribeResource(LIB_NAME, resType);
    }
    resTypes.clear();
    SOC_PERF_LOGI("SocPerfPlugin::Disable success");
}

void SocPerfPlugin::DispatchResource(const std::shared_ptr<ResData>& data)
{
    auto funcIter = functionMap.find(data->resType);
    if (funcIter != functionMap.end()) {
        auto function = funcIter->second;
        if (function) {
            function(data);
        }
    }
}

bool SocPerfPlugin::InitFeatureSwitch(std::string featureName)
{
    PluginConfig itemLists = PluginMgr::GetInstance().GetConfig(PLUGIN_NAME, CONFIG_NAME_SOCPERF_FEATURE_SWITCH);
    for (const Item& item : itemLists.itemList) {
        for (SubItem sub : item.subItemList) {
            if (sub.name == featureName) {
                return sub.value == "1";
            }
        }
    }
    return false;
}

static int32_t ParsePayload(const std::shared_ptr<ResData>& data, const std::string& key)
{
    if (!data->payload.contains(key)) {
        return INVALID_VALUE;
    }
    if (data->payload.at(key).is_string()) {
        return atoi(data->payload[key].get<std::string>().c_str());
    }
    if (data->payload.at(key).is_number_integer()) {
        return data->payload[key].get<int32_t>();
    }
    return INVALID_VALUE;
}

void SocPerfPlugin::HandleAppAbilityStart(const std::shared_ptr<ResData>& data)
{
    if (data->value == AppStartType::APP_COLD_START) {
        SOC_PERF_LOGI("SocPerfPlugin: socperf->APP_COLD_START");
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequest(PERF_REQUEST_CMD_ID_APP_START, "");
    } else if (data->value == AppStartType::APP_WARM_START) {
        SOC_PERF_LOGI("SocPerfPlugin: socperf->APP_WARM_START");
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequest(PERF_REQUEST_CMD_ID_WARM_START, "");
    }
}

void SocPerfPlugin::HandleWindowFocus(const std::shared_ptr<ResData>& data)
{
    if (data->value == WindowFocusStatus::WINDOW_FOCUS) {
        SOC_PERF_LOGI("SocPerfPlugin: socperf->WINDOW_SWITCH");
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequest(PERF_REQUEST_CMD_ID_WINDOW_SWITCH, "");
    }
}

void SocPerfPlugin::HandleEventClick(const std::shared_ptr<ResData>& data)
{
    SOC_PERF_LOGD("SocPerfPlugin: socperf->EVENT_CLICK: %{public}lld", (long long)data->value);
    if (data->value == ClickEventType::TOUCH_EVENT) {
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequest(PERF_REQUEST_CMD_ID_EVENT_TOUCH, "");
    } else if (data->value == ClickEventType::CLICK_EVENT) {
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequest(PERF_REQUEST_CMD_ID_EVENT_CLICK, "");
    }
}

void SocPerfPlugin::HandleLoadPage(const std::shared_ptr<ResData>& data)
{
    if (data->value == LOAD_PAGE_START) {
        SOC_PERF_LOGI("SocPerfPlugin: socperf->PUSH_PAGE_START");
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(PERF_REQUEST_CMD_ID_LOAD_PAGE_START, true, "");
    } else if (data->value == LOAD_PAGE_COMPLETE) {
        SOC_PERF_LOGI("SocPerfPlugin: socperf->PUSH_PAGE_COMPLETE");
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(PERF_REQUEST_CMD_ID_LOAD_PAGE_START, false, "");
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequest(PERF_REQUEST_CMD_ID_LOAD_PAGE_COMPLETE, "");
    }
}

void SocPerfPlugin::HandlePopPage(const std::shared_ptr<ResData>& data)
{
    SOC_PERF_LOGI("SocPerfPlugin: socperf->POP_PAGE: %{public}lld", (long long)data->value);
    OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequest(PERF_REQUEST_CMD_ID_POP_PAGE, "");
}

void SocPerfPlugin::HandleEventSlide(const std::shared_ptr<ResData>& data)
{
    SOC_PERF_LOGD("SocPerfPlugin: socperf->SLIDE_NORMAL: %{public}lld", (long long)data->value);
    if (data->value == SlideEventStatus::SLIDE_EVENT_ON) {
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(PERF_REQUEST_CMD_ID_EVENT_SLIDE, true, "");
    } else if (data->value == SlideEventStatus::SLIDE_EVENT_OFF) {
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(PERF_REQUEST_CMD_ID_EVENT_SLIDE, false, "");
    } else if (data->value == SlideEventStatus::SLIDE_NORMAL_BEGIN) {
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequest(PERF_REQUEST_CMD_ID_EVENT_SLIDE_OVER, "");
    } else if (data->value == SlideEventStatus::SLIDE_NORMAL_END) {
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(PERF_REQUEST_CMD_ID_EVENT_SLIDE_OVER, false, "");
    }
}

void SocPerfPlugin::HandleEventWebGesture(const std::shared_ptr<ResData>& data)
{
    if (data == nullptr) {
        return;
    }
    SOC_PERF_LOGD("SocPerfPlugin: socperf->WEB_GESTURE: %{public}lld", (long long)data->value);
    if (data->value == WebGesture::WEB_GESTURE_START) {
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(PERF_REQUEST_CMD_ID_EVENT_WEB_GESTURE, true, "");
    } else if (data->value == WebGesture::WEB_GESTURE_END) {
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(PERF_REQUEST_CMD_ID_EVENT_WEB_GESTURE, false, "");
    }
}

void SocPerfPlugin::HandleResizeWindow(const std::shared_ptr<ResData>& data)
{
    if (data == nullptr) {
        return;
    }
    SOC_PERF_LOGI("SocPerfPlugin: socperf->RESIZE_WINDOW: %{public}lld", (long long)data->value);
    if (data->value == WindowResizeType::WINDOW_RESIZING) {
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequest(PERF_REQUEST_CMD_ID_RESIZE_WINDOW, "");
    } else if (data->value == WindowResizeType::WINDOW_RESIZE_STOP) {
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(PERF_REQUEST_CMD_ID_RESIZE_WINDOW, false, "");
    }
}

void SocPerfPlugin::HandleMoveWindow(const std::shared_ptr<ResData>& data)
{
    if (data == nullptr) {
        return;
    }
    SOC_PERF_LOGI("SocPerfPlugin: socperf->MOVE_WINDOW: %{public}lld", (long long)data->value);
    if (data->value == WindowMoveType::WINDOW_MOVING) {
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequest(PERF_REQUEST_CMD_ID_MOVE_WINDOW, "");
    } else if (data->value == WindowMoveType::WINDOW_MOVE_STOP) {
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(PERF_REQUEST_CMD_ID_MOVE_WINDOW, false, "");
    }
}

void SocPerfPlugin::HandleRemoteAnimation(const std::shared_ptr<ResData>& data)
{
    if (data->value == ShowRemoteAnimationStatus::ANIMATION_BEGIN) {
        SOC_PERF_LOGI("SocPerfPlugin: socperf->REMOTE_ANIMATION: %{public}lld", (long long)data->value);
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(PERF_REQUEST_CMD_ID_REMOTE_ANIMATION, true, "");
    } else if (data->value == ShowRemoteAnimationStatus::ANIMATION_END) {
        SOC_PERF_LOGI("SocPerfPlugin: socperf->REMOTE_ANIMATION: %{public}lld", (long long)data->value);
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(PERF_REQUEST_CMD_ID_REMOTE_ANIMATION, false, "");
    }
}

void SocPerfPlugin::HandleDragStatusBar(const std::shared_ptr<ResData>& data)
{
    if (data == nullptr) {
        return;
    }
    SOC_PERF_LOGI("SocPerfPlugin: socperf->DRAG_STATUS_BAR: %{public}lld", (long long)data->value);
    if (data->value == StatusBarDragStatus::DRAG_START) {
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequest(PERF_REQUEST_CMD_ID_DRAG_STATUS_BAR, "");
    } else if (data->value == StatusBarDragStatus::DRAG_END) {
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(PERF_REQUEST_CMD_ID_DRAG_STATUS_BAR, false, "");
    }
}

void SocPerfPlugin::HandleWebGestureMove(const std::shared_ptr<ResData>& data)
{
    if (data == nullptr) {
        return;
    }
    SOC_PERF_LOGI("SocPerfPlugin: socperf->WEB_GESTURE_MOVE: %{public}lld", (long long)data->value);
    if (data->value == WebGestureMove::WEB_GESTURE_MOVE_START) {
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(PERF_REQUEST_CMD_ID_WEB_GESTURE_MOVE, true, "");
    } else if (data->value == WebGestureMove::WEB_GESTURE_MOVE_END) {
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(PERF_REQUEST_CMD_ID_WEB_GESTURE_MOVE, false, "");
    }
}

void SocPerfPlugin::HandleWebSlideNormal(const std::shared_ptr<ResData>& data)
{
    if (data == nullptr) {
        return;
    }
    SOC_PERF_LOGD("SocPerfPlugin: socperf->WEB_SLIDE_NORMAL: %{public}lld", (long long)data->value);
    if (data->value == WebSlideNormal::WEB_SLIDE_NORMAL_START) {
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(PERF_REQUEST_CMD_ID_WEB_SLIDE_NORMAL, true, "");
    } else if (data->value == WebSlideNormal::WEB_SLIDE_NORMAL_END) {
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(PERF_REQUEST_CMD_ID_WEB_SLIDE_NORMAL, false, "");
    }
}

void SocPerfPlugin::HandleLoadUrl(const std::shared_ptr<ResData>& data)
{
    SOC_PERF_LOGI("SocPerfPlugin: socperf->LOAD_URL");
    OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequest(PERF_REQUEST_CMD_ID_LOAD_URL, "");
}

void SocPerfPlugin::HandleMousewheel(const std::shared_ptr<ResData>& data)
{
    SOC_PERF_LOGI("SocPerfPlugin: socperf->MOUSEWHEEL");
    OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequest(PERF_REQUEST_CMD_ID_MOUSEWHEEL, "");
}

bool SocPerfPlugin::HandleAppStateChange(const std::shared_ptr<ResData>& data)
{
    if (data->value != ResType::ProcessStatus::PROCESS_CREATED) {
        return false;
    }
    int extensionType = ParsePayload(data, EXTENSION_TYPE_KEY);
    if (std::find(ResType::UI_SENSITIVE_EXTENSION.begin(), ResType::UI_SENSITIVE_EXTENSION.end(), extensionType) !=
        ResType::UI_SENSITIVE_EXTENSION.end()) {
        SOC_PERF_LOGI("SocPerfPlugin: socperf->APPSTATECHANGE");
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequest(PERF_REQUEST_CMD_ID_APP_START, "");
        return true;
    }
    return false;
}

void SocPerfPlugin::HandleDeviceModeStatusChange(const std::shared_ptr<ResData>& data)
{
    if ((data->value != DeviceModeStatus::MODE_ENTER) && (data->value != DeviceModeStatus::MODE_QUIT)) {
        SOC_PERF_LOGW("SocPerfPlugin: device mode status value is error");
        return;
    }

    if (!data->payload.contains(DEVICE_MODE_PAYMODE_NAME) || !data->payload[DEVICE_MODE_PAYMODE_NAME].is_string()) {
        SOC_PERF_LOGW("SocPerfPlugin: device mode status payload is error");
        return;
    }

    std::string deviceMode = data->payload[DEVICE_MODE_PAYMODE_NAME];
    bool status = (data->value == DeviceModeStatus::MODE_ENTER);
    OHOS::SOCPERF::SocPerfClient::GetInstance().RequestDeviceMode(deviceMode, status);
    SOC_PERF_LOGI("SocPerfPlugin: device mode %{public}s  status%{public}d", deviceMode.c_str(), status);
}

void SocPerfPlugin::HandleWebDragResize(const std::shared_ptr<ResData>& data)
{
    if (data == nullptr) {
        return;
    }
    SOC_PERF_LOGI("SocPerfPlugin: socperf->WEB_DRAG_RESIZE: %{public}lld", (long long)data->value);
    if (data->value == WebDragResizeStatus::WEB_DRAG_START) {
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequest(PERF_REQUEST_CMD_ID_WEB_DRAG_RESIZE, "");
    } else if (data->value == WebDragResizeStatus::WEB_DRAG_END) {
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(PERF_REQUEST_CMD_ID_WEB_DRAG_RESIZE, false, "");
    }
}

extern "C" bool OnPluginInit(std::string& libName)
{
    if (libName != LIB_NAME) {
        SOC_PERF_LOGE("SocPerfPlugin::OnPluginInit lib name is not match");
        return false;
    }
    SocPerfPlugin::GetInstance().Init();
    return true;
}

extern "C" void OnPluginDisable()
{
    SocPerfPlugin::GetInstance().Disable();
}

extern "C" void OnDispatchResource(const std::shared_ptr<ResData>& data)
{
    SocPerfPlugin::GetInstance().DispatchResource(data);
}
} // namespace ResourceSchedule
} // namespace OHOS
#endif // RESSCHED_RESOURCESCHEDULE_SOC_PERF_ENABLE