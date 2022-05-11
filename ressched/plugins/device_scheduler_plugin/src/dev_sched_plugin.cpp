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

#include "dev_sched_plugin.h"

#include <cinttypes>

#include "config_info.h"
#include "plugin_mgr.h"
#include "res_type.h"
#include "res_sched_log.h"

namespace OHOS {
namespace ResourceSchedule {
using namespace ResType;
namespace {
    const std::string LIB_NAME = "libdevice_scheduler_plugin.z.so";
}
IMPLEMENT_SINGLE_INSTANCE(DevSchedPlugin)

void DevSchedPlugin::Init()
{
    resTypes.insert(RES_TYPE_SCREEN_STATUS);
    resTypes.insert(RES_TYPE_APP_STATE_CHANGE);
    resTypes.insert(RES_TYPE_ABILITY_STATE_CHANGE);
    resTypes.insert(RES_TYPE_EXTENSION_STATE_CHANGE);
    resTypes.insert(RES_TYPE_PROCESS_STATE_CHANGE);
    resTypes.insert(RES_TYPE_WINDOW_FOCUS);

    for (auto resType : resTypes) {
        PluginMgr::GetInstance().SubscribeResource(LIB_NAME, resType);
    }
    RESSCHED_LOGI("DevSchedPlugin::Init success");
}

void DevSchedPlugin::Disable()
{
    for (auto resType : resTypes) {
        PluginMgr::GetInstance().UnSubscribeResource(LIB_NAME, resType);
    }
    resTypes.clear();
    RESSCHED_LOGI("DevSchedPlugin::Disable success");
}

void DevSchedPlugin::DispatchResource(const std::shared_ptr<ResData>& data)
{
    RESSCHED_LOGI("DevSchedPlugin::DispatchResource type:%{public}u, value:%{public}" PRId64,
        data->resType, data->value);
}

extern "C" bool OnPluginInit(std::string& libName)
{
    if (libName != LIB_NAME) {
        RESSCHED_LOGE("DevSchedPlugin::OnPluginInit lib name is not match");
        return false;
    }
    DevSchedPlugin::GetInstance().Init();
    RESSCHED_LOGI("DevSchedPlugin::OnPluginInit success.");
    return true;
}

extern "C" void OnPluginDisable()
{
    DevSchedPlugin::GetInstance().Disable();
    RESSCHED_LOGI("DevSchedPlugin::OnPluginDisable success.");
}

extern "C" void OnDispatchResource(const std::shared_ptr<ResData>& data)
{
    DevSchedPlugin::GetInstance().DispatchResource(data);
    RESSCHED_LOGI("DevSchedPlugin::OnDispatchResource success.");
}
} // namespace ResourceSchedule
} // namespace OHOS