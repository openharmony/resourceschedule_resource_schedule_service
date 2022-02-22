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

#ifndef FOUNDATION_RESOURCESCHEDULE_SERVICES_RESSCHEDMGR_RESSCHEDFWK_INCLUDE_PLUGIN_MGR_H
#define FOUNDATION_RESOURCESCHEDULE_SERVICES_RESSCHEDMGR_RESSCHEDFWK_INCLUDE_PLUGIN_MGR_H

#include <functional>
#include <list>
#include <string>
#include <memory>
#include <map>
#include "event_handler.h"
#include "config_reader.h"
#include "plugin_switch.h"
#include "plugin.h"
#include "nocopyable.h"
#include "res_data.h"
#include "single_instance.h"
#include "config_info.h"

namespace OHOS {
namespace ResourceSchedule {
using OnDispatchResourceFunc = void (*)(const std::shared_ptr<ResData>&);
using OnPluginDisableFunc = void (*)();

struct PluginLib {
    std::shared_ptr<void> handle;
    OnDispatchResourceFunc onDispatchResourceFunc_;
    OnPluginDisableFunc onPluginDisableFunc_;
};

class PluginMgr {
    DECLARE_SINGLE_INSTANCE_BASE(PluginMgr);

public:
    ~PluginMgr();

    /**
     * Init pluginmanager, load xml config file, construct plugin instances.
     */
    void Init();

    /**
     * Disable all plugins, maybe service exception happens or stopped.
     */
    void Stop();

    /**
     * receive all reported resource data, then dispatch all plugins.
     *
     * @param resData Reported resource data.
     */
    void DispatchResource(const std::shared_ptr<ResData>& resData);

    /**
     * Subscribe resource type from plugin.
     *
     * @param pluginLib The lib name of plugin.
     * @param resType interested in resource type.
     */
    void SubscribeResource(const std::string& pluginLib, uint32_t resType);

    /**
     * Unsubscribe resource type from plugin.
     *
     * @param pluginLib The lib name of plugin.
     * @param resType interested in resource type.
     */
    void UnSubscribeResource(const std::string& pluginLib, uint32_t resType);

    PluginConfig GetConfig(const std::string& pluginName, const std::string& configName);

private:
    PluginMgr() = default;
    void OnDestroy();
    void LoadPlugin();
    void UnLoadPlugin();
    void ClearResource();
    void deliverResourceToPlugin(const std::string& pluginLib, const std::shared_ptr<ResData>& resData);

    using DlHandle = void*;

    static void CloseHandle(const DlHandle& handle);

    std::unique_ptr<ConfigReader> configReader_;
    std::unique_ptr<PluginSwitch> pluginSwitch_;

    std::mutex pluginMutex_;
    std::mutex DispatcherHandlerMutex_;
    std::map<std::string, PluginLib> pluginLibMap_;

    // mutex for resTypeMap_
    std::mutex resTypeMutex_;
    std::map<uint32_t, std::list<std::string>> resTypeLibMap_;

    // handler use for dispatch resource data
    std::shared_ptr<OHOS::AppExecFwk::EventHandler> dispatcherHandler_;
};
} // namespace ResourceSchedule
} // namespace OHOS

#endif // FOUNDATION_RESOURCESCHEDULE_SERVICES_RESSCHEDMGR_RESSCHEDFWK_INCLUDE_PLUGIN_MGR_H