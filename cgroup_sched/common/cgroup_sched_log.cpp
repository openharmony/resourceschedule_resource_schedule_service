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

#include "cgroup_sched_log.h"

namespace OHOS {
namespace ResourceSchedule {
LogLevel CgroupSchedLog::level_ = { LOG_DEBUG };
const bool RESULT = CgroupSchedLog::InitOnLoaded();

bool CgroupSchedLog::JudgeLevel(const LogLevel &level)
{
    const LogLevel &curLevel = CgroupSchedLog::GetLogLevel();
    if (level < curLevel) {
        return false;
    }
    return true;
}

bool CgroupSchedLog::InitOnLoaded()
{
    const std::vector<LogLevel> levels = { LOG_DEBUG, LOG_INFO, LOG_WARN, LOG_ERROR, LOG_FATAL };
    for (const LogLevel& level : levels) {
        if (HiLogIsLoggable(LOG_TAG_DOMAIN_ID_RMS, LOG_TAG_CGROUP_SCHED, level)) {
            CgroupSchedLog::SetLogLevel(level);
            break;
        }
    }
    return true;
}
} // namespace ResourceSchedule
} // namespace OHOS