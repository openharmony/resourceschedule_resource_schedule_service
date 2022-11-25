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

#ifndef SOC_PERF_INCLUDE_CORE_SOCPERF_H
#define SOC_PERF_INCLUDE_CORE_SOCPERF_H

#include <string>
#include <vector>
#include <unordered_map>
#include <unistd.h>
#include "libxml/parser.h"
#include "libxml/tree.h"
#include "i_socperf_service.h"
#include "socperf_common.h"
#include "socperf_handler.h"

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

public:
    SocPerf();
    ~SocPerf();

private:
    std::unordered_map<int32_t, std::shared_ptr<Actions>> perfActionsInfo;
    std::vector<std::shared_ptr<SocPerfHandler>> handlers;
    bool enabled = false;
    bool debugLogEnabled = false;

    std::unordered_map<int32_t, std::shared_ptr<ResNode>> resNodeInfo;
    std::unordered_map<int32_t, std::shared_ptr<GovResNode>> govResNodeInfo;
    std::unordered_map<std::string, int32_t> resStrToIdInfo;
    std::vector<std::unordered_map<int32_t, int32_t>> limitRequest;

private:
    std::string GetRealConfigPath(const std::string configFile);
    std::shared_ptr<SocPerfHandler> GetHandlerByResId(int32_t resId);
    bool LoadConfigXmlFile(std::string configFile);
    bool CreateHandlers();
    void InitHandlerThreads();
    bool LoadResource(xmlNode* rootNode, std::string configFile);
    bool LoadGovResource(xmlNode* rootNode, std::string configFile);
    bool LoadCmd(xmlNode* rootNode, std::string configFile);
    bool CheckResourceTag(char* id, char* name, char* pair, char* mode, std::string configFile);
    bool CheckResourceTag(char* def, char* path, std::string configFile);
    bool LoadResourceAvailable(std::shared_ptr<ResNode> resNode, char* node);
    bool CheckPairResIdValid();
    bool CheckResDefValid();
    bool CheckGovResourceTag(char* id, char* name, std::string configFile);
    bool LoadGovResourceAvailable(std::shared_ptr<GovResNode> govResNode, char* level, char* node);
    bool CheckGovResDefValid();
    bool CheckCmdTag(char* id, char* name, std::string configFile);
    bool CheckActionResIdAndValueValid(std::string configFile);
    void DoFreqActions(std::shared_ptr<Actions> actions, int32_t onOff, int32_t actionType);
    void PrintCachedInfo();
};
} // namespace SOCPERF
} // namespace OHOS

#endif // SOC_PERF_INCLUDE_CORE_SOCPERF_H
