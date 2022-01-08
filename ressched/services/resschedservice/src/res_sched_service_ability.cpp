/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: Resource schedule service ability.
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