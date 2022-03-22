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

#ifndef RESSCHED_SERVICES_RESSCHEDMGR_RESSCHEDFWK_INCLUDE_CONFIG_READER_H
#define RESSCHED_SERVICES_RESSCHEDMGR_RESSCHEDFWK_INCLUDE_CONFIG_READER_H

#include <memory>
#include <mutex>
#include "config_info.h"
#include "libxml/parser.h"
#include "libxml/xpath.h"

namespace OHOS {
namespace ResourceSchedule {
class ConfigReader {
public:
    /**
     * Load the config file, parse the xml stream and construct the objects.
     *
     * @param configFile The xml filepath.
     * @return Returns true if parse successfully.
     */
    bool LoadFromCustConfigFile(const std::string& configFile);

    /**
     * Xml parse successfully, the config object has resolved.
     *
     * @param pluginName The plugin name.
     * @param configName The config name.
     * @return Returns the config.
     */
    PluginConfig GetConfig(const std::string& pluginName, const std::string& configName);

private:
    static bool IsInvalidNode(const xmlNode& currNode);
    void ParseProperties(const xmlNode& currNode, std::map<std::string, std::string>& properties);
    void ParseSubItem(const xmlNode& currNode, Item& item);
    void ParseItem(const xmlNode& parentNode, PluginConfig& pluginConfig);
    void ParseConfig(const xmlNode& parentNode, PluginConfigMap& pluginConfigMap);
    bool ParsePluginConfig(const xmlNode& currNode, std::map<std::string, PluginConfigMap>& pluginConfigs);

    std::mutex configMutex_;
    std::map<std::string, PluginConfigMap> allPluginConfigs_;
};
} // namespace ResourceSchedule
} // namespace OHOS

#endif // RESSCHED_SERVICES_RESSCHEDMGR_RESSCHEDFWK_INCLUDE_CONFIG_READER_H
