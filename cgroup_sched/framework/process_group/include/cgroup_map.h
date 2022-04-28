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

#ifndef CGROUP_SCHED_FRAMEWORK_PROCESS_GROUP_INCLUDE_CGROUP_MAP_H_
#define CGROUP_SCHED_FRAMEWORK_PROCESS_GROUP_INCLUDE_CGROUP_MAP_H_

#include <string>
#include <map>
#include <memory>
#include "sched_policy.h"
#include "json/value.h"
#include "cgroup_controller.h"

namespace OHOS {
namespace ResourceSchedule {
namespace CgroupSetting {
class CgroupMap {
public:
    static CgroupMap& GetInstance();

    bool SetThreadSchedPolicy(int tid, SchedPolicy policy, bool isSetThreadGroup);
    bool LoadConfigFromJsonObj(const Json::Value& jsonObj);
    bool FindFristEnableCgroupController(CgroupController** p);

private:
    CgroupMap() = default;
    ~CgroupMap() = default;

    CgroupMap(const CgroupMap&) = delete;
    CgroupMap& operator=(const CgroupMap &) = delete;
    CgroupMap(CgroupMap&&) = delete;
    CgroupMap& operator=(CgroupMap&&) = delete;

    std::map<std::string, CgroupController> controllers_;

    void AddCgroupController(const std::string& name, CgroupController& controller);
    static bool CheckCgroupJsonConfig(const Json::Value& cgroupObj);
};
} // namespace CgroupSetting
} // namespace ResourceSchedule
} // namespace OHOS
#endif // CGROUP_SCHED_FRAMEWORK_PROCESS_GROUP_INCLUDE_CGROUP_MAP_H_
