/*
 * Copyright (c) 2022-2025 Huawei Device Co., Ltd.
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

#include "config_reader.h"
#include "res_sched_log.h"

using namespace std;

namespace OHOS {
namespace ResourceSchedule {
namespace {
static const char* XML_TAG_RES_SCHED = "ressched";
static const char* XML_TAG_PLUGIN = "plugin";
static const char* XML_TAG_CONFIG = "config";
static const char* XML_TAG_ITEM = "item";
static const char* XML_ATTR_NAME = "name";
}

bool ConfigReader::IsInvalidNode(const xmlNode& currNode)
{
    if (!currNode.name || currNode.type == XML_COMMENT_NODE) {
        return true;
    }
    return false;
}

void ConfigReader::ParseProperties(const xmlNode& currNode, map<string, string>& properties)
{
    auto attrs = currNode.properties;
    xmlChar *value;
    for (; attrs; attrs = attrs->next) {
        auto name = attrs->name;
        if (!name) {
            RESSCHED_LOGW("%{public}s, name null!", __func__);
            continue;
        }
        value = xmlGetProp(&currNode, name);
        if (!value) {
            RESSCHED_LOGW("%{public}s, name(%{public}s) value null!", __func__, name);
            continue;
        }
        properties[reinterpret_cast<const char*>(name)] = reinterpret_cast<const char*>(value);
        xmlFree(value);
    }
}

void ConfigReader::ParseSubItem(const xmlNode& parentNode, Item& item)
{
    auto currNodePtr = parentNode.xmlChildrenNode;
    xmlChar *value;
    for (; currNodePtr; currNodePtr = currNodePtr->next) {
        if (IsInvalidNode(*currNodePtr)) {
            RESSCHED_LOGW("%{public}s, skip invalid node!", __func__);
            continue;
        }
        SubItem subItem;
        ParseProperties(*currNodePtr, subItem.properties);
        subItem.name = reinterpret_cast<const char*>(currNodePtr->name);
        value = xmlNodeGetContent(currNodePtr);
        if (value) {
            string itemValue(reinterpret_cast<const char*>(value));
            subItem.value = std::move(itemValue);
            xmlFree(value);
        }
        item.subItemList.emplace_back(subItem);
    }
}

void ConfigReader::ParseItem(const xmlNode& parentNode, PluginConfig& pluginConfig)
{
    auto currNodePtr = parentNode.xmlChildrenNode;
    for (; currNodePtr; currNodePtr = currNodePtr->next) {
        if (IsInvalidNode(*currNodePtr) ||
            xmlStrcmp(currNodePtr->name, reinterpret_cast<const xmlChar*>(XML_TAG_ITEM)) != 0) {
            continue;
        }
        Item item;
        ParseProperties(*currNodePtr, item.itemProperties);
        ParseSubItem(*currNodePtr, item);
        pluginConfig.itemList.emplace_back(item);
    }
}

void ConfigReader::ParseConfig(const xmlNode& parentNode, PluginConfigMap& pluginConfigMap)
{
    auto currNodePtr = parentNode.xmlChildrenNode;
    for (; currNodePtr; currNodePtr = currNodePtr->next) {
        if (IsInvalidNode(*currNodePtr) ||
            xmlStrcmp(currNodePtr->name, reinterpret_cast<const xmlChar*>(XML_TAG_CONFIG)) != 0) {
            continue;
        }
        auto propName = xmlGetProp(currNodePtr, reinterpret_cast<const xmlChar*>(XML_ATTR_NAME));
        if (!propName) {
            RESSCHED_LOGW("%{public}s, propName null!", __func__);
            continue;
        }

        string configName(reinterpret_cast<char*>(propName));
        xmlFree(propName);
        auto& pluginConfig = pluginConfigMap[configName];
        ParseItem(*currNodePtr, pluginConfig);
    }
}

bool ConfigReader::ParsePluginConfig(const xmlNode& currNode, map<string, PluginConfigMap>& pluginConfigs)
{
    auto propName = xmlGetProp(&currNode, reinterpret_cast<const xmlChar*>(XML_ATTR_NAME));
    if (!propName) {
        RESSCHED_LOGW("%{public}s, propName null!", __func__);
        return false;
    }
    string pluginName(reinterpret_cast<char*>(propName));
    xmlFree(propName);
    ParseConfig(currNode, pluginConfigs[pluginName]);
    return true;
}

bool ConfigReader::LoadFromConfigContent(const string& content)
{
    // skip the empty string, else you will get empty node
    xmlDocPtr xmlDocPtr = xmlReadMemory(content.c_str(), content.length(), nullptr, nullptr,
        XML_PARSE_NOBLANKS | XML_PARSE_NOERROR | XML_PARSE_NOWARNING);
    if (!xmlDocPtr) {
        RESSCHED_LOGE("%{public}s, xmlReadFile error!", __func__);
        return false;
    }
    xmlNodePtr rootNodePtr = xmlDocGetRootElement(xmlDocPtr);
    if (!rootNodePtr || !rootNodePtr->name ||
        xmlStrcmp(rootNodePtr->name, reinterpret_cast<const xmlChar*>(XML_TAG_RES_SCHED)) != 0) {
        RESSCHED_LOGE("%{public}s, root element tag wrong!", __func__);
        xmlFreeDoc(xmlDocPtr);
        return false;
    }
    map<string, PluginConfigMap> allPluginConfigs;
    xmlNodePtr currNodePtr = rootNodePtr->xmlChildrenNode;
    for (; currNodePtr; currNodePtr = currNodePtr->next) {
        if (IsInvalidNode(*currNodePtr)) {
            continue;
        }
        bool ret = false;
        if (xmlStrcmp(currNodePtr->name, reinterpret_cast<const xmlChar*>(XML_TAG_PLUGIN)) == 0) {
            ret = ParsePluginConfig(*currNodePtr, allPluginConfigs);
        }
        if (!ret) {
            RESSCHED_LOGW("%{public}s, plugin (%{public}s) config wrong!", __func__, currNodePtr->name);
            xmlFreeDoc(xmlDocPtr);
            return false;
        }
    }
    xmlFreeDoc(xmlDocPtr);
    lock_guard<mutex> autoLock(configMutex_);
    MergeConfigList(allPluginConfigs);
    return true;
}

void ConfigReader::MergeConfigList(std::map<std::string, PluginConfigMap>& pluginConfigs)
{
    for (auto iter : pluginConfigs) {
        if (allPluginConfigs_.find(iter.first) == allPluginConfigs_.end()) {
            allPluginConfigs_[iter.first] = iter.second;
            continue;
        }
        MergePluginConfigMap(allPluginConfigs_[iter.first], iter.second);
    }
}

void ConfigReader::MergePluginConfigMap(PluginConfigMap& curPluginConfigMap,
    const PluginConfigMap& nextPluginConfigMap)
{
    for (auto iter : nextPluginConfigMap) {
        curPluginConfigMap[iter.first] = iter.second;
    }
}

PluginConfig ConfigReader::GetConfig(const std::string& pluginName, const std::string& configName)
{
    lock_guard<mutex> autoLock(configMutex_);
    PluginConfig config;
    auto itMap = allPluginConfigs_.find(pluginName);
    if (itMap == allPluginConfigs_.end()) {
        RESSCHED_LOGE("%{public}s, no pluginName:%{public}s config!", __func__, pluginName.c_str());
        return config;
    }
    PluginConfigMap configMap = allPluginConfigs_[pluginName];
    auto itConfig = configMap.find(configName);
    if (itConfig == configMap.end()) {
        RESSCHED_LOGE("%{public}s, pluginName:%{public}s config:%{public}s null!", __func__,
            pluginName.c_str(), configName.c_str());
        return config;
    }
    return configMap[configName];
}

void ConfigReader::RemoveConfig(const std::string& pluginName, const std::string& configName)
{
    lock_guard<mutex> autoLock(configMutex_);
    PluginConfig config;
    auto itMap = allPluginConfigs_.find(pluginName);
    if (itMap == allPluginConfigs_.end()) {
        RESSCHED_LOGE("%{public}s, no pluginName:%{public}s config!", __func__, pluginName.c_str());
        return;
    }
    PluginConfigMap& configMap = allPluginConfigs_[pluginName];
    auto itConfig = configMap.find(configName);
    if (itConfig == configMap.end()) {
        RESSCHED_LOGE("%{public}s, pluginName:%{public}s config:%{public}s null!", __func__,
            pluginName.c_str(), configName.c_str());
        return;
    }
    configMap.erase(configName);
    if (configMap.size() == 0) {
        allPluginConfigs_.erase(pluginName);
    }
}

void ConfigReader::Dump(std::string &result)
{
    result.append("================Resource Schedule Plugin Config ================\n");
    for (auto pluginIter : allPluginConfigs_) {
        result.append("pluginName:").append(pluginIter.first).append(" \n");
        auto pluginConfigMap = pluginIter.second;
        for (auto pluginConfigMapIter : pluginConfigMap) {
            result.append("configName:").append(pluginConfigMapIter.first).append(" \n");
            auto pluginConfig = pluginConfigMapIter.second;
            for (auto item : pluginConfig.itemList) {
                DumpItem(item, result);
            }
        }
    }
}

void ConfigReader::DumpItem(const Item& item, std::string &result)
{
    result.append("Item : ");
    for (auto itemProPerty : item.itemProperties) {
        result.append(itemProPerty.first).append(" = ").append(itemProPerty.second).append(" ");
    }
    result.append(" \n");
    for (auto subItem : item.subItemList) {
        result.append("subItemName : ").append(subItem.name).append(" ");
        for (auto subItemProPerty : subItem.properties) {
            result.append(subItemProPerty.first).append(" = ").append(subItemProPerty.second).append(" ");
        }
        result.append(" value :").append(subItem.value).append(" \n");
    }
}
} // namespace ResourceSchedule
} // namespace OHOS
