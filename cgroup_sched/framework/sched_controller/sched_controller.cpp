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
#ifdef CONFIG_BGTASK_MGR
#include "background_task_mgr_helper.h"
#include "background_task_observer.h"
#endif
#include "bundle_mgr_interface.h"
#include "cgroup_adjuster.h"
#include "cgroup_event_handler.h"
#include "cgroup_sched_common.h"
#include "cgroup_sched_log.h"
#include "ressched_utils.h"
#include "res_type.h"
#include "supervisor.h"
#include "window_state_observer.h"

namespace OHOS {
namespace ResourceSchedule {
namespace {
    const std::string CG_HANDLER_THREAD = "CgroupEventHandler";
    constexpr HiviewDFX::HiLogLabel LOG_LABEL = {LOG_CORE, LOG_TAG_DOMAIN_ID_RMS, "SchedController"};
}

#ifdef CONFIG_BGTASK_MGR
using OHOS::BackgroundTaskMgr::BackgroundTaskMgrHelper;
#endif

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
}

void SchedController::UnregisterStateObservers()
{
    UnsubscribeAppState();
    UnsubscribeBackgroundTask();
    UnsubscribeWindowState();
}

int SchedController::GetProcessGroup(pid_t pid)
{
    if (!supervisor_) {
        CGS_LOGE("%{public}s, supervisor nullptr.", __func__);
        return (int32_t)(SP_DEFAULT);
    }
    std::shared_ptr<ProcessRecord> pr = supervisor_->FindProcessRecord(pid);
    return pr ? (int32_t)(pr->curSchedGroup_) : (int32_t)(SP_DEFAULT);
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

void SchedController::DispatchResource(uint32_t resType, int64_t value, const nlohmann::json& payload)
{
    auto handler = this->cgHandler_;
    if (!handler) {
        return;
    }
    handler->PostTask([handler, resType, value, payload] {
        switch (resType) {
            case ResType::RES_TYPE_REPORT_MMI_PROCESS: {
                handler->HandleReportMMIProcess(resType, value, payload);
                break;
            }
            case ResType::RES_TYPE_REPORT_RENDER_THREAD: {
                handler->HandleReportRenderThread(resType, value, payload);
                break;
            }
            default: {
                break;
            }
        }
    });
}

std::string SchedController::GetBundleNameByUid(const int32_t uid)
{
    std::string bundleName = "";
    OHOS::sptr<OHOS::ISystemAbilityManager> systemAbilityManager =
        OHOS::SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    OHOS::sptr<OHOS::IRemoteObject> object =
        systemAbilityManager->GetSystemAbility(OHOS::BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
    sptr<AppExecFwk::IBundleMgr> iBundleMgr = OHOS::iface_cast<OHOS::AppExecFwk::IBundleMgr>(object);
    if (!iBundleMgr) {
        CGS_LOGD("%{public}s null bundle manager.", __func__);
        return bundleName;
    }

    if (!iBundleMgr->GetBundleNameForUid(uid, bundleName)) {
        CGS_LOGD("%{public}s get bundle name failed for %{public}d.", __func__, uid);
    }
    return bundleName;
}

inline void SchedController::InitCgroupHandler()
{
    cgHandler_ = std::make_shared<CgroupEventHandler>(OHOS::AppExecFwk::EventRunner::Create(CG_HANDLER_THREAD));
    cgHandler_->SetSupervisor(supervisor_);
}

inline void SchedController::InitCgroupAdjuster()
{
    CgroupAdjuster::GetInstance().InitAdjuster();
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
        CGS_LOGE("%{public}s register to appmanager failed. err:%{public}d", __func__, err);
        appStateObserver_ = nullptr;
        return false;
    }
    CGS_LOGI("%{public}s success.", __func__);
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
            CGS_LOGI("%{public}s success.", __func__);
        } else {
            CGS_LOGE("%{public}s failed. err:%{public}d", __func__, err);
        }
    }
    appStateObserver_ = nullptr;
}

bool SchedController::SubscribeBackgroundTask()
{
#ifdef CONFIG_BGTASK_MGR
    if (backgroundTaskObserver_) {
        return true;
    }
    backgroundTaskObserver_ = std::make_shared<BackgroundTaskObserver>();
    int ret = BackgroundTaskMgrHelper::SubscribeBackgroundTask(*backgroundTaskObserver_);
    if (ret != 0) {
        backgroundTaskObserver_ = nullptr;
        CGS_LOGE("%{public}s failed, err:%{public}d.", __func__, ret);
        return false;
    }
    CGS_LOGI("%{public}s success.", __func__);
#endif
    return true;
}

void SchedController::UnsubscribeBackgroundTask()
{
#ifdef CONFIG_BGTASK_MGR
    if (!backgroundTaskObserver_) {
        return;
    }
    int32_t ret = BackgroundTaskMgrHelper::UnsubscribeBackgroundTask(*backgroundTaskObserver_);
    if (ret == 0) {
        CGS_LOGI("%{public}s success.", __func__);
    } else {
        CGS_LOGE("%{public}s failed. ret:%{public}d", __func__, ret);
    }
    backgroundTaskObserver_ = nullptr;
#endif
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
    CGS_LOGI("%{public}s success.", __func__);
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

extern "C" void CgroupSchedDispatch(uint32_t resType, int64_t value, const nlohmann::json& payload)
{
    SchedController::GetInstance().DispatchResource(resType, value, payload);
}
} // namespace ResourceSchedule
} // namespace OHOS
