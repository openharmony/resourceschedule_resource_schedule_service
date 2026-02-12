/*
 * Copyright (c) 2023-2025 Huawei Device Co., Ltd.
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
#include <sys/types.h>
#include "ability_manager_client.h"
#include "exit_reason.h"
#include "res_sched_errors.h"
#include "res_sched_exe_client.h"
#include "res_sched_kill_reason.h"
#include "res_sched_log.h"
#include "string_ex.h"
#include "res_common_util.h"

namespace OHOS {
namespace ResourceSchedule {
using namespace std;
namespace {
    static const char* UNKNOWN_PROCESS = "unknown_process";
}

int32_t KillProcess::KillProcessByPidWithClient(const nlohmann::json& payload)
{
    if ((payload == nullptr) || !(payload.contains("pid") && payload["pid"].is_string())) {
        return RES_SCHED_KILL_PROCESS_FAIL;
    }
    int32_t pid = -1;
    if (!ResCommonUtil::StrToInt32(payload["pid"].get<string>(), pid)) {
        return RES_SCHED_KILL_PROCESS_FAIL;
    }
    std::string processName = payload.contains("processName") && payload["processName"].is_string() ?
                                  payload["processName"].get<string>() : UNKNOWN_PROCESS;
    if (pid <= 0) {
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
    int32_t killRes = ResSchedExeClient::GetInstance().KillProcess(pid);
    if (killRes < 0) {
        RESSCHED_LOGE("kill process %{public}d:%{public}s failed", pid, processName.c_str());
    } else {
        RESSCHED_LOGI("kill process %{public}d:%{public}s success", pid, processName.c_str());
    }
    return killRes;
}
} // namespace ResourceSchedule
} // namespace OHOS