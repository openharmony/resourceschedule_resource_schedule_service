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
#include "ressched_utils.h"

#include <dlfcn.h>
#include "cgroup_sched_log.h"
#include "hisysevent.h"
#include "res_exe_type.h"
#include "res_sched_exe_client.h"
#include "nlohmann/json.hpp"

#undef LOG_TAG
#define LOG_TAG "ResSchedUtils"

namespace OHOS {
namespace ResourceSchedule {
namespace {
    const std::string RES_SCHED_SERVICE_SO = "libresschedsvc.z.so";
    const std::string RES_SCHED_CG_EXT_SO = "libcgroup_sched_ext.z.so";
    const int32_t UID_TRANSFORM_DIVISOR = 200000;
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
        HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::RSS, "INIT_FAULT", HiviewDFX::HiSysEvent::EventType::FAULT,
                        "COMPONENT_NAME", "MAIN",
                        "ERR_TYPE", "plugin failure",
                        "ERR_MSG", "ResSchedUtils dlopen " + RES_SCHED_SERVICE_SO + " failed!");
        return;
    }

    reportFunc_ = reinterpret_cast<ReportDataFunc>(dlsym(handle, "ReportDataInProcess"));
    if (!reportFunc_) {
        CGS_LOGW("%{public}s load function:ReportDataInProcess failed!", __func__);
        HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::RSS, "INIT_FAULT", HiviewDFX::HiSysEvent::EventType::FAULT,
                        "COMPONENT_NAME", "MAIN",
                        "ERR_TYPE", "plugin failure",
                        "ERR_MSG", "ResSchedUtils dlsym 'ReportDataInProcess' in " +
						RES_SCHED_SERVICE_SO + " failed!");
        dlclose(handle);
        return;
    }

    reportAppStateFunc_ = reinterpret_cast<ReportAppStateFunc>(dlsym(handle, "ReportAppStateInProcess"));
    if (!reportAppStateFunc_) {
        CGS_LOGW("%{public}s load function:ReportAppStateInProcess failed! Due to %{public}s", __func__, dlerror());
        HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::RSS, "INIT_FAULT", HiviewDFX::HiSysEvent::EventType::FAULT,
                        "COMPONENT_NAME", RES_SCHED_SERVICE_SO,
                        "ERR_TYPE", "plugin failure",
                        "ERR_MSG", "ResSchedUtils don't found dlsym " + RES_SCHED_SERVICE_SO + "!");
    }
}

void ResSchedUtils::LoadUtilsExtra()
{
    auto handle = dlopen(RES_SCHED_CG_EXT_SO.c_str(), RTLD_NOW);
    if (!handle) {
        CGS_LOGD("%{public}s load %{public}s failed! errno:%{public}d", __func__, RES_SCHED_CG_EXT_SO.c_str(), errno);
        HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::RSS, "INIT_FAULT", HiviewDFX::HiSysEvent::EventType::FAULT,
                        "COMPONENT_NAME", "MAIN",
                        "ERR_TYPE", "plugin failure",
                        "ERR_MSG", "ResSchedUtils dlopen " + RES_SCHED_CG_EXT_SO + " failed!");
        return;
    }

    reportArbitrationResultFunc_ =
        reinterpret_cast<ReportArbitrationResultFunc>(dlsym(handle, "ReportArbitrationResult"));
    if (!reportArbitrationResultFunc_) {
        CGS_LOGD("%{public}s load function:ReportArbitrationResult failed! errno:%{public}d", __func__, errno);
        HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::RSS, "INIT_FAULT", HiviewDFX::HiSysEvent::EventType::FAULT,
                        "COMPONENT_NAME", "MAIN",
                        "ERR_TYPE", "plugin failure",
                        "ERR_MSG", "ResSchedUtils dlsym 'ReportArbitrationResult' in " + RES_SCHED_CG_EXT_SO + "!");
        dlclose(handle);
        return;
    }

    dispatchResourceExtFunc_ =
        reinterpret_cast<DispatchResourceExtFunc>(dlsym(handle, "DispatchResourceExt"));
    if (!dispatchResourceExtFunc_) {
        CGS_LOGD("%{public}s load function:DispatchResourceExt failed! errno:%{public}d", __func__, errno);
        HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::RSS, "INIT_FAULT", HiviewDFX::HiSysEvent::EventType::FAULT,
                        "COMPONENT_NAME", "MAIN",
                        "ERR_TYPE", "plugin failure",
                        "ERR_MSG", "ResSchedUtils dlsym 'DispatchResourceExt' in " + RES_SCHED_CG_EXT_SO + "!");
        dlclose(handle);
        return;
    }

    reportSysEventFunc_ =
        reinterpret_cast<ReportSysEventFunc>(dlsym(handle, "ReportSysEvent"));
    if (!reportSysEventFunc_) {
        CGS_LOGD("%{public}s load function:ReportSysEvent failed! errno:%{public}d", __func__, errno);
        HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::RSS, "INIT_FAULT", HiviewDFX::HiSysEvent::EventType::FAULT,
                        "COMPONENT_NAME", RES_SCHED_SERVICE_SO,
                        "ERR_TYPE", "plugin failure",
                        "ERR_MSG", "ResSchedUtils don't found ReportSysEvent in " + RES_SCHED_CG_EXT_SO + "!");
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

