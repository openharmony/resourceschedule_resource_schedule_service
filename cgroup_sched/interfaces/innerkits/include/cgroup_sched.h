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

#ifndef CGROUP_SCHED_INTERFACES_INNERKITS_INCLUDE_CGROUP_SCHED_H_
#define CGROUP_SCHED_INTERFACES_INNERKITS_INCLUDE_CGROUP_SCHED_H_

#include "nlohmann/json.hpp"
#include "sys/types.h"

namespace OHOS {
namespace ResourceSchedule {
extern "C" void CgroupSchedInit();
extern "C" void CgroupSchedDeinit();
extern "C" int GetProcessGroup(pid_t pid);
extern "C" void ReportAbilityStatus(int32_t saId, const std::string& deviceId, uint32_t status);
extern "C" void CgroupSchedDispatch(uint32_t resType, int64_t value, const nlohmann::json& payload);
} // namespace ResourceSchedule
} // namespace OHOS

#endif // CGROUP_SCHED_INTERFACES_INNERKITS_INCLUDE_CGROUP_SCHED_H_
