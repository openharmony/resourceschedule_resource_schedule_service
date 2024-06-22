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

#ifndef RESSCHED_EXECUTOR_PLUGINS_FRAMEWORK_SOCPERF_EXECUTOR_PLUGIN_INCLUDE_SOCPERF_EXECUTOR_CONFIG_H
#define RESSCHED_EXECUTOR_PLUGINS_FRAMEWORK_SOCPERF_EXECUTOR_PLUGIN_INCLUDE_SOCPERF_EXECUTOR_CONFIG_H

#include "libxml/tree.h"
#include "socperf_common.h"
#include <string>
#include <vector>

namespace OHOS {
namespace ResourceSchedule {
class SocPerfConfig {
public:
    bool Init();
    bool IsGovResId(int32_t resId) const;
    bool IsValidResId(int32_t resId) const;
    int32_t GetResIdNumsPerType(int32_t resId) const;
    static SocPerfConfig& GetInstance();

public:
    std::mutex resourceNodeMutex_;
    std::unordered_map<int32_t, std::shared_ptr<ResourceNode>> resourceNodeInfo_;

private:
    SocPerfConfig();
    ~SocPerfConfig();
    std::string GetRealConfigPath(const std::string& configFile);
    bool LoadConfigXmlFile(const std::string& configFile);
    bool ParseResourceXmlFile(const xmlNode* rootNode, const std::string& realConfigFile, xmlDoc* file);
    bool LoadResource(xmlNode* rootNode, const std::string& configFile);
    bool TraversalFreqResource(xmlNode* grandson, const std::string& configFile);
    bool LoadFreqResourceContent(int32_t persistMode, xmlNode* greatGrandson, const std::string& configFile,
        std::shared_ptr<ResNode> resNode);
    int32_t GetXmlIntProp(const xmlNode* xmlNode, const char* propName) const;
    bool LoadGovResource(xmlNode* rootNode, const std::string& configFile);
    bool TraversalGovResource(int32_t persistMode, xmlNode* greatGrandson, const std::string& configFile,
        std::shared_ptr<GovResNode> govResNode);
    bool CheckResourceTag(const char* id, const char* name, const char* pair, const char* mode,
        const char* persistMode, const std::string& configFile) const;
    bool CheckResourcePersistMode(const char* persistMode, const std::string& configFile) const;
    bool CheckResourceTag(int32_t persistMode, const char* def, const char* path, const std::string& configFile) const;
    bool LoadResourceAvailable(std::shared_ptr<ResNode> resNode, const char* node);
    bool CheckPairResIdValid() const;
    bool CheckDefValid() const;
    bool CheckGovResourceTag(const char* id, const char* name, const char* persistMode,
        const std::string& configFile) const;
    bool LoadGovResourceAvailable(std::shared_ptr<GovResNode> govResNode, const char* level, const char* node);
    bool CheckActionResIdAndValueValid(const std::string& configFile);
    void PrintCachedInfo() const;
};
} // namespace ResourceSchedule
} // namespace OHOS
#endif // RESSCHED_EXECUTOR_PLUGINS_FRAMEWORK_SOCPERF_EXECUTOR_PLUGIN_INCLUDE_SOCPERF_EXECUTOR_CONFIG_H
