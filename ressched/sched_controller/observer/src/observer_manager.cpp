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

#include "observer_manager.h"

#include <dlfcn.h>
#include <string>

#include "app_mgr_interface.h"
#include "app_startup_scene_rec.h"
#include "display_manager.h"
#include "dm_common.h"
#include "hisysevent.h"
#ifdef RESOURCE_SCHEDULE_SERVICE_WITH_APP_NAP_ENABLE
#include "hisysevent_manager.h"
#endif
#include "ipc_skeleton.h"
#include "iservice_registry.h"
#include "parameters.h"
#include "res_sched_log.h"
#include "res_sched_mgr.h"
#include "res_type.h"
#include "connection_observer_client.h"
#ifdef RESSCHED_TELEPHONY_STATE_REGISTRY_ENABLE
#include "telephony_observer_client.h"
#endif
#include "oobe_manager.h"
#include "system_ability_definition.h"
#ifdef DEVICE_MOVEMENT_PERCEPTION_ENABLE
#include "movement_client.h"
#include "movement_data_utils.h"
#endif
#ifdef MMI_ENABLE
#include "input_manager.h"
#endif
#include "os_account_manager.h"
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
    system::GetBoolParameter("persist.sys.ressched_device_movement_observer_switch", true);
const static char* RES_SCHED_CG_EXT_SO = "libcgroup_sched_ext.z.so";

void ObserverManager::Init()
{
    InitSysAbilityListener();
    AppStartupSceneRec::GetInstance().Init();
}

void ObserverManager::Disable()
{
    AppStartupSceneRec::GetInstance().Deinit();
    handleObserverMap_.clear();
    removeObserverMap_.clear();
#ifdef RESOURCE_SCHEDULE_SERVICE_WITH_APP_NAP_ENABLE
    DisableHiSysEventObserver();
#endif
    DisableTelephonyObserver();
    sysAbilityListener_ = nullptr;
}

void ObserverManager::InitObserverCbMap()
{
    handleObserverMap_ = {
#ifdef RESOURCE_SCHEDULE_SERVICE_WITH_APP_NAP_ENABLE
        { DFX_SYS_EVENT_SERVICE_ABILITY_ID, []() { ObserverManager::GetInstance()->InitHiSysEventObserver(); }},
#endif
#ifdef MMI_ENABLE
        { MULTIMODAL_INPUT_SERVICE_ID, []() { ObserverManager::GetInstance()->InitMMiEventObserver(); }},
#endif
        { TELEPHONY_STATE_REGISTRY_SYS_ABILITY_ID, []() { ObserverManager::GetInstance()->InitTelephonyObserver(); }},
        { AUDIO_POLICY_SERVICE_ID, []() { ObserverManager::GetInstance()->InitAudioObserver(); }},
        { MSDP_MOVEMENT_SERVICE_ID, []() { ObserverManager::GetInstance()->InitDeviceMovementObserver(); }},
        { DISPLAY_MANAGER_SERVICE_ID, []() { ObserverManager::GetInstance()->InitDisplayModeObserver(); }},
        { DISPLAY_MANAGER_SERVICE_SA_ID, []() { ObserverManager::GetInstance()->InitDisplayOrientationObserver(); }},
        { ABILITY_MGR_SERVICE_ID, []() { ObserverManager::GetInstance()->InitConnectionSubscriber(); }},
        { DISTRIBUTED_KV_DATA_SERVICE_ABILITY_ID, []() { ObserverManager::GetInstance()->InitDataShareObserver(); }},
#ifdef RESOURCE_REQUEST_REQUEST
        { DOWNLOAD_SERVICE_ID, []() { ObserverManager::GetInstance()->InitDownloadUploadObserver(); }},
#endif
#ifdef RESSCHED_MULTIMEDIA_AV_SESSION_ENABLE
        { AVSESSION_SERVICE_ID, []() { ObserverManager::GetInstance()->InitAVSessionStateChangeListener(); }},
#endif
        { SUBSYS_ACCOUNT_SYS_ABILITY_ID_BEGIN, []() { ObserverManager::GetInstance()->InitAccountObserver(); }},
        { WINDOW_MANAGER_SERVICE_ID, []() { ObserverManager::GetInstance()->InitWindowStateObserver(); }},
        { APP_MGR_SERVICE_ID, []() { ObserverManager::GetInstance()->SubscribeAppState(); }},
#ifdef CONFIG_BGTASK_MGR
        { BACKGROUND_TASK_MANAGER_SERVICE_ID, []() { ObserverManager::GetInstance()->SubscribeBackgroundTask(); }},
#endif
    };
    InitRemoveObserverCbMap();
}

