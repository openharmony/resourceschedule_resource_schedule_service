/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef RESSCHED_INTERFACES_INNERKITS_RESSCHED_CLIENT_INCLUDE_RES_SCHED_KILL_REASON_H
#define RESSCHED_INTERFACES_INNERKITS_RESSCHED_CLIENT_INCLUDE_RES_SCHED_KILL_REASON_H

#include <string>

namespace OHOS {
namespace ResourceSchedule {
class KillReason {
public:
    static constexpr char CPU_HIGHLOAD[] = "Kill Reason:CPU Highload";
    static constexpr char CPU_EXT_HIGHLOAD[] = "Kill Reason:CPU_EXT Highload";
    static constexpr char IO_MANAGE_CONTROL[] = "Kill Reason:IO Manage Control";
    static constexpr char MEMORY_DETERIORATION[] = "Kill Reason:App Memory Deterioration";
    static constexpr char MEMORY_PRESSURE[] = "Kill Reason:Memory Pressure";
    static constexpr char TEMPERATURE_CONTROL[] = "Kill Reason:Temperature Control";
};
} // namespace ResourceSchedule
} // namespace OHOS

#endif // RESSCHED_INTERFACES_INNERKITS_RESSCHED_CLIENT_INCLUDE_RES_SCHED_KILL_REASON_H
