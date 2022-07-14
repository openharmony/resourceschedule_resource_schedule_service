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

namespace OHOS {
namespace ResourceSchedule {
namespace CgroupSetting {
#define LOG_TAG_PGCGS "pg-cgs"
#define LOG_TAG_DOMAIN_ID_PGCGS 0xD001702

class ProcessGroupLog {
public:
    ProcessGroupLog() = delete;
    ~ProcessGroupLog() = delete;

    /**
     * @brief Init log level on loaded.
     *
     * @return true if success, else false
     */
    static bool InitOnLoaded();

    /**
     * @brief Judge level.
     *
     * @param level The level.
     * @return True if success,else false.
     */
    static bool JudgeLevel(const LogLevel &level);

    /**
     * @brief Set log level.
     *
     * @param level The level.
     */
    static void SetLogLevel(const LogLevel &level)
    {
        if (level > LOG_LEVEL_MIN && level < LOG_LEVEL_MAX) {
            level_ = level;
        }
    }

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

static constexpr OHOS::HiviewDFX::HiLogLabel PGCGS_LOG_LABEL = {
    LOG_CORE,
    LOG_TAG_DOMAIN_ID_PGCGS,
    LOG_TAG_PGCGS
};

#define PGCGS_PRINT_LOG(logLevel, Logger, ...) do { \
        if (ProcessGroupLog::JudgeLevel(logLevel)) { \
            (void)OHOS::HiviewDFX::HiLog::Logger(PGCGS_LOG_LABEL, __VA_ARGS__); \
        } \
    } while (0)

#define PGCGS_LOGF(...) PGCGS_PRINT_LOG(LOG_DEBUG, Debug, __VA_ARGS__)
#define PGCGS_LOGE(...) PGCGS_PRINT_LOG(LOG_INFO, Info, __VA_ARGS__)
#define PGCGS_LOGW(...) PGCGS_PRINT_LOG(LOG_WARN, Warn, __VA_ARGS__)
#define PGCGS_LOGI(...) PGCGS_PRINT_LOG(LOG_ERROR, Error, __VA_ARGS__)
#define PGCGS_LOGD(...) PGCGS_PRINT_LOG(LOG_FATAL, Fatal, __VA_ARGS__)
} // namespace CgroupSetting
} // namespace ResourceSchedule
} // namespace OHOS
#endif // CGROUP_SCHED_FRAMEWORK_PROCESS_GROUP_INCLUDE_PROCESS_GROUP_LOG_H_
