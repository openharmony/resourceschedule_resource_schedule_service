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

#ifndef FOUNDATION_RESOURCESCHEDULE_COMMON_INCLUDE_RES_SCHED_LOG_H
#define FOUNDATION_RESOURCESCHEDULE_COMMON_INCLUDE_RES_SCHED_LOG_H

#include "hilog/log.h"

namespace OHOS {
namespace ResourceSchedule {
static constexpr OHOS::HiviewDFX::HiLogLabel RES_SCHED_LABEL = {
    LOG_CORE,
    0xD001700,
    "RSS"
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

#define RESSCHED_LOGF(...) (void)OHOS::HiviewDFX::HiLog::Fatal(RES_SCHED_LABEL, __VA_ARGS__)
#define RESSCHED_LOGE(...) (void)OHOS::HiviewDFX::HiLog::Error(RES_SCHED_LABEL, __VA_ARGS__)
#define RESSCHED_LOGW(...) (void)OHOS::HiviewDFX::HiLog::Warn(RES_SCHED_LABEL, __VA_ARGS__)
#define RESSCHED_LOGI(...) (void)OHOS::HiviewDFX::HiLog::Info(RES_SCHED_LABEL, __VA_ARGS__)
#define RESSCHED_LOGD(...) (void)OHOS::HiviewDFX::HiLog::Debug(RES_SCHED_LABEL, __VA_ARGS__)
} // namespace ResourceSchedule
} // namespace OHOS

#endif // FOUNDATION_RESOURCESCHEDULE_COMMON_INCLUDE_RES_SCHED_LOG_H