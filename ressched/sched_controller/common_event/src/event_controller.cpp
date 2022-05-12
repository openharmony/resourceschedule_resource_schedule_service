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
#include "res_sched_log.h"
#include "system_ability_definition.h"

#include "ressched_utils.h"
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
    sysAbilityListener_= new (std::nothrow) SystemAbilityStatusChangeListener();
    if (sysAbilityListener_ == nullptr) {
        RESSCHED_LOGE("Failed to create statusChangeListener due to no memory.");
        return;
    }
    sptr<ISystemAbilityManager> systemAbilityManager
        = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (systemAbilityManager == nullptr) {
        RESSCHED_LOGD("systemAbilityManager is null");
        sysAbilityListener_ = nullptr;
        return;
    }
    int32_t ret = systemAbilityManager->SubscribeSystemAbility(COMMON_EVENT_SERVICE_ID, sysAbilityListener_);
    if (ret != ERR_OK) {
        RESSCHED_LOGE("subscribe system ability id: %{public}d failed", COMMON_EVENT_SERVICE_ID);
        sysAbilityListener_ = nullptr;
    }
}

void EventController::HandlePkgRemove(const EventFwk::Want &want, Json::Value &payload) const
{
    AppExecFwk::ElementName ele = want.GetElement();
    std::string bundleName = ele.GetBundleName();
    int32_t userId = want.GetIntParam(AppExecFwk::Constants::USER_ID, -1);
    int32_t uid = GetUid(userId, bundleName);
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
    sptr<AppExecFwk::IBundleMgr> bundleMgr = iface_cast<AppExecFwk::IBundleMgr>(remoteObject);
    bundleMgr->GetApplicationInfo(bundleName, AppExecFwk::ApplicationFlag::GET_BASIC_APPLICATION_INFO, userId, info);
    return static_cast<int32_t>(info.uid);
}

void EventController::HandleConnectivityChange(
    const EventFwk::Want &want, const int32_t &code, Json::Value &payload)
{
    ReportDataInProcess(ResType::RES_TYPE_NET_CONNECT_STATE_CHANGE, code, payload);
    if (code == NetManagerStandard::NetConnState::NET_CONN_STATE_CONNECTED) {
        int32_t netType = want.GetIntParam("NetType", -1);
        ReportDataInProcess(ResType::RSE_TYPE_NET_BEAR_TYPE, netType, payload);
    }
}

void EventController::ReportDataInProcess(const uint32_t &resType, const int64_t &value, const Json::Value& payload)
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

    Json::Value payload;
    if (action == CommonEventSupport::COMMON_EVENT_PACKAGE_REMOVED) {
        HandlePkgRemove(want, payload);
        ReportDataInProcess(ResType::RES_TYPE_APP_INSTALL, 0, payload);
        return;
    }
    if (action == CommonEventSupport::COMMON_EVENT_PACKAGE_ADDED) {
        ReportDataInProcess(ResType::RES_TYPE_APP_INSTALL, 1, payload);
        return;
    }
    if (action == CommonEventSupport::COMMON_EVENT_SCREEN_ON) {
        ReportDataInProcess(ResType::RES_TYPE_SCREEN_STATUS, 1, payload);
        return;
    }
    if (action == CommonEventSupport::COMMON_EVENT_SCREEN_OFF) {
        ReportDataInProcess(ResType::RES_TYPE_SCREEN_STATUS, 0, payload);
        return;
    }
    if (action == CommonEventSupport::COMMON_EVENT_CONNECTIVITY_CHANGE) {
        int32_t code = data.GetCode();
        HandleConnectivityChange(want, code, payload);
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
}
}