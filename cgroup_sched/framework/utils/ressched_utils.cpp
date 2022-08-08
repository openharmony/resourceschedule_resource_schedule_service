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
#include "ressched_utils.h"

#include <dlfcn.h>
#include "cgroup_sched_log.h"
#include "nlohmann/json.hpp"

namespace OHOS {
namespace ResourceSchedule {
namespace {
#ifdef __aarch64__
    const std::string RES_SCHED_SERVICE_SO = "/system/lib64/libresschedsvc.z.so";
    const std::string RES_SCHED_CG_EXT_SO = "/system/lib64/libcgroup_sched_ext.z.so";
#else
    const std::string RES_SCHED_SERVICE_SO = "/system/lib/libresschedsvc.z.so";
    const std::string RES_SCHED_CG_EXT_SO = "/system/lib/libcgroup_sched_ext.z.so";
#endif
    constexpr HiviewDFX::HiLogLabel LOG_LABEL = {LOG_CORE, LOG_TAG_DOMAIN_ID_RMS, "ResSchedUtils"};
}

ResSchedUtils& ResSchedUtils::GetInstance()
{
    static ResSchedUtils instance;
    return instance;
}

void ResSchedUtils::LoadUtils()
{
    auto handle = dlopen(RES_SCHED_SERVICE_SO.c_str(), RTLD_NOW);
    if (!handle) {
        CGS_LOGW("%{public}s load %{public}s failed!", __func__, RES_SCHED_SERVICE_SO.c_str());
        return;
    }

    reportFunc_ = reinterpret_cast<ReportDataFunc>(dlsym(handle, "ReportDataInProcess"));
    if (!reportFunc_) {
        CGS_LOGW("%{public}s load function:ReportDataInProcess failed!", __func__);
        dlclose(handle);
        return;
    }
}

void ResSchedUtils::LoadUtilsExtra()
{
    auto handle = dlopen(RES_SCHED_CG_EXT_SO.c_str(), RTLD_NOW);
    if (!handle) {
        CGS_LOGD("%{public}s load %{public}s failed! errno:%{public}d", __func__, RES_SCHED_CG_EXT_SO.c_str(), errno);
        return;
    }

    reportArbitrationResultFunc_ =
        reinterpret_cast<ReportArbitrationResultFunc>(dlsym(handle, "ReportArbitrationResult"));
    if (!reportArbitrationResultFunc_) {
        CGS_LOGD("%{public}s load function:ReportArbitrationResult failed! errno:%{public}d", __func__, errno);
        dlclose(handle);
        return;
    }
}

void ResSchedUtils::ReportDataInProcess(uint32_t resType, int64_t value, const nlohmann::json& payload)
{
    if (!reportFunc_) {
        CGS_LOGD("%{public}s failed, function nullptr.", __func__);
        return;
    }
    reportFunc_(resType, value, payload);
}

void ResSchedUtils::ReportArbitrationResult(Application &app, ProcessRecord &pr, AdjustSource source)
{
    if (!reportArbitrationResultFunc_) {
        CGS_LOGD("%{public}s failed, function nullptr.", __func__);
        return;
    }
    reportArbitrationResultFunc_(app, pr, source);
}
} // namespace ResourceSchedule
} // namespace OHOS
