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

#include <unistd.h>
#include "unittest_plugin.h"
#include "res_type.h"
#include "res_sched_log.h"
#include "plugin_mgr.h"
#include "config_info.h"

namespace OHOS {
namespace ResourceSchedule {
using namespace ResType;
namespace {
    const std::string LIB_NAME = "libunittestplugin_cplusplus.z.so";
}
IMPLEMENT_SINGLE_INSTANCE(UnittestPlugin)

void UnittestPlugin::Init()
{
    PluginMgr::GetInstance().SubscribeResource(LIB_NAME, RES_TYPE_SCREEN_STATUS);
    eventProcessTime = 2000;
    RESSCHED_LOGI("UnittestPlugin::SubscribeResource and init success!");
}

void UnittestPlugin::Disable()
{
    PluginMgr::GetInstance().UnSubscribeResource(LIB_NAME, RES_TYPE_SCREEN_STATUS);
    RESSCHED_LOGI("UnittestPlugin::Disable and disable success!");
}

void UnittestPlugin::DispatchResource(const std::shared_ptr<ResData>& data)
{
    RESSCHED_LOGD("UnittestPlugin::DispatchResource status type %{public}u .", data->resType);
    usleep(eventProcessTime);
    eventProcessTime += 1000;
    PluginConfig config = PluginMgr::GetInstance().GetConfig("unittest", "sample");
    for (Item& item : config.itemList) {
        for (SubItem sub : item.subItemList) {
            RESSCHED_LOGD("UnittestPlugin::DispatchResource name: %{public}s value: %{public}s ", sub.name.c_str(), sub.value.c_str());
        }
    }

    if (data->resType != RES_TYPE_SCREEN_STATUS) {
        RESSCHED_LOGE("UnittestPlugin::DispatchResource resource type has not subcscribe.");
        return;
    }
    RESSCHED_LOGD("UnittestPlugin::DispatchResource success.");
}

extern "C" bool OnPluginInit(std::string& libName)
{
    if (libName != LIB_NAME) {
        RESSCHED_LOGE("UnittestPlugin::OnPluginInit lib name is not match");
        return false;
    }
    UnittestPlugin::GetInstance().Init();
    RESSCHED_LOGI("UnittestPlugin::OnPluginInit c++ success.");
    return true;
}

extern "C" void OnPluginDisable()
{
    RESSCHED_LOGI("UnittestPlugin::OnPluginDisable c++ success.");
    UnittestPlugin::GetInstance().Disable();
}

extern "C" void OnDispatchResource(const std::shared_ptr<ResData>& data)
{
    UnittestPlugin::GetInstance().DispatchResource(data);
    RESSCHED_LOGI("UnittestPlugin::DispatchResource c++ success.");
}

}
}