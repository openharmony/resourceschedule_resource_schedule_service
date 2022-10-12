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

#ifndef CGROUP_SCHED_FRAMEWORK_SCHED_CONTROLLER_INCLUDE_BACKGROUND_TASK_OBSERVER_H_
#define CGROUP_SCHED_FRAMEWORK_SCHED_CONTROLLER_INCLUDE_BACKGROUND_TASK_OBSERVER_H_

#ifdef CONFIG_BGTASK_MGR
#include <sys/types.h>
#include "background_mode.h"
#include "continuous_task_callback_info.h"
#include "transient_task_app_info.h"
#include "background_task_subscriber.h"

namespace OHOS {
namespace ResourceSchedule {
using OHOS::BackgroundTaskMgr::BackgroundTaskSubscriber;
using OHOS::BackgroundTaskMgr::ContinuousTaskCallbackInfo;
using OHOS::BackgroundTaskMgr::TransientTaskAppInfo;

class BackgroundTaskObserver : public BackgroundTaskSubscriber {
public:
    BackgroundTaskObserver() {}
    ~BackgroundTaskObserver() {}

    void OnConnected() override;
    void OnDisconnected() override;
    void OnTransientTaskStart(const std::shared_ptr<TransientTaskAppInfo>& info) override;
    void OnTransientTaskEnd(const std::shared_ptr<TransientTaskAppInfo>& info) override;
    void OnContinuousTaskStart(const std::shared_ptr<ContinuousTaskCallbackInfo> &continuousTaskCallbackInfo) override;
    void OnContinuousTaskStop(const std::shared_ptr<ContinuousTaskCallbackInfo> &continuousTaskCallbackInfo) override;
    void OnRemoteDied(const wptr<IRemoteObject> &object) override;

private:
    inline bool ValidateTaskInfo(const std::shared_ptr<TransientTaskAppInfo>& info) const
    {
        return info->GetUid() > 0 && info->GetPid() >= 0
            && info->GetPackageName().size() > 0;
    }

    inline bool ValidateTaskInfo(const std::shared_ptr<ContinuousTaskCallbackInfo>& eventData) const
    {
        return eventData->GetCreatorUid() > 0 && eventData->GetCreatorPid() >= 0
            && eventData->GetAbilityName().size() > 0 && eventData->GetTypeId() > 0
            && eventData->GetTypeId() <= BackgroundTaskMgr::BackgroundMode::TASK_KEEPING;
    }

    inline std::string PackPayload(const std::shared_ptr<TransientTaskAppInfo>& info) const
    {
        return std::to_string(info->GetPid()) + "," + std::to_string(info->GetUid()) + "," + info->GetPackageName();
    }

    inline std::string PackPayload(const std::shared_ptr<ContinuousTaskCallbackInfo>& eventData) const
    {
        return std::to_string(eventData->GetCreatorPid()) + "," +
               std::to_string(eventData->GetCreatorUid()) + "," +
               eventData->GetAbilityName();
    }
};
} // namespace ResourceSchedule
} // namespace OHOS
#endif
#endif // CGROUP_SCHED_FRAMEWORK_SCHED_CONTROLLER_INCLUDE_BACKGROUND_TASK_OBSERVER_H_