void ObserverManager::InitRemoveObserverCbMap()
{
    removeObserverMap_ = {
#ifdef RESOURCE_SCHEDULE_SERVICE_WITH_APP_NAP_ENABLE
        { DFX_SYS_EVENT_SERVICE_ABILITY_ID, []() { ObserverManager::GetInstance()->DisableHiSysEventObserver(); }},
#endif
#ifdef MMI_ENABLE
        { MULTIMODAL_INPUT_SERVICE_ID, []() { ObserverManager::GetInstance()->DisableMMiEventObserver(); }},
#endif
        { TELEPHONY_STATE_REGISTRY_SYS_ABILITY_ID, []() {
            ObserverManager::GetInstance()->DisableTelephonyObserver(); }},
        { AUDIO_POLICY_SERVICE_ID, []() { ObserverManager::GetInstance()->DisableAudioObserver(); }},
        { MSDP_MOVEMENT_SERVICE_ID, []() { ObserverManager::GetInstance()->DisableDeviceMovementObserver(); }},
        { DISPLAY_MANAGER_SERVICE_ID, []() { ObserverManager::GetInstance()->DisableDisplayModeObserver(); }},
        { DISPLAY_MANAGER_SERVICE_SA_ID, []() { ObserverManager::GetInstance()->DisableDisplayOrientationObserver(); }},
        { ABILITY_MGR_SERVICE_ID, []() { ObserverManager::GetInstance()->DisableConnectionSubscriber(); }},
        { DISTRIBUTED_KV_DATA_SERVICE_ABILITY_ID, []() {
            ObserverManager::GetInstance()->DisableDataShareObserver(); }},
#ifdef RESOURCE_REQUEST_REQUEST
        { DOWNLOAD_SERVICE_ID, []() { ObserverManager::GetInstance()->DisableDownloadUploadObserver(); }},
#endif
#ifdef RESSCHED_MULTIMEDIA_AV_SESSION_ENABLE
        { AVSESSION_SERVICE_ID, []() { ObserverManager::GetInstance()->DisableAVSessionStateChangeListener(); }},
#endif
        { SUBSYS_ACCOUNT_SYS_ABILITY_ID_BEGIN, []() { ObserverManager::GetInstance()->DisableAccountObserver(); }},
        { WINDOW_MANAGER_SERVICE_ID, []() { ObserverManager::GetInstance()->DisableWindowStateObserver(); }},
        { APP_MGR_SERVICE_ID, []() { ObserverManager::GetInstance()->UnsubscribeAppState(); }},
#ifdef CONFIG_BGTASK_MGR
        { BACKGROUND_TASK_MANAGER_SERVICE_ID, []() { ObserverManager::GetInstance()->UnsubscribeBackgroundTask(); }},
#endif
    };
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
    InitObserverCbMap();
    GetReportFunc();

    AddItemToSysAbilityListener(DFX_SYS_EVENT_SERVICE_ABILITY_ID, systemAbilityManager);
    AddItemToSysAbilityListener(TELEPHONY_STATE_REGISTRY_SYS_ABILITY_ID, systemAbilityManager);
    AddItemToSysAbilityListener(AUDIO_POLICY_SERVICE_ID, systemAbilityManager);
    AddItemToSysAbilityListener(MSDP_MOVEMENT_SERVICE_ID, systemAbilityManager);
    AddItemToSysAbilityListener(MULTIMODAL_INPUT_SERVICE_ID, systemAbilityManager);
    AddItemToSysAbilityListener(DISPLAY_MANAGER_SERVICE_ID, systemAbilityManager);
    AddItemToSysAbilityListener(DISPLAY_MANAGER_SERVICE_SA_ID, systemAbilityManager);
    AddItemToSysAbilityListener(ABILITY_MGR_SERVICE_ID, systemAbilityManager);
    AddItemToSysAbilityListener(DISTRIBUTED_KV_DATA_SERVICE_ABILITY_ID, systemAbilityManager);
#ifdef RESOURCE_REQUEST_REQUEST
    AddItemToSysAbilityListener(DOWNLOAD_SERVICE_ID, systemAbilityManager);
#endif
#ifdef RESSCHED_MULTIMEDIA_AV_SESSION_ENABLE
    AddItemToSysAbilityListener(AVSESSION_SERVICE_ID, systemAbilityManager);
#endif
    AddItemToSysAbilityListener(SUBSYS_ACCOUNT_SYS_ABILITY_ID_BEGIN, systemAbilityManager);
    AddItemToSysAbilityListener(WINDOW_MANAGER_SERVICE_ID, systemAbilityManager);
    AddItemToSysAbilityListener(APP_MGR_SERVICE_ID, systemAbilityManager);
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
    } else {
        RESSCHED_LOGI("%{public}s: subscribe system ability id: %{public}d succeed", __func__, systemAbilityId);
    }
}

