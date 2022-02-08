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

#ifndef SCHED_CONTROLLER_H
#define SCHED_CONTROLLER_H

#include <sys/types.h>
#include <string>
#include "hilog/log.h"

#include "window_state_observer.h"

namespace OHOS {
namespace ResourceSchedule {
class RmsApplicationStateObserver;
class TransientTaskObserver;
class ContinuousTaskObserver;
class Supervisor;
class Application;
class ProcessRecord;
class CgroupAdjuster;
class CgroupEventHandler;
enum class AdjustSource;

class SchedController {
public:
    static SchedController& GetInstance();

    void Init();
    void Deinit();
    void RegisterStateObservers();
    void UnregisterStateObservers();
    void AdjustProcessGroup(Application &app, ProcessRecord &pr, AdjustSource source);
    void AdjustAllProcessGroup(Application &app, AdjustSource source);
    int GetProcessGroup(pid_t pid);

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
    std::shared_ptr<CgroupAdjuster> cgAdjuster_;
    std::shared_ptr<Supervisor> supervisor_;
    std::shared_ptr<RmsApplicationStateObserver> appStateObserver_;
    std::shared_ptr<TransientTaskObserver> transientTaskObserver_;
    std::shared_ptr<ContinuousTaskObserver> continuousTaskObserver_;
    sptr<WindowStateObserver> windowStateObserver_;

    inline void InitCgroupHandler();
    inline void InitCgroupAdjuster();
    inline void InitSupervisor();
    inline void SubscribeAppState();
    inline void SubscribeTransientTask();
    inline void SubscribeContinuousTask();
    inline void SubscribeWindowState();
};
} // namespace ResourceSchedule
} // namespace OHOS
#endif // SCHED_CONTROLLER_H
