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

#ifndef RESSCHED_COMMON_INCLUDE_RES_SCHED_FILE_UTIL
#define RESSCHED_COMMON_INCLUDE_RES_SCHED_FILE_UTIL
#include <fcntl.h>
#include <string>
#include <unistd.h>

namespace OHOS {
namespace ResourceSchedule {
namespace ResCommonUtil {

/**
 * @brief Write reclaim status into given pid's node.
 *
 * @param pid Indicates the pid will write reclaim status
 *
 */
void WriteFileReclaim(int32_t pid);

/**
 * @brief Obtains the real path from a relative path.
 *
 * @param filePath the relative path
 * @param realPath the real path
 * @return 'true' if obtain succeed.
 */
bool GetRealPath(const std::string& filePath, std::string& realPath);

/**
 * @brief Judje path exists.
 *
 * @param path path
 * @return 'true' if exist.
 */
bool PathOrFileExists(const std::string& path);

/**
 * @brief traverse all sub path and file path in given file path.
 *
 * @param filePath file path
 * @param iters the traverse result
 * @return 'true' if dir is vaild
 */
bool DirIterator(const std::string& filePath, std::vector<std::string>& iters);

/**
 * @brief Checks weather a folder is empty.

 *
 * @param filePath file path will checked
 * @return 'true' if dir floder is empty.
 */
bool IsEmptyDir(const std::string& filePath);

/**
 * @brief Checks weather a string is floder dir.
 *
 * @param filePath file path will checked
 * @return 'true' if filePath is dir.
 */
bool IsDir(const std::string& filePath);

/**
 * @brief Create dir with mode.
 *
 * @param dir dir will create
 * @param mode the dir's mode
 * @return 'true' if dir create succeed.
 */
bool CreateDir(const std::string& dir, const mode_t& mode);

/**
 * @brief Create file with mode.
 *
 * @param filePath will create
 * @param mode the dir's mode
 * @return 'true' if file create succeed.
 */
bool CreateFile(const std::string& filePath, const mode_t& mode);

/**
 * @brief Remove dir.
 *
 * @param dir dir will removed
 * @return 'true' if dir remove succeed.
 */
bool RemoveDirs(const std::string& dir);

/**
 * @brief Remove file.
 *
 * @param filePath file will removed
 * @return 'true' if dir remove succeed.
 */
bool RemoveFile(const std::string& filePath);

/**
 * @brief Remove file or dir if exist.
 *
 * @param path dir or file which will be removed.
 * @return 'true' if dir remove succeed.
 */
bool RemoveFileOrDirIfExist(const std::string& path);

/**
 * @brief Extract file name from file path.
 *
 * @param filePath the file path
 * @return file name.
 */
std::string ExtractFileName(const std::string& filePath);

/**
 * @brief Check weather a file path is BLK mode.
 *
 * @param filePath the file path
 * @return 'true' if file path is BLK mode.
 */
bool IsBLKPath(const std::string& filePath);

/**
 * @brief Write a string to a file.
 *
 * @param filePath Indicates the path of target file
 * @param content Indicates the string object to write
 * @param truncated Indicates wheather to truncate the original file
 * @return 'true' if the string is written successfully.
 */
bool SaveStringToFile(const std::string& filePath, const std::string& content, bool truncated = true);

/**
 * @brief Read Lines from file.
 *
 * @param filePath Indicates the path of target file
 * @param lines the lines of the file
 * @return 'true' if read successfully.
 */
bool ReadLinesFromFile(const std::string& filePath, std::vector<std::string>& lines);

/**
 * @brief Copy file from src to des.
 *
 * @param src the file will be copied
 * @param tes the target path
 * @return 'true' if copy successfully.
 */
bool CopyFile(const std::string& src, const std::string& des);

/**
 * @brief Get the highest priority config file absolute path
 *
 * @param configPath the relative path of the config
 * @param realConfigPath the absolute path of the highest priority path
 * @return 'true' if the config is exist.
 */
bool GetRealConfigPath(const std::string& configPath, std::string& realConfigPath);

/**
 * @brief check path is valid
 *
 * @param path The path that needs to be checked
 * @return 'true' if the path is valid.
 */
bool IsValidPath(const std::string& path);

/**
 * @brief get file size
 *
 * @param path The file path
 * @return the size of the file.
 */
uint64_t GetFileSize(const std::string& path);

/**
 * @brief get device valid size
 *
 * @param path The partition of path
 * @return the remain size of partition.
 */
double GetDeviceVailidSize(const std::string& path);
}
} // namespace ResourceSchedule
} // namespace OHOS

#endif // RESSCHED_COMMON_INCLUDE_RES_SCHED_FILE_UTIL