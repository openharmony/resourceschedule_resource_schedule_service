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
    auto cgHandler = SchedController::GetInstance().GetCgroupEventHandler();
    if (cgHandler) {
        auto uid = info->GetUid();
        auto pid = info->GetPid();
        auto pkgName = info->GetPackageName();

        cgHandler->PostTask([cgHandler, uid, pid, pkgName] {
            cgHandler->HandleTransientTaskStart(uid, pid, pkgName);
        });
    }

    Json::Value payload;
    payload["pid"] = info->GetPid();
    payload["uid"] = info->GetUid();
    payload["bundleName"] = info->GetPackageName();
    ResSchedUtils::GetInstance().ReportDataInProcess(ResType::RES_TYPE_TRANSIENT_TASK, 0, payload);
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

    Json::Value payload;
    payload["pid"] = info->GetPid();
    payload["uid"] = info->GetUid();
    payload["bundleName"] = info->GetPackageName();
    ResSchedUtils::GetInstance().ReportDataInProcess(ResType::RES_TYPE_TRANSIENT_TASK, 1, payload);
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

    Json::Value payload;
    payload["pid"] = continuousTaskCallbackInfo->GetCreatorPid();
    payload["uid"] = continuousTaskCallbackInfo->GetCreatorUid();
    payload["abilityName"] = continuousTaskCallbackInfo->GetAbilityName();
    ResSchedUtils::GetInstance().ReportDataInProcess(ResType::RES_TYPE_CONTINUOUS_TASK, 0, payload);
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

    Json::Value payload;
    payload["pid"] = continuousTaskCallbackInfo->GetCreatorPid();
    payload["uid"] = continuousTaskCallbackInfo->GetCreatorUid();
    payload["abilityName"] = continuousTaskCallbackInfo->GetAbilityName();
    ResSchedUtils::GetInstance().ReportDataInProcess(ResType::RES_TYPE_CONTINUOUS_TASK, 0, payload);
}

void BackgroundTaskObserver::OnRemoteDied(const wptr<IRemoteObject> &object)
{
    CGS_LOGI("%{public}s.", __func__);
}
} // namespace ResourceSchedule
} // namespace OHOS
