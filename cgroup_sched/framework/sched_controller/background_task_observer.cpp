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
    /* class TransientTaskAppInfo {std::string& GetPackageName(); int32_t GetUid(); int32_t GetPid();} */
    auto cgHandler = SchedController::GetInstance().GetCgroupEventHandler();
    if (cgHandler) {
        auto uid = info->GetUid();
        auto pid = info->GetPid();
        auto pkgName = info->GetPackageName();

        cgHandler->PostTask([cgHandler, uid, pid, pkgName] {
            cgHandler->HandleTransientTaskStart(uid, pid, pkgName);
        });
    }

    nlohmann::json payload;
    MarshallingTransientTaskAppInfo(info, payload);
    ResSchedUtils::GetInstance().ReportDataInProcess(
        ResType::RES_TYPE_TRANSIENT_TASK, ResType::TransientTaskStatus::TRANSIENT_TASK_START, payload);
}

void BackgroundTaskObserver::OnTransientTaskEnd(const std::shared_ptr<TransientTaskAppInfo>& info)
{
    if (!ValidateTaskInfo(info)) {
        CGS_LOGE("%{public}s failed, invalid app info!", __func__);
        return;
    }
    auto cgHandler = SchedController::GetInstance().GetCgroupEventHandler();
    if (cgHandler) {
        auto uid = info->GetUid();
        auto pid = info->GetPid();
        auto pkgName = info->GetPackageName();

        cgHandler->PostTask([cgHandler, uid, pid, pkgName] {
            cgHandler->HandleTransientTaskEnd(uid, pid, pkgName);
        });
    }

    nlohmann::json payload;
    MarshallingTransientTaskAppInfo(info, payload);
    ResSchedUtils::GetInstance().ReportDataInProcess(
        ResType::RES_TYPE_TRANSIENT_TASK, ResType::TransientTaskStatus::TRANSIENT_TASK_END, payload);
}

void BackgroundTaskObserver::MarshallingContinuousTaskCallbackInfo(
    const std::shared_ptr<ContinuousTaskCallbackInfo>& continuousTaskCallbackInfo, nlohmann::json& payload)
{
    payload["pid"] = std::to_string(continuousTaskCallbackInfo->GetCreatorPid());
    payload["uid"] = std::to_string(continuousTaskCallbackInfo->GetCreatorUid());
    payload["abilityName"] = continuousTaskCallbackInfo->GetAbilityName();
    payload["typeId"] = std::to_string(continuousTaskCallbackInfo->GetTypeId());
    payload["isFromWebview"] = continuousTaskCallbackInfo->IsFromWebview();
}

void BackgroundTaskObserver::OnContinuousTaskStart(
    const std::shared_ptr<ContinuousTaskCallbackInfo> &continuousTaskCallbackInfo)
{
    if (!ValidateTaskInfo(continuousTaskCallbackInfo)) {
        CGS_LOGE("%{public}s failed, invalid event data!", __func__);
        return;
    }
    auto cgHandler = SchedController::GetInstance().GetCgroupEventHandler();
    if (cgHandler) {
        auto uid = continuousTaskCallbackInfo->GetCreatorUid();
        auto pid = continuousTaskCallbackInfo->GetCreatorPid();
        auto typeId = continuousTaskCallbackInfo->GetTypeId();
        auto abilityName = continuousTaskCallbackInfo->GetAbilityName();

        cgHandler->PostTask([cgHandler, uid, pid, typeId, abilityName] {
            cgHandler->HandleContinuousTaskStart(uid, pid, typeId, abilityName);
        });
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
        CGS_LOGE("%{public}s failed, invalid event data!", __func__);
        return;
    }
    auto cgHandler = SchedController::GetInstance().GetCgroupEventHandler();
    if (cgHandler) {
        auto uid = continuousTaskCallbackInfo->GetCreatorUid();
        auto pid = continuousTaskCallbackInfo->GetCreatorPid();
        auto typeId = continuousTaskCallbackInfo->GetTypeId();
        auto abilityName = continuousTaskCallbackInfo->GetAbilityName();

        cgHandler->PostTask([cgHandler, uid, pid, typeId, abilityName] {
            cgHandler->HandleContinuousTaskCancel(uid, pid, typeId, abilityName);
        });
    }

    nlohmann::json payload;
    MarshallingContinuousTaskCallbackInfo(continuousTaskCallbackInfo, payload);
    ResSchedUtils::GetInstance().ReportDataInProcess(
        ResType::RES_TYPE_CONTINUOUS_TASK, ResType::ContinuousTaskStatus::CONTINUOUS_TASK_END, payload);
}

void BackgroundTaskObserver::OnRemoteDied(const wptr<IRemoteObject> &object)
{
    CGS_LOGI("%{public}s.", __func__);
}

void BackgroundTaskObserver::MarshallingResourceInfo(
    const std::shared_ptr<BackgroundTaskMgr::ResourceCallbackInfo> &resourceInfo, nlohmann::json &payload)
{
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
