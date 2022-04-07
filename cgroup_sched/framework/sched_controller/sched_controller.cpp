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

#include "sched_controller.h"

#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"
#include "app_mgr_interface.h"
#include "app_state_observer.h"
#include "background_task_mgr_helper.h"
#include "cgroup_adjuster.h"
#include "cgroup_event_handler.h"
#include "cgroup_sched_common.h"
#include "cgroup_sched_log.h"
#include "ressched_utils.h"
#include "supervisor.h"
#include "background_task_observer.h"
#include "window_state_observer.h"

namespace OHOS {
namespace ResourceSchedule {
namespace {
    const std::string CG_HANDLER_THREAD = "CgroupEventHandler";
    constexpr HiviewDFX::HiLogLabel LOG_LABEL = {LOG_CORE, LOG_TAG_DOMAIN_ID_RMS, "SchedController"};
}

using OHOS::BackgroundTaskMgr::BackgroundTaskMgrHelper;
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
    static SchedController instance;
    return instance;
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
}

void SchedController::Deinit()
{
    if (cgHandler_) {
        cgHandler_->RemoveAllEvents();
        cgHandler_ = nullptr;
    }
    if (supervisor_) {
        supervisor_ = nullptr;
    }
    cgAdjuster_ = nullptr;
}

void SchedController::UnregisterStateObservers()
{
    UnsubscribeAppState();
    UnsubscribeBackgroundTask();
    UnsubscribeWindowState();
}

void SchedController::AdjustProcessGroup(Application &app, ProcessRecord &pr, AdjustSource source)
{
    if (!cgAdjuster_) {
        CGS_LOGE("SchedController is disabled due to null cgAdjuster_");
        return;
    }
    cgAdjuster_->AdjustProcessGroup(app, pr, source);
}

void SchedController::AdjustAllProcessGroup(Application &app, AdjustSource source)
{
    if (!cgAdjuster_) {
        CGS_LOGE("SchedController is disabled due to null cgAdjuster_");
        return;
    }
    cgAdjuster_->AdjustAllProcessGroup(app, source);
}

int SchedController::GetProcessGroup(pid_t pid)
{
    if (!supervisor_) {
        CGS_LOGE("SchedController::GetProcessCgroup, supervisor nullptr.");
        return VALUE_INT(SchedPolicy::SP_DEFAULT);
    }
    std::shared_ptr<ProcessRecord> pr = supervisor_->FindProcessRecord(pid);
    return pr ? VALUE_INT(pr->curSchedGroup_) : VALUE_INT(SchedPolicy::SP_DEFAULT);
}

void SchedController::ReportAbilityStatus(int32_t saId, const std::string& deviceId, uint32_t status)
{
    CGS_LOGD("%{public}s sdId:%{public}d, status:%{public}d", __func__, saId, status);
    auto handler = this->cgHandler_;
    if (!handler) {
        return;
    }
    handler->PostTask([handler, saId, deviceId, status] {
        if (status > 0) {
            handler->HandleAbilityAdded(saId, deviceId);
        } else {
            handler->HandleAbilityRemoved(saId, deviceId);
        }
    });
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

bool SchedController::SubscribeAppState()
{
    if (appStateObserver_) {
        return true;
    }
    sptr<OHOS::AppExecFwk::IAppMgr> appManager = GetAppManagerInstance();
    if (!appManager) {
        CGS_LOGE("%{public}s app manager nullptr!", __func__);
        return false;
    }
    appStateObserver_ = new (std::nothrow)RmsApplicationStateObserver();
    if (!appStateObserver_) {
        CGS_LOGE("%{public}s allocate app state observer failed!", __func__);
        return false;
    }
    int32_t err = appManager->RegisterApplicationStateObserver(appStateObserver_);
    if (err != 0) {
        CGS_LOGE("RegisterApplicationStateObserver failed. err:%{public}d", err);
        appStateObserver_ = nullptr;
        return false;
    }
    CGS_LOGI("RegisterApplicationStateObserver success.");
    return true;
}

void SchedController::UnsubscribeAppState()
{
    if (!appStateObserver_) {
        return;
    }

    sptr<OHOS::AppExecFwk::IAppMgr> appManager = GetAppManagerInstance();
    if (appManager) {
        int32_t err = appManager->UnregisterApplicationStateObserver(appStateObserver_);
        if (err == 0) {
            CGS_LOGI("UnregisterApplicationStateObserver success.");
        } else {
            CGS_LOGE("UnregisterApplicationStateObserver failed. err:%{public}d", err);
        }
    }
    appStateObserver_ = nullptr;
}

bool SchedController::SubscribeBackgroundTask()
{
    if (backgroundTaskObserver_) {
        return true;
    }
    backgroundTaskObserver_ = std::make_shared<BackgroundTaskObserver>();
    int ret = BackgroundTaskMgrHelper::SubscribeBackgroundTask(*backgroundTaskObserver_);
    if (ret != 0) {
        backgroundTaskObserver_ = nullptr;
        CGS_LOGE("Register BackgroundTaskObserver failed, err:%{public}d.", ret);
        return false;
    }
    CGS_LOGI("Register BackgroundTaskObserver success.");
    return true;
}

void SchedController::UnsubscribeBackgroundTask()
{
    if (!backgroundTaskObserver_) {
        return;
    }
    int32_t ret = BackgroundTaskMgrHelper::UnsubscribeBackgroundTask(*backgroundTaskObserver_);
    if (ret == 0) {
        CGS_LOGI("UnsubscribeBackgroundTask success.");
    } else {
        CGS_LOGE("UnsubscribeBackgroundTask failed. ret:%{public}d", ret);
    }
    backgroundTaskObserver_ = nullptr;
}

void SchedController::SubscribeWindowState()
{
    if (!windowStateObserver_) {
        windowStateObserver_ = new (std::nothrow)WindowStateObserver();
        if (windowStateObserver_) {
            OHOS::Rosen::WindowManager::GetInstance().RegisterFocusChangedListener(windowStateObserver_);
        }
    }
    if (!windowVisibilityObserver_) {
        windowVisibilityObserver_ = new (std::nothrow)WindowVisibilityObserver();
        if (windowVisibilityObserver_) {
            OHOS::Rosen::WindowManager::GetInstance().RegisterVisibilityChangedListener(windowVisibilityObserver_);
        }
    }
}

void SchedController::UnsubscribeWindowState()
{
    if (windowStateObserver_) {
        // unregister windowStateObserver_
        OHOS::Rosen::WindowManager::GetInstance().UnregisterFocusChangedListener(windowStateObserver_);
        windowStateObserver_ = nullptr;
    }

    if (windowVisibilityObserver_) {
        OHOS::Rosen::WindowManager::GetInstance().UnregisterVisibilityChangedListener(windowVisibilityObserver_);
        windowVisibilityObserver_ = nullptr;
    }
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

extern "C" void ReportAbilityStatus(int32_t saId, const std::string& deviceId, uint32_t status)
{
    SchedController::GetInstance().ReportAbilityStatus(saId, deviceId, status);
}
} // namespace ResourceSchedule
} // namespace OHOS
