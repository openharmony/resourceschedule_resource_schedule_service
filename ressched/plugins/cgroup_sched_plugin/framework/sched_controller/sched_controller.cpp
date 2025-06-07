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
#ifdef CONFIG_BGTASK_MGR
#include "background_task_mgr_helper.h"
#include "background_task_observer.h"
#endif
#include "bundle_mgr_interface.h"
#include "cgroup_adjuster.h"
#include "cgroup_event_handler.h"
#include "cgroup_sched_log.h"
#include "hisysevent.h"
#include "plugin_mgr.h"
#include "ressched_utils.h"
#include "res_type.h"
#include "supervisor.h"
#ifdef POWER_MANAGER_ENABLE
#include "power_mgr_client.h"
#endif

#undef LOG_TAG
#define LOG_TAG "SchedController"

namespace OHOS {
namespace ResourceSchedule {
namespace {
    const std::string CG_HANDLER_QUEUE = "CgroupEventHandlerQueue";
    const std::string LIB_NAME = "libcgroup_sched.z.so";
    constexpr int32_t DUMP_OPTION = 0;
}

#ifdef CONFIG_BGTASK_MGR
using OHOS::BackgroundTaskMgr::BackgroundTaskMgrHelper;
#endif

IMPLEMENT_SINGLE_INSTANCE(SchedController)

void SchedController::Init()
{
    ChronoScope cs("Init SchedController.");
    // Init supervisor which contains cached data for ccgroup controller.
    InitSupervisor();
    // init dispatch resource function map
    InitDispatchResFuncMap();
    // Init cgroup handler thread
    InitCgroupHandler();
    // Init cgroup adjuster thread
    InitCgroupAdjuster();
    // Subscribe ResTypes
    InitResTypes();
    for (auto resType: resTypes) {
        PluginMgr::GetInstance().SubscribeResource(LIB_NAME, resType);
    }
    ResSchedUtils::GetInstance().SubscribeResourceExt();
}

void SchedController::Disable()
{
    if (cgHandler_) {
        cgHandler_->PostTaskAndWait([]() {});
        cgHandler_ = nullptr;
    }
    if (supervisor_) {
        supervisor_ = nullptr;
    }
    UnregisterStateObservers();
}


void SchedController::UnregisterStateObservers()
{
    UnsubscribeBackgroundTask();
    //UnsubscribeWindowState();
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

void SchedController::InitResTypes()
{
    resTypes = {
        ResType::RES_TYPE_REPORT_MMI_PROCESS,
        ResType::RES_TYPE_REPORT_RENDER_THREAD,
        ResType::RES_TYPE_REPORT_KEY_THREAD,
        ResType::RES_TYPE_REPORT_WINDOW_STATE,
        ResType::RES_TYPE_WEBVIEW_AUDIO_STATUS_CHANGE,
        ResType::RES_TYPE_INNER_AUDIO_STATE,
        ResType::RES_TYPE_RUNNINGLOCK_STATE,
        ResType::RES_TYPE_REPORT_SCENE_BOARD,
        ResType::RES_TYPE_WEBVIEW_SCREEN_CAPTURE,
        ResType::RES_TYPE_WEBVIEW_VIDEO_STATUS_CHANGE,
        ResType::RES_TYPE_SYSTEM_ABILITY_STATUS_CHANGE,
        ResType::RES_TYPE_MMI_STATUS_CHANGE,
        ResType::RES_TYPE_REPORT_CAMERA_STATE,
        ResType::RES_TYPE_BLUETOOTH_A2DP_CONNECT_STATE_CHANGE,
        ResType::RES_TYPE_WIFI_CONNECT_STATE_CHANGE,
        ResType::RES_TYPE_MMI_INPUT_STATE,
        ResType::RES_TYPE_REPORT_SCREEN_CAPTURE,
        ResType::RES_TYPE_AV_CODEC_STATE,
        ResType::RES_TYPE_SA_CONTROL_APP_EVENT,
        ResType::RES_TYPE_FORM_STATE_CHANGE_EVENT,
        ResType::RES_TYPE_CONTINUOUS_STARTUP,
        ResType::RES_TYPE_SCREEN_LOCK,
        ResType::RES_TYPE_BOOT_COMPLETED,
        ResType::RES_TYPE_SYSTEM_CPU_LOAD,
        ResType::RES_TYPE_THERMAL_STATE,
        ResType::RES_TYPE_COSMIC_CUBE_STATE_CHANGE,
        ResType::RES_TYPE_APP_STOPPED,
        ResType::RES_TYPE_ABILITY_STATE_CHANGE,
        ResType::RES_TYPE_EXTENSION_STATE_CHANGE,
        ResType::RES_TYPE_PROCESS_STATE_CHANGE,
        ResType::RES_TYPE_APP_STATE_CHANGE,
    };
}

void SchedController::DispatchResource(const std::shared_ptr<ResData>& resData)
{
    auto handler = this->cgHandler_;
    if (!handler) {
        return;
    }

    auto iter = dispatchResFuncMap_.find(resData->resType);
    if (iter == dispatchResFuncMap_.end()) {
        DispatchOtherResource(resData->resType, resData->value, resData->payload);
        return;
    }
    handler->PostTask([func = iter->second, handler, resData] {
        func(handler, resData->resType, resData->value, resData->payload);
    });
    DispatchOtherResource(resData->resType, resData->value, resData->payload);
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
            case ResType::RES_TYPE_WIFI_CONNECT_STATE_CHANGE: {
                handler->HandleReportHisysEvent(resType, value, payload);
                break;
            }
            case ResType::RES_TYPE_MMI_INPUT_STATE: {
                handler->HandleMmiInputState(resType, value, payload);
                break;
            }
            case ResType::RES_TYPE_BLUETOOTH_A2DP_CONNECT_STATE_CHANGE: {
                handler->HandleReportBluetoothConnectState(resType, value, payload);
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
    cgHandler_ = std::make_shared<CgroupEventHandler>(CG_HANDLER_QUEUE);
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
        { ResType::RES_TYPE_INNER_AUDIO_STATE, [](std::shared_ptr<CgroupEventHandler> handler,
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
        { ResType::RES_TYPE_REPORT_SCREEN_CAPTURE, [](std::shared_ptr<CgroupEventHandler> handler,
            uint32_t resType, int64_t value, const nlohmann::json& payload)
            { handler->HandleReportScreenCaptureEvent(resType, value, payload); } },
        { ResType::RES_TYPE_WEBVIEW_VIDEO_STATUS_CHANGE, [](std::shared_ptr<CgroupEventHandler> handler,
            uint32_t resType, int64_t value, const nlohmann::json& payload)
            { handler->HandleReportWebviewVideoState(resType, value, payload); } },
        { ResType::RES_TYPE_SYSTEM_ABILITY_STATUS_CHANGE, [](std::shared_ptr<CgroupEventHandler> handler,
            uint32_t resType, int64_t value, const nlohmann::json& payload)
            { handler->ReportAbilityStatus(resType, value, payload); } },
        { ResType::RES_TYPE_MMI_STATUS_CHANGE, [](std::shared_ptr<CgroupEventHandler> handler,
            uint32_t resType, int64_t value, const nlohmann::json& payload)
            { handler->UpdateMmiStatus(resType, value, payload); } },
        { ResType::RES_TYPE_COSMIC_CUBE_STATE_CHANGE, [](std::shared_ptr<CgroupEventHandler> handler,
            uint32_t resType, int64_t value, const nlohmann::json& payload)
            { handler->HandleReportCosmicCubeState(resType, value, payload); } },
        { ResType::RES_TYPE_APP_STOPPED, [](std::shared_ptr<CgroupEventHandler> handler,
            uint32_t resType, int64_t value, const nlohmann::json& payload)
            { handler->HandleOnAppStopped(resType, value, payload); } },
    };
    InitAddDispatchResFuncMap();
}

void SchedController::InitAddDispatchResFuncMap()
{
    dispatchResFuncMap_.insert(std::make_pair(ResType::RES_TYPE_ABILITY_STATE_CHANGE,
        [](std::shared_ptr<CgroupEventHandler> handler, uint32_t resType, int64_t value,
        const nlohmann::json& payload) { handler->HandleAbilityStateChanged(resType, value, payload); }));
    dispatchResFuncMap_.insert(std::make_pair(ResType::RES_TYPE_EXTENSION_STATE_CHANGE,
        [](std::shared_ptr<CgroupEventHandler> handler, uint32_t resType, int64_t value,
        const nlohmann::json& payload) { handler->HandleExtensionStateChanged(resType, value, payload); }));
    dispatchResFuncMap_.insert(std::make_pair(ResType::RES_TYPE_PROCESS_STATE_CHANGE,
        [](std::shared_ptr<CgroupEventHandler> handler, uint32_t resType, int64_t value,
        const nlohmann::json& payload) { handler->HandleProcessStateChangedEx(resType, value, payload); }));
    dispatchResFuncMap_.insert(std::make_pair(ResType::RES_TYPE_APP_STATE_CHANGE,
        [](std::shared_ptr<CgroupEventHandler> handler, uint32_t resType, int64_t value,
        const nlohmann::json& payload) { handler->HandleApplicationStateChanged(resType, value, payload); }));
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

void SchedController::Dump(const std::vector<std::string>& args, std::string& result)
{
    if (args.size() == 0) {
        return;
    }
    if (args.size() == DUMP_OPTION + 1 && args[DUMP_OPTION] == "-h") {
        DumpHelp(result);
    } else if (args.size() == DUMP_OPTION + 1 && args[DUMP_OPTION] == "-getRunningLockInfo") {
        DumpProcessRunningLock(result);
    } else if (args.size() == DUMP_OPTION + 1 && args[DUMP_OPTION] == "-getProcessEventInfo") {
        DumpProcessEventState(result);
    } else if (args.size() == DUMP_OPTION + 1 && args[DUMP_OPTION] == "-getProcessWindowInfo") {
        DumpProcessWindowInfo(result);
    }
}

void SchedController::DumpHelp(std::string& result)
{
    result.append("        plugin name : libcgroup_sched.z.so \n")
        .append("                   -h: show the cgroup_sched_plugin help. \n")
        .append("                   -getRunningLockInfo: get all runnable process lock info. \n")
        .append("                   -getProcessEventInfo: get all runnable process event info. \n")
        .append("                   -getProcessWindowInfo: get all runnable process window info. \n");
}

void SchedController::DumpProcessRunningLock(std::string& result)
{
    std::map<int32_t, std::shared_ptr<Application>> uidMap = supervisor_->GetUidsMap();
    for (auto it = uidMap.begin(); it != uidMap.end(); it++) {
        int32_t uid = it->first;
        std::shared_ptr<Application> app = it->second;
        std::map<pid_t, std::shared_ptr<ProcessRecord>> pidMap = app->GetPidsMap();
        for (auto pidIt = pidMap.begin(); pidIt != pidMap.end(); pidIt++) {
            int32_t pid = pidIt->first;
            std::shared_ptr<ProcessRecord> process = pidIt->second;
            for (auto lockIt = process->runningLockState_.begin();
                lockIt != process->runningLockState_.end(); lockIt++) {
                uint32_t lockType = lockIt->first;
                bool lockState = lockIt->second;
                result.append("uid: ").append(ToString(uid))
                    .append(", pid: ").append(ToString(pid))
                    .append(", lockType: ").append(ToString(lockType))
                    .append(", lockState: ").append(ToString(lockState));
            }
        }
    }
}

void SchedController::DumpProcessEventState(std::string& result)
{
    std::map<int32_t, std::shared_ptr<Application>> uidMap = supervisor_->GetUidsMap();
    for (auto it = uidMap.begin(); it != uidMap.end(); it++) {
        int32_t uid = it->first;
        std::shared_ptr<Application> app = it->second;
        std::map<pid_t, std::shared_ptr<ProcessRecord>> pidMap = app->GetPidsMap();
        for (auto pidIt = pidMap.begin(); pidIt != pidMap.end(); pidIt++) {
            int32_t pid = pidIt->first;
            std::shared_ptr<ProcessRecord> process = pidIt->second;
            result.append("uid: ").append(ToString(uid))
                .append(", pid: ").append(ToString(pid))
                .append(", processState: ").append(ToString(process->processState_))
                .append(", napState: ").append(ToString(process->isNapState_))
                .append(", processDrawingState: ").append(ToString(process->processDrawingState_))
                .append(", mmiState: ").append(ToString(process->mmiStatus_))
                .append(", cameraState: ").append(ToString(process->cameraState_))
                .append(", bluetoothState: ").append(ToString(process->bluetoothState_))
                .append(", wifiState: ").append(ToString(process->wifiState_))
                .append(", screenCaptureState: ").append(ToString(process->screenCaptureState_))
                .append(", videoState: ").append(ToString(process->videoState_))
                .append(", isActive: ").append(ToString(process->isActive_))
                .append(", linkedWindowId: ").append(ToString(process->linkedWindowId_))
                .append(", audioPlayingState: ").append(ToString(process->audioPlayingState_));
            result.append("\n");
        }
    }
}

void SchedController::DumpProcessWindowInfo(std::string& result)
{
    std::map<int32_t, std::shared_ptr<Application>> uidMap = supervisor_->GetUidsMap();
    for (auto it = uidMap.begin(); it != uidMap.end(); it++) {
        int32_t uid = it->first;
        std::shared_ptr<Application> app = it->second;
        std::map<pid_t, std::shared_ptr<ProcessRecord>> pidMap = app->GetPidsMap();
        std::string bundleName = app->GetName();
        for (auto pidIt = pidMap.begin(); pidIt != pidMap.end(); pidIt++) {
            int32_t pid = pidIt->first;
            std::shared_ptr<ProcessRecord> process = pidIt->second;
            if (process->windows_.size() == 0) {
                continue;
            }
            result.append("uid: ").append(ToString(uid))
                .append(", pid: ").append(ToString(pid))
                .append(", bundleName: ").append(bundleName)
                .append(", processDrawingState: ").append(ToString(process->processDrawingState_))
                .append(", windowInfo: ")
                .append("\n");
            for (auto &windows : process->windows_) {
                result.append("    windowId:").append(ToString(windows->windowId_))
                    .append("    visibilityState:").append(ToString(windows->visibilityState_))
                    .append("    isVisible:").append(ToString(windows->isVisible_))
                    .append("    isFocus:").append(ToString(windows->isFocused_))
                    .append("    topWebviewRenderUid:").append(ToString(windows->topWebviewRenderUid_))
                    .append("\n");
            }
        }
    }
}

extern "C" bool OnPluginInit(std::string& libName)
{
    if (libName != LIB_NAME) {
        CGS_LOGE("SchedController::OnPluginInit lib name is not match.");
        return false;
    }
    SchedController::GetInstance().Init();
    return true;
}

extern "C" void OnPluginDisable()
{
    SchedController::GetInstance().Disable();
}

extern "C" void OnDispatchResource(const std::shared_ptr<ResData>& resData)
{
    SchedController::GetInstance().DispatchResource(resData);
}

extern "C" int GetProcessGroup(const pid_t pid)
{
    return SchedController::GetInstance().GetProcessGroup(pid);
}

extern "C" void OnDump(const std::vector<std::string>& args, std::string& result)
{
    SchedController::GetInstance().Dump(args, result);
}

} // namespace ResourceSchedule
} // namespace OHOS
