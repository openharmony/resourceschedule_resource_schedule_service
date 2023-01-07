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

#ifndef SOC_PERF_COMMON_INCLUDE_SOCPERF_LOG_H
#define SOC_PERF_COMMON_INCLUDE_SOCPERF_LOG_H

#include "hilog/log.h"

namespace OHOS {
namespace SOCPERF {
#ifndef LOG_TAG_SOC_PERF
#define LOG_TAG_SOC_PERF "socperf"
#endif

#ifndef LOG_TAG_DOMAIN_ID_SOC_PERF
#define LOG_TAG_DOMAIN_ID_SOC_PERF 0xD001703
#endif

static constexpr OHOS::HiviewDFX::HiLogLabel SOC_PERF_LOG_LABEL = {
    LOG_CORE,
    LOG_TAG_DOMAIN_ID_SOC_PERF,
    LOG_TAG_SOC_PERF
};

class SocPerfLog {
public:
    SocPerfLog() = delete;
    ~SocPerfLog() = delete;

    static bool IsDebugLogEnabled()
    {
        return isDebugLogEnabled_;
    }

    static void EnableDebugLog()
    {
        isDebugLogEnabled_ = true;
    }

    static void DisableDebugLog()
    {
        isDebugLogEnabled_ = false;
    }

private:
    static bool isDebugLogEnabled_;
};

#define SOC_PERF_PRINT_LOG(Level, fmt, ...)                     \
    OHOS::HiviewDFX::HiLog::Level(SOC_PERF_LOG_LABEL, "[%{public}s]: " fmt, __FUNCTION__, ##__VA_ARGS__)

#define SOC_PERF_LOGD(fmt, ...)                                 \
    if (SocPerfLog::IsDebugLogEnabled())                        \
    SOC_PERF_PRINT_LOG(Debug, fmt, ##__VA_ARGS__)

#define SOC_PERF_LOGI(fmt, ...) SOC_PERF_PRINT_LOG(Info, fmt, ##__VA_ARGS__)
#define SOC_PERF_LOGW(fmt, ...) SOC_PERF_PRINT_LOG(Warn, fmt, ##__VA_ARGS__)
#define SOC_PERF_LOGE(fmt, ...) SOC_PERF_PRINT_LOG(Error, fmt, ##__VA_ARGS__)
#define SOC_PERF_LOGF(fmt, ...) SOC_PERF_PRINT_LOG(Fatal, fmt, ##__VA_ARGS__)
} // namespace SOCPERF
} // namespace OHOS

#endif // SOC_PERF_COMMON_INCLUDE_SOCPERF_LOG_H
