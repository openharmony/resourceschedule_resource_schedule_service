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
    auto cgHander = SchedController::GetInstance().GetCgroupEventHandler();
    if (cgHander != nullptr) {
        cgHander->PostTask([cgHander, appStateData] {
            cgHander->HandleForegroundApplicationChanged(appStateData.uid, appStateData.bundleName,
                appStateData.state);
        });
    }

    std::string payload = std::to_string(appStateData.uid) + "," + // uid
            appStateData.bundleName; // bundle name
    ResSchedUtils::GetInstance().ReportDataInProcess(ResType::RES_TYPE_APP_STATE_CHANGE, appStateData.state, payload);
}

void RmsApplicationStateObserver::OnAbilityStateChanged(const AbilityStateData &abilityStateData)
{
    if (!ValidateAbilityStateData(abilityStateData)) {
        CGS_LOGE("%{public}s : validate ability state data failed!", __func__);
        return;
    }
    auto cgHander = SchedController::GetInstance().GetCgroupEventHandler();
    if (cgHander != nullptr) {
        cgHander->PostTask([cgHander, abilityStateData] {
            cgHander->HandleAbilityStateChanged(abilityStateData.uid, abilityStateData.pid,
                abilityStateData.bundleName, abilityStateData.abilityName,
                abilityStateData.token, abilityStateData.abilityState);
        });
    }

    std::string payload = std::to_string(abilityStateData.pid) + "," + // pid
            std::to_string(abilityStateData.uid) + "," + // uid
            abilityStateData.bundleName; // bundle name
    ResSchedUtils::GetInstance().ReportDataInProcess(ResType::RES_TYPE_ABILITY_STATE_CHANGE,
        abilityStateData.abilityState, payload);
}

void RmsApplicationStateObserver::OnExtensionStateChanged(const AbilityStateData &abilityStateData)
{
    if (!ValidateAbilityStateData(abilityStateData)) {
        CGS_LOGE("%{public}s : validate extension state data failed!", __func__);
        return;
    }
    auto cgHander = SchedController::GetInstance().GetCgroupEventHandler();
    if (cgHander != nullptr) {
        cgHander->PostTask([cgHander, abilityStateData] {
            cgHander->HandleExtensionStateChanged(abilityStateData.uid, abilityStateData.pid,
                abilityStateData.bundleName, abilityStateData.abilityName,
                abilityStateData.token, abilityStateData.abilityState);
        });
    }

    std::string payload = std::to_string(abilityStateData.pid) + "," + // pid
            std::to_string(abilityStateData.uid) + "," + // uid
            abilityStateData.bundleName; // bundle name
    ResSchedUtils::GetInstance().ReportDataInProcess(ResType::RES_TYPE_EXTENSION_STATE_CHANGE,
        abilityStateData.abilityState, payload);
}

void RmsApplicationStateObserver::OnProcessCreated(const ProcessData &processData)
{
    if (!ValidateProcessData(processData)) {
        CGS_LOGE("%{public}s : validate process data failed!", __func__);
        return;
    }
    auto cgHander = SchedController::GetInstance().GetCgroupEventHandler();
    if (cgHander != nullptr) {
        cgHander->PostTask([cgHander, processData] {
            cgHander->HandleProcessCreated(processData.uid, processData.pid, processData.bundleName);
        });
    }

    std::string payload = std::to_string(processData.pid) + "," + // pid
            std::to_string(processData.uid) + "," + // uid
            processData.bundleName; // bundle name
    ResSchedUtils::GetInstance().ReportDataInProcess(ResType::RES_TYPE_PROCESS_STATE_CHANGE, 0, payload);
}

void RmsApplicationStateObserver::OnProcessDied(const ProcessData &processData)
{
    if (!ValidateProcessData(processData)) {
        CGS_LOGE("%{public}s : validate process data failed!", __func__);
        return;
    }
    auto cgHander = SchedController::GetInstance().GetCgroupEventHandler();
    if (cgHander != nullptr) {
        cgHander->PostTask([cgHander, processData] {
            cgHander->HandleProcessDied(processData.uid, processData.pid, processData.bundleName);
        });
    }

    std::string payload = std::to_string(processData.pid) + "," + // pid
            std::to_string(processData.uid) + "," + // uid
            processData.bundleName; // bundle name
    ResSchedUtils::GetInstance().ReportDataInProcess(ResType::RES_TYPE_PROCESS_STATE_CHANGE, 1, payload);
}

void RmsApplicationStateObserver::OnApplicationStateChanged(const AppStateData &appStateData)
{
    if (!ValidateAppStateData(appStateData)) {
        CGS_LOGE("%{public}s : validate app state data failed!", __func__);
        return;
    }

    auto cgHander = SchedController::GetInstance().GetCgroupEventHandler();
    if (cgHander != nullptr) {
        cgHander->PostTask([cgHander, appStateData] {
            cgHander->HandleApplicationStateChanged(appStateData.uid, appStateData.bundleName,
                appStateData.state);
        });
    }

    std::string payload = std::to_string(appStateData.uid) + "," + // uid
            appStateData.bundleName; // bundle name
    ResSchedUtils::GetInstance().ReportDataInProcess(ResType::RES_TYPE_APP_STATE_CHANGE, appStateData.state, payload);
}
} // namespace ResourceSchedule
} // namespace OHOS
