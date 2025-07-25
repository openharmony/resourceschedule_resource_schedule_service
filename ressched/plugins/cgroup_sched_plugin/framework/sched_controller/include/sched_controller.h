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

#ifndef CGROUP_SCHED_FRAMEWORK_SCHED_CONTROLLER_INCLUDE_SCHED_CONTROLLER_H_
#define CGROUP_SCHED_FRAMEWORK_SCHED_CONTROLLER_INCLUDE_SCHED_CONTROLLER_H_

#include <set>
#include <sys/types.h>
#include <string>
#include "nlohmann/json.hpp"
#include "plugin.h"
#include "refbase.h"
#include "single_instance.h"

namespace OHOS {
namespace ResourceSchedule {
class Supervisor;
class CgroupAdjuster;
class CgroupEventHandler;

class SchedController : public Plugin {
DECLARE_SINGLE_INSTANCE(SchedController)
public:
    void Init() override;
    void Disable() override;

    int GetProcessGroup(pid_t pid);
    void DispatchResource(const std::shared_ptr<ResData>& resData) override;
    void DispatchOtherResource(uint32_t resType, int64_t value, const nlohmann::json& payload);
    std::string GetBundleNameByUid(const int32_t uid);
    void Dump(const std::vector<std::string>& args, std::string& result);
    void DumpHelp(std::string& result);
    void DumpProcessRunningLock(std::string &result);
    void DumpProcessEventState(std::string &result);
    void DumpProcessWindowInfo(std::string &result);
#ifdef POWER_MANAGER_ENABLE
    void GetRunningLockState();
#endif

    const inline std::shared_ptr<CgroupEventHandler> GetCgroupEventHandler() const
    {
        return cgHandler_;
    }

    const inline std::shared_ptr<Supervisor> GetSupervisor() const
    {
        return supervisor_;
    }

private:
    std::shared_ptr<CgroupEventHandler> cgHandler_;
    std::shared_ptr<Supervisor> supervisor_;

    std::set<uint32_t> resTypes;
    std::unordered_map<uint32_t, std::function<void(std::shared_ptr<CgroupEventHandler>,
        uint32_t, int64_t, const nlohmann::json&)>> dispatchResFuncMap_;

    void InitResTypes();
    inline void InitCgroupHandler();
    inline void InitCgroupAdjuster();
    inline void InitSupervisor();
    inline void InitAppStartupSceneRec();
    inline void DeinitAppStartupSceneRec();
    void InitDispatchResFuncMap();
    void InitAddDispatchResFuncMap();
};
} // namespace ResourceSchedule
} // namespace OHOS
#endif // CGROUP_SCHED_FRAMEWORK_SCHED_CONTROLLER_INCLUDE_SCHED_CONTROLLER_H_
