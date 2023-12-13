/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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
#include "time_service_client.h"

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

    PrintCachedInfo();

    if (!CreateThreadWraps()) {
        SOC_PERF_LOGE("Failed to create threadwraps threads");
        return false;
    }

    InitThreadWraps();

    for (auto threadWrap : socperfThreadWraps) {
        if (threadWrap && perfSoPath && perfSoFunc) {
            threadWrap->InitPerfFunc(perfSoPath, perfSoFunc);
        }
    }

    xmlFree(perfSoPath);
    xmlFree(perfSoFunc);
    resNodeInfo.clear();
    govResNodeInfo.clear();
    resStrToIdInfo.clear();
    limitRequest = std::vector<std::unordered_map<int32_t, int32_t>>(ACTION_TYPE_MAX);

    enabled = true;

    SOC_PERF_LOGD("SocPerf Init SUCCESS!");

    return true;
}

void SocPerf::PerfRequest(int32_t cmdId, const std::string& msg)
{
    if (!enabled) {
        SOC_PERF_LOGE("SocPerf disabled!");
        return;
    }
    if (perfActionsInfo.find(cmdId) == perfActionsInfo.end()) {
        SOC_PERF_LOGE("Invalid PerfRequest cmdId[%{public}d]", cmdId);
        return;
    }
    SOC_PERF_LOGD("cmdId[%{public}d]msg[%{public}s]", cmdId, msg.c_str());

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
        SOC_PERF_LOGE("SocPerf disabled!");
        return;
    }
    if (perfActionsInfo.find(cmdId) == perfActionsInfo.end()) {
        SOC_PERF_LOGE("Invalid PerfRequestEx cmdId[%{public}d]", cmdId);
        return;
    }
    SOC_PERF_LOGD("cmdId[%{public}d]onOffTag[%{public}d]msg[%{public}s]",
        cmdId, onOffTag, msg.c_str());

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
        SOC_PERF_LOGE("SocPerf disabled!");
        return;
    }
    SOC_PERF_LOGD("onOffTag[%{public}d]msg[%{public}s]", onOffTag, msg.c_str());

    std::string trace_str(__func__);
    trace_str.append(",onOff[").append(std::to_string(onOffTag)).append("]");
    trace_str.append(",msg[").append(msg).append("]");
    StartTrace(HITRACE_TAG_OHOS, trace_str, -1);
    for (auto threadWrap : socperfThreadWraps) {
        if (threadWrap) {
            threadWrap->UpdatePowerLimitBoostFreq(onOffTag);
        }
    }
    FinishTrace(HITRACE_TAG_OHOS);
}

void SocPerf::ThermalLimitBoost(bool onOffTag, const std::string& msg)
{
    if (!enabled) {
        SOC_PERF_LOGE("SocPerf disabled!");
        return;
    }
    SOC_PERF_LOGD("onOffTag[%{public}d]msg[%{public}s]", onOffTag, msg.c_str());
    std::string trace_str(__func__);
    trace_str.append(",onOff[").append(std::to_string(onOffTag)).append("]");
    trace_str.append(",msg[").append(msg).append("]");
    StartTrace(HITRACE_TAG_OHOS, trace_str, -1);
    for (auto threadWrap : socperfThreadWraps) {
        if (threadWrap) {
            threadWrap->UpdateThermalLimitBoostFreq(onOffTag);
        }
    }
    FinishTrace(HITRACE_TAG_OHOS);
}

void SocPerf::SendLimitRequestEventOff(std::shared_ptr<SocPerfThreadWrap> threadWrap,
    int32_t clientId, int32_t resId, int32_t eventId)
{
    auto iter = limitRequest[clientId].find(resId);
    if (iter != limitRequest[clientId].end()
        && limitRequest[clientId][resId] != INVALID_VALUE) {
        auto resAction = std::make_shared<ResAction>(
            limitRequest[clientId][resId], 0, clientId, EVENT_OFF, -1, MAX_INT_VALUE);
        threadWrap->UpdateLimitStatus(eventId, resAction, resId);
        limitRequest[clientId].erase(iter);
    }
}

