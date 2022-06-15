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

#include "cgroup_action.h"
#include "process_group_log.h"
#include "process_group_util.h"

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
                PGCGS_LOGD("SetThreadSchedPolicy failed, controller = %{public}s, policy = %{public}d.",
                    controller.GetName().c_str(), policy);
            }
        }
    }
    return true;
}

void CgroupMap::AddCgroupController(const std::string& name, CgroupController& controller)
{
    controllers_.insert(std::make_pair(name, std::move(controller)));
}

bool CgroupMap::LoadConfigFromJsonObj(const Json::Value& jsonObj)
{
    const Json::Value& jsonArrObj = jsonObj[JSON_KEY_CGROUPS];
    // check json format
    if (!jsonArrObj.isArray()) {
        PGCGS_LOGE("Cgroups json config format error, CgroupMap: disabled!.");
        return false;
    }
    int count = 0;
    for (Json::Value::ArrayIndex i = 0; i < jsonArrObj.size(); ++i) {
        const Json::Value& cgroupObj = jsonArrObj[i];
        const Json::Value& nameObj = cgroupObj[JSON_KEY_CONTROLLER];
        const Json::Value& pathObj = cgroupObj[JSON_KEY_PATH];
        if (!nameObj.isString() || !pathObj.isString()) {
            PGCGS_LOGE("invalid controller config.");
            continue;
        }
        std::string name = nameObj.asString();
        std::string rootPath = pathObj.asString();
        if (name.empty() || rootPath.empty()) {
            PGCGS_LOGE("empty controller config.");
            continue;
        }

        CgroupController controller(name, rootPath);
        if (LoadSchedPolicyConfig(controller, cgroupObj[JSON_KEY_SCHED_POLICY])) {
            this->AddCgroupController(name, controller);
            count++;
        } else {
            PGCGS_LOGE("no valid policy config item.");
        }
    }

    if (count == 0) {
        PGCGS_LOGW("The number of valid cgroup config is 0, CgroupMap: disabled!");
        return false;
    }
    PGCGS_LOGI("CgroupMap: enabled!");
    return true;
}

bool CgroupMap::LoadSchedPolicyConfig(CgroupController& controller, const Json::Value& policyObj)
{
    if (!policyObj.isObject()) {
        PGCGS_LOGE("invalid policy config.");
        return false;
    }

    auto policyList = CgroupAction::GetInstance().GetSchedPolicyList();
    uint32_t count = 0;
    for (SchedPolicy policy : policyList) {
        const char* keyString = GetSchedPolicyFullName(policy);
        if (!keyString || !strcmp(keyString, "error")) {
            continue;
        }
        const Json::Value& obj = policyObj[keyString];
        if (!obj.isString()) {
            PGCGS_LOGE("%s is not properly configed.", keyString);
            continue;
        }
        if (controller.AddSchedPolicy(policy, obj.asString())) {
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
