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
}   // namespace

// Since these macros are auto-generate, lint will cause false positive.
// NOLINTBEGIN
TH_EXPORT_CPP_API_setProcessPrioritySync(SetProcessPrioritySync);
TH_EXPORT_CPP_API_resetProcessPrioritySync(ResetProcessPrioritySync);
// NOLINTEND