/*
* Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "background_process_manager.h"
#include "res_sched_log.h"
#include "res_sched_client.h"

namespace OHOS {
namespace ResourceSchedule {

namespace {
    constexpr int SET_PRIORITY = 1;
    constexpr int RESET_PRIORITY = 0;
    const char* RES_APP_NAP_CLIENT_SO = "libapp_nap_client.z.so";
}
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Set the priority of process.
 *
 * @param pid Indicates the pid of the process to be set.
 * @param priority Indicates the priority to be set.
 *         Specific priority can be referenced {@link BackgroundProcessManager_ProcessPriority}.
 * @return {@link ERR_BACKGROUND_PROCESS_MANAGER_SUCCESS} 0 - Success.
 *         {@link ERR_BACKGROUND_PROCESS_MANAGER_INVALID_PARAM} 401 - Parameter error.
 * @since 17
 */
int OH_BackgroundProcessManager_SetProcessPriority(int pid, BackgroundProcessManager_ProcessPriority priority)
{
    if (priority != PROCESS_BACKGROUND && priority != PROCESS_INACTIVE) {
        RESSCHED_LOGE("priority is invalid.");
        return ERR_BACKGROUND_PROCESS_MANAGER_INVALID_PARAM;
    }
    RESSCHED_LOGI("Set process priority, pid:%{public}d priority:%{public}d", pid, priority);
    nlohmann::json payload;
    payload["PID"] = std::to_string(pid);
    payload["PRIORITY"] = std::to_string(priority);
    ResSchedClient::GetInstance().ReportData(ResType::RES_TYPE_SET_BACKGROUND_PROCESS_PRIORITY, SET_PRIORITY, payload);
    return ERR_BACKGROUND_PROCESS_MANAGER_SUCCESS;
}

/**
 * @brief Reset the priority of process.
 *
 * @param pid Indicates the pid of the process to be reset.
 * @return {@link ERR_BACKGROUND_PROCESS_MANAGER_SUCCESS} 0 - Success.
 * @since 17
 */
int OH_BackgroundProcessManager_ResetProcessPriority(int pid)
{
    RESSCHED_LOGI("Reset process priority, pid:%{public}d", pid);
    nlohmann::json payload;
    payload["PID"] = std::to_string(pid);
    ResSchedClient::GetInstance().ReportData(ResType::RES_TYPE_SET_BACKGROUND_PROCESS_PRIORITY,
        RESET_PRIORITY, payload);
    return ERR_BACKGROUND_PROCESS_MANAGER_SUCCESS;
}

/**
 * @brief Set the power saving mode of process. The setting may fail due to user setting
 * reasons or system scheduling reasons.
 * @param pid Indicates the pid of the power saving mode to be set.
 * @param powerSaveMode Indicates the power saving mode that needs to be set.
 * For details, please refer to BackgroundProcessManager_PowerSaveMode
 * @return {@link ERR_BACKGROUND_PROCESS_MANAGER_SUCCESS} 0 - Success.
 * @return {@link ERR_BACKGROUND_PROCESS_MANAGER_PERMISSION_DENIED} 201 - Permission denied.
 * @return {@link ERR_BACKGROUND_PROCESS_MANAGER_CAPABILITY_NOT_SUPPORTED} 801 - Capability not supported.
 * @return {@link ERR_BACKGROUND_PROCESS_MANAGER_PARAMETER_ERROR} 31800002 - Parameter error.
 * @return {@link ERR_BACKGROUND_PROCESS_MANAGER_SETUP_ERROR} 31800003 - Setup error.
 * @since 20
 */
int OH_BackgroundProcessManager_SetPowerSaveMode(int pid, BackgroundProcessManager_PowerSaveMode powerSaveMode)
{
    typedef int (*SetPowerSaveMode)(const int64_t, const int32_t);
    auto handle = dlopen(RES_APP_NAP_CLIENT_SO, RTLD_NOW);
    if (!handle) {
        RESSCHED_LOGE("Dlopen failed.");
        return ERR_BACKGROUND_PROCESS_MANAGER_CAPABILITY_NOT_SUPPORTED;
    }

    SetPowerSaveMode func = reinterpret_cast<SetPowerSaveMode>(dlsym(handle, "SetPowerSaveMode"));
    if (!func) {
        RESSCHED_LOGE("Dlsym SetPowerSaveMode failed.");
        dlclose(handle);
        return ERR_BACKGROUND_PROCESS_MANAGER_CAPABILITY_NOT_SUPPORTED;
    }
    int ret = func(pid, static_cast<int32_t>(powerSaveMode));
    dlclose(handle);
    return ret;
}

/**
 * @brief Check is the process is in power saving mode.
 *
 * @param pid Indicates the process to be checked is the pid of the power saving mode.
 * @return {@link ERR_BACKGROUND_PROCESS_MANAGER_SUCCESS} 0 - Success.
 * @return {@link ERR_BACKGROUND_PROCESS_MANAGER_PERMISSION_DENIED} 201 - Permission denied.
 * @return {@link ERR_BACKGROUND_PROCESS_MANAGER_CAPABILITY_NOT_SUPPORTED} 801 - Capability not supported.
 * @return {@link ERR_BACKGROUND_PROCESS_MANAGER_PARAMETER_ERROR} 31800002 - Parameter error.
 * @since 20
 */
int OH_BackgroundProcessManager_IsPowerSaveMode(int pid)
{
    typedef int (*IsPowerSaveMode)(const int64_t, bool&);
    auto handle = dlopen(RES_APP_NAP_CLIENT_SO, RTLD_NOW);
    if (!handle) {
        RESSCHED_LOGE("Dlopen failed.");
        return ERR_BACKGROUND_PROCESS_MANAGER_CAPABILITY_NOT_SUPPORTED;
    }

    IsPowerSaveMode func = reinterpret_cast<IsPowerSaveMode>(dlsym(handle, "IsPowerSaveMode"));
    if (!func) {
        RESSCHED_LOGE("Dlsym IsPowerSaveMode failed.");
        dlclose(handle);
        return ERR_BACKGROUND_PROCESS_MANAGER_CAPABILITY_NOT_SUPPORTED;
    }
    bool result = false;
    int ret = func(pid, result);
    dlclose(handle);
    if (ret == 0) {
        return static_cast<int>(result);
    }
    return ret;
}
#ifdef __cplusplus
}
#endif
}
}