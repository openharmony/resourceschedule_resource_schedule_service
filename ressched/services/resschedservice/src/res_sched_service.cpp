/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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

#include <cstdint>
#include <dlfcn.h>
#include "res_sched_service.h"
#include <file_ex.h>
#include <parameters.h>
#include <string_ex.h>
#include "accesstoken_kit.h"
#include "ipc_skeleton.h"
#include "notifier_mgr.h"
#include "plugin_mgr.h"
#include "res_sched_errors.h"
#include "res_sched_log.h"
#include "res_sched_mgr.h"
#include "tokenid_kit.h"
#include "sched_controller.h"
#include "supervisor.h"
#include "ressched_utils.h"

namespace OHOS {
namespace ResourceSchedule {
using namespace OHOS::Security;
namespace {
    constexpr int32_t DUMP_OPTION = 0;
    constexpr int32_t DUMP_PARAM_INDEX = 1;
    const int32_t ENG_MODE = OHOS::system::GetIntParameter("const.debuggable", 0);
    const std::string APP_PRELOAD_PLIGIN_NAME = "libapp_preload_plugin.z.so";
}

void ResSchedService::ReportData(uint32_t resType, int64_t value, const nlohmann::json& payload)
{
    int32_t clientPid = IPCSkeleton::GetCallingPid();
    RESSCHED_LOGD("ResSchedService receive data from ipc resType: %{public}u, value: %{public}lld, pid: %{public}d",
                  resType, (long long)value, clientPid);
    const nlohmann::json* payloadP = &payload;
    int32_t callingUid = IPCSkeleton::GetCallingUid();
    nlohmann::json* payloadM = const_cast<nlohmann::json*>(payloadP);
    (*payloadM)["callingUid"] = std::to_string(callingUid);
    (*payloadM)["clientPid"] = std::to_string(clientPid);
    ResSchedMgr::GetInstance().ReportData(resType, value, *payloadM);
}

int32_t ResSchedService::KillProcess(const nlohmann::json& payload)
{
    return ResSchedMgr::GetInstance().KillProcessByClient(payload);

}

void ResSchedService::RegisterSystemloadNotifier(const sptr<IRemoteObject>& notifier)
{
    NotifierMgr::GetInstance().RegisterNotifier(IPCSkeleton::GetCallingPid(), notifier);
}

void ResSchedService::UnRegisterSystemloadNotifier()
{
    NotifierMgr::GetInstance().UnRegisterNotifier(IPCSkeleton::GetCallingPid());
}

int32_t ResSchedService::GetSystemloadLevel()
{
    return NotifierMgr::GetInstance().GetSystemloadLevel();
}

void ResSchedService::OnDeviceLevelChanged(int32_t type, int32_t level)
{
    NotifierMgr::GetInstance().OnDeviceLevelChanged(type, level);
    nlohmann::json payload;
    payload["systemloadLevel"] = std::to_string(level);
    ResSchedUtils::GetInstance().ReportDataInProcess(type, level, payload);
}

bool ResSchedService::IsAllowedAppPreload(const std::string& bundleName, int32_t preloadMode)
{
    LoadAppPreloadPlugin();
    if (!appPreloadFunc_) {
        RESSCHED_LOGE("%{public}s, no allow AppPreload !", __func__, errno);
        return false;
    }
    return appPreloadFunc_(bundleName, preloadMode);
}

void ResSchedService::LoadAppPreloadPlugin()
{
    std::shared_ptr<PluginLib> libInfoPtr = PluginMgr::GetInstance().GetPluginLib(APP_PRELOAD_PLIGIN_NAME);
    if (libInfoPtr == nullptr) {
        RESSCHED_LOGE("ResSchedService::LoadAppPreloadPlugin libInfoPtr nullptr");
        isLoadAppPreloadPlugin_ = false;
        return;
    }

    if (isLoadAppPreloadPlugin_) {
        RESSCHED_LOGI("ResSchedService::LoadAppPreloadPlugin, already loaded AppPreloadPlugin");
        return;
    }

    appPreloadFunc_ = reinterpret_cast<OnIsAllowedAppPreloadFunc>(dlsym(libInfoPtr->handle.get(),
        "OnIsAllowedAppPreload"));
    isLoadAppPreloadPlugin_ = true;
}

bool ResSchedService::AllowDump()
{
    if (ENG_MODE == 0) {
        RESSCHED_LOGE("Not eng mode");
        return false;
    }
    Security::AccessToken::AccessTokenID tokenId = IPCSkeleton::GetCallingTokenID();
    int32_t ret = Security::AccessToken::AccessTokenKit::VerifyAccessToken(tokenId, "ohos.permission.DUMP");
    if (ret != Security::AccessToken::PermissionState::PERMISSION_GRANTED) {
        RESSCHED_LOGE("CheckPermission failed");
        return false;
    }
    return true;
}

int32_t ResSchedService::Dump(int32_t fd, const std::vector<std::u16string>& args)
{
    if (!AllowDump()) {
        return ERR_RES_SCHED_PERMISSION_DENIED;
    }
    RESSCHED_LOGI("%{public}s Dump service.", __func__);
    std::vector<std::string> argsInStr;
    std::transform(args.begin(), args.end(), std::back_inserter(argsInStr),
        [](const std::u16string &arg) {
        std::string ret = Str16ToStr8(arg);
        RESSCHED_LOGI("%{public}s arg: %{public}s.", __func__, ret.c_str());
        return ret;
    });
    std::string result;
    if (argsInStr.size() == 0) {
        // hidumper -s said '-h'
        DumpUsage(result);
    } else if (argsInStr.size() == DUMP_OPTION + 1) {
        // hidumper -s said '-h' or hidumper -s said '-a'
        if (argsInStr[DUMP_OPTION] == "-h") {
            DumpUsage(result);
        } else if (argsInStr[DUMP_OPTION] == "-a") {
            DumpAllInfo(result);
        } else if (argsInStr[DUMP_OPTION] == "-p") {
            PluginMgr::GetInstance().DumpAllPlugin(result);
        } else if (argsInStr[DUMP_OPTION] == "getRunningLockInfo") {
            DumpProcessRunningLock(result);
        } else if (argsInStr[DUMP_OPTION] == "getProcessEventInfo") {
            DumpProcessEventState(result);
        } else if (argsInStr[DUMP_OPTION] == "getProcessWindowInfo") {
            DumpProcessWindowInfo(result);
        } else if (argsInStr[DUMP_OPTION] == "getSystemloadInfo") {
            DumpSystemLoadInfo(result);
        } else {
            result.append("Error params.");
        }
    } else if (argsInStr.size() >= DUMP_PARAM_INDEX + 1) {
        if (argsInStr[DUMP_OPTION] == "-p") {
            std::vector<std::string> argsInStrToPlugin;
            argsInStrToPlugin.assign(argsInStr.begin() + DUMP_PARAM_INDEX + 1, argsInStr.end());
            PluginMgr::GetInstance().DumpOnePlugin(result, argsInStr[DUMP_PARAM_INDEX], argsInStrToPlugin);
        }
    }

    if (!SaveStringToFd(fd, result)) {
        RESSCHED_LOGE("%{public}s save to fd failed.", __func__);
    }
    return ERR_OK;
}

void ResSchedService::DumpProcessRunningLock(std::string &result)
{
    auto supervisor = SchedController::GetInstance().GetSupervisor();
    if (supervisor == nullptr) {
        result.append("get supervisor failed");
        return;
    }

    std::map<int32_t, std::shared_ptr<Application>> uidMap = supervisor->GetUidsMap();
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
                result.append("uid:").append(ToString(uid))
                    .append(", pid:").append(ToString(pid))
                    .append(", lockType:").append(ToString(lockType))
                    .append(", lockState:").append(ToString(lockState)).append("\n");
            }
        }
    }
}

