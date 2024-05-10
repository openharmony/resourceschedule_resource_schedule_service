/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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

#include "res_common_util.h"
#include "ffrt_inner.h"
#include "hisysevent.h"
#include "notifier_mgr.h"
#include "res_sched_service_ability.h"
#include "observer_manager_intf.h"
#include "res_sched_log.h"
#include "res_sched_mgr.h"
#include "res_sched_service.h"
#include "event_controller_intf.h"
#include "system_ability_definition.h"
#include "cgroup_sched.h"

namespace OHOS {
namespace ResourceSchedule {
const bool REGISTER_RESULT =
    SystemAbility::MakeAndRegisterAbility(DelayedSingleton<ResSchedServiceAbility>::GetInstance().get());

ResSchedServiceAbility::ResSchedServiceAbility() : SystemAbility(RES_SCHED_SYS_ABILITY_ID, true)
{
}

ResSchedServiceAbility::~ResSchedServiceAbility()
{
}

void ResSchedServiceAbility::OnStart()
{
    ResSchedMgr::GetInstance().Init();
    NotifierMgr::GetInstance().Init();
    if (!service_) {
        service_ = new (std::nothrow) ResSchedService();
    }
    if (service_ == nullptr) {
        RESSCHED_LOGE("ResSchedServiceAbility:: New ResSchedService failed.");
        HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::RSS, "INIT_FAULT", HiviewDFX::HiSysEvent::EventType::FAULT,
                        "COMPONENT_NAME", "MAIN",
                        "ERR_TYPE", "others",
                        "ERR_MSG", "New ResSchedService object failed!");
    }
    if (!Publish(service_)) {
        RESSCHED_LOGE("ResSchedServiceAbility:: Register service failed.");
        HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::RSS, "INIT_FAULT", HiviewDFX::HiSysEvent::EventType::FAULT,
                        "COMPONENT_NAME", "MAIN",
                        "ERR_TYPE", "register failure",
                        "ERR_MSG", "Register ResSchedService failed.");
    }
    CgroupSchedInit();
    if (!AddSystemAbilityListener(APP_MGR_SERVICE_ID)) {
        HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::RSS, "INIT_FAULT", HiviewDFX::HiSysEvent::EventType::FAULT,
                        "COMPONENT_NAME", "MAIN",
                        "ERR_TYPE", "register failure",
                        "ERR_MSG", "Register a listener of app manager service failed.");
    }
    if (!AddSystemAbilityListener(WINDOW_MANAGER_SERVICE_ID)) {
        HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::RSS, "INIT_FAULT", HiviewDFX::HiSysEvent::EventType::FAULT,
                        "COMPONENT_NAME", "MAIN",
                        "ERR_TYPE", "register failure",
                        "ERR_MSG", "Register a listener of window manager service failed.");
    }
    if (!AddSystemAbilityListener(BACKGROUND_TASK_MANAGER_SERVICE_ID)) {
        HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::RSS, "INIT_FAULT", HiviewDFX::HiSysEvent::EventType::FAULT,
                        "COMPONENT_NAME", "MAIN",
                        "ERR_TYPE", "register failure",
                        "ERR_MSG", "Register a listener of background task manager service failed.");
    }
    if (!AddSystemAbilityListener(POWER_MANAGER_SERVICE_ID)) {
        HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::RSS, "INIT_FAULT", HiviewDFX::HiSysEvent::EventType::FAULT,
                        "COMPONENT_NAME", "MAIN",
                        "ERR_TYPE", "register failure",
                        "ERR_MSG", "Register a listener of power manager service failed.");
    }
    EventControllerInit();
    ObserverManagerInit();
    ReclaimProcessMemory();
    RESSCHED_LOGI("ResSchedServiceAbility ::OnStart.");
}

void ResSchedServiceAbility::OnStop()
{
    ObserverManagerDisable();
    EventControllerStop();
    RemoveSystemAbilityListener(BACKGROUND_TASK_MANAGER_SERVICE_ID);
    RemoveSystemAbilityListener(WINDOW_MANAGER_SERVICE_ID);
    RemoveSystemAbilityListener(APP_MGR_SERVICE_ID);
    RemoveSystemAbilityListener(POWER_MANAGER_SERVICE_ID);
    ResSchedMgr::GetInstance().Stop();
    CgroupSchedDeinit();
    RESSCHED_LOGI("ResSchedServiceAbility::OnStop!");
}

void ResSchedServiceAbility::OnAddSystemAbility(int32_t systemAbilityId, const std::string& deviceId)
{
    ReportAbilityStatus(systemAbilityId, deviceId, 1);
}

void ResSchedServiceAbility::OnRemoveSystemAbility(int32_t systemAbilityId, const std::string& deviceId)
{
    ReportAbilityStatus(systemAbilityId, deviceId, 0);
}

void ResSchedServiceAbility::OnDeviceLevelChanged(int32_t type, int32_t level, std::string& action)
{
    if (service_ == nullptr) {
        RESSCHED_LOGE("On Device Level Changed failed due to service nullptr!");
        return;
    }
    service_->OnDeviceLevelChanged(type, level);
}

void ResSchedServiceAbility::ReclaimProcessMemory()
{
    const int32_t delayTime = 60 * 1000 * 1000;
    ffrt::task_attr taskattr;
    taskattr.delay(delayTime);
    ffrt::submit([]() {ResCommonUtil::WriteFileReclaim(getpid());}, {}, {}, {taskattr});
}
} // namespace ResourceSchedule
} // namespace OHOS

