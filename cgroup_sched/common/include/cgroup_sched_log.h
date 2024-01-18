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

#ifndef CGROUP_SCHED_COMMON_INCLUDE_CGROUP_SCHED_LOG_H_
#define CGROUP_SCHED_COMMON_INCLUDE_CGROUP_SCHED_LOG_H_

#include <string>
#include <vector>
#include "hilog/log.h"

#undef LOG_DOMAIN
#define LOG_DOMAIN 0xD001702

#undef LOG_TAG
#define LOG_TAG "CGS"

#define CGS_LOGD(...) HILOG_DEBUG(LOG_CORE, __VA_ARGS__)
#define CGS_LOGI(...) HILOG_INFO(LOG_CORE, __VA_ARGS__)
#define CGS_LOGW(...) HILOG_WARN(LOG_CORE, __VA_ARGS__)
#define CGS_LOGE(...) HILOG_ERROR(LOG_CORE, __VA_ARGS__)
#define CGS_LOGF(...) HILOG_FATAL(LOG_CORE, __VA_ARGS__)

namespace OHOS {
namespace ResourceSchedule {
class CgroupSchedLog {
public:
    CgroupSchedLog() = delete;
    ~CgroupSchedLog() = delete;
    
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
} // namespace ResourceSchedule
} // namespace OHOS

#endif // CGROUP_SCHED_COMMON_INCLUDE_CGROUP_SCHED_LOG_H_
