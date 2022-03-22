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

#ifndef RESSCHED_SERVICES_RESSCHEDMGR_RESSCHEDFWK_INCLUDE_PLUGIN_SWITCH_H
#define RESSCHED_SERVICES_RESSCHEDMGR_RESSCHEDFWK_INCLUDE_PLUGIN_SWITCH_H

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

#endif // RESSCHED_SERVICES_RESSCHEDMGR_RESSCHEDFWK_INCLUDE_PLUGIN_SWITCH_H
