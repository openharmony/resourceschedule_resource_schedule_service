/*
 * Copyright (c) 2026-2026 Huawei Device Co., Ltd.
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

#ifndef RESSCHED_PLUGINS_SOCPERF_PLUGIN_INCLUDE_UTILS_H
#define RESSCHED_PLUGINS_SOCPERF_PLUGIN_INCLUDE_UTILS_H

#include <string>
#include <set>
#include <optional>
#include "config_info.h"

namespace OHOS {
namespace ResourceSchedule {

class Utils {
public:
    static void AddKeyAppName(const std::string& subValue, std::set<std::string>& keyAppName);
    static void AddKeyAppType(const std::string& subValue, std::set<int32_t>& keyAppType);
    static std::string GetPolicyMode(const PluginConfig& socperfPolicy);
    static void ApplyPolicyLimits(const PluginConfig& limitPolicy, const std::string& policyMode,
        std::optional<bool>& powerLimit, std::optional<bool>& thermalLimit);
};

} // namespace ResourceSchedule
} // namespace OHOS

#endif // RESSCHED_PLUGINS_SOCPERF_PLUGIN_INCLUDE_UTILS_H