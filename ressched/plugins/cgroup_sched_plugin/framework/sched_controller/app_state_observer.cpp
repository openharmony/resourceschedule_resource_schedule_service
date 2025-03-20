/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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
#include "ffrt.h"
#include "app_startup_scene_rec.h"
#include "supervisor.h"
#include "ui_extension_utils.h"

#undef LOG_TAG
#define LOG_TAG "RmsAppStateObserver"

namespace OHOS {
namespace ResourceSchedule {

std::unordered_map<int32_t, int32_t> RmsApplicationStateObserver::extensionStateToAbilityState_ = {
    {static_cast<int32_t>(AppExecFwk::ExtensionState::EXTENSION_STATE_CREATE),
        static_cast<int32_t>(AppExecFwk::AbilityState::ABILITY_STATE_CREATE)},
    {static_cast<int32_t>(AppExecFwk::ExtensionState::EXTENSION_STATE_READY),
        static_cast<int32_t>(AppExecFwk::AbilityState::ABILITY_STATE_READY)},
    {static_cast<int32_t>(AppExecFwk::ExtensionState::EXTENSION_STATE_CONNECTED),
        static_cast<int32_t>(AppExecFwk::AbilityState::ABILITY_STATE_CONNECTED)},
    {static_cast<int32_t>(AppExecFwk::ExtensionState::EXTENSION_STATE_DISCONNECTED),
        static_cast<int32_t>(AppExecFwk::AbilityState::ABILITY_STATE_DISCONNECTED)},
    {static_cast<int32_t>(AppExecFwk::ExtensionState::EXTENSION_STATE_TERMINATED),
        static_cast<int32_t>(AppExecFwk::AbilityState::ABILITY_STATE_TERMINATED)},
    {static_cast<int32_t>(AppExecFwk::ExtensionState::EXTENSION_STATE_FOREGROUND),
        static_cast<int32_t>(AppExecFwk::AbilityState::ABILITY_STATE_FOREGROUND)},
    {static_cast<int32_t>(AppExecFwk::ExtensionState::EXTENSION_STATE_BACKGROUND),
        static_cast<int32_t>(AppExecFwk::AbilityState::ABILITY_STATE_BACKGROUND)},
};

extern "C" void ReportProcessStateInProcess(int32_t state, int32_t pid);

void RmsApplicationStateObserver::OnForegroundApplicationChanged(const AppStateData &appStateData)
{
    if (!ValidateAppStateData(appStateData)) {
        CGS_LOGE("%{public}s : validate app state data failed!", __func__);
        return;
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
    std::string bundleName = abilityStateData.bundleName;
    int32_t abilityState = abilityStateData.abilityState;
    nlohmann::json payload;

    // if is uiExtension state changed, need to change extensionState to abilityState and write payload.
    if (IsUIExtensionAbilityStateChanged(abilityStateData)) {
        abilityState = extensionStateToAbilityState_[abilityState];
        payload["extensionAbilityType"] = std::to_string(abilityStateData.extensionAbilityType);
        payload["processType"] = std::to_string(abilityStateData.processType);
    }

    if (cgHandler) {
        auto uid = abilityStateData.uid;
        auto pid = abilityStateData.pid;
        auto abilityName = abilityStateData.abilityName;
        auto recordId = abilityStateData.abilityRecordId;
        auto abilityType = abilityStateData.abilityType;

        cgHandler->PostTask([cgHandler, uid, pid, bundleName, abilityName, recordId, abilityState, abilityType] {
            cgHandler->HandleAbilityStateChanged(uid, pid, bundleName, abilityName,
                recordId, abilityState, abilityType);
        });
    }

    std::string uid = std::to_string(abilityStateData.uid);
    payload["pid"] = std::to_string(abilityStateData.pid);
    payload["uid"] = uid;
    payload["bundleName"] = bundleName;
    ResSchedUtils::GetInstance().ReportDataInProcess(ResType::RES_TYPE_ABILITY_STATE_CHANGE,
        abilityState, payload);
    if (!AppStartupSceneRec::GetInstance().IsAppStartUp(abilityState)) {
        CGS_LOGD("abilityState is not app startUp");
        return;
    }
    ffrt::submit([abilityState, uid, bundleName, this]() {
        AppStartupSceneRec::GetInstance().RecordIsContinuousStartup(uid, bundleName);
    });
}

void RmsApplicationStateObserver::OnExtensionStateChanged(const AbilityStateData &abilityStateData)
{
    if (!ValidateAbilityStateData(abilityStateData)) {
        CGS_LOGE("%{public}s : validate extension state data failed!", __func__);
        return;
    }

    // if current is uiExtension, goto onAbilityStateChanged and report
    if (IsUIExtensionAbilityStateChanged(abilityStateData)) {
        CGS_LOGD("UIExtensionAbility Changed, extensionType: %{public}d, bundleName: %{public}s,"
            " abilityRecordId: %{public}d, abilityState: %{public}d, processType: %{public}d",
            abilityStateData.extensionAbilityType, abilityStateData.bundleName.c_str(),
            abilityStateData.abilityRecordId, abilityStateData.abilityState, abilityStateData.processType);
        OnAbilityStateChanged(abilityStateData);
        return;
    }

    auto cgHandler = SchedController::GetInstance().GetCgroupEventHandler();
    if (cgHandler) {
        auto uid = abilityStateData.uid;
        auto pid = abilityStateData.pid;
        auto bundleName = abilityStateData.bundleName;
        auto abilityName = abilityStateData.abilityName;
        auto recordId = abilityStateData.abilityRecordId;
        auto abilityState = abilityStateData.abilityState;
        auto abilityType = abilityStateData.abilityType;

        cgHandler->PostTask([cgHandler, uid, pid, bundleName, abilityName, recordId, abilityState, abilityType] {
            cgHandler->HandleExtensionStateChanged(uid, pid, bundleName, abilityName,
                recordId, abilityState, abilityType);
        });
    }

    nlohmann::json payload;
    payload["pid"] = std::to_string(abilityStateData.pid);
    payload["uid"] = std::to_string(abilityStateData.uid);
    payload["bundleName"] = abilityStateData.bundleName;
    ResSchedUtils::GetInstance().ReportDataInProcess(ResType::RES_TYPE_EXTENSION_STATE_CHANGE,
        abilityStateData.abilityState, payload);
}

bool RmsApplicationStateObserver::IsUIExtensionAbilityStateChanged(const AbilityStateData &abilityStateData)
{
    if (!ValidateUIExtensionAbilityStateData(abilityStateData)) {
        CGS_LOGD("%{public}s : Validate UIExtensionAbility state data failed!", __func__);
        return false;
    }

    if (abilityStateData.abilityState >= static_cast<int32_t>(extensionStateToAbilityState_.size())) {
        CGS_LOGD("%{public}s : Validate UIExtensionAbility data out of bound!", __func__);
        return false;
    }

    // trans int32_t to ExtensionAbilityType and check whether it is an UiExtensionAbility or an Extension
    AppExecFwk::ExtensionAbilityType extType =
        static_cast<AppExecFwk::ExtensionAbilityType>(abilityStateData.extensionAbilityType);
    if (!AAFwk::UIExtensionUtils::IsUIExtension(extType)) {
        CGS_LOGD("%{public}s : Current is not a UIExtensionAbility!", __func__);
        return false;
    }

    return true;
}

void RmsApplicationStateObserver::MarshallingProcessData(const ProcessData &processData, nlohmann::json &payload)
{
    payload["pid"] = std::to_string(processData.pid);
    payload["uid"] = std::to_string(processData.uid);
    payload["processType"] = std::to_string(static_cast<int32_t>(processData.processType));
    payload["renderUid"] = std::to_string(processData.renderUid);
    payload["bundleName"] = processData.bundleName;
    payload["state"] = std::to_string(static_cast<uint32_t>(processData.state));
    payload["extensionType"] = std::to_string(static_cast<uint32_t>(processData.extensionType));
    payload["isKeepAlive"] = std::to_string(processData.isKeepAlive);
    payload["isTestMode"] = std::to_string(processData.isTestMode);
    payload["processName"] = processData.processName;
    payload["hostPid"] = std::to_string(processData.hostPid);
    payload["callerPid"] = std::to_string(processData.callerPid);
    payload["callerUid"] = std::to_string(processData.callerUid);
}

void RmsApplicationStateObserver::OnProcessCreated(const ProcessData &processData)
{
    if (!ValidateProcessData(processData)) {
        CGS_LOGE("%{public}s : validate process data failed!", __func__);
        return;
    }
    auto cgHandler = SchedController::GetInstance().GetCgroupEventHandler();
    if (cgHandler) {
        cgHandler->PostTask([cgHandler, processData] {
            cgHandler->HandleProcessCreated(processData);
        });
    }

    nlohmann::json payload;
    MarshallingProcessData(processData, payload);
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
    MarshallingProcessData(processData, payload);
    ResSchedUtils::GetInstance().ReportDataInProcess(
        ResType::RES_TYPE_PROCESS_STATE_CHANGE, ResType::ProcessStatus::PROCESS_DIED, payload);
    ReportProcessStateInProcess((int32_t)ResType::ProcessStatus::PROCESS_DIED,
        (int32_t)processData.pid);
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
        auto pid = appStateData.pid;
        auto bundleName = appStateData.bundleName;
        auto state = appStateData.state;

        cgHandler->PostTask([cgHandler, uid, pid, bundleName, state] {
            cgHandler->HandleApplicationStateChanged(uid, pid, bundleName, state);
        });
    }

    nlohmann::json payload;
    payload["pid"] = std::to_string(appStateData.pid);
    payload["uid"] = std::to_string(appStateData.uid);
    payload["bundleName"] = appStateData.bundleName;
    payload["extensionType"] = std::to_string(static_cast<uint32_t>(appStateData.extensionType));
    payload["isPreload"] = std::to_string(appStateData.isPreloadModule);
    ResSchedUtils::GetInstance().ReportDataInProcess(ResType::RES_TYPE_APP_STATE_CHANGE, appStateData.state,
        payload);
    ResSchedUtils::GetInstance().ReportAppStateInProcess(appStateData.state, appStateData.pid);
}

void RmsApplicationStateObserver::MarshallingAppStateData(const AppStateData &appStateData, nlohmann::json &payload)
{
    payload["pid"] = appStateData.pid;
    payload["uid"] = appStateData.uid;
    payload["bundleName"] = appStateData.bundleName;
    payload["state"] = static_cast<uint32_t>(appStateData.state);
    payload["extensionType"] = static_cast<uint32_t>(appStateData.extensionType);
    payload["isFocused"] = static_cast<bool>(appStateData.isFocused);
}

void RmsApplicationStateObserver::OnAppStateChanged(const AppStateData &appStateData)
{
    if (!ValidateAppStateData(appStateData)) {
        CGS_LOGE("%{public}s : validate app state data failed!", __func__);
        return;
    }

    nlohmann::json payload;
    MarshallingAppStateData(appStateData, payload);
    ResSchedUtils::GetInstance().ReportDataInProcess(ResType::RES_TYPE_ON_APP_STATE_CHANGED, appStateData.state,
        payload);
    ResSchedUtils::GetInstance().ReportAppStateInProcess(appStateData.state, appStateData.pid);
}

void RmsApplicationStateObserver::OnAppCacheStateChanged(const AppStateData &appStateData)
{
    if (!ValidateAppStateData(appStateData)) {
        CGS_LOGE("%{public}s : validate app state data failed!", __func__);
        return;
    }

    nlohmann::json payload;
    MarshallingAppStateData(appStateData, payload);
    const int RES_TYPE_EXT_ON_APP_CACHED_STATE_CHANGED = 10008;
    payload["extType"] = std::to_string(RES_TYPE_EXT_ON_APP_CACHED_STATE_CHANGED);
    ResSchedUtils::GetInstance().ReportDataInProcess(ResType::RES_TYPE_KEY_PERF_SCENE, appStateData.state, payload);
}

void RmsApplicationStateObserver::OnProcessStateChanged(const ProcessData &processData)
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
        auto state = static_cast<int32_t>(processData.state);

        cgHandler->PostTask([cgHandler, uid, pid, bundleName, state] {
            cgHandler->HandleProcessStateChanged(uid, pid, bundleName, state);
        });
    }

    nlohmann::json payload;
    MarshallingProcessData(processData, payload);
    ResSchedUtils::GetInstance().ReportDataInProcess(
        ResType::RES_TYPE_PROCESS_STATE_CHANGE, static_cast<int32_t>(processData.state), payload);
    ReportProcessStateInProcess((int32_t)processData.state, (int32_t)processData.pid);
}

void RmsApplicationStateObserver::OnAppStopped(const AppStateData &appStateData)
{
    if (!ValidateAppStateData(appStateData)) {
        CGS_LOGE("%{public}s : validate app state data failed!", __func__);
        return;
    }

    nlohmann::json payload;
    MarshallingAppStateData(appStateData, payload);
    ResSchedUtils::GetInstance().ReportDataInProcess(ResType::RES_TYPE_APP_STOPPED, appStateData.state, payload);
}
} // namespace ResourceSchedule
} // namespace OHOS
