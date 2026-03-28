/*
 * Copyright (c) 2025-2026 Huawei Device Co., Ltd.
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

#include "res_sched_json_storage_util.h"

#include <fcntl.h>
#include <fstream>
#include <unistd.h>
#include <sstream>
#include <sys/stat.h>

#include "errors.h"
#include "res_sched_log.h"

namespace {
constexpr size_t MAX_JSON_FILE_SIZE = 10 * 1024 * 1024;
constexpr size_t MAX_JSON_WRITE_SIZE = 10 * 1024 * 1024;
constexpr int32_t SECURE_FILE_PERMISSIONS = S_IRUSR | S_IWUSR;
constexpr int32_t FILE_OPEN_FLAGS = O_CREAT | O_RDWR;
}

namespace OHOS {
namespace ResourceSchedule {
namespace ResCommonUtil {
int32_t ResSchedJsonStorage::RefreshJsonFileInfo(const std::string &infos, const std::string &filePath)
{
    return SaveJsonValueToFile(infos, filePath);
}

int32_t ResSchedJsonStorage::RestoreJsonFileInfo(nlohmann::json &value, const std::string &filePath)
{
    return ParseJsonValueFromFile(value, filePath);
}

int32_t ResSchedJsonStorage::SaveJsonValueToFile(const std::string &value, const std::string &filePath)
{
    if (filePath.empty()) {
        RESSCHED_LOGE("File path is empty");
        return -1;
    }

    if (value.length() > MAX_JSON_WRITE_SIZE) {
        RESSCHED_LOGE("JSON content too large: %{private}zu bytes", value.length());
        return -1;
    }

    if (!CreateNodeFile(filePath)) {
        RESSCHED_LOGE("Create file failed.");
        return -1;
    }
    std::ofstream fout;
    std::string realPath;
    if (!ConvertFullPath(filePath, realPath)) {
        RESSCHED_LOGE("SaveJsonValueToFile Get real file path: %{private}s failed", filePath.c_str());
        return -1;
    }
    fout.open(realPath, std::ios::out);
    if (!fout.is_open()) {
        RESSCHED_LOGE("Open file: %{private}s failed.", filePath.c_str());
        return -1;
    }
    fout << value.c_str() << std::endl;
    fout.close();
    return ERR_OK;
}

int32_t ResSchedJsonStorage::ParseJsonValueFromFile(nlohmann::json &value, const std::string &filePath)
{
    std::ifstream fin;
    std::string realPath;
    if (!ConvertFullPath(filePath, realPath)) {
        RESSCHED_LOGE("Get real path failed");
        return -1;
    }
    fin.open(realPath, std::ios::in);
    if (!fin.is_open()) {
        RESSCHED_LOGE("cannot open file %{private}s", realPath.c_str());
        return -1;
    }

    std::stringstream ss;
    ss << fin.rdbuf();
    std::string data = ss.str();
    if (data.length() > MAX_JSON_FILE_SIZE) {
        RESSCHED_LOGE("File too large: %{private}zu bytes", data.length());
        fin.close();
        return -1;
    }

    value = nlohmann::json::parse(data, nullptr, false);
    if (value.is_discarded()) {
        RESSCHED_LOGE("failed due to data is discarded");
        return -1;
    }
    return ERR_OK;
}

bool ResSchedJsonStorage::CreateNodeFile(const std::string &filePath)
{
    if (access(filePath.c_str(), F_OK) == ERR_OK) {
        return true;
    }
    // 路径规范化处理，防止路径穿越攻击
    std::string realPath;
    if (!ConvertFullPath(filePath, realPath)) {
        RESSCHED_LOGD("CreateNodeFile Get real file path: %{private}s failed", filePath.c_str());
        // 文件不存在时realpath会失败，需要规范化父目录
        size_t lastSlash = filePath.find_last_of('/');
        if (lastSlash == std::string::npos) {
            RESSCHED_LOGE("Invalid file path: %{private}s", filePath.c_str());
            return false;
        }
        std::string parentDir = filePath.substr(0, lastSlash);
        std::string fileName = filePath.substr(lastSlash + 1);
        if (fileName.find('/') != std::string::npos ||
            fileName.find('\\') != std::string::npos) {
            RESSCHED_LOGE("Invalid file name with path separator: %{private}s", fileName.c_str());
            return false;
        }

        if (fileName.find("..") != std::string::npos) {
            RESSCHED_LOGE("Invalid file name with ..: %{private}s", fileName.c_str());
            return false;
        }

        std::string realParentPath;
        if (!ConvertFullPath(parentDir, realParentPath)) {
            RESSCHED_LOGE("CreateNodeFile Get real parent path: %{private}s failed", parentDir.c_str());
            return false;
        }
        realPath = realParentPath + "/" + fileName;
    }

    int32_t fd = open(realPath.c_str(), FILE_OPEN_FLAGS, SECURE_FILE_PERMISSIONS);
    if (fd < ERR_OK) {
        RESSCHED_LOGE("Fail to open file:%{private}s", realPath.c_str());
        return false;
    }
    close(fd);
    return true;
}

bool ResSchedJsonStorage::ConvertFullPath(const std::string &partialPath, std::string &fullPath)
{
    if (partialPath.empty() || partialPath.length() >= PATH_MAX) {
        RESSCHED_LOGE("partialPath.empty(): %{public}u", partialPath.empty());
        return false;
    }
    char tmpPath[PATH_MAX] = {0};
    if (realpath(partialPath.c_str(), tmpPath) == nullptr) {
        RESSCHED_LOGE("realpath err: %{public}s", strerror(errno));
        return false;
    }
    fullPath = tmpPath;
    return true;
}
} // namespace ResCommonUtil
} // namespace ResourceSchedule
} // namespace OHOS