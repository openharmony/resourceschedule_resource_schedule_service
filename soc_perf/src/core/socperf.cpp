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
#include "config_policy_utils.h"
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
    debugLogEnabled = HiLogIsLoggable(LOG_TAG_DOMAIN_ID_SOC_PERF, LOG_TAG_SOC_PERF, LOG_DEBUG);

    if (!LoadConfigXmlFile(SOCPERF_RESOURCE_CONFIG_XML)) {
        SOC_PERF_LOGE("%{public}s, Failed to load %{public}s", __func__, SOCPERF_RESOURCE_CONFIG_XML.c_str());
        return false;
    }

    if (!LoadConfigXmlFile(SOCPERF_BOOST_CONFIG_XML)) {
        SOC_PERF_LOGE("%{public}s, Failed to load %{public}s", __func__, SOCPERF_BOOST_CONFIG_XML.c_str());
        return false;
    }

    PrintCachedInfo();

    if (!CreateHandlers()) {
        SOC_PERF_LOGE("%{public}s, Failed to create handler threads", __func__);
        return false;
    }

    InitHandlerThreads();

    resNodeInfo.clear();
    govResNodeInfo.clear();
    resStrToIdInfo.clear();
    limitRequest = std::vector<std::unordered_map<int32_t, int32_t>>(ACTION_TYPE_MAX);

    enabled = true;

    if (debugLogEnabled) {
        SOC_PERF_LOGD("%{public}s, SocPerf Init SUCCESS!", __func__);
    }

    return true;
}

void SocPerf::PerfRequest(int32_t cmdId, const std::string& msg)
{
    if (!enabled) {
        SOC_PERF_LOGE("%{public}s, SocPerf disabled!", __func__);
        return;
    }
    if (perfActionsInfo.find(cmdId) == perfActionsInfo.end()) {
        SOC_PERF_LOGE("%{public}s, Invalid PerfRequest cmdId[%{public}d]", __func__, cmdId);
        return;
    }
    if (debugLogEnabled) {
        SOC_PERF_LOGD("%{public}s, cmdId[%{public}d]msg[%{public}s]", __func__, cmdId, msg.c_str());
    }
    std::string trace_str(__func__);
    trace_str.append(",cmdId[").append(std::to_string(cmdId)).append("]");
    trace_str.append(",msg[").append(msg).append("]");
    StartTrace(HITRACE_TAG_OHOS, trace_str, -1);
    DoFreqActions(perfActionsInfo[cmdId], EVENT_INVALID, ACTION_TYPE_PERF);
    FinishTrace(HITRACE_TAG_OHOS);
}

void SocPerf::PerfRequestEx(int32_t cmdId, bool onOffTag, const std::string& msg)
{
    if (!enabled) {
        SOC_PERF_LOGE("%{public}s, SocPerf disabled!", __func__);
        return;
    }
    if (perfActionsInfo.find(cmdId) == perfActionsInfo.end()) {
        SOC_PERF_LOGE("%{public}s, Invalid PerfRequestEx cmdId[%{public}d]", __func__, cmdId);
        return;
    }
    if (debugLogEnabled) {
        SOC_PERF_LOGD("%{public}s, cmdId[%{public}d]onOffTag[%{public}d]msg[%{public}s]",
            __func__, cmdId, onOffTag, msg.c_str());
    }
    std::string trace_str(__func__);
    trace_str.append(",cmdId[").append(std::to_string(cmdId)).append("]");
    trace_str.append(",onOff[").append(std::to_string(onOffTag)).append("]");
    trace_str.append(",msg[").append(msg).append("]");
    StartTrace(HITRACE_TAG_OHOS, trace_str, -1);
    DoFreqActions(perfActionsInfo[cmdId], onOffTag ? EVENT_ON : EVENT_OFF, ACTION_TYPE_PERF);
    FinishTrace(HITRACE_TAG_OHOS);
}

