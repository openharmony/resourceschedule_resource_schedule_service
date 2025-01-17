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

#include "res_sched_json_util.h"

#include <fstream>

#include "res_sched_file_util.h"
#include "res_sched_log.h"
#include "res_sched_string_util.h"

namespace OHOS {
namespace ResourceSchedule {
namespace ResCommonUtil {
bool ParseIntParameterFromJson(const std::string& name, int32_t &value, const nlohmann::json& jsonObj)
{
    // if target exist.
    if (!jsonObj.is_null() && !name.empty() && jsonObj.contains(name)) {
        // target value is string, convert is to int.
        if (jsonObj.at(name).is_string() && StrToInt32(jsonObj[name].get<std::string>(), value)) {
            return true;
        // target value is int, get and return directly.
        } else if (jsonObj.at(name).is_number_integer()) {
            value = jsonObj[name].get<int32_t>();
            return true;
        }
    }
    return false;
}

bool ParseStringParameterFromJson(const std::string& name, std::string &value, const nlohmann::json& jsonObj)
{
    if (!jsonObj.is_null() && !name.empty() && jsonObj.contains(name) && jsonObj.at(name).is_string()) {
        value = jsonObj[name].get<std::string>();
        return true;
    }
    return false;
}

bool ParseBoolParameterFromJson(const std::string& name, bool& value, const nlohmann::json& jsonObj)
{
    if (!jsonObj.is_null() && !name.empty() && jsonObj.contains(name) && jsonObj.at(name).is_boolean()) {
        value = jsonObj[name].get<bool>();
        return true;
    }
    return false;
}

bool LoadOneCfgFileToJsonObj(const std::string& relativeFilePath, nlohmann::json& jsonObj)
{
    std::string absolutePath;
    // judge input path exist.
    if (!GetRealConfigPath(relativeFilePath, absolutePath) || absolutePath.empty()) {
        RESSCHED_LOGE("%{public}s:get config file path failed.", __func__);
        return false;
    }
    return LoadFileToJsonObj(absolutePath, jsonObj);
}

bool LoadFileToJsonObj(const std::string& absolutePath, nlohmann::json& jsonObj)
{
    std::ifstream fileStream(absolutePath);
    // judge open success.
    if (!fileStream.is_open()) {
        RESSCHED_LOGE("%{public}s:open file fail.", __func__);
        return false;
    }
    // parse file stream to json obj
    jsonObj = nlohmann::json::parse(fileStream, nullptr, false);
    fileStream.close();
    // if parse fialed
    if (jsonObj.is_discarded()) {
        RESSCHED_LOGE("%{public}s:parse to json fail.", __func__);
        return false;
    }
    return true;
}

bool GetArrayFromJson(const nlohmann::json& jsonObj, const std::string& name, nlohmann::json& value)
{
    // check whether input is vaild.
    if (jsonObj.is_null() || name.empty()) {
        RESSCHED_LOGE("%{public}s:input err.", __func__);
        return false;
    }
    // check json whether has this name's key.
    if (jsonObj.find(name) == jsonObj.end() || !jsonObj[name].is_array()) {
        RESSCHED_LOGE("%{public}s:parse err.", __func__);
        return false;
    }
    value = jsonObj[name];
    return true;
}

bool GetObjFromJson(const nlohmann::json& jsonObj, const std::string& name, nlohmann::json& value)
{
    // check whether input is vaild.
    if (jsonObj.is_null() || name.empty()) {
        RESSCHED_LOGE("%{public}s:input err.", __func__);
        return false;
    }
    // check json whether has this name's key.
    if (jsonObj.find(name) == jsonObj.end() || !jsonObj[name].is_object()) {
        RESSCHED_LOGE("%{public}s:parse err.", __func__);
        return false;
    }
    value = jsonObj[name];
    return true;
}

bool LoadContentToJsonObj(const std::string& content, nlohmann::json& jsonObj)
{
    // check whether input is vaild.
    if (content.empty()) {
        RESSCHED_LOGE("%{public}s:input err.", __func__);
        return false;
    }
    // parse content to a json obj.
    jsonObj = nlohmann::json::parse(content, nullptr, false);
    // check whether parse success.
    if (jsonObj.is_discarded()) {
        RESSCHED_LOGE("%{public}s:parse fail.", __func__);
        return false;
    }
    // check json obj whether an obj.
    if (!jsonObj.is_object()) {
        RESSCHED_LOGE("%{public}s:the content is not an object.", __func__);
        return false;
    }
    return true;
}

void DumpJsonToString(const nlohmann::json& jsonObj, std::string& content)
{
    content = jsonObj.dump(-1, ' ', false, nlohmann::detail::error_handler_t::replace);
}
}
} // namespace ResourceSchedule
} // namespace OHOS
