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
#include "ability_info.h"
#include "app_mgr_constants.h"
#include "cgroup_adjuster.h"
#include "cgroup_sched_common.h"
#include "cgroup_sched_log.h"
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

#undef LOG_TAG
#define LOG_TAG "CgroupEventHandler"

namespace OHOS {
namespace ResourceSchedule {
namespace {
    constexpr uint32_t EVENT_ID_REG_APP_STATE_OBSERVER = 1;
    constexpr uint32_t EVENT_ID_REG_BGTASK_OBSERVER = 2;
    constexpr uint32_t DELAYED_RETRY_REGISTER_DURATION = 100;
    constexpr uint32_t MAX_RETRY_TIMES = 100;
    constexpr uint32_t MAX_SPAN_SERIAL = 99;
    const std::string MMI_SERVICE_NAME = "mmi_service";
}

using OHOS::AppExecFwk::ApplicationState;
using OHOS::AppExecFwk::AbilityState;
using OHOS::AppExecFwk::AbilityType;
using OHOS::AppExecFwk::ExtensionState;
using OHOS::AppExecFwk::ProcessType;

CgroupEventHandler::CgroupEventHandler(const std::string &queueName)
{
    cgroupEventQueue_ = std::make_shared<ffrt::queue>(queueName.c_str(),
        ffrt::queue_attr().qos(ffrt::qos_user_interactive));
    if (!cgroupEventQueue_) {
        CGS_LOGE("%{public}s : create cgroupEventQueue_ failed", __func__);
    }
}

CgroupEventHandler::~CgroupEventHandler()
{
    supervisor_ = nullptr;
    cgroupEventQueue_ = nullptr;
    delayTaskMap_.clear();
}

void CgroupEventHandler::ProcessEvent(uint32_t eventId, int64_t eventParam)
{
    CGS_LOGD("%{public}s : eventId:%{public}d param:%{public}" PRIu64,
        __func__, eventId, eventParam);
    switch (eventId) {
        case EVENT_ID_REG_APP_STATE_OBSERVER: {
            int64_t retry = eventParam;
            if (!SchedController::GetInstance().SubscribeAppState() &&
                retry < MAX_RETRY_TIMES) {
                eventId = EVENT_ID_REG_APP_STATE_OBSERVER;
                eventParam = retry + 1;
                this->PostTask(
                    [this, eventId, eventParam] {
                        this->ProcessEvent(eventId, eventParam);
                    },
                    std::to_string(eventId), DELAYED_RETRY_REGISTER_DURATION);
                if (retry + 1 == static_cast<int64_t>(MAX_RETRY_TIMES)) {
                    HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::RSS, "INIT_FAULT",
                        HiviewDFX::HiSysEvent::EventType::FAULT,
                        "COMPONENT_NAME", "MAIN",
                        "ERR_TYPE", "register failure",
                        "ERR_MSG", "Subscribe app status change observer failed.");
                }
            }
            break;
        }
        case EVENT_ID_REG_BGTASK_OBSERVER: {
            int64_t retry = eventParam;
            if (!SchedController::GetInstance().SubscribeBackgroundTask() &&
                retry < MAX_RETRY_TIMES) {
                eventId = EVENT_ID_REG_BGTASK_OBSERVER;
                eventParam = retry + 1;
                this->PostTask(
                    [this, eventId, eventParam] {
                        this->ProcessEvent(eventId, eventParam);
                    },
                    std::to_string(eventId), DELAYED_RETRY_REGISTER_DURATION);
                if (retry + 1 == static_cast<int64_t>(MAX_RETRY_TIMES)) {
                    HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::RSS, "INIT_FAULT",
                        HiviewDFX::HiSysEvent::EventType::FAULT,
                        "COMPONENT_NAME", "MAIN",
                        "ERR_TYPE", "register failure",
                        "ERR_MSG", "Subscribe background task observer failed.");
                }
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
            this->RemoveTask(std::to_string(EVENT_ID_REG_APP_STATE_OBSERVER));
            if (!SchedController::GetInstance().SubscribeAppState()) {
                uint32_t eventId = EVENT_ID_REG_APP_STATE_OBSERVER;
                int64_t eventParam = 0;
                this->PostTask(
                    [this, eventId, eventParam] {
                        this->ProcessEvent(eventId, eventParam);
                    },
                    std::to_string(eventId), DELAYED_RETRY_REGISTER_DURATION);
            }
            if (supervisor_ != nullptr) {
                supervisor_->InitSuperVisorContent();
            }
            break;
        case WINDOW_MANAGER_SERVICE_ID:
            SchedController::GetInstance().SubscribeWindowState();
            break;
        case BACKGROUND_TASK_MANAGER_SERVICE_ID:
            this->RemoveTask(std::to_string(EVENT_ID_REG_BGTASK_OBSERVER));
            if (!SchedController::GetInstance().SubscribeBackgroundTask()) {
                uint32_t eventId = EVENT_ID_REG_BGTASK_OBSERVER;
                int64_t eventParam = 0;
                this->PostTask(
                    [this, eventId, eventParam] {
                        this->ProcessEvent(eventId, eventParam);
                    },
                    std::to_string(eventId), DELAYED_RETRY_REGISTER_DURATION);
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
            this->RemoveTask(std::to_string(EVENT_ID_REG_APP_STATE_OBSERVER));
            SchedController::GetInstance().UnsubscribeAppState();
            break;
        case WINDOW_MANAGER_SERVICE_ID:
            SchedController::GetInstance().UnsubscribeWindowState();
            break;
        case BACKGROUND_TASK_MANAGER_SERVICE_ID:
            this->RemoveTask(std::to_string(EVENT_ID_REG_BGTASK_OBSERVER));
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
    if (state == (int32_t)(ApplicationState::APP_STATE_TERMINATED)) {
        return;
    }
    std::shared_ptr<Application> app = supervisor_->GetAppRecordNonNull(uid);
    app->SetName(bundleName);
    app->state_ = state;
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
    const std::string& abilityName, int32_t recordId, int32_t abilityState, int32_t abilityType)
{
    if (!supervisor_) {
        CGS_LOGE("%{public}s : supervisor nullptr!", __func__);
        return;
    }
    CGS_LOGD("%{public}s : %{public}d, %{public}d, %{public}s, %{public}s, %{public}d, %{public}d, %{public}d",
        __func__, uid, pid, bundleName.c_str(), abilityName.c_str(), recordId, abilityState, abilityType);
    if (abilityType == (int32_t)AbilityType::EXTENSION) {
        CGS_LOGD("%{public}s : this type of event is not dealt with here", __func__);
        return;
    }
    ChronoScope cs("HandleAbilityStateChanged");
    if (abilityState == (int32_t)(AbilityState::ABILITY_STATE_TERMINATED)) {
        auto app = supervisor_->GetAppRecord(uid);
        if (app) {
            auto procRecord = app->GetProcessRecord(pid);
            if (procRecord) {
                procRecord->RemoveAbilityByRecordId(recordId);
                CgroupAdjuster::GetInstance().AdjustProcessGroup(*(app.get()), *(procRecord.get()),
                    AdjustSource::ADJS_ABILITY_STATE);
            }
        }
        return;
    }
    auto app = supervisor_->GetAppRecordNonNull(uid);
    app->SetName(bundleName);
    auto procRecord = app->GetProcessRecordNonNull(pid);
    auto abiInfo = procRecord->GetAbilityInfoNonNull(recordId);
    abiInfo->name_ = abilityName;
    abiInfo->state_ = abilityState;
    abiInfo->type_ = abilityType;
    CgroupAdjuster::GetInstance().AdjustProcessGroup(*(app.get()), *(procRecord.get()),
        AdjustSource::ADJS_ABILITY_STATE);
}

void CgroupEventHandler::HandleExtensionStateChanged(uid_t uid, pid_t pid, const std::string& bundleName,
    const std::string& abilityName, int32_t recordId, int32_t extensionState, int32_t abilityType)
{
    if (!supervisor_) {
        CGS_LOGE("%{public}s : supervisor nullptr!", __func__);
        return;
    }
    CGS_LOGD("%{public}s : %{public}d, %{public}d, %{public}s, %{public}s, %{public}d, %{public}d, %{public}d",
        __func__, uid, pid, bundleName.c_str(), abilityName.c_str(), recordId, extensionState, abilityType);
    ChronoScope cs("HandleExtensionStateChanged");
    if (extensionState == (int32_t)(ExtensionState::EXTENSION_STATE_TERMINATED)) {
        auto app = supervisor_->GetAppRecord(uid);
        if (app) {
            auto procRecord = app->GetProcessRecord(pid);
            if (procRecord) {
                procRecord->RemoveAbilityByRecordId(recordId);
                CgroupAdjuster::GetInstance().AdjustProcessGroup(*(app.get()), *(procRecord.get()),
                    AdjustSource::ADJS_EXTENSION_STATE);
            }
        }
        return;
    }
    auto app = supervisor_->GetAppRecordNonNull(uid);
    app->SetName(bundleName);
    auto procRecord = app->GetProcessRecordNonNull(pid);
    auto abiInfo = procRecord->GetAbilityInfoNonNull(recordId);
    abiInfo->name_ = abilityName;
    abiInfo->estate_ = extensionState;
    abiInfo->type_ = abilityType;
    CgroupAdjuster::GetInstance().AdjustProcessGroup(*(app.get()), *(procRecord.get()),
        AdjustSource::ADJS_EXTENSION_STATE);
}

void CgroupEventHandler::HandleProcessCreated(const ProcessData &processData)
{
    if (!supervisor_) {
        CGS_LOGE("%{public}s : supervisor nullptr!", __func__);
        return;
    }
    auto processType = static_cast<int32_t>(processData.processType);
    auto extensionType = static_cast<int32_t>(processData.extensionType);
    CGS_LOGI("%{public}s : %{public}d, %{public}d, %{public}d, %{public}d, %{public}s, %{public}d",
        __func__, processData.uid, processData.pid, processData.hostPid, processType, processData.bundleName.c_str(),
        extensionType);
    ChronoScope cs("HandleProcessCreated");
    std::shared_ptr<Application> app = supervisor_->GetAppRecordNonNull(processData.uid);
    std::shared_ptr<ProcessRecord> procRecord = app->GetProcessRecordNonNull(processData.pid);
    app->SetName(processData.bundleName);
    switch (processType) {
        case static_cast<int32_t>(ProcessType::RENDER):
            procRecord->processType_ = ProcRecordType::RENDER;
            procRecord->hostPid_ = processData.hostPid;
            app->AddHostProcess(processData.hostPid);
            break;
        case static_cast<int32_t>(ProcessType::EXTENSION):
            procRecord->processType_ = ProcRecordType::EXTENSION;
            procRecord->extensionType_ = extensionType;
            break;
        case static_cast<int32_t>(ProcessType::GPU):
            procRecord->processType_ = ProcRecordType::GPU;
            procRecord->hostPid_ = processData.hostPid;
            app->AddHostProcess(processData.hostPid);
            break;
        case static_cast<int32_t>(ProcessType::CHILD):
            procRecord->processType_ = ProcRecordType::CHILD;
            procRecord->hostPid_ = processData.hostPid;
            app->AddHostProcess(processData.hostPid);
            break;
        default:
            break;
    }
    AdjustSource policy = processData.isPreloadModule ? AdjustSource::ADJS_APP_PRELOAD :
        AdjustSource::ADJS_PROCESS_CREATE;

    CgroupAdjuster::GetInstance().AdjustProcessGroup(*(app.get()), *(procRecord.get()), policy);
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

void CgroupEventHandler::HandleContinuousTaskUpdate(uid_t uid, pid_t pid,
    const std::vector<uint32_t>& typeIds, int32_t abilityId)
{
    if (!supervisor_) {
        CGS_LOGE("%{public}s : supervisor nullptr!", __func__);
        return;
    }
    ChronoScope cs("HandleContinuousTaskUpdate");
    auto app = supervisor_->GetAppRecordNonNull(uid);
    auto procRecord = app->GetProcessRecordNonNull(pid);
    procRecord->continuousTaskFlag_ = 0;
    procRecord->abilityIdAndContinuousTaskFlagMap_[abilityId] = typeIds;
    for (const auto& typeId : typeIds) {
        CGS_LOGI("%{public}s : %{public}d, %{public}d, %{public}d, abilityId %{public}d,",
            __func__, uid, pid, typeId, abilityId);
    }
    for (const auto& ablityIdAndcontinuousTaskFlag : procRecord->abilityIdAndContinuousTaskFlagMap_) {
        for (const auto& typeId : ablityIdAndcontinuousTaskFlag.second) {
            procRecord->continuousTaskFlag_ |= (1U << typeId);
        }
    }
    CgroupAdjuster::GetInstance().AdjustProcessGroup(*(app.get()), *(procRecord.get()),
        AdjustSource::ADJS_CONTINUOUS_BEGIN);
}

void CgroupEventHandler::HandleContinuousTaskCancel(uid_t uid, pid_t pid, int32_t typeId,
    int32_t abilityId)
{
    if (!supervisor_) {
        CGS_LOGE("%{public}s : supervisor nullptr!", __func__);
        return;
    }
    CGS_LOGI("%{public}s : %{public}d, %{public}d, %{public}d, %{public}d",
        __func__, uid, pid, typeId, abilityId);
    ChronoScope cs("HandleContinuousTaskCancel");
    auto app = supervisor_->GetAppRecordNonNull(uid);
    auto procRecord = app->GetProcessRecord(pid);
    if (!procRecord) {
        return;
    }
    procRecord->abilityIdAndContinuousTaskFlagMap_.erase(abilityId);
    procRecord->continuousTaskFlag_ = 0;
    for (const auto& ablityIdAndcontinuousTaskFlag : procRecord->abilityIdAndContinuousTaskFlagMap_) {
        for (const auto& typeId : ablityIdAndcontinuousTaskFlag.second) {
            procRecord->continuousTaskFlag_ |= (1U << typeId);
        }
    }
    CgroupAdjuster::GetInstance().AdjustProcessGroup(*(app.get()), *(procRecord.get()),
        AdjustSource::ADJS_CONTINUOUS_END);
}

void CgroupEventHandler::HandleFocusedWindow(uint32_t windowId, WindowType windowType,
    uint64_t displayId, int32_t pid, int32_t uid)
{
    if (!supervisor_) {
        CGS_LOGE("%{public}s : supervisor nullptr!", __func__);
        return;
    }
    CGS_LOGD("%{public}s : %{public}d, %{public}d, %{public}" PRIu64 ", %{public}d, %{public}d",
        __func__, windowId, windowType, displayId, pid, uid);
    std::shared_ptr<Application> app = nullptr;
    std::shared_ptr<ProcessRecord> procRecord = nullptr;
    {
        ChronoScope cs("HandleFocusedWindow");
        app = supervisor_->GetAppRecordNonNull(uid);
        procRecord = app->GetProcessRecordNonNull(pid);
        auto win = procRecord->GetWindowInfoNonNull(windowId);
        procRecord->linkedWindowId_ = (int32_t)(windowId);
        win->windowType_ = (int32_t)(windowType);
        win->isFocused_ = true;
        win->displayId_ = displayId;

        app->focusedProcess_ = procRecord;
        auto lastFocusApp = supervisor_->focusedApp_;
        supervisor_->focusedApp_ = app;
        CgroupAdjuster::GetInstance().AdjustAllProcessGroup(*(app.get()), AdjustSource::ADJS_FOCUSED_WINDOW);
        ResSchedUtils::GetInstance().ReportSysEvent(*(app.get()), *(procRecord.get()), ResType::RES_TYPE_WINDOW_FOCUS,
            ResType::WindowFocusStatus::WINDOW_FOCUS);
    }
    if (app->GetName().empty()) {
        app->SetName(SchedController::GetInstance().GetBundleNameByUid(uid));
    }
}

void CgroupEventHandler::HandleUnfocusedWindow(uint32_t windowId, WindowType windowType,
    uint64_t displayId, int32_t pid, int32_t uid)
{
    if (!supervisor_) {
        CGS_LOGE("%{public}s : supervisor nullptr!", __func__);
        return;
    }
    CGS_LOGD("%{public}s : %{public}d, %{public}d, %{public}" PRIu64 ", %{public}d, %{public}d",
        __func__, windowId, windowType, displayId, pid, uid);
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
        win->windowType_ = (int32_t)(windowType);
        win->isFocused_ = false;
        win->displayId_ = displayId;

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
    bool visibleStatusNotChanged = windowInfo->isVisible_ == isVisible;
    windowInfo->visibilityState_ = visibilityState;
    windowInfo->isVisible_ = isVisible;
    windowInfo->windowType_ = (int32_t)windowType;

    if (visibleStatusNotChanged) {
        return;
    }
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
    if (!windowInfo) {
        CGS_LOGE("%{public}s : windowInfo nullptr!", __func__);
        return;
    }
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

    std::shared_ptr<Application> app = nullptr;
    std::shared_ptr<ProcessRecord> procRecord = nullptr;
    if (!GetProcInfoByPayload(uid, pid, app, procRecord, payload)) {
        return;
    }

    if (!ParseValue(keyTid, "tid", payload) || !ParseValue(role, "role", payload)) {
        return;
    }

    if (!ResSchedUtils::GetInstance().CheckTidIsInPid(pid, keyTid)) {
        return;
    }

    if (value == ResType::ReportChangeStatus::CREATE) {
        procRecord->keyThreadRoleMap_.emplace(keyTid, role);
        procRecord->isReload_ = false;
    } else {
        procRecord->keyThreadRoleMap_.erase(keyTid);
    }

    // if role of thread is important display, adjust it
    auto hostProcRecord = app->GetProcessRecord(procRecord->hostPid_);
    if (!hostProcRecord) {
        return;
    }
    CGS_LOGI("%{public}s : appName: %{public}s, uid: %{public}d, pid: %{public}d, keyTid: %{public}d",
        __func__, app->GetName().c_str(), uid, pid, keyTid);
    CgroupAdjuster::GetInstance().AdjustProcessGroup(*(app.get()), *(hostProcRecord.get()),
        AdjustSource::ADJS_REPORT_IMPORTANT_DISPLAY_THREAD);
}

void CgroupEventHandler::HandleReportWindowState(uint32_t resType, int64_t value, const nlohmann::json& payload)
{
    int32_t uid = 0;
    int32_t pid = 0;
    int32_t windowId = -1;
    int32_t state = 0;
    int32_t nowSerialNum = -1;

    std::shared_ptr<Application> app = nullptr;
    std::shared_ptr<ProcessRecord> procRecord = nullptr;
    if (!GetProcInfoByPayload(uid, pid, app, procRecord, payload)) {
        return;
    }

    if (!ParseValue(windowId, "windowId", payload) || !ParseValue(state, "state", payload) ||
        !ParseValue(nowSerialNum, "serialNum", payload)) {
        CGS_LOGW("%{public}s : param is not valid or not exist", __func__);
        return;
    }
    CGS_LOGI("%{public}s : render process name: %{public}s, uid: %{public}d, pid: %{public}d, state: %{public}d",
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
    auto hostProcRecord = app->GetProcessRecord(procRecord->hostPid_);
    if (!hostProcRecord) {
        return;
    }
    CGS_LOGI("%{public}s : pid: %{public}d, winId: %{public}d, isActive_: %{public}d",
        __func__, pid, procRecord->linkedWindowId_, procRecord->isActive_);
    UpdateActivepWebRenderInfo(uid, pid, windowId, state, hostProcRecord);
    if (CheckVisibilityForRenderProcess(*(procRecord.get()), *hostProcRecord)) {
        CGS_LOGW("%{public}s : bundle name: %{public}s, uid: %{public}d, pid: %{public}d, winId: %{public}d" \
            "is not visible but active", __func__, app->GetName().c_str(), uid, pid, procRecord->linkedWindowId_);
    }
    CgroupAdjuster::GetInstance().AdjustProcessGroup(*(app.get()), *(hostProcRecord.get()),
        AdjustSource::ADJS_REPORT_WINDOW_STATE_CHANGED);
    ResSchedUtils::GetInstance().ReportSysEvent(*(app.get()), *(procRecord.get()),
        ResType::RES_TYPE_REPORT_WINDOW_STATE, state);
}

void CgroupEventHandler::UpdateActivepWebRenderInfo(int32_t uid, int32_t pid, int32_t windowId, int32_t state,
    const std::shared_ptr<ProcessRecord>& proc)
{
    if (state != ResType::WindowStates::ACTIVE) {
        return;
    }
    auto win = proc->GetWindowInfoNonNull(windowId);
    win->topWebviewRenderUid_ = (uint32_t)(uid);
}

void CgroupEventHandler::HandleReportAudioState(uint32_t resType, int64_t value, const nlohmann::json& payload)
{
    int32_t uid = 0;
    int32_t pid = 0;
    int32_t sessionId = -1;

    if (!supervisor_) {
        CGS_LOGE("%{public}s : supervisor nullptr!", __func__);
        return;
    }

    if (!ParseValue(uid, "uid", payload) || !ParseValue(pid, "pid", payload) ||
        !ParseValue(sessionId, "sessionId", payload)) {
        CGS_LOGE("%{public}s : payload does not contain uid or pid", __func__);
        return;
    }
    if (uid <= 0 || pid <= 0) {
        CGS_LOGE("%{public}s : uid or pid is less than 0", __func__);
        return;
    }

    std::shared_ptr<Application> app = supervisor_->GetAppRecord(uid);
    std::shared_ptr<ProcessRecord> procRecord = app ? app->GetProcessRecord(pid) : nullptr;
    if (!app || !procRecord) {
        return;
    }
    if (value == ResType::AudioStatus::RENDERER_RELEASED) {
        procRecord->audioPlayingState_.erase(sessionId);
    } else {
        auto it = procRecord->audioPlayingState_.find(sessionId);
        if (it != procRecord->audioPlayingState_.end() && it->second == static_cast<int32_t>(value)) {
            return;
        }
        procRecord->audioPlayingState_[sessionId] = static_cast<int32_t>(value);
    }

    long revertValue = static_cast<long>(value);
    CGS_LOGI("%{public}s :Appname:%{public}s, uid:%{public}d, pid:%{public}d, sessionId:%{public}d, "\
        "state:%{public}ld", __func__, app->GetName().c_str(), uid, pid, sessionId, revertValue);

    CgroupAdjuster::GetInstance().AdjustProcessGroup(*(app.get()), *(procRecord.get()),
        AdjustSource::ADJS_REPORT_AUDIO_STATE_CHANGED);
    ResSchedUtils::GetInstance().ReportSysEvent(*(app.get()), *(procRecord.get()),
        resType, static_cast<int32_t>(value));
}

void CgroupEventHandler::HandleReportWebviewAudioState(uint32_t resType, int64_t value, const nlohmann::json& payload)
{
    int32_t uid = 0;
    int32_t pid = 0;
    int32_t sessionId = 0;

    if (!supervisor_) {
        CGS_LOGE("%{public}s : supervisor nullptr!", __func__);
        return;
    }

    if (!ParseValue(uid, "uid", payload) || !ParseValue(pid, "pid", payload)) {
        return;
    }
    if (uid <= 0 || pid <= 0) {
        CGS_LOGW("%{public}s : uid or pid invalid, uid: %{public}d, pid: %{public}d!",
            __func__, uid, pid);
        return;
    }

    std::shared_ptr<Application> app = supervisor_->GetAppRecordNonNull(uid);
    std::shared_ptr<ProcessRecord> procRecord = app ? app->GetProcessRecord(pid) : nullptr;
    if (!app || !procRecord) {
        CGS_LOGW("%{public}s : proc record is not exist, uid: %{public}d, pid: %{public}d",
            __func__, uid, pid);
        return;
    }

    procRecord->audioPlayingState_[sessionId] = static_cast<int32_t>(value);
    CGS_LOGI("%{public}s : audio process name: %{public}s, uid: %{public}d, pid: %{public}d, state: %{public}d",
        __func__, app->GetName().c_str(), uid, pid, procRecord->audioPlayingState_[sessionId]);

    CgroupAdjuster::GetInstance().AdjustProcessGroup(*(app.get()), *(procRecord.get()),
        AdjustSource::ADJS_REPORT_WEBVIEW_AUDIO_STATE_CHANGED);
    ResSchedUtils::GetInstance().ReportSysEvent(*(app.get()), *(procRecord.get()),
        resType, static_cast<int32_t>(value));
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
    CGS_LOGI("report running lock event, uid:%{public}d, pid:%{public}d, lockType:%{public}d, state:%{public}d",
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

void CgroupEventHandler::HandleReportBluetoothConnectState(
    uint32_t resType, int64_t value, const nlohmann::json& payload)
{
    int32_t uid = 0;
    int32_t pid = 0;

    if (!supervisor_) {
        CGS_LOGE("%{public}s : supervisor nullptr.", __func__);
        return;
    }

    if (!ParseValue(uid, "uid", payload) || !ParseValue(pid, "pid", payload)) {
        CGS_LOGE("%{public}s : payload does not contain uid or pid", __func__);
        return;
    }
    if (uid <= 0 || pid <= 0) {
        CGS_LOGE("%{public}s : uid or pid is less than 0", __func__);
        return;
    }
    CGS_LOGD("report bluetooth connect state, uid:%{public}d, pid:%{public}d, value:%{public}lld",
        uid, pid, (long long)value);
    std::shared_ptr<Application> app = supervisor_->GetAppRecord(uid);
    std::shared_ptr<ProcessRecord> procRecord = app ? app->GetProcessRecord(pid) : nullptr;
    if (!app || !procRecord) {
        return;
    }
    procRecord->bluetoothState_ = static_cast<int32_t>(value);
    ResSchedUtils::GetInstance().ReportSysEvent(*(app.get()), *(procRecord.get()),
        resType, static_cast<int32_t>(value));
}

void CgroupEventHandler::HandleMmiInputState(uint32_t resType, int64_t value, const nlohmann::json& payload)
{
    int32_t uid = 0;
    int32_t pid = 0;

    if (!supervisor_) {
        CGS_LOGE("%{public}s : supervisor nullptr.", __func__);
        return;
    }

    if (!ParseValue(uid, "uid", payload) || !ParseValue(pid, "pid", payload)) {
        CGS_LOGE("%{public}s : payload does not contain uid or pid", __func__);
        return;
    }
    if (uid <= 0 || pid <= 0) {
        CGS_LOGE("%{public}s : uid or pid is less than 0", __func__);
        return;
    }
    CGS_LOGD("report mmi input state, uid:%{public}d, pid:%{public}d, value:%{public}lld",
        uid, pid, (long long)value);
    std::shared_ptr<Application> app = supervisor_->GetAppRecord(uid);
    std::shared_ptr<ProcessRecord> procRecord = app ? app->GetProcessRecord(pid) : nullptr;
    if (!app || !procRecord) {
        return;
    }

    if (payload.contains("syncStatus") && payload.at("syncStatus").is_string()) {
        procRecord->mmiStatus_ = atoi(payload["syncStatus"].get<std::string>().c_str());
    }
    ResSchedUtils::GetInstance().ReportSysEvent(*(app.get()), *(procRecord.get()),
        resType, static_cast<int32_t>(value));
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
        case ResType::RES_TYPE_WIFI_CONNECT_STATE_CHANGE: {
            procRecord->wifiState_ = static_cast<int32_t>(value);
            break;
        }
        default: {
            break;
        }
    }
    ResSchedUtils::GetInstance().ReportSysEvent(*(app.get()), *(procRecord.get()),
        resType, static_cast<int32_t>(value));
}

void CgroupEventHandler::HandleReportScreenCaptureEvent(uint32_t resType, int64_t value, const nlohmann::json& payload)
{
    int32_t uid = 0;
    int32_t pid = 0;

    if (!supervisor_) {
        CGS_LOGE("%{public}s : supervisor nullptr.", __func__);
        return;
    }

    if (!ParseValue(uid, "uid", payload) || !ParseValue(pid, "pid", payload)) {
        CGS_LOGE("%{public}s : payload does not contain uid or pid", __func__);
        return;
    }
    if (uid <= 0 || pid <= 0) {
        CGS_LOGE("%{public}s : uid or pid is less than 0", __func__);
        return;
    }
    CGS_LOGD("report Screen capture, uid:%{public}d, pid:%{public}d, value:%{public}lld",
        uid, pid, (long long)value);
    std::shared_ptr<Application> app = supervisor_->GetAppRecord(uid);
    std::shared_ptr<ProcessRecord> procRecord = app ? app->GetProcessRecord(pid) : nullptr;
    if (!app || !procRecord) {
        return;
    }

    procRecord->screenCaptureState_ = (value == ResType::ScreenCaptureStatus::START_SCREEN_CAPTURE);
    CgroupAdjuster::GetInstance().AdjustProcessGroup(*(app.get()), *(procRecord.get()),
        AdjustSource::ADJS_REPORT_SCREEN_CAPTURE);

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
    CGS_LOGI("report av_codec event, uid:%{public}d, pid:%{public}d, instanceId:%{public}d, state:%{public}d",
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
    int32_t sceneBoardPid = 0;
    if (!supervisor_) {
        CGS_LOGE("%{public}s : supervisor nullptr!", __func__);
        return;
    }

    if (!ParseValue(sceneBoardPid, "pid", payload)) {
        return;
    }
    if (sceneBoardPid <= 0) {
        return;
    }

    supervisor_->sceneBoardPid_ = sceneBoardPid;
    CGS_LOGI("%{public}s : set sceneboard pid: %{public}d", __func__, sceneBoardPid);
}

bool CgroupEventHandler::CheckVisibilityForRenderProcess(ProcessRecord &pr, ProcessRecord &mainProc)
{
    return (pr.processType_ == ProcRecordType::RENDER) && pr.isActive_ &&
        !mainProc.GetWindowInfoNonNull(pr.linkedWindowId_)->isVisible_;
}

void CgroupEventHandler::HandleWebviewScreenCapture(uint32_t resType, int64_t value, const nlohmann::json& payload)
{
    int32_t uid = 0;
    int32_t pid = 0;
    std::shared_ptr<Application> app = nullptr;
    std::shared_ptr<ProcessRecord> procRecord = nullptr;

    if (!GetProcInfoByPayload(uid, pid, app, procRecord, payload)) {
        return;
    }

    procRecord->screenCaptureState_= (value == ResType::WebScreenCapture::WEB_SCREEN_CAPTURE_START);
    CGS_LOGI("%{public}s : screen capture process: %{public}s, uid: %{public}d, pid: %{public}d, state: %{public}d",
        __func__, app->GetName().c_str(), uid, pid, procRecord->screenCaptureState_);

    CgroupAdjuster::GetInstance().AdjustProcessGroup(*(app.get()), *(procRecord.get()),
        AdjustSource::ADJS_REPORT_WEBVIEW_SCREEN_CAPTURE);
    ResSchedUtils::GetInstance().ReportSysEvent(*(app.get()), *(procRecord.get()), resType,
        procRecord->screenCaptureState_);
}

void CgroupEventHandler::ReportAbilityStatus(uint32_t resType, int64_t value, const nlohmann::json& payload)
{
    int32_t saId = -1;
    if (payload.contains("saId") && payload.at("saId").is_number_integer()) {
        saId = payload["saId"].get<int32_t>();
    }
    std::string deviceId = "";
    if (payload.contains("deviceId") && payload.at("deviceId").is_string()) {
        deviceId = payload["deviceId"].get<std::string>();
    }
    CGS_LOGD("%{public}s saId: %{public}d, status: %{public}lld", __func__, saId, (long long)value);
    PostTask([saId, deviceId, value, this] {
        if (value > 0) {
            HandleAbilityAdded(saId, deviceId);
        } else {
            HandleAbilityRemoved(saId, deviceId);
        }
    });
}

void CgroupEventHandler::UpdateMmiStatus(uint32_t resType, int64_t value, const nlohmann::json& payload)
{
    if (supervisor_ == nullptr) {
        return;
    }
    if (!payload.contains("pid") || !payload.at("pid").is_number_integer()) {
        return;
    }
    if (!payload.contains("uid") || !payload.at("uid").is_number_integer()) {
        return;
    }
    if (!payload.contains("status") || !payload.at("status").is_number_integer()) {
        return;
    }
    int32_t pid = payload["pid"].get<int32_t>();
    int32_t uid = payload["uid"].get<int32_t>();
    int32_t status = payload["status"].get<int32_t>();
    auto app = supervisor_->GetAppRecord(uid);
    auto procRecord = app ? app->GetProcessRecord(pid) : nullptr;
    if (procRecord) {
        procRecord->mmiStatus_ = status;
    }
}

void CgroupEventHandler::HandleEmptyPayloadForCosmicCubeState(uint32_t resType, int64_t value)
{
    bool isNeedRecover = resType == ResType::RES_TYPE_COSMIC_CUBE_STATE_CHANGE &&
        value == ResType::CosmicCubeState::APPLICATION_ABOUT_TO_RECOVER;
    if (!isNeedRecover) {
        return;
    }
    std::map <int32_t, std::shared_ptr<Application>> uidMap = supervisor_->GetUidsMap();
    for (auto it = uidMap.begin(); it != uidMap.end(); it++) {
        int32_t uid = it->first;
        std::shared_ptr <Application> app = it->second;
        if (!app->isCosmicCubeStateHide_) {
            continue;
        }
        app->isCosmicCubeStateHide_ = false;
        std::map <pid_t, std::shared_ptr<ProcessRecord>> pidMap = app->GetPidsMap();
        for (auto pidIt = pidMap.begin(); pidIt != pidMap.end(); pidIt++) {
            int32_t pid = pidIt->first;
            std::shared_ptr <ProcessRecord> procRecord = pidIt->second;
            if (procRecord->processType_ == ProcRecordType::NORMAL) {
                CGS_LOGI("%{public}s, uid:%{public}d pid:%{public}d recover", __func__, uid, pid);
                CgroupAdjuster::GetInstance().AdjustProcessGroup(*(app.get()), *(procRecord.get()),
                    AdjustSource::ADJS_PROCESS_STATE);
            }
        }
    }
}

void CgroupEventHandler::HandleReportCosmicCubeState(uint32_t resType, int64_t value, const nlohmann::json &payload)
{
    if (supervisor_ == nullptr) {
        return;
    }
    int32_t uid = 0;
    int32_t pid = 0;
    if (!ParsePayload(uid, pid, payload)) {
        CGS_LOGW("%{public}s : uid or pid invalid, uid:%{public}d, pid:%{public}d, value:%{public}lld",
            __func__, uid, pid, (long long)value);
        HandleEmptyPayloadForCosmicCubeState(resType, value);
        return;
    }
    std::shared_ptr <Application> app = supervisor_->GetAppRecord(uid);
    std::shared_ptr <ProcessRecord> procRecord = app ? app->GetProcessRecord(pid) : nullptr;
    if (!app || !procRecord) {
        CGS_LOGW("%{public}s : app or proc record is not exist, uid:%{public}d, pid:%{public}d!", __func__, uid, pid);
        return;
    }
    app->isCosmicCubeStateHide_ = (value == ResType::CosmicCubeState::APPLICATION_ABOUT_TO_HIDE);
    if (procRecord->processType_ == ProcRecordType::NORMAL) {
        CGS_LOGI("%{public}s uid:%{public}d, pid:%{public}d, value:%{public}lld", __func__, uid, pid, (long long)value);
        CgroupAdjuster::GetInstance().AdjustProcessGroup(*(app.get()), *(procRecord.get()),
            AdjustSource::ADJS_PROCESS_STATE);
    }
}

void CgroupEventHandler::HandleReportWebviewVideoState(uint32_t resType, int64_t value, const nlohmann::json& payload)
{
    int32_t uid = 0;
    int32_t pid = 0;
    std::shared_ptr<Application> app = nullptr;
    std::shared_ptr<ProcessRecord> procRecord = nullptr;

    if (!GetProcInfoByPayload(uid, pid, app, procRecord, payload)) {
        return;
    }

    procRecord->videoState_ = (value == ResType::WebVideoState::WEB_VIDEO_PLAYING_START);
    CGS_LOGI("%{public}s : video process name: %{public}s, uid: %{public}d, pid: %{public}d, state: %{public}d",
        __func__, app->GetName().c_str(), uid, pid, procRecord->videoState_);

    ResSchedUtils::GetInstance().ReportSysEvent(*(app.get()), *(procRecord.get()), resType,
        procRecord->videoState_);
}

void CgroupEventHandler::HandleOnAppStopped(uint32_t resType, int64_t value, const nlohmann::json& payload)
{
    if (!payload.contains("uid") || !payload.at("uid").is_number_integer()) {
        CGS_LOGE("%{public}s : uid invalid!", __func__);
        return;
    }
    int32_t uid = payload["uid"].get<int32_t>();
    if (!payload.contains("bundleName") || !payload.at("bundleName").is_string()) {
        CGS_LOGE("%{public}s : bundleName invalid!", __func__);
        return;
    }
    std::string bundleName = payload["bundleName"].get<std::string>();

    if (!supervisor_) {
        CGS_LOGE("%{public}s : supervisor nullptr!", __func__);
        return;
    }
    CGS_LOGI("%{public}s : %{public}d, %{public}s", __func__, uid, bundleName.c_str());
    supervisor_->RemoveApplication(uid);
}

bool CgroupEventHandler::GetProcInfoByPayload(int32_t &uid, int32_t &pid, std::shared_ptr<Application>& app,
    std::shared_ptr<ProcessRecord>& procRecord, const nlohmann::json& payload)
{
    if (!supervisor_) {
        CGS_LOGE("%{public}s : supervisor nullptr!", __func__);
        return false;
    }

    if (!ParsePayload(uid, pid, payload)) {
        CGS_LOGW("%{public}s : uid or pid invalid, uid: %{public}d, pid: %{public}d!",
            __func__, uid, pid);
        return false;
    }
    app = supervisor_->GetAppRecord(uid);
    if (app) {
        procRecord = app->GetProcessRecordNonNull(pid);
    }
    if (!app || !procRecord) {
        CGS_LOGW("%{public}s : app record or proc record is not exist, uid: %{public}d, pid: %{public}d!",
            __func__, uid, pid);
        return false;
    }
    return true;
}

bool CgroupEventHandler::ParsePayload(int32_t& uid, int32_t& pid, const nlohmann::json& payload)
{
    if (!ParseValue(uid, "uid", payload) || !ParseValue(pid, "pid", payload)) {
        return false;
    }
    if (uid <= 0 || pid <= 0) {
        return false;
    }
    return true;
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

void CgroupEventHandler::PostTask(const std::function<void()> task)
{
    if (!cgroupEventQueue_) {
        CGS_LOGE("%{public}s : cgroupEventQueue_ nullptr", __func__);
        return;
    }
    cgroupEventQueue_->submit([task, this] {
        task();
    });
}

void CgroupEventHandler::PostTask(const std::function<void()> task, const std::string &taskName,
    const int32_t delayTime)
{
    std::lock_guard<ffrt::mutex> autoLock(delayTaskMapMutex_);
    if (!cgroupEventQueue_) {
        CGS_LOGE("%{public}s : cgroupEventQueue_ nullptr", __func__);
        return;
    }
    delayTaskMap_[taskName] = cgroupEventQueue_->submit_h([task, this] {
        task();
    }, ffrt::task_attr().delay(delayTime * ffrtSwitch_));
}

void CgroupEventHandler::PostTaskAndWait(const std::function<void()> task)
{
    if (!cgroupEventQueue_) {
        CGS_LOGE("%{public}s : cgroupEventQueue_ nullptr", __func__);
        return;
    }
    ffrt::task_handle handle = cgroupEventQueue_->submit_h(task);
    cgroupEventQueue_->wait(handle);
}

void CgroupEventHandler::RemoveTask(const std::string &taskName)
{
    std::lock_guard<ffrt::mutex> autoLock(delayTaskMapMutex_);
    for (auto iter = delayTaskMap_.begin(); iter != delayTaskMap_.end(); iter++) {
        if (iter->first == taskName && iter->second != nullptr) {
            cgroupEventQueue_->cancel(iter->second);
            delayTaskMap_.erase(iter);
            return;
        }
    }
}
} // namespace ResourceSchedule
} // namespace OHOS
