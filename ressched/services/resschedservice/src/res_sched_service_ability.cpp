/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "res_sched_service_ability.h"
#include "res_sched_log.h"
#include "res_sched_mgr.h"
#include "system_ability_definition.h"

namespace OHOS {
namespace ResourceSchedule {

REGISTER_SYSTEM_ABILITY_BY_ID(ResSchedServiceAbility, RES_SCHED_SYS_ABILITY_ID, true);

void ResSchedServiceAbility::OnStart()
{
    ResSchedMgr::GetInstance().Init();
    RESSCHED_LOGI("ResSchedServiceAbility ::OnStart.");
}

void ResSchedServiceAbility::OnStop()
{
    ResSchedMgr::GetInstance().Stop();
    RESSCHED_LOGI("ResSchedServiceAbility::OnStop!");
}

} // namespace ResourceSchedule
} // namespace OHOS