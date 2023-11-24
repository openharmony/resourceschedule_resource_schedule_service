/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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
#include <string>

#include "observer_manager.h"

#include "hisysevent.h"
#include "hisysevent_manager.h"
#include "ipc_skeleton.h"
#include "iservice_registry.h"
#include "parameters.h"
#include "res_sched_log.h"
#include "connection_observer_client.h"
#ifdef RESSCHED_TELEPHONY_STATE_REGISTRY_ENABLE
#include "telephony_observer_client.h"
#endif
#include "system_ability_definition.h"
#ifdef DEVICE_MOVEMENT_PERCEPTION_ENABLE
#include "movement_client.h"
#include "movement_data_utils.h"
#endif
#include "input_manager.h"
#include "sched_controller.h"
#include "supervisor.h"
#ifdef RESSCHED_MULTIMEDIA_AV_SESSION_ENABLE
#include "avsession_manager.h"
#endif

namespace OHOS {
namespace ResourceSchedule {
const static int8_t OPERATION_SUCCESS = 0;
const static int32_t TUPLE_PID = 0;
const static int32_t TUPLE_UID = 1;
const static int32_t TUPLE_NAME = 2;
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
    DisableHiSysEventObserver();
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
        { DFX_SYS_EVENT_SERVICE_ABILITY_ID, std::bind(&ObserverManager::InitHiSysEventObserver,
            std::placeholders::_1) },
        { TELEPHONY_STATE_REGISTRY_SYS_ABILITY_ID,
            std::bind(&ObserverManager::InitTelephonyObserver, std::placeholders::_1) },
        { AUDIO_POLICY_SERVICE_ID, std::bind(&ObserverManager::InitAudioObserver, std::placeholders::_1) },
        { MSDP_MOVEMENT_SERVICE_ID, std::bind(&ObserverManager::InitDeviceMovementObserver, std::placeholders::_1) },
        { MULTIMODAL_INPUT_SERVICE_ID, std::bind(&ObserverManager::InitMMiEventObserver, std::placeholders::_1) },
        { ABILITY_MST_SERVICE_ID, std::bind(&ObserverManager::InitConnectionSubscriber, std::placeholders::_1) },
#ifdef RESSCHED_MULTIMEDIA_AV_SESSION_ENABLE
        { AVSESSION_SERVICE_ID, std::bind(&ObserverManager::InitAVSessionStateChangeListener, std::placeholders::_1) },
#endif
    };
    removeObserverMap_ = {
        { DFX_SYS_EVENT_SERVICE_ABILITY_ID, std::bind(&ObserverManager::DisableHiSysEventObserver,
            std::placeholders::_1) },
        { TELEPHONY_STATE_REGISTRY_SYS_ABILITY_ID,
            std::bind(&ObserverManager::DisableTelephonyObserver, std::placeholders::_1) },
        { AUDIO_POLICY_SERVICE_ID, std::bind(&ObserverManager::DisableAudioObserver, std::placeholders::_1) },
        { MSDP_MOVEMENT_SERVICE_ID, std::bind(&ObserverManager::DisableDeviceMovementObserver, std::placeholders::_1) },
        { MULTIMODAL_INPUT_SERVICE_ID, std::bind(&ObserverManager::DisableMMiEventObserver, std::placeholders::_1) },
        { ABILITY_MST_SERVICE_ID, std::bind(&ObserverManager::DisableConnectionSubscriber, std::placeholders::_1) },
#ifdef RESSCHED_MULTIMEDIA_AV_SESSION_ENABLE
        { AVSESSION_SERVICE_ID,
          std::bind(&ObserverManager::DisableAVSessionStateChangeListener, std::placeholders::_1) },
#endif
    };
    AddItemToSysAbilityListener(DFX_SYS_EVENT_SERVICE_ABILITY_ID, systemAbilityManager);
    AddItemToSysAbilityListener(TELEPHONY_STATE_REGISTRY_SYS_ABILITY_ID, systemAbilityManager);
    AddItemToSysAbilityListener(AUDIO_POLICY_SERVICE_ID, systemAbilityManager);
    AddItemToSysAbilityListener(MSDP_MOVEMENT_SERVICE_ID, systemAbilityManager);
    AddItemToSysAbilityListener(MULTIMODAL_INPUT_SERVICE_ID, systemAbilityManager);
    AddItemToSysAbilityListener(ABILITY_MST_SERVICE_ID, systemAbilityManager);
#ifdef RESSCHED_MULTIMEDIA_AV_SESSION_ENABLE
    AddItemToSysAbilityListener(AVSESSION_SERVICE_ID, systemAbilityManager);
#endif
}

