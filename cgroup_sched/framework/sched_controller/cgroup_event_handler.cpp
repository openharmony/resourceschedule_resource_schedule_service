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

#include "cgroup_event_handler.h"

#include "sched_controller.h"
#include "cgroup_adjuster.h"
#include "cgroup_sched_common.h"
#include "cgroup_sched_log.h"
#include "ressched_utils.h"
#include "res_type.h"

namespace OHOS {
namespace ResourceSchedule {

namespace {
    constexpr HiviewDFX::HiLogLabel LOG_LABEL = {LOG_CORE, LOG_TAG_DOMAIN_ID_RMS, "CgroupEventHandler"};
}

CgroupEventHandler::CgroupEventHandler(const std::shared_ptr<EventRunner> &runner)
        : EventHandler(runner)
{}

CgroupEventHandler::~CgroupEventHandler()
{
    supervisor_ = nullptr;
}

void CgroupEventHandler::ProcessEvent(const AppExecFwk::InnerEvent::Pointer& event)
{
    switch (event->GetInnerEventId()) {
        case INNER_EVENT_ID_REG_STATE_OBSERVERS: {
            ChronoScope cs("Delayed RegisterStateObservers.");
            SchedController::GetInstance().RegisterStateObservers();
            break;
        }
        default:
            break;
    }
}

void CgroupEventHandler::SetSupervisor(std::shared_ptr<Supervisor> supervisor)
{
    supervisor_ = supervisor;
}

void CgroupEventHandler::HandleForegroundApplicationChanged(uid_t uid, std::string bundleName, int32_t state)
{
    if (supervisor_ == nullptr) {
        CGS_LOGE("%{public}s : supervisor nullptr!", __func__);
        return;
    }
    CGS_LOGD("%{public}s : %{public}d, %{public}s, %{public}d", __func__, uid, bundleName.c_str(), state);
    ChronoScope cs("HandleForegroundApplicationChanged");
    std::shared_ptr<Application> app = supervisor_->GetAppRecordNonNull(uid, bundleName);
    app->state_ = state;
    SchedController::GetInstance().AdjustAllProcessGroup(*(app.get()), AdjustSource::ADJS_FG_APP_CHANGE);
}

void CgroupEventHandler::HandleAbilityStateChanged(uid_t uid, pid_t pid, std::string bundleName,
        std::string abilityName, sptr<IRemoteObject> token, int32_t abilityState)
{
    if (supervisor_ == nullptr) {
        CGS_LOGE("%{public}s : supervisor nullptr!", __func__);
        return;
    }
    CGS_LOGD("%{public}s : %{public}d, %{public}d, %{public}s, %{public}s, %{public}p, %{public}d",
            __func__, uid, pid, bundleName.c_str(), abilityName.c_str(), token.GetRefPtr(), abilityState);
    ChronoScope cs("HandleAbilityStateChanged");
    auto app = supervisor_->GetAppRecordNonNull(uid, bundleName);
    auto procRecord = app->GetProcessRecordNonNull(pid, abilityName);
    procRecord->abilityState_ = abilityState;
    procRecord->token_ = token;
    SchedController::GetInstance().AdjustProcessGroup(*(app.get()), *(procRecord.get()), AdjustSource::ADJS_ABILITY_STATE);
}

void CgroupEventHandler::HandleExtensionStateChanged(uid_t uid, pid_t pid, std::string bundleName,
        std::string abilityName, sptr<IRemoteObject> token, int32_t extensionState)
{
    if (supervisor_ == nullptr) {
        CGS_LOGE("%{public}s : supervisor nullptr!", __func__);
        return;
    }
    CGS_LOGD("%{public}s : %{public}d, %{public}d, %{public}s, %{public}s, %{public}p, %{public}d",
            __func__, uid, pid, bundleName.c_str(), abilityName.c_str(), token.GetRefPtr(), extensionState);
    ChronoScope cs("HandleExtensionStateChanged");
    auto app = supervisor_->GetAppRecordNonNull(uid, bundleName);
    auto procRecord = app->GetProcessRecordNonNull(pid, abilityName);
    procRecord->extensionState_ = extensionState;
    procRecord->token_ = token;
    SchedController::GetInstance().AdjustProcessGroup(*(app.get()), *(procRecord.get()), AdjustSource::ADJS_EXTENSION_STATE);
}

void CgroupEventHandler::HandleProcessCreated(uid_t uid, pid_t pid, std::string bundleName)
{
    if (supervisor_ == nullptr) {
        CGS_LOGE("%{public}s : supervisor nullptr!", __func__);
        return;
    }
    CGS_LOGD("%{public}s : %{public}d, %{public}d, %{public}s", __func__, uid, pid, bundleName.c_str());
    ChronoScope cs("HandleProcessCreated");
    std::shared_ptr<Application> app = supervisor_->GetAppRecordNonNull(uid, bundleName);
    std::shared_ptr<ProcessRecord> procRecord = std::make_shared<ProcessRecord>(uid, pid, bundleName);
    app->AddProcessRecord(procRecord);
    SchedController::GetInstance().AdjustProcessGroup(*(app.get()), *(procRecord.get()), AdjustSource::ADJS_PROCESS_CREATE);
}

void CgroupEventHandler::HandleProcessDied(uid_t uid, pid_t pid, std::string bundleName)
{
    if (supervisor_ == nullptr) {
        CGS_LOGE("%{public}s : supervisor nullptr!", __func__);
        return;
    }
    CGS_LOGD("%{public}s : %{public}d, %{public}d, %{public}s", __func__, uid, pid, bundleName.c_str());
    std::shared_ptr<Application> app = supervisor_->GetAppRecord(uid);
    if (app == nullptr) {
        CGS_LOGE("%{public}s : application %{public}s not exist!", __func__, bundleName.c_str());
        return;
    }
    std::shared_ptr<ProcessRecord> processRecord = app->RemoveProcessRecord(pid);
}

void CgroupEventHandler::HandleTransientTaskStart(uid_t uid, pid_t pid, std::string packageName)
{
    if (supervisor_ == nullptr) {
        CGS_LOGE("%{public}s : supervisor nullptr!", __func__);
        return;
    }
    CGS_LOGD("%{public}s : %{public}d, %{public}d, %{public}s", __func__, uid, pid, packageName.c_str());
    auto app = supervisor_->GetAppRecordNonNull(uid, packageName);
    auto procRecord = app->GetProcessRecord(pid);
    if (procRecord == nullptr) {
        return;
    }
    procRecord->runningTransientTask_ = true;
}

void CgroupEventHandler::HandleTransientTaskEnd(uid_t uid, pid_t pid, std::string packageName)
{
    if (supervisor_ == nullptr) {
        CGS_LOGE("%{public}s : supervisor nullptr!", __func__);
        return;
    }
    CGS_LOGD("%{public}s : %{public}d, %{public}d, %{public}s", __func__, uid, pid, packageName.c_str());
    auto app = supervisor_->GetAppRecordNonNull(uid, packageName);
    auto procRecord = app->GetProcessRecord(pid);
    if (procRecord == nullptr) {
        return;
    }
    procRecord->runningTransientTask_ = false;
}

void CgroupEventHandler::HandleContinuousTaskStart(uid_t uid, pid_t pid, std::string abilityName)
{
    if (supervisor_ == nullptr) {
        CGS_LOGE("%{public}s : supervisor nullptr!", __func__);
        return;
    }
    CGS_LOGD("%{public}s : %{public}d, %{public}d, %{public}s", __func__, uid, pid, abilityName.c_str());
    ChronoScope cs("HandleContinuousTaskStart");
    auto app = supervisor_->GetAppRecordNonNull(uid, abilityName);
    auto procRecord = app->GetProcessRecord(pid);
    if (procRecord == nullptr) {
        return;
    }
    procRecord->runningContinuousTask_ = true;
    SchedController::GetInstance().AdjustProcessGroup(*(app.get()), *(procRecord.get()), AdjustSource::ADJS_CONTINUOUS_BEGIN);
}

void CgroupEventHandler::HandleContinuousTaskCancel(uid_t uid, pid_t pid, std::string abilityName)
{
    if (supervisor_ == nullptr) {
        CGS_LOGE("%{public}s : supervisor nullptr!", __func__);
        return;
    }
    CGS_LOGD("%{public}s : %{public}d, %{public}d, %{public}s", __func__, uid, pid, abilityName.c_str());
    ChronoScope cs("HandleContinuousTaskCancel");
    auto app = supervisor_->GetAppRecordNonNull(uid, abilityName);
    auto procRecord = app->GetProcessRecord(pid);
    if (procRecord == nullptr) {
        return;
    }
    procRecord->runningContinuousTask_ = false;
    SchedController::GetInstance().AdjustProcessGroup(*(app.get()), *(procRecord.get()), AdjustSource::ADJS_CONTINUOUS_END);
}

void CgroupEventHandler::HandleFocusedWindow(uint32_t windowId, sptr<IRemoteObject> abilityToken, WindowType windowType, int32_t displayId)
{
    if (supervisor_ == nullptr) {
        CGS_LOGE("%{public}s : supervisor nullptr!", __func__);
        return;
    }
    CGS_LOGD("%{public}s : %{public}d, %{public}p, %{public}d, %{public}d", __func__, windowId, abilityToken.GetRefPtr(), windowType, displayId);
    std::shared_ptr<Application> app = nullptr;
    std::shared_ptr<ProcessRecord> procRecord = nullptr;
    {
        ChronoScope cs("HandleFocusedWindow");
        supervisor_->SearchAbilityToken(app, procRecord, abilityToken);
        if (app == nullptr || procRecord == nullptr) {
            return;
        }
        auto pidsMap = app->GetPidsMap();
        for (auto iter = pidsMap.begin(); iter != pidsMap.end(); iter++)
        {
            auto pr = iter->second;
            if (pr != procRecord) {
                pr->focused_ = false;
            }
        }
        procRecord->focused_ = true;
        procRecord->windowType_ = VALUE_INT(windowType);
        app->focusedProcess_ = procRecord;
        SchedController::GetInstance().AdjustAllProcessGroup(*(app.get()), AdjustSource::ADJS_FOCUSED_WINDOW);
    }

    std::string payload = std::to_string(procRecord->GetPid()) + "," + // pid
            std::to_string(procRecord->GetUid()) + "," + // uid
            app->GetName() + "," + // bundle name
            std::to_string(windowId) + "," + // window id
            std::to_string(VALUE_INT(windowType)) + "," + // window type
            std::to_string(displayId);// display id
    ResSchedUtils::GetInstance().ReportDataInProcess(ResType::RES_TYPE_WINDOW_FOCUS, 0, payload);
}

void CgroupEventHandler::HandleUnfocusedWindow(uint32_t windowId, sptr<IRemoteObject> abilityToken, WindowType windowType, int32_t displayId)
{
    if (supervisor_ == nullptr) {
        CGS_LOGE("%{public}s : supervisor nullptr!", __func__);
        return;
    }
    CGS_LOGD("%{public}s : %{public}d, %{public}p, %{public}d, %{public}d", __func__, windowId, abilityToken.GetRefPtr(), windowType, displayId);
    std::shared_ptr<Application> app = nullptr;
    std::shared_ptr<ProcessRecord> procRecord = nullptr;
    {
        ChronoScope cs("HandleUnfocusedWindow");
        supervisor_->SearchAbilityToken(app, procRecord, abilityToken);
        if (app == nullptr || procRecord == nullptr) {
            return;
        }
        procRecord->focused_ = false;
        procRecord->windowType_ = VALUE_INT(windowType);
        if (app->focusedProcess_ == procRecord) {
            app->focusedProcess_ = nullptr;
        }
        SchedController::GetInstance().AdjustAllProcessGroup(*(app.get()), AdjustSource::ADJS_UNFOCUSED_WINDOW);
    }

    std::string payload = std::to_string(procRecord->GetPid()) + "," + // pid
            std::to_string(procRecord->GetUid()) + "," + // uid
            app->GetName() + "," + // bundle name
            std::to_string(windowId) + "," + // window id
            std::to_string(VALUE_INT(windowType)) + "," + // window type
            std::to_string(displayId);// display id
    ResSchedUtils::GetInstance().ReportDataInProcess(ResType::RES_TYPE_WINDOW_FOCUS, 1, payload);
}
} // namespace ResourceSchedule
} // namespace OHOS
