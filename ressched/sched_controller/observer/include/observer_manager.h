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
#include "account_observer.h"
#include "app_state_observer.h"
#include "if_system_ability_manager.h"
#include "system_ability_status_change_stub.h"
#include "singleton.h"
#ifdef RESSCHED_AUDIO_FRAMEWORK_ENABLE
#include "audio_observer.h"
#endif
#ifdef RESOURCE_SCHEDULE_SERVICE_WITH_APP_NAP_ENABLE
#include "hisysevent_observer.h"
#endif
#include "connection_subscriber.h"
#ifdef DEVICE_MOVEMENT_PERCEPTION_ENABLE
#include "device_movement_observer.h"
#endif
#ifdef RESSCHED_TELEPHONY_STATE_REGISTRY_ENABLE
#include "sched_telephony_observer.h"
#endif
#ifdef MMI_ENABLE
#ifdef RESOURCE_SCHEDULE_SERVICE_WITH_APP_NAP_ENABLE
#include "mmi_observer.h"
#endif
#endif
#ifdef RESSCHED_MULTIMEDIA_AV_SESSION_ENABLE
#include "av_session_state_listener.h"
#endif
#include "fold_display_mode_observer.h"
#ifdef RESOURCE_REQUEST_REQUEST
#include "download_upload_observer.h"
#endif
#include "window_state_observer.h"
#ifdef CONFIG_BGTASK_MGR
#include "background_task_observer.h"
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

#ifdef RESOURCE_SCHEDULE_SERVICE_WITH_APP_NAP_ENABLE
    void InitHiSysEventObserver();
    void DisableHiSysEventObserver();
#ifdef MMI_ENABLE
    void GetAllMmiStatusData();
#endif
#endif
#ifdef MMI_ENABLE
    void InitMMiEventObserver();
    void DisableMMiEventObserver();
#endif
    void InitTelephonyObserver();
    void DisableTelephonyObserver();
    void InitAudioObserver();
    void DisableAudioObserver();
    void InitDeviceMovementObserver();
    void DisableDeviceMovementObserver();
    void InitDisplayModeObserver();
    void DisableDisplayModeObserver();
    void InitDisplayOrientationObserver();
    void DisableDisplayOrientationObserver();
    void InitSysAbilityListener();
    void AddItemToSysAbilityListener(int32_t systemAbilityId, sptr<ISystemAbilityManager>& systemAbilityManager);
    void InitObserverCbMap();
    void InitRemoveObserverCbMap();
    void InitConnectionSubscriber();
    void DisableConnectionSubscriber();
    void GetReportFunc();
    void InitDataShareObserver();
    void DisableDataShareObserver();
    void InitAccountObserver();
    void DisableAccountObserver();
    void InitWindowStateObserver();
    void DisableWindowStateObserver();
    void SubscribeWindowModeChange();
    void SubscribePipChange();
    void UnsubscribeWindowModeChange();
    void UnsubscribePipChange();
    void SubscribeAppState();
    void UnsubscribeAppState();
#ifdef CONFIG_BGTASK_MGR
    void InitBackgroundTask();
    void DisableBackgroundTask();
#endif
#ifdef RESOURCE_REQUEST_REQUEST
    void InitDownloadUploadObserver();
    void DisableDownloadUploadObserver();
#endif

#ifdef RESSCHED_MULTIMEDIA_AV_SESSION_ENABLE
    void InitAVSessionStateChangeListener();
    void DisableAVSessionStateChangeListener();
    std::shared_ptr<AvSessionStateListener> avSessionStateListener_ = nullptr;
#endif

    pid_t pid_ = -1;
    std::map<int32_t, std::function<void()>> handleObserverMap_;
    std::map<int32_t, std::function<void()>> removeObserverMap_;
#ifdef RESOURCE_SCHEDULE_SERVICE_WITH_APP_NAP_ENABLE
    std::shared_ptr<HiviewDFX::HiSysEventListener> hiSysEventObserver_ = nullptr;
#ifdef MMI_ENABLE
    std::shared_ptr<MmiObserver> mmiEventObserver_ = nullptr;
#endif
#endif
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
    std::shared_ptr<ConnectionSubscriber> connectionSubscriber_ = nullptr;
    sptr<FoldDisplayModeObserver> foldDisplayModeObserver_ = nullptr;
    sptr<FoldDisplayOrientationObserver> foldDisplayOrientationObserver_ = nullptr;
#ifdef RESOURCE_REQUEST_REQUEST
    std::shared_ptr<DownLoadUploadObserver> downLoadUploadObserver_ = nullptr;
#endif
    std::shared_ptr<AccountObserver> accountObserver_ = nullptr;
    sptr<PiPStateObserver> pipStateObserver_ = nullptr;
    sptr<WindowStateObserver> windowStateObserver_ = nullptr;
    sptr<WindowVisibilityObserver> windowVisibilityObserver_ = nullptr;
    sptr<WindowDrawingContentObserver> windowDrawingContentObserver_ = nullptr;
    sptr<WindowModeObserver> windowModeObserver_ = nullptr;
    sptr<RmsApplicationStateObserver> appStateObserver_ = nullptr;
#ifdef CONFIG_BGTASK_MGR
    bool isBgtaskSubscribed_ {false};
    std::shared_ptr<BackgroundTaskObserver> backgroundTaskObserver_ = nullptr;
#endif
    bool isNeedReport_ = true;
    int32_t powerKeySubscribeId_ = -1;
};
} // namespace ResourceSchedule
} // namespace OHOS
#endif // RESSCHED_SCHED_CONTROLLER_OBSERVER_INCLUDE_OBSERVER_MANAGER_H