void ResSchedUtils::ReportSysEvent(Application &app, ProcessRecord &pr, uint32_t resType, int32_t state)
{
    if (!reportSysEventFunc_) {
        CGS_LOGD("%{public}s failed, function nullptr.", __func__);
        return;
    }
    reportSysEventFunc_(app, pr, resType, state);
}

bool ResSchedUtils::CheckTidIsInPid(int32_t pid, int32_t tid)
{
    nlohmann::json payload;
    payload["pid"] = pid;
    payload["tid"] = tid;
    nlohmann::json reply;
    ResourceSchedule::ResSchedExeClient::GetInstance().SendRequestSync(
        ResExeType::RES_TYPE_CGROUP_PROC_TASK_SYNC_EVENT, 0, payload, reply);
    std::string resStr{"res"};
    if (!reply.contains(resStr) || !reply[resStr].is_boolean()) {
        return false;
    }
    return reply[resStr];
}

std::string ResSchedUtils::GetProcessFilePath(int32_t uid, std::string bundleName, int32_t pid)
{
    if (uid < 0 || pid < 0) {
        CGS_LOGE("%{public}s Parameter Error: UID: %{public}d, PID: %{public}d", __func__, uid, pid);
        return "";
    }
    int32_t userId = uid / UID_TRANSFORM_DIVISOR;
    std::string path;
    path.append("/dev/pids/")
        .append(std::to_string(userId))
        .append("/")
        .append(bundleName)
        .append("/app_")
        .append(std::to_string(pid))
        .append("/cgroup.procs");
    char absolutePath[PATH_MAX] = {0};
    if (!realpath(path.c_str(), absolutePath)) {
        CGS_LOGD("%{public}s Get Proc File Path Error.", __func__);
        return "";
    }
    return std::string(absolutePath);
}

void ResSchedUtils::DispatchResourceExt(uint32_t resType, int64_t value, const nlohmann::json& payload)
{
    if (!dispatchResourceExtFunc_) {
        CGS_LOGD("%{public}s failed, function nullptr.", __func__);
        return;
    }
    dispatchResourceExtFunc_(resType, value, payload);
}

void ResSchedUtils::ReportAppStateInProcess(int32_t state, int32_t pid)
{
    if (!reportAppStateFunc_) {
        CGS_LOGD("%{public}s failed, function nullptr.", __func__);
        return;
    }
    reportAppStateFunc_(state, pid);
}
} // namespace ResourceSchedule
} // namespace OHOS