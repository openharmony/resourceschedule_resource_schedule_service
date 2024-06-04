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
#include <csignal>
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

namespace {
    constexpr int32_t SIGNAL_KILL = 9;
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
}

int32_t ResSchedExeMgr::SendRequestSync(uint32_t resType, int64_t value,
    const nlohmann::json& payload, nlohmann::json& reply)
{
    RSSEXE_LOGD("receive resType = %{public}u, value = %{public}lld.", resType, (long long)value);
    std::string traceStr = BuildTraceStr(__func__, resType, value);
    HitraceScoped hitrace(HITRACE_TAG_OHOS, traceStr);
    auto resData = std::make_shared<ResData>(resType, value, payload, reply);
    int32_t ret = PluginMgr::GetInstance().DeliverResource(resData);
    if (ret != ResIpcErrCode::RSSEXE_PLUGIN_ERROR) {
        reply["retCode"] = std::to_string(ret);
    }
    return ResErrCode::RSSEXE_NO_ERR;
}

int32_t ResSchedExeMgr::KillProcess(pid_t pid)
{
    int32_t killRes = kill(pid, SIGNAL_KILL);
    return killRes;
}

void ResSchedExeMgr::SendRequestAsync(uint32_t resType, int64_t value, const nlohmann::json& payload)
{
    RSSEXE_LOGD("receive resType = %{public}u, value = %{public}lld.", resType, (long long)value);
    std::string traceStr = BuildTraceStr(__func__, resType, value);
    HitraceScoped hitrace(HITRACE_TAG_OHOS, traceStr);
    PluginMgr::GetInstance().DispatchResource(std::make_shared<ResData>(resType, value, payload));
}

std::string ResSchedExeMgr::BuildTraceStr(const std::string& func, uint32_t resType, int64_t value)
{
    std::string trace_str(func);
    trace_str.append(",resType[").append(std::to_string(resType)).append("]");
    trace_str.append(",value[").append(std::to_string(value)).append("]");
    return trace_str;
}
} // namespace ResourceSchedule
} // namespace OHOS
