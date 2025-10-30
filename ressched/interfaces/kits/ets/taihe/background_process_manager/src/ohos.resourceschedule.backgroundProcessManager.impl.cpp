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

#include "ohos.resourceschedule.backgroundProcessManager.proj.hpp"
#include "ohos.resourceschedule.backgroundProcessManager.impl.hpp"
#include "taihe/runtime.hpp"
#include "stdexcept"
#include "background_process_manager.h"
#include "res_sched_log.h"

using namespace OHOS;
using namespace OHOS::ResourceSchedule;
using namespace ohos::resourceschedule::backgroundProcessManager;

namespace {
std::string GetErrCodeMsg(const int32_t errorCode)
{
    switch (errorCode) {
        case BackgroundProcessManager_ErrorCode::ERR_BACKGROUND_PROCESS_MANAGER_PERMISSION_DENIED:
            return "Permission denied.";
        case BackgroundProcessManager_ErrorCode::ERR_BACKGROUND_PROCESS_MANAGER_INVALID_PARAM:
            return "Parameter error. Possible causes: priority is out of range.";
        case BackgroundProcessManager_ErrorCode::ERR_BACKGROUND_PROCESS_MANAGER_CAPABILITY_NOT_SUPPORTED:
            return "Capability not supported.";
        case BackgroundProcessManager_ErrorCode::ERR_BACKGROUND_PROCESS_MANAGER_PARAMETER_ERROR:
            return "Parameter error. Possible causes: priority is out of range.";
        case BackgroundProcessManager_ErrorCode::ERR_BACKGROUND_PROCESS_MANAGER_SETUP_ERROR:
            return "This setting is overridden by setting in Task Manager.";
        case BackgroundProcessManager_ErrorCode::ERR_BACKGROUND_PROCESS_MANAGER_SYSTEM_SCHEDULING:
            return "The setting failed due to system scheduling reasons.";
        default:
            return "Inner error";
    }
}

void SetProcessPrioritySync(int32_t pid, ProcessPriority priority)
{
    OH_BackgroundProcessManager_SetProcessPriority(pid,
        static_cast<BackgroundProcessManager_ProcessPriority>(priority.get_value()));
}

void ResetProcessPrioritySync(int32_t pid)
{
    OH_BackgroundProcessManager_ResetProcessPriority(pid);
}

void SetPowerSaveModeSync(int32_t pid, PowerSaveMode powerSaveMode)
{
    BackgroundProcessManager_PowerSaveMode processMode =
        static_cast<BackgroundProcessManager_PowerSaveMode>(powerSaveMode.get_value());
    int ret = OH_BackgroundProcessManager_SetPowerSaveMode(pid, processMode);
    if (ret != 0) {
        ::taihe::set_business_error(ret, GetErrCodeMsg(ret));
    }
}

bool IsPowerSaveModeSync(int32_t pid)
{
    constexpr int isPowerSaveOk = 1;
    constexpr int isPowerSaveNOk = 0;
    int ret = OH_BackgroundProcessManager_IsPowerSaveMode(pid);
    if (ret != isPowerSaveNOk && ret != isPowerSaveOk) {
        ::taihe::set_business_error(ret, GetErrCodeMsg(ret));
        return false;
    }
    return static_cast<bool>(ret);
}
}   // namespace

// Since these macros are auto-generate, lint will cause false positive.
// NOLINTBEGIN
TH_EXPORT_CPP_API_setProcessPrioritySync(SetProcessPrioritySync);
TH_EXPORT_CPP_API_resetProcessPrioritySync(ResetProcessPrioritySync);
TH_EXPORT_CPP_API_setPowerSaveModeSync(SetPowerSaveModeSync);
TH_EXPORT_CPP_API_isPowerSaveModeSync(IsPowerSaveModeSync);
// NOLINTEND