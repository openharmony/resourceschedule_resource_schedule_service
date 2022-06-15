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

#ifndef RESSCHED_COMMON_INCLUDE_RES_SCHED_LOG_H
#define RESSCHED_COMMON_INCLUDE_RES_SCHED_LOG_H

#include "hilog/log.h"

namespace OHOS {
namespace ResourceSchedule {
#define LOG_RESSCHED "RSS"
#define LOG_DOMAIN_ID_RESSCHED 0xD001700
enum class ResschedLogLevel : uint8_t { DEBUG = 0, INFO, WARN, ERROR, FATAL};
static constexpr OHOS::HiviewDFX::HiLogLabel RES_SCHED_LABEL = {
    LOG_CORE,
    LOG_DOMAIN_ID_RESSCHED,
    LOG_RESSCHED
};
class ResschedLog {
public:
    static ResschedLog& GetInstance()
    {
        static auto instance = new ResschedLog();
        return *instance;
    }
    bool& getLogEnableByLevel(const ResschedLogLevel &level)
    {
        if (level == ResschedLogLevel::DEBUG) {
            return debugLogEnabled_;
        } else {
            return otherLogEnable_;
        }
    }
    bool debugLogEnabled_ = false;
    bool otherLogEnable_ = true;
private:
    ResschedLog()
    {
        debugLogEnabled_ = HiLogIsLoggable(LOG_DOMAIN_ID_RESSCHED, LOG_RESSCHED, LOG_DEBUG);
    }
};
#ifdef RESSCHED_LOGF
#undef RESSCHED_LOGF
#endif

#ifdef RESSCHED_LOGE
#undef RESSCHED_LOGE
#endif

#ifdef RESSCHED_LOGW
#undef RESSCHED_LOGW
#endif

#ifdef RESSCHED_LOGI
#undef RESSCHED_LOGI
#endif

#ifdef RESSCHED_LOGD
#undef RESSCHED_LOGD
#endif

#ifdef RESSCHED_LOG
#undef RESSCHED_LOG
#endif

#define RESSCHED_LOG(RESSCHEDLOGLEVEL, level, ...)                                                  \
    if (ResschedLog::GetInstance().getLogEnableByLevel(RESSCHEDLOGLEVEL))                           \
        OHOS::HiviewDFX::HiLog::level(RES_SCHED_LABEL, ##__VA_ARGS__)                               \

#define RESSCHED_LOGF(...) RESSCHED_LOG(ResschedLogLevel::FATAL, Fatal, ##__VA_ARGS__)
#define RESSCHED_LOGE(...) RESSCHED_LOG(ResschedLogLevel::ERROR, Error, ##__VA_ARGS__)
#define RESSCHED_LOGW(...) RESSCHED_LOG(ResschedLogLevel::WARN, Warn, ##__VA_ARGS__)
#define RESSCHED_LOGI(...) RESSCHED_LOG(ResschedLogLevel::INFO, Info, ##__VA_ARGS__)
#define RESSCHED_LOGD(...) RESSCHED_LOG(ResschedLogLevel::DEBUG, Debug, ##__VA_ARGS__)
} // namespace ResourceSchedule
} // namespace OHOS

#endif // RESSCHED_COMMON_INCLUDE_RES_SCHED_LOG_H
