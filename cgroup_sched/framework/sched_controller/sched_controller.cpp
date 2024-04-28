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
#include "app_startup_scene_rec.h"
#ifdef CONFIG_BGTASK_MGR
#include "background_task_mgr_helper.h"
#include "background_task_observer.h"
#endif
#include "bundle_mgr_interface.h"
#include "cgroup_adjuster.h"
#include "cgroup_event_handler.h"
#include "cgroup_sched_common.h"
#include "cgroup_sched_log.h"
#include "hisysevent.h"
#include "ressched_utils.h"
#include "res_type.h"
#include "supervisor.h"
#include "window_state_observer.h"
#ifdef POWER_MANAGER_ENABLE
#include "power_mgr_client.h"
#endif

#undef LOG_TAG
#define LOG_TAG "SchedController"

namespace OHOS {
namespace ResourceSchedule {
namespace {
    const std::string CG_HANDLER_THREAD = "CgroupEventHandler";
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
    // init dispatch resource function map
    InitDispatchResFuncMap();
    InitAppStartupSceneRec();
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
    DeinitAppStartupSceneRec();
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

    auto iter = dispatchResFuncMap_.find(resType);
    if (iter == dispatchResFuncMap_.end()) {
        DispatchOtherResource(resType, value, payload);
        return;
    }
    handler->PostTask([func = iter->second, handler, resType, value, payload] {
        func(handler, resType, value, payload);
    });
    DispatchOtherResource(resType, value, payload);
}

void SchedController::DispatchOtherResource(uint32_t resType, int64_t value, const nlohmann::json& payload)
{
    auto handler = this->cgHandler_;
    if (!handler) {
        return;
    }
    handler->PostTask([handler, resType, value, payload] {
        switch (resType) {
            case ResType::RES_TYPE_REPORT_CAMERA_STATE:
            case ResType::RES_TYPE_BLUETOOTH_A2DP_CONNECT_STATE_CHANGE:
            case ResType::RES_TYPE_WIFI_CONNECT_STATE_CHANGE:
            case ResType::RES_TYPE_MMI_INPUT_STATE:
            case ResType::RES_TYPE_REPORT_SCREEN_CAPTURE: {
                handler->HandleReportHisysEvent(resType, value, payload);
                break;
            }
            case ResType::RES_TYPE_AV_CODEC_STATE: {
                handler->HandleReportAvCodecEvent(resType, value, payload);
                break;
            }
            default: {
                break;
            }
        }
    });
    ResSchedUtils::GetInstance().DispatchResourceExt(resType, value, payload);
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

    ErrCode ret = iBundleMgr->GetNameForUid(uid, bundleName);
    if (ret != ERR_OK) {
        CGS_LOGD("%{public}s get bundle name failed for %{public}d, err_code:%{public}d.", __func__, uid, ret);
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

inline void SchedController::InitAppStartupSceneRec()
{
    AppStartupSceneRec::GetInstance().Init();
}

inline void SchedController::DeinitAppStartupSceneRec()
{
    AppStartupSceneRec::GetInstance().Deinit();
}
void SchedController::InitDispatchResFuncMap()
{
    dispatchResFuncMap_ = {
        { ResType::RES_TYPE_REPORT_MMI_PROCESS, [](std::shared_ptr<CgroupEventHandler> handler,
            uint32_t resType, int64_t value, const nlohmann::json& payload)
            { handler->HandleReportMMIProcess(resType, value, payload); } },
        { ResType::RES_TYPE_REPORT_RENDER_THREAD, [](std::shared_ptr<CgroupEventHandler> handler,
            uint32_t resType, int64_t value, const nlohmann::json& payload)
            { handler->HandleReportRenderThread(resType, value, payload); } },
        { ResType::RES_TYPE_REPORT_KEY_THREAD, [](std::shared_ptr<CgroupEventHandler> handler,
            uint32_t resType, int64_t value, const nlohmann::json& payload)
            { handler->HandleReportKeyThread(resType, value, payload); } },
        { ResType::RES_TYPE_REPORT_WINDOW_STATE, [](std::shared_ptr<CgroupEventHandler> handler,
            uint32_t resType, int64_t value, const nlohmann::json& payload)
            { handler->HandleReportWindowState(resType, value, payload); } },
        { ResType::RES_TYPE_WEBVIEW_AUDIO_STATUS_CHANGE, [](std::shared_ptr<CgroupEventHandler> handler,
            uint32_t resType, int64_t value, const nlohmann::json& payload)
            { handler->HandleReportWebviewAudioState(resType, value, payload); } },
        { ResType::RES_TYPE_AUDIO_RENDER_STATE_CHANGE, [](std::shared_ptr<CgroupEventHandler> handler,
            uint32_t resType, int64_t value, const nlohmann::json& payload)
            { handler->HandleReportAudioState(resType, value, payload); } },
        { ResType::RES_TYPE_RUNNINGLOCK_STATE, [](std::shared_ptr<CgroupEventHandler> handler,
            uint32_t resType, int64_t value, const nlohmann::json& payload)
            { handler->HandleReportRunningLockEvent(resType, value, payload); } },
        { ResType::RES_TYPE_REPORT_SCENE_BOARD, [](std::shared_ptr<CgroupEventHandler> handler,
            uint32_t resType, int64_t value, const nlohmann::json& payload)
            { handler->HandleSceneBoardState(resType, value, payload); } },
        { ResType::RES_TYPE_WEBVIEW_SCREEN_CAPTURE, [](std::shared_ptr<CgroupEventHandler> handler,
            uint32_t resType, int64_t value, const nlohmann::json& payload)
            { handler->HandleWebviewScreenCapture(resType, value, payload); } },
        { ResType::RES_TYPE_WEBVIEW_VIDEO_STATUS_CHANGE, [](std::shared_ptr<CgroupEventHandler> handler,
            uint32_t resType, int64_t value, const nlohmann::json& payload)
            { handler->HandleReportWebviewVideoState(resType, value, payload); } },
    };
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
    if (isBgtaskSubscribed_) {
        return true;
    }
    if (!backgroundTaskObserver_) {
        backgroundTaskObserver_ = std::make_shared<BackgroundTaskObserver>();
    }
    int ret = BackgroundTaskMgrHelper::SubscribeBackgroundTask(*backgroundTaskObserver_);
    if (ret != 0) {
        CGS_LOGE("%{public}s failed, err:%{public}d.", __func__, ret);
        return false;
    }
    isBgtaskSubscribed_ = true;
    CGS_LOGI("%{public}s success.", __func__);
#endif
    return true;
}

void SchedController::UnsubscribeBackgroundTask()
{
#ifdef CONFIG_BGTASK_MGR
    if (!isBgtaskSubscribed_ || !backgroundTaskObserver_) {
        return;
    }
    int32_t ret = BackgroundTaskMgrHelper::UnsubscribeBackgroundTask(*backgroundTaskObserver_);
    if (ret == 0) {
        CGS_LOGI("%{public}s success.", __func__);
    } else {
        CGS_LOGE("%{public}s failed. ret:%{public}d", __func__, ret);
    }
    isBgtaskSubscribed_ = false;
#endif
}

void SchedController::SubscribeWindowState()
{
    if (!windowStateObserver_) {
        windowStateObserver_ = new (std::nothrow)WindowStateObserver();
        if (windowStateObserver_) {
            if (OHOS::Rosen::WindowManagerLite::GetInstance().
            RegisterFocusChangedListener(windowStateObserver_) != OHOS::Rosen::WMError::WM_OK) {
                HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::RSS, "INIT_FAULT", HiviewDFX::HiSysEvent::EventType::FAULT,
                                "COMPONENT_NAME", "MAIN", "ERR_TYPE", "register failure",
                                "ERR_MSG", "Register a listener of window focus change failed.");
            }
        }
    }
    if (!windowVisibilityObserver_) {
        windowVisibilityObserver_ = new (std::nothrow)WindowVisibilityObserver();
        if (windowVisibilityObserver_) {
            if (OHOS::Rosen::WindowManagerLite::GetInstance().
            RegisterVisibilityChangedListener(windowVisibilityObserver_) != OHOS::Rosen::WMError::WM_OK) {
                HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::RSS, "INIT_FAULT", HiviewDFX::HiSysEvent::EventType::FAULT,
                                "COMPONENT_NAME", "MAIN", "ERR_TYPE", "register failure",
                                "ERR_MSG", "Register a listener of window visibility change failed.");
            }
        }
    }
    if (!windowDrawingContentObserver_) {
        windowDrawingContentObserver_ = new (std::nothrow)WindowDrawingContentObserver();
        if (windowDrawingContentObserver_) {
            if (OHOS::Rosen::WindowManagerLite::GetInstance().
                RegisterDrawingContentChangedListener(windowDrawingContentObserver_) != OHOS::Rosen::WMError::WM_OK) {
                    HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::RSS, "INIT_FAULT",
                                    HiviewDFX::HiSysEvent::EventType::FAULT,
                                    "COMPONENT_NAME", "MAIN", "ERR_TYPE", "register failure",
                                    "ERR_MSG", "Register a listener of window draw content change failed.");
            }
        }
    }
    SubscribeWindowModeChange();
    CGS_LOGI("%{public}s success.", __func__);
}

void SchedController::SubscribeWindowModeChange()
{
    if (!windowModeObserver_) {
        windowModeObserver_ = new (std::nothrow)WindowModeObserver();
        if (windowModeObserver_) {
            if (OHOS::Rosen::WindowManagerLite::GetInstance().
                RegisterWindowModeChangedListener(windowModeObserver_) != OHOS::Rosen::WMError::WM_OK) {
                    CGS_LOGE("RegisterWindowModeChangedListener fail");
                    HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::RSS,
                                    "INIT_FAULT", HiviewDFX::HiSysEvent::EventType::FAULT,
                                    "COMPONENT_NAME", "MAIN", "ERR_TYPE", "register failure",
                                    "ERR_MSG", "Register a listener of window mode content change failed.");
            }
        }
    }
}

