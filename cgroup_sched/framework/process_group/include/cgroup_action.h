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

#ifndef CGROUP_SCHED_FRAMEWORK_PROCESS_GROUP_INCLUDE_CGROUP_ACTION_H_
#define CGROUP_SCHED_FRAMEWORK_PROCESS_GROUP_INCLUDE_CGROUP_ACTION_H_

#include "cgroup_map.h"

namespace OHOS {
namespace ResourceSchedule {
namespace CgroupSetting {
class CgroupAction {
public:
    static CgroupAction& GetInstance();

    void AddSchedPolicyDeclaration(SchedPolicy policy, const std::string& fullName, const std::string& abbrName);
    std::vector<SchedPolicy> GetSchedPolicyList();
    const char* GetSchedPolicyFullName(SchedPolicy policy);
    const char* GetSchedPolicyAbbrName(SchedPolicy policy);

    bool SetThreadSchedPolicy(int tid, SchedPolicy policy);
    bool SetThreadGroupSchedPolicy(int tid, SchedPolicy policy);
    int GetSchedPolicy(int tid, SchedPolicy* policy);
    int GetSchedPolicyByName(const std::string& name, SchedPolicy* policy);

private:
    CgroupAction();
    ~CgroupAction() = default;

    CgroupAction(const CgroupAction&) = delete;
    CgroupAction& operator=(const CgroupAction &) = delete;
    CgroupAction(CgroupAction&&) = delete;
    CgroupAction& operator=(CgroupAction&&) = delete;

    std::mutex mutex_; // for sched policy maps
    bool allowToAdd_ = true;
    std::map<SchedPolicy, std::string> fullNames_;
    std::map<SchedPolicy, std::string> abbrNames_;

    bool LoadConfigFile();
    bool IsEnabled();
    bool IsSchedPolicyValid(SchedPolicy policy);
    static bool ParseConfigFileToJsonObj(Json::Value& jsonObjRoot);
};
} // namespace CgroupSetting
} // namespace ResourceSchedule
} // namespace OHOS
#endif // CGROUP_SCHED_FRAMEWORK_PROCESS_GROUP_INCLUDE_CGROUP_ACTION_H_
