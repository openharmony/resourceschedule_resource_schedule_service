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
 *         {@link ERR_BACKGROUND_PROCESS_MANAGER_REMOTE_ERROR} 31800001 - Remote error.
 * @since 15
 */
int OH_BackgroundProcessManager_SetProcessPriority(int pid, BackgroundProcessManager_ProcessPriority priority)
{
    if (pid < 0) {
        RESSCHED_LOGE("the process to be SetProcessPriority is invalid.");
        return ERR_BACKGROUND_PROCESS_MANAGER_INVALID_PARAM;
    }

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
 *         {@link ERR_BACKGROUND_PROCESS_MANAGER_REMOTE_ERROR} 31800001 - Remote error.
 * @since 15
 */
int OH_BackgroundProcessManager_ResetProcessPriority(int pid)
{
    if (pid < 0) {
        RESSCHED_LOGE("the process to be SetProcessPriority is invalid.");
        return ERR_BACKGROUND_PROCESS_MANAGER_INVALID_PARAM;
    }
    RESSCHED_LOGI("Reset process priority, pid:%{public}d", pid);
    nlohmann::json payload;
    payload["PID"] = std::to_string(pid);
    ResSchedClient::GetInstance().ReportData(ResType::RES_TYPE_SET_BACKGROUND_PROCESS_PRIORITY,
        RESET_PRIORITY, payload);
    return ERR_BACKGROUND_PROCESS_MANAGER_SUCCESS;
}

#ifdef __cplusplus
}
#endif
}
}