void SocPerf::SendLimitRequestEventOn(std::shared_ptr<SocPerfThreadWrap> threadWrap,
    int32_t clientId, int32_t resId, int64_t resValue, int32_t eventId)
{
    if (resValue != INVALID_VALUE && resValue != RESET_VALUE) {
        auto resAction = std::make_shared<ResAction>(resValue, 0, clientId, EVENT_ON, -1, MAX_INT_VALUE);
        threadWrap->UpdateLimitStatus(eventId, resAction, resId);
        limitRequest[clientId].insert(std::pair<int32_t, int32_t>(resId, resValue));
    }
}

void SocPerf::SendLimitRequestEvent(int32_t clientId, int32_t resId, int64_t resValue)
{
    int32_t eventId, realResId, levelResId;
    if (resId > RES_ID_ADDITION) {
        realResId = resId - RES_ID_ADDITION;
        levelResId = resId;
        eventId = INNER_EVENT_ID_DO_FREQ_ACTION_LEVEL;
    } else {
        realResId = resId;
        levelResId = resId + RES_ID_ADDITION;
        eventId = INNER_EVENT_ID_DO_FREQ_ACTION;
    }

    auto threadWrap = GetThreadWrapByResId(realResId);
    if (!threadWrap) {
        return;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    SendLimitRequestEventOff(threadWrap, clientId, realResId, INNER_EVENT_ID_DO_FREQ_ACTION);
    SendLimitRequestEventOff(threadWrap, clientId, levelResId, INNER_EVENT_ID_DO_FREQ_ACTION_LEVEL);
    SendLimitRequestEventOn(threadWrap, clientId, resId, resValue, eventId);
}

void SocPerf::LimitRequest(int32_t clientId,
    const std::vector<int32_t>& tags, const std::vector<int64_t>& configs, const std::string& msg)
{
    if (!enabled) {
        SOC_PERF_LOGE("SocPerf disabled!");
        return;
    }
    if (tags.size() != configs.size()) {
        SOC_PERF_LOGE("tags'size and configs' size must be the same!");
        return;
    }
    if (clientId <= (int32_t)ACTION_TYPE_PERF || clientId >= (int32_t)ACTION_TYPE_MAX) {
        SOC_PERF_LOGE("clientId must be between ACTION_TYPE_PERF and ACTION_TYPE_MAX!");
        return;
    }
    for (int32_t i = 0; i < (int32_t)tags.size(); i++) {
        SOC_PERF_LOGD("clientId[%{public}d],tags[%{public}d],configs[%{public}lld],msg[%{public}s]",
            clientId, tags[i], (long long)configs[i], msg.c_str());
        SendLimitRequestEvent(clientId, tags[i], configs[i]);
    }
}

void SocPerf::DoFreqActions(std::shared_ptr<Actions> actions, int32_t onOff, int32_t actionType)
{
    std::shared_ptr<ResActionItem> header[MAX_QUEUE_NUM] = { nullptr };
    std::shared_ptr<ResActionItem> curItem[MAX_QUEUE_NUM] = { nullptr };
    int64_t curMs = MiscServices::TimeServiceClient::GetInstance()->GetBootTimeMs();
    for (auto iter = actions->actionList.begin(); iter != actions->actionList.end(); iter++) {
        std::shared_ptr<Action> action = *iter;
        for (int32_t i = 0; i < (int32_t)action->variable.size() - 1; i += RES_ID_AND_VALUE_PAIR) {
            if (!IsValidResId(action->variable[i])) {
                continue;
            }
            auto resActionItem = std::make_shared<ResActionItem>(action->variable[i]);
            int64_t endTime = action->duration == 0 ? MAX_INT_VALUE : curMs + action->duration;
            resActionItem->resAction =
                std::make_shared<ResAction>(action->variable[i + 1], action->duration,
                    actionType, onOff, actions->id, endTime);
            int32_t id = action->variable[i] / RES_ID_NUMS_PER_TYPE - 1;
            if (curItem[id]) {
                curItem[id]->next = resActionItem;
            } else {
                header[id] = resActionItem;
            }
            curItem[id] = resActionItem;
        }
    }
    for (int32_t i = 0; i < MAX_QUEUE_NUM; ++i) {
        if (!socperfThreadWraps[i] || !header[i]) {
            continue;
        }
        socperfThreadWraps[i]->DoFreqActionPack(header[i]);
    }
}

std::string SocPerf::GetRealConfigPath(const std::string& configFile)
{
    char buf[PATH_MAX + 1];
    char* configFilePath = GetOneCfgFile(configFile.c_str(), buf, PATH_MAX + 1);
    char tmpPath[PATH_MAX + 1] = {0};
    if (!configFilePath || strlen(configFilePath) == 0 || strlen(configFilePath) > PATH_MAX ||
        !realpath(configFilePath, tmpPath)) {
        SOC_PERF_LOGE("load %{public}s file fail", configFile.c_str());
        return "";
    }
    return std::string(tmpPath);
}

std::shared_ptr<SocPerfThreadWrap> SocPerf::GetThreadWrapByResId(int32_t resId) const
{
    if (!IsValidResId(resId)) {
        return nullptr;
    }
    return socperfThreadWraps[resId / RES_ID_NUMS_PER_TYPE - 1];
}

bool SocPerf::LoadConfigXmlFile(const std::string& configFile)
{
    std::string realConfigFile = GetRealConfigPath(configFile);
    if (realConfigFile.size() <= 0) {
        return false;
    }
    xmlKeepBlanksDefault(0);
    xmlDoc* file = xmlReadFile(realConfigFile.c_str(), nullptr, XML_PARSE_NOERROR | XML_PARSE_NOWARNING);
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
        if (realConfigFile.find(SOCPERF_RESOURCE_CONFIG_XML) != std::string::npos) {
            bool ret = ParseResourceXmlFile(rootNode, realConfigFile, file);
            if (!ret) {
                return false;
            }
        } else {
            bool ret = ParseBoostXmlFile(rootNode, realConfigFile, file);
            if (!ret) {
                return false;
            }
        }
    } else {
        SOC_PERF_LOGE("Wrong format for xml file");
        xmlFreeDoc(file);
        return false;
    }
    xmlFreeDoc(file);
    SOC_PERF_LOGD("Success to Load %{public}s", configFile.c_str());
    return true;
}

