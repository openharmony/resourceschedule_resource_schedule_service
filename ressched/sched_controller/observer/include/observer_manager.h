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

#include "camera_observer.h"
#include "schd_telephony_observer.h"
#include "single_instance.h"
#include "system_ability_status_change_stub.h"

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
    void InitSysAbilityListener();

    std::shared_ptr<HiviewDFX::HiSysEventSubscribeCallBack> cameraObserver_ = nullptr;
    int32_t slotId_;
    sptr<schdTelephonyObserver> telephonyObserver_ = nullptr;
    sptr<SystemAbilityStatusChangeListener> sysAbilityListener_ = nullptr;
};
} // namespace ResourceSchedule
} // namespace OHOS
#endif // RESSCHED_SCHED_CONTROLLER_OBSERVER_INCLUDE_OBSERVER_MANAGER_H
