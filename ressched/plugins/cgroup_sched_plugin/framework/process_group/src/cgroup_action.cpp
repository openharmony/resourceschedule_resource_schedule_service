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
#include "cgroup_action.h"
#include <algorithm>              // for replace
#include <vector>                 // for vector
#include <climits>                // for PATH_MAX
#include <memory>                 // for unique_ptr
#include <cstdlib>                // for realpath
#include <cstring>                // for strlen
#include <utility>                // for pair
#include "cgroup_controller.h"    // for CgroupController
#include "cgroup_map.h"           // for CgroupMap
#include "cgroup_sched_log.h"
#include "config_policy_utils.h"  // for GetOneCfgFile
#include "nlohmann/json.hpp"           // for Value
#include "process_group_util.h"   // for ReadFileToString
#include "res_common_util.h"
#include "res_sched_exe_client.h" // for ResSchedExeClient
#include "sched_policy.h"         // for SchedPolicy, SP_UPPER_LIMIT, SP_DEF...

namespace OHOS {
namespace ResourceSchedule {
namespace CgroupSetting {
namespace {
    static constexpr const char* CGROUP_SETTING_CONFIG_FILE = "etc/cgroup_sched/cgroup_action_config.json";

    static const std::string STR_SP_DEFAULT = "sp_default";
    static const std::string STR_SP_BACKGROUND = "sp_background";
    static const std::string STR_SP_FOREGROUND = "sp_foreground";
    static const std::string STR_SP_SYSTEM_BACKGROUND = "sp_system_background";
    static const std::string STR_SP_TOP_APP = "sp_top_app";

