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

#include <sys/types.h>
#include <string>
#include "hilog/log.h"
#include "nlohmann/json.hpp"
#include "refbase.h"

namespace OHOS {
namespace ResourceSchedule {
class RmsApplicationStateObserver;
#ifdef CONFIG_BGTASK_MGR
class BackgroundTaskObserver;
#endif
class WindowStateObserver;
class WindowVisibilityObserver;
class WindowUpdateStateObserver;
class Supervisor;
class CgroupAdjuster;
class CgroupEventHandler;

class SchedController {
public:
    static SchedController& GetInstance();

    void Init();
    void Deinit();
    bool SubscribeAppState();
    void UnsubscribeAppState();
    bool SubscribeBackgroundTask();
    void UnsubscribeBackgroundTask();
    void SubscribeWindowState();
    void UnsubscribeWindowState();
    void UnregisterStateObservers();
    int GetProcessGroup(pid_t pid);
    void ReportAbilityStatus(int32_t saId, const std::string& deviceId, uint32_t status);
    void DispatchResource(uint32_t resType, int64_t value, const nlohmann::json& payload);
    std::string GetBundleNameByUid(const int32_t uid);

    const inline std::shared_ptr<CgroupEventHandler> GetCgroupEventHandler() const
    {
        return cgHandler_;
    }

    const inline std::shared_ptr<Supervisor> GetSupervisor() const
    {
        return supervisor_;
    }

private:
    SchedController() = default;
    ~SchedController() = default;

    SchedController(const SchedController&) = delete;
    SchedController& operator=(const SchedController &) = delete;
    SchedController(SchedController&&) = delete;
    SchedController& operator=(SchedController&&) = delete;

    std::shared_ptr<CgroupEventHandler> cgHandler_;
    std::shared_ptr<Supervisor> supervisor_;
    sptr<RmsApplicationStateObserver> appStateObserver_;
#ifdef CONFIG_BGTASK_MGR
    std::shared_ptr<BackgroundTaskObserver> backgroundTaskObserver_;
#endif
    sptr<WindowStateObserver> windowStateObserver_;
    sptr<WindowVisibilityObserver> windowVisibilityObserver_;

    inline void InitCgroupHandler();
    inline void InitCgroupAdjuster();
    inline void InitSupervisor();
};
} // namespace ResourceSchedule
} // namespace OHOS
#endif // CGROUP_SCHED_FRAMEWORK_SCHED_CONTROLLER_INCLUDE_SCHED_CONTROLLER_H_
