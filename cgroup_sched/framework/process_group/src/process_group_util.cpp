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

#include <fstream>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <linux/limits.h>
#include "json/writer.h"
#include "securec.h"
#include "process_group_log.h"

namespace OHOS {
namespace ResourceSchedule {
namespace CgroupSetting {
std::string FormatString(const char* fmt, va_list vararg)
{
    std::string strResult;
    if (fmt) {
        va_list tmpArgs;
        va_copy(tmpArgs, vararg);
        size_t nLength = vsnprintf(nullptr, 0, fmt, tmpArgs); // compute buffer size
        va_end(tmpArgs);
        std::vector<char> vBuffer(nLength + 1, '\0');
        int nWritten = vsnprintf_s(&vBuffer[0], nLength + 1, nLength, fmt, vararg);
        if (nWritten > 0) {
            strResult = &vBuffer[0];
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

bool GetRealPath(const std::string& path, std::string& realPath)
{
    char resolvedPath[PATH_MAX] = { 0 };
    if (path.size() > PATH_MAX || !realpath(path.c_str(), resolvedPath)) {
        PGCGS_LOGE("Error: _fullpath %{public}s failed", path.c_str());
        return false;
    }
    realPath = std::string(resolvedPath);
    return true;
}

bool ReadFileToString(const std::string& filePath, std::string& content)
{
    std::string realPath;
    if (!GetRealPath(filePath, realPath)) {
        return false;
    }
    int fd = open(realPath.c_str(), O_RDONLY | O_CLOEXEC);
    if (fd < 0) {
        return false;
    }
    struct stat sb {};
    if (fstat(fd, &sb) != -1 && sb.st_size > 0) {
        content.resize(sb.st_size);
    }
    ssize_t n;
    ssize_t remaining = sb.st_size;
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


bool ReadFileToStringForVFS(const std::string& filePath, std::string& content)
{
    std::string realPath;
    if (!GetRealPath(filePath, realPath)) {
        return false;
    }
    std::ifstream fin(realPath.c_str(), std::ios::in);
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
        if (n == -1) {
            return false;
        }
        p += n;
        remaining -= n;
    }
    return true;
}

bool WriteStringToFile(const std::string& content, const std::string& filePath)
{
    std::string realPath;
    if (!GetRealPath(filePath, realPath)) {
        return false;
    }
    if (access(realPath.c_str(), W_OK)) {
        return false;
    }
    int fd = open(realPath.c_str(), O_WRONLY | O_CLOEXEC);
    if (fd < 0) {
        PGCGS_LOGE("WriteStringToFile fail. file: %{public}s, fd = %{public}d", realPath.c_str(), fd);
        return false;
    }
    bool result =  WriteStringToFile(fd, content);
    close(fd);
    return result;
}

std::string JsonToString(const Json::Value &json)
{
    std::string result;
    Json::StreamWriterBuilder builder;
    builder["indentation"] = ""; // Optional if you want whitespace-less output
    result = Json::writeString(builder, json);
    return result;
}
} // namespace CgroupSetting
} // namespace ResourceSchedule
} // namespace OHOS