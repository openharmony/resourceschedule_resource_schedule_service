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

#include "observer_manager.h"

#include "hisysevent_manager.h"
#include "if_system_ability_manager.h"
#include "ipc_skeleton.h"
#include "iservice_registry.h"
#include "res_sched_log.h"
#include "telephony_observer_client.h"
#include "core_service_client.h"
#include "system_ability_definition.h"

namespace OHOS {
namespace ResourceSchedule {
const static int8_t TELEPHONY_SUCCESS = 0;
IMPLEMENT_SINGLE_INSTANCE(ObserverManager)

void ObserverManager::Init()
{
    InitSysAbilityListener();
}

void ObserverManager::Disable()
{
    DisableCameraObserver();
    sysAbilityListener_ = nullptr;
}

void ObserverManager::InitSysAbilityListener()
{
    if (sysAbilityListener_ != nullptr) {
        return;
    }

    sysAbilityListener_ = new (std::nothrow) SystemAbilityStatusChangeListener();
    if (sysAbilityListener_ == nullptr) {
        RESSCHED_LOGE("Failed to create statusChangeListener due to no memory");
        return;
    }

    sptr<ISystemAbilityManager> systemAbilityManager
        = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (systemAbilityManager == nullptr) {
        sysAbilityListener_ = nullptr;
        RESSCHED_LOGE("systemAbilityManager is null");
        return;
    }

    int32_t ret = systemAbilityManager->SubscribeSystemAbility(DFX_SYS_EVENT_SERVICE_ABILITY_ID, sysAbilityListener_);
    if (ret != ERR_OK) {
        sysAbilityListener_ = nullptr;
        RESSCHED_LOGE("subscribe system ability id: %{public}d failed", DFX_SYS_EVENT_SERVICE_ABILITY_ID);
    }
    ret = systemAbilityManager->SubscribeSystemAbility(TELEPHONY_STATE_REGISTRY_SYS_ABILITY_ID, sysAbilityListener_);
    if (ret != ERR_OK) {
        sysAbilityListener_ = nullptr;
        RESSCHED_LOGE("%{public}s: subscribe system ability id: %{public}d failed",
            __func__, TELEPHONY_STATE_REGISTRY_SYS_ABILITY_ID);
    }
}

void ObserverManager::SystemAbilityStatusChangeListener::OnAddSystemAbility(
    int32_t systemAbilityId, const std::string& deviceId)
{
    RESSCHED_LOGI("Add system ability, system ability id: %{public}d", systemAbilityId);
    switch (systemAbilityId) {
        case DFX_SYS_EVENT_SERVICE_ABILITY_ID: {
            ObserverManager::GetInstance().InitCameraObserver();
            break;
        }
        case TELEPHONY_STATE_REGISTRY_SYS_ABILITY_ID: {
            ObserverManager::GetInstance().InitTelephonyObserver();
            break;
        }
        default: {
            break;
        }
    }
}

void ObserverManager::SystemAbilityStatusChangeListener::OnRemoveSystemAbility(
    int32_t systemAbilityId, const std::string& deviceId)
{
    RESSCHED_LOGD("Remove system ability, system ability id: %{public}d", systemAbilityId);
    switch (systemAbilityId) {
        case DFX_SYS_EVENT_SERVICE_ABILITY_ID: {
            ObserverManager::GetInstance().DisableCameraObserver();
            break;
        }
        case TELEPHONY_STATE_REGISTRY_SYS_ABILITY_ID: {
            ObserverManager::GetInstance().DisableTelephonyObserver();
            break;
        }
        default: {
            break;
        }
    }
}

void ObserverManager::InitCameraObserver()
{
    RESSCHED_LOGI("Init camera observer");
    if (!cameraObserver_) {
        cameraObserver_ = std::make_shared<ResourceSchedule::CameraObserver>();
    }

    HiviewDFX::ListenerRule cameraStateRule("CAMERA", "CAMERA_STATE");
    HiviewDFX::ListenerRule cameraStatisticRule("CAMERA", "CAMERA_STATISTIC");
    std::vector<HiviewDFX::ListenerRule> sysRules;
    sysRules.push_back(cameraStateRule);
    sysRules.push_back(cameraStatisticRule);
    auto res = HiviewDFX::HiSysEventManager::AddEventListener(cameraObserver_, sysRules);
    if (res == 0) {
        RESSCHED_LOGD("ObserverManager init camera observer successfully");
    } else {
        RESSCHED_LOGW("ObserverManager init camera observer failed");
    }
}

void ObserverManager::DisableCameraObserver()
{
    RESSCHED_LOGI("Disable camera observer");
    if (cameraObserver_ == nullptr) {
        return;
    }

    auto res = HiviewDFX::HiSysEventManager::RemoveListener(cameraObserver_);
    if (res == 0) {
        RESSCHED_LOGD("ObserverManager disable camera observer successfully");
    } else {
        RESSCHED_LOGW("ObserverManager disable camera observer failed");
    }
    cameraObserver_ = nullptr;
}

void ObserverManager::InitTelephonyObserver()
{
    RESSCHED_LOGI("Init telephony observer");
    if (!telephonyObserver_) {
        telephonyObserver_ = std::make_unique<SchedTelephonyObserver>().release();
    }
    slotId_ = 0;
    auto res = Telephony::TelephonyObserverClient::GetInstance().AddStateObserver(
        telephonyObserver_, slotId_, Telephony::TelephonyObserverBroker::OBSERVER_MASK_CALL_STATE, true);
    if (res == TELEPHONY_SUCCESS) {
        RESSCHED_LOGD("ObserverManager init telephony observer successfully");
    } else {
        RESSCHED_LOGW("ObserverManager init telephony observer failed");
    }
}

void ObserverManager::DisableTelephonyObserver()
{
    RESSCHED_LOGI("Disable telephony observer");
    if (telephonyObserver_) {
        telephonyObserver_ = nullptr;
    }
    slotId_ = 0;
    Telephony::TelephonyObserverClient::GetInstance().RemoveStateObserver(
        slotId_, Telephony::TelephonyObserverBroker::OBSERVER_MASK_CALL_STATE);
}
} // namespace ResourceSchedule
} // namespace OHOS