void ObserverManager::SystemAbilityStatusChangeListener::OnAddSystemAbility(
    int32_t systemAbilityId, const std::string& deviceId)
{
    RESSCHED_LOGI("Add system ability, system ability id: %{public}d", systemAbilityId);
    auto funcIter = ObserverManager::GetInstance()->handleObserverMap_.find(systemAbilityId);
    if (funcIter != ObserverManager::GetInstance()->handleObserverMap_.end()) {
        auto function = funcIter->second;
            if (function) {
                function();
            }
    }
}

void ObserverManager::SystemAbilityStatusChangeListener::OnRemoveSystemAbility(
    int32_t systemAbilityId, const std::string& deviceId)
{
    RESSCHED_LOGD("Remove system ability, system ability id: %{public}d", systemAbilityId);
    auto funcIter = ObserverManager::GetInstance()->removeObserverMap_.find(systemAbilityId);
    if (funcIter != ObserverManager::GetInstance()->removeObserverMap_.end()) {
        auto function = funcIter->second;
            if (function) {
                function();
            }
    }
}

void ObserverManager::GetReportFunc()
{
    auto handle = dlopen(RES_SCHED_CG_EXT_SO, RTLD_NOW);
    if (!handle) {
        RESSCHED_LOGE("GetReportFunc dlopen failed");
        return;
    }
    ReportFunc reportFunc = reinterpret_cast<ReportFunc>(dlsym(handle, "IsNeedReportEvents"));
    if (!reportFunc) {
        RESSCHED_LOGE("GetReportFunc dlsym 'IsNeedReportEvents' failed");
        dlclose(handle);
        return;
    }

    isNeedReport_ = reportFunc();
}

