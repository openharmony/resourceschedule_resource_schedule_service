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
#undef LOG_DOMAIN
#define LOG_DOMAIN 0xD001703

#undef LOG_TAG
#define LOG_TAG "socperf"

#define SOC_PERF_LOGD(fmt, ...) HILOG_DEBUG(LOG_CORE, "[%{public}s]: " fmt, __FUNCTION__, ##__VA_ARGS__)
#define SOC_PERF_LOGI(fmt, ...) HILOG_INFO(LOG_CORE, "[%{public}s]: " fmt, __FUNCTION__, ##__VA_ARGS__)
#define SOC_PERF_LOGW(fmt, ...) HILOG_WARN(LOG_CORE, "[%{public}s]: " fmt, __FUNCTION__, ##__VA_ARGS__)
#define SOC_PERF_LOGE(fmt, ...) HILOG_ERROR(LOG_CORE, "[%{public}s]: " fmt, __FUNCTION__, ##__VA_ARGS__)
#define SOC_PERF_LOGF(fmt, ...) HILOG_FATAL(LOG_CORE, "[%{public}s]: " fmt, __FUNCTION__, ##__VA_ARGS__)
} // namespace SOCPERF
} // namespace OHOS

#endif // SOC_PERF_COMMON_INCLUDE_SOCPERF_LOG_H
