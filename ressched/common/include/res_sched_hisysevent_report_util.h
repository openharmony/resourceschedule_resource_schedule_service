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

#ifndef RESSCHED_COMMON_INCLUDE_RES_SCHED_HISYSEVENT_REPORT_UTIL
#define RESSCHED_COMMON_INCLUDE_RES_SCHED_HISYSEVENT_REPORT_UTIL
#include <string>

namespace OHOS {
namespace ResourceSchedule {
namespace ResCommonUtil {

/**
 * @brief report res sched abnormal err event.
 *
 * @param moduleName the moduleName of the abnormalErr
 * @param funcName the funcName of the abnormalErr
 * @param errInfo the errInfo of the abnormalErr
 *
 */
void HiSysAbnormalErrReport(const std::string& moduleName, const std::string& funcName, const std::string errInfo);
}
} // namespace ResourceSchedule
} // namespace OHOS

#endif // RESSCHED_COMMON_INCLUDE_RES_SCHED_HISYSEVENT_REPORT_UTIL