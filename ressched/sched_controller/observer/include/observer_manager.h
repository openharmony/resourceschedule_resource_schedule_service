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

#ifndef RESSCHED_SCHED_CONTROLLER_OBSERVER_INCLUDE_OBSERVER_MANAGER_H
#define RESSCHED_SCHED_CONTROLLER_OBSERVER_INCLUDE_OBSERVER_MANAGER_H

#include <memory>
#include "if_system_ability_manager.h"
#include "system_ability_status_change_stub.h"
#include "singleton.h"
#ifdef RESSCHED_AUDIO_FRAMEWORK_ENABLE
#include "audio_observer.h"
#endif
#include "hisysevent_observer.h"
#include "connection_subscriber.h"
#ifdef DEVICE_MOVEMENT_PERCEPTION_ENABLE
#include "device_movement_observer.h"
#endif
#ifdef RESSCHED_TELEPHONY_STATE_REGISTRY_ENABLE
#include "sched_telephony_observer.h"
#endif
#include "mmi_observer.h"
#ifdef RESSCHED_MULTIMEDIA_AV_SESSION_ENABLE
#include "av_session_state_listener.h"
#endif
#include "fold_display_mode_observer.h"
#ifndef RESOURCE_REQUEST_REQUEST
#include "download_upload_observer.h"
#endif

namespace OHOS {
namespace ResourceSchedule {
using ReportFunc = bool (*)();
class ObserverManager : public DelayedSingleton<ObserverManager>,
    public std::enable_shared_from_this<ObserverManager> {
public:
    ObserverManager() {}
    ~ObserverManager() {}
    void Init();
    void Disable();

private:
class SystemAbilityStatusChangeListener : public OHOS::SystemAbilityStatusChangeStub {
public:
    SystemAbilityStatusChangeListener() {};
    virtual void OnAddSystemAbility(int32_t systemAbilityId, const std::string& deviceId) override;
    virtual void OnRemoveSystemAbility(int32_t systemAbilityId, const std::string& deviceId) override;
};

    void InitHiSysEventObserver();
    void DisableHiSysEventObserver();
    void InitTelephonyObserver();
    void DisableTelephonyObserver();
    void InitAudioObserver();
    void DisableAudioObserver();
    void InitDeviceMovementObserver();
    void DisableDeviceMovementObserver();
    void InitMMiEventObserver();
    void DisableMMiEventObserver();
    void InitDisplayModeObserver();
    void DisableDisplayModeObserver();
    void InitSysAbilityListener();
    void AddItemToSysAbilityListener(int32_t systemAbilityId, sptr<ISystemAbilityManager>& systemAbilityManager);
    void GetAllMmiStatusData();
    void InitObserverCbMap();
    void InitConnectionSubscriber();
    void DisableConnectionSubscriber();
    void GetReportFunc();
    void InitDataShareObserver();
    void DisableDataShareObserver();
#ifndef RESOURCE_REQUEST_REQUEST
    void InitDownloadUploadObserver();
    void DisableDownloadUploadObserver();
#endif

#ifdef RESSCHED_MULTIMEDIA_AV_SESSION_ENABLE
    void InitAVSessionStateChangeListener();
    void DisableAVSessionStateChangeListener();
    std::shared_ptr<AvSessionStateListener> avSessionStateListener_ = nullptr;
#endif

    pid_t pid_ = -1;
    std::map<int32_t, std::function<void(std::shared_ptr<ObserverManager>)>> handleObserverMap_;
    std::map<int32_t, std::function<void(std::shared_ptr<ObserverManager>)>> removeObserverMap_;
    std::shared_ptr<HiviewDFX::HiSysEventListener> hiSysEventObserver_ = nullptr;
#ifdef RESSCHED_TELEPHONY_STATE_REGISTRY_ENABLE
    int32_t slotId_ = 0;
    sptr<SchedTelephonyObserver> telephonyObserver_ = nullptr;
#endif
#ifdef RESSCHED_AUDIO_FRAMEWORK_ENABLE
    std::shared_ptr<AudioObserver> audioObserver_ = nullptr;
#endif
#ifdef DEVICE_MOVEMENT_PERCEPTION_ENABLE
    sptr<DeviceMovementObserver> deviceMovementObserver_ = nullptr;
#endif
    sptr<SystemAbilityStatusChangeListener> sysAbilityListener_ = nullptr;
    std::shared_ptr<MmiObserver> mmiEventObserver_ = nullptr;
    std::shared_ptr<ConnectionSubscriber> connectionSubscriber_ = nullptr;
    sptr<FoldDisplayModeObserver> foldDisplayModeObserver_ = nullptr;
#ifndef RESOURCE_REQUEST_REQUEST
    std::shared_ptr<DownLoadUploadObserver> downLoadUploadObserver_ = nullptr;
#endif
    bool isNeedReport_ = true;
};
} // namespace ResourceSchedule
} // namespace OHOS
#endif // RESSCHED_SCHED_CONTROLLER_OBSERVER_INCLUDE_OBSERVER_MANAGER_H
