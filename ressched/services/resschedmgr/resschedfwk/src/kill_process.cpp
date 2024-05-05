/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
#include "kill_process.h"

#include <cstdint>
#include <csignal>
#include <fstream>
#include <sys/types.h>
#include "ability_manager_client.h"
#include "exit_reason.h"
#include "res_sched_errors.h"
#include "res_sched_exe_client.h"
#include "res_sched_kill_reason.h"
#include "res_sched_log.h"
#include "string_ex.h"

namespace OHOS {
namespace ResourceSchedule {
using namespace std;
namespace {
    constexpr int32_t MAX_INDEX = 22;
    constexpr int32_t START_TIME_INDEX = 19;
    const std::string UNKNOWN_PROCESS = "unknown_process";
}

int32_t KillProcess::KillProcessByPidWithClient(const nlohmann::json& payload)
{
    if ((payload == nullptr) || !(payload.contains("pid") && payload["pid"].is_string())) {
        return RES_SCHED_KILL_PROCESS_FAIL;
    }

    pid_t pid = static_cast<int32_t>(atoi(payload["pid"].get<string>().c_str()));
    std::string processName = payload.contains("processName") && payload["processName"].is_string() ?
                                  payload["processName"].get<string>() : UNKNOWN_PROCESS;
    time_t processStartTime = GetProcessStartTime(pid);
    if (pid < 0 || processStartTime <=0) {
        RESSCHED_LOGE("process %{public}d:%{public}s is invalid", pid, processName.c_str());
        return RES_SCHED_KILL_PROCESS_FAIL;
    }

    if (payload.contains("killReason") && payload["killReason"].is_string()) {
        std::string killReason = payload["killReason"].get<string>();
        AAFwk::ExitReason reason = {AAFwk::REASON_PERFORMANCE_CONTROL, killReason};
        if (AAFwk::AbilityManagerClient::GetInstance()->RecordProcessExitReason(pid, reason) == 0) {
            RESSCHED_LOGI("process %{public}d exitReason:%{public}s record success", pid, killReason.c_str());
        } else {
            RESSCHED_LOGE("process %{public}d exitReason:%{public}s record failed", pid, killReason.c_str());
        }
    }

    if (processStartTime != GetProcessStartTime(pid)) {
        RESSCHED_LOGE("process %{public}d:%{public}s has killed in advance", pid, processName.c_str());
        return RES_SCHED_KILL_PROCESS_FAIL;
    }

    int32_t killRes = ResSchedExeClient::GetInstance().KillProcess(pid);
    if (killRes < 0) {
        RESSCHED_LOGE("kill process %{public}d:%{public}s failed", pid, processName.c_str());
    } else {
        RESSCHED_LOGI("kill process %{public}d:%{public}s success", pid, processName.c_str());
    }
    return killRes;
}

time_t KillProcess::GetProcessStartTime(int pid)
{
    std::string path = "/proc/" + std::to_string(pid) + "/stat";
    std::vector<std::string> statInfo = GetStatInfo(path);
    if (statInfo.size() < MAX_INDEX) {
        return -1;
    }
    std::string startTime = statInfo[START_TIME_INDEX];
    if (!isdigit(startTime[0])) {
        RESSCHED_LOGE("failed to get process start time, reason: not digital, pid: %d\n", pid);
        return -1;
    }
    return stol(startTime);
}

std::vector<std::string> KillProcess::GetStatInfo(const std::string &path)
{
    std::string statInfo = ReadFileByChar(path);
    // process_name was included in pair (), find ")" as part for skip speical in process_name.
    size_t pos = statInfo.find(')');
    if (pos != std::string::npos) {
        statInfo = statInfo.substr(++pos);
    }
    vector<string> stats;
    SplitStr(statInfo, " ", stats);
    return stats;
}

std::string KillProcess::ReadFileByChar(const std::string &path)
{
    std::ifstream fin(path);
    if (!fin.is_open()) {
        return UNKNOWN_PROCESS;
    }
    std::string content;
    char c;
    while (!fin.eof()) {
        fin >> std::noskipws >> c;
        if (c == '\0' || c == '\n') {
            break;
        }
        content += c;
    }
    return content;
}
} // namespace ResourceSchedule
} // namespace OHOS