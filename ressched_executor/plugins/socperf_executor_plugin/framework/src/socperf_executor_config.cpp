/*
 * Copyright (c) 2024-2024 Huawei Device Co., Ltd.
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
#include "socperf_executor_config.h"

#include "config_policy_utils.h"
#include "socperf_common.h"
#include "socperf_log.h"

namespace OHOS {
namespace ResourceSchedule {
namespace {
    const std::string SPLIT_OR = "|";
    const std::string SPLIT_SPACE = " ";
}

SocPerfConfig& SocPerfConfig::GetInstance()
{
    static SocPerfConfig socPerfConfig_;
    return socPerfConfig_;
}

SocPerfConfig::SocPerfConfig() {}

SocPerfConfig::~SocPerfConfig() {}

bool SocPerfConfig::Init()
{
    if (!LoadAllConfigXmlFile(SOCPERF_RESOURCE_CONFIG_XML)) {
        SOC_PERF_LOGE("Failed to load %{private}s", SOCPERF_RESOURCE_CONFIG_XML.c_str());
        return false;
    }
    PrintCachedInfo();
    SOC_PERF_LOGD("SocPerf exe Init SUCCESS!");
    return true;
}

bool SocPerfConfig::IsGovResId(int32_t resId) const
{
    auto item = resourceNodeInfo_.find(resId);
    if (item != resourceNodeInfo_.end() && item->second->isGov) {
        return true;
    }
    return false;
}

bool SocPerfConfig::IsValidResId(int32_t resId) const
{
    if (resourceNodeInfo_.find(resId) == resourceNodeInfo_.end()) {
        return false;
    }
    return true;
}

std::string SocPerfConfig::GetRealConfigPath(const std::string& configFile)
{
    char buf[PATH_MAX + 1];
    char* configFilePath = GetOneCfgFile(configFile.c_str(), buf, PATH_MAX + 1);
    char tmpPath[PATH_MAX + 1] = {0};
    if (!configFilePath || strlen(configFilePath) == 0 || strlen(configFilePath) > PATH_MAX ||
        !realpath(configFilePath, tmpPath)) {
        SOC_PERF_LOGE("load %{private}s file fail", configFile.c_str());
        return "";
    }
    return std::string(tmpPath);
}

std::vector<std::string> SocPerfConfig::GetAllRealConfigPath(const std::string& configFile)
{
    std::vector<std::string> configFilePaths;
    auto cfgFiles = GetCfgFiles(configFile.c_str());
    if (cfgFiles == nullptr) {
        return configFilePaths;
    }
    for (const auto& file : cfgFiles->paths) {
        if (file == nullptr) {
            continue;
        }
        configFilePaths.emplace_back(std::string(file));
    }
    FreeCfgFiles(cfgFiles);
    reverse(configFilePaths.begin(), configFilePaths.end());
    return configFilePaths;
}

bool SocPerfConfig::LoadAllConfigXmlFile(const std::string& configFile)
{
    std::vector<std::string> filePaths = GetAllRealConfigPath(configFile);
    for (auto realConfigFile : filePaths) {
        if (!(LoadConfigXmlFile(realConfigFile.c_str()))) {
            return false;
        }
    }
    SOC_PERF_LOGD("Success to Load %{private}s", configFile.c_str());
    return true;
}

bool SocPerfConfig::LoadConfigXmlFile(const std::string& realConfigFile)
{
    if (realConfigFile.size() == 0) {
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
    SOC_PERF_LOGD("Success to Load %{private}s", realConfigFile.c_str());
    return true;
}

bool SocPerfConfig::ParseResourceXmlFile(const xmlNode* rootNode, const std::string& realConfigFile, xmlDoc* file)
{
    xmlNode* child = rootNode->children;
    for (; child; child = child->next) {
        if (!xmlStrcmp(child->name, reinterpret_cast<const xmlChar*>("Resource"))) {
            if (!LoadResource(child, realConfigFile)) {
                return false;
            }
        } else if (!xmlStrcmp(child->name, reinterpret_cast<const xmlChar*>("GovResource"))) {
            if (!LoadGovResource(child, realConfigFile)) {
                return false;
            }
        }
    }
    return true;
}

bool SocPerfConfig::LoadResource(xmlNode* child, const std::string& configFile)
{
    xmlNode* grandson = child->children;
    for (; grandson; grandson = grandson->next) {
        if (!xmlStrcmp(grandson->name, reinterpret_cast<const xmlChar*>("res"))) {
            if (!TraversalFreqResource(grandson, configFile)) {
                return false;
            }
        }
    }

    if (!CheckPairResIdValid() || !CheckDefValid()) {
        return false;
    }

    return true;
}

bool SocPerfConfig::TraversalFreqResource(xmlNode* grandson, const std::string& configFile)
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
    auto it = resourceNodeInfo_.find(atoi(id));
    if (it != resourceNodeInfo_.end()) {
        xmlFree(id);
        xmlFree(name);
        xmlFree(pair);
        xmlFree(mode);
        xmlFree(persistMode);
        return true;
    }
    xmlNode* greatGrandson = grandson->children;
    std::shared_ptr<ResNode> resNode = std::make_shared<ResNode>(atoi(id), name, mode ? atoi(mode) : 0,
        pair ? atoi(pair) : INVALID_VALUE, persistMode ? atoi(persistMode) : 0);
    xmlFree(id);
    xmlFree(name);
    xmlFree(pair);
    xmlFree(mode);
    if (!LoadFreqResourceContent(persistMode ? atoi(persistMode) : 0, greatGrandson, configFile, resNode)) {
        xmlFree(persistMode);
        return false;
    }
    xmlFree(persistMode);
    return true;
}

bool SocPerfConfig::LoadFreqResourceContent(int32_t persistMode, xmlNode* greatGrandson, const std::string& configFile,
    std::shared_ptr<ResNode> resNode)
{
    char *def = nullptr;
    char *path  = nullptr;
    char *node  = nullptr;
    for (; greatGrandson; greatGrandson = greatGrandson->next) {
        if (!xmlStrcmp(greatGrandson->name, reinterpret_cast<const xmlChar*>("default"))) {
            xmlFree(def);
            def = reinterpret_cast<char*>(xmlNodeGetContent(greatGrandson));
        } else if (persistMode != REPORT_TO_PERFSO &&
            !xmlStrcmp(greatGrandson->name, reinterpret_cast<const xmlChar*>("path"))) {
            xmlFree(path);
            path = reinterpret_cast<char*>(xmlNodeGetContent(greatGrandson));
        } else if (!xmlStrcmp(greatGrandson->name, reinterpret_cast<const xmlChar*>("node"))) {
            xmlFree(node);
            node = reinterpret_cast<char*>(xmlNodeGetContent(greatGrandson));
        }
    }
    if (!CheckResourceTag(persistMode, def, path, configFile)) {
        xmlFree(def);
        xmlFree(path);
        xmlFree(node);
        return false;
    }
    resNode->def = atoll(def);
    if (persistMode != REPORT_TO_PERFSO) {
        resNode->path = path;
    }
    xmlFree(def);
    xmlFree(path);
    if (node && !LoadResourceAvailable(resNode, node)) {
        SOC_PERF_LOGE("Invalid resource node for %{private}s", configFile.c_str());
        xmlFree(node);
        return false;
    }
    xmlFree(node);

    std::unique_lock<std::mutex> lockResourceNode(resourceNodeMutex_);
    resourceNodeInfo_.insert(std::pair<int32_t, std::shared_ptr<ResNode>>(resNode->id, resNode));
    lockResourceNode.unlock();
    return true;
}

bool SocPerfConfig::LoadGovResource(xmlNode* child, const std::string& configFile)
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
        auto it = resourceNodeInfo_.find(atoi(id));
        if (it != resourceNodeInfo_.end()) {
            xmlFree(id);
            xmlFree(name);
            xmlFree(persistMode);
            continue;
        }
        xmlNode* greatGrandson = grandson->children;
        std::shared_ptr<GovResNode> govResNode = std::make_shared<GovResNode>(atoi(id),
            name, persistMode ? atoi(persistMode) : 0);
        xmlFree(id);
        xmlFree(name);
        
        std::unique_lock<std::mutex> lockResourceNode(resourceNodeMutex_);
        resourceNodeInfo_.insert(std::pair<int32_t, std::shared_ptr<GovResNode>>(govResNode->id, govResNode));
        lockResourceNode.unlock();

        if (!TraversalGovResource(persistMode ? atoi(persistMode) : 0, greatGrandson, configFile, govResNode)) {
            xmlFree(persistMode);
            return false;
        }
        xmlFree(persistMode);
    }

    if (!CheckDefValid()) {
        return false;
    }

    return true;
}

int32_t SocPerfConfig::GetResIdNumsPerType(int32_t resId) const
{
    auto item = resourceNodeInfo_.find(resId);
    if (item != resourceNodeInfo_.end() && item->second->persistMode == REPORT_TO_PERFSO) {
        return RES_ID_NUMS_PER_TYPE_EXT;
    }
    return RES_ID_NUMS_PER_TYPE;
}

bool SocPerfConfig::TraversalGovResource(int32_t persistMode, xmlNode* greatGrandson, const std::string& configFile,
    std::shared_ptr<GovResNode> govResNode)
{
    for (; greatGrandson; greatGrandson = greatGrandson->next) {
        if (!xmlStrcmp(greatGrandson->name, reinterpret_cast<const xmlChar*>("default"))) {
            char* def = reinterpret_cast<char*>(xmlNodeGetContent(greatGrandson));
            if (!def || !IsNumber(def)) {
                SOC_PERF_LOGE("Invalid governor resource default for %{private}s", configFile.c_str());
                xmlFree(def);
                return false;
            }
            govResNode->def = atoll(def);
            xmlFree(def);
        } else if (persistMode != REPORT_TO_PERFSO &&
            !xmlStrcmp(greatGrandson->name, reinterpret_cast<const xmlChar*>("path"))) {
            char* path = reinterpret_cast<char*>(xmlNodeGetContent(greatGrandson));
            if (!path) {
                SOC_PERF_LOGE("Invalid governor resource path for %{private}s", configFile.c_str());
                return false;
            }
            govResNode->paths.push_back(path);
            xmlFree(path);
        } else if (persistMode != REPORT_TO_PERFSO &&
            !xmlStrcmp(greatGrandson->name, reinterpret_cast<const xmlChar*>("node"))) {
            char* level = reinterpret_cast<char*>(
                xmlGetProp(greatGrandson, reinterpret_cast<const xmlChar*>("level")));
            char* node = reinterpret_cast<char*>(xmlNodeGetContent(greatGrandson));
            if (!level || !IsNumber(level) || !node
                || !LoadGovResourceAvailable(govResNode, level, node)) {
                SOC_PERF_LOGE("Invalid governor resource node for %{private}s", configFile.c_str());
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

int32_t SocPerfConfig::GetXmlIntProp(const xmlNode* xmlNode, const char* propName) const
{
    int ret = -1;
    char* propValue = reinterpret_cast<char*>(xmlGetProp(xmlNode, reinterpret_cast<const xmlChar*>(propName)));
    if (propValue != nullptr && IsNumber(propValue)) {
        ret = atoi(propValue);
    }
    if (propValue != nullptr) {
        xmlFree(propValue);
    }
    return ret;
}

bool SocPerfConfig::CheckResourceTag(const char* id, const char* name, const char* pair, const char* mode,
    const char* persistMode, const std::string& configFile) const
{
    if (!id || !IsNumber(id) || !IsValidRangeResId(atoi(id))) {
        SOC_PERF_LOGE("Invalid resource id for %{private}s", configFile.c_str());
        return false;
    }
    if (!name) {
        SOC_PERF_LOGE("Invalid resource name for %{private}s", configFile.c_str());
        return false;
    }
    if (pair && (!IsNumber(pair) || !IsValidRangeResId(atoi(pair)))) {
        SOC_PERF_LOGE("Invalid resource pair for %{private}s", configFile.c_str());
        return false;
    }
    if (mode && !IsNumber(mode)) {
        SOC_PERF_LOGE("Invalid resource mode for %{private}s", configFile.c_str());
        return false;
    }
    return CheckResourcePersistMode(persistMode, configFile);
}

bool SocPerfConfig::CheckResourcePersistMode(const char* persistMode, const std::string& configFile) const
{
    if (persistMode && (!IsNumber(persistMode) || !IsValidPersistMode(atoi(persistMode)))) {
        SOC_PERF_LOGE("Invalid resource persistMode for %{private}s", configFile.c_str());
        return false;
    }
    return true;
}

bool SocPerfConfig::CheckResourceTag(int32_t persistMode, const char* def,
    const char* path, const std::string& configFile) const
{
    if (!def || !IsNumber(def)) {
        SOC_PERF_LOGE("Invalid resource default for %{private}s", configFile.c_str());
        return false;
    }
    if (persistMode != REPORT_TO_PERFSO && !path) {
        SOC_PERF_LOGE("Invalid resource path for %{private}s", configFile.c_str());
        return false;
    }
    return true;
}

bool SocPerfConfig::LoadResourceAvailable(std::shared_ptr<ResNode> resNode, const char* node)
{
    std::string nodeStr = node;
    std::vector<std::string> result = Split(nodeStr, SPLIT_SPACE);
    for (auto str : result) {
        if (IsNumber(str)) {
            resNode->available.insert(atoi(str.c_str()));
        } else {
            return false;
        }
    }
    return true;
}

bool SocPerfConfig::CheckPairResIdValid() const
{
    for (auto iter = resourceNodeInfo_.begin(); iter != resourceNodeInfo_.end(); ++iter) {
        if (iter->second->isGov) {
            continue;
        }
        int32_t resId = iter->first;
        std::shared_ptr<ResNode> resNode = std::static_pointer_cast<ResNode>(iter->second);
        int32_t pairResId = resNode->pair;
        if (pairResId != INVALID_VALUE && resourceNodeInfo_.find(pairResId) == resourceNodeInfo_.end()) {
            SOC_PERF_LOGE("resId[%{public}d]'s pairResId[%{public}d] is not valid", resId, pairResId);
            return false;
        }
    }
    return true;
}

bool SocPerfConfig::CheckDefValid() const
{
    for (auto iter = resourceNodeInfo_.begin(); iter != resourceNodeInfo_.end(); ++iter) {
        int32_t resId = iter->first;
        std::shared_ptr<ResourceNode> resourceNode = iter->second;
        int64_t def = resourceNode->def;
        if (!resourceNode->available.empty() && resourceNode->available.find(def) == resourceNode->available.end()) {
            SOC_PERF_LOGE("resId[%{public}d]'s def[%{public}lld] is not valid", resId, (long long)def);
            return false;
        }
    }
    return true;
}

bool SocPerfConfig::CheckGovResourceTag(const char* id, const char* name,
    const char* persistMode, const std::string& configFile) const
{
    if (!id || !IsNumber(id) || !IsValidRangeResId(atoi(id))) {
        SOC_PERF_LOGE("Invalid governor resource id for %{private}s", configFile.c_str());
        return false;
    }
    if (!name) {
        SOC_PERF_LOGE("Invalid governor resource name for %{private}s", configFile.c_str());
        return false;
    }
    if (persistMode && (!IsNumber(persistMode) || !IsValidPersistMode(atoi(persistMode)))) {
        SOC_PERF_LOGE("Invalid governor resource persistMode for %{private}s", configFile.c_str());
        return false;
    }
    return true;
}

bool SocPerfConfig::LoadGovResourceAvailable(std::shared_ptr<GovResNode> govResNode,
    const char* level, const char* node)
{
    govResNode->available.insert(atoll(level));
    std::string nodeStr = node;
    std::vector<std::string> result = Split(nodeStr, SPLIT_OR);
    if (result.size() != govResNode->paths.size()) {
        SOC_PERF_LOGE("Invalid governor resource node matches paths");
        return false;
    }
    std::unique_lock<std::mutex> levelMutex(govResNode->levelToStrMutex_);
    govResNode->levelToStr.insert(std::pair<int32_t, std::vector<std::string>>(atoll(level), result));
    levelMutex.unlock();
    return true;
}

void SocPerfConfig::PrintCachedInfo() const
{
    SOC_PERF_LOGD("------------------------------------");
    SOC_PERF_LOGD("resourceNodeInfo_(%{public}d)", (int32_t)resourceNodeInfo_.size());
    for (auto iter = resourceNodeInfo_.begin(); iter != resourceNodeInfo_.end(); ++iter) {
        iter->second->PrintString();
    }
    SOC_PERF_LOGD("------------------------------------");
}
} // namespace SOCPERF
} // namespace OHOS
