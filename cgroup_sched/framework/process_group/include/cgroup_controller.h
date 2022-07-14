
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

#ifndef CGROUP_SCHED_FRAMEWORK_PROCESS_GROUP_INCLUDE_CGROUP_CONTROLLER_H_
#define CGROUP_SCHED_FRAMEWORK_PROCESS_GROUP_INCLUDE_CGROUP_CONTROLLER_H_

#include <map>                // for map
#include <string>             // for basic_string
#include <functional>         // for less
#include <iosfwd>             // for string
#include "sched_policy.h"     // for SchedPolicy

namespace OHOS {
namespace ResourceSchedule {
namespace CgroupSetting {
class CgroupController {
public:
    CgroupController(const std::string& name, const std::string& path);
    ~CgroupController();

    CgroupController(const CgroupController& controller) = delete;
    CgroupController& operator=(const CgroupController& controller) = delete;
    CgroupController(CgroupController&& controller);
    CgroupController& operator=(CgroupController&& controller);

    const inline std::string GetName() const
    {
        return name_;
    }

    const inline std::string GetPath() const
    {
        return path_;
    }
    bool IsEnabled();
    bool SetThreadSchedPolicy(int tid, SchedPolicy policy, bool isSetThreadGroup);
    bool AddSchedPolicy(SchedPolicy policy, const std::string& subgroup);
    bool GetTaskGroup(int tid, std::string& group);

private:
    std::string name_;
    std::string path_;
    std::map<SchedPolicy, int> policyToTaskFd_;
    std::map<SchedPolicy, int> policyToProcFd_;

    bool AddThreadSchedPolicy(SchedPolicy policy, const std::string& subgroup);
    bool AddThreadGroupSchedPolicy(SchedPolicy policy, const std::string& subgroup);
    static bool AddTidToCgroup(int tid, int fd);
};
} // namespace CgroupSetting
} // namespace ResourceSchedule
} // namespace OHOS
#endif // CGROUP_SCHED_FRAMEWORK_PROCESS_GROUP_INCLUDE_CGROUP_CONTROLLER_H_
