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

void SetProcessPrioritySync(int32_t pid, ProcessPriority priority)
{
    OH_BackgroundProcessManager_SetProcessPriority(pid,
        static_cast<BackgroundProcessManager_ProcessPriority>(priority.get_value()));
}

void ResetProcessPrioritySync(int32_t pid)
{
    OH_BackgroundProcessManager_ResetProcessPriority(pid);
}

static std::string GetErrCodeMsg(int errorCode)
{
    switch (errorCode) {
        case ERR_BACKGROUND_PROCESS_MANAGER_INVALID_PARAM:
            return "Parameter error. Possible causes: priority is out of range.";
        case ERR_BACKGROUND_PROCESS_MANAGER_PARAMETER_ERROR:
            return "Parameter error. Possible causes: priority is out of range.";
        case ERR_BACKGROUND_PROCESS_MANAGER_PERMISSION_DENIED:
            return "Permission denied.";
        case ERR_BACKGROUND_PROCESS_MANAGER_CAPABILITY_NOT_SUPPORTED:
            return "Capability not supported.";
        case ERR_BACKGROUND_PROCESS_MANAGER_SETUP_ERROR:
            return "This setting is overridden by setting in Task Manager.";
        case ERR_BACKGROUND_PROCESS_MANAGER_SYSTEM_SCHEDULING:
            return "The setting failed due to system scheduling reasons.";
        default:
            return "";
    }
}

void SetPowerSaveModeSync(int32_t pid, PowerSaveMode powerSaveMode)
{
    int ret = OH_BackgroundProcessManager_SetPowerSaveMode(pid,
        static_cast<BackgroundProcessManager_PowerSaveMode>(powerSaveMode.get_value()));
    if (ret != ERR_BACKGROUND_PROCESS_MANAGER_SUCCESS) {
        ::taihe::set_business_error(ret, GetErrCodeMsg(ret));
    }
}

PowerSaveMode GetPowerSaveModeSync(int32_t pid)
{
    BackgroundProcessManager_PowerSaveMode powerSaveMode;
    int ret = OH_BackgroundProcessManager_GetPowerSaveMode(pid, &powerSaveMode);
    if (ret != ERR_BACKGROUND_PROCESS_MANAGER_SUCCESS) {
        PowerSaveMode aniPowerSaveMode(PowerSaveMode::key_t::EFFICIENCY_MODE);
        ::taihe::set_business_error(ret, GetErrCodeMsg(ret));
        return aniPowerSaveMode;
    }
    if (powerSaveMode == DEFAULT_MODE) {
        return PowerSaveMode(PowerSaveMode::key_t::DEFAULT_MODE);
    } else {
        return PowerSaveMode(PowerSaveMode::key_t::EFFICIENCY_MODE);
    }
}

bool IsPowerSaveModeSync(int32_t pid)
{
    constexpr int isPowerSaveModeOk = 1;
    constexpr int isPowerSaveModeNok = 0;
    int ret = OH_BackgroundProcessManager_IsPowerSaveMode(pid);
    if (ret != isPowerSaveModeOk && ret != isPowerSaveModeNok) {
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
TH_EXPORT_CPP_API_getPowerSaveModeSync(GetPowerSaveModeSync);
TH_EXPORT_CPP_API_isPowerSaveModeSync(IsPowerSaveModeSync);
// NOLINTEND