void ResSchedService::DumpProcessWindowInfo(std::string &result)
{
    auto supervisor = SchedController::GetInstance().GetSupervisor();
    if (supervisor == nullptr) {
        result.append("get supervisor failed");
        return;
    }

    std::map<int32_t, std::shared_ptr<Application>> uidMap = supervisor->GetUidsMap();
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
            result.append("uid:").append(ToString(uid))
                .append(", pid:").append(ToString(pid))
                .append(", bundleName:").append(bundleName)
                .append(", processDrawingState:").append(ToString(process->processDrawingState_))
                .append(", windowInfo:").append("\n");
            for (auto &windows : process->windows_) {
                result.append("    windowId:").append(ToString(windows->windowId_))
                    .append(", visibilityState:").append(ToString(windows->visibilityState_))
                    .append(", isVisible:").append(ToString(windows->isVisible_))
                    .append(", isFocus:").append(ToString(windows->isFocused_))
                    .append(", topWebRenderUid:").append(ToString(windows->topWebviewRenderUid_))
                    .append("\n");
            }
        }
    }
}

void ResSchedService::DumpProcessEventState(std::string &result)
{
    auto supervisor = SchedController::GetInstance().GetSupervisor();
    if (supervisor == nullptr) {
        result.append("get supervisor failed");
        return;
    }

    std::map<int32_t, std::shared_ptr<Application>> uidMap = supervisor->GetUidsMap();
    for (auto it = uidMap.begin(); it != uidMap.end(); it++) {
        int32_t uid = it->first;
        std::shared_ptr<Application> app = it->second;
        std::map<pid_t, std::shared_ptr<ProcessRecord>> pidMap = app->GetPidsMap();
        for (auto pidIt = pidMap.begin(); pidIt != pidMap.end(); pidIt++) {
            int32_t pid = pidIt->first;
            std::shared_ptr<ProcessRecord> process = pidIt->second;
            result.append("uid:").append(ToString(uid))
                .append(", pid:").append(ToString(pid))
                .append(", processState:").append(ToString(process->processState_))
                .append(", napState:").append(ToString(process->isNapState_))
                .append(", processDrawingState:").append(ToString(process->processDrawingState_))
                .append(", mmiState:").append(ToString(process->mmiStatus_))
                .append(", camearaStatus:").append(ToString(process->cameraState_))
                .append(", bluetoothStatus:").append(ToString(process->bluetoothState_))
                .append(", wifiStatus:").append(ToString(process->wifiState_))
                .append(", screenCaptureState:").append(ToString(process->screenCaptureState_))
                .append(", videoState:").append(ToString(process->videoState_))
                .append(", audioPlayingState:").append(ToString(process->audioPlayingState_))
                .append(", isActive:").append(ToString(process->isActive_))
                .append(", linkedWindowId:").append(ToString(process->linkedWindowId_))
                .append("\n");
        }
    }
}

