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

#include "component_sched_plugin.h"

#include "bluetooth_def.h"
#ifdef COMPONENT_SCHED_ENABLE
#include "component_sched_client.h"
#endif

#include "config_info.h"
#include "plugin_mgr.h"
#include "res_sched_log.h"
#include "res_type.h"

namespace OHOS {
namespace ResourceSchedule {
using namespace ResType;
namespace {
    const std::string LIB_NAME = "libcomponent_sched_plugin.z.so";
}
IMPLEMENT_SINGLE_INSTANCE(ComponentSchedPlugin)

void ComponentSchedPlugin::Init()
{
    resTypes_.insert(RES_TYPE_ABILITY_STATE_CHANGE);
    resTypes_.insert(RES_TYPE_APP_INSTALL_UNINSTALL);
    resTypes_.insert(RES_TYPE_APP_STATE_CHANGE);  // app switch
    resTypes_.insert(RES_TYPE_EXTENSION_STATE_CHANGE);
    resTypes_.insert(RES_TYPE_WIFI_CONNECT_STATE_CHANGE);
    resTypes_.insert(RES_TYPE_PROCESS_STATE_CHANGE);  // process create and died
    resTypes_.insert(RES_TYPE_PUSH_PAGE);
    resTypes_.insert(RES_TYPE_SCREEN_STATUS);
    resTypes_.insert(RES_TYPE_WINDOW_FOCUS);
    resTypes_.insert(RES_TYPE_USER_SWITCH);
    resTypes_.insert(RES_TYPE_USER_REMOVE);
    resTypes_.insert(RES_TYPE_SCREEN_LOCK);
    resTypes_.insert(RES_TYPE_BLUETOOTH_A2DP_CONNECT_STATE_CHANGE);

    for (auto resType : resTypes_) {
        PluginMgr::GetInstance().SubscribeResource(LIB_NAME, resType);
    }
    RESSCHED_LOGI("ComponentSchedPlugin::Init success");
}

void ComponentSchedPlugin::Disable()
{
    for (auto resType : resTypes_) {
        PluginMgr::GetInstance().UnSubscribeResource(LIB_NAME, resType);
    }
    resTypes_.clear();
    RESSCHED_LOGI("ComponentSchedPlugin::Disable success");
}

void ComponentSchedPlugin::DispatchResource(const std::shared_ptr<ResData>& data)
{
    if (!data) {
        RESSCHED_LOGW("ComponentSchedPlugin::DispatchResource data is null");
        return;
    }
    
    RESSCHED_LOGD(
        "ComponentSchedPlugin::DispatchResource type=%{public}u value=%{public}lld payload=%{public}s",
        data->resType, (long long)(data->value), (data->payload.toStyledString()).c_str());

#ifdef COMPONENT_SCHED_ENABLE
    ComponentScheduler::ComponentSchedClient::GetInstance().ReportSceneInfo(data);
#endif
}

extern "C" bool OnPluginInit(std::string& libName)
{
    if (libName != LIB_NAME) {
        RESSCHED_LOGE("ComponentSchedPlugin::OnPluginInit lib name is not match");
        return false;
    }
    ComponentSchedPlugin::GetInstance().Init();
    RESSCHED_LOGI("ComponentSchedPlugin::OnPluginInit success.");
    return true;
}

extern "C" void OnPluginDisable()
{
    ComponentSchedPlugin::GetInstance().Disable();
    RESSCHED_LOGI("ComponentSchedPlugin::OnPluginDisable success.");
}

extern "C" void OnDispatchResource(const std::shared_ptr<ResData>& data)
{
    ComponentSchedPlugin::GetInstance().DispatchResource(data);
    RESSCHED_LOGD("ComponentSchedPlugin::OnDispatchResource success.");
}
} // namespace ResourceSchedule
} // namespace OHOS