#ifdef RESOURCE_SCHEDULE_SERVICE_WITH_APP_NAP_ENABLE
void ObserverManager::InitHiSysEventObserver()
{
    RESSCHED_LOGI("Init hisysevent observer");
    if (!hiSysEventObserver_) {
        hiSysEventObserver_ = std::make_shared<ResourceSchedule::HiSysEventObserver>();
    }

    HiviewDFX::ListenerRule audioStreamState("AUDIO", "STREAM_CHANGE");
    HiviewDFX::ListenerRule cameraConnectState("CAMERA", "CAMERA_CONNECT");
    HiviewDFX::ListenerRule cameraDisconnectState("CAMERA", "CAMERA_DISCONNECT");
    HiviewDFX::ListenerRule brSwitchState("BT_SERVICE", "BR_SWITCH_STATE");
    HiviewDFX::ListenerRule bleSwitchState("BT_SERVICE", "BLE_SWITCH_STATE");
    HiviewDFX::ListenerRule wifiConnectionState("COMMUNICATION", "WIFI_CONNECTION");
    HiviewDFX::ListenerRule wifiScanState("COMMUNICATION", "WIFI_SCAN");
    HiviewDFX::ListenerRule avCodecStartState("AV_CODEC", "CODEC_START_INFO");
    HiviewDFX::ListenerRule avCodecStopState("AV_CODEC", "CODEC_STOP_INFO");
    HiviewDFX::ListenerRule screenCaptureState("MULTIMEDIA", "PLAYER_STATE");
    std::vector<HiviewDFX::ListenerRule> sysRules;
    sysRules.push_back(audioStreamState);
    sysRules.push_back(cameraConnectState);
    sysRules.push_back(cameraDisconnectState);
    sysRules.push_back(brSwitchState);
    sysRules.push_back(bleSwitchState);
    sysRules.push_back(wifiConnectionState);
    sysRules.push_back(wifiScanState);
    sysRules.push_back(avCodecStartState);
    sysRules.push_back(avCodecStopState);
    sysRules.push_back(screenCaptureState);
    if (isNeedReport_) {
        HiviewDFX::ListenerRule runninglockState("POWER", "RUNNINGLOCK");
        sysRules.push_back(runninglockState);
    }
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
#endif

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
    if (res != OPERATION_SUCCESS) {
        RESSCHED_LOGW("ObserverManager init audioRenderStateObserver failed");
        HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::RSS, "INIT_FAULT", HiviewDFX::HiSysEvent::EventType::FAULT,
                        "COMPONENT_NAME", "MAIN", "ERR_TYPE", "register failure",
                        "ERR_MSG", "Register a audio observer failed!");
    }
    res = -1;
    auto audioSystemMgr = AudioStandard::AudioSystemManager::GetInstance();
    if (audioSystemMgr) {
        auto groupMgr = audioSystemMgr->GetGroupManager(AudioStandard::DEFAULT_VOLUME_GROUP_ID);
        if (groupMgr) {
            res = groupMgr->SetRingerModeCallback(pid_, audioObserver_);
        }
    }
    if (res != OPERATION_SUCCESS) {
        RESSCHED_LOGW("ObserverManager init audioRingModeObserver failed");
    }
    res = AudioStandard::AudioSystemManager::GetInstance()->RegisterVolumeKeyEventCallback(pid_, audioObserver_);
    if (res != OPERATION_SUCCESS) {
        RESSCHED_LOGW("ObserverManager init audioVolumeKeyObserver failed");
    }
    res = AudioStandard::AudioSystemManager::GetInstance()->SetAudioSceneChangeCallback(audioObserver_);
    if (res != OPERATION_SUCCESS) {
        RESSCHED_LOGW("ObserverManager init audioSceneKeyObserver failed");
    }
    AudioStandard::AudioRendererInfo rendererInfo = {};
    rendererInfo.streamUsage = AudioStandard::StreamUsage::STREAM_USAGE_MUSIC;
    res = AudioStandard::AudioRoutingManager::GetInstance()
        ->SetPreferredOutputDeviceChangeCallback(rendererInfo, audioObserver_);
    if (res != OPERATION_SUCCESS) {
        RESSCHED_LOGW("ObserverManager init audioOutputDeviceChangeObserver failed");
    }
    audioObserver_->Init();
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

    res = AudioStandard::AudioSystemManager::GetInstance()->UnsetAudioSceneChangeCallback();
    if (res == OPERATION_SUCCESS) {
        RESSCHED_LOGD("ObserverManager disable audioSceneKeyObserver successfully");
    } else {
        RESSCHED_LOGW("ObserverManager disable audioSceneKeyObserver failed");
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

#ifdef MMI_ENABLE
void ObserverManager::InitMMiEventObserver()
{
    std::shared_ptr<OHOS::MMI::KeyOption> keyOption = std::make_shared<OHOS::MMI::KeyOption>();
    keyOption = std::make_shared<OHOS::MMI::KeyOption>();
    keyOption->SetFinalKey(OHOS::MMI::KeyEvent::KEYCODE_POWER);
    keyOption->SetFinalKeyDown(true);
    keyOption->SetFinalKeyDownDuration(0);
    powerKeySubscribeId_ = MMI::InputManager::GetInstance()->SubscribeKeyEvent(keyOption,
        [](std::shared_ptr<OHOS::MMI::KeyEvent> keyEvent) {
        ResSchedMgr::GetInstance().ReportData(ResType::RES_TYPE_MMI_INPUT_POWER_KEY, keyEvent->GetKeyCode());
    });
    RESSCHED_LOGI("Subscribe power key event successfully.");
#ifdef RESOURCE_SCHEDULE_SERVICE_WITH_APP_NAP_ENABLE
    if (!isNeedReport_) {
        RESSCHED_LOGI("not need init mmi observer.");
        return;
    }
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
        return;
    }
    // Get all events registered in multimodal input.
    GetAllMmiStatusData();
#endif
}

void ObserverManager::DisableMMiEventObserver()
{
    RESSCHED_LOGI("Unsubscribes power key event");
    MMI::InputManager::GetInstance()->UnsubscribeKeyEvent(powerKeySubscribeId_);
#ifdef RESOURCE_SCHEDULE_SERVICE_WITH_APP_NAP_ENABLE
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
#endif
}

