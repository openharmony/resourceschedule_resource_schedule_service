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
#include "cgroup_map.h"
#include <cstdint>              // for uint32_t
#include <string>               // for basic_string, operator<
#include <cstring>              // for strcmp
#include <type_traits>          // for remove_reference<>::type, move
#include <utility>              // for pair, make_pair
#include <vector>               // for vector
#include "cgroup_action.h"      // for CgroupAction
#include "process_group_log.h"  // for PGCGS_LOGE, PGCGS_LOGD, PGCGS_LOGI

namespace OHOS {
namespace ResourceSchedule {
namespace CgroupSetting {
namespace {
    constexpr const char * const JSON_KEY_CGROUPS = "Cgroups";
    constexpr const char * const JSON_KEY_CONTROLLER = "controller";
    constexpr const char * const JSON_KEY_PATH = "path";
    constexpr const char * const JSON_KEY_SCHED_POLICY = "sched_policy";
}

CgroupMap& CgroupMap::GetInstance()
{
    static CgroupMap instance;
    return instance;
}

bool CgroupMap::SetThreadSchedPolicy(int tid, SchedPolicy policy, bool isThreadGroup)
{
    for (auto it = controllers_.begin(); it != controllers_.end(); ++it) {
        CgroupController& controller = it -> second;
        if (controller.IsEnabled()) {
            if (!controller.SetThreadSchedPolicy(tid, policy, isThreadGroup)) {
                PGCGS_LOGD("%{public}s failed, controller = %{public}s, policy = %{public}d.",
                    __func__, controller.GetName().c_str(), policy);
            }
        }
    }
    return true;
}

void CgroupMap::AddCgroupController(const std::string& name, CgroupController& controller)
{
    controllers_.insert(std::make_pair(name, std::move(controller)));
}

bool CgroupMap::LoadConfigFromJsonObj(const nlohmann::json& jsonObj)
{
    const nlohmann::json& jsonArrObj = jsonObj[JSON_KEY_CGROUPS];
    // check json format
    if (!jsonArrObj.is_array()) {
        PGCGS_LOGE("%{public}s json config format error, CgroupMap: disabled!", __func__);
        return false;
    }
    int count = 0;
    int32_t objSize = static_cast<int32_t>(jsonArrObj.size());
    for (int32_t i = 0; i < objSize; ++i) {
        const nlohmann::json& cgroupObj = jsonArrObj[i];
        const nlohmann::json& nameObj = cgroupObj[JSON_KEY_CONTROLLER];
        const nlohmann::json& pathObj = cgroupObj[JSON_KEY_PATH];
        if (!nameObj.is_string() || !pathObj.is_string()) {
            PGCGS_LOGE("%{public}s invalid controller config.", __func__);
            continue;
        }
        std::string name = nameObj.get<std::string>();
        std::string rootPath = pathObj.get<std::string>();
        if (name.empty() || rootPath.empty()) {
            PGCGS_LOGE("%{public}s empty controller config.", __func__);
            continue;
        }

        CgroupController controller(name, rootPath);
        if (LoadSchedPolicyConfig(controller, cgroupObj[JSON_KEY_SCHED_POLICY])) {
            this->AddCgroupController(name, controller);
            count++;
        } else {
            PGCGS_LOGE("%{public}s no valid policy config item.", __func__);
        }
    }

    if (count == 0) {
        PGCGS_LOGW("%{public}s The number of valid cgroup config is 0, CgroupMap: disabled!", __func__);
        return false;
    }
    PGCGS_LOGI("%{public}s CgroupMap: enabled!", __func__);
    return true;
}

bool CgroupMap::LoadSchedPolicyConfig(CgroupController& controller, const nlohmann::json& policyObj)
{
    if (!policyObj.is_object()) {
        PGCGS_LOGE("%{public}s invalid policy config.", __func__);
        return false;
    }

    auto policyList = CgroupAction::GetInstance().GetSchedPolicyList();
    uint32_t count = 0;
    for (SchedPolicy policy : policyList) {
        const char* keyString = GetSchedPolicyFullName(policy);
        if (!keyString || !strcmp(keyString, "error")) {
            continue;
        }
        const nlohmann::json& obj = policyObj[keyString];
        if (!obj.is_string()) {
            PGCGS_LOGE("%{public}s %s is not properly configed.", __func__, keyString);
            continue;
        }
        if (controller.AddSchedPolicy(policy, obj.get<std::string>())) {
            count++;
        }
    }
    return (count > 0);
}

bool CgroupMap::FindFristEnableCgroupController(CgroupController** p)
{
    for (auto it = controllers_.begin(); it != controllers_.end(); ++it) {
        if (it->second.IsEnabled()) {
            *p = &(it->second);
            return true;
        }
    }
    return false;
}
} // namespace CgroupSetting
} // namespace ResourceSchedule
} // namespace OHOS
