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
#include "system_ability_definition.h"

namespace OHOS {
namespace ResourceSchedule {
namespace {
    constexpr HiviewDFX::HiLogLabel LOG_LABEL = {LOG_CORE, LOG_TAG_DOMAIN_ID_RMS, "CgroupEventHandler"};
    constexpr uint32_t EVENT_ID_REG_APP_STATE_OBSERVER = 1;
    constexpr uint32_t EVENT_ID_REG_BGTASK_OBSERVER = 2;
    constexpr uint32_t DELAYED_RETRY_REGISTER_DURATION = 100;
    constexpr uint32_t MAX_RETRY_TIMES = 100;
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
    CGS_LOGD("%{public}s : eventId:%{public}d param:%{public}llu",
        __func__, event->GetInnerEventId(), event->GetParam());
    switch (event->GetInnerEventId()) {
        case EVENT_ID_REG_APP_STATE_OBSERVER: {
                int64_t retry = event->GetParam();
                if (!SchedController::GetInstance().SubscribeAppState() &&
                    retry < MAX_RETRY_TIMES) {
                    auto event = AppExecFwk::InnerEvent::Get(EVENT_ID_REG_APP_STATE_OBSERVER, retry + 1);
                    this->SendEvent(event, DELAYED_RETRY_REGISTER_DURATION);
                }
                break;
            }
        case EVENT_ID_REG_BGTASK_OBSERVER: {
                int64_t retry = event->GetParam();
                if (!SchedController::GetInstance().SubscribeBackgroundTask() &&
                    retry < MAX_RETRY_TIMES) {
                    auto event = AppExecFwk::InnerEvent::Get(EVENT_ID_REG_BGTASK_OBSERVER, retry + 1);
                    this->SendEvent(event, DELAYED_RETRY_REGISTER_DURATION);
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

void CgroupEventHandler::HandleAbilityAdded(int32_t saId, const std::string& deviceId)
{
    switch (saId) {
        case APP_MGR_SERVICE_ID:
            this->RemoveEvent(EVENT_ID_REG_APP_STATE_OBSERVER);
            if (!SchedController::GetInstance().SubscribeAppState()) {
                auto event = AppExecFwk::InnerEvent::Get(EVENT_ID_REG_APP_STATE_OBSERVER, 0);
                this->SendEvent(event, DELAYED_RETRY_REGISTER_DURATION);
            }
            break;
        case WINDOW_MANAGER_SERVICE_ID:
            SchedController::GetInstance().SubscribeWindowState();
            break;
        case BACKGROUND_TASK_MANAGER_SERVICE_ID:
            this->RemoveEvent(EVENT_ID_REG_BGTASK_OBSERVER);
            if (!SchedController::GetInstance().SubscribeBackgroundTask()) {
                auto event = AppExecFwk::InnerEvent::Get(EVENT_ID_REG_BGTASK_OBSERVER, 0);
                this->SendEvent(event, DELAYED_RETRY_REGISTER_DURATION);
            }
            break;
        default:
            break;
    }
}

void CgroupEventHandler::HandleAbilityRemoved(int32_t saId, const std::string& deviceId)
{
    switch (saId) {
        case APP_MGR_SERVICE_ID:
            this->RemoveEvent(EVENT_ID_REG_APP_STATE_OBSERVER);
            SchedController::GetInstance().UnsubscribeAppState();
            break;
        case WINDOW_MANAGER_SERVICE_ID:
            SchedController::GetInstance().UnsubscribeWindowState();
            break;
        case BACKGROUND_TASK_MANAGER_SERVICE_ID:
            this->RemoveEvent(EVENT_ID_REG_BGTASK_OBSERVER);
            SchedController::GetInstance().UnsubscribeBackgroundTask();
            break;
        default:
            break;
    }
}

void CgroupEventHandler::HandleForegroundApplicationChanged(uid_t uid, std::string bundleName, int32_t state)
{
    if (!supervisor_) {
        CGS_LOGE("%{public}s : supervisor nullptr!", __func__);
        return;
    }
    CGS_LOGD("%{public}s : %{public}d, %{public}s, %{public}d", __func__, uid, bundleName.c_str(), state);
    ChronoScope cs("HandleForegroundApplicationChanged");
    std::shared_ptr<Application> app = supervisor_->GetAppRecordNonNull(uid);
    app->name_ = bundleName;
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
    std::shared_ptr<Application> app = supervisor_->GetAppRecordNonNull(uid);
    app->name_ = bundleName;
    app->state_ = state;
}

void CgroupEventHandler::HandleAbilityStateChanged(uid_t uid, pid_t pid, std::string bundleName,
    std::string abilityName, uint64_t token, int32_t abilityState, int32_t abilityType)
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
                SchedController::GetInstance().AdjustProcessGroup(*(app.get()), *(procRecord.get()),
                    AdjustSource::ADJS_ABILITY_STATE);
            }
        }
        return;
    }
    auto app = supervisor_->GetAppRecordNonNull(uid);
    app->name_ = bundleName;
    auto procRecord = app->GetProcessRecordNonNull(pid);
    auto abiInfo = procRecord->GetAbilityInfoNonNull(token);
    abiInfo->state_ = abilityState;
    abiInfo->type_ = abilityType;
    SchedController::GetInstance().AdjustProcessGroup(*(app.get()), *(procRecord.get()),
        AdjustSource::ADJS_ABILITY_STATE);
}

void CgroupEventHandler::HandleExtensionStateChanged(uid_t uid, pid_t pid, std::string bundleName,
    std::string abilityName, uint64_t token, int32_t extensionState, int32_t abilityType)
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
                SchedController::GetInstance().AdjustProcessGroup(*(app.get()), *(procRecord.get()),
                    AdjustSource::ADJS_EXTENSION_STATE);
            }
        }
        return;
    }
    auto app = supervisor_->GetAppRecordNonNull(uid);
    app->name_ = bundleName;
    auto procRecord = app->GetProcessRecordNonNull(pid);
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
    std::shared_ptr<Application> app = supervisor_->GetAppRecordNonNull(uid);
    app->name_ = bundleName;
    std::shared_ptr<ProcessRecord> procRecord = std::make_shared<ProcessRecord>(uid, pid);
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
    auto app = supervisor_->GetAppRecordNonNull(uid);
    app->name_ = packageName;
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
    auto app = supervisor_->GetAppRecordNonNull(uid);
    app->name_ = packageName;
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
    auto app = supervisor_->GetAppRecordNonNull(uid);
    auto procRecord = app->GetProcessRecordNonNull(pid);
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
    auto app = supervisor_->GetAppRecordNonNull(uid);
    auto procRecord = app->GetProcessRecord(pid);
    if (!procRecord) {
        return;
    }
    procRecord->runningContinuousTask_ = false;
    SchedController::GetInstance().AdjustProcessGroup(*(app.get()), *(procRecord.get()),
        AdjustSource::ADJS_CONTINUOUS_END);
}

