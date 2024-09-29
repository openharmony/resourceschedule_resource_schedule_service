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

#ifndef RESSCHED_SERVICES_RESSCHEDSERVICE_INCLUDE_RES_SCHED_UTILS_H
#define RESSCHED_SERVICES_RESSCHEDSERVICE_INCLUDE_RES_SCHED_UTILS_H
#include <chrono>

namespace OHOS {
namespace ResourceSchedule {
class ResSchedUtils {
public:
    ResSchedUtils() = default;
    ~ResSchedUtils() = default;
    static int64_t GetNowMillTime();
};
} // namespace ResourceSchedule
} // namespace OHOS

#endif // RESSCHED_SERVICES_RESSCHEDSERVICE_INCLUDE_RES_SCHED_UTILS_H