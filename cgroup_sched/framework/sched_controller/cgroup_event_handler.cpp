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
            int retry = event->GetParam();
            if (SchedController::GetInstance().RegisterStateObservers()) {
                CGS_LOGW("%{public}s register state observer success.", __func__);
                break;
            }
            if (retry < MAX_RETRY_TIMES) {
                CGS_LOGW("%{public}s retry register state observers, retry:%{public}d.", __func__, retry);
                auto event = AppExecFwk::InnerEvent::Get(INNER_EVENT_ID_REG_STATE_OBSERVERS, retry + 1);
                this->SendEvent(event, DELAYED_RETRY_REGISTER_DURATION);
            } else {
                CGS_LOGE("%{public}s register state observer failed eventually, reach max retry times!", __func__);
            }
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
    if (!supervisor_) {
        CGS_LOGE("%{public}s : supervisor nullptr!", __func__);
        return;
    }
    CGS_LOGD("%{public}s : %{public}d, %{public}s, %{public}d", __func__, uid, bundleName.c_str(), state);
    ChronoScope cs("HandleForegroundApplicationChanged");
    std::shared_ptr<Application> app = supervisor_->GetAppRecordNonNull(uid, bundleName);
    app->state_ = state;
    SchedController::GetInstance().AdjustAllProcessGroup(*(app.get()), AdjustSource::ADJS_FG_APP_CHANGE);
}

void CgroupEventHandler::HandleApplicationStateChanged(uid_t uid, std::string bundleName, int32_t state)
{
    if (!supervisor_) {
        CGS_LOGE("%{public}s : supervisor nullptr!", __func__);
        return;
    }
    CGS_LOGD("%{public}s : %{public}d, %{public}s, %{public}d", __func__, uid, bundleName.c_str(), state);
    ChronoScope cs("HandleApplicationStateChanged");
    // remove terminated application
    if (state == VALUE_INT(ApplicationState::APP_STATE_TERMINATED)) {
        supervisor_->RemoveApplication(uid);
        return;
    }
    std::shared_ptr<Application> app = supervisor_->GetAppRecordNonNull(uid, bundleName);
    app->state_ = state;
}

void CgroupEventHandler::HandleAbilityStateChanged(uid_t uid, pid_t pid, std::string bundleName,
    std::string abilityName, sptr<IRemoteObject> token, int32_t abilityState, int32_t abilityType)
{
    if (!supervisor_) {
        CGS_LOGE("%{public}s : supervisor nullptr!", __func__);
        return;
    }
    CGS_LOGD("%{public}s : %{public}d, %{public}d, %{public}s, %{public}s, %{public}d, %{public}d",
        __func__, uid, pid, bundleName.c_str(), abilityName.c_str(), abilityState, abilityType);
    ChronoScope cs("HandleAbilityStateChanged");
    if (abilityState == VALUE_INT(AbilityState::ABILITY_STATE_TERMINATED)) {
        auto app = supervisor_->GetAppRecord(uid);
        if (app) {
            auto procRecord = app->GetProcessRecord(pid);
            if (procRecord) {
                procRecord->RemoveAbilityByToken(token);
            }
        }
        return;
    }
    auto app = supervisor_->GetAppRecordNonNull(uid, bundleName);
    auto procRecord = app->GetProcessRecordNonNull(pid, abilityName);
    auto abiInfo = procRecord->GetAbilityInfoNonNull(token);
    abiInfo->state_ = abilityState;
    abiInfo->type_ = abilityType;
    SchedController::GetInstance().AdjustProcessGroup(*(app.get()), *(procRecord.get()),
        AdjustSource::ADJS_ABILITY_STATE);
}