inline void ObserverManager::AddItemToSysAbilityListener(int32_t systemAbilityId,
    sptr<ISystemAbilityManager>& systemAbilityManager)
{
    if (systemAbilityManager->SubscribeSystemAbility(systemAbilityId, sysAbilityListener_) != ERR_OK) {
        sysAbilityListener_ = nullptr;
        RESSCHED_LOGE("%{public}s: subscribe system ability id: %{public}d failed", __func__, systemAbilityId);
        HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::RSS, "INIT_FAULT", HiviewDFX::HiSysEvent::EventType::FAULT,
                        "COMPONENT_NAME", "MAIN",
                        "ERR_TYPE", "register failure",
                        "ERR_MSG", "Register a staus change listener of the " + std::to_string(systemAbilityId) + " SA failed!");
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

void ObserverManager::InitHiSysEventObserver()
{
    RESSCHED_LOGI("Init hisysevent observer");
    if (!hiSysEventObserver_) {
        hiSysEventObserver_ = std::make_shared<ResourceSchedule::HiSysEventObserver>();
    }

    HiviewDFX::ListenerRule statsRule("PowerStats");
    HiviewDFX::ListenerRule avCodecStartState("AV_CODEC", "CODEC_START_INFO");
    HiviewDFX::ListenerRule avCodecStopState("AV_CODEC", "CODEC_STOP_INFO");
    std::vector<HiviewDFX::ListenerRule> sysRules;
    sysRules.push_back(statsRule);
    sysRules.push_back(avCodecStartState);
    sysRules.push_back(avCodecStopState);
    auto res = HiviewDFX::HiSysEventManager::AddListener(hiSysEventObserver_, sysRules);
    if (res == 0) {
        RESSCHED_LOGD("ObserverManager init hisysevent observer successfully");
    } else {
        RESSCHED_LOGW("ObserverManager init hisysevent observer failed");
        HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::RSS, "INIT_FAULT", HiviewDFX::HiSysEvent::EventType::FAULT,
                        "COMPONENT_NAME", "MAIN",
                        "ERR_TYPE", "register failure",
                        "ERR_MSG", "Register a hisysevent observer failed!");
    }
}

void ObserverManager::DisableHiSysEventObserver()
{
    RESSCHED_LOGI("Disable hisysevent observer");
    if (hiSysEventObserver_ == nullptr) {
        return;
    }

    auto res = HiviewDFX::HiSysEventManager::RemoveListener(hiSysEventObserver_);
    if (res == 0) {
        RESSCHED_LOGD("ObserverManager disable hisysevent observer successfully");
    } else {
        RESSCHED_LOGW("ObserverManager disable hisysevent observer failed");
    }
    hiSysEventObserver_ = nullptr;
}

void ObserverManager::InitTelephonyObserver()
{
#ifdef RESSCHED_TELEPHONY_STATE_REGISTRY_ENABLE
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
        HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::RSS, "INIT_FAULT", HiviewDFX::HiSysEvent::EventType::FAULT,
                        "COMPONENT_NAME", "MAIN",
                        "ERR_TYPE", "register failure",
                        "ERR_MSG", "Register a telephony observer failed!");
    }
#endif
}

void ObserverManager::DisableTelephonyObserver()
{
#ifdef RESSCHED_TELEPHONY_STATE_REGISTRY_ENABLE
    RESSCHED_LOGI("Disable telephony observer");
    if (!telephonyObserver_) {
        RESSCHED_LOGD("ObserverManager has been disable telephony observer");
        return ;
    }
    slotId_ = 0;
    Telephony::TelephonyObserverClient::GetInstance().RemoveStateObserver(
        slotId_, Telephony::TelephonyObserverBroker::OBSERVER_MASK_CALL_STATE);
    telephonyObserver_ = nullptr;
#endif
}

void ObserverManager::InitAudioObserver()
{
#ifdef RESSCHED_AUDIO_FRAMEWORK_ENABLE
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
        HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::RSS, "INIT_FAULT", HiviewDFX::HiSysEvent::EventType::FAULT,
                        "COMPONENT_NAME", "MAIN",
                        "ERR_TYPE", "register failure",
                        "ERR_MSG", "Register a audio observer failed!");
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
#endif
}

void ObserverManager::DisableAudioObserver()
{
#ifdef RESSCHED_AUDIO_FRAMEWORK_ENABLE
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
#endif
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
    if (Msdp::MovementClient::GetInstance().SubscribeCallback(
        Msdp::MovementDataUtils::MovementType::TYPE_STILL, deviceMovementObserver_) != ERR_OK) {
            HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::RSS, "INIT_FAULT", HiviewDFX::HiSysEvent::EventType::FAULT,
                        "COMPONENT_NAME", "MAIN",
                        "ERR_TYPE", "register failure",
                        "ERR_MSG", "Register a device movement observer failed!");
        }
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

void ObserverManager::InitMMiEventObserver()
{
    RESSCHED_LOGI("ObserverManager Init mmi observer.");
    if (!mmiEventObserver_) {
        mmiEventObserver_ = std::make_shared<MmiObserver>();
    }

    auto res = MMI::InputManager::GetInstance()->AddInputEventObserver(mmiEventObserver_);
    if (res == OPERATION_SUCCESS) {
        RESSCHED_LOGD("ObserverManager init mmiEventObserver successfully");
    } else {
        RESSCHED_LOGE("ObserverManager init mmiEventObserver failed");
        HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::RSS, "INIT_FAULT", HiviewDFX::HiSysEvent::EventType::FAULT,
                        "COMPONENT_NAME", "MAIN",
                        "ERR_TYPE", "register failure",
                        "ERR_MSG", "Register a mmi observer failed!");
    }
    // Get all events registered in multimodal input.
    GetAllMmiStatusData();
}

