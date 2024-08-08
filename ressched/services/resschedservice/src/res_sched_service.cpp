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
#include "res_sched_exe_client.h"
#include "res_sched_log.h"
#include "res_sched_mgr.h"
#include "tokenid_kit.h"
#include "event_listener_mgr.h"

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

int32_t ResSchedService::ReportSyncEvent(const uint32_t resType, const int64_t value, const nlohmann::json& payload,
    nlohmann::json& reply)
{
    return PluginMgr::GetInstance().DeliverResource(std::make_shared<ResData>(resType, value, payload, reply));
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

void ResSchedService::RegisterEventListener(const sptr<IRemoteObject>& eventListener, uint32_t eventType)
{
    EventListenerMgr::GetInstance().RegisterEventListener(IPCSkeleton::GetCallingPid(), eventListener, eventType);
}

void ResSchedService::UnRegisterEventListener(uint32_t eventType)
{
    EventListenerMgr::GetInstance().UnRegisterEventListener(IPCSkeleton::GetCallingPid(), eventType);
}

int32_t ResSchedService::GetSystemloadLevel()
{
    return NotifierMgr::GetInstance().GetSystemloadLevel();
}

void ResSchedService::OnDeviceLevelChanged(int32_t type, int32_t level)
{
    NotifierMgr::GetInstance().OnDeviceLevelChanged(type, level);
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
        DumpExt(argsInStr, result);
    } else if (argsInStr.size() >= DUMP_PARAM_INDEX + 1) {
        if (argsInStr[DUMP_OPTION] == "-p") {
            std::vector<std::string> argsInStrToPlugin;
            argsInStrToPlugin.assign(argsInStr.begin() + DUMP_PARAM_INDEX + 1, argsInStr.end());
            PluginMgr::GetInstance().DumpOnePlugin(result, argsInStr[DUMP_PARAM_INDEX], argsInStrToPlugin);
        } else if (argsInStr[DUMP_OPTION] == "sendDebugToExecutor") {
            DumpExecutorDebugCommand(argsInStr, result);
        }
    }

    if (!SaveStringToFd(fd, result)) {
        RESSCHED_LOGE("%{public}s save to fd failed.", __func__);
    }
    return ERR_OK;
}


void ResSchedService::DumpExt(const std::vector<std::string>& argsInStr, std::string &result)
{
    if (argsInStr[DUMP_OPTION] == "-h") {
        DumpUsage(result);
    } else if (argsInStr[DUMP_OPTION] == "-a") {
        DumpAllInfo(result);
    } else if (argsInStr[DUMP_OPTION] == "-p") {
        PluginMgr::GetInstance().DumpAllPlugin(result);
    } else if (argsInStr[DUMP_OPTION] == "getSystemloadInfo") {
        DumpSystemLoadInfo(result);
    } else if (argsInStr[DUMP_OPTION] == "sendDebugToExecutor") {
        DumpExecutorDebugCommand(argsInStr, result);
    } else if (argsInStr[DUMP_OPTION] == "PluginConfig") {
        DumpAllPluginConfig(result);
    } else {
        result.append("Error params.");
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

void ResSchedService::DumpExecutorDebugCommand(const std::vector<std::string>& args, std::string& result)
{
    // hidumper -s said 'sendDebugToExecutor [isSync times]' isSync - 0/1(default 0), times - 1~...(default 1)
    result.append("Send debug command to resource_schedule_executor.\n");
    bool isSync = true;
    int times = 1;
    if (args.size() > DUMP_PARAM_INDEX + 1) {
        int arg = atoi(args[DUMP_PARAM_INDEX + 1].c_str());
        times = arg > 0 ? arg : times;
    }
    if (args.size() > DUMP_PARAM_INDEX) {
        isSync = atoi(args[DUMP_PARAM_INDEX].c_str()) == 0;
    }
    uint32_t internal = 200;
    for (int i = 0; i < times; i++) {
        ResSchedExeClient::GetInstance().SendDebugCommand(isSync);
        usleep(internal);
    }
}

void ResSchedService::DumpAllPluginConfig(std::string &result)
{
    result.append("================Resource Schedule Plugin Config================\n");
    PluginMgr::GetInstance().DumpAllPluginConfig(result);
}
} // namespace ResourceSchedule
} // namespace OHOS

