/*
 * Copyright (c) 2022=4-2024 Huawei Device Co., Ltd.
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

#ifndef RESSCHED_EXECUTOR_PLUGINS_FRAMEWORK_SOCPERF_EXECUTOR_PLUGIN_INCLUDE_SOCPERF_COMMON_H
#define RESSCHED_EXECUTOR_PLUGINS_FRAMEWORK_SOCPERF_EXECUTOR_PLUGIN_INCLUDE_SOCPERF_COMMON_H

#include <climits>
#include <list>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include "socperf_log.h"

namespace OHOS {
namespace ResourceSchedule {

const std::string SOCPERF_RESOURCE_CONFIG_XML = "etc/soc_perf/socperf_resource_config.xml";
const int32_t INVALID_VALUE                       = INT_MIN;
const int32_t MIN_RESOURCE_ID                     = 1000;
const int32_t MAX_RESOURCE_ID                     = 5999;
const int32_t RES_ID_NUMS_PER_TYPE                = 1000;
const int32_t RES_ID_NUMS_PER_TYPE_EXT            = 10000;
const int32_t WRITE_NODE                          = 0;
const int32_t REPORT_TO_PERFSO                    = 1;
const int32_t MAX_FREQUE_NODE                     = 1;
const int32_t NODE_DEFAULT_VALUE                  = -1;

class ResourceNode {
public:
    int32_t id;
    std::string name;
    int64_t def;
    std::unordered_set<int64_t> available;
    int32_t persistMode;
    bool isGov;
    bool isMaxValue;
public:
    ResourceNode(int32_t id, const std::string& name, int32_t persistMode, bool isGov, bool isMaxValue) : id(id),
        name(name), def(INVALID_VALUE), persistMode(persistMode), isGov(isGov), isMaxValue(isMaxValue) {}
    virtual ~ResourceNode() {};
    virtual void PrintString() {};
};

class ResNode : public ResourceNode {
public:
    std::string path;
    int32_t pair;

public:
    ResNode(int32_t resId, const std::string& resName, int32_t resMode, int32_t resPair, int32_t resPersistMode)
        : ResourceNode(resId, resName, resPersistMode, false, resMode == MAX_FREQUE_NODE)
    {
        pair = resPair;
    }
    ~ResNode() {}

    void PrintString()
    {
        SOC_PERF_LOGD("resNode-> id: [%{public}d], name: [%{public}s]", id, name.c_str());
        SOC_PERF_LOGD("          path: [%{public}s]", path.c_str());
        SOC_PERF_LOGD("          def: [%{public}lld], mode: [%{public}d], pair: [%{public}d]",
            (long long)def, isMaxValue, pair);
        std::string str;
        str.append("available(").append(std::to_string((int32_t)available.size())).append("): ");
        str.append("[");
        for (auto validValue : available) {
            str.append(std::to_string(validValue)).append(",");
        }
        if (!available.empty()) {
            str.pop_back();
        }
        str.append("]");
        SOC_PERF_LOGD("          %{public}s", str.c_str());
    }
};

class GovResNode : public ResourceNode {
public:
    std::vector<std::string> paths;
    std::unordered_map<int64_t, std::vector<std::string>> levelToStr;

public:
    GovResNode(int32_t govResId, const std::string& govResName, int32_t govPersistMode)
        : ResourceNode(govResId, govResName, govPersistMode, true, false) {}
    ~GovResNode() {}

    void PrintString()
    {
        SOC_PERF_LOGD("govResNode-> id: [%{public}d], name: [%{public}s]", id, name.c_str());
        SOC_PERF_LOGD("             def: [%{public}lld]", (long long)def);
        for (auto path : paths) {
            SOC_PERF_LOGD("             path: [%{public}s]", path.c_str());
        }
        std::string str;
        str.append("available(").append(std::to_string((int32_t)available.size())).append("): ");
        str.append("[");
        for (auto validValue : available) {
            str.append(std::to_string(validValue)).append(",");
        }
        if (!available.empty()) {
            str.pop_back();
        }
        str.append("]");
        SOC_PERF_LOGD("             %{public}s", str.c_str());
        for (auto iter = levelToStr.begin(); iter != levelToStr.end(); ++iter) {
            std::string str2;
            int64_t level = iter->first;
            std::vector<std::string> result = iter->second;
            for (int32_t i = 0; i < (int32_t)result.size(); i++) {
                str2.append(result[i]).append(",");
            }
            if (!result.empty()) {
                str2.pop_back();
            }
            SOC_PERF_LOGD("             %{public}lld: [%{public}s]", (long long)level, str2.c_str());
        }
    }
};

static inline int64_t Max(int64_t num1, int64_t num2)
{
    if (num1 >= num2) {
        return num1;
    }
    return num2;
}

static inline int64_t Max(int64_t num1, int64_t num2, int64_t num3)
{
    return Max(Max(num1, num2), num3);
}

static inline int64_t Min(int64_t num1, int64_t num2)
{
    if (num1 <= num2) {
        return num1;
    }
    return num2;
}

static inline int64_t Min(int64_t num1, int64_t num2, int64_t num3)
{
    return Min(Min(num1, num2), num3);
}

static inline bool IsNumber(const std::string& str)
{
    for (int32_t i = 0; i < (int32_t)str.size(); i++) {
        if (i == 0 && str.at(i) == '-') {
            continue;
        }
        if (str.at(i) < '0' || str.at(i) > '9') {
            return false;
        }
    }
    return true;
}

static inline bool IsValidRangeResId(int32_t id)
{
    if (id < MIN_RESOURCE_ID || id > MAX_RESOURCE_ID) {
        return false;
    }
    return true;
}

static inline bool IsValidPersistMode(int32_t persistMode)
{
    if (persistMode != WRITE_NODE && persistMode != REPORT_TO_PERFSO) {
        return false;
    }
    return true;
}

static std::vector<std::string> SplitEx(const std::string& str, const std::string& pattern)
{
    int32_t position;
    std::vector<std::string> result;
    std::string tempStr = str;
    tempStr += pattern;
    int32_t length = (int32_t)tempStr.size();
    for (int32_t i = 0; i < length; i++) {
        position = (int32_t)tempStr.find(pattern, i);
        if (position < length) {
            std::string tmp = tempStr.substr(i, position - i);
            result.push_back(tmp);
            i = position + (int32_t)pattern.size() - 1;
        }
    }
    return result;
}

static inline std::vector<std::string> Split(const std::string& str, const std::string& pattern)
{
    return SplitEx(str, pattern);
}

} // namespace ResourceSchedule
} // namespace OHOS

#endif // RESSCHED_EXECUTOR_PLUGINS_FRAMEWORK_SOCPERF_EXECUTOR_PLUGIN_INCLUDE_SOCPERF_COMMON_H
