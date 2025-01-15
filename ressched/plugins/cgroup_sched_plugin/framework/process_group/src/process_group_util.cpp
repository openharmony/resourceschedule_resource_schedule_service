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

#include "process_group_util.h"
#include <sstream>
#include <string>
#include <fstream>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <linux/limits.h>
#include "nlohmann/json.hpp"
#include "securec.h"
#include "process_group_log.h"
#include "res_common_util.h"
#include "res_exe_type.h"
#include "res_sched_exe_client.h"

namespace OHOS {
namespace ResourceSchedule {
namespace CgroupSetting {
namespace {
    static constexpr int FMT_STR_BUFF_LEN = 256;
}

constexpr uint64_t SCHEDULE_CGROUP_FDSAN_TAG = 0xd001702;
constexpr uint64_t COMMON_CGROUP_FDSAN_TAG = 0;

std::string FormatString(const char* fmt, va_list vararg)
{
    std::string strResult;
    if (fmt) {
        char buffer[FMT_STR_BUFF_LEN] = { 0 };
        int nWritten = vsnprintf_s(buffer, FMT_STR_BUFF_LEN, FMT_STR_BUFF_LEN - 1, fmt, vararg);
        if (nWritten > 0) {
            strResult.append(buffer, 0, nWritten);
        }
    }
    return strResult;
}

std::string StringPrintf(const char* fmt, ...)
{
    va_list vararg;
    va_start(vararg, fmt);
    std::string result = FormatString(fmt, vararg);
    va_end(vararg);
    return result;
}

bool ReadFileToStringForVFSFromExecutor(int tid, std::string& content)
{
    nlohmann::json payload;
    payload["pid"] = tid;
    nlohmann::json reply;
    ResourceSchedule::ResSchedExeClient::GetInstance().SendRequestSync(
        ResExeType::RES_TYPE_CGROUP_SYNC_EVENT, 0, payload, reply);
    std::string resStr{"res"};
    if (!reply.contains(resStr) || !reply[resStr].is_string()) {
        return false;
    }
    content = reply[resStr].get<std::string>();
    return true;
}

bool ReadFileToStringForVFS(const std::string& filePath, std::string& content)
{
    std::string realPath;
    if (!ResCommonUtil::GetRealPath(filePath, realPath)) {
        return false;
    }
    std::ifstream fin(realPath.c_str(), std::ios::in);
    if (!fin) {
        return false;
    }
    std::stringstream ss;
    ss << fin.rdbuf();
    content = ss.str();
    fin.close();
    return true;
}
} // namespace CgroupSetting
} // namespace ResourceSchedule
} // namespace OHOS
