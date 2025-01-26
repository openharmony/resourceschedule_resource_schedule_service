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

#ifndef RESSCHED_COMMON_INCLUDE_RES_SCHED_STRING_UTIL
#define RESSCHED_COMMON_INCLUDE_RES_SCHED_STRING_UTIL
#include <string>

#include "nlohmann/json.hpp"

namespace OHOS {
namespace ResourceSchedule {
namespace ResCommonUtil {

/**
 * @brief Converts a string to an float value.
 *
 * @param value Indicates the string to be converted
 * @param result Indicates the `float` variable to store the result
 * @return Returns 'true' is the operation is successful;
 * return `false` otherwise
 */
bool StrToFloat(const std::string& value, float& result);

/**
 * @brief Check a bundle name is vaild
 *
 * @param bundleName Indicates the bundle name to be check
 * @return Returns 'true' is the bundle name valid;
 * return `false` otherwise
 */
bool CheckBundleName(const std::string &bundleName);

/**
 * @brief Converts a string to an int32_t value.
 *
 * @param value Indicates the string to be converted
 * @param result Indicates the `int32_t` variable to store the result
 * @return Returns 'true' is the operation is successful;
 * return `false` otherwise
 */
bool StrToInt32(const std::string& value, int32_t& result);

/**
 * @brief Converts a string to an int64_t value.
 *
 * @param value Indicates the string to be converted
 * @param result Indicates the `int64_t` variable to store the result
 * @return Returns 'true' is the operation is successful;
 * return `false` otherwise
 */
bool StrToInt64(const std::string& value, int64_t& result);

/**
 * @brief Converts a string to an uint32_t value.
 *
 * @param value Indicates the string to be converted
 * @param result Indicates the `uint32_t` variable to store the result
 * @return Returns 'true' is the operation is successful;
 * return `false` otherwise
 */
bool StrToUInt32(const std::string& value, uint32_t& result);

/**
 * @brief Judge string is number format
 *
 * @param value Indicates the string to be judged.
 * @return Returns 'true' is the string is number format;
 * return `false` otherwise
 */
bool IsNumericString(const std::string& value);

/**
 * @brief Trim string(include '\f' '\v' '\r' '\t' '\n' ' ')
 *
 * @param value Indicates the string to be trimed.
 * @return Returns trimed string;
 */
std::string StringTrim(const std::string& str);

/**
 * @brief Trim string(only ' ')
 *
 * @param value Indicates the string to be trimed.
 * @return Returns trimed string;
 */
std::string StringTrimSpace(const std::string& value);

/**
 * @brief Convert a string to json object
 *
 * @param str Indicates the string to be converted.
 * @return Returns 'true' if convert success;
 */
bool StringToJson(const std::string& str, nlohmann::json& payload);
}
} // namespace ResourceSchedule
} // namespace OHOS

#endif // RESSCHED_COMMON_INCLUDE_RES_SCHED_STRING_UTIL