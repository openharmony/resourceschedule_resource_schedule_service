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
#include "ipc_skeleton.h"
#include "iservice_registry.h"
#include "parameters.h"
#include "res_sched_log.h"
#include "telephony_observer_client.h"
#include "core_service_client.h"
#include "system_ability_definition.h"
#ifdef DEVICE_MOVEMENT_PERCEPTION_ENABLE
#include "movement_client.h"
#include "movement_data_utils.h"
#endif

namespace OHOS {
namespace ResourceSchedule {
const static int8_t OPERATION_SUCCESS = 0;
const static bool DEVICE_MOVEMENT_OBSERVER_ENABLE =
    system::GetBoolParameter("persist.sys.ressched_device_movement_observer_switch", false);
IMPLEMENT_SINGLE_INSTANCE(ObserverManager)

void ObserverManager::Init()
{
    InitSysAbilityListener();
}

void ObserverManager::Disable()
{
    handleObserverMap_.clear();
    removeObserverMap_.clear();
    DisableCameraObserver();
    DisableTelephonyObserver();
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

    handleObserverMap_ = {
        { DFX_SYS_EVENT_SERVICE_ABILITY_ID, std::bind(&ObserverManager::InitCameraObserver, std::placeholders::_1) },
        { TELEPHONY_STATE_REGISTRY_SYS_ABILITY_ID,
            std::bind(&ObserverManager::InitTelephonyObserver, std::placeholders::_1) },
        { AUDIO_POLICY_SERVICE_ID, std::bind(&ObserverManager::InitAudioObserver, std::placeholders::_1) },
        { MSDP_MOVEMENT_SERVICE_ID, std::bind(&ObserverManager::InitDeviceMovementObserver, std::placeholders::_1) }
    };
    removeObserverMap_ = {
        { DFX_SYS_EVENT_SERVICE_ABILITY_ID, std::bind(&ObserverManager::DisableCameraObserver, std::placeholders::_1) },
        { TELEPHONY_STATE_REGISTRY_SYS_ABILITY_ID,
            std::bind(&ObserverManager::DisableTelephonyObserver, std::placeholders::_1) },
        { AUDIO_POLICY_SERVICE_ID, std::bind(&ObserverManager::DisableAudioObserver, std::placeholders::_1) },
        { MSDP_MOVEMENT_SERVICE_ID, std::bind(&ObserverManager::DisableDeviceMovementObserver, std::placeholders::_1) }
    };
    AddItemToSysAbilityListener(DFX_SYS_EVENT_SERVICE_ABILITY_ID, systemAbilityManager);
    AddItemToSysAbilityListener(TELEPHONY_STATE_REGISTRY_SYS_ABILITY_ID, systemAbilityManager);
    AddItemToSysAbilityListener(AUDIO_POLICY_SERVICE_ID, systemAbilityManager);
    AddItemToSysAbilityListener(MSDP_MOVEMENT_SERVICE_ID, systemAbilityManager);
}

inline void ObserverManager::AddItemToSysAbilityListener(int32_t systemAbilityId,
    sptr<ISystemAbilityManager>& systemAbilityManager)
{
    if (systemAbilityManager->SubscribeSystemAbility(systemAbilityId, sysAbilityListener_) != ERR_OK) {
        sysAbilityListener_ = nullptr;
        RESSCHED_LOGE("%{public}s: subscribe system ability id: %{public}d failed", __func__, systemAbilityId);
    }
}

void ObserverManager::SystemAbilityStatusChangeListener::OnAddSystemAbility(
    int32_t systemAbilityId, const std::string& deviceId)
{
    RESSCHED_LOGI("Add system ability, system ability id: %{public}d", systemAbilityId);
    auto funcIter = ObserverManager::GetInstance().handleObserverMap_.find(systemAbilityId);
    if (funcIter != ObserverManager::GetInstance().handleObserverMap_.end()) {
        auto function = funcIter->second;
            if (function) {
                function(&ObserverManager::GetInstance());
            }
    }
}

void ObserverManager::SystemAbilityStatusChangeListener::OnRemoveSystemAbility(
    int32_t systemAbilityId, const std::string& deviceId)
{
    RESSCHED_LOGD("Remove system ability, system ability id: %{public}d", systemAbilityId);
    auto funcIter = ObserverManager::GetInstance().removeObserverMap_.find(systemAbilityId);
    if (funcIter != ObserverManager::GetInstance().removeObserverMap_.end()) {
        auto function = funcIter->second;
            if (function) {
                function(&ObserverManager::GetInstance());
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
    auto res = HiviewDFX::HiSysEventManager::AddListener(cameraObserver_, sysRules);
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
        telephonyObserver_, slotId_, Telephony::TelephonyObserverBroker::OBSERVER_MASK_CALL_STATE, false);
    if (res == OPERATION_SUCCESS) {
        RESSCHED_LOGD("ObserverManager init telephony observer successfully");
    } else {
        RESSCHED_LOGW("ObserverManager init telephony observer failed");
    }
}

void ObserverManager::DisableTelephonyObserver()
{
    RESSCHED_LOGI("Disable telephony observer");
    if (!telephonyObserver_) {
        RESSCHED_LOGD("ObserverManager has been disable telephony observer");
        return ;
    }
    slotId_ = 0;
    Telephony::TelephonyObserverClient::GetInstance().RemoveStateObserver(
        slotId_, Telephony::TelephonyObserverBroker::OBSERVER_MASK_CALL_STATE);
    telephonyObserver_ = nullptr;
}

void ObserverManager::InitAudioObserver()
{
    pid_ = getpid();
    RESSCHED_LOGI("ObserverManager Init audio observer, pid: %{public}d", pid_);
    if (!audioObserver_) {
        audioObserver_ = std::make_shared<AudioObserver>();
    }

    auto res = AudioStandard::AudioStreamManager::GetInstance()->RegisterAudioRendererEventListener(pid_,
        audioObserver_);
    if (res == OPERATION_SUCCESS) {
        RESSCHED_LOGD("ObserverManager init audioRenderStateObserver successfully");
    } else {
        RESSCHED_LOGW("ObserverManager init audioRenderStateObserver failed");
    }

    res = AudioStandard::AudioSystemManager::GetInstance()->SetRingerModeCallback(pid_, audioObserver_);
    if (res == OPERATION_SUCCESS) {
        RESSCHED_LOGD("ObserverManager init audioRingModeObserver successfully");
    } else {
        RESSCHED_LOGW("ObserverManager init audioRingModeObserver failed");
    }

    res = AudioStandard::AudioSystemManager::GetInstance()->RegisterVolumeKeyEventCallback(pid_, audioObserver_);
    if (res == OPERATION_SUCCESS) {
        RESSCHED_LOGD("ObserverManager init audioVolumeKeyObserver successfully");
    } else {
        RESSCHED_LOGW("ObserverManager init audioVolumeKeyObserver failed");
    }
}

void ObserverManager::DisableAudioObserver()
{
    RESSCHED_LOGI("Disable telephony observer");
    if (!audioObserver_) {
        RESSCHED_LOGD("ObserverManager has been disable audioObserver");
        return ;
    }

    auto res = AudioStandard::AudioStreamManager::GetInstance()->UnregisterAudioRendererEventListener(pid_);
    if (res == OPERATION_SUCCESS) {
        RESSCHED_LOGD("ObserverManager disable audioVolumeKeyObserver successfully");
    } else {
        RESSCHED_LOGW("ObserverManager disable audioVolumeKeyObserver failed");
    }

    res = AudioStandard::AudioSystemManager::GetInstance()->UnsetRingerModeCallback(pid_);
    if (res == OPERATION_SUCCESS) {
        RESSCHED_LOGD("ObserverManager disable audioVolumeKeyObserver successfully");
    } else {
        RESSCHED_LOGW("ObserverManager disable audioVolumeKeyObserver failed");
    }

    res = AudioStandard::AudioSystemManager::GetInstance()->UnregisterVolumeKeyEventCallback(pid_);
    if (res == OPERATION_SUCCESS) {
        RESSCHED_LOGD("ObserverManager disable audioVolumeKeyObserver successfully");
    } else {
        RESSCHED_LOGW("ObserverManager disable audioVolumeKeyObserver failed");
    }
    audioObserver_ = nullptr;
}

void ObserverManager::InitDeviceMovementObserver()
{
    if (!DEVICE_MOVEMENT_OBSERVER_ENABLE) {
        RESSCHED_LOGI("Device movement observer is not enable");
        return;
    }

#ifdef DEVICE_MOVEMENT_PERCEPTION_ENABLE
    RESSCHED_LOGI("InitDeviceMovementObserver");
    if (!deviceMovementObserver_) {
        deviceMovementObserver_ = sptr<DeviceMovementObserver>(new DeviceMovementObserver());
    }
    Msdp::MovementClient::GetInstance().SubscribeCallback(
        Msdp::MovementDataUtils::MovementType::TYPE_STILL, deviceMovementObserver_);
#endif
}

void ObserverManager::DisableDeviceMovementObserver()
{
    if (!DEVICE_MOVEMENT_OBSERVER_ENABLE) {
        RESSCHED_LOGI("Device movement observer is not enable");
        return;
    }

#ifdef DEVICE_MOVEMENT_PERCEPTION_ENABLE
    RESSCHED_LOGI("DisableDeviceMovementObserver");
    if (!deviceMovementObserver_) {
        RESSCHED_LOGD("ObserverManager has been disable deviceMovementObserver");
        return;
    }
    Msdp::MovementClient::GetInstance().UnSubscribeCallback(
        Msdp::MovementDataUtils::MovementType::TYPE_STILL, deviceMovementObserver_);
    deviceMovementObserver_ = nullptr;
#endif
}

extern "C" void ObserverManagerInit()
{
    ObserverManager::GetInstance().Init();
}

extern "C" void ObserverManagerDisable()
{
    ObserverManager::GetInstance().Disable();
}
} // namespace ResourceSchedule
} // namespace OHOS
