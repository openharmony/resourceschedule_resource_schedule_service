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

#ifndef SOC_PERF_SERVICES_CORE_INCLUDE_SOCPERF_H
#define SOC_PERF_SERVICES_CORE_INCLUDE_SOCPERF_H

#include <chrono>
#include <string>
#include <vector>
#include <unordered_map>
#include <unistd.h>
#include "libxml/parser.h"
#include "libxml/tree.h"
#include "socperf_common.h"
#include "socperf_thread_wrap.h"

namespace OHOS {
namespace SOCPERF {
class SocPerf {
public:
    bool Init();
    void PerfRequest(int32_t cmdId, const std::string& msg);
    void PerfRequestEx(int32_t cmdId, bool onOffTag, const std::string& msg);
    void PowerLimitBoost(bool onOffTag, const std::string& msg);
    void ThermalLimitBoost(bool onOffTag, const std::string& msg);
    void LimitRequest(int32_t clientId,
        const std::vector<int32_t>& tags, const std::vector<int64_t>& configs, const std::string& msg);
    void SetRequestStatus(bool status, const std::string& msg);
    void SetThermalLevel(int32_t level);
public:
    SocPerf();
    ~SocPerf();

private:
    std::unordered_map<int32_t, std::shared_ptr<Actions>> perfActionsInfo;
    std::vector<std::shared_ptr<SocPerfThreadWrap>> socperfThreadWraps;
    bool wrapSwitch[MAX_QUEUE_NUM] = {false };
    bool enabled = false;
    std::string resourceConfigXml;
    std::unordered_map<int32_t, std::shared_ptr<ResNode>> resNodeInfo;
    std::unordered_map<int32_t, std::shared_ptr<GovResNode>> govResNodeInfo;
    std::unordered_map<std::string, int32_t> resStrToIdInfo;
    std::vector<std::unordered_map<int32_t, int32_t>> limitRequest;
    char* perfSoPath = nullptr;
    char* perfSoFunc = nullptr;
    volatile bool perfRequestEnable_ = true;
    int32_t thermalLvl_ = MIN_THERMAL_LVL;

private:
    std::mutex mutex_;
    std::string GetRealConfigPath(const std::string& configFile);
    std::shared_ptr<SocPerfThreadWrap> GetThreadWrapByResId(int32_t resId) const;
    bool LoadConfigXmlFile(const std::string& configFile);
    bool ParseBoostXmlFile(const xmlNode* rootNode, const std::string& realConfigFile, xmlDoc* file);
    bool ParseResourceXmlFile(const xmlNode* rootNode, const std::string& realConfigFile, xmlDoc* file);
    bool CreateThreadWraps();
    void InitThreadWraps();
    bool LoadResource(xmlNode* rootNode, const std::string& configFile);
    bool TraversalFreqResource(xmlNode* grandson, const std::string& configFile);
    bool LoadFreqResourceContent(xmlNode* greatGrandson, const std::string& configFile,
        std::shared_ptr<ResNode> resNode);
    int32_t GetXmlIntProp(const xmlNode* xmlNode, const char* propName) const;
    bool LoadGovResource(xmlNode* rootNode, const std::string& configFile);
    bool TraversalGovResource(xmlNode* greatGrandson, const std::string& configFile,
        std::shared_ptr<GovResNode> govResNode);
    void LoadInfo(xmlNode* child, const std::string& configFile);
    bool LoadCmd(const xmlNode* rootNode, const std::string& configFile);
    bool TraversalBoostResource(xmlNode* grandson, const std::string& configFile, std::shared_ptr<Actions> actions);
    bool ParseDuration(xmlNode* greatGrandson, const std::string& configFile, std::shared_ptr<Action> action) const;
    bool ParseResValue(xmlNode* greatGrandson, const std::string& configFile, std::shared_ptr<Action> action);
    bool CheckResourceTag(const char* id, const char* name, const char* pair, const char* mode,
        const char* persistMode, const std::string& configFile) const;
    bool CheckResourcePersistMode(const char* persistMode, const std::string& configFile) const;
    bool CheckResourceTag(const char* def, const char* path, const std::string& configFile) const;
    bool LoadResourceAvailable(std::shared_ptr<ResNode> resNode, const char* node);
    bool CheckPairResIdValid() const;
    bool CheckResDefValid() const;
    bool CheckGovResourceTag(const char* id, const char* name, const char* persistMode,
        const std::string& configFile) const;
    bool LoadGovResourceAvailable(std::shared_ptr<GovResNode> govResNode, const char* level, const char* node);
    bool CheckGovResDefValid() const;
    bool CheckCmdTag(const char* id, const char* name, const std::string& configFile) const;
    bool CheckActionResIdAndValueValid(const std::string& configFile);
    bool TraversalActions(std::shared_ptr<Action> action, int32_t actionId);
    void DoFreqActions(std::shared_ptr<Actions> actions, int32_t onOff, int32_t actionType);
    bool DoPerfRequestThremalLvl(int32_t cmdId, std::shared_ptr<Action> action, int32_t onOff);
    void PrintCachedInfo() const;
    void SendLimitRequestEvent(int32_t clientId, int32_t resId, int64_t resValue);
    void SendLimitRequestEventOff(std::shared_ptr<SocPerfThreadWrap> threadWrap,
        int32_t clientId, int32_t resId, int32_t eventId);
    void SendLimitRequestEventOn(std::shared_ptr<SocPerfThreadWrap> threadWrap,
        int32_t clientId, int32_t resId, int64_t resValue, int32_t eventId);
    void ClearAllAliveRequest();
};
} // namespace SOCPERF
} // namespace OHOS
#endif // SOC_PERF_SERVICES_CORE_INCLUDE_SOCPERF_H
