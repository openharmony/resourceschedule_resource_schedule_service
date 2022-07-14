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
#include "sched_policy.h"
#include <unistd.h>         // for gettid, getpid
#include "cgroup_action.h"  // for CgroupAction

namespace OHOS {
namespace ResourceSchedule {
namespace CgroupSetting {
int SetThreadSchedPolicy(int tid, int policy)
{
    if (tid < 0) {
        return -1;
    }
    if (tid == 0) {
        tid = gettid();
    }
    SchedPolicy schedPolicy = SchedPolicy(policy);
    return CgroupAction::GetInstance().SetThreadSchedPolicy(tid, schedPolicy) ? 0 : -1;
}

int SetThreadGroupSchedPolicy(int pid, int policy)
{
    if (pid < 0) {
        return -1;
    }
    if (pid == 0) {
        pid = getpid();
    }
    SchedPolicy schedPolicy = SchedPolicy(policy);
    return CgroupAction::GetInstance().SetThreadGroupSchedPolicy(pid, schedPolicy) ? 0 : -1;
}

int GetThreadSchedPolicy(int tid, SchedPolicy* policy)
{
    if (tid < 0) {
        return -1;
    }
    if (tid == 0) {
        tid = gettid();
    }
    return CgroupAction::GetInstance().GetSchedPolicy(tid, policy);
}

int GetSchedPolicyByName(const std::string& name, SchedPolicy* policy)
{
    return CgroupAction::GetInstance().GetSchedPolicyByName(name, policy);
}

const char* GetSchedPolicyShortenedName(SchedPolicy policy)
{
    return CgroupAction::GetInstance().GetSchedPolicyAbbrName(policy);
}

const char* GetSchedPolicyFullName(SchedPolicy policy)
{
    return CgroupAction::GetInstance().GetSchedPolicyFullName(policy);
}

void AddSchedPolicyDeclaration(SchedPolicy policy, const std::string& fullName, const std::string& abbrName)
{
    return CgroupAction::GetInstance().AddSchedPolicyDeclaration(policy, fullName, abbrName);
}
} // namespace CgroupSetting
} // namespace ResourceSchedule
} // namespace OHOS