bool SocPerf::ParseBoostXmlFile(const xmlNode* rootNode, const std::string& realConfigFile, xmlDoc* file)
{
    if (!LoadCmd(rootNode, realConfigFile)) {
        xmlFreeDoc(file);
        return false;
    }
    return true;
}

bool SocPerf::ParseResourceXmlFile(const xmlNode* rootNode, const std::string& realConfigFile, xmlDoc* file)
{
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
        } else if (!xmlStrcmp(child->name, reinterpret_cast<const xmlChar*>("Info"))) {
            LoadInfo(child, realConfigFile);
        }
    }
    return true;
}

bool SocPerf::CreateThreadWraps()
{
    socperfThreadWraps = std::vector<std::shared_ptr<SocPerfThreadWrap>>(MAX_QUEUE_NUM);
    std::string threadName = "socperf#";
    for (int32_t i = 0; i < (int32_t)socperfThreadWraps.size(); i++) {
        if (!wrapSwitch[i]) {
            socperfThreadWraps[i] = nullptr;
            continue;
        }
        auto socPerfThreadWrap = std::make_shared<SocPerfThreadWrap>();
        if (!socPerfThreadWrap) {
            SOC_PERF_LOGE("Failed to Create socPerfThreadWrap");
            return false;
        }
        socPerfThreadWrap->InitQueue(threadName.append(std::to_string(i)));
        socperfThreadWraps[i] = socPerfThreadWrap;
    }
    SOC_PERF_LOGD("Success to Create All threadWrap threads");
    return true;
}

void SocPerf::InitThreadWraps()
{
    for (auto iter = resNodeInfo.begin(); iter != resNodeInfo.end(); ++iter) {
        std::shared_ptr<ResNode> resNode = iter->second;
        auto threadWrap = GetThreadWrapByResId(resNode->id);
        if (!threadWrap) {
            continue;
        }
        threadWrap->InitResNodeInfo(resNode);
    }
    for (auto iter = govResNodeInfo.begin(); iter != govResNodeInfo.end(); ++iter) {
        std::shared_ptr<GovResNode> govResNode = iter->second;
        auto threadWrap = GetThreadWrapByResId(govResNode->id);
        if (!threadWrap) {
            continue;
        }
        threadWrap->InitGovResNodeInfo(govResNode);
    }
}

