/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: Definition of resource schedule service ability.
 */

#ifndef FOUNDATION_RESOURCESCHEDULE_SERVICES_RESSCHEDSERVICE_INCLUDE_RES_SCHED_SERVICE_ABILITY_H
#define FOUNDATION_RESOURCESCHEDULE_SERVICES_RESSCHEDSERVICE_INCLUDE_RES_SCHED_SERVICE_ABILITY_H

#include "system_ability.h"

namespace OHOS {
namespace ResourceSchedule {

class ResSchedServiceAbility : public SystemAbility {
    DECLARE_SYSTEM_ABILITY(ResSchedServiceAbility);

public:
    ResSchedServiceAbility(int32_t sysAbilityId, bool runOnCreate) : SystemAbility(sysAbilityId, runOnCreate) {}
    ~ResSchedServiceAbility() override = default;

private:
    void OnStart() override;

    void OnStop() override;

    DISALLOW_COPY_AND_MOVE(ResSchedServiceAbility);
};

} // namespace ResourceSchedule
} // namespace OHOS

#endif // FOUNDATION_RESOURCESCHEDULE_SERVICES_RESSCHEDSERVICE_INCLUDE_RES_SCHED_SERVICE_ABILITY_H