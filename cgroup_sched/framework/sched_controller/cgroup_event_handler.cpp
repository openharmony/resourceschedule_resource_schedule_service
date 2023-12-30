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
#include <cinttypes>
#include "app_mgr_constants.h"
#include "cgroup_adjuster.h"
#include "cgroup_sched_common.h"
#include "cgroup_sched_log.h"
#include "ffrt_inner.h"
#include "hisysevent.h"
#include "ressched_utils.h"
#include "res_type.h"
#include "sched_controller.h"
#include "sched_policy.h"
#include "system_ability_definition.h"
#ifdef POWER_MANAGER_ENABLE
#include "power_mgr_client.h"
#endif
#include "window_manager.h"

namespace OHOS {
namespace ResourceSchedule {
namespace {
    constexpr HiviewDFX::HiLogLabel LOG_LABEL = {LOG_CORE, LOG_TAG_DOMAIN_ID_RMS, "CgroupEventHandler"};
    constexpr uint32_t EVENT_ID_REG_APP_STATE_OBSERVER = 1;
    constexpr uint32_t EVENT_ID_REG_BGTASK_OBSERVER = 2;
    constexpr uint32_t DELAYED_RETRY_REGISTER_DURATION = 100;
    constexpr uint32_t MAX_RETRY_TIMES = 100;
    constexpr uint32_t MAX_SPAN_SERIAL = 99;
    constexpr const uint32_t TIME_US_TO_MS = 1000;

