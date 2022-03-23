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

#ifndef CGROUP_EVENT_HANDLER_H
#define CGROUP_EVENT_HANDLER_H

#include <sys/types.h>
#include "event_handler.h"
#include "supervisor.h"
#include "wm_common.h"

namespace OHOS {
namespace ResourceSchedule {
namespace {
    const uint32_t EVENT_ID_REG_APP_STATE_OBSERVER = 1;
    const uint32_t EVENT_ID_REG_BGTASK_OBSERVER = 2;
    const uint32_t DELAYED_RETRY_REGISTER_DURATION = 100;
    const uint32_t MAX_RETRY_TIMES = 100;
}

using OHOS::AppExecFwk::EventHandler;
using OHOS::AppExecFwk::EventRunner;
using OHOS::Rosen::WindowType;

class CgroupEventHandler : public EventHandler {
public:
    CgroupEventHandler(const std::shared_ptr<EventRunner> &runner);
    ~CgroupEventHandler();
    virtual void ProcessEvent(const AppExecFwk::InnerEvent::Pointer& event) override;
    void SetSupervisor(std::shared_ptr<Supervisor> supervisor);
    void HandleAbilityAdded(int32_t saId, const std::string& deviceId);
    void HandleAbilityRemoved(int32_t saId, const std::string& deviceId);
    void HandleForegroundApplicationChanged(uid_t uid, std::string bundleName, int32_t state);
    void HandleApplicationStateChanged(uid_t uid, std::string bundleName, int32_t state);
    void HandleAbilityStateChanged(uid_t uid, pid_t pid, std::string bundleName, std::string abilityName,
        uint64_t token, int32_t abilityState, int32_t abilityType);
    void HandleExtensionStateChanged(uid_t uid, pid_t pid, std::string bundleName, std::string abilityName,
        uint64_t token, int32_t extensionState, int32_t abilityType);
    void HandleProcessCreated(uid_t uid, pid_t pid, std::string bundleName);
    void HandleProcessDied(uid_t uid, pid_t pid, std::string bundleName);
    void HandleTransientTaskStart(uid_t uid, pid_t pid, std::string packageName);
    void HandleTransientTaskEnd(uid_t uid, pid_t pid, std::string packageName);
    void HandleContinuousTaskStart(uid_t uid, pid_t pid, std::string abilityName);
    void HandleContinuousTaskCancel(uid_t uid, pid_t pid, std::string abilityName);
    void HandleWindowFocusChange(int32_t windowId, int32_t displayId, WindowType windowType, uint64_t token);
    void HandleFocusedWindow(uint32_t windowId, uint64_t abilityToken,
        WindowType windowType, uint64_t displayId, int32_t pid, int32_t uid);
    void HandleUnfocusedWindow(uint32_t windowId, uint64_t abilityToken,
        WindowType windowType, uint64_t displayId, int32_t pid, int32_t uid);
    void HandleWindowVisibilityChanged(uint32_t windowId, bool isVisible, int32_t pid, int32_t uid);
private:
    std::shared_ptr<Supervisor> supervisor_;
};
} // namespace ResourceSchedule
} // namespace OHOS

#endif // CGROUP_EVENT_HANDLER_H
