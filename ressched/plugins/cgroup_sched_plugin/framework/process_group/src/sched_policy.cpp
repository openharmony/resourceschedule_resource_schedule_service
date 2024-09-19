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
#include "parameters.h"     // for GetParameter
#include "process_group_log.h" // for PGCGS_LOGI, PGCGS_LOGE
#include "res_exe_type.h"   // for ResExeType
#include "res_sched_exe_client.h" // for ResSchedExeClient

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
    int ret = CgroupAction::GetInstance().SetThreadSchedPolicy(tid, schedPolicy) ? 0 : -1;
    bool isLinuxKernel = system::GetParameter("ohos.boot.kernel", "").size() > 0;
    if (isLinuxKernel) {
        nlohmann::json payload;
        payload["tid"] = tid;
        payload["policy"] = policy;
        nlohmann::json reply;
        ret = ResourceSchedule::ResSchedExeClient::GetInstance().SendRequestSync(
            ResourceSchedule::ResExeType::RES_TYPE_SET_THREAD_SCHED_POLICY_SYNC_EVENT, 0, payload, reply);
        if (ret != 0) {
            PGCGS_LOGE("%{public}s SendRequestSync RES_TYPE_SET_THREAD_SCHED_POLICY_SYNC_EVENT failed", __func__);
            return ret;
        }
        if (reply.contains("ret") && reply.at("ret").is_string()) {
            ret = atoi(reply["ret"].get<std::string>().c_str());
        }
    }
    return ret;
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
    int ret = CgroupAction::GetInstance().SetThreadGroupSchedPolicy(pid, schedPolicy) ? 0 : -1;
    bool isLinuxKernel = system::GetParameter("ohos.boot.kernel", "").size() > 0;
    if (isLinuxKernel) {
        nlohmann::json payload;
        payload["tid"] = pid;
        payload["policy"] = policy;
        nlohmann::json reply;
        ret = ResourceSchedule::ResSchedExeClient::GetInstance().SendRequestSync(
            ResourceSchedule::ResExeType::RES_TYPE_SET_THREAD_GROUP_SCHED_POLICY_SYNC_EVENT, 0, payload, reply);
        if (ret != 0) {
            PGCGS_LOGE("%{public}s SendRequestSync RES_TYPE_SET_THREAD_GROUP_SCHED_POLICY_SYNC_EVENT failed", __func__);
            return ret;
        }
        if (reply.contains("ret") && reply.at("ret").is_string()) {
            ret = atoi(reply["ret"].get<std::string>().c_str());
        }
    }
    return ret;
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
