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
#include <sys/types.h>
#include "res_sched_errors.h"
#include "res_sched_log.h"

namespace OHOS {
namespace ResourceSchedule {
using namespace std;
namespace {
    constexpr int32_t SIGNAL_KILL = 9;
}

int32_t KillProcess::KillProcessByPidWithClient(const nlohmann::json& payload, std::string killClientInitiator)
{
    if ((payload == nullptr) || !(payload.contains("pid") && payload["pid"].is_string())) {
        return RES_SCHED_KILL_PROCESS_FAIL;
    }

    pid_t pid = static_cast<uint32_t>(atoi(payload["pid"].get<string>().c_str()));
    if (pid == 0) {
        return RES_SCHED_KILL_PROCESS_FAIL;
    }
    auto it = find(ALLOWED_CLIENT.begin(), ALLOWED_CLIENT.end(), killClientInitiator);
    if (it == ALLOWED_CLIENT.end()) {
        RESSCHED_LOGE("kill process fail, %{public}s no permission.", killClientInitiator.c_str());
        return RES_SCHED_KILL_PROCESS_FAIL;
    }

    int32_t killRes = KillProcessByPid(pid);
    if (killRes < 0) {
        RESSCHED_LOGE("kill process %{public}d failed.", pid);
    } else {
        string processName = "unknown process";
        if (payload.contains("processName") && payload["processName"].is_string()) {
            processName = payload["processName"].get<string>();
        }
        RESSCHED_LOGI("kill process, killer is %{public}s, %{public}s to be killed, pid is %{public}d.",
            killClientInitiator.c_str(), processName.c_str(), pid);
    }
    return killRes;
}

int32_t KillProcess::KillProcessByPid(const pid_t pid) const
{
    int32_t ret = -1;
    if (pid > 0) {
        ret = kill(pid, SIGNAL_KILL);
        RESSCHED_LOGI("kill pid %{public}d.", pid);
    }
    return ret;
}
} // namespace ResourceSchedule
} // namespace OHOS