void ObserverManager::DisableMMiEventObserver()
{
    RESSCHED_LOGI("Disable mmi observer");
    if (!mmiEventObserver_) {
        RESSCHED_LOGD("ObserverManager has been disable mmiEventObserver");
        return;
    }

    auto res = MMI::InputManager::GetInstance()->RemoveInputEventObserver(mmiEventObserver_);
    if (res == OPERATION_SUCCESS) {
        RESSCHED_LOGD("ObserverManager disable mmiEventObserver successfully");
    } else {
        RESSCHED_LOGW("ObserverManager disable mmiEventObserver failed");
    }
    mmiEventObserver_ = nullptr;
}

void ObserverManager::GetAllMmiStatusData()
{
    RESSCHED_LOGI("get all mmi subscribed events.");
    MMI::InputManager::GetInstance()->GetAllMmiSubscribedEvents(mmiStatusData_);
    if (mmiStatusData_.empty()) {
        RESSCHED_LOGI("get mmi subscribed events is null.");
        return;
    }
    auto supervisor = SchedController::GetInstance().GetSupervisor();
    if (supervisor == nullptr) {
        RESSCHED_LOGE("get supervisor is null.");
        return;
    }

    for (auto data = mmiStatusData_.begin(); data != mmiStatusData_.end(); ++data) {
        int32_t pid = std::get<TUPLE_PID>(data->first);
        int32_t uid = std::get<TUPLE_UID>(data->first);
        std::string bundleName = std::get<TUPLE_NAME>(data->first);
        int32_t status = data->second;
        RESSCHED_LOGD(
            "get mmi subscribed events, pid:%{public}d, uid:%{public}d, bundleName:%{public}s, status:%{public}d.",
            pid, uid, bundleName.c_str(), status);
        auto app = supervisor->GetAppRecordNonNull(uid);
        auto procRecord = app->GetProcessRecordNonNull(pid);
        app->SetName(bundleName);
        procRecord->mmiStatus_ = status;
    }
}

void ObserverManager::InitConnectionSubscriber()
{
    if (!connectionSubscriber_) {
        connectionSubscriber_ = std::make_shared<ConnectionSubscriber>();
    }

    auto res = AbilityRuntime::ConnectionObserverClient::GetInstance().RegisterObserver(connectionSubscriber_);
    if (res == OPERATION_SUCCESS) {
        RESSCHED_LOGD("ObserverManager init connectionSubscriber successfully");
    } else {
        RESSCHED_LOGW("ObserverManager init connectionSubscriber failed");
        HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::RSS, "INIT_FAULT", HiviewDFX::HiSysEvent::EventType::FAULT,
                        "COMPONENT_NAME", "MAIN",
                        "ERR_TYPE", "register failure",
                        "ERR_MSG", "Register a connect subscriber failed!");
    }
}

void ObserverManager::DisableConnectionSubscriber()
{
    RESSCHED_LOGI("Disable connect subscriber state listener");
    if (!connectionSubscriber_) {
        RESSCHED_LOGD("ObserverManager has been disable connect subscriber state listener");
        return;
    }

    auto res = AbilityRuntime::ConnectionObserverClient::GetInstance().UnregisterObserver(connectionSubscriber_);
    if (res == OPERATION_SUCCESS) {
        RESSCHED_LOGD("ObserverManager disable connect subscriber state listener successfully");
    } else {
        RESSCHED_LOGW("ObserverManager disable connect subscriber state listener failed");
    }

    connectionSubscriber_ = nullptr;
}

#ifdef RESSCHED_MULTIMEDIA_AV_SESSION_ENABLE
void ObserverManager::InitAVSessionStateChangeListener()
{
    if (!avSessionStateListener_) {
        avSessionStateListener_ = std::make_shared<AvSessionStateListener>();
    }

    auto res = AVSession::AVSessionManager::GetInstance().RegisterSessionListener(avSessionStateListener_);
    if (res == OPERATION_SUCCESS) {
        RESSCHED_LOGI("ObserverManager init session state listener successfully");
    } else {
        RESSCHED_LOGW("ObserverManager init session state listener failed");
        HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::RSS, "INIT_FAULT", HiviewDFX::HiSysEvent::EventType::FAULT,
                        "COMPONENT_NAME", "MAIN",
                        "ERR_TYPE", "register failure",
                        "ERR_MSG", "Register a session state listener failed!");
    }
}

void ObserverManager::DisableAVSessionStateChangeListener()
{
    RESSCHED_LOGI("Disable session state listener");
    avSessionStateListener_ = nullptr;
}
#endif

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
