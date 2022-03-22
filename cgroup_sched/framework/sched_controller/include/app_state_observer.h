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

#ifndef CGROUP_SCHED_FRAMEWORK_SCHED_CONTROLLER_INCLUDE_APP_STATE_OBSERVER_H_
#define CGROUP_SCHED_FRAMEWORK_SCHED_CONTROLLER_INCLUDE_APP_STATE_OBSERVER_H_

#include "application_state_observer_stub.h"

namespace OHOS {
namespace ResourceSchedule {
using OHOS::AppExecFwk::ApplicationStateObserverStub;
using OHOS::AppExecFwk::AppStateData;
using OHOS::AppExecFwk::AbilityStateData;
using OHOS::AppExecFwk::ProcessData;

class RmsApplicationStateObserver : public ApplicationStateObserverStub {
public:
    void OnForegroundApplicationChanged(const AppStateData &appStateData) override;
    void OnAbilityStateChanged(const AbilityStateData &abilityStateData) override;
    void OnExtensionStateChanged(const AbilityStateData &abilityStateData) override;
    void OnProcessCreated(const ProcessData &processData) override;
    void OnProcessDied(const ProcessData &processData) override;
    void OnApplicationStateChanged(const AppStateData &appStateData) override;

private:
    inline bool ValidateAppStateData(const AppStateData &appStateData) const
    {
        return appStateData.uid > 0
            && appStateData.bundleName.size() > 0;
    }

    inline bool ValidateAbilityStateData(const AbilityStateData &abilityStateData) const
    {
        return abilityStateData.uid > 0 && abilityStateData.pid >= 0
            && abilityStateData.bundleName.size() > 0
            && abilityStateData.abilityName.size() > 0
            && abilityStateData.token;
    }

    inline bool ValidateProcessData(const ProcessData &processData) const
    {
        return processData.uid > 0 && processData.pid >= 0
            && processData.bundleName.size() > 0;
    }
};
} // namespace ResourceSchedule
} // namespace OHOS
#endif // CGROUP_SCHED_FRAMEWORK_SCHED_CONTROLLER_INCLUDE_APP_STATE_OBSERVER_H_
