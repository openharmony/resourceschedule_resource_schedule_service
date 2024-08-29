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
#include <unistd.h>
#include <fcntl.h>

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

    static void WriteFileReclaim(int32_t pid)
    {
        std::string path = "/proc/" + std::to_string(pid) + "/reclaim";
        std::string contentStr = "1";
        int fd = open(path.c_str(), O_WRONLY);
        if (fd < 0) {
            return;
        }
        write(fd, contentStr.c_str(), contentStr.length());
        close(fd);
    }
};
} // namespace ResourceSchedule
} // namespace OHOS

#endif // RESSCHED_COMMON_INCLUDE_RES_COMMON_UTIL_H