void CgroupEventHandler::HandleFocusedWindow(uint32_t windowId, uint64_t abilityToken,
    WindowType windowType, uint64_t displayId, int32_t pid, int32_t uid)
{
    Json::Value payload;
    payload["pid"] = pid;
    payload["uid"] = uid;
    payload["windowId"] = windowId;
    payload["windowType"] = VALUE_INT(windowType);
    payload["displayId"] = displayId;

    if (!supervisor_) {
        CGS_LOGE("%{public}s : supervisor nullptr!", __func__);
        payload["bundleName"] = "";
        ResSchedUtils::GetInstance().ReportDataInProcess(ResType::RES_TYPE_WINDOW_FOCUS, 0, payload);
        return;
    }
    CGS_LOGD("%{public}s : %{public}d, %{public}d, %{public}llu, %{public}d, %{public}d",
        __func__, windowId, windowType, displayId, pid, uid);
    if (!abilityToken) {
        CGS_LOGW("%{public}s : abilityToken nullptr!", __func__);
    }
    std::shared_ptr<Application> app = nullptr;
    std::shared_ptr<ProcessRecord> procRecord = nullptr;
    {
        ChronoScope cs("HandleFocusedWindow");
        app = supervisor_->GetAppRecordNonNull(uid);
        procRecord = app->GetProcessRecordNonNull(pid);
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
    payload["bundleName"] = app->name_;
    ResSchedUtils::GetInstance().ReportDataInProcess(ResType::RES_TYPE_WINDOW_FOCUS, 0, payload);
}

void CgroupEventHandler::HandleUnfocusedWindow(uint32_t windowId, uint64_t abilityToken,
    WindowType windowType, uint64_t displayId, int32_t pid, int32_t uid)
{
    Json::Value payload;
    payload["pid"] = pid;
    payload["uid"] = uid;
    payload["windowId"] = windowId;
    payload["windowType"] = VALUE_INT(windowType);
    payload["displayId"] = displayId;

    if (!supervisor_) {
        CGS_LOGE("%{public}s : supervisor nullptr!", __func__);
        payload["bundleName"] = "";
        ResSchedUtils::GetInstance().ReportDataInProcess(ResType::RES_TYPE_WINDOW_FOCUS, 1, payload);
        return;
    }
    CGS_LOGD("%{public}s : %{public}d, %{public}d, %{public}llu, %{public}d, %{public}d",
        __func__, windowId, windowType, displayId, pid, uid);
    if (!abilityToken) {
        CGS_LOGW("%{public}s : abilityToken nullptr!", __func__);
    }
    std::shared_ptr<Application> app = nullptr;
    std::shared_ptr<ProcessRecord> procRecord = nullptr;
    {
        ChronoScope cs("HandleUnfocusedWindow");
        app = supervisor_->GetAppRecord(uid);
        if (!app) {
            payload["bundleName"] = "";
            ResSchedUtils::GetInstance().ReportDataInProcess(ResType::RES_TYPE_WINDOW_FOCUS, 1, payload);
            return;
        }
        procRecord = app->GetProcessRecord(pid);
        if (!procRecord) {
            payload["bundleName"] = "";
            ResSchedUtils::GetInstance().ReportDataInProcess(ResType::RES_TYPE_WINDOW_FOCUS, 1, payload);
            return;
        }
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
    payload["bundleName"] = app->name_;
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

    std::shared_ptr<Application> app = nullptr;
    std::shared_ptr<ProcessRecord> procRecord = nullptr;
    if (isVisible) {
        app = supervisor_->GetAppRecordNonNull(uid);
        procRecord = app->GetProcessRecordNonNull(pid);
    } else {
        app = supervisor_->GetAppRecord(uid);
        if (app) {
            procRecord = app->GetProcessRecord(pid);
        }
    }
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