void CgroupEventHandler::HandleExtensionStateChanged(uid_t uid, pid_t pid, std::string bundleName,
    std::string abilityName, sptr<IRemoteObject> token, int32_t extensionState, int32_t abilityType)
{
    if (!supervisor_) {
        CGS_LOGE("%{public}s : supervisor nullptr!", __func__);
        return;
    }
    CGS_LOGD("%{public}s : %{public}d, %{public}d, %{public}s, %{public}s, %{public}d, %{public}d",
        __func__, uid, pid, bundleName.c_str(), abilityName.c_str(), extensionState, abilityType);
    ChronoScope cs("HandleExtensionStateChanged");
    if (extensionState == VALUE_INT(ExtensionState::EXTENSION_STATE_TERMINATED)) {
        auto app = supervisor_->GetAppRecord(uid);
        if (app) {
            auto procRecord = app->GetProcessRecord(pid);
            if (procRecord) {
                procRecord->RemoveAbilityByToken(token);
            }
        }
        return;
    }
    auto app = supervisor_->GetAppRecordNonNull(uid, bundleName);
    auto procRecord = app->GetProcessRecordNonNull(pid, abilityName);
    auto abiInfo = procRecord->GetAbilityInfoNonNull(token);
    abiInfo->estate_ = extensionState;
    abiInfo->type_ = abilityType;
    SchedController::GetInstance().AdjustProcessGroup(*(app.get()), *(procRecord.get()),
        AdjustSource::ADJS_EXTENSION_STATE);
}

void CgroupEventHandler::HandleProcessCreated(uid_t uid, pid_t pid, std::string bundleName)
{
    if (!supervisor_) {
        CGS_LOGE("%{public}s : supervisor nullptr!", __func__);
        return;
    }
    CGS_LOGD("%{public}s : %{public}d, %{public}d, %{public}s", __func__, uid, pid, bundleName.c_str());
    ChronoScope cs("HandleProcessCreated");
    std::shared_ptr<Application> app = supervisor_->GetAppRecordNonNull(uid, bundleName);
    std::shared_ptr<ProcessRecord> procRecord = std::make_shared<ProcessRecord>(uid, pid, bundleName);
    app->AddProcessRecord(procRecord);
    SchedController::GetInstance().AdjustProcessGroup(*(app.get()), *(procRecord.get()),
        AdjustSource::ADJS_PROCESS_CREATE);
}

void CgroupEventHandler::HandleProcessDied(uid_t uid, pid_t pid, std::string bundleName)
{
    if (!supervisor_) {
        CGS_LOGE("%{public}s : supervisor nullptr!", __func__);
        return;
    }
    CGS_LOGD("%{public}s : %{public}d, %{public}d, %{public}s", __func__, uid, pid, bundleName.c_str());
    std::shared_ptr<Application> app = supervisor_->GetAppRecord(uid);
    if (!app) {
        CGS_LOGE("%{public}s : application %{public}s not exist!", __func__, bundleName.c_str());
        return;
    }
    app->RemoveProcessRecord(pid);
    // if all processes died, remove current app
    if (app->GetPidsMap().size() == 0) {
        supervisor_->RemoveApplication(uid);
    }
}

void CgroupEventHandler::HandleTransientTaskStart(uid_t uid, pid_t pid, std::string packageName)
{
    if (!supervisor_) {
        CGS_LOGE("%{public}s : supervisor nullptr!", __func__);
        return;
    }
    CGS_LOGD("%{public}s : %{public}d, %{public}d, %{public}s", __func__, uid, pid, packageName.c_str());
    auto app = supervisor_->GetAppRecordNonNull(uid, packageName);
    auto procRecord = app->GetProcessRecord(pid);
    if (!procRecord) {
        return;
    }
    procRecord->runningTransientTask_ = true;
}

void CgroupEventHandler::HandleTransientTaskEnd(uid_t uid, pid_t pid, std::string packageName)
{
    if (!supervisor_) {
        CGS_LOGE("%{public}s : supervisor nullptr!", __func__);
        return;
    }
    CGS_LOGD("%{public}s : %{public}d, %{public}d, %{public}s", __func__, uid, pid, packageName.c_str());
    auto app = supervisor_->GetAppRecordNonNull(uid, packageName);
    auto procRecord = app->GetProcessRecord(pid);
    if (!procRecord) {
        return;
    }
    procRecord->runningTransientTask_ = false;
}

