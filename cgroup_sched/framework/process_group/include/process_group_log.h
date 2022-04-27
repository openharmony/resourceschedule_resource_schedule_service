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

#include <string>
#include <vector>
#include "hilog/log.h"

namespace OHOS {
namespace ResourceSchedule {
namespace CgroupSetting {
#define LOG_TAG_PGCGS "pg-cgs"
#define LOG_TAG_DOMAIN_ID_PGCGS 0xD001702

static constexpr OHOS::HiviewDFX::HiLogLabel PGCGS_LOG_LABEL = {
    LOG_CORE,
    LOG_TAG_DOMAIN_ID_PGCGS,
    LOG_TAG_PGCGS
};

#define PGCGS_LOGF(...) (void)OHOS::HiviewDFX::HiLog::Fatal(PGCGS_LOG_LABEL, __VA_ARGS__)
#define PGCGS_LOGE(...) (void)OHOS::HiviewDFX::HiLog::Error(PGCGS_LOG_LABEL, __VA_ARGS__)
#define PGCGS_LOGW(...) (void)OHOS::HiviewDFX::HiLog::Warn(PGCGS_LOG_LABEL, __VA_ARGS__)
#define PGCGS_LOGI(...) (void)OHOS::HiviewDFX::HiLog::Info(PGCGS_LOG_LABEL, __VA_ARGS__)
#define PGCGS_LOGD(...) (void)OHOS::HiviewDFX::HiLog::Debug(PGCGS_LOG_LABEL, __VA_ARGS__)
} // namespace CgroupSetting
} // namespace ResourceSchedule
} // namespace OHOS
#endif // CGROUP_SCHED_FRAMEWORK_PROCESS_GROUP_INCLUDE_PROCESS_GROUP_LOG_H_
