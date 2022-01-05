/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: load config file and create data struct for plugin
 */

#ifndef FOUNDATION_RESOURCESCHEDULE_SERVICES_RESSCHEDMGR_RESSCHEDFWK_INCLUDE_PLUGIN_SWITCH_H
#define FOUNDATION_RESOURCESCHEDULE_SERVICES_RESSCHEDMGR_RESSCHEDFWK_INCLUDE_PLUGIN_SWITCH_H

#include <memory>
#include <mutex>
#include "config_info.h"
#include "libxml/parser.h"
#include "libxml/xpath.h"

namespace OHOS {
namespace ResourceSchedule {

struct PluginInfo {
    std::string libPath;
    bool switchOn = false;
};

class PluginSwitch {
public:
    /**
     * Load the config file, parse the xml stream and construct the objects.
     *
     * @param configFile The xml filepath.
     * @return Returns true if parse successfully.
     */
    bool LoadFromConfigFile(const std::string& configFile);

    /**
     * Get all plugin info, about plugin switch status.
     *
     * @return Returns the plugin info.
     */
    std::list<PluginInfo> GetPluginSwitch();

private:
    static bool IsInvalidNode(const xmlNode& currNode);

    using PluginInfoMap = std::map<std::string, std::string>;

    std::list<PluginInfo> pluginInfoList_;
};

} // namespace ResourceSchedule
} // namespace OHOS

#endif // FOUNDATION_RESOURCESCHEDULE_SERVICES_RESSCHEDMGR_RESSCHEDFWK_INCLUDE_PLUGIN_SWITCH_H