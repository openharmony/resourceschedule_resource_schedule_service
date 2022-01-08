/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: demo plugin.
 */

#include <unistd.h>
#include "demo_plugin.h"
#include "res_type.h"
#include "res_sched_log.h"
#include "plugin_mgr.h"
#include "config_info.h"

namespace OHOS {
namespace ResourceSchedule {
using namespace ResType;
namespace {
    const std::string LIB_NAME = "libdemoplugin_cplusplus.z.so";
}
IMPLEMENT_SINGLE_INSTANCE(DemoPlugin)

void DemoPlugin::Init()
{
    PluginMgr::GetInstance().SubscribeResource(LIB_NAME, RES_TYPE_SCREEN_STATUS);
    eventProcessTime = 2000;
    RESSCHED_LOGI("DemoPlugin::SubscribeResource and init success!");
}

void DemoPlugin::Disable()
{
    PluginMgr::GetInstance().UnSubscribeResource(LIB_NAME, RES_TYPE_SCREEN_STATUS);
    RESSCHED_LOGI("DemoPlugin::Disable and disable success!");
}

void DemoPlugin::DispatchResource(const std::shared_ptr<ResData>& data)
{
    RESSCHED_LOGD("DemoPlugin::DispatchResource status type %{public}u .", data->resType);
    usleep(eventProcessTime);
    eventProcessTime += 1000;
    PluginConfig config = PluginMgr::GetInstance().GetConfig("demo", "sample");
    for (Item& item : config.itemList) {
        for (SubItem sub : item.subItemList) {
            RESSCHED_LOGD("DemoPlugin::DispatchResource name: %{public}s value: %{public}s ", sub.name.c_str(), sub.value.c_str());
        }
    }

    if (data->resType != RES_TYPE_SCREEN_STATUS) {
        RESSCHED_LOGE("DemoPlugin::DispatchResource resource type has not subcscribe.");
        return;
    }
    RESSCHED_LOGD("DemoPlugin::DispatchResource success.");
}

extern "C" bool OnPluginInit(std::string& libName)
{
    if (libName != LIB_NAME) {
        RESSCHED_LOGE("DemoPlugin::OnPluginInit lib name is not match");
        return false;
    }
    DemoPlugin::GetInstance().Init();
    RESSCHED_LOGI("DemoPlugin::OnPluginInit c++ success.");
    return true;
}

extern "C" void OnPluginDisable()
{
    RESSCHED_LOGI("DemoPlugin::OnPluginDisable c++ success.");
    DemoPlugin::GetInstance().Disable();
}

extern "C" void OnDispatchResource(const std::shared_ptr<ResData>& data)
{
    DemoPlugin::GetInstance().DispatchResource(data);
    RESSCHED_LOGI("DemoPlugin::DispatchResource c++ success.");
}

}
}