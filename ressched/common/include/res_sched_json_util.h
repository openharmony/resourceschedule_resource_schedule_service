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

#ifndef RESSCHED_COMMON_INCLUDE_RES_SCHED_JSON_UTIL
#define RESSCHED_COMMON_INCLUDE_RES_SCHED_JSON_UTIL
#include <string>

#include "nlohmann/json.hpp"

namespace OHOS {
namespace ResourceSchedule {
namespace ResCommonUtil {

/**
 * @brief Parse a int value with key name from the json object.
 *
 * @param name the key of the the target item
 * @param value the value of the target item
 * @param jsonObj the target object
 * @return 'true' if parse success
 *
 */
bool ParseIntParameterFromJson(const std::string& name, int32_t &value, const nlohmann::json& jsonObj);

/**
 * @brief Parse a string value with key name from the json object.
 *
 * @param name the key of the the target item
 * @param value the value of the target item
 * @param jsonObj the target object
 * @return 'true' if parse success
 *
 */
bool ParseStringParameterFromJson(const std::string& name, std::string &value, const nlohmann::json& jsonObj);

/**
 * @brief Parse a bool value with key name from the json object.
 *
 * @param name the key of the the target item
 * @param value the value of the target item
 * @param jsonObj the target object
 * @return 'true' if parse success
 *
 */
bool ParseBoolParameterFromJson(const std::string& name, bool& value, const nlohmann::json& jsonObj);

/**
 * @brief Get a array value with key name from the json object.
 *
 * @param jsonObj the target object
 * @param name the key of the the target item
 * @param value the value of the target item
 * @return 'true' if get success
 *
 */
bool GetArrayFromJson(const nlohmann::json& jsonObj, const std::string& name, nlohmann::json& value);

/**
 * @brief Get a obj value with key name from the json object.
 *
 * @param jsonObj the target object
 * @param name the key of the the target item
 * @param value the value of the target item
 * @return 'true' if get success
 *
 */
bool GetObjFromJson(const nlohmann::json& jsonObj, const std::string& name, nlohmann::json& value);

/**
 * @brief Load the file's content to a json object.
 *
 * @param absolutePath the file's absolutePath path
 * @param jsonObj the target object
 * @return 'true' if parse success
 *
 */
bool LoadFileToJsonObj(const std::string& absolutePath, nlohmann::json& jsonObj);

/**
 * @brief Load the config file's content to a json object.
 *
 * @param relativeFilePath the config file's relative path
 * @param jsonObj the target object
 * @return 'true' if parse success
 *
 */
bool LoadOneCfgFileToJsonObj(const std::string& relativeFilePath, nlohmann::json& jsonObj);

/**
 * @brief Load the string content to a json object.
 *
 * @param content the string content
 * @param jsonObj the target object
 * @return 'true' if parse success
 *
 */
bool LoadContentToJsonObj(const std::string& content, nlohmann::json& jsonObj);

/**
 * @brief Dump json to string.
 *
 * @param jsonObj the json object
 * @param content the string format content
 *
 */
void DumpJsonToString(const nlohmann::json& jsonObj, std::string& content);
}
} // namespace ResourceSchedule
} // namespace OHOS

#endif // RESSCHED_COMMON_INCLUDE_RES_SCHED_JSON_UTIL