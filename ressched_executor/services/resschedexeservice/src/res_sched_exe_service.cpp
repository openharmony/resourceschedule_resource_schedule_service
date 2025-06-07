/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "res_sched_exe_service.h"

#include <file_ex.h>
#include <parameters.h>
#include <string_ex.h>

#include "accesstoken_kit.h"
#include "ipc_skeleton.h"
#include "system_ability_definition.h"

#include "plugin_mgr.h"
#include "executor_hitrace_chain.h"
#include "res_common_util.h"
#include "res_exe_type.h"
#include "res_sched_exe_constants.h"
#include "res_sched_exe_log.h"
#include "res_sched_exe_mgr.h"

namespace OHOS {
namespace ResourceSchedule {
namespace {
    constexpr int32_t DUMP_OPTION = 0;
    constexpr int32_t DUMP_PARAM_INDEX = 1;
    const int32_t ENG_MODE = OHOS::system::GetIntParameter("const.debuggable", 0);
    const std::string DUMP_PERMISSION = "ohos.permission.DUMP";
    constexpr int32_t PAYLOAD_MAX_SIZE = 4096;
    constexpr int32_t KILL_PROCESS_FAILED = -1;
    constexpr int32_t RSS_UID = 1096;
    const std::string RES_TYPE_EXT = "extType";

    bool IsTypeVaild(uint32_t type)
    {
        return type >= ResExeType::RES_TYPE_FIRST && type < ResExeType::RES_TYPE_LAST;
    }

    bool IsCallingClientRss()
    {
        int32_t clientUid = IPCSkeleton::GetCallingUid();
        RSSEXE_LOGD("calling client uid is %{public}d, allowed uid is %{public}d", clientUid, RSS_UID);
        return RSS_UID == clientUid;
    }

