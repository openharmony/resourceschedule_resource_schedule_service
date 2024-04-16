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

#include "res_sched_exe_service_ability.h"

#include "system_ability_definition.h"

#include "res_sched_exe_log.h"
#include "res_sched_exe_mgr.h"
#include "res_sched_exe_service.h"

namespace OHOS {
namespace ResourceSchedule {
const bool REGISTER_RESULT =
    SystemAbility::MakeAndRegisterAbility(DelayedSingleton<ResSchedExeServiceAbility>::GetInstance().get());

ResSchedExeServiceAbility::ResSchedExeServiceAbility() : SystemAbility(RES_SCHED_EXE_ABILITY_ID, true)
{
}

ResSchedExeServiceAbility::~ResSchedExeServiceAbility()
{
}

void ResSchedExeServiceAbility::OnStart()
{
    ResSchedExeMgr::GetInstance().Init();
    if (!service_) {
        service_ = new (std::nothrow) ResSchedExeService();
    }
    if (service_ == nullptr) {
        RSSEXE_LOGE("ResSchedExeServiceAbility::New ResSchedExeService failed.");
    }
    if (!Publish(service_)) {
        RSSEXE_LOGE("ResSchedExeServiceAbility::Register service failed.");
    }
    RSSEXE_LOGI("ResSchedExeServiceAbility::OnStart.");
}

void ResSchedExeServiceAbility::OnStop()
{
    ResSchedExeMgr::GetInstance().Stop();
    RSSEXE_LOGI("ResSchedExeServiceAbility::OnStop!");
}
} // namespace ResourceSchedule
} // namespace OHOS

