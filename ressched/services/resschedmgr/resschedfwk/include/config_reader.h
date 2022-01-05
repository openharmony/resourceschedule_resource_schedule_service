/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: load config file and create data struct for plugin
 */

#ifndef FOUNDATION_RESOURCESCHEDULE_SERVICES_RESSCHEDMGR_RESSCHEDFWK_INCLUDE_CONFIG_READER_H
#define FOUNDATION_RESOURCESCHEDULE_SERVICES_RESSCHEDMGR_RESSCHEDFWK_INCLUDE_CONFIG_READER_H

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

#endif // FOUNDATION_RESOURCESCHEDULE_SERVICES_RESSCHEDMGR_RESSCHEDFWK_INCLUDE_CONFIG_READER_H