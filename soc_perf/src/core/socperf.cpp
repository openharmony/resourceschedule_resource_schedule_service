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

#include "socperf.h"
#include "hitrace_meter.h"

namespace OHOS {
namespace SOCPERF {
SocPerf::SocPerf()
{
}

SocPerf::~SocPerf()
{
}

bool SocPerf::Init()
{
    if (!LoadConfigXmlFile(SOCPERF_RESOURCE_CONFIG_XML)) {
        SOC_PERF_LOGE("Failed to load %{public}s", SOCPERF_RESOURCE_CONFIG_XML.c_str());
        return false;
    }

    if (!LoadConfigXmlFile(SOCPERF_BOOST_CONFIG_XML)) {
        SOC_PERF_LOGE("Failed to load %{public}s", SOCPERF_BOOST_CONFIG_XML.c_str());
        return false;
    }

    if (!LoadConfigXmlFile(SOCPERF_POWER_CONFIG_XML)) {
        SOC_PERF_LOGE("Failed to load %{public}s", SOCPERF_POWER_CONFIG_XML.c_str());
        return false;
    }

    if (!LoadConfigXmlFile(SOCPERF_THERMAL_CONFIG_XML)) {
        SOC_PERF_LOGE("Failed to load %{public}s", SOCPERF_THERMAL_CONFIG_XML.c_str());
        return false;
    }

    PrintCachedInfo();

    if (!CreateHandlers()) {
        SOC_PERF_LOGE("Failed to create handler threads");
        return false;
    }

    InitHandlerThreads();

    resNodeInfo.clear();
    govResNodeInfo.clear();
    resStrToIdInfo.clear();

    enabled = true;
    return true;
}

void SocPerf::PerfRequest(int32_t cmdId, const std::string& msg)
{
    if (!enabled) {
        SOC_PERF_LOGE("SocPerf disabled!");
        return;
    }
    if (perfActionInfo.find(cmdId) == perfActionInfo.end()
        || perfActionInfo[cmdId]->duration == 0) {
        SOC_PERF_LOGE("Invalid PerfRequest cmdId[%{public}d]", cmdId);
        return;
    }
    SOC_PERF_LOGI("%{public}s, cmdId[%{public}d]msg[%{public}s]", __func__, cmdId, msg.c_str());
    std::string trace_str(__func__);
    trace_str.append(",cmdId[").append(std::to_string(cmdId)).append("]");
    trace_str.append(",msg[").append(msg).append("]");
    StartTrace(HITRACE_TAG_OHOS, trace_str, -1);
    DoFreqAction(perfActionInfo[cmdId], EVENT_INVALID, ACTION_TYPE_PERF);
    FinishTrace(HITRACE_TAG_OHOS);
}

void SocPerf::PerfRequestEx(int32_t cmdId, bool onOffTag, const std::string& msg)
{
    if (!enabled) {
        SOC_PERF_LOGE("SocPerf disabled!");
        return;
    }
    if (perfActionInfo.find(cmdId) == perfActionInfo.end()) {
        SOC_PERF_LOGE("Invalid PerfRequestEx cmdId[%{public}d]", cmdId);
        return;
    }
    SOC_PERF_LOGI("%{public}s, cmdId[%{public}d]onOffTag[%{public}d]msg[%{public}s]",
        __func__, cmdId, onOffTag, msg.c_str());
    std::string trace_str(__func__);
    trace_str.append(",cmdId[").append(std::to_string(cmdId)).append("]");
    trace_str.append(",onOff[").append(std::to_string(onOffTag)).append("]");
    trace_str.append(",msg[").append(msg).append("]");
    StartTrace(HITRACE_TAG_OHOS, trace_str, -1);
    DoFreqAction(perfActionInfo[cmdId], onOffTag ? EVENT_ON : EVENT_OFF, ACTION_TYPE_PERF);
    FinishTrace(HITRACE_TAG_OHOS);
}

void SocPerf::PowerRequest(int32_t cmdId, const std::string& msg)
{
    if (!enabled) {
        SOC_PERF_LOGE("SocPerf disabled!");
        return;
    }
    if (powerActionInfo.find(cmdId) == powerActionInfo.end()
        || powerActionInfo[cmdId]->duration == 0) {
        SOC_PERF_LOGE("Invalid PowerRequest cmdId[%{public}d]", cmdId);
        return;
    }
    SOC_PERF_LOGI("%{public}s, cmdId[%{public}d]msg[%{public}s]", __func__, cmdId, msg.c_str());
    std::string trace_str(__func__);
    trace_str.append(",cmdId[").append(std::to_string(cmdId)).append("]");
    trace_str.append(",msg[").append(msg).append("]");
    StartTrace(HITRACE_TAG_OHOS, trace_str, -1);
    DoFreqAction(powerActionInfo[cmdId], EVENT_INVALID, ACTION_TYPE_POWER);
    FinishTrace(HITRACE_TAG_OHOS);
}

void SocPerf::PowerRequestEx(int32_t cmdId, bool onOffTag, const std::string& msg)
{
    if (!enabled) {
        SOC_PERF_LOGE("SocPerf disabled!");
        return;
    }
    if (powerActionInfo.find(cmdId) == powerActionInfo.end()) {
        SOC_PERF_LOGE("Invalid PowerRequestEx cmdId[%{public}d]", cmdId);
        return;
    }
    SOC_PERF_LOGI("%{public}s, cmdId[%{public}d]onOffTag[%{public}d]msg[%{public}s]",
        __func__, cmdId, onOffTag, msg.c_str());
    std::string trace_str(__func__);
    trace_str.append(",cmdId[").append(std::to_string(cmdId)).append("]");
    trace_str.append(",onOff[").append(std::to_string(onOffTag)).append("]");
    trace_str.append(",msg[").append(msg).append("]");
    StartTrace(HITRACE_TAG_OHOS, trace_str, -1);
    DoFreqAction(powerActionInfo[cmdId], onOffTag ? EVENT_ON : EVENT_OFF, ACTION_TYPE_POWER);
    FinishTrace(HITRACE_TAG_OHOS);
}

void SocPerf::PowerLimitBoost(bool onOffTag, const std::string& msg)
{
    if (!enabled) {
        SOC_PERF_LOGE("SocPerf disabled!");
        return;
    }
    SOC_PERF_LOGI("%{public}s, onOffTag[%{public}d]msg[%{public}s]", __func__, onOffTag, msg.c_str());
    std::string trace_str(__func__);
    trace_str.append(",onOff[").append(std::to_string(onOffTag)).append("]");
    trace_str.append(",msg[").append(msg).append("]");
    StartTrace(HITRACE_TAG_OHOS, trace_str, -1);
    for (auto handler : handlers) {
        auto event = AppExecFwk::InnerEvent::Get(INNER_EVENT_ID_POWER_LIMIT_BOOST_FREQ, onOffTag ? 1 : 0);
        handler->SendEvent(event);
    }
    FinishTrace(HITRACE_TAG_OHOS);
}

void SocPerf::ThermalRequest(int32_t cmdId, const std::string& msg)
{
    if (!enabled) {
        SOC_PERF_LOGE("SocPerf disabled!");
        return;
    }
    if (thermalActionInfo.find(cmdId) == thermalActionInfo.end()
        || thermalActionInfo[cmdId]->duration == 0) {
        SOC_PERF_LOGE("Invalid ThermalRequest cmdId[%{public}d]", cmdId);
        return;
    }
    SOC_PERF_LOGI("%{public}s, cmdId[%{public}d]msg[%{public}s]", __func__, cmdId, msg.c_str());
    std::string trace_str(__func__);
    trace_str.append(",cmdId[").append(std::to_string(cmdId)).append("]");
    trace_str.append(",msg[").append(msg).append("]");
    StartTrace(HITRACE_TAG_OHOS, trace_str, -1);
    DoFreqAction(thermalActionInfo[cmdId], EVENT_INVALID, ACTION_TYPE_THERMAL);
    FinishTrace(HITRACE_TAG_OHOS);
}

void SocPerf::ThermalRequestEx(int32_t cmdId, bool onOffTag, const std::string& msg)
{
    if (!enabled) {
        SOC_PERF_LOGE("SocPerf disabled!");
        return;
    }
    if (thermalActionInfo.find(cmdId) == thermalActionInfo.end()) {
        SOC_PERF_LOGE("Invalid ThermalRequestEx cmdId[%{public}d]", cmdId);
        return;
    }
    SOC_PERF_LOGI("%{public}s, cmdId[%{public}d]onOffTag[%{public}d]msg[%{public}s]",
        __func__, cmdId, onOffTag, msg.c_str());
    std::string trace_str(__func__);
    trace_str.append(",cmdId[").append(std::to_string(cmdId)).append("]");
    trace_str.append(",onOff[").append(std::to_string(onOffTag)).append("]");
    trace_str.append(",msg[").append(msg).append("]");
    StartTrace(HITRACE_TAG_OHOS, trace_str, -1);
    DoFreqAction(thermalActionInfo[cmdId], onOffTag ? EVENT_ON : EVENT_OFF, ACTION_TYPE_THERMAL);
    FinishTrace(HITRACE_TAG_OHOS);
}

void SocPerf::ThermalLimitBoost(bool onOffTag, const std::string& msg)
{
    if (!enabled) {
        SOC_PERF_LOGE("SocPerf disabled!");
        return;
    }
    SOC_PERF_LOGI("%{public}s, onOffTag[%{public}d]msg[%{public}s]", __func__, onOffTag, msg.c_str());
    std::string trace_str(__func__);
    trace_str.append(",onOff[").append(std::to_string(onOffTag)).append("]");
    trace_str.append(",msg[").append(msg).append("]");
    StartTrace(HITRACE_TAG_OHOS, trace_str, -1);
    for (auto handler : handlers) {
        auto event = AppExecFwk::InnerEvent::Get(INNER_EVENT_ID_THERMAL_LIMIT_BOOST_FREQ, onOffTag ? 1 : 0);
        handler->SendEvent(event);
    }
    FinishTrace(HITRACE_TAG_OHOS);
}

void SocPerf::DoFreqAction(std::shared_ptr<Action> action, int32_t onOff, int32_t actionType)
{
    for (int32_t i = 0; i < (int32_t)action->variable.size(); i += RES_ID_AND_VALUE_PAIR) {
        auto resAction = std::make_shared<ResAction>(action->variable[i + 1], action->duration, actionType, onOff);
        auto event = AppExecFwk::InnerEvent::Get(INNER_EVENT_ID_DO_FREQ_ACTION, resAction, action->variable[i]);
        handlers[action->variable[i] / RES_ID_NUMS_PER_TYPE - 1]->SendEvent(event);
    }
}

bool SocPerf::LoadConfigXmlFile(std::string configFile)
{
    xmlKeepBlanksDefault(0);
    xmlDoc* file = xmlReadFile(configFile.c_str(), nullptr, XML_PARSE_NOERROR | XML_PARSE_NOWARNING);
    if (!file) {
        SOC_PERF_LOGE("Failed to open xml file");
        return false;
    }
    xmlNode* rootNode = xmlDocGetRootElement(file);
    if (!rootNode) {
        SOC_PERF_LOGE("Failed to get xml file's RootNode");
        xmlFreeDoc(file);
        return false;
    }
    if (!xmlStrcmp(rootNode->name, reinterpret_cast<const xmlChar*>("Configs"))) {
        if (configFile == SOCPERF_RESOURCE_CONFIG_XML) {
            xmlNode* child = rootNode->children;
            for (; child; child = child->next) {
                if (!xmlStrcmp(child->name, reinterpret_cast<const xmlChar*>("Resource"))) {
                    if (!LoadResource(child, configFile)) {
                        xmlFreeDoc(file);
                        return false;
                    }
                } else if (!xmlStrcmp(child->name, reinterpret_cast<const xmlChar*>("GovResource"))) {
                    if (!LoadGovResource(child, configFile)) {
                        xmlFreeDoc(file);
                        return false;
                    }
                }
            }
        } else {
            if (!LoadCmd(rootNode, configFile)) {
                xmlFreeDoc(file);
                return false;
            }
        }
    } else {
        SOC_PERF_LOGE("Wrong format for xml file");
        xmlFreeDoc(file);
        return false;
    }
    xmlFreeDoc(file);
    SOC_PERF_LOGI("Success to Load %{public}s", configFile.c_str());
    return true;
}

bool SocPerf::CreateHandlers()
{
    handlers = std::vector<std::shared_ptr<SocPerfHandler>>(MAX_HANDLER_THREADS);
    std::string threadName = "socperf_handler";
    for (int32_t i = 0; i < (int32_t)handlers.size(); i++) {
        auto runner = AppExecFwk::EventRunner::Create(threadName);
        if (!runner) {
            SOC_PERF_LOGE("Failed to Create EventRunner");
            return false;
        }
        handlers[i] = std::make_shared<SocPerfHandler>(runner);
        SOC_PERF_LOGI("Success to Create SocPerfHandler");
    }
    SOC_PERF_LOGI("Success to Create All Handler threads");
    return true;
}

void SocPerf::InitHandlerThreads()
{
    for (auto iter = resNodeInfo.begin(); iter != resNodeInfo.end(); ++iter) {
        std::shared_ptr<ResNode> resNode = iter->second;
        auto event = AppExecFwk::InnerEvent::Get(INNER_EVENT_ID_INIT_RES_NODE_INFO, resNode);
        handlers[resNode->id / RES_ID_NUMS_PER_TYPE - 1]->SendEvent(event);
    }
    for (auto iter = govResNodeInfo.begin(); iter != govResNodeInfo.end(); ++iter) {
        std::shared_ptr<GovResNode> govResNode = iter->second;
        auto event = AppExecFwk::InnerEvent::Get(INNER_EVENT_ID_INIT_GOV_RES_NODE_INFO, govResNode);
        handlers[govResNode->id / RES_ID_NUMS_PER_TYPE - 1]->SendEvent(event);
    }
}

bool SocPerf::LoadResource(xmlNode* child, std::string configFile)
{
    xmlNode* grandson = child->children;
    for (; grandson; grandson = grandson->next) {
        if (!xmlStrcmp(grandson->name, reinterpret_cast<const xmlChar*>("res"))) {
            char* id = reinterpret_cast<char*>(xmlGetProp(grandson, reinterpret_cast<const xmlChar*>("id")));
            char* name = reinterpret_cast<char*>(xmlGetProp(grandson, reinterpret_cast<const xmlChar*>("name")));
            char* pair = reinterpret_cast<char*>(xmlGetProp(grandson, reinterpret_cast<const xmlChar*>("pair")));
            char* mode = reinterpret_cast<char*>(xmlGetProp(grandson, reinterpret_cast<const xmlChar*>("mode")));
            if (!CheckResourceTag(id, name, pair, mode, configFile)) {
                return false;
            }
            xmlNode* greatGrandson = grandson->children;
            std::shared_ptr<ResNode> resNode = std::make_shared<ResNode>(
                atoi(id), name, mode ? atoi(mode) : 0, pair ? atoi(pair) : INVALID_VALUE);
            char *def = nullptr;
            char *path  = nullptr;
            char *node  = nullptr;
            for (; greatGrandson; greatGrandson = greatGrandson->next) {
                if (!xmlStrcmp(greatGrandson->name, reinterpret_cast<const xmlChar*>("default"))) {
                    def = reinterpret_cast<char*>(xmlNodeGetContent(greatGrandson));
                } else if (!xmlStrcmp(greatGrandson->name, reinterpret_cast<const xmlChar*>("path"))) {
                    path = reinterpret_cast<char*>(xmlNodeGetContent(greatGrandson));
                } else if (!xmlStrcmp(greatGrandson->name, reinterpret_cast<const xmlChar*>("node"))) {
                    node = reinterpret_cast<char*>(xmlNodeGetContent(greatGrandson));
                }
            }
            if (!CheckResourceTag(def, path, configFile)) {
                return false;
            }
            resNode->def = atoi(def);
            resNode->path = path;
            if (node && !LoadResourceAvailable(resNode, node)) {
                SOC_PERF_LOGE("Invalid resource node for %{public}s", configFile.c_str());
                return false;
            }

            resStrToIdInfo.insert(std::pair<std::string, int32_t>(resNode->name, resNode->id));
            resNodeInfo.insert(std::pair<int32_t, std::shared_ptr<ResNode>>(resNode->id, resNode));
        }
    }

    if (!CheckPairResIdValid() || !CheckResDefValid()) {
        return false;
    }

    return true;
}

bool SocPerf::LoadGovResource(xmlNode* child, std::string configFile)
{
    xmlNode* grandson = child->children;
    for (; grandson; grandson = grandson->next) {
        if (!xmlStrcmp(grandson->name, reinterpret_cast<const xmlChar*>("res"))) {
            char* id = reinterpret_cast<char*>(xmlGetProp(grandson, reinterpret_cast<const xmlChar*>("id")));
            char* name = reinterpret_cast<char*>(xmlGetProp(grandson, reinterpret_cast<const xmlChar*>("name")));
            if (!CheckGovResourceTag(id, name, configFile)) {
                return false;
            }
            xmlNode* greatGrandson = grandson->children;
            std::shared_ptr<GovResNode> govResNode = std::make_shared<GovResNode>(atoi(id), name);
            for (; greatGrandson; greatGrandson = greatGrandson->next) {
                if (!xmlStrcmp(greatGrandson->name, reinterpret_cast<const xmlChar*>("default"))) {
                    char* def = reinterpret_cast<char*>(xmlNodeGetContent(greatGrandson));
                    if (!def || !IsNumber(def)) {
                        SOC_PERF_LOGE("Invalid governor resource default for %{public}s", configFile.c_str());
                        return false;
                    }
                    govResNode->def = atoi(def);
                } else if (!xmlStrcmp(greatGrandson->name, reinterpret_cast<const xmlChar*>("path"))) {
                    char* path = reinterpret_cast<char*>(xmlNodeGetContent(greatGrandson));
                    if (!path) {
                        SOC_PERF_LOGE("Invalid governor resource path for %{public}s", configFile.c_str());
                        return false;
                    }
                    govResNode->paths.push_back(path);
                } else if (!xmlStrcmp(greatGrandson->name, reinterpret_cast<const xmlChar*>("node"))) {
                    char* level = reinterpret_cast<char*>(
                        xmlGetProp(greatGrandson, reinterpret_cast<const xmlChar*>("level")));
                    char* node = reinterpret_cast<char*>(xmlNodeGetContent(greatGrandson));
                    if (!level || !IsNumber(level) || !node
                        || !LoadGovResourceAvailable(govResNode, level, node)) {
                        SOC_PERF_LOGE("Invalid governor resource node for %{public}s", configFile.c_str());
                        return false;
                    }
                }
            }

            resStrToIdInfo.insert(std::pair<std::string, int32_t>(govResNode->name, govResNode->id));
            govResNodeInfo.insert(std::pair<int32_t, std::shared_ptr<GovResNode>>(govResNode->id, govResNode));
        }
    }

    if (!CheckGovResDefValid()) {
        return false;
    }

    return true;
}

bool SocPerf::LoadCmd(xmlNode* rootNode, std::string configFile)
{
    xmlNode* child = rootNode->children;
    for (; child; child = child->next) {
        if (!xmlStrcmp(child->name, reinterpret_cast<const xmlChar*>("cmd"))) {
            char* id = reinterpret_cast<char*>(xmlGetProp(child, reinterpret_cast<const xmlChar*>("id")));
            char* name = reinterpret_cast<char*>(xmlGetProp(child, reinterpret_cast<const xmlChar*>("name")));
            if (!CheckCmdTag(id, name, configFile)) {
                return false;
            }
            xmlNode* grandson = child->children;
            std::shared_ptr<Action> action = std::make_shared<Action>(atoi(id), name);
            for (; grandson; grandson = grandson->next) {
                if (!xmlStrcmp(grandson->name, reinterpret_cast<const xmlChar*>("duration"))) {
                    char* duration = reinterpret_cast<char*>(xmlNodeGetContent(grandson));
                    if (!duration || !IsNumber(duration)) {
                        SOC_PERF_LOGE("Invalid cmd duration for %{public}s", configFile.c_str());
                        return false;
                    }
                    action->duration = atoi(duration);
                } else {
                    char* resStr = reinterpret_cast<char*>(const_cast<xmlChar*>(grandson->name));
                    char* resValue = reinterpret_cast<char*>(xmlNodeGetContent(grandson));
                    if (!resStr || resStrToIdInfo.find(resStr) == resStrToIdInfo.end()
                        || !resValue || !IsNumber(resValue)) {
                        SOC_PERF_LOGE("Invalid cmd resource(%{public}s) for %{public}s, cannot find resId",
                            resStr, configFile.c_str());
                        return false;
                    }
                    action->variable.push_back(resStrToIdInfo[resStr]);
                    action->variable.push_back(atoi(resValue));
                }
            }

            if (configFile == SOCPERF_BOOST_CONFIG_XML) {
                perfActionInfo.insert(std::pair<int32_t, std::shared_ptr<Action>>(action->id, action));
            } else if (configFile == SOCPERF_POWER_CONFIG_XML) {
                powerActionInfo.insert(std::pair<int32_t, std::shared_ptr<Action>>(action->id, action));
            } else if (configFile == SOCPERF_THERMAL_CONFIG_XML) {
                thermalActionInfo.insert(std::pair<int32_t, std::shared_ptr<Action>>(action->id, action));
            }
        }
    }

    if (!CheckActionResIdAndValueValid(configFile)) {
        return false;
    }

    return true;
}

bool SocPerf::CheckResourceTag(char* id, char* name, char* pair, char* mode, std::string configFile)
{
    if (!id || !IsNumber(id) || !IsValidResId(atoi(id))) {
        SOC_PERF_LOGE("Invalid resource id for %{public}s", configFile.c_str());
        return false;
    }
    if (!name) {
        SOC_PERF_LOGE("Invalid resource name for %{public}s", configFile.c_str());
        return false;
    }
    if (pair && (!IsNumber(pair) || !IsValidResId(atoi(pair)))) {
        SOC_PERF_LOGE("Invalid resource pair for %{public}s", configFile.c_str());
        return false;
    }
    if (mode && !IsNumber(mode)) {
        SOC_PERF_LOGE("Invalid resource mode for %{public}s", configFile.c_str());
        return false;
    }
    return true;
}

bool SocPerf::CheckResourceTag(char* def, char* path, std::string configFile)
{
    if (!def || !IsNumber(def)) {
        SOC_PERF_LOGE("Invalid resource default for %{public}s", configFile.c_str());
        return false;
    }
    if (!path) {
        SOC_PERF_LOGE("Invalid resource path for %{public}s", configFile.c_str());
        return false;
    }
    return true;
}

bool SocPerf::LoadResourceAvailable(std::shared_ptr<ResNode> resNode, char* node)
{
    std::string nodeStr = node;
    std::vector<std::string> result = Split(nodeStr, " ");
    for (auto str : result) {
        if (IsNumber(str)) {
            resNode->available.insert(stoi(str));
        } else {
            return false;
        }
    }
    return true;
}

bool SocPerf::CheckPairResIdValid()
{
    for (auto iter = resNodeInfo.begin(); iter != resNodeInfo.end(); ++iter) {
        int32_t resId = iter->first;
        std::shared_ptr<ResNode> resNode = iter->second;
        int32_t pairResId = resNode->pair;
        if (resNodeInfo.find(pairResId) == resNodeInfo.end()) {
            SOC_PERF_LOGE("resId[%{public}d]'s pairResId[%{public}d] is not valid", resId, pairResId);
            return false;
        }
    }
    return true;
}

bool SocPerf::CheckResDefValid()
{
    for (auto iter = resNodeInfo.begin(); iter != resNodeInfo.end(); ++iter) {
        int32_t resId = iter->first;
        std::shared_ptr<ResNode> resNode = iter->second;
        int32_t def = resNode->def;
        if (!resNode->available.empty() && resNode->available.find(def) == resNode->available.end()) {
            SOC_PERF_LOGE("resId[%{public}d]'s def[%{public}d] is not valid", resId, def);
            return false;
        }
    }
    return true;
}

bool SocPerf::CheckGovResourceTag(char* id, char* name, std::string configFile)
{
    if (!id || !IsNumber(id) || !IsValidResId(atoi(id))) {
        SOC_PERF_LOGE("Invalid governor resource id for %{public}s", configFile.c_str());
        return false;
    }
    if (!name) {
        SOC_PERF_LOGE("Invalid governor resource name for %{public}s", configFile.c_str());
        return false;
    }
    return true;
}

bool SocPerf::LoadGovResourceAvailable(std::shared_ptr<GovResNode> govResNode, char* level, char* node)
{
    govResNode->available.insert(atoi(level));
    std::string nodeStr = node;
    std::vector<std::string> result = Split(nodeStr, "|");
    if (result.size() != govResNode->paths.size()) {
        SOC_PERF_LOGE("Invalid governor resource node matches paths");
        return false;
    }
    govResNode->levelToStr.insert(std::pair<int32_t, std::vector<std::string>>(atoi(level), result));
    return true;
}

bool SocPerf::CheckGovResDefValid()
{
    for (auto iter = govResNodeInfo.begin(); iter != govResNodeInfo.end(); ++iter) {
        int32_t govResId = iter->first;
        std::shared_ptr<GovResNode> govResNode = iter->second;
        int32_t def = govResNode->def;
        if (govResNode->available.find(def) == govResNode->available.end()) {
            SOC_PERF_LOGE("govResId[%{public}d]'s def[%{public}d] is not valid", govResId, def);
            return false;
        }
    }
    return true;
}

bool SocPerf::CheckCmdTag(char* id, char* name, std::string configFile)
{
    if (!id || !IsNumber(id)) {
        SOC_PERF_LOGE("Invalid cmd id for %{public}s", configFile.c_str());
        return false;
    }
    if (!name) {
        SOC_PERF_LOGE("Invalid cmd name for %{public}s", configFile.c_str());
        return false;
    }
    return true;
}

bool SocPerf::CheckActionResIdAndValueValid(std::string configFile)
{
    std::unordered_map<int32_t, std::shared_ptr<Action>> actionInfo;
    if (configFile == SOCPERF_BOOST_CONFIG_XML) {
        actionInfo = perfActionInfo;
    } else if (configFile == SOCPERF_POWER_CONFIG_XML) {
        actionInfo = powerActionInfo;
    } else if (configFile == SOCPERF_THERMAL_CONFIG_XML) {
        actionInfo = thermalActionInfo;
    }
    for (auto iter = actionInfo.begin(); iter != actionInfo.end(); ++iter) {
        int32_t actionId = iter->first;
        std::shared_ptr<Action> action = iter->second;
        for (int32_t i = 0; i < (int32_t)action->variable.size(); i += RES_ID_AND_VALUE_PAIR) {
            int32_t resId = action->variable[i];
            int32_t resValue = action->variable[i + 1];
            if (resNodeInfo.find(resId) != resNodeInfo.end()) {
                if (!resNodeInfo[resId]->available.empty()
                    && resNodeInfo[resId]->available.find(resValue) == resNodeInfo[resId]->available.end()) {
                    SOC_PERF_LOGE("action[%{public}d]'s resValue[%{public}d] is not valid", actionId, resValue);
                    return false;
                }
            } else if (govResNodeInfo.find(resId) != govResNodeInfo.end()) {
                if (govResNodeInfo[resId]->available.find(resValue) == govResNodeInfo[resId]->available.end()) {
                    SOC_PERF_LOGE("action[%{public}d]'s resValue[%{public}d] is not valid", actionId, resValue);
                    return false;
                }
            } else {
                SOC_PERF_LOGE("action[%{public}d]'s resId[%{public}d] is not valid", actionId, resId);
                return false;
            }
        }
    }
    return true;
}

void SocPerf::PrintCachedInfo()
{
    SOC_PERF_LOGI("------------------------------------");
    SOC_PERF_LOGI("resNodeInfo(%{public}d)", (int32_t)resNodeInfo.size());
    for (auto iter = resNodeInfo.begin(); iter != resNodeInfo.end(); ++iter) {
        std::shared_ptr<ResNode> resNode = iter->second;
        resNode->PrintString();
    }
    SOC_PERF_LOGI("------------------------------------");
    SOC_PERF_LOGI("govResNodeInfo(%{public}d)", (int32_t)govResNodeInfo.size());
    for (auto iter = govResNodeInfo.begin(); iter != govResNodeInfo.end(); ++iter) {
        std::shared_ptr<GovResNode> govResNode = iter->second;
        govResNode->PrintString();
    }
    SOC_PERF_LOGI("------------------------------------");
    SOC_PERF_LOGI("perfActionInfo(%{public}d)", (int32_t)perfActionInfo.size());
    for (auto iter = perfActionInfo.begin(); iter != perfActionInfo.end(); ++iter) {
        std::shared_ptr<Action> action = iter->second;
        action->PrintString();
    }
    SOC_PERF_LOGI("------------------------------------");
    SOC_PERF_LOGI("powerActionInfo(%{public}d)", (int32_t)powerActionInfo.size());
    for (auto iter = powerActionInfo.begin(); iter != powerActionInfo.end(); ++iter) {
        std::shared_ptr<Action> action = iter->second;
        action->PrintString();
    }
    SOC_PERF_LOGI("------------------------------------");
    SOC_PERF_LOGI("thermalActionInfo(%{public}d)", (int32_t)thermalActionInfo.size());
    for (auto iter = thermalActionInfo.begin(); iter != thermalActionInfo.end(); ++iter) {
        std::shared_ptr<Action> action = iter->second;
        action->PrintString();
    }
    SOC_PERF_LOGI("------------------------------------");
}
} // namespace SOCPERF
} // namespace OHOS
