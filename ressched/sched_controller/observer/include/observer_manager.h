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

#ifndef RESSCHED_SCHED_CONTROLLER_OBSERVER_INCLUDE_OBSERVER_MANAGER_H
#define RESSCHED_SCHED_CONTROLLER_OBSERVER_INCLUDE_OBSERVER_MANAGER_H

#include "if_system_ability_manager.h"
#include "system_ability_status_change_stub.h"
#include "single_instance.h"

#include "audio_observer.h"
#include "camera_observer.h"
#ifdef DEVICE_MOVEMENT_PERCEPTION_ENABLE
#include "device_movement_observer.h"
#endif
#include "sched_telephony_observer.h"

namespace OHOS {
namespace ResourceSchedule {
class ObserverManager {
    DECLARE_SINGLE_INSTANCE_BASE(ObserverManager);

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

    void InitCameraObserver();
    void DisableCameraObserver();
    void InitTelephonyObserver();
    void DisableTelephonyObserver();
    void InitAudioObserver();
    void DisableAudioObserver();
    void InitDeviceMovementObserver();
    void DisableDeviceMovementObserver();
    void InitSysAbilityListener();
    void AddItemToSysAbilityListener(int32_t systemAbilityId, sptr<ISystemAbilityManager>& systemAbilityManager);

    int32_t slotId_ = 0;
    pid_t pid_ = -1;
    std::map<int32_t, std::function<void(ObserverManager *)>> handleObserverMap_;
    std::map<int32_t, std::function<void(ObserverManager *)>> removeObserverMap_;
    std::shared_ptr<HiviewDFX::HiSysEventListener> cameraObserver_ = nullptr;
    sptr<SchedTelephonyObserver> telephonyObserver_ = nullptr;
    std::shared_ptr<AudioObserver> audioObserver_ = nullptr;
#ifdef DEVICE_MOVEMENT_PERCEPTION_ENABLE
    sptr<DeviceMovementObserver> deviceMovementObserver_ = nullptr;
#endif
    sptr<SystemAbilityStatusChangeListener> sysAbilityListener_ = nullptr;
};
} // namespace ResourceSchedule
} // namespace OHOS
#endif // RESSCHED_SCHED_CONTROLLER_OBSERVER_INCLUDE_OBSERVER_MANAGER_H
