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

#include "plugin_switch.h"
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
    if (!currNode.name || currNode.type == XML_COMMENT_NODE) {
        return true;
    }
    return false;
}

bool PluginSwitch::LoadFromConfigFile(const string& configFile)
{
    // skip the empty string, else you will get empty node
    xmlDocPtr xmlDocPtr = xmlReadFile(configFile.c_str(), nullptr,
        XML_PARSE_NOBLANKS | XML_PARSE_NOERROR | XML_PARSE_NOWARNING);
    if (!xmlDocPtr) {
        RESSCHED_LOGE("%{public}s, xmlReadFile error!", __func__);
        return false;
    }
    xmlNodePtr rootNodePtr = xmlDocGetRootElement(xmlDocPtr);
    if (!rootNodePtr || !rootNodePtr->name ||
        xmlStrcmp(rootNodePtr->name, reinterpret_cast<const xmlChar*>(XML_TAG_PLUGIN_LIST)) != 0) {
        RESSCHED_LOGE("%{public}s, root element tag wrong!", __func__);
        xmlFreeDoc(xmlDocPtr);
        return false;
    }

    std::list<PluginInfo> pluginInfoList;
    xmlNodePtr currNodePtr = rootNodePtr->xmlChildrenNode;
    for (; currNodePtr; currNodePtr = currNodePtr->next) {
        if (IsInvalidNode(*currNodePtr)) {
            continue;
        }

        xmlChar *attrValue = nullptr;
        if (xmlStrcmp(currNodePtr->name, reinterpret_cast<const xmlChar*>(XML_TAG_PLUGIN)) != 0) {
            RESSCHED_LOGW("%{public}s, plugin (%{public}s) config wrong!", __func__, currNodePtr->name);
            xmlFreeDoc(xmlDocPtr);
            return false;
        }
        PluginInfo info;
        attrValue = xmlGetProp(currNodePtr, reinterpret_cast<const xmlChar*>(XML_ATTR_LIB_PATH));
        if (!attrValue) {
            RESSCHED_LOGW("%{public}s, libPath null!", __func__);
            continue;
        }
        info.libPath = reinterpret_cast<const char*>(attrValue);
        xmlFree(attrValue);

        attrValue = xmlGetProp(currNodePtr, reinterpret_cast<const xmlChar*>(XML_ATTR_SWITCH));
        if (attrValue) {
            std::string value = reinterpret_cast<const char*>(attrValue);
            if (value == "1") {
                info.switchOn = true;
            }
            xmlFree(attrValue);
        }
        pluginInfoList.emplace_back(info);
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
