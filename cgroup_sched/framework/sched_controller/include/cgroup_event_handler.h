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

#ifndef CGROUP_EVENT_HANDLER_H
#define CGROUP_EVENT_HANDLER_H

#include <sys/types.h>
#include "event_handler.h"
#include "supervisor.h"
#include "wm_common.h"

namespace OHOS {
namespace ResourceSchedule {
namespace {
    const int INNER_EVENT_ID_REG_STATE_OBSERVERS = 0;
    const int DELAYED_REGISTER_DURATION = 5000;
    const int DELAYED_RETRY_REGISTER_DURATION = 2000;
    const int MAX_RETRY_TIMES = 50;
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
    void HandleForegroundApplicationChanged(uid_t uid, std::string bundleName, int32_t state);
    void HandleApplicationStateChanged(uid_t uid, std::string bundleName, int32_t state);
    void HandleAbilityStateChanged(uid_t uid, pid_t pid, std::string bundleName, std::string abilityName,
        sptr<IRemoteObject> token, int32_t abilityState);
    void HandleExtensionStateChanged(uid_t uid, pid_t pid, std::string bundleName, std::string abilityName,
        sptr<IRemoteObject> token, int32_t extensionState);
    void HandleProcessCreated(uid_t uid, pid_t pid, std::string bundleName);
    void HandleProcessDied(uid_t uid, pid_t pid, std::string bundleName);
    void HandleTransientTaskStart(uid_t uid, pid_t pid, std::string packageName);
    void HandleTransientTaskEnd(uid_t uid, pid_t pid, std::string packageName);
    void HandleContinuousTaskStart(uid_t uid, pid_t pid, std::string abilityName);
    void HandleContinuousTaskCancel(uid_t uid, pid_t pid, std::string abilityName);
    void HandleWindowFocusChange(int32_t windowId, int32_t displayId, WindowType windowType, sptr<IRemoteObject> token);
    void HandleFocusedWindow(uint32_t windowId, sptr<IRemoteObject> abilityToken,
        WindowType windowType, int32_t displayId);
    void HandleUnfocusedWindow(uint32_t windowId, sptr<IRemoteObject> abilityToken,
        WindowType windowType, int32_t displayId);
private:
    std::shared_ptr<Supervisor> supervisor_;
};
} // namespace ResourceSchedule
} // namespace OHOS

#endif // CGROUP_EVENT_HANDLER_H
