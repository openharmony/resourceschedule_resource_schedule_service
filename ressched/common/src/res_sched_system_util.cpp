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

#include "res_sched_system_util.h"

#include <sys/statvfs.h>
#include <sys/sysinfo.h>

#include "res_sched_log.h"
#include "res_sched_string_util.h"
#include "parameters.h"

namespace OHOS {
namespace ResourceSchedule {
namespace ResCommonUtil {
namespace {
    constexpr int64_t BYTES_PER_GB = 1024 * 1024 * 1024;
    constexpr int64_t BYTES_PER_KB = 1024;
    constexpr int64_t BYTES_PER_MB = 1024 * 1024;
    const std::string PATH_DATA = "/data";
    const std::string PATH_ROOT = "/";
}

int64_t GetTotalRamSize(const std::string& ddrSysProp)
{
    int64_t totalRamSize = 0;
    // attempt obtain ram size for system prop
    if (!ddrSysProp.empty()) {
        std::string ddrString = GetSystemProperties(ddrSysProp, "");
        if (StrToInt64(ddrString, totalRamSize)) {
            totalRamSize *= BYTES_PER_KB;
            return totalRamSize;
        }
    }
    // not give sys prop or get info from sys info failed, obtain through system call
    struct sysinfo info;
    if (sysinfo(&info) != 0) {
        RESSCHED_LOGE("%{public}s error", __func__);
        return totalRamSize;
    }
    auto totalSizeBytes = static_cast<int64_t>(info.totalram);
    // convert byte to MB
    totalRamSize = ceil(totalSizeBytes / BYTES_PER_GB) * BYTES_PER_KB;
    return totalRamSize;
}

inline bool GetTotalSizeByStatvfs(const std::string& path, int64_t& size)
{
    struct statvfs diskInfo;
    int res = statvfs(path.c_str(), &diskInfo);
    if (res != 0) {
        RESSCHED_LOGE("%{public}s get disk info failed", __func__);
        return false;
    }
    size = static_cast<int64_t>(diskInfo.f_bsize) * static_cast<int64_t>(diskInfo.f_blocks);
    return true;
}

int64_t RoundStorageSize(const int64_t& size)
{
    // check input size is vaild
    if (size < 0) {
        return size;
    }
    uint64_t uSize = static_cast<uint64_t>(size);
    uint16_t value = 1;
    uint32_t unit = 1;
    // value indicate size of unit, when this value of size less than input size
    while ((value * unit) < uSize) {
        value <<= 1;
        if (value >= BYTES_PER_KB) {
            value = 1;
            unit *= BYTES_PER_KB;
        }
    }
    return static_cast<int64_t>(value * unit);
}

int64_t GetTotalRomSize()
{
    int64_t totalRomSize = 0;
    int64_t dataRomSize = 0;
    // get /data path's total rom
    if (!GetTotalSizeByStatvfs(PATH_DATA, dataRomSize)) {
        RESSCHED_LOGE("%{public}s: get data size error", __func__);
        return totalRomSize;
    }
    // get / path's tatoal rom
    int64_t rootRomSize = 0;
    if (!GetTotalSizeByStatvfs(PATH_ROOT, rootRomSize)) {
        RESSCHED_LOGE("%{public}s: get root size error", __func__);
        return totalRomSize;
    }
    int64_t totalSizeBytes = dataRomSize + rootRomSize;
    // round up total rom size
    totalRomSize = RoundStorageSize(totalSizeBytes / BYTES_PER_MB);
    return totalRomSize;
}

std::string GetSystemProperties(const std::string& name, const std::string &defaultValue)
{
    return system::GetParameter(name, defaultValue);
}
}
} // namespace ResourceSchedule
} // namespace OHOS