    static const std::string ABBR_SP_DEFAULT = "df";
    static const std::string ABBR_SP_BACKGROUND = "bg";
    static const std::string ABBR_SP_FOREGROUND = "fg";
    static const std::string ABBR_SP_SYSTEM_BACKGROUND = "sy";
    static const std::string ABBR_SP_TOP_APP = "ta";
    constexpr const char * const JSON_KEY_CGROUPS = "Cgroups";
}

CgroupAction& CgroupAction::GetInstance()
{
    static CgroupAction instance;
    return instance;
}

CgroupAction::CgroupAction()
{
    AddSchedPolicyDeclaration(SP_DEFAULT, STR_SP_DEFAULT, ABBR_SP_DEFAULT);
    AddSchedPolicyDeclaration(SP_BACKGROUND, STR_SP_BACKGROUND, ABBR_SP_BACKGROUND);
    AddSchedPolicyDeclaration(SP_FOREGROUND, STR_SP_FOREGROUND, ABBR_SP_FOREGROUND);
    AddSchedPolicyDeclaration(SP_SYSTEM_BACKGROUND, STR_SP_SYSTEM_BACKGROUND, ABBR_SP_SYSTEM_BACKGROUND);
    AddSchedPolicyDeclaration(SP_TOP_APP, STR_SP_TOP_APP, ABBR_SP_TOP_APP);
}

void CgroupAction::AddSchedPolicyDeclaration(const SchedPolicy policy,
    const std::string& fullName, const std::string& abbrName)
{
    std::lock_guard<ffrt::mutex> lock(mutex_);
    if (!allowToAdd_) {
        CGS_LOGI("%{public}s not allowed: %{public}u, %{public}s, %{public}s",
            __func__, policy, fullName.c_str(), abbrName.c_str());
        return;
    }
    if (policy >= SP_UPPER_LIMIT) {
        CGS_LOGI("%{public}s out of range: %{public}u, %{public}s, %{public}s",
            __func__, policy, fullName.c_str(), abbrName.c_str());
        return;
    }
    if (fullName.empty() || abbrName.empty()) {
        return;
    }
    if (fullNames_.find(policy) != fullNames_.end()) {
        return;
    }
    if (std::any_of(fullNames_.begin(), fullNames_.end(),
        [ &fullName ] (const auto& kv) { return kv.second == fullName; })) {
        return;
    }
    CGS_LOGI("%{public}s add sched policy: %{public}u, %{public}s, %{public}s",
        __func__, policy, fullName.c_str(), abbrName.c_str());
    fullNames_[policy] = fullName;
    abbrNames_[policy] = abbrName;
}

std::vector<SchedPolicy> CgroupAction::GetSchedPolicyList()
{
    std::lock_guard<ffrt::mutex> lock(mutex_);
    std::vector<SchedPolicy> policyList;
    std::transform(fullNames_.begin(), fullNames_.end(), std::back_inserter(policyList),
        [] (const auto& kv) { return kv.first; });
    return policyList;
}

bool CgroupAction::IsSchedPolicyValid(SchedPolicy policy)
{
    std::lock_guard<ffrt::mutex> lock(mutex_);
    return fullNames_.find(policy) != fullNames_.end();
}

const char* CgroupAction::GetSchedPolicyFullName(SchedPolicy policy)
{
    std::lock_guard<ffrt::mutex> lock(mutex_);
    if (fullNames_.find(policy) != fullNames_.end()) {
        return fullNames_[policy].c_str();
    }
    return "error";
}

const char* CgroupAction::GetSchedPolicyAbbrName(SchedPolicy policy)
{
    std::lock_guard<ffrt::mutex> lock(mutex_);
    if (abbrNames_.find(policy) != abbrNames_.end()) {
        return abbrNames_[policy].c_str();
    }
    return "error";
}

bool CgroupAction::SetThreadSchedPolicy(int tid, SchedPolicy policy)
{
    if (!IsSchedPolicyValid(policy)) {
        return false;
    }
    if (!IsEnabled()) {
        return false;
    }
    return CgroupMap::GetInstance().SetThreadSchedPolicy(tid, policy, false);
}

bool CgroupAction::SetThreadGroupSchedPolicy(int tid, SchedPolicy policy)
{
    if (!IsSchedPolicyValid(policy)) {
        return false;
    }
    if (!IsEnabled()) {
        return false;
    }
    return CgroupMap::GetInstance().SetThreadSchedPolicy(tid, policy, true);
}

int CgroupAction::SetSchedPolicyByExecutor(int tid, int policy, uint32_t resType)
{
    int ret = 0;
    nlohmann::json payload;
    payload["tid"] = std::to_string(tid);
    payload["policy"] = std::to_string(policy);
    nlohmann::json reply;
    ret = ResourceSchedule::ResSchedExeClient::GetInstance().SendRequestSync(resType, 0, payload, reply);
    if (ret != 0) {
        CGS_LOGE("%{public}s SendRequestSync %{public}d failed", __func__, resType);
        return ret;
    }
    if (reply.contains("ret") && reply.at("ret").is_string()) {
        ret = atoi(reply["ret"].get<std::string>().c_str());
    }
    return ret;
}

bool CgroupAction::LoadConfigFile()
{
    CGS_LOGI("%{public}s CgroupAction::LoadConfigFile loading config file", __func__);
    nlohmann::json jsonObjRoot;
    if (!ParseConfigFileToJsonObj(jsonObjRoot)) {
        return false;
    }
    return CgroupMap::GetInstance().LoadConfigFromJsonObj(jsonObjRoot);
}

bool CgroupAction::IsEnabled()
{
    {
        std::lock_guard<ffrt::mutex> lock(mutex_);
        allowToAdd_ = false;
    }
    static bool enable = LoadConfigFile();
    return enable;
}

int CgroupAction::GetSchedPolicy(int tid, SchedPolicy* policy)
{
    if (!IsEnabled()) {
        *policy = SP_UPPER_LIMIT;
        return -1;
    }
    std::string subgroup;
    std::vector<CgroupController *> controllers;
    bool getTaskGroup = false;
    CgroupMap& instance = CgroupMap::GetInstance();
    if (instance.FindAllEnableCgroupControllers(controllers)) {
        for (const auto controller : controllers) {
            if (controller->GetTaskGroup(tid, subgroup)) {
                getTaskGroup = true;
                break;
            }
        }
    }
    if (!getTaskGroup) {
        *policy = SP_UPPER_LIMIT;
        return -1;
    }
    if (subgroup.empty()) {
        *policy = SP_DEFAULT;
        return 0;
    }

    replace(subgroup.begin(), subgroup.end(), '-', '_');
    subgroup = "sp_" + subgroup;
    return GetSchedPolicyByName(subgroup, policy);
}

int CgroupAction::GetSchedPolicyByName(const std::string& name, SchedPolicy* policy)
{
    const auto& result = std::find_if(fullNames_.begin(), fullNames_.end(),
        [ &name ] (const auto& kv) { return kv.second == name; });
    if (result != fullNames_.end()) {
        *policy = result->first;
        return 0;
    }
    *policy = SP_UPPER_LIMIT;
    return -1;
}

bool CgroupAction::ParseConfigFileToJsonObj(nlohmann::json& jsonObjRoot)
{
    bool result = false;
    auto cfgFilePaths = GetCfgFiles(CGROUP_SETTING_CONFIG_FILE);
    if (!cfgFilePaths) {
        return result;
    }
    for (const auto& configFilePath : cfgFilePaths->paths) {
        char tmpPath[PATH_MAX + 1] = {0};
        if (!configFilePath || strlen(configFilePath) == 0 || strlen(configFilePath) > PATH_MAX ||
            !realpath(configFilePath, tmpPath)) {
            continue;
        }
        std::string realConfigFile(tmpPath);
        nlohmann::json jsonTemp;
        if (!ResCommonUtil::LoadFileToJsonObj(realConfigFile, jsonTemp)) {
            continue;
        }
        if (jsonTemp.contains(JSON_KEY_CGROUPS)) {
            jsonObjRoot = jsonTemp;
            result = true;
        }
    }
    FreeCfgFiles(cfgFilePaths);
    return result;
}
} // namespace CgroupSetting
} // namespace ResourceSchedule
} // namespace OHOS