#ifdef RESOURCE_SCHEDULE_SERVICE_WITH_APP_NAP_ENABLE
void ObserverManager::GetAllMmiStatusData()
{
    RESSCHED_LOGI("get all mmi subscribed events.");
    std::map<std::tuple<int32_t, int32_t, std::string>, int32_t> mmiStatusData;
    MMI::InputManager::GetInstance()->GetAllMmiSubscribedEvents(mmiStatusData);
    if (mmiStatusData.empty()) {
        RESSCHED_LOGI("get mmi subscribed events is null.");
        return;
    }

    for (auto data = mmiStatusData.begin(); data != mmiStatusData.end(); ++data) {
        int32_t pid = std::get<TUPLE_PID>(data->first);
        int32_t uid = std::get<TUPLE_UID>(data->first);
        std::string bundleName = std::get<TUPLE_NAME>(data->first);
        int32_t status = data->second;
        RESSCHED_LOGD(
            "get mmi subscribed events, pid:%{public}d, uid:%{public}d, bundleName:%{public}s, status:%{public}d.",
            pid, uid, bundleName.c_str(), status);
        nlohmann::json payload;
        payload["pid"] = pid;
        payload["uid"] = uid;
        payload["bundleName"] = bundleName;
        payload["status"] = status;
        ResSchedMgr::GetInstance().ReportData(ResType::RES_TYPE_MMI_STATUS_CHANGE, 0, payload);
    }
}
#endif
#endif

void ObserverManager::InitDisplayModeObserver()
{
    RESSCHED_LOGI("ObserverManager Init display mode observer.");
    bool isFoldable = OHOS::Rosen::DisplayManager::GetInstance().IsFoldable();
    if (!isFoldable) {
        RESSCHED_LOGI("ObserverManager Init display mode observer return for foldable.");
        return;
    }

    if (!foldDisplayModeObserver_) {
        foldDisplayModeObserver_ = new (std::nothrow)FoldDisplayModeObserver();
        if (foldDisplayModeObserver_ == nullptr) {
            RESSCHED_LOGE("Failed to create fold ChangeListener due to no memory");
            return;
        }
    }

    auto ret = OHOS::Rosen::DisplayManager::GetInstance().RegisterDisplayModeListener(foldDisplayModeObserver_);
    if (ret == OHOS::Rosen::DMError::DM_OK) {
        RESSCHED_LOGI("ObserverManager init displayModeObserver successfully");
        auto displayMode = OHOS::Rosen::DisplayManager::GetInstance().GetFoldDisplayMode();
        foldDisplayModeObserver_->OnDisplayModeChanged(displayMode);
    } else {
        RESSCHED_LOGW("ObserverManager init displayModeObserver failed");
        foldDisplayModeObserver_ = nullptr;
        return;
    }
}

void ObserverManager::DisableDisplayModeObserver()
{
    RESSCHED_LOGI("ObserverManager Disable display mode observer.");
    if (!foldDisplayModeObserver_) {
        RESSCHED_LOGE("ObserverManager has been disable displayModeObserver");
        return;
    }

    auto ret = OHOS::Rosen::DisplayManager::GetInstance().UnregisterDisplayModeListener(foldDisplayModeObserver_);
    if (ret == OHOS::Rosen::DMError::DM_OK) {
        RESSCHED_LOGI("ObserverManager disable displayModeObserver successfully");
    } else {
        RESSCHED_LOGW("ObserverManager disable displayModeObserver failed");
        return;
    }
    foldDisplayModeObserver_ = nullptr;
}

void ObserverManager::InitDisplayOrientationObserver()
{
    RESSCHED_LOGI("ObserverManager Init display orientation observer.");
    bool isFoldable = OHOS::Rosen::DisplayManager::GetInstance().IsFoldable();
    if (!isFoldable) {
        RESSCHED_LOGI("ObserverManager Init display mode observer return for foldable.");
        return;
    }

    if (!foldDisplayOrientationObserver_) {
        foldDisplayOrientationObserver_ = new (std::nothrow)FoldDisplayOrientationObserver();
        if (foldDisplayOrientationObserver_ == nullptr) {
            RESSCHED_LOGE("Failed to create fold ChangeListener due to no memory");
            return;
        }
    }

    auto ret = OHOS::Rosen::DisplayManager::GetInstance().RegisterDisplayListener(foldDisplayOrientationObserver_);
    if (ret == OHOS::Rosen::DMError::DM_OK) {
        RESSCHED_LOGI("ObserverManager init displayOrientationObserver successfully");
    } else {
        RESSCHED_LOGW("ObserverManager init displayOrientationObserver failed");
        foldDisplayOrientationObserver_ = nullptr;
        return;
    }
}

