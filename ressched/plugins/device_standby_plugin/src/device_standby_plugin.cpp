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

#ifdef RSS_DEVICE_STANDBY_ENABLE
#include "device_standby_plugin.h"
#include "standby_service_client.h"
#include "standby_res_data.h"
#include "standby_service_log.h"
#include "config_info.h"
#include "plugin_mgr.h"
#include "res_type.h"

namespace OHOS {
namespace ResourceSchedule {
using namespace ResType;
namespace {
    const std::string LIB_NAME = "libdevice_standby_plugin.z.so";
}
IMPLEMENT_SINGLE_INSTANCE(DeviceStandbyPlugin)

void DeviceStandbyPlugin::Init()
{
    resTypes_.insert(RES_TYPE_APP_INSTALL_UNINSTALL);
    resTypes_.insert(RES_TYPE_SCREEN_STATUS);
    resTypes_.insert(RES_TYPE_TIME_CHANGED);
    resTypes_.insert(RES_TYPE_NITZ_TIME_CHANGED);
    resTypes_.insert(RES_TYPE_TIMEZONE_CHANGED);
    resTypes_.insert(RES_TYPE_NITZ_TIMEZONE_CHANGED);
    resTypes_.insert(RES_TYPE_CHARGING_DISCHARGING);
    resTypes_.insert(RES_TYPE_USB_DEVICE);
    resTypes_.insert(RES_TYPE_CALL_STATE_CHANGED);
    resTypes_.insert(RES_TYPE_WIFI_P2P_STATE_CHANGED);
    resTypes_.insert(RES_TYPE_POWER_MODE_CHANGED);
    resTypes_.insert(RES_TYPE_EFFICIENCY_RESOURCES_STATE_CHANGED);

    for (auto resType : resTypes_) {
        PluginMgr::GetInstance().SubscribeResource(LIB_NAME, resType);
    }
    STANDBYSERVICE_LOGI("DevicesStandbyPlugin::Init success");
}

void DeviceStandbyPlugin::Disable()
{
    for (auto resType : resTypes_) {
        PluginMgr::GetInstance().UnSubscribeResource(LIB_NAME, resType);
    }
    resTypes_.clear();
    STANDBYSERVICE_LOGI("DevicesStandbyPlugin::Disable success");
}

void DeviceStandbyPlugin::DispatchResource(const std::shared_ptr<ResData>& data)
{
    if (!data) {
        STANDBYSERVICE_LOGW("DeviceStandbyPlugin::DispatchResource data is null");
        return;
    }

    STANDBYSERVICE_LOGI(
        "DeviceStandbyPlugin::DispatchResource type=%{public}u value=%{public}lld",
        data->resType, (long long)(data->value));

    DevStandbyMgr::StandbyServiceClient::GetInstance().HandleEvent(
        std::make_shared<DevStandbyMgr::ResData>(data->resType, data->value, data->payload));
}

extern "C" bool OnPluginInit(std::string& libName)
{
    if (libName != LIB_NAME) {
        STANDBYSERVICE_LOGI("DeviceStandbyPlugin::OnPluginInit lib name is not match");
        return false;
    }
    DeviceStandbyPlugin::GetInstance().Init();
    STANDBYSERVICE_LOGI("DeviceStandbyPlugin::OnPluginInit success.");
    return true;
}

extern "C" void OnPluginDisable()
{
    DeviceStandbyPlugin::GetInstance().Disable();
    STANDBYSERVICE_LOGI("DeviceStandbyPlugin::OnPluginDisable success.");
}

extern "C" void OnDispatchResource(const std::shared_ptr<ResData>& data)
{
    DeviceStandbyPlugin::GetInstance().DispatchResource(data);
    STANDBYSERVICE_LOGD("DeviceStandbyPlugin::OnDispatchResource success.");
}
} // namespace ResourceSchedule
} // namespace OHOS
#endif