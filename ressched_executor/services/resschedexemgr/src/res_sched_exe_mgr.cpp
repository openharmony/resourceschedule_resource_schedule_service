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
#include <fstream>
#include <map>
#include <sstream>

#include "cgroup_action.h"
#include "hitrace_meter.h"

#include "plugin_mgr.h"
#include "res_exe_type.h"
#include "res_sched_exe_constants.h"
#include "res_sched_exe_log.h"
#include "directory_ex.h"

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
        { ResExeType::RES_TYPE_SOCK_EXECUTOR_ASYNC_EVENT, "SOCK_EXECUTOR_ASYNC_EVENT" },
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
    switch (resType) {
        case ResExeType::RES_TYPE_EXECUTOR_PLUGIN_INIT:
            InitPluginMgr(payload);
            break;
        case ResExeType::RES_TYPE_CGROUP_SYNC_EVENT:
        case ResExeType::RES_TYPE_CGROUP_PROC_TASK_SYNC_EVENT:
        case ResExeType::RES_TYPE_SET_THREAD_SCHED_POLICY_SYNC_EVENT:
        case ResExeType::RES_TYPE_SET_THREAD_GROUP_SCHED_POLICY_SYNC_EVENT:
            HandleRequestForCgroup(resType, payload, reply);
            break;
        default:
            auto resData = std::make_shared<ResData>(resType, value, payload, reply);
            int32_t ret = PluginMgr::GetInstance().DeliverResource(resData);
            if (ret != ResIpcErrCode::RSSEXE_PLUGIN_ERROR) {
                reply["retCode"] = std::to_string(ret);
            }
            break;
    }
    return ResErrCode::RSSEXE_NO_ERR;
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

int32_t ResSchedExeMgr::KillProcess(pid_t pid)
{
    int32_t killRes = kill(pid, SIGNAL_KILL);
    return killRes;
}

void ResSchedExeMgr::InitPluginMgr(const nlohmann::json& payload)
{
    if (isInit) {
        RSSEXE_LOGE("plugin manager has init");
        return;
    }

    if (!payload.contains(STR_CONFIG_READER) || !payload[STR_CONFIG_READER].is_array()
        || !payload.contains(STR_PLUGIN_SWITCH) || !payload[STR_PLUGIN_SWITCH].is_array()) {
        RSSEXE_LOGE("recieve config string error");
        return;
    }

    isInit = true;
    std::vector<std::string> configStrs = payload[STR_CONFIG_READER].get<std::vector<std::string>>();
    std::vector<std::string> switchStrs = payload[STR_PLUGIN_SWITCH].get<std::vector<std::string>>();
    for (auto configStr : configStrs) {
        if (configStr.size() > MAX_CONFIG_SIZE) {
            RSSEXE_LOGE("recieve config string too large");
            return;
        }
    }
    PluginMgr::GetInstance().ParseConfigReader(configStrs);

    for (auto switchStr : switchStrs) {
        if (switchStr.size() > MAX_CONFIG_SIZE) {
            RSSEXE_LOGE("recieve switch config string too large");
            return;
        }
    }
    PluginMgr::GetInstance().ParsePluginSwitch(switchStrs, true);
}

std::string ResSchedExeMgr::BuildTraceStr(const std::string& func, uint32_t resType, int64_t value)
{
    std::string trace_str(func);
    trace_str.append(",resType[").append(std::to_string(resType)).append("]");
    trace_str.append(",value[").append(std::to_string(value)).append("]");
    return trace_str;
}

void ResSchedExeMgr::HandleRequestForCgroup(uint32_t resType, const nlohmann::json& payload, nlohmann::json& reply)
{
    if (resType != ResExeType::RES_TYPE_CGROUP_SYNC_EVENT &&
        resType != ResExeType::RES_TYPE_CGROUP_PROC_TASK_SYNC_EVENT &&
        resType != ResExeType::RES_TYPE_SET_THREAD_SCHED_POLICY_SYNC_EVENT &&
        resType != ResExeType::RES_TYPE_SET_THREAD_GROUP_SCHED_POLICY_SYNC_EVENT) {
        return;
    }

    int tid;
    int policy;
    CgroupSetting::SchedPolicy schedPolicy;
    if (resType == ResExeType::RES_TYPE_SET_THREAD_SCHED_POLICY_SYNC_EVENT ||
        resType == ResExeType::RES_TYPE_SET_THREAD_GROUP_SCHED_POLICY_SYNC_EVENT) {
        if (!(ParseValue(tid, "tid", payload)) || !(ParseValue(policy, "policy", payload))) {
            RSSEXE_LOGE("%{public}s : ParseValue failed", __func__);
            return;
        }
        schedPolicy = CgroupSetting::SchedPolicy(policy);
    }

    int ret = 0;
    switch (resType) {
        case ResExeType::RES_TYPE_CGROUP_SYNC_EVENT:
            GetCgroupFileContent(resType, payload, reply);
            break;
        case ResExeType::RES_TYPE_CGROUP_PROC_TASK_SYNC_EVENT:
            CheckProcTaskForCgroup(resType, payload, reply);
            break;
        case ResExeType::RES_TYPE_SET_THREAD_SCHED_POLICY_SYNC_EVENT:
            ret = CgroupSetting::CgroupAction::GetInstance().SetThreadSchedPolicy(tid, schedPolicy) ? 0 : -1;
            reply["ret"] = std::to_string(ret);
            break;
        case ResExeType::RES_TYPE_SET_THREAD_GROUP_SCHED_POLICY_SYNC_EVENT:
            ret = CgroupSetting::CgroupAction::GetInstance().SetThreadGroupSchedPolicy(tid, schedPolicy) ? 0 : -1;
            reply["ret"] = std::to_string(ret);
            break;
        default:
            break;
    }
    return;
}

bool ResSchedExeMgr::ParseValue(int32_t& value, const char* name, const nlohmann::json& payload)
{
    if (payload.contains(name) && payload.at(name).is_string()) {
        value = atoi(payload[name].get<std::string>().c_str());
        return true;
    }
    return false;
}

void ResSchedExeMgr::GetCgroupFileContent(uint32_t resType, const nlohmann::json& payload, nlohmann::json& reply)
{
    if (!payload.contains("pid") || !payload["pid"].is_number_integer()) {
        return;
    }
    int pid = payload["pid"];
    std::string path = std::string("/proc/").append(std::to_string(pid)).append("/cgroup");
    char resolvedPath[PATH_MAX] = { 0 };
    if (path.size() > PATH_MAX || !realpath(path.c_str(), resolvedPath)) {
        RSSEXE_LOGE("%{public}s realpath failed", __func__);
        return;
    }
    auto realPath = std::string(resolvedPath);
    std::ifstream fin(realPath.c_str(), std::ios::in);
    if (!fin) {
        return;
    }
    std::stringstream ss;
    ss << fin.rdbuf();
    reply["res"] = ss.str();
}

void ResSchedExeMgr::CheckProcTaskForCgroup(uint32_t resType, const nlohmann::json& payload, nlohmann::json& reply)
{
    if (!payload.contains("pid") || !payload["pid"].is_number_integer()) {
        return;
    }
    if (!payload.contains("tid") || !payload["tid"].is_number_integer()) {
        return;
    }
    int pid = payload["pid"];
    int tid = payload["tid"];
    std::string pathName = std::string("/proc/").append(std::to_string(pid))
        .append("/task/").append(std::to_string(tid)).append("/comm");
    std::string realPath;
    reply["res"] = PathToRealPath(pathName, realPath);
    return;
}
} // namespace ResourceSchedule
} // namespace OHOS
