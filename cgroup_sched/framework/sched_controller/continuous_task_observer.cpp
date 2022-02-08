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

#include "continuous_task_observer.h"

#include "cgroup_sched_log.h"
#include "sched_controller.h"
#include "cgroup_event_handler.h"
#include "ressched_utils.h"
#include "res_type.h"

namespace OHOS {
namespace ResourceSchedule {
namespace {
    constexpr HiviewDFX::HiLogLabel LOG_LABEL = {LOG_CORE, LOG_TAG_DOMAIN_ID_RMS, "ContinuousTaskObserver"};
}

void ContinuousTaskObserver::OnSubscribeResult(ContinuousTaskConstant::SubscribeResult result)
{
    CGS_LOGI("%{public}s result:%{public}d!", __func__, result);
}

void ContinuousTaskObserver::OnUnsubscribeResult(ContinuousTaskConstant::SubscribeResult result)
{
    CGS_LOGI("%{public}s result:%{public}d!", __func__, result);
}

void ContinuousTaskObserver::OnContinuousTaskStart(const std::shared_ptr<ContinuousTaskEventData> &eventData)
{
    if (!ValidateTransientTaskAppInfo(eventData)) {
        CGS_LOGE("%{public}s failed, invalid event data!", __func__);
        return;
    }
    auto cgHander = SchedController::GetInstance().GetCgroupEventHandler();
    if (cgHander != nullptr) {
        cgHander->PostTask([cgHander, eventData] {
            cgHander->HandleContinuousTaskStart(eventData->GetCreatorUid(), eventData->GetCreatorPid(),
                eventData->GetAbilityName());
        });
    }

    ResSchedUtils::GetInstance().ReportDataInProcess(ResType::RES_TYPE_CONTINUOUS_TASK, 0, PackPayload(eventData));
}

void ContinuousTaskObserver::OnContinuousTaskCancel(const std::shared_ptr<ContinuousTaskEventData> &eventData)
{
    if (!ValidateTransientTaskAppInfo(eventData)) {
        CGS_LOGE("%{public}s failed, invalid event data!", __func__);
        return;
    }
    auto cgHander = SchedController::GetInstance().GetCgroupEventHandler();
    if (cgHander != nullptr) {
        cgHander->PostTask([cgHander, eventData] {
            cgHander->HandleContinuousTaskCancel(eventData->GetCreatorUid(), eventData->GetCreatorPid(),
                eventData->GetAbilityName());
        });
    }

    ResSchedUtils::GetInstance().ReportDataInProcess(ResType::RES_TYPE_CONTINUOUS_TASK, 0, PackPayload(eventData));
}

void ContinuousTaskObserver::OnDied()
{
}
} // namespace ResourceSchedule
} // namespace OHOS
