/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef CONTINUOUS_TASK_OBSERVER_H
#define CONTINUOUS_TASK_OBSERVER_H

#include "../../../dummy_adaption/background_task_mgr/continuous_task_dummy.h"

namespace OHOS {
namespace ResourceSchedule {
using OHOS::BackgroundTaskMgr::ContinuousTaskEventSubscriber;
using OHOS::BackgroundTaskMgr::ContinuousTaskConstant;
using OHOS::BackgroundTaskMgr::ContinuousTaskEventData;

class ContinuousTaskObserver : public ContinuousTaskEventSubscriber {
public:
    ContinuousTaskObserver() {};
    ~ContinuousTaskObserver() {};

    void OnSubscribeResult(ContinuousTaskConstant::SubscribeResult result);
    void OnUnsubscribeResult(ContinuousTaskConstant::SubscribeResult result);
    void OnContinuousTaskStart(const std::shared_ptr<ContinuousTaskEventData> &eventData);
    void OnContinuousTaskCancel(const std::shared_ptr<ContinuousTaskEventData> &eventData);
    void OnDied();

private:
    void HandleContinuousTaskStart(uid_t uid, pid_t pid, std::string abilityName);
    void HandleContinuousTaskCancel(uid_t uid, pid_t pid, std::string abilityName);

    inline bool ValidateTransientTaskAppInfo(const std::shared_ptr<ContinuousTaskEventData>& eventData) const
    {
        return eventData->GetCreatorUid() > 0 && eventData->GetCreatorPid() >= 0
            && eventData->GetAbilityName().size() > 0;
    }

    inline std::string PackPayload(const std::shared_ptr<ContinuousTaskEventData>& eventData) const
    {
        return std::to_string(eventData->GetCreatorPid()) + "," +
               std::to_string(eventData->GetCreatorUid()) + "," +
               eventData->GetAbilityName();
    }
};
} // namespace ResourceSchedule
} // namespace OHOS
#endif // CONTINUOUS_TASK_OBSERVER_H
