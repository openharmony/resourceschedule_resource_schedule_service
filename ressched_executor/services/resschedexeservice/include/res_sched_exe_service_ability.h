/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef RESSCHED_EXECUTOR_SERVICES_RESSCHEDEXESERVICE_INCLUDE_RES_SCHED_EXE_SERVICE_ABILITY_H
#define RESSCHED_EXECUTOR_SERVICES_RESSCHEDEXESERVICE_INCLUDE_RES_SCHED_EXE_SERVICE_ABILITY_H

#include "singleton.h"
#include "system_ability.h"

#include "res_sched_exe_service.h"

namespace OHOS {
namespace ResourceSchedule {
class ResSchedExeServiceAbility : public SystemAbility,
    public std::enable_shared_from_this<ResSchedExeServiceAbility> {
    DECLARE_SYSTEM_ABILITY(ResSchedExeServiceAbility);
    DECLARE_DELAYED_SINGLETON(ResSchedExeServiceAbility);

public:
    /**
     * @brief Construct a new ResSchedExeServiceAbilityResSchedExeServiceAbility object.
     *
     * @param sysAbilityId SA id
     * @param runOnCreate register sa to samgr time, process start or used
     */
    ResSchedExeServiceAbility(int32_t sysAbilityId, bool runOnCreate) : SystemAbility(sysAbilityId, runOnCreate) {}

private:
    void OnStart() override;
    void OnStop() override;

    sptr<ResSchedExeService> service_;
    DISALLOW_COPY_AND_MOVE(ResSchedExeServiceAbility);
};
} // namespace ResourceSchedule
} // namespace OHOS

#endif // RESSCHED_EXECUTOR_SERVICES_RESSCHEDEXESERVICE_INCLUDE_RES_SCHED_EXE_SERVICE_ABILITY_H
