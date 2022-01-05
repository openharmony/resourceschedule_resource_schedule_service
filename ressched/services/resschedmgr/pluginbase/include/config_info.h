/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: base config info of plugin.
 */

#ifndef FOUNDATION_RESOURCESCHEDULE_SERVICES_RESSCHEDMGR_PLUGINBASE_INCLUDE_CONFIG_INFO_H
#define FOUNDATION_RESOURCESCHEDULE_SERVICES_RESSCHEDMGR_PLUGINBASE_INCLUDE_CONFIG_INFO_H

#include <list>
#include <map>
#include <memory>
#include <string>

namespace OHOS {
namespace ResourceSchedule {
struct SubItem {
    std::string name;
    std::string value;
    std::map<std::string, std::string> properties;
};

struct Item {
    std::map<std::string, std::string> itemProperties;
    std::list<SubItem> subItemList;
};

struct PluginConfig {
    std::list<Item> itemList;
};

using PluginConfigMap = std::map<std::string, PluginConfig>;

} // namespace ResourceSchedule
} // namespace OHOS

#endif // FOUNDATION_RESOURCESCHEDULE_SERVICES_RESSCHEDMGR_PLUGINBASE_INCLUDE_CONFIG_INFO_H