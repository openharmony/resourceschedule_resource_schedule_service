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

#include "sched_controller.h"

#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"
#include "app_mgr_interface.h"
#include "app_state_observer.h"
#include "background_task_mgr_helper.h"
#include "cgroup_adjuster.h"
#include "cgroup_event_handler.h"
#include "continuous_task_observer.h"
#include "cgroup_sched_common.h"
#include "cgroup_sched_log.h"
#include "ressched_utils.h"
#include "supervisor.h"
#include "transient_task_observer.h"
#include "window_state_observer.h"

namespace OHOS {
namespace ResourceSchedule {
namespace {
    const std::string CG_HANDLER_THREAD = "CgroupEventHandler";
    constexpr HiviewDFX::HiLogLabel LOG_LABEL = {LOG_CORE, LOG_TAG_DOMAIN_ID_RMS, "SchedController"};
}

using OHOS::ResourceSchedule::CgroupSetting::SchedPolicy;

OHOS::sptr<OHOS::AppExecFwk::IAppMgr> GetAppManagerInstance()
{
    OHOS::sptr<OHOS::ISystemAbilityManager> systemAbilityManager =
        OHOS::SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    OHOS::sptr<OHOS::IRemoteObject> object = systemAbilityManager->GetSystemAbility(OHOS::APP_MGR_SERVICE_ID);
    return OHOS::iface_cast<OHOS::AppExecFwk::IAppMgr>(object);
}

SchedController& SchedController::GetInstance()
{
    static auto instance = new SchedController();
    return *instance;
}

void SchedController::Init()
{
    ChronoScope cs("Init SchedController.");
    // Init supervisor which contains cached data for ccgroup controller.
    InitSupervisor();
    // Init cgroup handler thread
    InitCgroupHandler();
    // Init cgroup adjuster thread
    InitCgroupAdjuster();

    auto event = AppExecFwk::InnerEvent::Get(INNER_EVENT_ID_REG_STATE_OBSERVERS);
    cgHandler_->SendEvent(event, DELAYED_REGISTER_DURATION);
}

void SchedController::Deinit()
{
    if (cgHandler_ != nullptr) {
        cgHandler_->RemoveAllEvents();
        cgHandler_ = nullptr;
    }
    if (supervisor_ != nullptr) {
        supervisor_ = nullptr;
    }
}

void SchedController::RegisterStateObservers()
{
    // register callback observers for app state
    SubscribeAppState();
    // register callback observers for background task
    SubscribeTransientTask();
    SubscribeContinuousTask();
    // register callback observers for window state
    SubscribeWindowState();
}

void SchedController::UnregisterStateObservers()
{
    sptr<OHOS::AppExecFwk::IAppMgr> appManager = GetAppManagerInstance();
    if (appManager != nullptr && appStateObserver_ != nullptr) {
        int32_t err = appManager->UnregisterApplicationStateObserver(appStateObserver_.get());
        if (err == 0) {
            CGS_LOGI("UnregisterApplicationStateObserver success.");
        } else {
            CGS_LOGE("UnregisterApplicationStateObserver failed. err:%{public}d", err);
        }
    }
    appStateObserver_ = nullptr;

    if (transientTaskObserver_ != nullptr) {
        OHOS::BackgroundTaskMgr::BackgroundTaskMgrHelper::UnsubscribeBackgroundTask(*transientTaskObserver_);
    }
    transientTaskObserver_ = nullptr;

    if (continuousTaskObserver_ != nullptr) {
        OHOS::BackgroundTaskMgr::BackgroundTaskMgrHelper_::RequestUnsubscribe(*continuousTaskObserver_);
    }
    continuousTaskObserver_ = nullptr;

    if (windowStateObserver_ != nullptr) {
        // unregister windowStateObserver_
        OHOS::Rosen::WindowManager::GetInstance().UnregisterFocusChangedListener(windowStateObserver_);
    }
    windowStateObserver_ = nullptr;
}

void SchedController::AdjustProcessGroup(Application &app, ProcessRecord &pr, AdjustSource source)
{
    if (cgAdjuster_ == nullptr) {
        CGS_LOGE("SchedController is disabled due to null cgAdjuster_");
        return;
    }
    cgAdjuster_->AdjustProcessGroup(app, pr, source);
}

void SchedController::AdjustAllProcessGroup(Application &app, AdjustSource source)
{
    if (cgAdjuster_ == nullptr) {
        CGS_LOGE("SchedController is disabled due to null cgAdjuster_");
        return;
    }
    cgAdjuster_->AdjustAllProcessGroup(app, source);
}

int SchedController::GetProcessGroup(pid_t pid)
{
    if (supervisor_ == nullptr) {
        CGS_LOGE("SchedController::GetProcessCgroup, supervisor nullptr.");
        return VALUE_INT(SchedPolicy::SP_DEFAULT);
    }
    std::shared_ptr<ProcessRecord> pr = supervisor_->FindProcessRecord(pid);
    return pr == nullptr ? VALUE_INT(SchedPolicy::SP_DEFAULT) : VALUE_INT(pr->curSchedGroup_);
}

inline void SchedController::InitCgroupHandler()
{
    cgHandler_ = std::make_shared<CgroupEventHandler>(OHOS::AppExecFwk::EventRunner::Create(CG_HANDLER_THREAD));
    cgHandler_->SetSupervisor(supervisor_);
}

inline void SchedController::InitCgroupAdjuster()
{
    cgAdjuster_ = std::make_shared<CgroupAdjuster>();
    cgAdjuster_->InitAdjuster();
}

inline void SchedController::InitSupervisor()
{
    supervisor_ = std::make_shared<Supervisor>();
}

inline void SchedController::SubscribeAppState()
{
    sptr<OHOS::AppExecFwk::IAppMgr> appManager = GetAppManagerInstance();
    if (appManager == nullptr) {
        CGS_LOGE("%{public}s app manager nullptr!", __func__);
        return;
    }
    appStateObserver_ = std::make_shared<RmsApplicationStateObserver>();
    int32_t err = appManager->RegisterApplicationStateObserver(appStateObserver_.get());
    if (err == 0) {
        CGS_LOGI("RegisterApplicationStateObserver success.");
    } else {
        CGS_LOGE("RegisterApplicationStateObserver failed. err:%{public}d", err);
    }
}

inline void SchedController::SubscribeTransientTask()
{
    transientTaskObserver_ = std::make_shared<TransientTaskObserver>();
    bool ret = OHOS::BackgroundTaskMgr::BackgroundTaskMgrHelper::SubscribeBackgroundTask(*transientTaskObserver_);
    if (ret) {
        CGS_LOGI("Register TransientTaskObserver success.");
    } else {
        CGS_LOGE("Register TransientTaskObserver failed.");
    }
}

inline void SchedController::SubscribeContinuousTask()
{
    continuousTaskObserver_ = std::make_shared<ContinuousTaskObserver>();
    OHOS::BackgroundTaskMgr::BackgroundTaskMgrHelper_::RequestSubscribe(*continuousTaskObserver_);
}

inline void SchedController::SubscribeWindowState()
{
    windowStateObserver_ = new WindowStateObserver();
    OHOS::Rosen::WindowManager::GetInstance().RegisterFocusChangedListener(windowStateObserver_);
}

extern "C" void CgroupSchedInit()
{
    SchedController::GetInstance().Init();
}

extern "C" void CgroupSchedDeinit()
{
    SchedController::GetInstance().Deinit();
    SchedController::GetInstance().UnregisterStateObservers();
}

extern "C" int GetProcessGroup(pid_t pid)
{
    return SchedController::GetInstance().GetProcessGroup(pid);
}
} // namespace ResourceSchedule
} // namespace OHOS
