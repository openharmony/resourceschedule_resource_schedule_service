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

#ifndef CGROUP_SCHED_FRAMEWORK_PROCESS_GROUP_INCLUDE_SCHED_POLICY_H_
#define CGROUP_SCHED_FRAMEWORK_PROCESS_GROUP_INCLUDE_SCHED_POLICY_H_

#include <cstdint>    // for uint32_t
#include <iosfwd>     // for string

namespace OHOS {
namespace ResourceSchedule {
namespace CgroupSetting {
using SchedPolicy = uint32_t;

/**
 * Schedule policy define, keep in sync with
 * (1) cgroup_controller.cpp: bool QuerySchedPolicyFullName(const std::string& name, SchedPolicy& policy);
 * (2) sched_policy.cpp: const char* GetSchedPolicyShortenedName(SchedPolicy policy);
 */

static constexpr SchedPolicy SP_DEFAULT = 0;
static constexpr SchedPolicy SP_BACKGROUND = 1;
static constexpr SchedPolicy SP_FOREGROUND = 2;
static constexpr SchedPolicy SP_SYSTEM_BACKGROUND = 3;
static constexpr SchedPolicy SP_TOP_APP = 4;
static constexpr SchedPolicy SP_SYSTEM_DEFAULT = SP_DEFAULT;

static constexpr SchedPolicy SP_UPPER_LIMIT = 0xff;

/**
 * Assign all threads in process pid to the cgroup associated with the specified policy.
 * On platforms which support getpid(), zero pid means current process.
 * Return value: 0 for success, or -errno for error.
 *
 * @param pid process id.
 * @param policy schedule policy.
 * @return Return value: 0 for success, or -errno for error.
 */
int SetThreadGroupSchedPolicy(int pid, int policy);

/**
 * Assign thread tid to the cgroup associated with the specified policy.
 * On platforms which support gettid(), zero tid means current thread.
 * If the thread is a thread group leader, that is it's gettid() == getpid(),
 * then the other threads in the same thread group are _not_ affected.
 * Return value: 0 for success, or -errno for error.
 *
 * @param tid thread id.
 * @param policy variable to accept return value.
 * @return Return value: 0 for success, or -errno for error.
 */
int SetThreadSchedPolicy(int pid, int policy);

/**
 * Return the policy associated with the cgroup of thread tid via policy pointer.
 * On platforms which support gettid(), zero tid means current thread.
 * Return value: 0 for success, or -1 for error and set errno.
 *
 * @param tid thread id.
 * @param policy a policy pointer.
 * @return Return value: 0 for success, or -errno for error.
 */
int GetThreadSchedPolicy(int tid, SchedPolicy* policy);

/**
 * Return the policy associated with policy name via policy pointer.
 * Return value: 0 for success, or -1 for error and set errno.
 *
 * @param name policy full name.
 * @param policy a policy pointer.
 * @return Return value: 0 for success, or -errno for error.
 */
int GetSchedPolicyByName(const std::string& name, SchedPolicy* policy);

/**
 * Return a displayable string corresponding to policy: shortened name.
 * Return value: NUL-terminated name of unspecified length, nullptr if invalid;
 * the caller is responsible for displaying the useful part of the string.
 *
 * @param policy schedule policy.
 * @return Return a displayable string corresponding to policy.
 */
const char* GetSchedPolicyShortenedName(SchedPolicy policy);

/**
 * Return a displayable string corresponding to policy: full name.
 * Return value: NUL-terminated name of unspecified length, nullptr if invalid;
 * the caller is responsible for displaying the useful part of the string.
 *
 * @param policy schedule policy.
 * @return Return a displayable string corresponding to policy.
 */
const char* GetSchedPolicyFullName(SchedPolicy policy);

/**
 * Add a new kind of sched policy, with unique policy id, full
 * name and abbreviation name.
 *
 * @param policy schedule policy.
 * @param fullName full name for policy.
 * @param abbrName abbreviation name for policy.
 */
void AddSchedPolicyDeclaration(SchedPolicy policy, const std::string& fullName, const std::string& abbrName);
} // namespace CgroupSetting
} // namespace ResourceSchedule
} // namespace OHOS

#endif // CGROUP_SCHED_FRAMEWORK_PROCESS_GROUP_INCLUDE_SCHED_POLICY_H_
