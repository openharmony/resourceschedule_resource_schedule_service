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

#ifndef CGROUP_SCHED_FRAMEWORK_PROCESS_GROUP_INCLUDE_PROCESS_GROUP_LOG_H_
#define CGROUP_SCHED_FRAMEWORK_PROCESS_GROUP_INCLUDE_PROCESS_GROUP_LOG_H_

#include "hilog/log_c.h"    // for LogLevel, LOG_CORE, LOG_LEVEL_MAX, LOG_LE...
#include "hilog/log_cpp.h"  // for HiLogLabel

#undef LOG_DOMAIN
#define LOG_DOMAIN 0xD001702

#undef LOG_TAG
#define LOG_TAG "pg-cgs"

#define PGCGS_LOGD(...) HILOG_DEBUG(LOG_CORE, __VA_ARGS__)
#define PGCGS_LOGI(...) HILOG_INFO(LOG_CORE, __VA_ARGS__)
#define PGCGS_LOGW(...) HILOG_WARN(LOG_CORE, __VA_ARGS__)
#define PGCGS_LOGE(...) HILOG_ERROR(LOG_CORE, __VA_ARGS__)
#define PGCGS_LOGF(...) HILOG_FATAL(LOG_CORE, __VA_ARGS__)

namespace OHOS {
namespace ResourceSchedule {
namespace CgroupSetting {
class ProcessGroupLog {
public:
    ProcessGroupLog() = delete;
    ~ProcessGroupLog() = delete;

    /**
     * @brief Judge level.
     *
     * @param level The level.
     * @return True if success,else false.
     */
    static bool JudgeLevel(const LogLevel &level);

    /**
     * @brief Get log level.
     *
     * @return Level.
     */
    static const LogLevel &GetLogLevel()
    {
        return level_;
    }

private:
    static LogLevel level_;
};
} // namespace CgroupSetting
} // namespace ResourceSchedule
} // namespace OHOS
#endif // CGROUP_SCHED_FRAMEWORK_PROCESS_GROUP_INCLUDE_PROCESS_GROUP_LOG_H_
