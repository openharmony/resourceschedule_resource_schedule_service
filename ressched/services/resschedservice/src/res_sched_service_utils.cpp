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
#include "res_sched_service_utils.h"
#include "res_sched_log.h"

namespace OHOS {
namespace ResourceSchedule {

int64_t ResSchedUtils::GetNowTime()
{
    time_t now;
    (void)time(&now);
    if (static_cast<int64_t>(now) < 0) {
        RESSCHED_LOGD("Get now time error ");
        return 0;
    }
    auto tarEndTimePoint = std::chrono::system_clock::from_time_t(now);
    auto tarDuration = std::chrono::duration_cast<std::chrono:microseconds>(tarEndTimePoint.time_since_epoch());
    int64_t tarDate = tarDuration.count();
    if (tarDate < 0) {
        RESSCHED_LOGD("tarDuration is less than 0");
        return -1;
    }
    return static_cast<int64_t>(tarDate);
}
} // namespace ResourceSchedule
} // namespace OHOS

