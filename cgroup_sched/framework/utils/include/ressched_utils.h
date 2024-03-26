/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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

#ifndef CGROUP_SCHED_FRAMEWORK_UTILS_INCLUDE_RESSCHED_UTILS_H_
#define CGROUP_SCHED_FRAMEWORK_UTILS_INCLUDE_RESSCHED_UTILS_H_

#include <iostream>
#include "cgroup_adjuster.h"
#include "supervisor.h"
#include "sys/types.h"
#include "nlohmann/json.hpp"

namespace OHOS {
namespace ResourceSchedule {
using ReportDataFunc = void (*)(uint32_t resType, int64_t value, const nlohmann::json& payload);
using ReportArbitrationResultFunc = void (*)(Application &app, ProcessRecord &pr, AdjustSource source);
using ReportSysEventFunc = void (*)(Application &app, ProcessRecord &pr, uint32_t resType, int32_t state);
using DispatchResourceExtFunc = void (*)(uint32_t resType, int64_t value, const nlohmann::json& payload);
using ReportAppStateFunc = void (*)(int32_t state, int32_t pid);
class ResSchedUtils {
public:
    static ResSchedUtils& GetInstance();
    void ReportDataInProcess(uint32_t resType, int64_t value, const nlohmann::json& payload);
    void ReportArbitrationResult(Application &app, ProcessRecord &pr, AdjustSource source);
    void ReportSysEvent(Application &app, ProcessRecord &pr, uint32_t resType, int32_t state);
    void DispatchResourceExt(uint32_t resType, int64_t value, const nlohmann::json& payload);
    bool CheckTidIsInPid(int32_t pid, int32_t tid);
    void ReportAppStateInProcess(int32_t state, int32_t pid);

private:
    ResSchedUtils()
    {
        LoadUtils();
        LoadUtilsExtra();
    }
    ~ResSchedUtils()
    {
        reportFunc_ = nullptr;
        reportArbitrationResultFunc_ = nullptr;
        reportSysEventFunc_ = nullptr;
    }
    void LoadUtils();
    void LoadUtilsExtra();

    ResSchedUtils(const ResSchedUtils&) = delete;
    ResSchedUtils& operator=(const ResSchedUtils &) = delete;
    ResSchedUtils(ResSchedUtils&&) = delete;
    ResSchedUtils& operator=(ResSchedUtils&&) = delete;

    ReportDataFunc reportFunc_ = nullptr;
    ReportArbitrationResultFunc reportArbitrationResultFunc_ = nullptr;
    ReportSysEventFunc reportSysEventFunc_ = nullptr;
    DispatchResourceExtFunc dispatchResourceExtFunc_ = nullptr;
    ReportAppStateFunc reportAppStateFunc_ = nullptr;
};
} // namespace ResourceSchedule
} // namespace OHOS
#endif // CGROUP_SCHED_FRAMEWORK_UTILS_INCLUDE_RESSCHED_UTILS_H_