    const std::string MMI_SERVICE_NAME = "mmi_service";
}

using OHOS::AppExecFwk::ApplicationState;
using OHOS::AppExecFwk::AbilityState;
using OHOS::AppExecFwk::ExtensionState;
using OHOS::AppExecFwk::ProcessType;

CgroupEventHandler::CgroupEventHandler()
{
    queue_ = std::make_shared<ffrt::queue>("cgroup_event_queue",
    ffrt::queue_attr().qos(ffrt::qos_user_interactive));
    if (!queue_) {
        CGS_LOGE("create queue_ failed!");
    }
}

CgroupEventHandler::~CgroupEventHandler()
{
    supervisor_ = nullptr;
}

void CgroupEventHandler::ProcessEvent(int32_t eventId, int32_t retryTimes)
{
    CGS_LOGD("%{public}s : eventId:%{public}d param:%{public}d",
        __func__, eventId, retryTimes);
    switch (eventId) {
        case EVENT_ID_REG_APP_STATE_OBSERVER: {
            if (!SchedController::GetInstance().SubscribeAppState() &&
                retryTimes < static_cast<int32_t>(MAX_RETRY_TIMES)) {
                    this->SubmitH([this, eventId, retryTimes]() {
                    this->ProcessEvent(EVENT_ID_REG_APP_STATE_OBSERVER, retryTimes + 1);
                        }, EVENT_ID_REG_APP_STATE_OBSERVER, DELAYED_RETRY_REGISTER_DURATION);
                if(retryTimes + 1 == static_cast<int32_t>(MAX_RETRY_TIMES)) {
                    HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::RSS, "INIT_FAULT", HiviewDFX::HiSysEvent::EventType::FAULT,
                    "COMPONENT_NAME", "MAIN",
                    "ERR_TYPE", "register failure",
                    "ERR_MSG", "Subscribe app status change observer failed.");
                }
            } else {
                RemoveEvent(EVENT_ID_REG_APP_STATE_OBSERVER);
            }
            break;
        }
        case EVENT_ID_REG_BGTASK_OBSERVER: {
            if (!SchedController::GetInstance().SubscribeBackgroundTask() &&
                retryTimes < static_cast<int32_t>(MAX_RETRY_TIMES)) {
                    this->SubmitH([this, eventId, retryTimes]() {
                    this->ProcessEvent(EVENT_ID_REG_BGTASK_OBSERVER, retryTimes + 1);
                        }, EVENT_ID_REG_BGTASK_OBSERVER, DELAYED_RETRY_REGISTER_DURATION);
                if(retryTimes + 1 == static_cast<int32_t>(MAX_RETRY_TIMES)) {
                    HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::RSS, "INIT_FAULT", HiviewDFX::HiSysEvent::EventType::FAULT,
                    "COMPONENT_NAME", "MAIN",
                    "ERR_TYPE", "register failure",
                    "ERR_MSG", "Subscribe background task observer failed.");
                }
            } else {
                RemoveEvent(EVENT_ID_REG_BGTASK_OBSERVER);
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

void CgroupEventHandler::RemoveAllEvents()
{
    if (!CheckQueuePtr()) {
        return;
    }
    queue_.reset();
    eventMap_.clear();
}

void CgroupEventHandler::RemoveEvent(uint32_t eventId)
{
    if (!CheckQueuePtr()) {
        return;
    }
    if (eventMap_.find(eventId) != eventMap_.end()) {
        for (size_t i = 0; i < eventMap_[eventId].size(); i++) {
            queue_->cancel(eventMap_[eventId][i]);
        }
        eventMap_.erase(eventId);
    }
}

void CgroupEventHandler::HandleAbilityAdded(int32_t saId, const std::string& deviceId)
{
    switch (saId) {
        case APP_MGR_SERVICE_ID:
            this->RemoveEvent(EVENT_ID_REG_APP_STATE_OBSERVER);
            if (!SchedController::GetInstance().SubscribeAppState()) {
                this->SubmitH([this]() {
                    this->ProcessEvent(EVENT_ID_REG_APP_STATE_OBSERVER, 1);
                    }, EVENT_ID_REG_APP_STATE_OBSERVER, DELAYED_RETRY_REGISTER_DURATION);
            }
            break;
        case WINDOW_MANAGER_SERVICE_ID:
            SchedController::GetInstance().SubscribeWindowState();
            break;
        case BACKGROUND_TASK_MANAGER_SERVICE_ID:
            this->RemoveEvent(EVENT_ID_REG_BGTASK_OBSERVER);
            if (!SchedController::GetInstance().SubscribeBackgroundTask()) {
                this->SubmitH([this]() {
                    this->ProcessEvent(EVENT_ID_REG_BGTASK_OBSERVER, 1);
                    }, EVENT_ID_REG_BGTASK_OBSERVER, DELAYED_RETRY_REGISTER_DURATION);
            }
            break;
#ifdef POWER_MANAGER_ENABLE
        case POWER_MANAGER_SERVICE_ID:
            SchedController::GetInstance().GetRunningLockState();
            break;
#endif
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

void CgroupEventHandler::HandleApplicationStateChanged(uid_t uid, pid_t pid,
    const std::string& bundleName, int32_t state)
{
    if (!supervisor_) {
        CGS_LOGE("%{public}s : supervisor nullptr!", __func__);
        return;
    }
    CGS_LOGD("%{public}s : %{public}d, %{public}s, %{public}d", __func__, uid, bundleName.c_str(), state);
    ChronoScope cs("HandleApplicationStateChanged");
    // remove terminated application
    if (state == (int32_t)(ApplicationState::APP_STATE_TERMINATED)) {
        supervisor_->RemoveApplication(uid);
        return;
    }
    std::shared_ptr<Application> app = supervisor_->GetAppRecordNonNull(uid);
    std::shared_ptr<ProcessRecord> procRecord = app->GetProcessRecordNonNull(pid);
    app->SetName(bundleName);
    app->state_ = state;
    app->SetMainProcess(procRecord);
}

void CgroupEventHandler::HandleProcessStateChanged(uid_t uid, pid_t pid,
    const std::string& bundleName, int32_t state)
{
    if (!supervisor_) {
        CGS_LOGE("%{public}s : supervisor nullptr!", __func__);
        return;
    }
    CGS_LOGD("%{public}s : %{public}d, %{public}d, %{public}s, %{public}d", __func__, uid,
        pid, bundleName.c_str(), state);
    ChronoScope cs("HandleProcessStateChanged");
    std::shared_ptr<Application> app = supervisor_->GetAppRecordNonNull(uid);
    std::shared_ptr<ProcessRecord> procRecord = app->GetProcessRecordNonNull(pid);
    procRecord->processState_ = state;
    CgroupAdjuster::GetInstance().AdjustProcessGroup(*(app.get()), *(procRecord.get()),
        AdjustSource::ADJS_PROCESS_STATE);
}

void CgroupEventHandler::HandleAbilityStateChanged(uid_t uid, pid_t pid, const std::string& bundleName,
    const std::string& abilityName, uintptr_t token, int32_t abilityState, int32_t abilityType)
{
    if (!supervisor_) {
        CGS_LOGE("%{public}s : supervisor nullptr!", __func__);
        return;
    }
    CGS_LOGD("%{public}s : %{public}d, %{public}d, %{public}s, %{public}s, %{public}d, %{public}d",
        __func__, uid, pid, bundleName.c_str(), abilityName.c_str(), abilityState, abilityType);
    ChronoScope cs("HandleAbilityStateChanged");
    if (abilityState == (int32_t)(AbilityState::ABILITY_STATE_TERMINATED)) {
        auto app = supervisor_->GetAppRecord(uid);
        if (app) {
            auto procRecord = app->GetProcessRecord(pid);
            if (procRecord) {
                procRecord->RemoveAbilityByToken(token);
                CgroupAdjuster::GetInstance().AdjustProcessGroup(*(app.get()), *(procRecord.get()),
                    AdjustSource::ADJS_ABILITY_STATE);
            }
        }
        return;
    }
    auto app = supervisor_->GetAppRecordNonNull(uid);
    app->SetName(bundleName);
    auto procRecord = app->GetProcessRecordNonNull(pid);
    auto abiInfo = procRecord->GetAbilityInfoNonNull(token);
    abiInfo->name_ = abilityName;
    abiInfo->state_ = abilityState;
    abiInfo->type_ = abilityType;
    CgroupAdjuster::GetInstance().AdjustProcessGroup(*(app.get()), *(procRecord.get()),
        AdjustSource::ADJS_ABILITY_STATE);
}

void CgroupEventHandler::HandleExtensionStateChanged(uid_t uid, pid_t pid, const std::string& bundleName,
    const std::string& abilityName, uintptr_t token, int32_t extensionState, int32_t abilityType)
{
    if (!supervisor_) {
        CGS_LOGE("%{public}s : supervisor nullptr!", __func__);
        return;
    }
    CGS_LOGD("%{public}s : %{public}d, %{public}d, %{public}s, %{public}s, %{public}d, %{public}d",
        __func__, uid, pid, bundleName.c_str(), abilityName.c_str(), extensionState, abilityType);
    ChronoScope cs("HandleExtensionStateChanged");
    if (extensionState == (int32_t)(ExtensionState::EXTENSION_STATE_TERMINATED)) {
        auto app = supervisor_->GetAppRecord(uid);
        if (app) {
            auto procRecord = app->GetProcessRecord(pid);
            if (procRecord) {
                procRecord->RemoveAbilityByToken(token);
                CgroupAdjuster::GetInstance().AdjustProcessGroup(*(app.get()), *(procRecord.get()),
                    AdjustSource::ADJS_EXTENSION_STATE);
            }
        }
        return;
    }
    auto app = supervisor_->GetAppRecordNonNull(uid);
    app->SetName(bundleName);
    auto procRecord = app->GetProcessRecordNonNull(pid);
    auto abiInfo = procRecord->GetAbilityInfoNonNull(token);
    abiInfo->name_ = abilityName;
    abiInfo->estate_ = extensionState;
    abiInfo->type_ = abilityType;
    CgroupAdjuster::GetInstance().AdjustProcessGroup(*(app.get()), *(procRecord.get()),
        AdjustSource::ADJS_EXTENSION_STATE);
}

void CgroupEventHandler::HandleProcessCreated(uid_t uid, pid_t pid, int32_t processType,
    const std::string& bundleName, int32_t extensionType)
{
    if (!supervisor_) {
        CGS_LOGE("%{public}s : supervisor nullptr!", __func__);
        return;
    }
    CGS_LOGD("%{public}s : %{public}d, %{public}d, %{public}s", __func__, uid, pid, bundleName.c_str());
    ChronoScope cs("HandleProcessCreated");
    std::shared_ptr<Application> app = supervisor_->GetAppRecordNonNull(uid);
    std::shared_ptr<ProcessRecord> procRecord = app->GetProcessRecordNonNull(pid);
    app->SetName(bundleName);
    if (processType == static_cast<int32_t>(ProcessType::NORMAL)) {
        app->SetMainProcess(procRecord);
    } else if (processType == static_cast<int32_t>(ProcessType::RENDER)) {
        procRecord->isRenderProcess_ = true;
    } else if (processType == static_cast<int32_t>(ProcessType::EXTENSION)) {
        procRecord->isExtensionProcess_ = true;
        procRecord->extensionType_ = extensionType;
    }
    CgroupAdjuster::GetInstance().AdjustProcessGroup(*(app.get()), *(procRecord.get()),
        AdjustSource::ADJS_PROCESS_CREATE);
}

void CgroupEventHandler::HandleProcessDied(uid_t uid, pid_t pid, const std::string& bundleName)
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
    std::shared_ptr<ProcessRecord> procRecord = app->GetProcessRecord(pid);
    if (procRecord) {
        ResSchedUtils::GetInstance().ReportSysEvent(*(app.get()), *(procRecord.get()),
            ResType::RES_TYPE_PROCESS_STATE_CHANGE, ResType::ProcessStatus::PROCESS_DIED);
    }
    app->RemoveProcessRecord(pid);
    // if all processes died, remove current app
    if (app->GetPidsMap().size() == 0) {
        supervisor_->RemoveApplication(uid);
    }
}

void CgroupEventHandler::HandleTransientTaskStart(uid_t uid, pid_t pid, const std::string& packageName)
{
    if (!supervisor_) {
        CGS_LOGE("%{public}s : supervisor nullptr!", __func__);
        return;
    }
    CGS_LOGD("%{public}s : %{public}d, %{public}d, %{public}s", __func__, uid, pid, packageName.c_str());
    auto app = supervisor_->GetAppRecordNonNull(uid);
    app->SetName(packageName);
    auto procRecord = app->GetProcessRecord(pid);
    if (!procRecord) {
        return;
    }
    procRecord->runningTransientTask_ = true;
}

void CgroupEventHandler::HandleTransientTaskEnd(uid_t uid, pid_t pid, const std::string& packageName)
{
    if (!supervisor_) {
        CGS_LOGE("%{public}s : supervisor nullptr!", __func__);
        return;
    }
    CGS_LOGD("%{public}s : %{public}d, %{public}d, %{public}s", __func__, uid, pid, packageName.c_str());
    auto app = supervisor_->GetAppRecordNonNull(uid);
    app->SetName(packageName);
    auto procRecord = app->GetProcessRecord(pid);
    if (!procRecord) {
        return;
    }
    procRecord->runningTransientTask_ = false;
}

void CgroupEventHandler::HandleContinuousTaskStart(uid_t uid, pid_t pid, int32_t typeId,
    const std::string& abilityName)
{
    if (!supervisor_) {
        CGS_LOGE("%{public}s : supervisor nullptr!", __func__);
        return;
    }
    CGS_LOGD("%{public}s : %{public}d, %{public}d, %{public}d, %{public}s",
        __func__, uid, pid, typeId, abilityName.c_str());
    ChronoScope cs("HandleContinuousTaskStart");
    auto app = supervisor_->GetAppRecordNonNull(uid);
    auto procRecord = app->GetProcessRecordNonNull(pid);
    procRecord->continuousTaskFlag_ |= (1U << typeId);
    CgroupAdjuster::GetInstance().AdjustProcessGroup(*(app.get()), *(procRecord.get()),
        AdjustSource::ADJS_CONTINUOUS_BEGIN);
}

void CgroupEventHandler::HandleContinuousTaskCancel(uid_t uid, pid_t pid, int32_t typeId,
    const std::string& abilityName)
{
    if (!supervisor_) {
        CGS_LOGE("%{public}s : supervisor nullptr!", __func__);
        return;
    }
    CGS_LOGD("%{public}s : %{public}d, %{public}d, %{public}d, %{public}s",
        __func__, uid, pid, typeId, abilityName.c_str());
    ChronoScope cs("HandleContinuousTaskCancel");
    auto app = supervisor_->GetAppRecordNonNull(uid);
    auto procRecord = app->GetProcessRecord(pid);
    if (!procRecord) {
        return;
    }
    procRecord->continuousTaskFlag_ &= ~(1U << typeId);
    CgroupAdjuster::GetInstance().AdjustProcessGroup(*(app.get()), *(procRecord.get()),
        AdjustSource::ADJS_CONTINUOUS_END);
}

void CgroupEventHandler::HandleFocusedWindow(uint32_t windowId, uintptr_t abilityToken,
    WindowType windowType, uint64_t displayId, int32_t pid, int32_t uid)
{
    if (!supervisor_) {
        CGS_LOGE("%{public}s : supervisor nullptr!", __func__);
        return;
    }
    CGS_LOGD("%{public}s : %{public}d, %{public}d, %{public}" PRIu64 ", %{public}d, %{public}d",
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
        win->windowType_ = (int32_t)(windowType);
        win->isFocused_ = true;
        win->displayId_ = displayId;
        win->ability_ = abi;

        app->focusedProcess_ = procRecord;
        auto lastFocusApp = supervisor_->focusedApp_;
        if (lastFocusApp && lastFocusApp != app) {
            lastFocusApp->focusedProcess_ = nullptr;
            CgroupAdjuster::GetInstance().AdjustAllProcessGroup(*(lastFocusApp.get()),
                AdjustSource::ADJS_FOCUSED_WINDOW);
        }
        supervisor_->focusedApp_ = app;
        CgroupAdjuster::GetInstance().AdjustAllProcessGroup(*(app.get()), AdjustSource::ADJS_FOCUSED_WINDOW);
        ResSchedUtils::GetInstance().ReportSysEvent(*(app.get()), *(procRecord.get()), ResType::RES_TYPE_WINDOW_FOCUS,
            ResType::WindowFocusStatus::WINDOW_FOCUS);
    }
    if (app->GetName().empty()) {
        app->SetName(SchedController::GetInstance().GetBundleNameByUid(uid));
    }
}

void CgroupEventHandler::HandleUnfocusedWindow(uint32_t windowId, uintptr_t abilityToken,
    WindowType windowType, uint64_t displayId, int32_t pid, int32_t uid)
{
    if (!supervisor_) {
        CGS_LOGE("%{public}s : supervisor nullptr!", __func__);
        return;
    }
    CGS_LOGD("%{public}s : %{public}d, %{public}d, %{public}" PRIu64 ", %{public}d, %{public}d",
        __func__, windowId, windowType, displayId, pid, uid);
    if (!abilityToken) {
        CGS_LOGW("%{public}s : abilityToken nullptr!", __func__);
    }
    std::shared_ptr<Application> app = nullptr;
    std::shared_ptr<ProcessRecord> procRecord = nullptr;
    {
        ChronoScope cs("HandleUnfocusedWindow");
        app = supervisor_->GetAppRecord(uid);
        procRecord = app ? app->GetProcessRecord(pid) : nullptr;
        if (!app || !procRecord) {
            return;
        }
        auto win = procRecord->GetWindowInfoNonNull(windowId);
        auto abi = procRecord->GetAbilityInfo(abilityToken);
        win->windowType_ = (int32_t)(windowType);
        win->isFocused_ = false;
        win->displayId_ = displayId;
        win->ability_ = abi;

        if (app->focusedProcess_ == procRecord) {
            app->focusedProcess_ = nullptr;
        }
        CgroupAdjuster::GetInstance().AdjustAllProcessGroup(*(app.get()), AdjustSource::ADJS_UNFOCUSED_WINDOW);
        ResSchedUtils::GetInstance().ReportSysEvent(*(app.get()), *(procRecord.get()), ResType::RES_TYPE_WINDOW_FOCUS,
            ResType::WindowFocusStatus::WINDOW_UNFOCUS);
    }
    if (app->GetName().empty()) {
        app->SetName(SchedController::GetInstance().GetBundleNameByUid(uid));
    }
}

void CgroupEventHandler::HandleWindowVisibilityChanged(
    uint32_t windowId, uint32_t visibilityState, WindowType windowType, int32_t pid, int32_t uid)
{
    if (!supervisor_) {
        CGS_LOGE("%{public}s : supervisor nullptr!", __func__);
        return;
    }
    bool isVisible = visibilityState < Rosen::WindowVisibilityState::WINDOW_VISIBILITY_STATE_TOTALLY_OCCUSION;
    CGS_LOGD("%{public}s : %{public}d, %{public}d, %{public}d, %{public}d, %{public}d", __func__, windowId,
        visibilityState, (int32_t)windowType, pid, uid);

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
    windowInfo->visibilityState_ = visibilityState;
    windowInfo->isVisible_ = isVisible;
    windowInfo->windowType_ = (int32_t)windowType;

    CgroupAdjuster::GetInstance().AdjustProcessGroup(*(app.get()), *(procRecord.get()),
        AdjustSource::ADJS_WINDOW_VISIBILITY_CHANGED);
}

void CgroupEventHandler::HandleDrawingContentChangeWindow(
    uint32_t windowId, WindowType windowType, bool drawingContentState, int32_t pid, int32_t uid)
{
    if (!supervisor_) {
        CGS_LOGE("%{public}s : supervisor nullptr!", __func__);
        return;
    }
    CGS_LOGD("%{public}s : %{public}d, %{public}d, %{public}d, %{public}d, %{public}d", __func__, windowId,
        drawingContentState, (int32_t)windowType, pid, uid);

    std::shared_ptr<Application> app = supervisor_->GetAppRecord(uid);
    std::shared_ptr<ProcessRecord> procRecord = app ? app->GetProcessRecord(pid) : nullptr;
    if (!app || !procRecord) {
        return;
    }
    procRecord->processDrawingState_ = drawingContentState;
    auto windowInfo = procRecord->GetWindowInfoNonNull(windowId);
    windowInfo->drawingContentState_ = drawingContentState;
    ResSchedUtils::GetInstance().ReportSysEvent(*(app.get()), *(procRecord.get()),
        ResType::RES_TYPE_WINDOW_DRAWING_CONTENT_CHANGE,
        drawingContentState ? ResType::WindowDrawingStatus::Drawing : ResType::WindowDrawingStatus::NotDrawing);
}

void CgroupEventHandler::HandleReportMMIProcess(uint32_t resType, int64_t value, const nlohmann::json& payload)
{
    int32_t uid = 0;
    int32_t pid = 0;
    int32_t mmi_service;

    if (!supervisor_) {
        CGS_LOGE("%{public}s : supervisor nullptr!", __func__);
        return;
    }

    if (!ParsePayload(uid, pid, mmi_service, value, payload)) {
        return;
    }

    CGS_LOGD("%{public}s : %{public}u, %{public}d, %{public}d, %{public}d",
        __func__, resType, uid, pid, mmi_service);
    if (uid <= 0 || pid <= 0 || mmi_service <= 0) {
        return;
    }

    auto app = supervisor_->GetAppRecordNonNull(uid);
    app->SetName(MMI_SERVICE_NAME);
    auto procRecord = app->GetProcessRecordNonNull(mmi_service);
    CgroupAdjuster::GetInstance().AdjustProcessGroup(*(app.get()), *(procRecord.get()),
        AdjustSource::ADJS_REPORT_MMI_SERVICE_THREAD);
}

void CgroupEventHandler::HandleReportRenderThread(uint32_t resType, int64_t value, const nlohmann::json& payload)
{
    int32_t uid = 0;
    int32_t pid = 0;
    int32_t render = 0;

    if (!supervisor_) {
        CGS_LOGE("%{public}s : supervisor nullptr!", __func__);
        return;
    }

    if (!ParsePayload(uid, pid, render, value, payload)) {
        return;
    }

    CGS_LOGD("%{public}s : %{public}u, %{public}d, %{public}d, %{public}d",
        __func__, resType, uid, pid, render);
    if (uid <= 0 || pid <= 0 || render <= 0) {
        return;
    }

    auto app = supervisor_->GetAppRecordNonNull(uid);
    auto procRecord = app->GetProcessRecordNonNull(pid);
    procRecord->renderTid_ = render;
    CgroupAdjuster::GetInstance().AdjustProcessGroup(*(app.get()), *(procRecord.get()),
        AdjustSource::ADJS_REPORT_RENDER_THREAD);
}

void CgroupEventHandler::HandleReportKeyThread(uint32_t resType, int64_t value, const nlohmann::json& payload)
{
    int32_t uid = 0;
    int32_t pid = 0;
    int32_t keyTid = 0;
    int32_t role = 0;

    if (!supervisor_) {
        CGS_LOGE("%{public}s : supervisor nullptr!", __func__);
        return;
    }

    if (!ParseValue(uid, "uid", payload) || !ParseValue(pid, "pid", payload) ||
        !ParseValue(keyTid, "tid", payload) || !ParseValue(role, "role", payload)) {
        return;
    }

    if (uid <= 0 || pid <= 0) {
        return;
    }

    auto app = supervisor_->GetAppRecordNonNull(uid);
    auto procRecord = app->GetProcessRecordNonNull(pid);
    if (value == ResType::ReportChangeStatus::CREATE) {
        procRecord->keyThreadRoleMap_.emplace(keyTid, role);
    } else {
        procRecord->keyThreadRoleMap_.erase(keyTid);
    }

    // if role of thread is important display, adjust it
    auto mainProcRecord = app->GetMainProcessRecord();
    if (!mainProcRecord) {
        return;
    }
    CgroupAdjuster::GetInstance().AdjustProcessGroup(*(app.get()), *(mainProcRecord.get()),
        AdjustSource::ADJS_REPORT_IMPORTANT_DISPLAY_THREAD);
}

void CgroupEventHandler::HandleReportWindowState(uint32_t resType, int64_t value, const nlohmann::json& payload)
{
    int32_t uid = 0;
    int32_t pid = 0;
    int32_t windowId = -1;
    int32_t state = 0;
    int32_t nowSerialNum = -1;

    if (!supervisor_) {
        CGS_LOGE("%{public}s : supervisor nullptr!", __func__);
        return;
    }

    if (!ParseValue(uid, "uid", payload) || !ParseValue(pid, "pid", payload) ||
        !ParseValue(windowId, "windowId", payload) || !ParseValue(state, "state", payload) ||
        !ParseValue(nowSerialNum, "serialNum", payload)) {
        return;
    }
    if (uid <= 0 || pid <= 0) {
        return;
    }

    auto app = supervisor_->GetAppRecordNonNull(uid);
    auto procRecord = app->GetProcessRecordNonNull(pid);
    CGS_LOGD("%{public}s : render process name: %{public}s, uid: %{public}d, pid: %{public}d, state: %{public}d",
        __func__, app->GetName().c_str(), uid, pid, state);
    if (nowSerialNum <= procRecord->serialNum_ &&
        (procRecord->serialNum_ - nowSerialNum <= static_cast<int32_t>(MAX_SPAN_SERIAL))) {
        return;
    }
    procRecord->serialNum_ = nowSerialNum;

    if (state == ResType::WindowStates::ACTIVE) {
        procRecord->linkedWindowId_ = windowId;
        procRecord->isActive_ = true;
    } else {
        procRecord->linkedWindowId_ = -1;
        procRecord->isActive_ = false;
    }
    auto mainProcRecord = app->GetMainProcessRecord();
    if (!mainProcRecord) {
        return;
    }
    if (CheckVisibilityForRenderProcess(*(procRecord.get()), *mainProcRecord)) {
        CGS_LOGW("%{public}s : bundle name: %{public}s, uid: %{public}d, pid: %{public}d is not visible but active",
            __func__, app->GetName().c_str(), uid, pid);
    }
    CgroupAdjuster::GetInstance().AdjustProcessGroup(*(app.get()), *(mainProcRecord.get()),
        AdjustSource::ADJS_REPORT_WINDOW_STATE_CHANGED);
}

void CgroupEventHandler::HandleReportAudioState(uint32_t resType, int64_t value, const nlohmann::json& payload)
{
    int32_t uid = 0;
    int32_t pid = 0;

    if (!supervisor_) {
        CGS_LOGE("%{public}s : supervisor nullptr!", __func__);
        return;
    }

    if (payload.contains("uid") && payload.at("uid").is_number_integer()) {
        uid = payload["uid"].get<std::int32_t>();
    }
    if (payload.contains("pid") && payload.at("pid").is_number_integer()) {
        pid = payload["pid"].get<std::int32_t>();
    }
    if (uid <= 0 || pid <= 0) {
        return;
    }

    std::shared_ptr<Application> app = supervisor_->GetAppRecord(uid);
    std::shared_ptr<ProcessRecord> procRecord = app ? app->GetProcessRecord(pid) : nullptr;
    if (!app || !procRecord) {
        return;
    }
    procRecord->audioState_ = static_cast<int32_t>(value);
    CGS_LOGD("%{public}s : audio process name: %{public}s, uid: %{public}d, pid: %{public}d, state: %{public}d",
        __func__, app->GetName().c_str(), uid, pid, procRecord->audioState_);

    CgroupAdjuster::GetInstance().AdjustProcessGroup(*(app.get()), *(procRecord.get()),
        AdjustSource::ADJS_REPORT_AUDIO_STATE_CHANGED);
    ResSchedUtils::GetInstance().ReportSysEvent(*(app.get()), *(procRecord.get()),
        resType, static_cast<int32_t>(value));
}

void CgroupEventHandler::HandleReportWebviewAudioState(uint32_t resType, int64_t value, const nlohmann::json& payload)
{
    int32_t uid = 0;
    int32_t pid = 0;

    if (!supervisor_) {
        CGS_LOGE("%{public}s : supervisor nullptr!", __func__);
        return;
    }

    if (!ParseValue(uid, "uid", payload) || !ParseValue(pid, "pid", payload)) {
        return;
    }
    if (uid <= 0 || pid <= 0) {
        return;
    }

    std::shared_ptr<ProcessRecord> procRecord = supervisor_->FindProcessRecord(pid);
    if (!procRecord) {
        return;
    }

    std::shared_ptr<Application> app = supervisor_->GetAppRecordNonNull(procRecord->GetUid());
    procRecord->audioState_ = static_cast<int32_t>(value);
    CGS_LOGD("%{public}s : audio process name: %{public}s, uid: %{public}d, pid: %{public}d, state: %{public}d",
        __func__, app->GetName().c_str(), uid, pid, procRecord->audioState_);

    CgroupAdjuster::GetInstance().AdjustProcessGroup(*(app.get()), *(procRecord.get()),
        AdjustSource::ADJS_REPORT_WEBVIEW_STATE_CHANGED);
}

void CgroupEventHandler::HandleReportRunningLockEvent(uint32_t resType, int64_t value, const nlohmann::json& payload)
{
    int32_t uid = 0;
    int32_t pid = 0;
    uint32_t type = 0;
    int32_t state = -1;

    if (!supervisor_) {
        CGS_LOGE("%{public}s : supervisor nullptr.", __func__);
        return;
    }

    if (!ParseValue(uid, "uid", payload) || !ParseValue(pid, "pid", payload)) {
        return;
    }
    if (uid <= 0 || pid <= 0) {
        return;
    }
    if (payload.contains("type") && payload.at("type").is_string()) {
        type = static_cast<uint32_t>(atoi(payload["type"].get<std::string>().c_str()));
    }
    state = static_cast<int32_t>(value);
    CGS_LOGD("report running lock event, uid:%{public}d, pid:%{public}d, lockType:%{public}d, state:%{public}d",
        uid, pid, type, state);
#ifdef POWER_MANAGER_ENABLE
    if (type == static_cast<uint32_t>(PowerMgr::RunningLockType::RUNNINGLOCK_PROXIMITY_SCREEN_CONTROL)) {
        return;
    }
    std::shared_ptr<Application> app = supervisor_->GetAppRecord(uid);
    std::shared_ptr<ProcessRecord> procRecord = app ? app->GetProcessRecord(pid) : nullptr;
    if (!app || !procRecord) {
        return;
    }
    procRecord->runningLockState_[type] = (state == ResType::RunninglockState::RUNNINGLOCK_STATE_ENABLE);
    ResSchedUtils::GetInstance().ReportSysEvent(*(app.get()), *(procRecord.get()), resType, state);
#endif
}

void CgroupEventHandler::HandleReportHisysEvent(uint32_t resType, int64_t value, const nlohmann::json& payload)
{
    int32_t uid = 0;
    int32_t pid = 0;

    if (!supervisor_) {
        CGS_LOGE("%{public}s : supervisor nullptr.", __func__);
        return;
    }

    if (!ParseValue(uid, "uid", payload) || !ParseValue(pid, "pid", payload)) {
        return;
    }
    if (uid <= 0 || pid <= 0) {
        return;
    }
    std::shared_ptr<Application> app = supervisor_->GetAppRecord(uid);
    std::shared_ptr<ProcessRecord> procRecord = app ? app->GetProcessRecord(pid) : nullptr;
    if (!app || !procRecord) {
        return;
    }

    switch (resType) {
        case ResType::RES_TYPE_REPORT_CAMERA_STATE: {
            procRecord->cameraState_ = static_cast<int32_t>(value);
            break;
        }
        case ResType::RES_TYPE_BLUETOOTH_A2DP_CONNECT_STATE_CHANGE: {
            procRecord->bluetoothState_ = static_cast<int32_t>(value);
            break;
        }
        case ResType::RES_TYPE_WIFI_CONNECT_STATE_CHANGE: {
            procRecord->wifiState_ = static_cast<int32_t>(value);
            break;
        }
        case ResType::RES_TYPE_MMI_INPUT_STATE: {
            if (payload.contains("syncStatus") && payload.at("syncStatus").is_string()) {
                procRecord->mmiStatus_ = atoi(payload["syncStatus"].get<std::string>().c_str());
            }
            break;
        }
        case ResType::RES_TYPE_REPORT_SCREEN_CAPTURE: {
            if (value == ResType::ScreenCaptureStatus::START_SCREEN_CAPTURE) {
                procRecord->screenCaptureState_ = true;
            } else {
                procRecord->screenCaptureState_ = false;
            }
            CgroupAdjuster::GetInstance().AdjustProcessGroup(*(app.get()), *(procRecord.get()),
                AdjustSource::ADJS_REPORT_SCREEN_CAPTURE);
            break;
        }
        default: {
            break;
        }
    }
    ResSchedUtils::GetInstance().ReportSysEvent(*(app.get()), *(procRecord.get()),
        resType, static_cast<int32_t>(value));
}

void CgroupEventHandler::HandleReportAvCodecEvent(uint32_t resType, int64_t value, const nlohmann::json& payload)
{
    int32_t uid = 0;
    int32_t pid = 0;
    int32_t instanceId = -1;
    int32_t state = -1;

    if (!supervisor_) {
        CGS_LOGE("%{public}s : supervisor nullptr.", __func__);
        return;
    }

    if (!ParseValue(uid, "uid", payload) || !ParseValue(pid, "pid", payload)) {
        return;
    }
    if (uid <= 0 || pid <= 0) {
        return;
    }
    if (!ParseValue(instanceId, "instanceId", payload)) {
        return;
    }
    if (instanceId < 0) {
        return;
    }
    state = static_cast<int32_t>(value);
    CGS_LOGD("report av_codec event, uid:%{public}d, pid:%{public}d, instanceId:%{public}d, state:%{public}d",
        uid, pid, instanceId, state);
    std::shared_ptr<Application> app = supervisor_->GetAppRecord(uid);
    std::shared_ptr<ProcessRecord> procRecord = app ? app->GetProcessRecord(pid) : nullptr;
    if (!app || !procRecord) {
        return;
    }
    procRecord->avCodecState_[instanceId] = (state == ResType::AvCodecState::CODEC_START_INFO);
    ResSchedUtils::GetInstance().ReportSysEvent(*(app.get()), *(procRecord.get()), resType, state);
}

void CgroupEventHandler::HandleSceneBoardState(uint32_t resType, int64_t value, const nlohmann::json& payload)
{
    int32_t keyTid = 0;
    if (!supervisor_) {
        CGS_LOGE("%{public}s : supervisor nullptr!", __func__);
        return;
    }

    if (!ParseValue(keyTid, "tid", payload)) {
        return;
    }
    if (keyTid <= 0) {
        return;
    }
    
    supervisor_->sceneBoardPid_ = keyTid;
    CGS_LOGD("%{public}s : set sceneboard pid: %{public}d", __func__, keyTid);
}

bool CgroupEventHandler::CheckVisibilityForRenderProcess(ProcessRecord &pr, ProcessRecord &mainProc)
{
    return pr.isRenderProcess_ && pr.isActive_ && !mainProc.GetWindowInfoNonNull(pr.linkedWindowId_)->isVisible_;
}

bool CgroupEventHandler::ParsePayload(int32_t& uid, int32_t& pid, int32_t& tid,
    int64_t value, const nlohmann::json& payload)
{
    if (payload.contains("uid") && payload.at("uid").is_string()) {
        uid = atoi(payload["uid"].get<std::string>().c_str());
    }

    if (payload.contains("pid") && payload.at("pid").is_string()) {
        pid = atoi(payload["pid"].get<std::string>().c_str());
    }
    tid = static_cast<int32_t>(value);
    return true;
}

bool CgroupEventHandler::ParseValue(int32_t& value, const char* name,
    const nlohmann::json& payload)
{
    if (payload.contains(name) && payload.at(name).is_string()) {
        value = atoi(payload[name].get<std::string>().c_str());
        return true;
    }
    return false;
}

void CgroupEventHandler::Submit(std::function<void()>&& func, uint32_t delay)
{
    ffrt::task_attr taskAttr;
    taskAttr.delay(delay * TIME_US_TO_MS);
    if (!CheckQueuePtr()) {
        return;
    }
    queue_->submit(func, taskAttr);
}

void CgroupEventHandler::SubmitH(std::function<void()>&& func, uint32_t eventId, uint32_t delay)
{
    ffrt::task_attr taskAttr;
    taskAttr.delay(delay * TIME_US_TO_MS);
    if (!CheckQueuePtr()) {
        return;
    }
    ffrt::task_handle syncTask = queue_->submit_h(func, taskAttr);
    eventMap_[eventId].emplace_back(std::move(syncTask));
}

bool CgroupEventHandler::CheckQueuePtr()
{
    if (queue_ != nullptr) {
        return true;
    }
    queue_ = std::make_shared<ffrt::queue>("cgroup_event_queue",
    ffrt::queue_attr().qos(ffrt::qos_default));
    if (queue_ == nullptr) {
        CGS_LOGE("queue_ is nullptr retry create");
        return  false;
    }
    return true;
}
} // namespace ResourceSchedule
} // namespace OHOS
