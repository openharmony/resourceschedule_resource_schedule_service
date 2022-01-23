/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef OHOS_RESOURCE_SCHEDULE_SERVICE_CGROUP_SCHED_FRAMEWORKS_PROCESS_GROUP_INCLUDE_CGROUP_ACTION_H
#define OHOS_RESOURCE_SCHEDULE_SERVICE_CGROUP_SCHED_FRAMEWORKS_PROCESS_GROUP_INCLUDE_CGROUP_ACTION_H

#include "cgroup_map.h"

namespace OHOS {
namespace ResourceSchedule {
namespace CgroupSetting {

class CgroupAction{
public:
    static CgroupAction& GetInstance();

    CgroupAction(const CgroupAction& cgroupAction) = delete;
    CgroupAction& operator= (const CgroupAction& cgroupAction) = delete;
    ~CgroupAction() = default;
    bool SetThreadSchedPolicy(int tid, SchedPolicy policy);
    bool SetThreadGroupSchedPolicy(int tid, SchedPolicy policy);
    int GetSchedPolicy(int tid, SchedPolicy* policy);

private:
    CgroupAction() = default;
    bool loadConfigFile();
    bool IsEnabled();
    static bool ParseConfigFileToJsonObj(Json::Value& jsonObjRoot);
};

} // namepsace CgroupSetting
} // namespace ResourceSchedule
} // namespace OHOS
#endif

