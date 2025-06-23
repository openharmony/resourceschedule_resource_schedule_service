/*
 * Copyright (c) 2025-2025 Huawei Device Co., Ltd.
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

#include "cgroup_log.h"

#include "cgroup_sched_log.h"

namespace OHOS {
namespace ResourceSchedule {

void CgsLogD(const std::string& str)
{
    CGS_LOGD("%{public}s", str.c_str());
}

void CgsLogI(const std::string& str)
{
    CGS_LOGI("%{public}s", str.c_str());
}

void CgsLogW(const std::string& str)
{
    CGS_LOGW("%{public}s", str.c_str());
}

void CgsLogE(const std::string& str)
{
    CGS_LOGE("%{public}s", str.c_str());
}

void CgsLogF(const std::string& str)
{
    CGS_LOGF("%{public}s", str.c_str());
}

} // namespace ResourceSchedule
} // namespace OHOS