void ObserverManager::DisableDisplayOrientationObserver()
{
    RESSCHED_LOGI("ObserverManager Disable display orientation observer.");
    if (!foldDisplayOrientationObserver_) {
        RESSCHED_LOGE("ObserverManager has been disable displayModeObserver");
        return;
    }

    auto ret = OHOS::Rosen::DisplayManager::GetInstance().UnregisterDisplayListener(foldDisplayOrientationObserver_);
    if (ret == OHOS::Rosen::DMError::DM_OK) {
        RESSCHED_LOGI("ObserverManager disable displayModeObserver successfully");
    } else {
        RESSCHED_LOGW("ObserverManager disable displayModeObserver failed");
        return;
    }
    foldDisplayOrientationObserver_ = nullptr;
}

void ObserverManager::InitConnectionSubscriber()
{
    if (connectionSubscriber_ == nullptr) {
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
    if (connectionSubscriber_ == nullptr) {
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

void ObserverManager::InitDataShareObserver()
{
    RESSCHED_LOGI("ObserverManager Init dataShare observer.");
    OOBEManager::GetInstance().StartListen();
}

void ObserverManager::DisableDataShareObserver()
{
    RESSCHED_LOGI("Disable dataShare observer.");
    OOBEManager::GetInstance().UnregisterObserver();
}
#ifdef RESSCHED_MULTIMEDIA_AV_SESSION_ENABLE
void ObserverManager::InitAVSessionStateChangeListener()
{
    if (avSessionStateListener_ == nullptr) {
        avSessionStateListener_ = std::make_shared<AvSessionStateListener>();
    }

    auto res = AVSession::AVSessionManager::GetInstance().RegisterSessionListenerForAllUsers(avSessionStateListener_);
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
#ifdef RESOURCE_REQUEST_REQUEST
void ObserverManager::InitDownloadUploadObserver()
{
    if (downLoadUploadObserver_ == nullptr) {
        downLoadUploadObserver_ = std::make_shared<DownLoadUploadObserver>();
    }

    auto res = OHOS::Request::SubscribeRunningTaskCount(downLoadUploadObserver_);
    if (res == OPERATION_SUCCESS) {
        RESSCHED_LOGI("ObserverManager init download Upload observer successfully");
    } else {
        RESSCHED_LOGW("ObserverManager init download Upload observer failed");
        HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::RSS, "INIT_FAULT", HiviewDFX::HiSysEvent::EventType::FAULT,
                        "COMPONENT_NAME", "MAIN",
                        "ERR_TYPE", "register failure",
                        "ERR_MSG", "Register a download Upload observer failed!");
    }
}

void ObserverManager::DisableDownloadUploadObserver()
{
    OHOS::Request::UnsubscribeRunningTaskCount(downLoadUploadObserver_);
    RESSCHED_LOGI("Disable download Upload observer");
    downLoadUploadObserver_ = nullptr;
}
#endif

void ObserverManager::InitAccountObserver()
{
    RESSCHED_LOGI("InitAccountObserver");
    if (!accountObserver_) {
        AccountSA::OsAccountSubscribeInfo osAccountSubscribeInfo;
        osAccountSubscribeInfo.SetOsAccountSubscribeType(AccountSA::OS_ACCOUNT_SUBSCRIBE_TYPE::ACTIVATING);
        osAccountSubscribeInfo.SetName("ResschdeAccountActivatingSubscriber");
        accountObserver_ = std::make_shared<AccountObserver>(osAccountSubscribeInfo);
    }
    if (!accountObserver_) {
        RESSCHED_LOGE("account observer make failed");
    }
    ErrCode errCode = AccountSA::OsAccountManager::SubscribeOsAccount(accountObserver_);
    if (errCode == ERR_OK) {
        RESSCHED_LOGI("account observer register success");
    } else {
        RESSCHED_LOGW("account observer register failed");
        HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::RSS, "INIT_FAULT", HiviewDFX::HiSysEvent::EventType::FAULT,
                        "COMPONENT_NAME", "MAIN",
                        "ERR_TYPE", "register failure",
                        "ERR_MSG", "Register a account observer failed!");
    }
}

void ObserverManager::InitWindowStateObserver()
{
    if (!windowStateObserver_) {
        windowStateObserver_ = new (std::nothrow)WindowStateObserver();
        if (windowStateObserver_) {
            if (OHOS::Rosen::WindowManager::GetInstance().
            RegisterFocusChangedListener(windowStateObserver_) != OHOS::Rosen::WMError::WM_OK) {
                HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::RSS, "INIT_FAULT",
                                HiviewDFX::HiSysEvent::EventType::FAULT,
                                "COMPONENT_NAME", "MAIN", "ERR_TYPE", "register failure",
                                "ERR_MSG", "Register a listener of window focus change failed.");
            }
        }
    }
    if (!windowVisibilityObserver_) {
        windowVisibilityObserver_ = new (std::nothrow)WindowVisibilityObserver();
        if (windowVisibilityObserver_) {
            if (OHOS::Rosen::WindowManager::GetInstance().
            RegisterVisibilityChangedListener(windowVisibilityObserver_) != OHOS::Rosen::WMError::WM_OK) {
                HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::RSS, "INIT_FAULT",
                                HiviewDFX::HiSysEvent::EventType::FAULT,
                                "COMPONENT_NAME", "MAIN", "ERR_TYPE", "register failure",
                                "ERR_MSG", "Register a listener of window visibility change failed.");
            }
        }
    }
    if (!windowDrawingContentObserver_) {
        windowDrawingContentObserver_ = new (std::nothrow)WindowDrawingContentObserver();
        if (windowDrawingContentObserver_) {
            if (OHOS::Rosen::WindowManager::GetInstance().
                RegisterDrawingContentChangedListener(windowDrawingContentObserver_) != OHOS::Rosen::WMError::WM_OK) {
                    HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::RSS, "INIT_FAULT",
                                    HiviewDFX::HiSysEvent::EventType::FAULT,
                                    "COMPONENT_NAME", "MAIN", "ERR_TYPE", "register failure",
                                    "ERR_MSG", "Register a listener of window draw content change failed.");
            }
        }
    }
    SubscribeWindowModeChange();
    SubscribePipChange();
    RESSCHED_LOGI("%{public}s success.", __func__);
}

