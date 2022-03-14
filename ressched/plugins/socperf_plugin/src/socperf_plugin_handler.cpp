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

#include "socperf_plugin_handler.h"

namespace OHOS {
namespace ResourceSchedule {
using namespace ResType;
using namespace SOCPERF;

SocPerfPluginHandler::SocPerfPluginHandler(
    const std::shared_ptr<AppExecFwk::EventRunner>& runner) : AppExecFwk::EventHandler(runner)
{
    functionMap = {
        { RES_TYPE_APP_STATE_CHANGE,
            [this](const std::shared_ptr<ResData>& data) { HandleAppStateChange(data); } },
        { RES_TYPE_WINDOW_FOCUS,
            [this](const std::shared_ptr<ResData>& data) { HandleWindowFocus(data); } },
        { RES_TYPE_CLICK_RECOGNIZE,
            [this](const std::shared_ptr<ResData>& data) { HandleEventClick(data); } },
        { RES_TYPE_PUSH_PAGE,
            [this](const std::shared_ptr<ResData>& data) { HandlePushPage(data); } },
        { RTS_TYPE_SLIDE_RECOGNIZE,
            [this](const std::shared_ptr<ResData>& data) { HandleEventSlide(data); } },
    };
}

SocPerfPluginHandler::~SocPerfPluginHandler()
{
    functionMap.clear();
}

void SocPerfPluginHandler::ProcessEvent(const AppExecFwk::InnerEvent::Pointer &event)
{
    switch (event->GetInnerEventId()) {
        case INNER_EVENT_ID_SOC_PERF_PLUGIN_DISPATCH: {
            auto data = event->GetSharedObject<ResData>();
            auto funcIter = functionMap.find(data->resType);
            if (funcIter != functionMap.end()) {
                auto function = funcIter->second;
                if (function) {
                    function(data);
                }
            }
            break;
        }
        default: {
            break;
        }
    }
}

void SocPerfPluginHandler::HandleAppStateChange(const std::shared_ptr<ResData>& data)
{
    if (data->value == static_cast<int32_t>(AppExecFwk::ApplicationState::APP_STATE_CREATE)) {
        RESSCHED_LOGI("SocPerfPluginHandler: socperf->APP_COLD_START");
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequest(PERF_REQUEST_CMD_ID_APP_COLD_START_FIRST, "");
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequest(PERF_REQUEST_CMD_ID_APP_COLD_START_SECOND, "");
    } else if (data->value == static_cast<int32_t>(AppExecFwk::ApplicationState::APP_STATE_FOREGROUND)) {
        RESSCHED_LOGI("SocPerfPluginHandler: socperf->APP_WARM_START");
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequest(PERF_REQUEST_CMD_ID_APP_WARM_START_FIRST, "");
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequest(PERF_REQUEST_CMD_ID_APP_WARM_START_SECOND, "");
    }
}

void SocPerfPluginHandler::HandleWindowFocus(const std::shared_ptr<ResData>& data)
{
    if (data->value == WINDOW_FOCUSED) {
        RESSCHED_LOGI("SocPerfPluginHandler: socperf->WINDOW_SWITCH");
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequest(PERF_REQUEST_CMD_ID_WINDOW_SWITCH_FIRST, "");
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequest(PERF_REQUEST_CMD_ID_WINDOW_SWITCH_SECOND, "");
    }
}

void SocPerfPluginHandler::HandleEventClick(const std::shared_ptr<ResData>& data)
{
    RESSCHED_LOGI("SocPerfPluginHandler: socperf->CLICK_NORMAL");
    OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequest(PERF_REQUEST_CMD_ID_EVENT_CLICK, "");
}

void SocPerfPluginHandler::HandlePushPage(const std::shared_ptr<ResData>& data)
{
    RESSCHED_LOGI("SocPerfPluginHandler: socperf->PUSH_PAGE");
    OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequest(PERF_REQUEST_CMD_ID_PUSH_PAGE, "");
}

void SocPerfPluginHandler::HandleEventSlide(const std::shared_ptr<ResData>& data)
{
    RESSCHED_LOGI("SocPerfPluginHandler: socperf->SLIDE_NORMAL: %{public}lld", data->value);
    if (data->value == EVENT_ON) {
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(PERF_REQUEST_CMD_ID_EVENT_SLIDE, true, "");
    } else if (data->value == EVENT_OFF) {
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(PERF_REQUEST_CMD_ID_EVENT_SLIDE, false, "");
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequest(PERF_REQUEST_CMD_ID_EVENT_SLIDE_OVER, "");
    }
}
} // namespace ResourceSchedule
} // namespace OHOS