void SchedController::UnsubscribeWindowState()
{
    if (windowStateObserver_) {
        // unregister windowStateObserver_
        OHOS::Rosen::WindowManagerLite::GetInstance().UnregisterFocusChangedListener(windowStateObserver_);
        windowStateObserver_ = nullptr;
    }

    if (windowVisibilityObserver_) {
        OHOS::Rosen::WindowManagerLite::GetInstance().UnregisterVisibilityChangedListener(windowVisibilityObserver_);
        windowVisibilityObserver_ = nullptr;
    }

    if (windowDrawingContentObserver_) {
        OHOS::Rosen::WindowManagerLite::GetInstance().
            UnregisterDrawingContentChangedListener(windowDrawingContentObserver_);
        windowDrawingContentObserver_ = nullptr;
    }
    UnsubscribeWindowModeChange();
}

void SchedController::UnsubscribeWindowModeChange()
{
    if (windowModeObserver_) {
        OHOS::Rosen::WindowManagerLite::GetInstance().
            UnregisterWindowModeChangedListener(windowModeObserver_);
        windowModeObserver_ = nullptr;
    }
    CGS_LOGI("UnsubscribeWindowModeChange success");
}

#ifdef POWER_MANAGER_ENABLE
void SchedController::GetRunningLockState()
{
    if (!supervisor_) {
        CGS_LOGE("%{public}s, supervisor nullptr.", __func__);
        return;
    }
    std::map<std::string, PowerMgr::RunningLockInfo> runningLockLists;
    bool ret = PowerMgr::PowerMgrClient::GetInstance().QueryRunningLockLists(runningLockLists);
    if (!ret) {
        CGS_LOGE("%{public}s get running lock list failed.", __func__);
        return;
    }
    for (auto it = runningLockLists.begin(); it != runningLockLists.end(); it++) {
        std::string bundleName = it->first;
        PowerMgr::RunningLockInfo lockInfo = it->second;
        std::shared_ptr<Application> app = supervisor_->GetAppRecordNonNull(lockInfo.uid);
        std::shared_ptr<ProcessRecord> procRecord = app->GetProcessRecordNonNull(lockInfo.pid);
        uint32_t key = static_cast<uint32_t>(lockInfo.type);
        procRecord->runningLockState_[key] = true;
    }
}
#endif

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
