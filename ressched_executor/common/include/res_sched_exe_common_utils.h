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

#ifndef RESSCHED_EXECUTOR_COMMON_INCLUDE_RES_SCHED_EXE_COMMON_UTILS_H
#define RESSCHED_EXECUTOR_COMMON_INCLUDE_RES_SCHED_EXE_COMMON_UTILS_H

#include <string>

#include "nocopyable.h"
#include "nlohmann/json.hpp"

namespace OHOS {
namespace ResourceSchedule {
class ResSchedExeCommonUtils {
public:
    ResSchedExeCommonUtils() = delete;
    ~ResSchedExeCommonUtils() = delete;
    DISALLOW_COPY_AND_MOVE(ResSchedExeCommonUtils);

    /**
     * @brief get current timestamp
     *
     * @return current timestamp, unit ms
     */
    static uint64_t GetCurrentTimestampMs();

    /**
     * @brief get current timestamp
     *
     * @return current timestamp, unit us
     */
    static uint64_t GetCurrentTimestampUs();

    /**
     * @brief check device is eng mode
     *
     * @return device is eng mode or not
     */
    static bool IsDebugMode();

    /**
     * @brief check process has permission
     *
     * @param permission permission name to check
     * @return process has permission or not
     */
    static bool CheckPermission(const std::string& permission);

    /**
     * @brief convert string to json
     *
     * @param str message string
     * @param payload message will be convert to json
     */
    static void StringToJson(const std::string& str, nlohmann::json& payload);
};
} // namespace ResourceSchedule
} // namespace OHOS

#endif // RESSCHED_EXECUTOR_COMMON_INCLUDE_RES_SCHED_EXE_COMMON_UTILS_H
