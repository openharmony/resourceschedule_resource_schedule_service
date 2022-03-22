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

#include <array>
#include <string>
#include <unistd.h>
#include "cgroup_action.h"
#include "cgroup_map.h"
#include "process_group_util.h"

namespace OHOS {
namespace ResourceSchedule {
namespace CgroupSetting {
int SetThreadSchedPolicy(int tid, int policy)
{
    if (policy < SP_DEFAULT || policy > SP_MAX || tid < 0) {
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
    if (policy < SP_DEFAULT || policy > SP_MAX || pid < 0) {
        return -1;
    }
    if (pid == 0) {
        pid = getpid();
    }
    SchedPolicy schedPolicy = SchedPolicy(policy);
    return  CgroupAction::GetInstance().SetThreadGroupSchedPolicy(pid, schedPolicy) ? 0 : -1;
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

const char* GetSchedPolicyShortenedName(SchedPolicy policy)
{
    static constexpr const char*  kShortenedNames[] = {
        [SP_DEFAULT] = "df",
        [SP_BACKGROUND] = "bg",
        [SP_FOREGROUND] = "fg",
        [SP_SYSTEM] = "sy",
        [SP_TOP_APP] = "ta",
    };
    constexpr int size = sizeof(kShortenedNames) / sizeof(const char*);
    static_assert(size == SP_CNT, "miss name");
    if (policy >= SP_DEFAULT && policy < SP_CNT && kShortenedNames[policy]) {
        return kShortenedNames[policy];
    } else {
        return "error";
    }
}

const char* GetSchedPolicyFullName(SchedPolicy policy)
{
    static constexpr const char*  kFullNames[] = {
        [SP_DEFAULT] = "sp_default",
        [SP_BACKGROUND] = "sp_background",
        [SP_FOREGROUND] = "sp_foreground",
        [SP_SYSTEM] = "sp_system",
        [SP_TOP_APP] = "sp_top_app",
    };
    constexpr int size = sizeof(kFullNames) / sizeof(const char*);
    static_assert(size == SP_CNT, "miss name");
    if (policy >= SP_DEFAULT && policy < SP_CNT && kFullNames[policy]) {
        return kFullNames[policy];
    } else {
        return "error";
    }
}
} // namespace CgroupSetting
} // namespace ResourceSchedule
} // namespace OHOS
