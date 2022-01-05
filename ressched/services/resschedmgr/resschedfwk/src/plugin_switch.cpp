/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: PluginSwitch class Implement.
 */

#include "plugin_switch.h"
#include <map>
#include "res_sched_log.h"

using namespace std;

namespace OHOS {
namespace ResourceSchedule {
namespace {
constexpr auto XML_TAG_PLUGIN_LIST = "pluginlist";
constexpr auto XML_TAG_PLUGIN = "plugin";
constexpr auto XML_ATTR_LIB_PATH = "libpath";
constexpr auto XML_ATTR_SWITCH = "switch";
}

bool PluginSwitch::IsInvalidNode(const xmlNode& currNode)
{
    if (currNode.name == nullptr || currNode.type == XML_COMMENT_NODE) {
        return true;
    }
    return false;
}

bool PluginSwitch::LoadFromConfigFile(const string& configFile)
{
    // skip the empty string, else you will get empty node
    xmlDocPtr xmlDocPtr = xmlReadFile(configFile.c_str(), nullptr, XML_PARSE_NOBLANKS);
    if (xmlDocPtr == nullptr) {
        RESSCHED_LOGE("PluginSwitch::LoadFromConfigFile xmlReadFile error!");
        return false;
    }
    xmlNodePtr rootNodePtr = xmlDocGetRootElement(xmlDocPtr);
    if (rootNodePtr == nullptr || rootNodePtr->name == nullptr ||
        xmlStrcmp(rootNodePtr->name, reinterpret_cast<const xmlChar*>(XML_TAG_PLUGIN_LIST)) != 0) {
        RESSCHED_LOGE("PluginSwitch::LoadFromConfigFile root element tag wrong!");
        xmlFreeDoc(xmlDocPtr);
        return false;
    }

    std::list<PluginInfo> pluginInfoList;
    xmlNodePtr currNodePtr = rootNodePtr->xmlChildrenNode;
    for (; currNodePtr != nullptr; currNodePtr = currNodePtr->next) {
        if (IsInvalidNode(*currNodePtr)) {
            continue;
        }

        xmlChar *attrValue = nullptr;
        if (xmlStrcmp(currNodePtr->name, reinterpret_cast<const xmlChar*>(XML_TAG_PLUGIN)) == 0) {
            PluginInfo info;
            attrValue = xmlGetProp(currNodePtr, reinterpret_cast<const xmlChar*>(XML_ATTR_LIB_PATH));
            if (attrValue == nullptr) {
                RESSCHED_LOGW("PluginSwitch::LoadFromConfigFile libPath null!");
                continue;
            }
            info.libPath = reinterpret_cast<const char*>(attrValue);
            xmlFree(attrValue);

            attrValue = xmlGetProp(currNodePtr, reinterpret_cast<const xmlChar*>(XML_ATTR_SWITCH));
            if (attrValue != nullptr) {
                std::string value = reinterpret_cast<const char*>(attrValue);
                if (value == "1") {
                    info.switchOn = true;
                }
                xmlFree(attrValue);
            }
            pluginInfoList.emplace_back(info);
        } else {
            RESSCHED_LOGW("PluginSwitch::LoadFromConfigFile plugin (%{public}s) config wrong!", currNodePtr->name);
            xmlFreeDoc(xmlDocPtr);
            return false;
        }
    }
    xmlFreeDoc(xmlDocPtr);
    pluginInfoList_ = std::move(pluginInfoList);
    return true;
}

std::list<PluginInfo> PluginSwitch::GetPluginSwitch()
{
    return pluginInfoList_;
}

} // namespace ResourceSchedule
} // namespace OHOS