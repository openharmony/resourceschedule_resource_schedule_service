/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef RESSCHED_COMMON_INCLUDE_RES_SCHED_SYSTEM_UTIL
#define RESSCHED_COMMON_INCLUDE_RES_SCHED_SYSTEM_UTIL

#include <string>

namespace OHOS {
namespace ResourceSchedule {
namespace ResCommonUtil {

/**
 * @brief Obtain total ram size.
 *
 * @param ddrSysProp if ddrSysProp not empty, try parse ram size from this system prop
 * @return Returns total ram size;
 */
int64_t GetTotalRamSize(const std::string& ddrSysProp = "");

/**
 * @brief Obtain total rom size.
 *
 * @return Returns total rom size;
 */
int64_t GetTotalRomSize();

/**
 * @brief Obtain system prop.
 *
 * @param name system prop name
 * @param defaultValue when obtain failed return this value
 * @return Returns the value of this system prop;
 */
std::string GetSystemProperties(const std::string& name, const std::string &defaultValue);
}
} // namespace ResourceSchedule
} // namespace OHOS

#endif // RESSCHED_COMMON_INCLUDE_RES_SCHED_SYSTEM_UTIL