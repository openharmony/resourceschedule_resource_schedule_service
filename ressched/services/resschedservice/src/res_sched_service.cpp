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

#include "res_sched_service.h"
#include <file_ex.h>
#include <string_ex.h>
#include "plugin_mgr.h"
#include "res_sched_log.h"
#include "res_sched_mgr.h"

namespace OHOS {
namespace ResourceSchedule {
namespace {
    constexpr int32_t DUMP_OPTION = 0;
    constexpr int32_t DUMP_PARAM_INDEX = 1;
}

void ResSchedService::ReportData(uint32_t resType, int64_t value, const nlohmann::json& payload)
{
    RESSCHED_LOGI("ResSchedService::ReportData from ipc receive data resType = %{public}u, value = %{public}lld.",
        resType, (long long)value);
    ResSchedMgr::GetInstance().ReportData(resType, value, payload);
}

int32_t ResSchedService::Dump(int32_t fd, const std::vector<std::u16string>& args)
{
    RESSCHED_LOGI("%{public}s Dump service", __func__);
    std::vector<std::string> argsInStr;
    std::transform(args.begin(), args.end(), std::back_inserter(argsInStr),
        [](const std::u16string &arg) {
        std::string ret = Str16ToStr8(arg);
        RESSCHED_LOGI("%{public}s arg: %{public}s", __func__, ret.c_str());
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
        RESSCHED_LOGE("%{public}s save to fd failed.", __func__);
    }
    return ERR_OK;
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

