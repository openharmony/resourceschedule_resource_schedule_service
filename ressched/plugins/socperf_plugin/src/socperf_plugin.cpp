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

#include "socperf_plugin.h"
#include "res_type.h"
#include "res_sched_log.h"
#include "plugin_mgr.h"
#include "config_info.h"

namespace OHOS {
namespace ResourceSchedule {
using namespace ResType;
namespace {
    const std::string LIB_NAME = "libsocperf_plugin.z.so";
    const int PERF_REQUEST_CMD_ID_APP_COLD_START_FIRST  = 10000;
    const int PERF_REQUEST_CMD_ID_APP_COLD_START_SECOND = 10001;
    const int PERF_REQUEST_CMD_ID_APP_WARM_START_FIRST  = 10002;
    const int PERF_REQUEST_CMD_ID_APP_WARM_START_SECOND = 10003;
    const int PERF_REQUEST_CMD_ID_WINDOW_SWITCH_FIRST   = 10004;
    const int PERF_REQUEST_CMD_ID_WINDOW_SWITCH_SECOND  = 10005;
    const int PERF_REQUEST_CMD_ID_EVENT_CLICK           = 10006;
    const int PERF_REQUEST_CMD_ID_PUSH_PAGE             = 10007;
    const int PERF_REQUEST_CMD_ID_EVENT_SLIDE           = 10008;
    const int PERF_REQUEST_CMD_ID_EVENT_SLIDE_OVER      = 10009;
    const int EVENT_ON                                  = 1;
    const int EVENT_OFF                                 = 0;
    const int WINDOW_FOCUSED                            = 0;
}
IMPLEMENT_SINGLE_INSTANCE(SocPerfPlugin)

void SocPerfPlugin::Init()
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
        { RES_TYPE_SLIDE_RECOGNIZE,
            [this](const std::shared_ptr<ResData>& data) { HandleEventSlide(data); } },
    };
    resTypes = {
        RES_TYPE_APP_STATE_CHANGE,
        RES_TYPE_WINDOW_FOCUS,
        RES_TYPE_CLICK_RECOGNIZE,
        RES_TYPE_PUSH_PAGE,
        RES_TYPE_SLIDE_RECOGNIZE,
    };
    for (auto resType : resTypes) {
        PluginMgr::GetInstance().SubscribeResource(LIB_NAME, resType);
    }
    RESSCHED_LOGI("SocPerfPlugin::Init success");
}

void SocPerfPlugin::Disable()
{
    functionMap.clear();
    for (auto resType : resTypes) {
        PluginMgr::GetInstance().UnSubscribeResource(LIB_NAME, resType);
    }
    resTypes.clear();
    RESSCHED_LOGI("SocPerfPlugin::Disable success");
}

void SocPerfPlugin::DispatchResource(const std::shared_ptr<ResData>& data)
{
    RESSCHED_LOGI("SocPerfPlugin::DispatchResource resType=%{public}u, value=%{public}lld",
        data->resType, (long long)data->value);
    auto funcIter = functionMap.find(data->resType);
    if (funcIter != functionMap.end()) {
        auto function = funcIter->second;
        if (function) {
            function(data);
        }
    }
}

void SocPerfPlugin::HandleAppStateChange(const std::shared_ptr<ResData>& data)
{
    if (data->value == static_cast<int32_t>(AppExecFwk::ApplicationState::APP_STATE_CREATE)) {
        RESSCHED_LOGI("SocPerfPlugin: socperf->APP_COLD_START");
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequest(PERF_REQUEST_CMD_ID_APP_COLD_START_FIRST, "");
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequest(PERF_REQUEST_CMD_ID_APP_COLD_START_SECOND, "");
    } else if (data->value == static_cast<int32_t>(AppExecFwk::ApplicationState::APP_STATE_FOREGROUND)) {
        RESSCHED_LOGI("SocPerfPlugin: socperf->APP_WARM_START");
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequest(PERF_REQUEST_CMD_ID_APP_WARM_START_FIRST, "");
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequest(PERF_REQUEST_CMD_ID_APP_WARM_START_SECOND, "");
    }
}

void SocPerfPlugin::HandleWindowFocus(const std::shared_ptr<ResData>& data)
{
    if (data->value == WINDOW_FOCUSED) {
        RESSCHED_LOGI("SocPerfPlugin: socperf->WINDOW_SWITCH");
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequest(PERF_REQUEST_CMD_ID_WINDOW_SWITCH_FIRST, "");
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequest(PERF_REQUEST_CMD_ID_WINDOW_SWITCH_SECOND, "");
    }
}

void SocPerfPlugin::HandleEventClick(const std::shared_ptr<ResData>& data)
{
    RESSCHED_LOGI("SocPerfPlugin: socperf->CLICK_NORMAL");
    OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequest(PERF_REQUEST_CMD_ID_EVENT_CLICK, "");
}

void SocPerfPlugin::HandlePushPage(const std::shared_ptr<ResData>& data)
{
    RESSCHED_LOGI("SocPerfPlugin: socperf->PUSH_PAGE");
    OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequest(PERF_REQUEST_CMD_ID_PUSH_PAGE, "");
}

void SocPerfPlugin::HandleEventSlide(const std::shared_ptr<ResData>& data)
{
    RESSCHED_LOGI("SocPerfPlugin: socperf->SLIDE_NORMAL: %{public}lld", (long long)data->value);
    if (data->value == EVENT_ON) {
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(PERF_REQUEST_CMD_ID_EVENT_SLIDE, true, "");
    } else if (data->value == EVENT_OFF) {
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(PERF_REQUEST_CMD_ID_EVENT_SLIDE, false, "");
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequest(PERF_REQUEST_CMD_ID_EVENT_SLIDE_OVER, "");
    }
}

extern "C" bool OnPluginInit(std::string& libName)
{
    if (libName != LIB_NAME) {
        RESSCHED_LOGE("SocPerfPlugin::OnPluginInit lib name is not match");
        return false;
    }
    SocPerfPlugin::GetInstance().Init();
    RESSCHED_LOGI("SocPerfPlugin::OnPluginInit success.");
    return true;
}

extern "C" void OnPluginDisable()
{
    SocPerfPlugin::GetInstance().Disable();
    RESSCHED_LOGI("SocPerfPlugin::OnPluginDisable success.");
}

extern "C" void OnDispatchResource(const std::shared_ptr<ResData>& data)
{
    SocPerfPlugin::GetInstance().DispatchResource(data);
    RESSCHED_LOGI("SocPerfPlugin::OnDispatchResource success.");
}
} // namespace ResourceSchedule
} // namespace OHOS