    bool GetExtResType(uint32_t& resType, const nlohmann::json& context)
    {
        if (resType != ResExeType::RES_TYPE_COMMON_SYNC && resType != ResExeType::RES_TYPE_COMMON_ASYNC) {
            return true;
        }
        int type = 0;
        if (!context.contains(RES_TYPE_EXT) || !context[RES_TYPE_EXT].is_string()
            || !StrToInt(context[RES_TYPE_EXT], type)) {
            RSSEXE_LOGE("use extend resType, but not send resTypeExt with payload");
            return false;
        }
        resType = (uint32_t)type;
        RSSEXE_LOGD("use extend resType = %{public}d.", resType);
        return true;
    }
}

const bool REGISTER_RESULT =
    SystemAbility::MakeAndRegisterAbility(DelayedSingleton<ResSchedExeService>::GetInstance().get());

ResSchedExeService::ResSchedExeService() : SystemAbility(RES_SCHED_EXE_ABILITY_ID, true)
{
}

ResSchedExeService::~ResSchedExeService()
{
}

void ResSchedExeService::OnStart()
{
    ExecutorHiTraceChain traceChain(__func__);
    ResSchedExeMgr::GetInstance().Init();
    if (!Publish(DelayedSingleton<ResSchedExeService>::GetInstance().get())) {
        RSSEXE_LOGE("ResSchedExeService::Register service failed.");
        return;
    }
    RSSEXE_LOGI("ResSchedExeService::OnStart.");
}

void ResSchedExeService::OnStop()
{
    ExecutorHiTraceChain traceChain(__func__);
    ResSchedExeMgr::GetInstance().Stop();
    RSSEXE_LOGI("ResSchedExeService::OnStop!");
}

ErrCode ResSchedExeService::SendRequestSync(uint32_t resType, int64_t value,
                                            const ResJsonType &context, ResJsonType &response, int32_t &funcResult)
{
    ExecutorHiTraceChain traceChain(__func__);
    RSSEXE_LOGD("ResSchedExeService start to exec SendRequestSync, restType: %{public}u", resType);
    if (!IsCallingClientRss()) {
        RSSEXE_LOGE("calling process has no permission!");
        return ERR_INVALID_OPERATION;
    }
    if (!IsTypeVaild(resType)) {
        RSSEXE_LOGE("The resType is invalid. Dos");
        return ERR_INVALID_VALUE;
    }

    nlohmann::json jsonContext = context.jsonContext;
    if (jsonContext.size() > PAYLOAD_MAX_SIZE) {
        RSSEXE_LOGE("The payload is too long. DoS.");
        return ERR_INVALID_VALUE;
    }
    if (!GetExtResType(resType, jsonContext)) {
        RSSEXE_LOGE("Get ResType Error DoS.");
        return ERR_INVALID_VALUE;
    }

    nlohmann::json jsonResponse;
    funcResult = ResSchedExeMgr::GetInstance().SendRequestSync(resType, value, jsonContext, jsonResponse);
    response.jsonContext = jsonResponse;
    return ERR_OK;
}

ErrCode ResSchedExeService::SendRequestAsync(uint32_t resType, int64_t value, const ResJsonType& context)
{
    ExecutorHiTraceChain traceChain(__func__);
    RSSEXE_LOGD("ResSchedExeService start to exec SendRequestAsync, restType: %{public}u", resType);
    if (!IsCallingClientRss()) {
        RSSEXE_LOGE("calling process has no permission!");
        return ERR_INVALID_OPERATION;
    }
    if (!IsTypeVaild(resType)) {
        RSSEXE_LOGE("The resType is invalid. Dos");
        return ERR_INVALID_VALUE;
    }

    nlohmann::json jsonContext = context.jsonContext;
    if (jsonContext.size() > PAYLOAD_MAX_SIZE) {
        RSSEXE_LOGE("The payload is too long. DoS.");
        return ERR_INVALID_VALUE;
    }
    if (!GetExtResType(resType, jsonContext)) {
        RSSEXE_LOGE("Get ResType Error DoS.");
        return ERR_INVALID_VALUE;
    }

    ResSchedExeMgr::GetInstance().SendRequestAsync(resType, value, jsonContext);
    return ERR_OK;
}

ErrCode ResSchedExeService::KillProcess(uint32_t pid, int32_t& funcResult)
{
    ExecutorHiTraceChain traceChain(__func__);
    RSSEXE_LOGD("ResSchedExeService start to exec KillProcess, pid: %{public}u", pid);
    if (!IsCallingClientRss()) {
        RSSEXE_LOGE("Calling process has no permission!");
        return ERR_INVALID_OPERATION;
    }
    if (pid <= 0) {
        RSSEXE_LOGE("Pid should be more than 0.");
        return ERR_INVALID_VALUE;
    }

    funcResult = ResSchedExeMgr::GetInstance().KillProcess(static_cast<pid_t>(pid));
    return ERR_OK;
}

bool ResSchedExeService::AllowDump()
{
    if (ENG_MODE == 0) {
        RSSEXE_LOGE("Not eng mode");
        return false;
    }
    Security::AccessToken::AccessTokenID tokenId = IPCSkeleton::GetCallingTokenID();
    int32_t ret = Security::AccessToken::AccessTokenKit::VerifyAccessToken(tokenId, DUMP_PERMISSION);
    if (ret != Security::AccessToken::PermissionState::PERMISSION_GRANTED) {
        RSSEXE_LOGE("CheckPermission failed");
        return false;
    }
    return true;
}

int32_t ResSchedExeService::Dump(int32_t fd, const std::vector<std::u16string>& args)
{
    if (!AllowDump()) {
        return ResErrCode::RSSEXE_PERMISSION_DENIED;
    }
    RSSEXE_LOGI("Dump service.");
    std::vector<std::string> argsInStr;
    std::transform(args.begin(), args.end(), std::back_inserter(argsInStr),
        [](const std::u16string &arg) {
        std::string ret = Str16ToStr8(arg);
        RSSEXE_LOGI("arg: %{public}s.", ret.c_str());
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
        RSSEXE_LOGE("save to fd failed.");
    }
    return ResErrCode::RSSEXE_NO_ERR;
}

void ResSchedExeService::DumpUsage(std::string &result)
{
    result.append("usage: resource schedule executor dump [<options>]\n")
        .append("    -h: show the help.\n")
        .append("    -a: show all info.\n")
        .append("    -p: show the all plugin info.\n")
        .append("    -p (plugin name): show one plugin info.\n");
    PluginMgr::GetInstance().DumpHelpFromPlugin(result);
}

void ResSchedExeService::DumpAllInfo(std::string &result)
{
    result.append("================Resource Schedule Executor Infos================\n");
    PluginMgr::GetInstance().DumpAllPlugin(result);
}
} // namespace ResourceSchedule
} // namespace OHOS
