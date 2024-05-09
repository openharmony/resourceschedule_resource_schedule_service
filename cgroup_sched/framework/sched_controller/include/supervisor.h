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

#ifndef CGROUP_SCHED_FRAMEWORK_SCHED_CONTROLLER_INCLUDE_SUPERVISOR_H_
#define CGROUP_SCHED_FRAMEWORK_SCHED_CONTROLLER_INCLUDE_SUPERVISOR_H_

#include <iostream>
#include <map>
#include <sys/types.h>
#include <string>
#include <vector>
#include <set>
#include <map>

#include "sched_policy.h"

namespace OHOS {
namespace ResourceSchedule {
using OHOS::ResourceSchedule::CgroupSetting::SchedPolicy;
using OHOS::ResourceSchedule::CgroupSetting::SP_DEFAULT;
using OHOS::ResourceSchedule::CgroupSetting::SP_BACKGROUND;
using OHOS::ResourceSchedule::CgroupSetting::SP_FOREGROUND;
using OHOS::ResourceSchedule::CgroupSetting::SP_SYSTEM_BACKGROUND;
using OHOS::ResourceSchedule::CgroupSetting::SP_TOP_APP;
using OHOS::ResourceSchedule::CgroupSetting::SP_UPPER_LIMIT;

class AbilityInfo;
class WindowInfo {
public:
    explicit WindowInfo(int32_t windowId) : windowId_(windowId) {}
    ~WindowInfo()
    {
        ability_ = nullptr;
    }

    uint32_t windowId_;
    uint32_t visibilityState_ = 2;
    bool isVisible_ = false;
    bool isFocused_ = false;
    bool drawingContentState_ = false;
    int32_t windowType_ = 0;
    uint64_t displayId_ = 0;
    std::shared_ptr<AbilityInfo> ability_ = nullptr;
    // webview app corresponding with top tab page in this window
    uid_t topWebviewRenderUid_ = 0;
};

class AbilityInfo {
public:
    AbilityInfo(uintptr_t token) : token_(token) {}
    ~AbilityInfo() {}

    int32_t state_ = -1; // normal ability state
    int32_t estate_ = -1; // extension state
    int32_t type_ = -1; // ability type
    uintptr_t token_ = 0;
    std::string name_;
};

class ProcessRecord {
public:
    ProcessRecord(uid_t uid, pid_t pid) : uid_(uid), pid_(pid) {}
    ~ProcessRecord()
    {
        abilities_.clear();
        windows_.clear();
    };

    std::shared_ptr<AbilityInfo> GetAbilityInfoNonNull(uintptr_t token);
    std::shared_ptr<AbilityInfo> GetAbilityInfo(uintptr_t token);
    std::shared_ptr<WindowInfo> GetWindowInfoNonNull(uint32_t windowId);
    void RemoveAbilityByToken(uintptr_t token);
    bool HasAbility(uintptr_t token) const;
    bool HasServiceExtension() const;
    bool IsVisible() const;
    std::set<int32_t> GetKeyTidSetByRole(int64_t role);

    inline pid_t GetPid() const
    {
        return pid_;
    }

    inline uid_t GetUid() const
    {
        return uid_;
    }

    SchedPolicy lastSchedGroup_ = SP_UPPER_LIMIT;
    SchedPolicy curSchedGroup_ = SP_UPPER_LIMIT;
    SchedPolicy setSchedGroup_ = SP_UPPER_LIMIT;
    bool isRenderProcess_ = false;
    bool runningTransientTask_ = false;
    bool isActive_ {false};
    bool inSelfRenderCgroup_ = false;
    bool isExtensionProcess_ = false;
    bool isNapState_ = false;
    bool processDrawingState_ = false;
    bool screenCaptureState_ = false;
    bool videoState_ = false;