void SocPerf::PowerLimitBoost(bool onOffTag, const std::string& msg)
{
    if (!enabled) {
        SOC_PERF_LOGE("%{public}s, SocPerf disabled!", __func__);
        return;
    }
    if (debugLogEnabled) {
        SOC_PERF_LOGD("%{public}s, onOffTag[%{public}d]msg[%{public}s]", __func__, onOffTag, msg.c_str());
    }
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

void SocPerf::ThermalLimitBoost(bool onOffTag, const std::string& msg)
{
    if (!enabled) {
        SOC_PERF_LOGE("%{public}s, SocPerf disabled!", __func__);
        return;
    }
    if (debugLogEnabled) {
        SOC_PERF_LOGD("%{public}s, onOffTag[%{public}d]msg[%{public}s]", __func__, onOffTag, msg.c_str());
    }
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

void SocPerf::LimitRequest(int32_t clientId,
    const std::vector<int32_t>& tags, const std::vector<int64_t>& configs, const std::string& msg)
{
    if (!enabled) {
        SOC_PERF_LOGE("%{public}s, SocPerf disabled!", __func__);
        return;
    }
    if (tags.size() != configs.size()) {
        SOC_PERF_LOGE("%{public}s, tags'size and configs' size must be the same!", __func__);
        return;
    }
    if (clientId <= ACTION_TYPE_PERF || clientId >= ACTION_TYPE_MAX) {
        SOC_PERF_LOGE("%{public}s, clientId must be between ACTION_TYPE_PERF and ACTION_TYPE_MAX!", __func__);
        return;
    }
    if (debugLogEnabled) {
        for (int32_t i = 0; i < (int32_t)tags.size(); i++) {
            SOC_PERF_LOGD("%{public}s, clientId[%{public}d],tags[%{public}d],configs[%{public}lld],msg[%{public}s]",
                __func__, clientId, tags[i], (long long)configs[i], msg.c_str());
        }
    }
    for (int32_t i = 0; i < (int32_t)tags.size(); i++) {
        int32_t resId = tags[i];
        auto handler = GetHandlerByResId(resId);
        if (!handler) {
            continue;
        }
        int64_t resValue = configs[i];
        auto iter = limitRequest[clientId].find(resId);
        if (iter != limitRequest[clientId].end()
            && limitRequest[clientId][resId] != INVALID_VALUE) {
            auto resAction = std::make_shared<ResAction>(
                limitRequest[clientId][resId], 0, clientId, EVENT_OFF);
            auto event = AppExecFwk::InnerEvent::Get(INNER_EVENT_ID_DO_FREQ_ACTION, resAction, resId);
            handler->SendEvent(event);
            limitRequest[clientId].erase(iter);
        }
        if (resValue != INVALID_VALUE) {
            auto resAction = std::make_shared<ResAction>(resValue, 0, clientId, EVENT_ON);
            auto event = AppExecFwk::InnerEvent::Get(INNER_EVENT_ID_DO_FREQ_ACTION, resAction, resId);
            handler->SendEvent(event);
            limitRequest[clientId].insert(std::pair<int32_t, int32_t>(resId, resValue));
        }
    }
}

void SocPerf::DoFreqActions(std::shared_ptr<Actions> actions, int32_t onOff, int32_t actionType)
{
    for (auto iter = actions->actionList.begin(); iter != actions->actionList.end(); iter++) {
        std::shared_ptr<Action> action = *iter;
        for (int32_t i = 0; i < (int32_t)action->variable.size() - 1; i += RES_ID_AND_VALUE_PAIR) {
            auto handler = GetHandlerByResId(action->variable[i]);
            if (!handler) {
                continue;
            }
            auto resAction = std::make_shared<ResAction>(action->variable[i + 1], action->duration, actionType, onOff);
            auto event = AppExecFwk::InnerEvent::Get(INNER_EVENT_ID_DO_FREQ_ACTION, resAction, action->variable[i]);
            handler->SendEvent(event);
        }
    }
}

std::string SocPerf::GetRealConfigPath(const std::string configFile)
{
    char buf[PATH_MAX + 1];
    char* configFilePath = GetOneCfgFile(configFile.c_str(), buf, PATH_MAX + 1);
    char tmpPath[PATH_MAX + 1] = {0};
    if (!configFilePath || strlen(configFilePath) == 0 || strlen(configFilePath) > PATH_MAX ||
        !realpath(configFilePath, tmpPath)) {
        SOC_PERF_LOGE("%{public}s, load %{public}s file fail", __func__, configFile.c_str());
        return "";
    }
    return std::string(tmpPath);
}

std::shared_ptr<SocPerfHandler> SocPerf::GetHandlerByResId(int32_t resId)
{
    if (!IsValidResId(resId)) {
        return nullptr;
    }
    return handlers[resId / RES_ID_NUMS_PER_TYPE - 1];
}

bool SocPerf::LoadConfigXmlFile(std::string configFile)
{
    std::string realConfigFile = GetRealConfigPath(configFile);
    if (realConfigFile.size() <= 0) {
        return false;
    }
    xmlKeepBlanksDefault(0);
    xmlDoc* file = xmlReadFile(realConfigFile.c_str(), nullptr, XML_PARSE_NOERROR | XML_PARSE_NOWARNING);
    if (!file) {
        SOC_PERF_LOGE("%{public}s, Failed to open xml file", __func__);
        return false;
    }
    xmlNode* rootNode = xmlDocGetRootElement(file);
    if (!rootNode) {
        SOC_PERF_LOGE("%{public}s, Failed to get xml file's RootNode", __func__);
        xmlFreeDoc(file);
        return false;
    }
    if (!xmlStrcmp(rootNode->name, reinterpret_cast<const xmlChar*>("Configs"))) {
        if (realConfigFile.find(SOCPERF_RESOURCE_CONFIG_XML) != std::string::npos) {
            xmlNode* child = rootNode->children;
            for (; child; child = child->next) {
                if (!xmlStrcmp(child->name, reinterpret_cast<const xmlChar*>("Resource"))) {
                    if (!LoadResource(child, realConfigFile)) {
                        xmlFreeDoc(file);
                        return false;
                    }
                } else if (!xmlStrcmp(child->name, reinterpret_cast<const xmlChar*>("GovResource"))) {
                    if (!LoadGovResource(child, realConfigFile)) {
                        xmlFreeDoc(file);
                        return false;
                    }
                }
            }
        } else {
            if (!LoadCmd(rootNode, realConfigFile)) {
                xmlFreeDoc(file);
                return false;
            }
        }
    } else {
        SOC_PERF_LOGE("%{public}s, Wrong format for xml file", __func__);
        xmlFreeDoc(file);
        return false;
    }
    xmlFreeDoc(file);
    if (debugLogEnabled) {
        SOC_PERF_LOGD("%{public}s, Success to Load %{public}s", __func__, configFile.c_str());
    }
    return true;
}

bool SocPerf::CreateHandlers()
{
    handlers = std::vector<std::shared_ptr<SocPerfHandler>>(MAX_HANDLER_THREADS);
    std::string threadName = "socperf_handler";
    for (int32_t i = 0; i < (int32_t)handlers.size(); i++) {
        auto runner = AppExecFwk::EventRunner::Create(threadName);
        if (!runner) {
            SOC_PERF_LOGE("%{public}s, Failed to Create EventRunner", __func__);
            return false;
        }
        handlers[i] = std::make_shared<SocPerfHandler>(runner);
    }
    if (debugLogEnabled) {
        SOC_PERF_LOGD("%{public}s, Success to Create All Handler threads", __func__);
    }
    return true;
}

void SocPerf::InitHandlerThreads()
{
    for (auto iter = resNodeInfo.begin(); iter != resNodeInfo.end(); ++iter) {
        std::shared_ptr<ResNode> resNode = iter->second;
        auto handler = GetHandlerByResId(resNode->id);
        if (!handler) {
            continue;
        }
        auto event = AppExecFwk::InnerEvent::Get(INNER_EVENT_ID_INIT_RES_NODE_INFO, resNode);
        handler->SendEvent(event);
    }
    for (auto iter = govResNodeInfo.begin(); iter != govResNodeInfo.end(); ++iter) {
        std::shared_ptr<GovResNode> govResNode = iter->second;
        auto handler = GetHandlerByResId(govResNode->id);
        if (!handler) {
            continue;
        }
        auto event = AppExecFwk::InnerEvent::Get(INNER_EVENT_ID_INIT_GOV_RES_NODE_INFO, govResNode);
        handler->SendEvent(event);
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
            resNode->def = atoll(def);
            resNode->path = path;
            if (node && !LoadResourceAvailable(resNode, node)) {
                SOC_PERF_LOGE("%{public}s, Invalid resource node for %{public}s", __func__, configFile.c_str());
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
        if (xmlStrcmp(grandson->name, reinterpret_cast<const xmlChar*>("res"))) {
            continue;
        }
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
                    SOC_PERF_LOGE("%{public}s, Invalid governor resource default for %{public}s",
                        __func__, configFile.c_str());
                    return false;
                }
                govResNode->def = atoll(def);
            } else if (!xmlStrcmp(greatGrandson->name, reinterpret_cast<const xmlChar*>("path"))) {
                char* path = reinterpret_cast<char*>(xmlNodeGetContent(greatGrandson));
                if (!path) {
                    SOC_PERF_LOGE("%{public}s, Invalid governor resource path for %{public}s",
                        __func__, configFile.c_str());
                    return false;
                }
                govResNode->paths.push_back(path);
            } else if (!xmlStrcmp(greatGrandson->name, reinterpret_cast<const xmlChar*>("node"))) {
                char* level = reinterpret_cast<char*>(
                    xmlGetProp(greatGrandson, reinterpret_cast<const xmlChar*>("level")));
                char* node = reinterpret_cast<char*>(xmlNodeGetContent(greatGrandson));
                if (!level || !IsNumber(level) || !node
                    || !LoadGovResourceAvailable(govResNode, level, node)) {
                    SOC_PERF_LOGE("%{public}s, Invalid governor resource node for %{public}s",
                        __func__, configFile.c_str());
                    return false;
                }
            }
        }

        resStrToIdInfo.insert(std::pair<std::string, int32_t>(govResNode->name, govResNode->id));
        govResNodeInfo.insert(std::pair<int32_t, std::shared_ptr<GovResNode>>(govResNode->id, govResNode));
    }

    if (!CheckGovResDefValid()) {
        return false;
    }

    return true;
}

