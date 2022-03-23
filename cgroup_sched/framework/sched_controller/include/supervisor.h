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

#ifndef SUPERVISOR_H
#define SUPERVISOR_H

#include <iostream>
#include <map>
#include <sys/types.h>
#include <string>

#include "iremote_object.h"
#include "sched_policy.h"
#include "app_mgr_constants.h"

namespace OHOS {
namespace ResourceSchedule {
using OHOS::AppExecFwk::ApplicationState;
using OHOS::AppExecFwk::AbilityState;
using OHOS::AppExecFwk::ExtensionState;
using OHOS::ResourceSchedule::CgroupSetting::SchedPolicy;

class AbilityInfo;
class WindowInfo {
public:
    WindowInfo(int32_t windowId) : windowId_(windowId) {};
    ~WindowInfo() {};

    uint32_t windowId_;
    bool isVisible_ = false;
    bool isFocused_ = false;
    int32_t windowType_ = 0;
    uint64_t displayId_ = -1ULL;
    std::shared_ptr<AbilityInfo> ability_ = nullptr;
};

class AbilityInfo {
public:
    AbilityInfo(uint64_t token) : token_(token) {};
    ~AbilityInfo() = default;

    int32_t state_ = -1; // normal ability state
    int32_t estate_ = -1; // extension state
    int32_t type_ = -1; // ability type
    uint64_t token_ = 0;
    std::shared_ptr<WindowInfo> window_ = nullptr;
};

class ProcessRecord {
public:
    ProcessRecord(uid_t uid, pid_t pid) : uid_(uid), pid_(pid) {};
    ~ProcessRecord()
    {
        abilities_.clear();
        windows_.clear();
    };

    std::shared_ptr<AbilityInfo> GetAbilityInfoNonNull(uint64_t token);
    std::shared_ptr<AbilityInfo> GetAbilityInfo(uint64_t token);
    std::shared_ptr<WindowInfo> GetWindowInfoNonNull(uint32_t windowId);
    void RemoveAbilityByToken(uint64_t token);
    bool HasAbility(uint64_t token) const;
    bool HasServiceExtension() const;
    bool IsVisible() const;

    inline pid_t GetPid() const
    {
        return pid_;
    }

    inline uid_t GetUid() const
    {
        return uid_;
    }

    SchedPolicy lastSchedGroup_ = SchedPolicy::SP_DEFAULT;
    SchedPolicy curSchedGroup_ = SchedPolicy::SP_DEFAULT;
    SchedPolicy setSchedGroup_ = SchedPolicy::SP_DEFAULT;
    bool runningTransientTask_ = false;
    bool runningContinuousTask_ = false;

    std::vector<std::shared_ptr<AbilityInfo>> abilities_;
    std::vector<std::shared_ptr<WindowInfo>> windows_;
private:
    uid_t uid_;
    pid_t pid_;
};

class Application {
public:
    Application(uid_t uid) : uid_(uid) {};
    ~Application() = default;

    std::shared_ptr<ProcessRecord> AddProcessRecord(std::shared_ptr<ProcessRecord> pr);
    void RemoveProcessRecord(pid_t pid);
    std::shared_ptr<ProcessRecord> GetProcessRecord(pid_t pid);
    std::shared_ptr<ProcessRecord> GetProcessRecordNonNull(pid_t pid);
    std::shared_ptr<ProcessRecord> FindProcessRecord(uint64_t token);
    std::shared_ptr<ProcessRecord> FindProcessRecord(uint32_t windowId);

    inline uid_t GetUid() const
    {
        return uid_;
    }

    inline std::map<pid_t, std::shared_ptr<ProcessRecord>> GetPidsMap() const
    {
        return pidsMap_;
    }

    std::string name_;
    std::shared_ptr<ProcessRecord> focusedProcess_ = nullptr;
    SchedPolicy lastSchedGroup_ = SchedPolicy::SP_DEFAULT;
    SchedPolicy curSchedGroup_ = SchedPolicy::SP_DEFAULT;
    SchedPolicy setSchedGroup_ = SchedPolicy::SP_DEFAULT;
    int32_t state_ = static_cast<int32_t>(ApplicationState::APP_STATE_TERMINATED);

private:
    uid_t uid_;
    std::map<pid_t, std::shared_ptr<ProcessRecord>> pidsMap_;
};

class Supervisor {
public:
    std::shared_ptr<Application> GetAppRecord(int32_t uid);
    std::shared_ptr<Application> GetAppRecordNonNull(int32_t uid);
    std::shared_ptr<ProcessRecord> FindProcessRecord(pid_t pid);
    void RemoveApplication(int32_t uid);
    void SearchAbilityToken(std::shared_ptr<Application> &app, std::shared_ptr<ProcessRecord> &procRecord,
        uint64_t token);
    void SearchWindowId(std::shared_ptr<Application> &application, std::shared_ptr<ProcessRecord> &procRecord,
        uint32_t windowId);

    std::shared_ptr<Application> focusedApp_ = nullptr;
private:
    std::map<int32_t, std::shared_ptr<Application>> uidsMap_;
};
} // namespace ResourceSchedule
} // namespace OHOS
#endif // SUPERVISOR_H
