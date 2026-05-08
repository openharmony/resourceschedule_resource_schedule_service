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

#include "utils.h"
#include "common_define.h"

namespace OHOS {
namespace ResourceSchedule {

void Utils::AddKeyAppName(const std::string& subValue, std::set<std::string>& keyAppName)
{
    if (subValue.empty()) {
        return;
    }
    keyAppName.insert(subValue);
}

void Utils::AddKeyAppType(const std::string& subValue, std::set<int32_t>& keyAppType)
{
    if (subValue.empty()) {
        return;
    }
    keyAppType.insert(atoi(subValue.c_str()));
}

std::string Utils::GetPolicyMode(const PluginConfig& socperfPolicy)
{
    std::string policyMode;
    for (const Item& item : socperfPolicy.itemList) {
        for (const SubItem& sub : item.subItemList) {
            if (sub.name == SOCPERF_POLICY_MODE) {
                policyMode = sub.value;
            }
        }
    }
    return policyMode;
}

void Utils::ApplyPolicyLimits(const PluginConfig& limitPolicy, const std::string& policyMode,
    std::optional<bool>& powerLimit, std::optional<bool>& thermalLimit)
{
    auto it = std::find_if(limitPolicy.itemList.begin(), limitPolicy.itemList.end(), [&policyMode](const Item& item) {
        auto tempIt = item.itemProperties.find("key");
        if (tempIt == item.itemProperties.end()) {
            return false;
        }
        return tempIt->second == policyMode;
    });
    if (it == limitPolicy.itemList.end()) {
        return;
    }

    const auto& itemList = it->subItemList;
    for (const auto& item : itemList) {
        if (item.name == "power_limit") {
            powerLimit = static_cast<bool>(atoi(item.value.c_str()));
        }
        if (item.name == "thermal_limit") {
            thermalLimit = static_cast<bool>(atoi(item.value.c_str()));
        }
    }
}

} // namespace ResourceSchedule
} // namespace OHOS