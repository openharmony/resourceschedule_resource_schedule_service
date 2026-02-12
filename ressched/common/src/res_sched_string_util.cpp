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

#include "res_sched_string_util.h"

#include "res_sched_log.h"

namespace OHOS {
namespace ResourceSchedule {
namespace ResCommonUtil {

namespace {
    constexpr uint32_t MIN_BUNDLE_NAME_LEN = 7;
    constexpr uint32_t MAX_BUNDLE_NAME_LEN = 127;
    constexpr uint32_t MAX_NUMBER_SIZE = 10;
}
bool StrToFloat(const std::string& value, float& result)
{
    char* pEnd = nullptr;
    errno = 0;
    float res = std::strtof(value.c_str(), &pEnd);
    // check whether convert success
    if (errno == ERANGE || pEnd == value.c_str() || *pEnd != '\0') {
        RESSCHED_LOGD("%{public}s:convert err or overflow.", __func__);
        return false;
    }
    result = res;
    return true;
}

bool CheckBundleName(const std::string &bundleName)
{
    if (bundleName.empty()) {
        RESSCHED_LOGE("%{public}s:input bundle name is empty.", __func__);
        return false;
    }
    // check input length whether vaild.
    if (bundleName.size() < MIN_BUNDLE_NAME_LEN || bundleName.size() > MAX_BUNDLE_NAME_LEN) {
        RESSCHED_LOGE("%{public}s:input bundle %{public}s length is invalid.",
            __func__, bundleName.c_str());
        return false;
    }
    // check first character whether letter
    if (!isalpha(bundleName.front())) {
        RESSCHED_LOGE("%{public}s: %{public}s first character not letter.",
            __func__, bundleName.c_str());
        return false;
    }
    for (const auto &ch : bundleName) {
        // check all item whether number, letter, '_' or '.'
        if (!isalnum(ch) && ch != '_' && ch != '.') {
            RESSCHED_LOGE("%{public}s: %{public}s the item of bundle name is invalid.",
                __func__, bundleName.c_str());
            return false;
        }
    }
    return true;
}

bool StrToInt32(const std::string& value, int32_t& result)
{
    char* pEnd = nullptr;
    errno = 0;
    int64_t res = std::strtol(value.c_str(), &pEnd, 10);
    // check whether convert success
    if (errno == ERANGE || pEnd == value.c_str() || *pEnd != '\0' ||
        (res < INT_MIN || res > INT_MAX)) {
        RESSCHED_LOGD("%{public}s:convert err or overflow.", __func__);
        return false;
    }
    result = res;
    return true;
}

bool StrToInt64(const std::string& value, int64_t& result)
{
    char* pEnd = nullptr;
    errno = 0;
    // check whether convert success
    int64_t res = std::strtoll(value.c_str(), &pEnd, 10);
    if (errno == ERANGE || pEnd == value.c_str() || *pEnd != '\0') {
        RESSCHED_LOGE("%{public}s:convert err.", __func__);
        return false;
    }
    result = res;
    return true;
}

bool StrToUInt32(const std::string& value, uint32_t& result)
{
    char* pEnd = nullptr;
    errno = 0;
    // check whether convert success
    uint32_t res = (uint32_t)std::strtoul(value.c_str(), &pEnd, 10);
    if (errno == ERANGE || pEnd == value.c_str() || *pEnd != '\0' ||
        (res > UINT_MAX)) {
        RESSCHED_LOGD("%{public}s:convert err or overflow.", __func__);
        return false;
    }
    result = res;
    return true;
}

bool IsNumericString(const std::string& str)
{
    // check the length of input str whether vaild.
    if (str.size() > MAX_NUMBER_SIZE || str.size() == 0) {
        RESSCHED_LOGE("%{public}s: input length is invalid.", __func__);
        return false;
    }
    // when input length is one, check it whether number.
    if (str.size() == 1) {
        return isdigit(str[0]);
    }
    for (std::string::size_type i = 0;i < str.size(); ++i) {
        // first character is number or "-" can convert
        if (i == 0 && !(isdigit(str[0]) || str[0] == '-')) {
            RESSCHED_LOGE("%{public}s: input %{public}s is invalid.",
                __func__, str.c_str());
            return false;
        }
        // other character must be number
        if (i != 0 && !isdigit(str[i])) {
            RESSCHED_LOGE("%{public}s: input %{public}s is invalid.",
                __func__, str.c_str());
            return false;
        }
    }
    return true;
}

std::string StringTrim(const std::string& value)
{
    if (value.empty()) {
        return value;
    }
    auto strTmp = value;
    auto blanks("\f\v\r\t\n ");
    auto start = strTmp.find_first_not_of(blanks);
    if (start == std::string::npos) {
        return "";
    }
    strTmp.erase(0, start);
    strTmp.erase(strTmp.find_last_not_of(blanks) + 1);
    return strTmp;
}

std::string StringTrimSpace(const std::string& value)
{
    if (value.empty()) {
        return value;
    }
    auto strTmp = value;
    auto blanks(" ");
    auto start = strTmp.find_first_not_of(blanks);
    if (start == std::string::npos) {
        return "";
    }
    strTmp.erase(0, start);
    auto end = strTmp.find_last_not_of(blanks);
    strTmp.erase(end + 1);
    return strTmp;
}

bool StringToJson(const std::string& str, nlohmann::json& payload)
{
    // check input is vaild.
    if (str.empty()) {
        RESSCHED_LOGE("%{public}s: input empty.", __func__);
        return false;
    }
    auto jsonObj = nlohmann::json::parse(str, nullptr, false);
    // check whether parse success
    if (jsonObj.is_discarded()) {
        RESSCHED_LOGE("%{public}s: parse failed.", __func__);
        return false;
    }
    // check result whether an object
    if (!jsonObj.is_object()) {
        RESSCHED_LOGE("%{public}s: target not object.", __func__);
        return false;
    }
    // fill content to result.
    for (auto& [key, value] : jsonObj.items()) {
        payload[key] = value;
    }
    return true;
}
}
} // namespace ResourceSchedule
} // namespace OHOS
