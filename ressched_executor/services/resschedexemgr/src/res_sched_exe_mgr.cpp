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
    constexpr int32_t SIGNAL_KILL = 9;
    const int32_t MAX_CONFIG_SIZE = 1024 * 1024;
    const std::string STR_CONFIG_READER = "config";
    const std::string STR_PLUGIN_SWITCH = "switch";

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
    if (resType == ResExeType::RES_TYPE_EXECUTOR_PLUGIN_INIT) {
        InitPluginMgr(payload);
        return;
    }
    std::string traceStr = BuildTraceStr(__func__, resType, value);
    HitraceScoped hitrace(HITRACE_TAG_OHOS, traceStr);
    PluginMgr::GetInstance().DispatchResource(std::make_shared<ResData>(resType, value, payload));
}

void ResSchedExeMgr::InitPluginMgr(const nlohmann::json& payload)
{
    if (!payload.contains(STR_CONFIG_READER) || !payload[STR_CONFIG_READER].is_string()
        || !payload.contains(STR_PLUGIN_SWITCH) || !payload[STR_PLUGIN_SWITCH].is_string()) {
        RSSEXE_LOGE("recieve config string error");
        return;
    }

    std::vector<std::string> configStrs = payload[STR_CONFIG_READER].get<std::vector<std::string>>();
    std::vector<std::string> switchStr = payload[STR_PLUGIN_SWITCH].get<std::vector<std::string>>();
    for (auto configStr : configStrs) {
        if (configStr.size() > MAX_CONFIG_SIZE) {
            RSSEXE_LOGE("recieve config string too large");
            return;
        }
        PluginMgr::GetInstance().ParseConfigReader(configStr);
    }

    for (auto switchStr : switchStrs) {
        if (switchStr.size() > MAX_CONFIG_SIZE) {
            RSSEXE_LOGE("recieve switch config string too large");
            return;
        }
        PluginMgr::GetInstance().ParseConfigReader(switchStr, true);
    }
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
