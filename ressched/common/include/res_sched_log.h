/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: wrapper hilog interface for resource scheduler.
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