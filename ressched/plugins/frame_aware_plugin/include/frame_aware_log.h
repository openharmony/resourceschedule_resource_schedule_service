/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef RME_LOG_DOMAIN_H
#define RME_LOG_DOMAIN_H

#undef LOG_DOMAIN
#define LOG_DOMAIN 0xD001706

#include <cstdint>
#include "hilog/log.h"

namespace OHOS {
namespace RmeLogDomain {

/*
#ifdef RME_LOGF
#undef RME_LOGF
#endif

#ifdef RME_LOGE
#undef RME_LOGE
#endif

#ifdef RME_LOGW
#undef RME_LOGW
#endif

#ifdef RME_LOGI
#undef RME_LOGI
#endif

#ifdef RME_LOGD
#undef RME_LOGD
#endif
*/

#define RME_LOGF(...) HILOG_FATAL(LOG_CORE, ##__VA_ARGS__)
#define RME_LOGE(...) HILOG_ERROR(LOG_CORE, ##__VA_ARGS__)
#define RME_LOGW(...) HILOG_WARN(LOG_CORE, ##__VA_ARGS__)
#define RME_LOGI(...) HILOG_INFO(LOG_CORE, ##__VA_ARGS__)
#define RME_LOGD(...) HILOG_DEBUG(LOG_CORE, ##__VA_ARGS__)
} // namespace RmeLogDomain
} // namespace OHOS
#endif // RME_LOG_DOMAIN_H
