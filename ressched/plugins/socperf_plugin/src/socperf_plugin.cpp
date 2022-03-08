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
}
IMPLEMENT_SINGLE_INSTANCE(SocPerfPlugin)

void SocPerfPlugin::Init()
{
    resTypes.insert(RES_TYPE_APP_STATE_CHANGE);
    resTypes.insert(RES_TYPE_WINDOW_FOCUS);
    resTypes.insert(RES_TYPE_CLICK_RECOGNIZE);
    resTypes.insert(RES_TYPE_PUSH_PAGE);
    resTypes.insert(RTS_TYPE_SLIDE_RECOGNIZE);
    for (auto resType : resTypes) {
        PluginMgr::GetInstance().SubscribeResource(LIB_NAME, resType);
    }
    runner = AppExecFwk::EventRunner::Create("socperf_plugin_handler");
    if (runner == nullptr) {
        RESSCHED_LOGE("Failed to Create EventRunner");
        return;
    }
    handler = std::make_shared<SocPerfPluginHandler>(runner);
    RESSCHED_LOGI("SocPerfPlugin::Init success");
}

void SocPerfPlugin::Disable()
{
    for (auto resType : resTypes) {
        PluginMgr::GetInstance().UnSubscribeResource(LIB_NAME, resType);
    }
    resTypes.clear();
    std::lock_guard<std::mutex> autoLock(socperfPluginMutex_);
    if (handler != nullptr) {
        handler->RemoveAllEvents();
        handler = nullptr;
    }
    if (runner != nullptr) {
        runner->Stop();
        runner = nullptr;
    }
    RESSCHED_LOGI("SocPerfPlugin::Disable success");
}

void SocPerfPlugin::DispatchResource(const std::shared_ptr<ResData>& data)
{
    RESSCHED_LOGI("SocPerfPlugin::DispatchResource resType=%{public}u, value=%{public}lld",
        data->resType, data->value);
    auto event = AppExecFwk::InnerEvent::Get(INNER_EVENT_ID_SOC_PERF_PLUGIN_DISPATCH, data);
    std::lock_guard<std::mutex> autoLock(socperfPluginMutex_);
    if (handler != nullptr) {
        handler->SendEvent(event);
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
}
}
