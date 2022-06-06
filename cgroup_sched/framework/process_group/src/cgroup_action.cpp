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

#include <algorithm>
#include <vector>

#include "json/reader.h"
#include "json/value.h"
#include "cgroup_map.h"
#include "process_group_log.h"
#include "process_group_util.h"
#include "sched_policy.h"

namespace OHOS {
namespace ResourceSchedule {
namespace CgroupSetting {
namespace {
    static constexpr const char* CGROUP_SETTING_CONFIG_FILE = "etc/process_group/cgroup_action_config.json";

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
    std::lock_guard<std::mutex> lock(mutex_);
    if (!allowToAdd_) {
        PGCGS_LOGI("%{public}s not allowed: %{public}u, %{public}s, %{public}s",
            __func__, policy, fullName.c_str(), abbrName.c_str());
        return;
    }
    if (policy >= SP_UPPER_LIMIT) {
        PGCGS_LOGI("%{public}s out of range: %{public}u, %{public}s, %{public}s",
            __func__, policy, fullName.c_str(), abbrName.c_str());
        return;
    }
    if (fullName.empty() || abbrName.empty()) {
        return;
    }
    if (fullNames_.find(policy) != fullNames_.end()) {
        return;
    }
    for (auto& kv : fullNames_) {
        if (kv.second == fullName) {
            return;
        }
    }
    PGCGS_LOGI("%{public}s add sched policy: %{public}u, %{public}s, %{public}s",
        __func__, policy, fullName.c_str(), abbrName.c_str());
    fullNames_[policy] = fullName;
    abbrNames_[policy] = abbrName;
}

std::vector<SchedPolicy> CgroupAction::GetSchedPolicyList()
{
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<SchedPolicy> policyList;
    for (auto& kv : fullNames_) {
        policyList.push_back(kv.first);
    }
    return policyList;
}

bool CgroupAction::IsSchedPolicyValid(SchedPolicy policy)
{
    std::lock_guard<std::mutex> lock(mutex_);
    return fullNames_.find(policy) != fullNames_.end();
}

const char* CgroupAction::GetSchedPolicyFullName(SchedPolicy policy)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (fullNames_.find(policy) != fullNames_.end()) {
        return fullNames_[policy].c_str();
    }
    return "error";
}

const char* CgroupAction::GetSchedPolicyAbbrName(SchedPolicy policy)
{
    std::lock_guard<std::mutex> lock(mutex_);
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

bool CgroupAction::LoadConfigFile()
{
    PGCGS_LOGI("CgroupAction::LoadConfigFile loading config file");
    Json::Value jsonObjRoot;
    if (!ParseConfigFileToJsonObj(jsonObjRoot)) {
        return false;
    }
    return CgroupMap::GetInstance().LoadConfigFromJsonObj(jsonObjRoot);
}

bool CgroupAction::IsEnabled()
{
    {
        std::lock_guard<std::mutex> lock(mutex_);
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
    CgroupController *controller;
    CgroupMap& instance = CgroupMap::GetInstance();
    if (instance.FindFristEnableCgroupController(&controller)) {
        if (!controller->GetTaskGroup(tid, subgroup)) {
            *policy = SP_UPPER_LIMIT;
            return -1;
        }
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
    for (auto& kv : fullNames_) {
        if (kv.second == name) {
            *policy = kv.first;
            return 0;
        }
    }
    *policy = SP_UPPER_LIMIT;
    return -1;
}

bool CgroupAction::ParseConfigFileToJsonObj(Json::Value& jsonObjRoot)
{
    std::string jsonString;
    if (!ReadFileToString(CGROUP_SETTING_CONFIG_FILE, jsonString)) {
        PGCGS_LOGE("ParseConfigFileToJsonObj: read config file failed");
        return false;
    }
    Json::CharReaderBuilder builder;
    std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
    std::string errorMsg;
    if (!reader->parse(&*jsonString.begin(), &*jsonString.end(), &jsonObjRoot, &errorMsg)) {
        PGCGS_LOGE("ParseConfigFileToJsonObj: parse json failed, errorMsg:%{public}s", errorMsg.c_str());
        return false;
    }
    return true;
}
} // namespace CgroupSetting
} // namespace ResourceSchedule
} // namespace OHOS