void CgroupEventHandler::HandleContinuousTaskStart(uid_t uid, pid_t pid, std::string abilityName)
{
    if (!supervisor_) {
        CGS_LOGE("%{public}s : supervisor nullptr!", __func__);
        return;
    }
    CGS_LOGD("%{public}s : %{public}d, %{public}d, %{public}s", __func__, uid, pid, abilityName.c_str());
    ChronoScope cs("HandleContinuousTaskStart");
    auto app = supervisor_->GetAppRecordNonNull(uid, abilityName);
    auto procRecord = app->GetProcessRecord(pid);
    if (!procRecord) {
        return;
    }
    procRecord->runningContinuousTask_ = true;
    SchedController::GetInstance().AdjustProcessGroup(*(app.get()), *(procRecord.get()),
        AdjustSource::ADJS_CONTINUOUS_BEGIN);
}

void CgroupEventHandler::HandleContinuousTaskCancel(uid_t uid, pid_t pid, std::string abilityName)
{
    if (!supervisor_) {
        CGS_LOGE("%{public}s : supervisor nullptr!", __func__);
        return;
    }
    CGS_LOGD("%{public}s : %{public}d, %{public}d, %{public}s", __func__, uid, pid, abilityName.c_str());
    ChronoScope cs("HandleContinuousTaskCancel");
    auto app = supervisor_->GetAppRecordNonNull(uid, abilityName);
    auto procRecord = app->GetProcessRecord(pid);
    if (!procRecord) {
        return;
    }
    procRecord->runningContinuousTask_ = false;
    SchedController::GetInstance().AdjustProcessGroup(*(app.get()), *(procRecord.get()),
        AdjustSource::ADJS_CONTINUOUS_END);
}

void CgroupEventHandler::HandleFocusedWindow(uint32_t windowId, sptr<IRemoteObject> abilityToken,
    WindowType windowType, uint64_t displayId, int32_t pid, int32_t uid)
{
    if (!supervisor_) {
        CGS_LOGE("%{public}s : supervisor nullptr!", __func__);
        return;
    }
    CGS_LOGD("%{public}s : %{public}d, %{public}d, %{public}llu", __func__, windowId,
        windowType, displayId);
    if (!abilityToken) {
        CGS_LOGW("%{public}s : abilityToken nullptr!", __func__);
    }
    std::shared_ptr<Application> app = nullptr;
    std::shared_ptr<ProcessRecord> procRecord = nullptr;
    {
        ChronoScope cs("HandleFocusedWindow");
        supervisor_->SearchAbilityToken(app, procRecord, abilityToken);
        if (!app || !procRecord) {
            supervisor_->SearchWindowId(app, procRecord, windowId);
            if (!app || !procRecord) {
                return;
            }
        }
        CGS_LOGD("%{public}s : focused window belongs to %{public}s %{public}d",
            __func__, app->GetName().c_str(), procRecord->GetPid());

        auto win = procRecord->GetWindowInfoNonNull(windowId);
        auto abi = procRecord->GetAbilityInfo(abilityToken);
        win->windowType_ = VALUE_INT(windowType);
        win->isFocused_ = true;
        win->displayId_ = displayId;
        win->ability_ = abi;
        if (abi) {
            abi->window_ = win;
        }

        app->focusedProcess_ = procRecord;
        auto lastFocusApp = supervisor_->focusedApp_;
        if (lastFocusApp && lastFocusApp != app) {
            lastFocusApp->focusedProcess_ = nullptr;
            SchedController::GetInstance().AdjustAllProcessGroup(*(lastFocusApp.get()),
                AdjustSource::ADJS_FOCUSED_WINDOW);
        }
        supervisor_->focusedApp_ = app;
        SchedController::GetInstance().AdjustAllProcessGroup(*(app.get()), AdjustSource::ADJS_FOCUSED_WINDOW);
    }

    Json::Value payload;
    payload["pid"] = procRecord->GetPid();
    payload["uid"] = procRecord->GetUid();
    payload["bundleName"] = app->GetName();
    payload["windowId"] = windowId;
    payload["windowType"] = VALUE_INT(windowType);
    payload["displayId"] = displayId;
    ResSchedUtils::GetInstance().ReportDataInProcess(ResType::RES_TYPE_WINDOW_FOCUS, 0, payload);
}

