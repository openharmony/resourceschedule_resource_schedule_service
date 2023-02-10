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

#ifndef RESSCHED_SERVICES_RESSCHEDMGR_RESSCHEDFWK_INCLUDE_PLUGIN_MGR_H
#define RESSCHED_SERVICES_RESSCHEDMGR_RESSCHEDFWK_INCLUDE_PLUGIN_MGR_H

#include <functional>
#include <list>
#include <string>
#include <memory>
#include <map>
#include "datetime_ex.h"
#include "event_handler.h"
#include "config_reader.h"
#include "plugin_switch.h"
#include "plugin.h"
#include "nocopyable.h"
#include "res_data.h"
#include "res_type.h"
#include "single_instance.h"
#include "config_info.h"

namespace OHOS {
namespace ResourceSchedule {
using Clock = std::chrono::high_resolution_clock;
using TimePoint = std::chrono::time_point<Clock>;
using OnPluginInitFunc = bool (*)(const std::string);
using OnDispatchResourceFunc = void (*)(const std::shared_ptr<ResData>&);
using OnDumpFunc = void (*)(const std::vector<std::string>&, std::string&);
using OnPluginDisableFunc = void (*)();

struct PluginLib {
    std::shared_ptr<void> handle = nullptr;
    OnPluginInitFunc onPluginInitFunc_;
    OnDispatchResourceFunc onDispatchResourceFunc_;
    OnDumpFunc onDumpFunc_;
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

    void DumpAllPlugin(std::string &result);

    void DumpOnePlugin(std::string &result, std::string pluginName, std::vector<std::string>& args);

    std::string DumpInfoFromPlugin(std::string& result, std::string libPath, std::vector<std::string>& args);

    void DumpHelpFromPlugin(std::string& result);

    PluginConfig GetConfig(const std::string& pluginName, const std::string& configName);

private:
    PluginMgr() = default;
    std::string GetRealConfigPath(const char* configName);
    void OnDestroy();
    void LoadPlugin();
    std::shared_ptr<PluginLib> LoadOnePlugin(const PluginInfo& info);
    void UnLoadPlugin();
    void ClearResource();
    void deliverResourceToPlugin(const std::string& pluginLib, const std::shared_ptr<ResData>& resData);
    void RepairPlugin(TimePoint endTime, const std::string& pluginLib, PluginLib libInfo);
    void RemoveDisablePluginHandler();
    void DumpPluginInfoAppend(std::string &result, PluginInfo info);

    // plugin crash 3 times in 60s, will be disable forever
    const int32_t MAX_PLUGIN_TIMEOUT_TIMES = 3;
    const int32_t DISABLE_PLUGIN_TIME = 60000;
    const int32_t DUMP_ONE_STRING_SIZE = 32;
    std::unique_ptr<ConfigReader> configReader_ = nullptr;
    std::unique_ptr<PluginSwitch> pluginSwitch_ = nullptr;

    std::mutex pluginMutex_;
    std::mutex dispatcherHandlerMutex_;
    std::map<std::string, PluginLib> pluginLibMap_;

    // mutex for resTypeMap_
    std::mutex resTypeMutex_;
    std::map<uint32_t, std::list<std::string>> resTypeLibMap_;

    // handler map use for dispatch resource data
    std::map<std::string, std::shared_ptr<OHOS::AppExecFwk::EventHandler>> dispatcherHandlerMap_;
    int32_t handlerNum_ = 0;
    std::map<std::string, std::list<TimePoint>> pluginTimeoutTime_;
    std::list<std::string> disablePlugins_;
    std::mutex disablePluginsMutex_;
};
} // namespace ResourceSchedule
} // namespace OHOS

#endif // RESSCHED_SERVICES_RESSCHEDMGR_RESSCHEDFWK_INCLUDE_PLUGIN_MGR_H
