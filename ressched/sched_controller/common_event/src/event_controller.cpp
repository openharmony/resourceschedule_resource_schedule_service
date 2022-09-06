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

#include "event_controller.h"

#include "application_info.h"
#include "bundle_constants.h"
#include "bundle_mgr_interface.h"
#include "common_event_manager.h"
#include "common_event_support.h"
#include "if_system_ability_manager.h"
#include "ipc_skeleton.h"
#include "iservice_registry.h"
#include "net_supplier_info.h"
#include "system_ability_definition.h"

#include "ressched_utils.h"
#include "res_sched_log.h"
#include "res_type.h"

using namespace OHOS::EventFwk;
namespace OHOS {
namespace ResourceSchedule {
IMPLEMENT_SINGLE_INSTANCE(EventController);

void EventController::Init()
{
    if (sysAbilityListener_ != nullptr) {
        return;
    }
    sysAbilityListener_ = new (std::nothrow) SystemAbilityStatusChangeListener();
    if (sysAbilityListener_ == nullptr) {
        RESSCHED_LOGE("Failed to create statusChangeListener due to no memory.");
        return;
    }
    sptr<ISystemAbilityManager> systemAbilityManager
        = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (systemAbilityManager == nullptr) {
        RESSCHED_LOGE("systemAbilityManager is null");
        sysAbilityListener_ = nullptr;
        return;
    }
    int32_t ret = systemAbilityManager->SubscribeSystemAbility(COMMON_EVENT_SERVICE_ID, sysAbilityListener_);
    if (ret != ERR_OK) {
        RESSCHED_LOGE("subscribe system ability id: %{public}d failed", COMMON_EVENT_SERVICE_ID);
        sysAbilityListener_ = nullptr;
    }
}

void EventController::HandlePkgAddRemove(const EventFwk::Want &want, nlohmann::json &payload) const
{
    AppExecFwk::ElementName ele = want.GetElement();
    std::string bundleName = ele.GetBundleName();
    int32_t uid = want.GetIntParam(AppExecFwk::Constants::UID, -1);
    payload["bundleName"] = bundleName;
    payload["uid"] = uid;
}

int32_t EventController::GetUid(const int32_t &userId, const std::string &bundleName) const
{
    AppExecFwk::ApplicationInfo info;
    sptr<ISystemAbilityManager> systemAbilityManager
        = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (systemAbilityManager == nullptr) {
        RESSCHED_LOGE("Failed to get uid due to get systemAbilityManager is null.");
        return -1;
    }
    sptr<IRemoteObject> remoteObject  = systemAbilityManager->GetSystemAbility(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
    if (remoteObject == nullptr) {
        RESSCHED_LOGE("Failed to get uid due to get BMS is null.");
        return -1;
    }
    sptr<AppExecFwk::IBundleMgr> bundleMgr = iface_cast<AppExecFwk::IBundleMgr>(remoteObject);
    bundleMgr->GetApplicationInfo(bundleName, AppExecFwk::ApplicationFlag::GET_BASIC_APPLICATION_INFO, userId, info);
    return static_cast<int32_t>(info.uid);
}

void EventController::HandleConnectivityChange(
    const EventFwk::Want &want, const int32_t &code, nlohmann::json &payload)
{
    int32_t netType = want.GetIntParam("NetType", -1);
    if (netType != 1) {
        return;
    }
    ReportDataInProcess(ResType::RES_TYPE_WIFI_CONNECT_STATE_CHANGE, code, payload);
}

void EventController::ReportDataInProcess(const uint32_t &resType, const int64_t &value, const nlohmann::json& payload)
{
    ResSchedUtils::GetInstance().ReportDataInProcess(resType, value, payload);
    resType_ = resType;
    value_ = value;
    payload_ = payload;
}

void EventController::Stop()
{
    if (sysAbilityListener_ == nullptr) {
        return;
    }
    sysAbilityListener_->Stop();
}

void EventController::SystemAbilityStatusChangeListener::OnAddSystemAbility(
    int32_t systemAbilityId, const std::string& deviceId)
{
    MatchingSkills matchingSkills;
    matchingSkills.AddEvent(CommonEventSupport::COMMON_EVENT_CONNECTIVITY_CHANGE);
    matchingSkills.AddEvent(CommonEventSupport::COMMON_EVENT_PACKAGE_ADDED);
    matchingSkills.AddEvent(CommonEventSupport::COMMON_EVENT_PACKAGE_REMOVED);
    matchingSkills.AddEvent(CommonEventSupport::COMMON_EVENT_SCREEN_ON);
    matchingSkills.AddEvent(CommonEventSupport::COMMON_EVENT_SCREEN_OFF);
    matchingSkills.AddEvent(CommonEventSupport::COMMON_EVENT_USER_SWITCHED);
    matchingSkills.AddEvent(CommonEventSupport::COMMON_EVENT_USER_REMOVED);
    matchingSkills.AddEvent("common.event.UNLOCK_SCREEN");
    matchingSkills.AddEvent("common.event.LOCK_SCREEN");
    CommonEventSubscribeInfo subscriberInfo(matchingSkills);
    subscriber_ = std::make_shared<EventController>(subscriberInfo);
    if (CommonEventManager::SubscribeCommonEvent(subscriber_)) {
        RESSCHED_LOGD("SubscribeCommonEvent ok");
    } else {
        RESSCHED_LOGW("SubscribeCommonEvent fail");
    }
}

void EventController::OnReceiveEvent(const EventFwk::CommonEventData &data)
{
    Want want = data.GetWant();
    std::string action = want.GetAction();
    RESSCHED_LOGD("Recieved common event:%{public}s", action.c_str());

    nlohmann::json payload = nlohmann::json::object();
    if (action == CommonEventSupport::COMMON_EVENT_PACKAGE_REMOVED) {
        HandlePkgAddRemove(want, payload);
        ReportDataInProcess(ResType::RES_TYPE_APP_INSTALL_UNINSTALL, ResType::AppInstallStatus::APP_UNINSTALL, payload);
        return;
    }
    if (action == CommonEventSupport::COMMON_EVENT_PACKAGE_ADDED) {
        HandlePkgAddRemove(want, payload);
        ReportDataInProcess(ResType::RES_TYPE_APP_INSTALL_UNINSTALL, ResType::AppInstallStatus::APP_INSTALL, payload);
        return;
    }
    if (action == CommonEventSupport::COMMON_EVENT_SCREEN_ON) {
        ReportDataInProcess(ResType::RES_TYPE_SCREEN_STATUS, ResType::ScreenStatus::SCREEN_ON, payload);
        return;
    }
    if (action == CommonEventSupport::COMMON_EVENT_SCREEN_OFF) {
        ReportDataInProcess(ResType::RES_TYPE_SCREEN_STATUS, ResType::ScreenStatus::SCREEN_OFF, payload);
        return;
    }
    if (action == CommonEventSupport::COMMON_EVENT_CONNECTIVITY_CHANGE) {
        int32_t code = data.GetCode();
        HandleConnectivityChange(want, code, payload);
        return;
    }
    if (action == CommonEventSupport::COMMON_EVENT_USER_SWITCHED) {
        int32_t userId = data.GetCode();
        ReportDataInProcess(ResType::RES_TYPE_USER_SWITCH, static_cast<int64_t>(userId), payload);
        return;
    }
    if (action == CommonEventSupport::COMMON_EVENT_USER_REMOVED) {
        int32_t userId = data.GetCode();
        ReportDataInProcess(ResType::RES_TYPE_USER_REMOVE, static_cast<int64_t>(userId), payload);
        return;
    }
    if (action == "common.event.UNLOCK_SCREEN") {
        ReportDataInProcess(ResType::RES_TYPE_SCREEN_LOCK, ResType::ScreenLockStatus::SCREEN_UNLOCK, payload);
        return;
    }
    if (action == "common.event.LOCK_SCREEN") {
        ReportDataInProcess(ResType::RES_TYPE_SCREEN_LOCK, ResType::ScreenLockStatus::SCREEN_LOCK, payload);
        return;
    }
}

void EventController::SystemAbilityStatusChangeListener::OnRemoveSystemAbility(
    int32_t systemAbilityId, const std::string& deviceId)
{
    RESSCHED_LOGW("common event service is removed.");
    subscriber_ = nullptr;
}

void EventController::SystemAbilityStatusChangeListener::Stop()
{
    if (subscriber_ == nullptr) {
        return;
    }
    CommonEventManager::UnSubscribeCommonEvent(subscriber_);
    subscriber_ = nullptr;
}

extern "C" void EventControllerInit()
{
    EventController::GetInstance().Init();
}

extern "C" void EventControllerStop()
{
    EventController::GetInstance().Stop();
}
}
}