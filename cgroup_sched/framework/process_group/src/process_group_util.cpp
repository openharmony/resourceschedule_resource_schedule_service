/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <fstream>
#include "process_group_log.h"
#include "process_group_util.h"

namespace OHOS {
namespace ResourceSchedule {
namespace CgroupSetting {

std::string FormatString(const char* fmt, va_list vararg)
{
    static std::vector<char> buffer(512);
    // Attempt to just print to the current buffer
    int len = vsnprintf(&buffer[0], buffer.size(), fmt, vararg);
    if (len < 0 || static_cast<size_t>(len) >= buffer.size()) {
        // Buffer was not large enough, calculate the required size and resize the buffer
        len = vsnprintf(nullptr, 0, fmt, vararg);
        buffer.resize(len + 1);
        // Print again
        vsnprintf(&buffer[0], buffer.size(), fmt, vararg);
    }
    return std::string(buffer.data(), len);
}

std::string StringPrintf(const char* fmt, ...)
{
    va_list vararg;
    va_start(vararg, fmt);
    std::string result = FormatString(fmt, vararg);
    va_end(vararg);
    return result;
}

bool ReadFileToString(const std::string& fileName, std::string& content)
{
    int fd = open(fileName.c_str(), O_RDONLY | O_CLOEXEC);
    if (fd < 0) {
        return false;
    }
    struct stat sb {};
    if (fstat(fd, &sb) != -1 && sb.st_size > 0) {
        content.resize(sb.st_size);
    }
    ssize_t n;
    size_t remaining = sb.st_size;
    char* p = const_cast<char*>(content.data());
    while (remaining > 0) {
        n = read(fd, p, remaining);
        if (n < 0) {
            return false;
        }
        p += n;
        remaining -= n;
    }
    close(fd);
    return true;
}


bool ReadFileToStringForVFS(const std::string& fileName, std::string& content) {
    std::ifstream fin(fileName.c_str(), std::ios::in);
    if (!fin) {
        return false;
    }
    fin >> content;
    return true;
}


bool WriteStringToFile(int fd, const std::string& content)
{
    const char *p = content.data();
    size_t remaining = content.size();
    while (remaining > 0) {
        ssize_t n = write(fd, p, remaining);
        if(n == -1) {
            return false;
        }
        p += n;
        remaining -= n;
    }
    return true;
}

bool WriteStringToFile(const std::string& content, const std::string& filePath)
{
    if (access(filePath.c_str(), W_OK)) {
        return false;
    }
    int fd = open(filePath.c_str(), O_WRONLY | O_CLOEXEC);
    if (fd < 0) {
        PGCGS_LOGE("WriteStringToFile fail. file: %{public}s, fd = %{public}d", filePath.c_str(), fd);
        return false;
    }
    bool result =  WriteStringToFile(fd, content);
    close(fd);
    return result;
}

} // namespace CgroupSetting
} // namespace ResourceSchedule
} // namespace OHOS