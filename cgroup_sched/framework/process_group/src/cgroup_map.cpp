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

bool CgroupMap::loadConfigFromJsonObj(const Json::Value& jsonObj)
{
    const Json::Value& jsonArrObj = jsonObj[JSON_KEY_CGROUPS];
    // check json format
    if (jsonArrObj.isNull() || !jsonArrObj.isArray()) {
        PGCGS_LOGI("Cgroups json config format error, CgroupMap: disabled!.");
        return false;
    }
    int count = 0;
    for (Json::Value::ArrayIndex i = 0; i < jsonArrObj.size(); ++i) {
        const Json::Value& cgroupObj = jsonArrObj[i];
        // check cgroup schedule policy json config format
        if (!CheckCgroupJsonConfig(cgroupObj)) {
            const std::string cgroupObjStr = JsonToString(cgroupObj);
            PGCGS_LOGE("cgroup json config format error, ingore it: %{public}s", cgroupObjStr.c_str());
            continue;
        }

        std::string name = cgroupObj[JSON_KEY_CONTROLLER].asString();
        std::string rootPath =  cgroupObj[JSON_KEY_PATH].asString();
        const Json::Value& schedPolicyJsonObj = cgroupObj[JSON_KEY_SCHED_POLICY];
        CgroupController controller(name, rootPath);

        for (int i = 0; i < SP_CNT; i++) {
            SchedPolicy policy = SchedPolicy(i);
            const char* keyString = GetSchedPolicyFullName(policy);
            std::string relPath = schedPolicyJsonObj[keyString].asString();
            controller.AddSchedPolicy(policy, relPath);
        }
        this->AddCgroupController(name, controller);
        count++;
    }
    if (count == 0) {
        PGCGS_LOGI("The number of valid cgroup config is 0, CgroupMap: disabled!");
        return false;
    }
    PGCGS_LOGI("CgroupMap: enabled!");
    return true;
}

bool CgroupMap::CheckCgroupJsonConfig(const Json::Value& cgroupObj)
{
    if (cgroupObj[JSON_KEY_CONTROLLER].isNull() || !cgroupObj[JSON_KEY_CONTROLLER].isString()
        || cgroupObj[JSON_KEY_PATH].isNull() || !cgroupObj[JSON_KEY_PATH].isString()
        || cgroupObj[JSON_KEY_SCHED_POLICY].isNull() || !cgroupObj[JSON_KEY_SCHED_POLICY].isObject()) {
        return false;
    }
    for (int i = SP_DEFAULT; i < SP_CNT; i++) {
        SchedPolicy policy = SchedPolicy(i);
        const char* string = GetSchedPolicyFullName(policy);
        if (!strcmp(string, "error")) {
            return false;
        }
        const Json::Value& jsonObj = cgroupObj[JSON_KEY_SCHED_POLICY][string];
        if (jsonObj.isNull() || !jsonObj.isString()) {
            return false;
        }
    }
    return true;
}

bool CgroupMap::findFristEnableCgroupController(CgroupController** p)
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