bool SocPerf::LoadResource(xmlNode* child, const std::string& configFile)
{
    xmlNode* grandson = child->children;
    for (; grandson; grandson = grandson->next) {
        if (!xmlStrcmp(grandson->name, reinterpret_cast<const xmlChar*>("res"))) {
            if (!TraversalFreqResource(grandson, configFile)) {
                return false;
            }
        }
    }

    if (!CheckPairResIdValid() || !CheckResDefValid()) {
        return false;
    }

    return true;
}

bool SocPerf::TraversalFreqResource(xmlNode* grandson, const std::string& configFile)
{
    char* id = reinterpret_cast<char*>(xmlGetProp(grandson, reinterpret_cast<const xmlChar*>("id")));
    char* name = reinterpret_cast<char*>(xmlGetProp(grandson, reinterpret_cast<const xmlChar*>("name")));
    char* pair = reinterpret_cast<char*>(xmlGetProp(grandson, reinterpret_cast<const xmlChar*>("pair")));
    char* mode = reinterpret_cast<char*>(xmlGetProp(grandson, reinterpret_cast<const xmlChar*>("mode")));
    char* persistMode = reinterpret_cast<char*>(xmlGetProp(grandson, reinterpret_cast<const xmlChar*>("switch")));
    if (!CheckResourceTag(id, name, pair, mode, persistMode, configFile)) {
        xmlFree(id);
        xmlFree(name);
        xmlFree(pair);
        xmlFree(mode);
        xmlFree(persistMode);
        return false;
    }
    xmlNode* greatGrandson = grandson->children;
    std::shared_ptr<ResNode> resNode = std::make_shared<ResNode>(atoi(id), name, mode ? atoi(mode) : 0,
        pair ? atoi(pair) : INVALID_VALUE, persistMode ? atoi(persistMode) : 0);
    xmlFree(id);
    xmlFree(name);
    xmlFree(pair);
    xmlFree(mode);
    xmlFree(persistMode);
    if (!LoadFreqResourceContent(greatGrandson, configFile, resNode)) {
        return false;
    }
    return true;
}

bool SocPerf::LoadFreqResourceContent(xmlNode* greatGrandson, const std::string& configFile,
    std::shared_ptr<ResNode> resNode)
{
    char *def = nullptr;
    char *path  = nullptr;
    char *node  = nullptr;
    for (; greatGrandson; greatGrandson = greatGrandson->next) {
        if (!xmlStrcmp(greatGrandson->name, reinterpret_cast<const xmlChar*>("default"))) {
            xmlFree(def);
            def = reinterpret_cast<char*>(xmlNodeGetContent(greatGrandson));
        } else if (!xmlStrcmp(greatGrandson->name, reinterpret_cast<const xmlChar*>("path"))) {
            xmlFree(path);
            path = reinterpret_cast<char*>(xmlNodeGetContent(greatGrandson));
        } else if (!xmlStrcmp(greatGrandson->name, reinterpret_cast<const xmlChar*>("node"))) {
            xmlFree(node);
            node = reinterpret_cast<char*>(xmlNodeGetContent(greatGrandson));
        }
    }
    if (!CheckResourceTag(def, path, configFile)) {
        xmlFree(def);
        xmlFree(path);
        xmlFree(node);
        return false;
    }
    resNode->def = atoll(def);
    resNode->path = path;
    xmlFree(def);
    xmlFree(path);
    if (node && !LoadResourceAvailable(resNode, node)) {
        SOC_PERF_LOGE("Invalid resource node for %{public}s", configFile.c_str());
        xmlFree(node);
        return false;
    }
    xmlFree(node);

    resStrToIdInfo.insert(std::pair<std::string, int32_t>(resNode->name, resNode->id));
    resNodeInfo.insert(std::pair<int32_t, std::shared_ptr<ResNode>>(resNode->id, resNode));
    wrapSwitch[resNode->id / RES_ID_NUMS_PER_TYPE - 1] = true;
    return true;
}

