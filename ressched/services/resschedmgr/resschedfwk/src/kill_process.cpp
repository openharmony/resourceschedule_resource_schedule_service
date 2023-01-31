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
#include "res_sched_log.h"

namespace OHOS {
namespace ResourceSchedule {
namespace {
constexpr int32_t SIGNAL_KILL = 9;
}

int32_t KillProcess::KillProcessByPid(const pid_t pid) const
{
    int32_t ret = -1;
    if (pid > 0) {
        ret = kill(pid, SIGNAL_KILL);
        RESSCHED_LOGI("kill pid %{public}d", pid);
    }
    return ret;
}
} // namespace ResourceSchedule
} // namespace OHOS