void ResSchedService::DumpSystemLoadInfo(std::string &result)
{
    result.append("systemloadLevel:")
        .append(ToString(NotifierMgr::GetInstance().GetSystemloadLevel()))
        .append("\n");
    auto notifierInfo = NotifierMgr::GetInstance().DumpRegisterInfo();
    std::string native("natives:");
    std::string hap("apps:");
    for (auto& info : notifierInfo) {
        std::string str = ToString(info.first).append(" ");
        if (info.second) {
            hap.append(str);
        } else {
            native.append(str);
        }
    }
    hap.append("\n");
    native.append("\n");
    result.append(native).append(hap);
}

void ResSchedService::DumpUsage(std::string &result)
{
    result.append("usage: resource schedule service dump [<options>]\n")
        .append("    -h: show the help.\n")
        .append("    -a: show all info.\n")
        .append("    -p: show the all plugin info.\n")
        .append("    -p (plugin name): show one plugin info.\n");
    PluginMgr::GetInstance().DumpHelpFromPlugin(result);
}

void ResSchedService::DumpAllInfo(std::string &result)
{
    result.append("================Resource Schedule Service Infos================\n");
    PluginMgr::GetInstance().DumpAllPlugin(result);
}
} // namespace ResourceSchedule
} // namespace OHOS