bool SocPerf::LoadGovResource(xmlNode* child, const std::string& configFile)
{
    xmlNode* grandson = child->children;
    for (; grandson; grandson = grandson->next) {
        if (xmlStrcmp(grandson->name, reinterpret_cast<const xmlChar*>("res"))) {
            continue;
        }
        char* id = reinterpret_cast<char*>(xmlGetProp(grandson, reinterpret_cast<const xmlChar*>("id")));
        char* name = reinterpret_cast<char*>(xmlGetProp(grandson, reinterpret_cast<const xmlChar*>("name")));
        char* persistMode = reinterpret_cast<char*>(xmlGetProp(grandson,
            reinterpret_cast<const xmlChar*>("switch")));
        if (!CheckGovResourceTag(id, name, persistMode, configFile)) {
            xmlFree(id);
            xmlFree(name);
            xmlFree(persistMode);
            return false;
        }
        xmlNode* greatGrandson = grandson->children;
        std::shared_ptr<GovResNode> govResNode = std::make_shared<GovResNode>(atoi(id),
            name, persistMode ? atoi(persistMode) : 0);
        xmlFree(id);
        xmlFree(name);
        xmlFree(persistMode);
        wrapSwitch[govResNode->id / RES_ID_NUMS_PER_TYPE - 1] = true;
        if (!TraversalGovResource(greatGrandson, configFile, govResNode)) {
            return false;
        }
        resStrToIdInfo.insert(std::pair<std::string, int32_t>(govResNode->name, govResNode->id));
        govResNodeInfo.insert(std::pair<int32_t, std::shared_ptr<GovResNode>>(govResNode->id, govResNode));
    }

    if (!CheckGovResDefValid()) {
        return false;
    }

    return true;
}

void SocPerf::LoadInfo(xmlNode* child, const std::string& configFile)
{
    xmlNode* grandson = child->children;
    if (!grandson || xmlStrcmp(grandson->name, reinterpret_cast<const xmlChar*>("inf"))) {
        return;
    }
    perfSoPath = reinterpret_cast<char*>(xmlGetProp(grandson, reinterpret_cast<const xmlChar*>("path")));
    perfSoFunc = reinterpret_cast<char*>(xmlGetProp(grandson, reinterpret_cast<const xmlChar*>("func")));
}

bool SocPerf::TraversalGovResource(xmlNode* greatGrandson, const std::string& configFile,
    std::shared_ptr<GovResNode> govResNode)
{
    for (; greatGrandson; greatGrandson = greatGrandson->next) {
        if (!xmlStrcmp(greatGrandson->name, reinterpret_cast<const xmlChar*>("default"))) {
            char* def = reinterpret_cast<char*>(xmlNodeGetContent(greatGrandson));
            if (!def || !IsNumber(def)) {
                SOC_PERF_LOGE("Invalid governor resource default for %{public}s", configFile.c_str());
                xmlFree(def);
                return false;
            }
            govResNode->def = atoll(def);
            xmlFree(def);
        } else if (!xmlStrcmp(greatGrandson->name, reinterpret_cast<const xmlChar*>("path"))) {
            char* path = reinterpret_cast<char*>(xmlNodeGetContent(greatGrandson));
            if (!path) {
                SOC_PERF_LOGE("Invalid governor resource path for %{public}s", configFile.c_str());
                return false;
            }
            govResNode->paths.push_back(path);
            xmlFree(path);
        } else if (!xmlStrcmp(greatGrandson->name, reinterpret_cast<const xmlChar*>("node"))) {
            char* level = reinterpret_cast<char*>(
                xmlGetProp(greatGrandson, reinterpret_cast<const xmlChar*>("level")));
            char* node = reinterpret_cast<char*>(xmlNodeGetContent(greatGrandson));
            if (!level || !IsNumber(level) || !node
                || !LoadGovResourceAvailable(govResNode, level, node)) {
                SOC_PERF_LOGE("Invalid governor resource node for %{public}s", configFile.c_str());
                xmlFree(level);
                xmlFree(node);
                return false;
            }
            xmlFree(level);
            xmlFree(node);
        }
    }
    return true;
}

