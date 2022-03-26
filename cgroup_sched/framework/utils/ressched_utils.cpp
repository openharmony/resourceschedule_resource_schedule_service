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
#include "json/value.h"

namespace OHOS {
namespace ResourceSchedule {
namespace {
    const std::string RES_SCHED_CLIENT_SO = "/system/lib/libressched_client.z.so";
    constexpr HiviewDFX::HiLogLabel LOG_LABEL = {LOG_CORE, LOG_TAG_DOMAIN_ID_RMS, "ResSchedUtils"};
}

ResSchedUtils& ResSchedUtils::GetInstance()
{
    static ResSchedUtils instance;
    return instance;
}

void ResSchedUtils::LoadUtils()
{
    auto handle = dlopen(RES_SCHED_CLIENT_SO.c_str(), RTLD_NOW);
    if (!handle) {
        CGS_LOGE("%{public}s load %{public}s failed!", __func__, RES_SCHED_CLIENT_SO.c_str());
        return;
    }

    auto func = reinterpret_cast<ReportDataFunc>(dlsym(handle, "ReportDataInProcess"));
    if (!func) {
        CGS_LOGE("%{public}s load function:ReportDataInProcess failed!", __func__);
        dlclose(handle);
        return;
    }
    reportFunc_ = func;
}

void ResSchedUtils::ReportDataInProcess(uint32_t resType, int64_t value, const Json::Value& payload)
{
    if (!reportFunc_) {
        CGS_LOGE("%{public}s failed, function nullptr.", __func__);
        return;
    }
    reportFunc_(resType, value, payload);
}
} // namespace ResourceSchedule
} // namespace OHOS
