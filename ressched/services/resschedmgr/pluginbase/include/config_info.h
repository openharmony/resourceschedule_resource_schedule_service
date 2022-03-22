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

#ifndef RESSCHED_SERVICES_RESSCHEDMGR_PLUGINBASE_INCLUDE_CONFIG_INFO_H
#define RESSCHED_SERVICES_RESSCHEDMGR_PLUGINBASE_INCLUDE_CONFIG_INFO_H

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

#endif // RESSCHED_SERVICES_RESSCHEDMGR_PLUGINBASE_INCLUDE_CONFIG_INFO_H