void ObserverManager::SubscribeWindowModeChange()
{
    if (!windowModeObserver_) {
        windowModeObserver_ = new (std::nothrow)WindowModeObserver();
        if (windowModeObserver_) {
            if (OHOS::Rosen::WindowManager::GetInstance().
                RegisterWindowModeChangedListener(windowModeObserver_) != OHOS::Rosen::WMError::WM_OK) {
                    RESSCHED_LOGE("RegisterWindowModeChangedListener fail");
                    HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::RSS,
                                    "INIT_FAULT", HiviewDFX::HiSysEvent::EventType::FAULT,
                                    "COMPONENT_NAME", "MAIN", "ERR_TYPE", "register failure",
                                    "ERR_MSG", "Register a listener of window mode content change failed.");
            }
        }
    }
}

void ObserverManager::SubscribePipChange()
{
    if (!pipStateObserver_) {
        pipStateObserver_ = new (std::nothrow)PiPStateObserver();
        if (!pipStateObserver_) {
            RESSCHED_LOGI("new PiPStateObserver fail");
            return;
        }
        if (OHOS::Rosen::WindowManagerLite::GetInstance().
            RegisterPiPStateChangedListener(pipStateObserver_) != OHOS::Rosen::WMError::WM_OK) {
                RESSCHED_LOGE("RegisterPiPStateChangedListener fail");
                HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::RSS, "INIT_FAULT",
                    HiviewDFX::HiSysEvent::EventType::FAULT,
                    "COMPONENT_NAME", "MAIN", "ERR_TYPE", "register failure",
                    "ERR_MSG", "Register a listener of window pip content change failed.");
        } else {
            RESSCHED_LOGI("RegisterPiPStateChangedListener success");
        }
    } else {
        RESSCHED_LOGI("PiPStateObserver not null");
    }
}

void ObserverManager::DisableWindowStateObserver()
{
    if (windowStateObserver_) {
        // unregister windowStateObserver_
        OHOS::Rosen::WindowManager::GetInstance().UnregisterFocusChangedListener(windowStateObserver_);
        windowStateObserver_ = nullptr;
    }

    if (windowVisibilityObserver_) {
        OHOS::Rosen::WindowManager::GetInstance().UnregisterVisibilityChangedListener(windowVisibilityObserver_);
        windowVisibilityObserver_ = nullptr;
    }

    if (windowDrawingContentObserver_) {
        OHOS::Rosen::WindowManager::GetInstance().
            UnregisterDrawingContentChangedListener(windowDrawingContentObserver_);
        windowDrawingContentObserver_ = nullptr;
    }
    UnsubscribeWindowModeChange();
    UnsubscribePipChange();
}