void CgroupEventHandler::HandleUnfocusedWindow(uint32_t windowId, sptr<IRemoteObject> abilityToken,
    WindowType windowType, uint64_t displayId, int32_t pid, int32_t uid)
{
    if (!supervisor_) {
        CGS_LOGE("%{public}s : supervisor nullptr!", __func__);
        return;
    }
    CGS_LOGD("%{public}s : %{public}d, %{public}d, %{public}llu", __func__, windowId,
        windowType, displayId);
    if (!abilityToken) {
        CGS_LOGW("%{public}s : abilityToken nullptr!", __func__);
    }
    std::shared_ptr<Application> app = nullptr;
    std::shared_ptr<ProcessRecord> procRecord = nullptr;
    {
        ChronoScope cs("HandleUnfocusedWindow");
        supervisor_->SearchAbilityToken(app, procRecord, abilityToken);
        if (!app || !procRecord) {
            supervisor_->SearchWindowId(app, procRecord, windowId);
            if (!app || !procRecord) {
                return;
            }
        }
        CGS_LOGD("%{public}s : unfocused window belongs to %{public}s %{public}d",
            __func__, app->GetName().c_str(), procRecord->GetPid());

        auto win = procRecord->GetWindowInfoNonNull(windowId);
        auto abi = procRecord->GetAbilityInfo(abilityToken);
        win->windowType_ = VALUE_INT(windowType);
        win->isFocused_ = false;
        win->displayId_ = displayId;
        win->ability_ = abi;
        if (abi) {
            abi->window_ = nullptr;
        }

        if (app->focusedProcess_ == procRecord) {
            app->focusedProcess_ = nullptr;
        }
        SchedController::GetInstance().AdjustAllProcessGroup(*(app.get()), AdjustSource::ADJS_UNFOCUSED_WINDOW);
    }
    Json::Value payload;
    payload["pid"] = procRecord->GetPid();
    payload["uid"] = procRecord->GetUid();
    payload["bundleName"] = app->GetName();
    payload["windowId"] = windowId;
    payload["windowType"] = VALUE_INT(windowType);
    payload["displayId"] = displayId;
    ResSchedUtils::GetInstance().ReportDataInProcess(ResType::RES_TYPE_WINDOW_FOCUS, 1, payload);
}

void CgroupEventHandler::HandleWindowVisibilityChanged(uint32_t windowId, bool isVisible, int32_t pid, int32_t uid)
{
    if (!supervisor_) {
        CGS_LOGE("%{public}s : supervisor nullptr!", __func__);
        return;
    }
    CGS_LOGD("%{public}s : %{public}d, %{public}d, %{public}d, %{public}d", __func__, windowId,
        isVisible, pid, uid);

    auto app = supervisor_->GetAppRecord(uid);
    if (!app) {
        return;
    }
    auto procRecord = app->GetProcessRecord(pid);
    if (!procRecord) {
        return;
    }
    auto windowInfo = procRecord->GetWindowInfoNonNull(windowId);
    windowInfo->isVisible_ = isVisible;

    SchedController::GetInstance().AdjustProcessGroup(*(app.get()), *(procRecord.get()),
        AdjustSource::ADJS_WINDOW_VISIBILITY_CHANGED);
}
} // namespace ResourceSchedule
} // namespace OHOS
