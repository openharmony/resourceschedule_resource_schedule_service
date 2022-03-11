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

#include "background_task_observer.h"

#include "cgroup_sched_log.h"
#include "sched_controller.h"
#include "cgroup_event_handler.h"
#include "ressched_utils.h"
#include "res_type.h"

namespace OHOS {
namespace ResourceSchedule {
namespace {
    constexpr HiviewDFX::HiLogLabel LOG_LABEL = {LOG_CORE, LOG_TAG_DOMAIN_ID_RMS, "BackgroundTaskObserver"};
}

void BackgroundTaskObserver::OnConnected()
{
    CGS_LOGI("%{public}s.", __func__);
}

void BackgroundTaskObserver::OnDisconnected()
{
    CGS_LOGI("%{public}s.", __func__);
}

void BackgroundTaskObserver::OnTransientTaskStart(const std::shared_ptr<TransientTaskAppInfo>& info)
{
    if (!ValidateTaskInfo(info)) {
        return;
    }
    /* class TransientTaskAppInfo {std::string& GetPackageName(); int32_t GetUid(); int32_t GetPid();} */
    auto cgHander = SchedController::GetInstance().GetCgroupEventHandler();
    if (cgHander) {
        cgHander->PostTask([cgHander, info] {
            cgHander->HandleTransientTaskStart(info->GetUid(), info->GetPid(), info->GetPackageName());
        });
    }

    ResSchedUtils::GetInstance().ReportDataInProcess(ResType::RES_TYPE_TRANSIENT_TASK, 0, PackPayload(info));
}

void BackgroundTaskObserver::OnTransientTaskEnd(const std::shared_ptr<TransientTaskAppInfo>& info)
{
    if (!ValidateTaskInfo(info)) {
        CGS_LOGE("%{public}s failed, invalid app info!", __func__);
        return;
    }
    auto cgHander = SchedController::GetInstance().GetCgroupEventHandler();
    if (cgHander) {
        cgHander->PostTask([cgHander, info] {
            cgHander->HandleTransientTaskEnd(info->GetUid(), info->GetPid(), info->GetPackageName());
        });
    }

    ResSchedUtils::GetInstance().ReportDataInProcess(ResType::RES_TYPE_TRANSIENT_TASK, 1, PackPayload(info));
}

void BackgroundTaskObserver::OnContinuousTaskStart(
    const std::shared_ptr<ContinuousTaskCallbackInfo> &continuousTaskCallbackInfo)
{
    if (!ValidateTaskInfo(continuousTaskCallbackInfo)) {
        CGS_LOGE("%{public}s failed, invalid event data!", __func__);
        return;
    }
    auto cgHander = SchedController::GetInstance().GetCgroupEventHandler();
    if (cgHander) {
        cgHander->PostTask([cgHander, continuousTaskCallbackInfo] {
            cgHander->HandleContinuousTaskStart(continuousTaskCallbackInfo->GetCreatorUid(),
                continuousTaskCallbackInfo->GetCreatorPid(),
                continuousTaskCallbackInfo->GetAbilityName());
        });
    }

    ResSchedUtils::GetInstance().ReportDataInProcess(ResType::RES_TYPE_CONTINUOUS_TASK, 0,
        PackPayload(continuousTaskCallbackInfo));
}

void BackgroundTaskObserver::OnContinuousTaskStop(
    const std::shared_ptr<ContinuousTaskCallbackInfo> &continuousTaskCallbackInfo)
{
    if (!ValidateTaskInfo(continuousTaskCallbackInfo)) {
        CGS_LOGE("%{public}s failed, invalid event data!", __func__);
        return;
    }
    auto cgHander = SchedController::GetInstance().GetCgroupEventHandler();
    if (cgHander) {
        cgHander->PostTask([cgHander, continuousTaskCallbackInfo] {
            cgHander->HandleContinuousTaskCancel(continuousTaskCallbackInfo->GetCreatorUid(),
                continuousTaskCallbackInfo->GetCreatorPid(),
                continuousTaskCallbackInfo->GetAbilityName());
        });
    }

    ResSchedUtils::GetInstance().ReportDataInProcess(ResType::RES_TYPE_CONTINUOUS_TASK, 0,
        PackPayload(continuousTaskCallbackInfo));
}

void BackgroundTaskObserver::OnRemoteDied(const wptr<IRemoteObject> &object)
{
    CGS_LOGI("%{public}s.", __func__);
}
} // namespace ResourceSchedule
} // namespace OHOS
