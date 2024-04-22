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
#include "supervisor.h"

#undef LOG_TAG
#define LOG_TAG "RmsAppStateObserver"

namespace OHOS {
namespace ResourceSchedule {
static const int32_t CONTINUOUS_START_TIME_OUT = 15 * 1000 * 1000;
static const int32_t MAX_NO_REPEAT_APP_COUNT = 4;
static const int32_t MAX_CONTINUOUS_START_NUM = 5;
static const int32_t APP_START_UP = 0;
static const std::string RUNNER_NAME = "RmsApplicationStateObserverQueue";
RmsApplicationStateObserver::RmsApplicationStateObserver()
{
    ffrtQueue_ = std::make_shared<ffrt::queue>(RUNNER_NAME.c_str(),
        ffrt::queue_attr().qos(ffrt::qos_user_initiated));
}
RmsApplicationStateObserver::~RmsApplicationStateObserver()
{
    exitContinuousStartUpTask = nullptr;
    ffrtQueue_ = nullptr;
    startPkgs_.clear();
    startUidSet_.clear();
    startIgnorePkgs_.clear();
}

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
    if (abilityStateData.abilityState == APP_START_UP) {
        std::string uid = std::to_string(abilityStateData.uid);
        std::string bundleName = abilityStateData.bundleName;
        ffrtQueue_->submit([uid, bundleName, this]() {
            recordIsContinuousStartUp(uid, bundleName);
        });
    }
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

void RmsApplicationStateObserver::MarshallingProcessData(const ProcessData &processData, nlohmann::json &payload)
{
    payload["pid"] = std::to_string(processData.pid);
    payload["uid"] = std::to_string(processData.uid);
    payload["processType"] = std::to_string(static_cast<int32_t>(processData.processType));
    payload["renderUid"] = std::to_string(processData.renderUid);
    payload["bundleName"] = processData.bundleName;
    payload["state"] = std::to_string(static_cast<uint32_t>(processData.state));
    payload["extensionType"] = std::to_string(static_cast<uint32_t>(processData.extensionType));
    payload["isTestMode"] = std::to_string(processData.isTestMode);
    payload["processName"] = processData.processName;
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
        auto processType = static_cast<int32_t>(processData.processType);
        auto extensionType = static_cast<int32_t>(processData.extensionType);
        cgHandler->PostTask([cgHandler, uid, pid, processType, bundleName, extensionType] {
            cgHandler->HandleProcessCreated(uid, pid, processType, bundleName, extensionType);
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
    ResSchedUtils::GetInstance().ReportDataInProcess(ResType::RES_TYPE_APP_STATE_CHANGE, appStateData.state, payload);
    ResSchedUtils::GetInstance().ReportAppStateInProcess(appStateData.state, appStateData.pid);
}

void RmsApplicationStateObserver::MarshallingAppStateData(const AppStateData &appStateData, nlohmann::json &payload)
{
    payload["pid"] = appStateData.pid;
    payload["uid"] = appStateData.uid;
    payload["bundleName"] = appStateData.bundleName;
    payload["state"] = static_cast<uint32_t>(appStateData.state);
    payload["extensionType"] = static_cast<uint32_t>(appStateData.extensionType);
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
}

void RmsApplicationStateObserver::recordIsContinuousStartUp(std::string uid, std::string bundleName)
{
    if(startIgnorePkg_.find(bundleName) != startIgnorePkgs_.end()) {
        return;
    }
    if (exitContinuousStartUpTask != nullptr) {
        ffrtQueue_->cancel(exitContinuousStartUpTask);
    }
    auto tarEndTimePoint = std::chrono:system_clock::now();
    auto tarDuration = std::chrono::duration_cast<std::chrono::microseconds>(tarEndTimePoint.time_since_epoch());
    int64_t curTime = tarDuration.count();
    CGS_LOGI("recordIsContinuousStartUp uid: %{public}s bundleName: %{public}s curTime:%{public}ld",
        uid.c_str(), bundleName.c_str(), curTime);
    if (curTime - lastAppStartTime_ >= CONTINUOUS_START_TIME_OUT) {
        cleanRecordSceneData();
    }
    updateAppStartupNum(uid, curTime, bundleName);
    if (isContinuousStartUp()) {
        if (isReportContinuousStartUp_.load()) {
            return;
        }
        nlohmann::json payload;
        ResSchedUtils::GetInstance().ReportDataInProcess(
            ResType::RES_TYPE_CONTINUOUS_STARTUP, ResType::ContinuousStartUpStatus::START_CONTINUOUS_STARTUP, payload);
        isReportContinuousStartUp_ = true;
        exitContinuousStartUpTask = ffrtQueue_->submit_h([this]) {
            cleanRecordSceneData();
        }, ffrt_task_attr().delay(CONTINUOUS_START_TIME_OUT);
    }
}
void RmsApplicationStateObserver::cleanRecordSceneData()
{
    CGS_LOGI("cleanRecordSceneData");
    std::unique_lock<ffrt_mutex> lock(mutex_);
    lastStartUid_ = "";
    startPkgs_.clear();
    startUidSet_.clear();
    startIgnorePkgs_.clear();
    if (isReportContinuousStartUp_.load()) {
        nlohmann::json payload;
        ResSchedUtils::GetInstance().ReportDataInProcess(
            ResType::RES_TYPE_CONTINUOUS_STARTUP, ResType::ContinuousStartUpStatus::STOP_CONTINUOUS_STARTUP, payload);
        isReportContinuousStartUp_ = false;
    }
}
void RmsApplicationStateObserver::updateAppStartupNum(std::string uid, int64_t curTime,std::string bundleName)
{
    std::unique_lock<ffrt_mutex> lock(mutex_);
    lastAppStartTime_ = curTime;
    if (lastStartUid_ == uid) {
        CGS_LOGE("same uid: %{public}s, not update app startUp", uid.c_str());
        return;
    }
    lastStartUid_ = uid;
    startPkgs_.emplace_back(bundleName);
    startUidSet_.insert(uid);
}
bool RmsApplicationStateObserver::isContinuousStartUp()
{
    std::unique_lock<ffrt_mutex> lock(mutex_);
    if (startPkgs_.size() >= MAX_CONTINUOUS_START_NUM && startUidSet_.size() >= MAX_NO_REPEAT_APP_COUNT) {
        return true;
    }
    return false;
}
} // namespace ResourceSchedule
} // namespace OHOS
