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

#ifndef RESSCHED_SERVICES_RESSCHEDSERVICE_INCLUDE_RES_SCHED_SERVICE_ABILITY_H
#define RESSCHED_SERVICES_RESSCHEDSERVICE_INCLUDE_RES_SCHED_SERVICE_ABILITY_H

#include "system_ability.h"
#include "res_sched_service.h"
#include "singleton.h"

namespace OHOS {
namespace ResourceSchedule {
class ResSchedServiceAbility : public SystemAbility,
    public std::enable_shared_from_this<ResSchedServiceAbility> {
    DECLARE_SYSTEM_ABILITY(ResSchedServiceAbility);
    DECLARE_DELAYED_SINGLETON(ResSchedServiceAbility);
public:
    ResSchedServiceAbility(int32_t sysAbilityId, bool runOnCreate) : SystemAbility(sysAbilityId, runOnCreate) {}

private:
    void OnStart() override;

    void OnStop() override;

    void OnAddSystemAbility(int32_t systemAbilityId, const std::string& deviceId) override;

    void OnRemoveSystemAbility(int32_t systemAbilityId, const std::string& deviceId) override;

    sptr<ResSchedService> service_;

    DISALLOW_COPY_AND_MOVE(ResSchedServiceAbility);
};
} // namespace ResourceSchedule
} // namespace OHOS

#endif // RESSCHED_SERVICES_RESSCHEDSERVICE_INCLUDE_RES_SCHED_SERVICE_ABILITY_H
