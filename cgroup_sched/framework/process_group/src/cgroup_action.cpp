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
#include "cgroup_action.h"

#include "json/reader.h"
#include "json/value.h"
#include "cgroup_map.h"
#include "process_group_log.h"
#include "process_group_util.h"
#include "sched_policy.h"

namespace OHOS {
namespace ResourceSchedule {
namespace CgroupSetting {
namespace {
    static constexpr const char* CGROUP_SETTING_CONFIG_FILE = "etc/process_group/cgroup_action_config.json";
}

CgroupAction& CgroupAction::GetInstance()
{
    static CgroupAction instance;
    return instance;
}

bool CgroupAction::SetThreadSchedPolicy(int tid, SchedPolicy policy)
{
    if (!IsEnabled()) {
        return false;
    }
    return CgroupMap::GetInstance().SetThreadSchedPolicy(tid, policy, false);
}

bool CgroupAction::SetThreadGroupSchedPolicy(int tid, SchedPolicy policy)
{
    if (!IsEnabled()) {
        return false;
    }
    return  CgroupMap::GetInstance().SetThreadSchedPolicy(tid, policy, true);
}

bool CgroupAction::loadConfigFile()
{
    PGCGS_LOGI("CgroupAction::loadConfigFile loading config file");
    Json::Value jsonObjRoot;
    if (!ParseConfigFileToJsonObj(jsonObjRoot)) {
        return false;
    }
    return CgroupMap::GetInstance().loadConfigFromJsonObj(jsonObjRoot);
}

bool CgroupAction::IsEnabled()
{
    static bool enable = loadConfigFile();
    return enable;
}

int CgroupAction::GetSchedPolicy(int tid, SchedPolicy* policy)
{
    if (!IsEnabled()) {
        return -1;
    }
    std::string subgroup;
    CgroupController *controller;
    CgroupMap& instance = CgroupMap::GetInstance();
    if (instance.findFristEnableCgroupController(&controller)) {
        if (!controller->GetTaskGroup(tid, subgroup)) {
            return -1;
        }
    }
    if (subgroup.empty()) {
        *policy = SP_DEFAULT;
    } else if (subgroup == "foreground") {
        *policy = SP_FOREGROUND;
    } else if (subgroup == "background") {
        *policy = SP_BACKGROUND;
    } else if (subgroup == "top-app") {
        *policy = SP_TOP_APP;
    } else if (subgroup == "system") {
        *policy = SP_SYSTEM;
    } else {
        return -1;
    }
    return 0;
}

bool CgroupAction::ParseConfigFileToJsonObj(Json::Value& jsonObjRoot)
{
    std::string jsonString;
    if (!ReadFileToString(CGROUP_SETTING_CONFIG_FILE, jsonString)) {
        PGCGS_LOGE("ParseConfigFileToJsonObj: read config file failed");
        return false;
    }
    Json::CharReaderBuilder builder;
    std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
    std::string errorMsg;
    if (!reader->parse(&*jsonString.begin(), &*jsonString.end(), &jsonObjRoot, &errorMsg)) {
        PGCGS_LOGE("ParseConfigFileToJsonObj: parse json failed, errorMsg:%{public}s", errorMsg.c_str());
        return false;
    }
    return true;
}
} // namespace CgroupSetting
} // namespace ResourceSchedule
} // namespace OHOS