void ObserverManager::UnsubscribeWindowModeChange()
{
    if (windowModeObserver_) {
        OHOS::Rosen::WindowManager::GetInstance().
            UnregisterWindowModeChangedListener(windowModeObserver_);
        windowModeObserver_ = nullptr;
    }
}

void ObserverManager::UnsubscribePipChange()
{
    if (pipStateObserver_) {
        OHOS::Rosen::WindowManagerLite::GetInstance().UnregisterPiPStateChangedListener(pipStateObserver_);
        pipStateObserver_ = nullptr;
    }
    RESSCHED_LOGI("UnsubscribePipchange success");
}

#ifdef CONFIG_BGTASK_MGR
bool ObserverManager::SubscribeBackgroundTask()
{
    if (isBgtaskSubscribed_) {
        return true;
    }
    if (!backgroundTaskObserver_) {
        backgroundTaskObserver_ = std::make_shared<BackgroundTaskObserver>();
    }
    int ret = BackgroundTaskMgrHelper::SubscribeBackgroundTask(*backgroundTaskObserver_);
    if (ret != 0) {
        CGS_LOGE("%{public}s failed, err:%{public}d.", __func__, ret);
        return false;
    }
    isBgtaskSubscribed_ = true;
    CGS_LOGI("%{public}s success.", __func__);
    return true;
}

void ObserverManager::UnsubscribeBackgroundTask()
{
    if (!isBgtaskSubscribed_ || !backgroundTaskObserver_) {
        return;
    }
    int32_t ret = BackgroundTaskMgrHelper::UnsubscribeBackgroundTask(*backgroundTaskObserver_);
    if (ret == 0) {
        CGS_LOGI("%{public}s success.", __func__);
    } else {
        CGS_LOGE("%{public}s failed. ret:%{public}d", __func__, ret);
    }
    isBgtaskSubscribed_ = false;
}
#endif

OHOS::sptr<OHOS::AppExecFwk::IAppMgr> GetAppManagerInstance()
{
    OHOS::sptr<OHOS::ISystemAbilityManager> systemAbilityManager =
        OHOS::SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (!systemAbilityManager) {
        RESSCHED_LOGE("%{public}s : systemAbilityManager nullptr!", __func__);
        return nullptr;
    }
    OHOS::sptr<OHOS::IRemoteObject> object = systemAbilityManager->GetSystemAbility(OHOS::APP_MGR_SERVICE_ID);
    return OHOS::iface_cast<OHOS::AppExecFwk::IAppMgr>(object);
}

void ObserverManager::SubscribeAppState()
{
    sptr<OHOS::AppExecFwk::IAppMgr> appManager = GetAppManagerInstance();
    if (!appManager) {
        RESSCHED_LOGE("%{public}s app manager nullptr!", __func__);
        return;
    }
    appStateObserver_ = new (std::nothrow)RmsApplicationStateObserver();
    if (!appStateObserver_) {
        RESSCHED_LOGE("%{public}s allocate app state observer failed!", __func__);
        return;
    }
    int32_t err = appManager->RegisterApplicationStateObserver(appStateObserver_);
    if (err != 0) {
        RESSCHED_LOGE("%{public}s register to appmanager failed. err:%{public}d", __func__, err);
        appStateObserver_ = nullptr;
        return;
    }
    RESSCHED_LOGI("%{public}s success.", __func__);
    return;
}

void ObserverManager::UnsubscribeAppState()
{
    if (!appStateObserver_) {
        return;
    }

    sptr<OHOS::AppExecFwk::IAppMgr> appManager = GetAppManagerInstance();
    if (appManager) {
        int32_t err = appManager->UnregisterApplicationStateObserver(appStateObserver_);
        if (err == 0) {
            RESSCHED_LOGI("%{public}s success.", __func__);
        } else {
            RESSCHED_LOGE("%{public}s failed. err:%{public}d", __func__, err);
        }
    }
    appStateObserver_ = nullptr;
}

void ObserverManager::DisableAccountObserver()
{
    RESSCHED_LOGI("account sa removed");
}

extern "C" void ObserverManagerInit()
{
    auto instance = ObserverManager::GetInstance();
    if (instance) {
        instance->Init();
    } else {
        RESSCHED_LOGE("ObserverManager GetInstance failed");
    }
}

extern "C" void ObserverManagerDisable()
{
    ObserverManager::GetInstance()->Disable();
}
} // namespace ResourceSchedule
} // namespace OHOS