    uint32_t continuousTaskFlag_ = 0;
    int32_t audioPlayingState_ = -1;
    int32_t renderTid_ = 0;
    int32_t processState_ = 0;
    int32_t linkedWindowId_ {-1};
    int32_t serialNum_ {-1};
    int32_t extensionType_ = -1;
    int32_t cameraState_ = -1;
    int32_t bluetoothState_ = -1;
    int32_t wifiState_ = -1;
    int32_t mmiStatus_ {-1};

    std::map<uint32_t, bool> runningLockState_;
    std::map<int32_t, bool> avCodecState_;
    std::unordered_map<int32_t, std::vector<uint32_t>> abilityIdAndContinuousTaskFlagMap_;
    std::vector<std::shared_ptr<AbilityInfo>> abilities_;
    std::vector<std::shared_ptr<WindowInfo>> windows_;

    std::map<int32_t, int32_t> keyThreadRoleMap_ {}; // items in keyThreadMap_ is (tid, role)

    std::unordered_map<int32_t, SchedPolicy> specialSchedThread_;
private:
    uid_t uid_;
    pid_t pid_;
};

class Application {
public:
    Application(uid_t uid) : uid_(uid) {}
    ~Application() = default;

    std::shared_ptr<ProcessRecord> AddProcessRecord(std::shared_ptr<ProcessRecord> pr);
    void RemoveProcessRecord(pid_t pid);
    std::shared_ptr<ProcessRecord> GetProcessRecord(pid_t pid);
    std::shared_ptr<ProcessRecord> GetProcessRecordNonNull(pid_t pid);
    std::shared_ptr<ProcessRecord> FindProcessRecordByToken(uintptr_t token);
    std::shared_ptr<ProcessRecord> FindProcessRecordByWindowId(uint32_t windowId);
    void SetName(const std::string& name);
    void SetMainProcess(std::shared_ptr<ProcessRecord> pr);

    inline uid_t GetUid() const
    {
        return uid_;
    }

    inline std::map<pid_t, std::shared_ptr<ProcessRecord>> GetPidsMap() const
    {
        return pidsMap_;
    }

    std::shared_ptr<ProcessRecord> GetMainProcessRecord() const
    {
        return mainProcess_;
    }

    const std::string& GetName() const
    {
        return name_;
    }

    int32_t state_ = -1;
    std::shared_ptr<ProcessRecord> focusedProcess_ = nullptr;
    SchedPolicy lastSchedGroup_ = SP_UPPER_LIMIT;
    SchedPolicy curSchedGroup_ = SP_UPPER_LIMIT;
    SchedPolicy setSchedGroup_ = SP_UPPER_LIMIT;

private:
    uid_t uid_;
    std::string name_;
    std::shared_ptr<ProcessRecord> mainProcess_ = nullptr;
    std::map<pid_t, std::shared_ptr<ProcessRecord>> pidsMap_;
};

class Supervisor {
public:
    std::shared_ptr<Application> GetAppRecord(int32_t uid);
    std::shared_ptr<Application> GetAppRecordNonNull(int32_t uid);
    std::shared_ptr<ProcessRecord> FindProcessRecord(pid_t pid);
    void RemoveApplication(int32_t uid);
    void SearchAbilityToken(std::shared_ptr<Application> &app, std::shared_ptr<ProcessRecord> &procRecord,
        uintptr_t token);
    void SearchWindowId(std::shared_ptr<Application> &application, std::shared_ptr<ProcessRecord> &procRecord,
        uint32_t windowId);
    void SetSystemLoadLevelState(int32_t level);
    int32_t GetSystemLoadLevel();

    int32_t sceneBoardPid_ = -1;
    std::shared_ptr<Application> focusedApp_ = nullptr;

    inline std::map<int32_t, std::shared_ptr<Application>> GetUidsMap() const
    {
        return uidsMap_;
    }

private:
    std::map<int32_t, std::shared_ptr<Application>> uidsMap_;
    int32_t systemLoadLevel_ = -1;
};
} // namespace ResourceSchedule
} // namespace OHOS
#endif // CGROUP_SCHED_FRAMEWORK_SCHED_CONTROLLER_INCLUDE_SUPERVISOR_H_