bool SocPerf::LoadCmd(const xmlNode* rootNode, const std::string& configFile)
{
    xmlNode* child = rootNode->children;
    for (; child; child = child->next) { // Iterate all cmdID
        if (xmlStrcmp(child->name, reinterpret_cast<const xmlChar*>("cmd"))) {
            continue;
        }
        char* id = reinterpret_cast<char*>(xmlGetProp(child, reinterpret_cast<const xmlChar*>("id")));
        char* name = reinterpret_cast<char*>(xmlGetProp(child, reinterpret_cast<const xmlChar*>("name")));
        if (!CheckCmdTag(id, name, configFile)) {
            xmlFree(id);
            xmlFree(name);
            return false;
        }
        xmlNode* grandson = child->children;
        std::shared_ptr<Actions> actions = std::make_shared<Actions>(atoi(id), name);
        xmlFree(id);
        xmlFree(name);
        if (!TraversalBoostResource(grandson, configFile, actions)) {
            return false;
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

bool SocPerf::ParseDuration(xmlNode *greatGrandson, const std::string& configFile, std::shared_ptr<Action> action) const
{
    if (xmlStrcmp(greatGrandson->name, reinterpret_cast<const xmlChar*>("duration"))) {
        return true;
    }
    char* duration = reinterpret_cast<char*>(xmlNodeGetContent(greatGrandson));
    if (!duration || !IsNumber(duration)) {
        SOC_PERF_LOGE("Invalid cmd duration for %{public}s", configFile.c_str());
        xmlFree(duration);
        return false;
    }
    action->duration = atoi(duration);
    xmlFree(duration);
    return true;
}

bool SocPerf::ParseResValue(xmlNode* greatGrandson, const std::string& configFile, std::shared_ptr<Action> action)
{
    if (!xmlStrcmp(greatGrandson->name, reinterpret_cast<const xmlChar*>("duration"))) {
        return true;
    }
    char* resStr = reinterpret_cast<char*>(const_cast<xmlChar*>(greatGrandson->name));
    char* resValue = reinterpret_cast<char*>(xmlNodeGetContent(greatGrandson));
    if (!resStr || resStrToIdInfo.find(resStr) == resStrToIdInfo.end()
        || !resValue || !IsNumber(resValue)) {
        SOC_PERF_LOGE("Invalid cmd resource(%{public}s) for %{public}s", resStr, configFile.c_str());
        xmlFree(resValue);
        return false;
    }
    action->variable.push_back(resStrToIdInfo[resStr]);
    action->variable.push_back(atoll(resValue));
    xmlFree(resValue);
    return true;
}

bool SocPerf::TraversalBoostResource(xmlNode* grandson, const std::string& configFile, std::shared_ptr<Actions> actions)
{
    for (; grandson; grandson = grandson->next) { // Iterate all Action
        std::shared_ptr<Action> action = std::make_shared<Action>();
        xmlNode* greatGrandson = grandson->children;
        for (; greatGrandson; greatGrandson = greatGrandson->next) { // Iterate duration and all res
            bool ret = ParseDuration(greatGrandson, configFile, action);
            if (!ret) {
                return false;
            }
            ret = ParseResValue(greatGrandson, configFile, action);
            if (!ret) {
                return false;
            }
        }
        actions->actionList.push_back(action);
    }
    return true;
}

bool SocPerf::CheckResourceTag(const char* id, const char* name, const char* pair, const char* mode,
    const char* persistMode, const std::string& configFile) const
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
    return CheckResourcePersistMode(persistMode, configFile);
}

bool SocPerf::CheckResourcePersistMode(const char* persistMode, const std::string& configFile) const
{
    if (persistMode && (!IsNumber(persistMode) || !IsValidPersistMode(atoi(persistMode)))) {
        SOC_PERF_LOGE("Invalid resource persistMode for %{public}s", configFile.c_str());
        return false;
    }
    return true;
}

bool SocPerf::CheckResourceTag(const char* def, const char* path, const std::string& configFile) const
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

bool SocPerf::LoadResourceAvailable(std::shared_ptr<ResNode> resNode, const char* node)
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

bool SocPerf::CheckPairResIdValid() const
{
    for (auto iter = resNodeInfo.begin(); iter != resNodeInfo.end(); ++iter) {
        int32_t resId = iter->first;
        std::shared_ptr<ResNode> resNode = iter->second;
        int32_t pairResId = resNode->pair;
        if (pairResId != INVALID_VALUE && resNodeInfo.find(pairResId) == resNodeInfo.end()) {
            SOC_PERF_LOGE("resId[%{public}d]'s pairResId[%{public}d] is not valid", resId, pairResId);
            return false;
        }
    }
    return true;
}

bool SocPerf::CheckResDefValid() const
{
    for (auto iter = resNodeInfo.begin(); iter != resNodeInfo.end(); ++iter) {
        int32_t resId = iter->first;
        std::shared_ptr<ResNode> resNode = iter->second;
        int64_t def = resNode->def;
        if (!resNode->available.empty() && resNode->available.find(def) == resNode->available.end()) {
            SOC_PERF_LOGE("resId[%{public}d]'s def[%{public}lld] is not valid", resId, (long long)def);
            return false;
        }
    }
    return true;
}

bool SocPerf::CheckGovResourceTag(const char* id, const char* name,
    const char* persistMode, const std::string& configFile) const
{
    if (!id || !IsNumber(id) || !IsValidResId(atoi(id))) {
        SOC_PERF_LOGE("Invalid governor resource id for %{public}s", configFile.c_str());
        return false;
    }
    if (!name) {
        SOC_PERF_LOGE("Invalid governor resource name for %{public}s", configFile.c_str());
        return false;
    }
    if (persistMode && (!IsNumber(persistMode) || !IsValidPersistMode(atoi(persistMode)))) {
        SOC_PERF_LOGE("Invalid governor resource persistMode for %{public}s", configFile.c_str());
        return false;
    }
    return true;
}

bool SocPerf::LoadGovResourceAvailable(std::shared_ptr<GovResNode> govResNode, const char* level, const char* node)
{
    govResNode->available.insert(atoll(level));
    std::string nodeStr = node;
    std::vector<std::string> result = Split(nodeStr, "|");
    if (result.size() != govResNode->paths.size()) {
        SOC_PERF_LOGE("Invalid governor resource node matches paths");
        return false;
    }
    govResNode->levelToStr.insert(std::pair<int32_t, std::vector<std::string>>(atoll(level), result));
    return true;
}

bool SocPerf::CheckGovResDefValid() const
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

bool SocPerf::CheckCmdTag(const char* id, const char* name, const std::string& configFile) const
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

bool SocPerf::TraversalActions(std::shared_ptr<Action> action, int32_t actionId)
{
    for (int32_t i = 0; i < (int32_t)action->variable.size() - 1; i += RES_ID_AND_VALUE_PAIR) {
        int32_t resId = action->variable[i];
        int64_t resValue = action->variable[i + 1];
        if (resNodeInfo.find(resId) != resNodeInfo.end()) {
            if (resNodeInfo[resId]->persistMode != REPORT_TO_PERFSO && !resNodeInfo[resId]->available.empty()
                && resNodeInfo[resId]->available.find(resValue) == resNodeInfo[resId]->available.end()) {
                SOC_PERF_LOGE("action[%{public}d]'s resValue[%{public}lld] is not valid",
                    actionId, (long long)resValue);
                return false;
            }
        } else if (govResNodeInfo.find(resId) != govResNodeInfo.end()) {
            if (govResNodeInfo[resId]->persistMode != REPORT_TO_PERFSO
                && govResNodeInfo[resId]->available.find(resValue) == govResNodeInfo[resId]->available.end()) {
                SOC_PERF_LOGE("action[%{public}d]'s resValue[%{public}lld] is not valid",
                    actionId, (long long)resValue);
                return false;
            }
        } else {
            SOC_PERF_LOGE("action[%{public}d]'s resId[%{public}d] is not valid", actionId, resId);
            return false;
        }
    }
    return true;
}

bool SocPerf::CheckActionResIdAndValueValid(const std::string& configFile)
{
    std::unordered_map<int32_t, std::shared_ptr<Actions>> actionsInfo;
    if (configFile.find(SOCPERF_BOOST_CONFIG_XML) != std::string::npos) {
        actionsInfo = perfActionsInfo;
    }
    for (auto actionsIter = actionsInfo.begin(); actionsIter != actionsInfo.end(); ++actionsIter) {
        int32_t actionId = actionsIter->first;
        std::shared_ptr<Actions> actions = actionsIter->second;
        for (auto actionIter = actions->actionList.begin(); actionIter != actions->actionList.end(); ++actionIter) {
            bool ret = TraversalActions(*actionIter, actionId);
            if (!ret) {
                return false;
            }
        }
    }
    return true;
}

void SocPerf::PrintCachedInfo() const
{
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
} // namespace SOCPERF
} // namespace OHOS
