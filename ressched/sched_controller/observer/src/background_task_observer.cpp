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

#include "background_task_observer.h"

#ifdef CONFIG_BGTASK_MGR
//#include "cgroup_sched_log.h"
//#include "sched_controller.h"
//#include "cgroup_event_handler.h"
//#include "ressched_utils.h"
#include "res_type.h"

#undef LOG_TAG
#define LOG_TAG "BackgroundTaskObserver"

namespace OHOS {
namespace ResourceSchedule {
void BackgroundTaskObserver::MarshallingTransientTaskAppInfo(
    const std::shared_ptr<TransientTaskAppInfo>& info, nlohmann::json& payload)
{
    payload["pid"] = std::to_string(info->GetPid());
    payload["uid"] = std::to_string(info->GetUid());
    payload["bundleName"] = info->GetPackageName();
}

void BackgroundTaskObserver::OnTransientTaskStart(const std::shared_ptr<TransientTaskAppInfo>& info)
{
    if (!ValidateTaskInfo(info)) {
        return;
    }

    nlohmann::json payload;
    MarshallingTransientTaskAppInfo(info, payload);
    ResSchedUtils::GetInstance().ReportDataInProcess(
        ResType::RES_TYPE_TRANSIENT_TASK, ResType::TransientTaskStatus::TRANSIENT_TASK_START, payload);
}

void BackgroundTaskObserver::OnTransientTaskEnd(const std::shared_ptr<TransientTaskAppInfo>& info)
{
    if (!ValidateTaskInfo(info)) {
        RESSCHED_LOGE("%{public}s failed, invalid app info!", __func__);
        return;
    }

    nlohmann::json payload;
    MarshallingTransientTaskAppInfo(info, payload);
    ResSchedUtils::GetInstance().ReportDataInProcess(
        ResType::RES_TYPE_TRANSIENT_TASK, ResType::TransientTaskStatus::TRANSIENT_TASK_END, payload);
}

void BackgroundTaskObserver::OnTransientTaskErr(const std::shared_ptr<TransientTaskAppInfo>& info)
{
    if (!ValidateTaskInfo(info)) {
        RESSCHED_LOGE("%{public}s failed, invalid app info!", __func__);
        return;
    }

    nlohmann::json payload;
    MarshallingTransientTaskAppInfo(info, payload);
    ResSchedUtils::GetInstance().ReportDataInProcess(
        ResType::RES_TYPE_TRANSIENT_TASK, ResType::TransientTaskStatus::TRANSIENT_TASK_ERR, payload);
}

void BackgroundTaskObserver::OnAppTransientTaskStart(const std::shared_ptr<TransientTaskAppInfo>& info)
{
    nlohmann::json payload;
    MarshallingTransientTaskAppInfo(info, payload);
    ResSchedUtils::GetInstance().ReportDataInProcess(
        ResType::RES_TYPE_TRANSIENT_TASK, ResType::TransientTaskStatus::APP_TRANSIENT_TASK_START, payload);
}

void BackgroundTaskObserver::OnAppTransientTaskEnd(const std::shared_ptr<TransientTaskAppInfo>& info)
{
    nlohmann::json payload;
    MarshallingTransientTaskAppInfo(info, payload);
    ResSchedUtils::GetInstance().ReportDataInProcess(
        ResType::RES_TYPE_TRANSIENT_TASK, ResType::TransientTaskStatus::APP_TRANSIENT_TASK_END, payload);
}

void BackgroundTaskObserver::MarshallingContinuousTaskCallbackInfo(
    const std::shared_ptr<ContinuousTaskCallbackInfo>& continuousTaskCallbackInfo, nlohmann::json& payload)
{
    payload["pid"] = std::to_string(continuousTaskCallbackInfo->GetCreatorPid());
    payload["uid"] = std::to_string(continuousTaskCallbackInfo->GetCreatorUid());
    payload["abilityName"] = continuousTaskCallbackInfo->GetAbilityName();
    payload["isBatchApi"] =  std::to_string(continuousTaskCallbackInfo->IsBatchApi());
    payload["typeId"] = std::to_string(continuousTaskCallbackInfo->GetTypeId());
    payload["abilityId"] = std::to_string(continuousTaskCallbackInfo->GetAbilityId());
    payload["isFromWebview"] = continuousTaskCallbackInfo->IsFromWebview();
    payload["typeIds"] = continuousTaskCallbackInfo->GetTypeIds();
    payload["tokenId"] = continuousTaskCallbackInfo->GetTokenId();
}

void BackgroundTaskObserver::OnContinuousTaskStart(
    const std::shared_ptr<ContinuousTaskCallbackInfo> &continuousTaskCallbackInfo)
{
    if (!ValidateTaskInfo(continuousTaskCallbackInfo)) {
        RESSCHED_LOGE("%{public}s failed, invalid event data!", __func__);
        return;
    }

    nlohmann::json payload;
    MarshallingContinuousTaskCallbackInfo(continuousTaskCallbackInfo, payload);
    ResSchedUtils::GetInstance().ReportDataInProcess(
        ResType::RES_TYPE_CONTINUOUS_TASK, ResType::ContinuousTaskStatus::CONTINUOUS_TASK_START, payload);
}

void BackgroundTaskObserver::OnContinuousTaskStop(
    const std::shared_ptr<ContinuousTaskCallbackInfo> &continuousTaskCallbackInfo)
{
    if (!ValidateTaskInfo(continuousTaskCallbackInfo)) {
        RESSCHED_LOGE("%{public}s failed, invalid event data!", __func__);
        return;
    }

    nlohmann::json payload;
    MarshallingContinuousTaskCallbackInfo(continuousTaskCallbackInfo, payload);
    ResSchedUtils::GetInstance().ReportDataInProcess(
        ResType::RES_TYPE_CONTINUOUS_TASK, ResType::ContinuousTaskStatus::CONTINUOUS_TASK_END, payload);
}

void BackgroundTaskObserver::OnContinuousTaskUpdate(
    const std::shared_ptr<ContinuousTaskCallbackInfo> &continuousTaskCallbackInfo)
{
    if (!ValidateTaskInfo(continuousTaskCallbackInfo)) {
        RESSCHED_LOGE("%{public}s failed, invalid event data!", __func__);
        return;
    }

    nlohmann::json payload;
    MarshallingContinuousTaskCallbackInfo(continuousTaskCallbackInfo, payload);
    ResSchedUtils::GetInstance().ReportDataInProcess(
        ResType::RES_TYPE_CONTINUOUS_TASK, ResType::ContinuousTaskStatus::CONTINUOUS_TASK_UPDATE, payload);
}

void BackgroundTaskObserver::OnRemoteDied(const wptr<IRemoteObject> &object)
{
    RESSCHED_LOGI("%{public}s.", __func__);
}

void BackgroundTaskObserver::MarshallingResourceInfo(
    const std::shared_ptr<BackgroundTaskMgr::ResourceCallbackInfo> &resourceInfo, nlohmann::json &payload)
{
    if (!resourceInfo) {
        RESSCHED_LOGE("%{public}s : resourceInfo nullptr!", __func__);
        return;
    }
    payload["pid"] = resourceInfo->GetPid();
    payload["uid"] = resourceInfo->GetUid();
    payload["resourceNumber"] = resourceInfo->GetResourceNumber();
    payload["bundleName"] = resourceInfo->GetBundleName();
}

void BackgroundTaskObserver::OnAppEfficiencyResourcesApply(
    const std::shared_ptr<BackgroundTaskMgr::ResourceCallbackInfo> &resourceInfo)
{
    nlohmann::json payload;
    MarshallingResourceInfo(resourceInfo, payload);
    ResSchedUtils::GetInstance().ReportDataInProcess(
        ResType::RES_TYPE_EFFICIENCY_RESOURCES_STATE_CHANGED,
        ResType::EfficiencyResourcesStatus::APP_EFFICIENCY_RESOURCES_APPLY,
        payload);
}

void BackgroundTaskObserver::OnAppEfficiencyResourcesReset(
    const std::shared_ptr<BackgroundTaskMgr::ResourceCallbackInfo> &resourceInfo)
{
    nlohmann::json payload;
    MarshallingResourceInfo(resourceInfo, payload);
    ResSchedUtils::GetInstance().ReportDataInProcess(
        ResType::RES_TYPE_EFFICIENCY_RESOURCES_STATE_CHANGED,
        ResType::EfficiencyResourcesStatus::APP_EFFICIENCY_RESOURCES_RESET,
        payload);
}

void BackgroundTaskObserver::OnProcEfficiencyResourcesApply(
    const std::shared_ptr<BackgroundTaskMgr::ResourceCallbackInfo> &resourceInfo)
{
    nlohmann::json payload;
    MarshallingResourceInfo(resourceInfo, payload);
    ResSchedUtils::GetInstance().ReportDataInProcess(
        ResType::RES_TYPE_EFFICIENCY_RESOURCES_STATE_CHANGED,
        ResType::EfficiencyResourcesStatus::PROC_EFFICIENCY_RESOURCES_APPLY,
        payload);
}

void BackgroundTaskObserver::OnProcEfficiencyResourcesReset(
    const std::shared_ptr<BackgroundTaskMgr::ResourceCallbackInfo> &resourceInfo)
{
    nlohmann::json payload;
    MarshallingResourceInfo(resourceInfo, payload);
    ResSchedUtils::GetInstance().ReportDataInProcess(
        ResType::RES_TYPE_EFFICIENCY_RESOURCES_STATE_CHANGED,
        ResType::EfficiencyResourcesStatus::PROC_EFFICIENCY_RESOURCES_RESET,
        payload);
}
} // namespace ResourceSchedule
} // namespace OHOS
#endif
