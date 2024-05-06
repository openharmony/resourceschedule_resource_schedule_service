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

#include "res_sched_exe_mgr.h"

#include <cinttypes>
#include <map>

#include "hitrace_meter.h"

#include "plugin_mgr.h"
#include "res_exe_type.h"
#include "res_sched_exe_constants.h"
#include "res_sched_exe_log.h"

namespace OHOS {
namespace ResourceSchedule {
namespace {
    const std::map<uint32_t, std::string> resTypeToStr = {
        { ResExeType::RES_TYPE_COMMON_SYNC, "RES_TYPE_COMMON_SYNC" },
        { ResExeType::RES_TYPE_COMMON_ASYNC, "RES_TYPE_COMMON_ASYNC" },
        { ResExeType::RES_TYPE_THERMAL_AWARE_SYNC_EVENT, "THERMAL_AWARE_SYNC_EVENT" },
        { ResExeType::RES_TYPE_THERMAL_AWARE_ASYNC_EVENT, "THERMAL_AWARE_ASYNC_EVENT" },
        { ResExeType::RES_TYPE_DEBUG, "DEBUG_COMMAND" },
    };
}

IMPLEMENT_SINGLE_INSTANCE(ResSchedExeMgr);

void ResSchedExeMgr::Init()
{
    PluginMgr::GetInstance().Init(true);
    PluginMgr::GetInstance().SetResTypeStrMap(resTypeToStr);
}

void ResSchedExeMgr::Stop()
{
    PluginMgr::GetInstance().Stop();
    PluginMgr::GetInstance().ClearResTypeStrMap();
}

int32_t ResSchedExeMgr::SendRequestSync(uint32_t resType, int64_t value,
    const nlohmann::json& payload, nlohmann::json& reply)
{
    RSSEXE_LOGD("receive resType = %{public}u, value = %{public}lld.", resType, (long long)value);
    std::string trace_str(__func__);
    trace_str.append(",resType[").append(std::to_string(resType)).append("]");
    trace_str.append(",value[").append(std::to_string(value)).append("]");
    StartTrace(HITRACE_TAG_OHOS, trace_str, -1);
    int32_t ret = PluginMgr::GetInstance().DispatchResource(std::make_shared<ResData>(resType, value, payload, reply));
    FinishTrace(HITRACE_TAG_OHOS);
    return ret;
}

void ResSchedExeMgr::SendRequestAsync(uint32_t resType, int64_t value, const nlohmann::json& payload)
{
    RSSEXE_LOGD("receive resType = %{public}u, value = %{public}lld.", resType, (long long)value);
    std::string trace_str(__func__);
    trace_str.append(",resType[").append(std::to_string(resType)).append("]");
    trace_str.append(",value[").append(std::to_string(value)).append("]");
    StartTrace(HITRACE_TAG_OHOS, trace_str, -1);
    PluginMgr::GetInstance().DispatchResource(std::make_shared<ResData>(resType, value, payload));
    FinishTrace(HITRACE_TAG_OHOS);
}
} // namespace ResourceSchedule
} // namespace OHOS
