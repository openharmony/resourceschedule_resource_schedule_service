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

#include "app_state_observer.h"

#include "sched_controller.h"
#include "cgroup_event_handler.h"
#include "cgroup_sched_log.h"
#include "ressched_utils.h"
#include "res_type.h"
#include "supervisor.h"

namespace OHOS {
namespace ResourceSchedule {
namespace {
    constexpr HiviewDFX::HiLogLabel LOG_LABEL = {LOG_CORE, LOG_TAG_DOMAIN_ID_RMS, "RmsAppStateObserver"};
}

void RmsApplicationStateObserver::OnForegroundApplicationChanged(const AppStateData &appStateData)
{
    if (!ValidateAppStateData(appStateData)) {
        CGS_LOGE("%{public}s : validate app state data failed!", __func__);
        return;
    }
    auto cgHandler = SchedController::GetInstance().GetCgroupEventHandler();
    if (cgHandler) {
        auto uid = appStateData.uid;
        auto bundleName = appStateData.bundleName;
        auto state = appStateData.state;

        cgHandler->PostTask([cgHandler, uid, bundleName, state] {
            cgHandler->HandleForegroundApplicationChanged(uid, bundleName, state);
        });
    }

    nlohmann::json payload;
    payload["pid"] = std::to_string(appStateData.pid);
    payload["uid"] = std::to_string(appStateData.uid);
    payload["bundleName"] = appStateData.bundleName;
    ResSchedUtils::GetInstance().ReportDataInProcess(ResType::RES_TYPE_APP_STATE_CHANGE, appStateData.state, payload);
}

void RmsApplicationStateObserver::OnAbilityStateChanged(const AbilityStateData &abilityStateData)
{
    if (!ValidateAbilityStateData(abilityStateData)) {
        CGS_LOGE("%{public}s : validate ability state data failed!", __func__);
        return;
    }
    auto cgHandler = SchedController::GetInstance().GetCgroupEventHandler();
    if (cgHandler) {
        auto uid = abilityStateData.uid;
        auto pid = abilityStateData.pid;
        auto bundleName = abilityStateData.bundleName;
        auto abilityName = abilityStateData.abilityName;
        auto token = reinterpret_cast<uintptr_t>(abilityStateData.token.GetRefPtr());
        auto abilityState = abilityStateData.abilityState;
        auto abilityType = abilityStateData.abilityType;

        cgHandler->PostTask([cgHandler, uid, pid, bundleName, abilityName, token, abilityState, abilityType] {
            cgHandler->HandleAbilityStateChanged(uid, pid, bundleName, abilityName,
                token, abilityState, abilityType);
        });
    }

    nlohmann::json payload;
    payload["pid"] = std::to_string(abilityStateData.pid);
    payload["uid"] = std::to_string(abilityStateData.uid);
    payload["bundleName"] = abilityStateData.bundleName;
    ResSchedUtils::GetInstance().ReportDataInProcess(ResType::RES_TYPE_ABILITY_STATE_CHANGE,
        abilityStateData.abilityState, payload);
}

void RmsApplicationStateObserver::OnExtensionStateChanged(const AbilityStateData &abilityStateData)
{
    if (!ValidateAbilityStateData(abilityStateData)) {
        CGS_LOGE("%{public}s : validate extension state data failed!", __func__);
        return;
    }
    auto cgHandler = SchedController::GetInstance().GetCgroupEventHandler();
    if (cgHandler) {
        auto uid = abilityStateData.uid;
        auto pid = abilityStateData.pid;
        auto bundleName = abilityStateData.bundleName;
        auto abilityName = abilityStateData.abilityName;
        auto token = reinterpret_cast<uintptr_t>(abilityStateData.token.GetRefPtr());
        auto abilityState = abilityStateData.abilityState;
        auto abilityType = abilityStateData.abilityType;

        cgHandler->PostTask([cgHandler, uid, pid, bundleName, abilityName, token, abilityState, abilityType] {
            cgHandler->HandleExtensionStateChanged(uid, pid, bundleName, abilityName,
                token, abilityState, abilityType);
        });
    }

    nlohmann::json payload;
    payload["pid"] = std::to_string(abilityStateData.pid);
    payload["uid"] = std::to_string(abilityStateData.uid);
    payload["bundleName"] = abilityStateData.bundleName;
    ResSchedUtils::GetInstance().ReportDataInProcess(ResType::RES_TYPE_EXTENSION_STATE_CHANGE,
        abilityStateData.abilityState, payload);
}

void RmsApplicationStateObserver::OnProcessCreated(const ProcessData &processData)
{
    if (!ValidateProcessData(processData)) {
        CGS_LOGE("%{public}s : validate process data failed!", __func__);
        return;
    }
    auto cgHandler = SchedController::GetInstance().GetCgroupEventHandler();
    if (cgHandler) {
        auto uid = processData.uid;
        auto pid = processData.pid;
        auto bundleName = processData.bundleName;

        cgHandler->PostTask([cgHandler, uid, pid, bundleName] {
            cgHandler->HandleProcessCreated(uid, pid, bundleName);
        });
    }

    nlohmann::json payload;
    payload["pid"] = std::to_string(processData.pid);
    payload["uid"] = std::to_string(processData.uid);
    payload["bundleName"] = processData.bundleName;
    ResSchedUtils::GetInstance().ReportDataInProcess(
        ResType::RES_TYPE_PROCESS_STATE_CHANGE, ResType::ProcessStatus::PROCESS_CREATED, payload);
}

void RmsApplicationStateObserver::OnProcessDied(const ProcessData &processData)
{
    if (!ValidateProcessData(processData)) {
        CGS_LOGE("%{public}s : validate process data failed!", __func__);
        return;
    }
    auto cgHandler = SchedController::GetInstance().GetCgroupEventHandler();
    if (cgHandler) {
        auto uid = processData.uid;
        auto pid = processData.pid;
        auto bundleName = processData.bundleName;

        cgHandler->PostTask([cgHandler, uid, pid, bundleName] {
            cgHandler->HandleProcessDied(uid, pid, bundleName);
        });
    }

    nlohmann::json payload;
    payload["pid"] = std::to_string(processData.pid);
    payload["uid"] = std::to_string(processData.uid);
    payload["bundleName"] = processData.bundleName;
    ResSchedUtils::GetInstance().ReportDataInProcess(
        ResType::RES_TYPE_PROCESS_STATE_CHANGE, ResType::ProcessStatus::PROCESS_DIED, payload);
}

void RmsApplicationStateObserver::OnApplicationStateChanged(const AppStateData &appStateData)
{
    if (!ValidateAppStateData(appStateData)) {
        CGS_LOGE("%{public}s : validate app state data failed!", __func__);
        return;
    }

    auto cgHandler = SchedController::GetInstance().GetCgroupEventHandler();
    if (cgHandler) {
        auto uid = appStateData.uid;
        auto bundleName = appStateData.bundleName;
        auto state = appStateData.state;

        cgHandler->PostTask([cgHandler, uid, bundleName, state] {
            cgHandler->HandleApplicationStateChanged(uid, bundleName, state);
        });
    }

    nlohmann::json payload;
    payload["pid"] = std::to_string(appStateData.pid);
    payload["uid"] = std::to_string(appStateData.uid);
    payload["bundleName"] = appStateData.bundleName;
    ResSchedUtils::GetInstance().ReportDataInProcess(ResType::RES_TYPE_APP_STATE_CHANGE, appStateData.state, payload);
}
} // namespace ResourceSchedule
} // namespace OHOS
