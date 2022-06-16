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

#ifndef CGROUP_SCHED_COMMON_INCLUDE_CGROUP_SCHED_COMMON_H_
#define CGROUP_SCHED_COMMON_INCLUDE_CGROUP_SCHED_COMMON_H_

#include <chrono>
#include <ctime>

#include "cgroup_sched_log.h"

namespace OHOS {
namespace ResourceSchedule {
using Clock = std::chrono::high_resolution_clock;
using MilliSecondsType = std::chrono::duration<double, std::milli>;

class ChronoScope {
public:
    explicit ChronoScope(const std::string outmsg) : outmsg_(outmsg)
    {
        out_ = nullptr;
        t1 = Clock::now();
    }

    ChronoScope(const std::string outmsg, double* out) : outmsg_(outmsg), out_(out)
    {
        t1 = Clock::now();
    }

    ~ChronoScope()
    {
        Clock::time_point t2 = Clock::now();
        MilliSecondsType time_span = std::chrono::duration_cast<MilliSecondsType>(t2 - t1);
        HiviewDFX::HiLogLabel LOG_LABEL = {LOG_CORE, LOG_TAG_DOMAIN_ID_RMS, "ChronoScope"};
        CGS_LOGD("[%{public}s] cost %{public}lf milliseconds.", outmsg_.c_str(), time_span.count());
        if (out_) {
            *out_ = time_span.count();
        }
    }

private:
    ChronoScope(const ChronoScope&) = delete;
    ChronoScope& operator=(const ChronoScope &) = delete;
    ChronoScope(ChronoScope&&) = delete;
    ChronoScope& operator=(ChronoScope&&) = delete;

    std::string outmsg_;
    double* out_;
    Clock::time_point t1;
};
} // namespace ResourceSchedule
} // namespace OHOS
#endif // CGROUP_SCHED_COMMON_INCLUDE_CGROUP_SCHED_COMMON_H_
