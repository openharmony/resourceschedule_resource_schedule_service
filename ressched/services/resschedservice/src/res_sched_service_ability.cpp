/*
 * Copyright (c) 2022-2026 Huawei Device Co., Ltd.
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

#include <sstream>
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
    int64_t initStartTime = ResCommonUtil::GetNowMillTime(true);
    ResSchedMgr::GetInstance().Init();
    NotifierMgr::GetInstance().Init();
    EventListenerMgr::GetInstance().Init();
    if (!service_) {
        service_ = new (std::nothrow) ResSchedService();
    }
    if (service_ == nullptr) {
        RESSCHED_LOGE("ResSchedServiceAbility:: New ResSchedService failed.");
        HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::RSS, "INIT_FAULT", HiviewDFX::HiSysEvent::EventType::FAULT,
                        "MODULE_NAME", "ResSchedService",
                        "SCENE_NAME", "ServiceCreateFailed",
                        "ERR_INFO", "New ResSchedService object failed!");
    }
    if (service_) {
        service_->InitAllowIpcReportRes();
    }
    if (!Publish(service_)) {
        RESSCHED_LOGE("ResSchedServiceAbility:: Register service failed.");
        HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::RSS, "INIT_FAULT", HiviewDFX::HiSysEvent::EventType::FAULT,
                        "MODULE_NAME", "ResSchedService",
                        "SCENE_NAME", "ServicePublishFailed",
                        "ERR_INFO", "Publish ResSchedService failed.");
    }
    SystemAbilityListenerInit();
    EventControllerInit();
    ObserverManagerInit();
    ReclaimProcessMemory();
    int64_t initDuration = ResCommonUtil::GetNowMillTime(true) - initStartTime;
    HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::RSS, "SA_START_TIME", HiviewDFX::HiSysEvent::EventType::STATISTIC,
                    "SAID", RES_SCHED_SYS_ABILITY_ID, "TOTAL_TIME", initDuration);
    RESSCHED_LOGI("ResSchedServiceAbility ::OnStart cost %{public}lld", (long long)initDuration);
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
    std::map<int, std::string> saList = {
        {APP_MGR_SERVICE_ID, "APP_MGR_SERVICE_ID"},
        {WINDOW_MANAGER_SERVICE_ID, "WINDOW_MANAGER_SERVICE_ID"},
        {BACKGROUND_TASK_MANAGER_SERVICE_ID, "BACKGROUND_TASK_MANAGER_SERVICE_ID"},
        {RES_SCHED_EXE_ABILITY_ID, "RES_SCHED_EXE_ABILITY_ID"},
        {POWER_MANAGER_SERVICE_ID, "POWER_MANAGER_SERVICE_ID"},
        {GAME_SERVICE_SERVICE_ID, "GAME_SERVICE_SERVICE_ID"},
        {WIFI_DEVICE_SYS_ABILITY_ID, "WIFI_DEVICE_SYS_ABILITY_ID"},
        {MSDP_USER_STATUS_SERVICE_ID, "MSDP_USER_STATUS_SERVICE_ID"},
        {BUNDLE_MGR_SERVICE_SYS_ABILITY_ID, "BUNDLE_MGR_SERVICE_SYS_ABILITY_ID"},
        {SOC_PERF_SERVICE_SA_ID, "SOC_PERF_SERVICE_SA_ID"},
        {MEMORY_MANAGER_SA_ID, "MEMORY_MANAGER_SA_ID"},
        {LOCATION_LOCATOR_SA_ID, "LOCATION_LOCATOR_SA_ID"},
        {ADVANCED_NOTIFICATION_SERVICE_ABILITY_ID, "ADVANCED_NOTIFICATION_SERVICE_ABILITY_ID"},
    };
    std::vector<std::string> errIds;
    for (const auto& [saId, saIdStr] : saList) {
        if (!AddSystemAbilityListener(saId)) {
            errIds.push_back(saIdStr);
            RESSCHED_LOGE("AddSystemAbilityListener failed saId:%{public}d|%{public}s", saId, saIdStr.c_str());
        }
    }
    ReportSAListenerResult(errIds);
}

bool ResSchedServiceAbility::ReportSAListenerResult(const std::vector<std::string>& errIds)
{
    if (errIds.empty()) {
        RESSCHED_LOGI("Init SystemAbilityListener finish");
        return true;
    }
    std::ostringstream oss;
    oss << "Register SA listener failed : ";
    for (const auto& errId : errIds) {
        oss << errId << " ";
    }
    HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::RSS, "INIT_FAULT", HiviewDFX::HiSysEvent::EventType::FAULT,
                    "MODULE_NAME", "ResSchedService",
                    "SCENE_NAME", "SaListenerFailed",
                    "ERR_INFO", oss.str());
    RESSCHED_LOGE("Init SystemAbilityListener finish with some errors!");
    return false;
}
} // namespace ResourceSchedule
} // namespace OHOS

