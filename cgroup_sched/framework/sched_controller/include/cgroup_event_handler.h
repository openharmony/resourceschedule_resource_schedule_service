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

#ifndef CGROUP_SCHED_FRAMEWORK_SCHED_CONTROLLER_INCLUDE_CGROUP_EVENT_HANDLER_H_
#define CGROUP_SCHED_FRAMEWORK_SCHED_CONTROLLER_INCLUDE_CGROUP_EVENT_HANDLER_H_

#include <sys/types.h>
#include "ffrt.h"
#include "ffrt_inner.h"
#include "nlohmann/json.hpp"
#include "supervisor.h"
#include "wm_common.h"

namespace OHOS {
namespace ResourceSchedule {
using OHOS::Rosen::WindowType;

class CgroupEventHandler {
public:
    explicit CgroupEventHandler(const std::string &queueName);
    ~CgroupEventHandler();
    void ProcessEvent(uint32_t eventId, int64_t eventParam);
    void SetSupervisor(std::shared_ptr<Supervisor> supervisor);
    void HandleAbilityAdded(int32_t saId, const std::string& deviceId);
    void HandleAbilityRemoved(int32_t saId, const std::string& deviceId);
    void HandleApplicationStateChanged(uid_t uid, pid_t pid, const std::string& bundleName, int32_t state);
    void HandleProcessStateChanged(uid_t uid, pid_t pid, const std::string& bundleName, int32_t state);
    void HandleAbilityStateChanged(uid_t uid, pid_t pid, const std::string& bundleName,
        const std::string& abilityName, uintptr_t token, int32_t abilityState, int32_t abilityType);
    void HandleExtensionStateChanged(uid_t uid, pid_t pid, const std::string& bundleName,
        const std::string& abilityName, uintptr_t token, int32_t extensionState, int32_t abilityType);
    void HandleProcessCreated(uid_t uid, pid_t pid, int32_t hostPid, int32_t processType,
        const std::string& bundleName, int32_t extensionType);
    void HandleProcessDied(uid_t uid, pid_t pid, const std::string& bundleName);
    void HandleTransientTaskStart(uid_t uid, pid_t pid, const std::string& packageName);
    void HandleTransientTaskEnd(uid_t uid, pid_t pid, const std::string& packageName);
    void HandleContinuousTaskCancel(uid_t uid, pid_t pid, int32_t typeId, int32_t abilityId);
    void HandleContinuousTaskUpdate(
        uid_t uid, pid_t pid, const std::vector<uint32_t>& typeIds, int32_t abilityId);
    void HandleFocusedWindow(uint32_t windowId, uintptr_t abilityToken,
        WindowType windowType, uint64_t displayId, int32_t pid, int32_t uid);
    void HandleUnfocusedWindow(uint32_t windowId, uintptr_t abilityToken,
        WindowType windowType, uint64_t displayId, int32_t pid, int32_t uid);
    void HandleWindowVisibilityChanged(uint32_t windowId, uint32_t visibilityState,
        WindowType windowType, int32_t pid, int32_t uid);
    void HandleDrawingContentChangeWindow(uint32_t windowId, WindowType windowType,
        bool drawingContentState, int32_t pid, int32_t uid);
    void HandleReportMMIProcess(uint32_t resType, int64_t value, const nlohmann::json& payload);
    void HandleReportRenderThread(uint32_t resType, int64_t value, const nlohmann::json& payload);
    void HandleReportKeyThread(uint32_t resType, int64_t value, const nlohmann::json& payload);
    void HandleReportWindowState(uint32_t resType, int64_t value, const nlohmann::json& payload);
    void HandleReportAudioState(uint32_t resType, int64_t value, const nlohmann::json& payload);
    void HandleReportWebviewAudioState(uint32_t resType, int64_t value, const nlohmann::json& payload);
    void HandleReportRunningLockEvent(uint32_t resType, int64_t value, const nlohmann::json& payload);
    void HandleReportHisysEvent(uint32_t resType, int64_t value, const nlohmann::json& payload);
    void HandleReportAvCodecEvent(uint32_t resType, int64_t value, const nlohmann::json& payload);
    void HandleSceneBoardState(uint32_t resType, int64_t value, const nlohmann::json& payload);
    void HandleWebviewScreenCapture(uint32_t resType, int64_t value, const nlohmann::json& payload);
    void HandleReportScreenCaptureEvent(uint32_t resType, int64_t value, const nlohmann::json& payload);
    void HandleReportWebviewVideoState(uint32_t resType, int64_t value, const nlohmann::json& payload);
    void PostTask(const std::function<void()> task);
    void PostTask(const std::function<void()> task, const std::string &taskName, const int32_t delayTime);
    void RemoveTask(const std::string &taskName);

private:
    bool CheckVisibilityForRenderProcess(ProcessRecord &pr, ProcessRecord &mainProc);
    bool GetProcInfoByPayload(int32_t &uid, int32_t &pid, std::shared_ptr<Application>& app,
        std::shared_ptr<ProcessRecord>& procRecord, const nlohmann::json& payload);
    bool ParsePayload(int32_t& uid, int32_t& pid, const nlohmann::json& payload);
    bool ParsePayload(int32_t& uid, int32_t& pid, int32_t& tid, int64_t value, const nlohmann::json& payload);
    bool ParseValue(int32_t& value, const char* name, const nlohmann::json& payload);
    void UpdateActivepWebRenderInfo(int32_t uid, int32_t pid, int32_t windowId, int32_t state,
        const std::shared_ptr<ProcessRecord>& proc);
    std::shared_ptr<Supervisor> supervisor_;
    std::shared_ptr<ffrt::queue> cgroupEventQueue_;
    std::unordered_map<std::string, ffrt::task_handle> delayTaskMap_;
    ffrt::mutex delayTaskMapMutex_;
    int32_t ffrtSwitch_ = 1000; // delayTime transfer to ffrt
};
} // namespace ResourceSchedule
} // namespace OHOS

#endif // CGROUP_SCHED_FRAMEWORK_SCHED_CONTROLLER_INCLUDE_CGROUP_EVENT_HANDLER_H_
