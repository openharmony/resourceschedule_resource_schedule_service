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

#include "res_sched_mgr.h"
#include <cinttypes>
#include "cgroup_sched.h"
#include "notifier_mgr.h"
#include "res_sched_log.h"
#include "plugin_mgr.h"
#include "hitrace_meter.h"

namespace OHOS {
namespace ResourceSchedule {
using namespace AppExecFwk;

IMPLEMENT_SINGLE_INSTANCE(ResSchedMgr);

void ResSchedMgr::Init()
{
    PluginMgr::GetInstance().Init();
    PluginMgr::GetInstance().SetResTypeStrMap(ResType::resTypeToStr);

    if (!killProcess_) {
        killProcess_ = std::make_shared<KillProcess>();
    }
}

void ResSchedMgr::Stop()
{
    PluginMgr::GetInstance().Stop();
    PluginMgr::GetInstance().ClearResTypeStrMap();
}

void ResSchedMgr::ReportData(uint32_t resType, int64_t value, const nlohmann::json& payload)
{
    ReportDataInner(resType, value, payload);
    DispatchResourceInner(resType, value, payload);
}

void ResSchedMgr::ReportDataInner(uint32_t resType, int64_t value, const nlohmann::json& payload)
{
    RESSCHED_LOGD("%{public}s, receive resType = %{public}u, value = %{public}lld.", __func__,
        resType, (long long)value);
    std::string trace_str(__func__);
    trace_str.append(",resType[").append(std::to_string(resType)).append("]");
    trace_str.append(",value[").append(std::to_string(value)).append("]");
    StartTrace(HITRACE_TAG_OHOS, trace_str, -1);
    PluginMgr::GetInstance().DispatchResource(std::make_shared<ResData>(resType, value, payload));
    FinishTrace(HITRACE_TAG_OHOS);
}

int32_t ResSchedMgr::KillProcessByClient(const nlohmann::json& payload)
{
    return killProcess_->KillProcessByPidWithClient(payload);
}

void ResSchedMgr::DispatchResourceInner(uint32_t resType, int64_t value, const nlohmann::json& payload)
{
    CgroupSchedDispatch(resType, value, payload);
}

extern "C" void ReportDataInProcess(uint32_t resType, int64_t value, const nlohmann::json& payload)
{
    ResSchedMgr::GetInstance().ReportData(resType, value, payload);
}

extern "C" void ReportAppStateInProcess(int32_t state, int32_t pid)
{
    NotifierMgr::GetInstance().OnApplicationStateChange(state, pid);
}
} // namespace ResourceSchedule
} // namespace OHOS
