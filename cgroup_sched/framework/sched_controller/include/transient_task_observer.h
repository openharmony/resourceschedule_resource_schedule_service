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

#ifndef TRANSIENT_TASK_OBSERVER_H
#define TRANSIENT_TASK_OBSERVER_H

#include <sys/types.h>
#include "transient_task_app_info.h"
#include "background_task_subscriber.h"

namespace OHOS {
namespace ResourceSchedule {
using OHOS::BackgroundTaskMgr::BackgroundTaskSubscriber;
using OHOS::BackgroundTaskMgr::TransientTaskAppInfo;

class TransientTaskObserver : public BackgroundTaskSubscriber {
public:
    TransientTaskObserver() {};
    ~TransientTaskObserver() {};

    void OnTransientTaskStart(const std::shared_ptr<TransientTaskAppInfo>& info);
    void OnTransientTaskEnd(const std::shared_ptr<TransientTaskAppInfo>& info);

private:
    void HandleTransientTaskStart(uid_t uid, pid_t pid, std::string packageName);
    void HandleTransientTaskEnd(uid_t uid, pid_t pid, std::string packageName);

    inline bool ValidateTransientTaskAppInfo(const std::shared_ptr<TransientTaskAppInfo>& info) const
    {
        return info->GetUid() > 0 && info->GetPid() >= 0
            && info->GetPackageName().size() > 0;
    }

    inline std::string PackPayload(const std::shared_ptr<TransientTaskAppInfo>& info) const
    {
        return std::to_string(info->GetPid()) + "," + std::to_string(info->GetUid()) + "," + info->GetPackageName();
    }
};

} // namespace ResourceSchedule
} // namespace OHOS
#endif // TRANSIENT_TASK_OBSERVER_H 
