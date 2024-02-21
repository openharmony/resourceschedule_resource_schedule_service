/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef RESSCHED_COMMON_INCLUDE_RES_COMMON_UTIL_H
#define RESSCHED_COMMON_INCLUDE_RES_COMMON_UTIL_H

#include <string>

namespace OHOS {
namespace ResourceSchedule {
namespace {
    constexpr uint32_t MIN_BUNDLE_NAME_LEN = 7;
    constexpr uint32_t MAX_BUNDLE_NAME_LEN = 127;
}
class ResCommonUtil {
public:
    static bool CheckBundleName(const std::string &bundleName)
    {
        if (bundleName.empty()) {
            return false;
        }
        if (bundleName.size() < MIN_BUNDLE_NAME_LEN || bundleName.size() > MAX_BUNDLE_NAME_LEN) {
            return false;
        }
        if (!isalpha(bundleName.front())) {
            return false;
        }
        for (const auto &ch : bundleName) {
            if (!isalnum(ch) && ch != '_' && ch != '.') {
                return false;
            }
        }
        return true;
    }
    static bool StringToInt32(const std::string &inValue, int32_t &outValue)
    {
        char* pEnd = nullptr;
        errno = 0;
        int64_t result = std::strtol(inValue.c_str(), &pEnd, 10);
        if (errno == ERANGE || pEnd == inValue.c_str() || *pEnd != '\0' ||
          (result < INT_MIN || result > INT_MAX)) {
            return false;
        } else {
            outValue = result;
            return true;
        }
    }
};
} // namespace ResourceSchedule
} // namespace OHOS

#endif // RESSCHED_COMMON_INCLUDE_RES_COMMON_UTIL_H