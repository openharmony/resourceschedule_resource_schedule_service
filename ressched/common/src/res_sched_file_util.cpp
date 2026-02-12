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

#include "res_sched_file_util.h"

#include <system_error>
#include <cstring>
#include <dirent.h>
#include <fstream>
#include <filesystem>
#include <sys/statfs.h>

#include "config_policy_utils.h"
#include "directory_ex.h"
#include "file_ex.h"
#include "res_sched_log.h"

namespace OHOS {
namespace ResourceSchedule {
namespace ResCommonUtil {
void WriteFileReclaim(int32_t pid)
{
    std::string path = "/proc/" + std::to_string(pid) + "/reclaim";
    std::string contentStr = "1";
    int fd = open(path.c_str(), O_WRONLY);
    // judge whether open failed
    if (fd < 0) {
        RESSCHED_LOGE("%{public}s: open failed.", __func__);
        return;
    }
    fdsan_exchange_owner_tag(fd, 0, LOG_DOMAIN);
    // write content to fd
    write(fd, contentStr.c_str(), contentStr.length());
    fdsan_close_with_tag(fd, LOG_DOMAIN);
}

bool GetRealPath(const std::string& filePath, std::string& realPath)
{
    if (!OHOS::PathToRealPath(filePath, realPath)) {
        RESSCHED_LOGE("%{public}s: get real path failed.", __func__);
        return false;
    }
    return true;
}

bool PathOrFileExists(const std::string& path)
{
    if (access(path.c_str(), F_OK) != 0) {
        RESSCHED_LOGD("%{public}s: access failed.", __func__);
        return false;
    }
    return true;
}

bool DirIterator(const std::string& filePath, std::vector<std::string>& iters)
{
    DIR *dir = opendir(filePath.c_str());
    // input invaild, open failed
    if (dir == nullptr) {
        RESSCHED_LOGE("%{public}s: open failed.", __func__);
        return false;
    }
    struct dirent *ptr;
    while ((ptr = readdir(dir)) != nullptr) {
        // current dir is path of '.' or '..'
        if ((strcmp(ptr->d_name, ".") == 0) || (strcmp(ptr->d_name, "..") == 0)) {
            RESSCHED_LOGD("%{public}s: skip current and pre path", __func__);
            continue;
        }
        iters.emplace_back(IncludeTrailingPathDelimiter(filePath) + std::string(ptr->d_name));
    }
    closedir(dir);
    return true;
}

bool IsEmptyDir(const std::string& filePath)
{
    return OHOS::IsEmptyFolder(filePath);
}

bool IsDir(const std::string& filePath)
{
    struct stat info{};
    // dose not exist
    if (stat(filePath.c_str(), &info) != 0) {
        RESSCHED_LOGE("%{public}s: file path not exist.", __func__);
        return false;
    }
    // is not directory
    if (!(info.st_mode & S_IFDIR)) {
        return false;
    }
    return true;
}

bool CreateDir(const std::string& dir, const mode_t& mode)
{
    // create directory
    if (!OHOS::ForceCreateDirectory(dir)) {
        RESSCHED_LOGE("%{public}s: Failed to create dir", __func__);
        return false;
    }
    // change directory's mode.
    if (!OHOS::ChangeModeDirectory(dir, mode)) {
        RESSCHED_LOGE("%{public}s: Failed to change dir mode to %{public}ud", __func__,
            mode);
        // if change failed, remove it.
        RemoveDirs(dir);
        return false;
    }
    return true;
}

bool CreateFile(const std::string& filePath, const mode_t& mode)
{
    std::ofstream fd;
    // create and open file.
    fd.open(filePath);
    if (!fd.is_open()) {
        //create filed
        RESSCHED_LOGE("%{public}s: Failed to create file,errno=%{public}d", __func__, errno);
        return false;
    }
    fd.close();

    //change file mode
    if (!OHOS::ChangeModeFile(filePath, mode)) {
        //change failed
        RESSCHED_LOGE("%{public}s: Failed to change file mode to %{public}ud", __func__,
            mode);
        RemoveFile(filePath);
        return false;
    }
    return true;
}

bool RemoveDirs(const std::string& dir)
{
    if (!OHOS::ForceRemoveDirectory(dir)) {
        RESSCHED_LOGE("%{public}s: Failed to remove dir", __func__);
        return false;
    }
    return true;
}

bool RemoveFile(const std::string& filePath)
{
    if (!OHOS::RemoveFile(filePath)) {
        RESSCHED_LOGE("%{public}s: Failed to remove file", __func__);
        return false;
    }
    return true;
}

std::string ExtractFileName(const std::string& filePath)
{
    return OHOS::ExtractFileName(filePath);
}

bool IsBLKPath(const std::string& filePath)
{
    struct stat sb;
    if (stat(filePath.c_str(), &sb) == 0 && S_ISBLK(sb.st_mode)) {
        return true;
    }
    return false;
}

bool SaveStringToFile(const std::string& filePath, const std::string& content, bool truncated)
{
    return OHOS::SaveStringToFile(filePath, content, truncated);
}

bool ReadLinesFromFile(const std::string& filePath, std::vector<std::string>& lines)
{
    std::string line;
    char tmpPath[PATH_MAX + 1] = {0};
    auto len = filePath.length();
    if (len == 0 || len > PATH_MAX || !realpath(filePath.c_str(), tmpPath)) {
        RESSCHED_LOGE("%{public}s: file path invalid", __func__);
        return false;
    }
    std::string realConfigPath(tmpPath);
    std::ifstream fin(realConfigPath, std::ifstream::in);
    // judge whether open failed.
    if (!fin) {
        RESSCHED_LOGE("%{public}s: open file failed.", __func__);
        return false;
    }
    // clear container and read lines
    lines.clear();
    while (fin.peek() != EOF) {
        getline(fin, line);
        lines.emplace_back(line);
    }
    fin.close();
    return true;
}

bool DoCopy(const std::string& src, const std::string& des)
{
    std::filesystem::path sPath(src);
    std::filesystem::path dPath(des);
    std::error_code errNo;

    const auto copyOptions = std::filesystem::copy_options::overwrite_existing |
        std::filesystem::copy_options::recursive |
        std::filesystem::copy_options::skip_symlinks;
    std::filesystem::copy(sPath, dPath, copyOptions, errNo);
    // if has some error in copy, record errno
    if (errNo.value()) {
        RESSCHED_LOGE("copy failed errno:%{public}d", errNo.value());
        return false;
    }
    RESSCHED_LOGD("copy success");
    return true;
}

bool CopyFile(const std::string& src, const std::string& des)
{
    // judge src exist.
    if (!PathOrFileExists(src)) {
        RESSCHED_LOGE("%{public}s: src path invalid!", __func__);
        return false;
    }
    if (PathOrFileExists(des)) {
        // target path is exist, remove it before copy.
        if (!RemoveDirs(des)) {
            RESSCHED_LOGE("%{public}s: target path is exist and remove failed!", __func__);
            return false;
        }
    }
    errno = 0;
    // create target directory.
    if (mkdir(des.c_str(), S_IRWXU) != 0 && errno != EEXIST) {
        RESSCHED_LOGE("%{public}s: create target path failed!", __func__);
        return false;
    }
    // do real copy action.
    return DoCopy(src, des);
}

bool GetRealConfigPath(const std::string& configPath, std::string& realConfigPath)
{
    // judge input path vaild.
    if (configPath.empty()) {
        RESSCHED_LOGE("%{public}s:the input config path is empty", __func__);
        return false;
    }
    char buf[PATH_MAX];
    char* tmpPath = GetOneCfgFile(configPath.c_str(), buf, PATH_MAX);
    char absolutePath[PATH_MAX] = {0};
    // if config path is vaild, obtain real path.
    if (!tmpPath || strlen(tmpPath) > PATH_MAX || !realpath(tmpPath, absolutePath)) {
        RESSCHED_LOGE("%{public}s:get one config file failed", __func__);
        return false;
    }
    realConfigPath = std::string(absolutePath);
    return true;
}

bool IsValidPath(const std::string& path)
{
    return !(path.empty() || path.find('\0') != std::string::npos);
}

uint64_t GetFileSize(const std::string& path)
{
    struct stat st;
    return stat(path.c_str(), &st) ? 0 : static_cast<uint64_t>(st.st_size);
}

std::string GetPartitionName(const std::string& path)
{
    std::string partition;
    std::size_t first = path.find_first_of("/");
    if (first == std::string::npos) {
        partition = "/" + path;
        return partition;
    }
    std::size_t second = path.find_first_of("/", first + 1);
    if (second == std::string::npos) {
        if (path.at(0) != '/') {
            partition = "/" + path.substr(0, first);
        } else {
            partition = path;
        }
        return partition;
    }
    partition = path.substr(0, second - first);
    return partition;
}

double GetDeviceValidSize(const std::string& path)
{
    std::string partitionName = GetPartitionName(path);
    struct statfs stat;
    if (statfs(partitionName.c_str(), &stat) != 0) {
        return 0;
    }
    constexpr double units = 1024.0;
    return (static_cast<double>(stat.f_bfree) / units) * (static_cast<double>(stat.f_bsize) / units);
}
} // namespace ResCommonUtil
} // namespace ResourceSchedule
} // namespace OHOS
