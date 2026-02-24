/*
 * Copyright (c) 2022-2025 Huawei Device Co., Ltd.
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

#include "res_common_util.h"
#include "ffrt_inner.h"
#include "hisysevent.h"
#include "notifier_mgr.h"
#include "observer_manager_intf.h"
#include "res_sched_log.h"
#include "res_sched_mgr.h"
#include "res_sched_service.h"
#include "event_controller_intf.h"
#include "event_listener_mgr.h"
#include "system_ability_definition.h"

#define GAME_SERVICE_SERVICE_ID 66058

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
    EventListenerMgr::GetInstance().Init();
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
    if (service_) {
        service_->InitAllowIpcReportRes();
    }
    if (!Publish(service_)) {
        RESSCHED_LOGE("ResSchedServiceAbility:: Register service failed.");
        HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::RSS, "INIT_FAULT", HiviewDFX::HiSysEvent::EventType::FAULT,
                        "COMPONENT_NAME", "MAIN",
                        "ERR_TYPE", "register failure",
                        "ERR_MSG", "Register ResSchedService failed.");
    }
    SystemAbilityListenerInit();
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
    RemoveSystemAbilityListener(SOC_PERF_SERVICE_SA_ID);
    ResSchedMgr::GetInstance().Stop();
    RESSCHED_LOGI("ResSchedServiceAbility::OnStop!");
}

void ResSchedServiceAbility::OnAddSystemAbility(int32_t systemAbilityId, const std::string& deviceId)
{
    if (systemAbilityId == RES_SCHED_EXE_ABILITY_ID) {
        ResSchedMgr::GetInstance().InitExecutorPlugin();
    } else if (systemAbilityId == APP_MGR_SERVICE_ID) {
        ResSchedMgr::GetInstance().InitForegroundAppInfo();
    }
    nlohmann::json payload;
    payload["saId"] = systemAbilityId;
    payload["deviceId"] = deviceId;
    ResSchedMgr::GetInstance().ReportData(ResType::RES_TYPE_SYSTEM_ABILITY_STATUS_CHANGE,
        ResType::SystemAbilityStatus::SA_ADD, payload);
    RESSCHED_LOGI("OnAddSystemAbility systemAbilityId:%{public}d", systemAbilityId);
}

void ResSchedServiceAbility::OnRemoveSystemAbility(int32_t systemAbilityId, const std::string& deviceId)
{
    nlohmann::json payload;
    payload["saId"] = systemAbilityId;
    payload["deviceId"] = deviceId;
    ResSchedMgr::GetInstance().ReportData(ResType::RES_TYPE_SYSTEM_ABILITY_STATUS_CHANGE,
        ResType::SystemAbilityStatus::SA_REMOVE, payload);
    RESSCHED_LOGI("OnRemoveSystemAbility systemAbilityId:%{public}d", systemAbilityId);
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

void ResSchedServiceAbility::SystemAbilityListenerInit()
{
    if (!AddSystemAbilityListener(APP_MGR_SERVICE_ID)) {
        HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::RSS, "INIT_FAULT", HiviewDFX::HiSysEvent::EventType::FAULT,
                        "COMPONENT_NAME", "MAIN",
                        "ERR_TYPE", "register failure",
                        "ERR_MSG", "Register a listener of app manager service failed.");
        RESSCHED_LOGI("AddSystemAbilityListener failed saId:%{public}d", APP_MGR_SERVICE_ID);
    }
    if (!AddSystemAbilityListener(WINDOW_MANAGER_SERVICE_ID)) {
        HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::RSS, "INIT_FAULT", HiviewDFX::HiSysEvent::EventType::FAULT,
                        "COMPONENT_NAME", "MAIN",
                        "ERR_TYPE", "register failure",
                        "ERR_MSG", "Register a listener of window manager service failed.");
        RESSCHED_LOGI("AddSystemAbilityListener failed saId:%{public}d", WINDOW_MANAGER_SERVICE_ID);
    }
    if (!AddSystemAbilityListener(BACKGROUND_TASK_MANAGER_SERVICE_ID)) {
        HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::RSS, "INIT_FAULT", HiviewDFX::HiSysEvent::EventType::FAULT,
                        "COMPONENT_NAME", "MAIN",
                        "ERR_TYPE", "register failure",
                        "ERR_MSG", "Register a listener of background task manager service failed.");
        RESSCHED_LOGI("AddSystemAbilityListener failed saId:%{public}d", BACKGROUND_TASK_MANAGER_SERVICE_ID);
    }
    if (!AddSystemAbilityListener(RES_SCHED_EXE_ABILITY_ID)) {
        HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::RSS, "INIT_FAULT", HiviewDFX::HiSysEvent::EventType::FAULT,
                        "COMPONENT_NAME", "MAIN",
                        "ERR_TYPE", "register failure",
                        "ERR_MSG", "Register a listener of background task manager service failed.");
        RESSCHED_LOGI("AddSystemAbilityListener failed saId:%{public}d", RES_SCHED_EXE_ABILITY_ID);
    }
    if (!AddSystemAbilityListener(POWER_MANAGER_SERVICE_ID)) {
        HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::RSS, "INIT_FAULT", HiviewDFX::HiSysEvent::EventType::FAULT,
                        "COMPONENT_NAME", "MAIN",
                        "ERR_TYPE", "register failure",
                        "ERR_MSG", "Register a listener of power manager service failed.");
        RESSCHED_LOGI("AddSystemAbilityListener failed saId:%{public}d", POWER_MANAGER_SERVICE_ID);
    }
    if (!AddSystemAbilityListener(GAME_SERVICE_SERVICE_ID)) {
        HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::RSS, "INIT_FAULT", HiviewDFX::HiSysEvent::EventType::FAULT,
                        "COMPONENT_NAME", "MAIN",
                        "ERR_TYPE", "register failure",
                        "ERR_MSG", "Register a listener of power manager service failed.");
        RESSCHED_LOGI("AddSystemAbilityListener failed saId:%{public}d", GAME_SERVICE_SERVICE_ID);
    }
    SystemAbilityListenerInitExtPartOne();
    SystemAbilityListenerInitExtPartTwo();
    RESSCHED_LOGI("Init SystemAbilityListener finish");
}

void ResSchedServiceAbility::SystemAbilityListenerInitExtPartOne()
{
    if (!AddSystemAbilityListener(WIFI_DEVICE_SYS_ABILITY_ID)) {
        HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::RSS, "INIT_FAULT", HiviewDFX::HiSysEvent::EventType::FAULT,
                        "COMPONENT_NAME", "MAIN",
                        "ERR_TYPE", "register failure",
                        "ERR_MSG", "Register a listener of wifi manager service failed.");
        RESSCHED_LOGI("AddSystemAbilityListener failed saId:%{public}d", WIFI_DEVICE_SYS_ABILITY_ID);
    }
    if (!AddSystemAbilityListener(MSDP_USER_STATUS_SERVICE_ID)) {
        HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::RSS, "INIT_FAULT", HiviewDFX::HiSysEvent::EventType::FAULT,
                        "COMPONENT_NAME", "MAIN",
                        "ERR_TYPE", "register failure",
                        "ERR_MSG", "Register a listener of power manager service failed.");
        RESSCHED_LOGI("AddSystemAbilityListener failed saId:%{public}d", MSDP_USER_STATUS_SERVICE_ID);
    }
    if (!AddSystemAbilityListener(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID)) {
        HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::RSS, "INIT_FAULT", HiviewDFX::HiSysEvent::EventType::FAULT,
                        "COMPONENT_NAME", "MAIN",
                        "ERR_TYPE", "register failure",
                        "ERR_MSG", "Register a listener of power manager service failed.");
        RESSCHED_LOGI("AddSystemAbilityListener failed saId:%{public}d", BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
    }
    if (!AddSystemAbilityListener(SOC_PERF_SERVICE_SA_ID)) {
        HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::RSS, "INIT_FAULT", HiviewDFX::HiSysEvent::EventType::FAULT,
                        "COMPONENT_NAME", "MAIN",
                        "ERR_TYPE", "register failure",
                        "ERR_MSG", "Register a listener of power manager service failed.");
        RESSCHED_LOGI("AddSystemAbilityListener failed saId:%{public}d", SOC_PERF_SERVICE_SA_ID);
    }
    if (!AddSystemAbilityListener(MEMORY_MANAGER_SA_ID)) {
        HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::RSS, "INIT_FAULT", HiviewDFX::HiSysEvent::EventType::FAULT,
                        "COMPONENT_NAME", "MAIN",
                        "ERR_TYPE", "register failure",
                        "ERR_MSG", "Register a listener of memory manager service failed.");
        RESSCHED_LOGI("AddSystemAbilityListener failed saId:%{public}d", MEMORY_MANAGER_SA_ID);
    }
}

void ResSchedServiceAbility::SystemAbilityListenerInitExtPartTwo()
{
    if (!AddSystemAbilityListener(LOCATION_LOCATOR_SA_ID)) {
        HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::RSS, "INIT_FAULT", HiviewDFX::HiSysEvent::EventType::FAULT,
                        "COMPONENT_NAME", "MAIN",
                        "ERR_TYPE", "register failure",
                        "ERR_MSG", "Register a listener of location locator failed.");
        RESSCHED_LOGI("AddSystemAbilityListener failed saId:%{public}d", LOCATION_LOCATOR_SA_ID);
    }
    if (!AddSystemAbilityListener(ADVANCED_NOTIFICATION_SERVICE_ABILITY_ID)) {
        HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::RSS, "INIT_FAULT", HiviewDFX::HiSysEvent::EventType::FAULT,
                        "COMPONENT_NAME", "MAIN",
                        "ERR_TYPE", "register failure",
                        "ERR_MSG", "Register a listener of advanced notification service failed.");
        RESSCHED_LOGI("AddSystemAbilityListener failed saId:%{public}d", ADVANCED_NOTIFICATION_SERVICE_ABILITY_ID);
    }
}
} // namespace ResourceSchedule
} // namespace OHOS