bool SocPerf::LoadCmd(xmlNode* rootNode, std::string configFile)
{
    xmlNode* child = rootNode->children;
    for (; child; child = child->next) { // Iterate all cmdID
        if (xmlStrcmp(child->name, reinterpret_cast<const xmlChar*>("cmd"))) {
            continue;
        }
        char* id = reinterpret_cast<char*>(xmlGetProp(child, reinterpret_cast<const xmlChar*>("id")));
        char* name = reinterpret_cast<char*>(xmlGetProp(child, reinterpret_cast<const xmlChar*>("name")));
        if (!CheckCmdTag(id, name, configFile)) {
            return false;
        }
        xmlNode* grandson = child->children;
        std::shared_ptr<Actions> actions = std::make_shared<Actions>(atoi(id), name);
        for (; grandson; grandson = grandson->next) { // Iterate all Action
            std::shared_ptr<Action> action = std::make_shared<Action>();
            xmlNode* greatGrandson = grandson->children;
            for (; greatGrandson; greatGrandson = greatGrandson->next) { // Iterate duration and all res
                if (!xmlStrcmp(greatGrandson->name, reinterpret_cast<const xmlChar*>("duration"))) {
                    char* duration = reinterpret_cast<char*>(xmlNodeGetContent(greatGrandson));
                    if (!duration || !IsNumber(duration)) {
                        SOC_PERF_LOGE("%{public}s, Invalid cmd duration for %{public}s",
                            __func__, configFile.c_str());
                        return false;
                    }
                    action->duration = atoi(duration);
                } else {
                    char* resStr = reinterpret_cast<char*>(const_cast<xmlChar*>(greatGrandson->name));
                    char* resValue = reinterpret_cast<char*>(xmlNodeGetContent(greatGrandson));
                    if (!resStr || resStrToIdInfo.find(resStr) == resStrToIdInfo.end()
                        || !resValue || !IsNumber(resValue)) {
                        SOC_PERF_LOGE("%{public}s, Invalid cmd resource(%{public}s) for %{public}s",
                            __func__, resStr, configFile.c_str());
                        return false;
                    }
                    action->variable.push_back(resStrToIdInfo[resStr]);
                    action->variable.push_back(atoll(resValue));
                }
            }
            actions->actionList.push_back(action);
        }

        if (configFile.find(SOCPERF_BOOST_CONFIG_XML) != std::string::npos) {
            perfActionsInfo.insert(std::pair<int32_t, std::shared_ptr<Actions>>(actions->id, actions));
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
        SOC_PERF_LOGE("%{public}s, Invalid resource id for %{public}s", __func__, configFile.c_str());
        return false;
    }
    if (!name) {
        SOC_PERF_LOGE("%{public}s, Invalid resource name for %{public}s", __func__, configFile.c_str());
        return false;
    }
    if (pair && (!IsNumber(pair) || !IsValidResId(atoi(pair)))) {
        SOC_PERF_LOGE("%{public}s, Invalid resource pair for %{public}s", __func__, configFile.c_str());
        return false;
    }
    if (mode && !IsNumber(mode)) {
        SOC_PERF_LOGE("%{public}s, Invalid resource mode for %{public}s", __func__, configFile.c_str());
        return false;
    }
    return true;
}

bool SocPerf::CheckResourceTag(char* def, char* path, std::string configFile)
{
    if (!def || !IsNumber(def)) {
        SOC_PERF_LOGE("%{public}s, Invalid resource default for %{public}s", __func__, configFile.c_str());
        return false;
    }
    if (!path) {
        SOC_PERF_LOGE("%{public}s, Invalid resource path for %{public}s", __func__, configFile.c_str());
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
            resNode->available.insert(stoll(str));
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
            SOC_PERF_LOGE("%{public}s, resId[%{public}d]'s pairResId[%{public}d] is not valid",
                __func__, resId, pairResId);
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
        int64_t def = resNode->def;
        if (!resNode->available.empty() && resNode->available.find(def) == resNode->available.end()) {
            SOC_PERF_LOGE("%{public}s, resId[%{public}d]'s def[%{public}lld] is not valid", __func__,
                resId, (long long)def);
            return false;
        }
    }
    return true;
}

bool SocPerf::CheckGovResourceTag(char* id, char* name, std::string configFile)
{
    if (!id || !IsNumber(id) || !IsValidResId(atoi(id))) {
        SOC_PERF_LOGE("%{public}s, Invalid governor resource id for %{public}s", __func__, configFile.c_str());
        return false;
    }
    if (!name) {
        SOC_PERF_LOGE("%{public}s, Invalid governor resource name for %{public}s", __func__, configFile.c_str());
        return false;
    }
    return true;
}

bool SocPerf::LoadGovResourceAvailable(std::shared_ptr<GovResNode> govResNode, char* level, char* node)
{
    govResNode->available.insert(atoll(level));
    std::string nodeStr = node;
    std::vector<std::string> result = Split(nodeStr, "|");
    if (result.size() != govResNode->paths.size()) {
        SOC_PERF_LOGE("%{public}s, Invalid governor resource node matches paths", __func__);
        return false;
    }
    govResNode->levelToStr.insert(std::pair<int32_t, std::vector<std::string>>(atoll(level), result));
    return true;
}

bool SocPerf::CheckGovResDefValid()
{
    for (auto iter = govResNodeInfo.begin(); iter != govResNodeInfo.end(); ++iter) {
        int32_t govResId = iter->first;
        std::shared_ptr<GovResNode> govResNode = iter->second;
        int32_t def = govResNode->def;
        if (govResNode->available.find(def) == govResNode->available.end()) {
            SOC_PERF_LOGE("%{public}s, govResId[%{public}d]'s def[%{public}d] is not valid", __func__, govResId, def);
            return false;
        }
    }
    return true;
}

bool SocPerf::CheckCmdTag(char* id, char* name, std::string configFile)
{
    if (!id || !IsNumber(id)) {
        SOC_PERF_LOGE("%{public}s, Invalid cmd id for %{public}s", __func__, configFile.c_str());
        return false;
    }
    if (!name) {
        SOC_PERF_LOGE("%{public}s, Invalid cmd name for %{public}s", __func__, configFile.c_str());
        return false;
    }
    return true;
}

bool SocPerf::CheckActionResIdAndValueValid(std::string configFile)
{
    std::unordered_map<int32_t, std::shared_ptr<Actions>> actionsInfo;
    if (configFile.find(SOCPERF_BOOST_CONFIG_XML) != std::string::npos) {
        actionsInfo = perfActionsInfo;
    }
    for (auto actionsIter = actionsInfo.begin(); actionsIter != actionsInfo.end(); ++actionsIter) {
        int32_t actionId = actionsIter->first;
        std::shared_ptr<Actions> actions = actionsIter->second;
        for (auto actionIter = actions->actionList.begin(); actionIter != actions->actionList.end(); ++actionIter) {
            std::shared_ptr<Action> action = *actionIter;
            for (int32_t i = 0; i < (int32_t)action->variable.size() - 1; i += RES_ID_AND_VALUE_PAIR) {
                int32_t resId = action->variable[i];
                int64_t resValue = action->variable[i + 1];
                if (resNodeInfo.find(resId) != resNodeInfo.end()) {
                    if (resNodeInfo[resId]->available.find(resValue) == resNodeInfo[resId]->available.end()) {
                        SOC_PERF_LOGE("%{public}s, action[%{public}d]'s resValue[%{public}lld] is not valid",
                            __func__, actionId, (long long)resValue);
                        return false;
                    }
                } else if (govResNodeInfo.find(resId) != govResNodeInfo.end()) {
                    if (govResNodeInfo[resId]->available.find(resValue) == govResNodeInfo[resId]->available.end()) {
                        SOC_PERF_LOGE("%{public}s, action[%{public}d]'s resValue[%{public}lld] is not valid",
                            __func__, actionId, (long long)resValue);
                        return false;
                    }
                } else {
                    SOC_PERF_LOGE("%{public}s, action[%{public}d]'s resId[%{public}d] is not valid",
                        __func__, actionId, resId);
                    return false;
                }
            }
        }
    }
    return true;
}

void SocPerf::PrintCachedInfo()
{
    if (debugLogEnabled) {
        SOC_PERF_LOGD("------------------------------------");
        SOC_PERF_LOGD("resNodeInfo(%{public}d)", (int32_t)resNodeInfo.size());
        for (auto iter = resNodeInfo.begin(); iter != resNodeInfo.end(); ++iter) {
            std::shared_ptr<ResNode> resNode = iter->second;
            resNode->PrintString();
        }
        SOC_PERF_LOGD("------------------------------------");
        SOC_PERF_LOGD("govResNodeInfo(%{public}d)", (int32_t)govResNodeInfo.size());
        for (auto iter = govResNodeInfo.begin(); iter != govResNodeInfo.end(); ++iter) {
            std::shared_ptr<GovResNode> govResNode = iter->second;
            govResNode->PrintString();
        }
        SOC_PERF_LOGD("------------------------------------");
        SOC_PERF_LOGD("perfActionsInfo(%{public}d)", (int32_t)perfActionsInfo.size());
        for (auto iter = perfActionsInfo.begin(); iter != perfActionsInfo.end(); ++iter) {
            std::shared_ptr<Actions> actions = iter->second;
            actions->PrintString();
        }
        SOC_PERF_LOGD("------------------------------------");
    }
}
} // namespace SOCPERF
} // namespace OHOS
