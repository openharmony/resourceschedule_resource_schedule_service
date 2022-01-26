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

class ProcessRecord {
public:
    ProcessRecord();
    ProcessRecord(uid_t uid, pid_t pid) : uid_(uid), pid_(pid) {};
    ProcessRecord(uid_t uid, pid_t pid, std::string name) : uid_(uid), pid_(pid), name_(name) {};
    ~ProcessRecord() = default;

    inline pid_t GetPid() const
    {
        return pid_;
    }

    inline uid_t GetUid() const
    {
        return uid_;
    }

    inline std::string GetName() const
    {
        return name_;
    }

    SchedPolicy lastSchedGroup_ = SchedPolicy::SP_DEFAULT;
    SchedPolicy curSchedGroup_ = SchedPolicy::SP_DEFAULT;
    SchedPolicy setSchedGroup_ = SchedPolicy::SP_DEFAULT;
    bool runningTransientTask_ = false;
    bool runningContinuousTask_ = false;
    bool focused_ = false;
    int32_t abilityState_ = static_cast<int32_t>(AbilityState::ABILITY_STATE_TERMINATED);
    int32_t extensionState_ = static_cast<int32_t>(ExtensionState::EXTENSION_STATE_TERMINATED);
    int32_t windowType_;
    sptr<IRemoteObject> token_ = nullptr;

private:
    uid_t uid_;
    pid_t pid_;
    std::string name_;
};

class Application {
public:
    Application();
    Application(uid_t uid, std::string name) : uid_(uid), name_(name) {};
    ~Application() = default;

    std::shared_ptr<ProcessRecord> AddProcessRecord(std::shared_ptr<ProcessRecord> pr);
    void RemoveProcessRecord(pid_t pid);
    std::shared_ptr<ProcessRecord> GetProcessRecord(pid_t pid);
    std::shared_ptr<ProcessRecord> GetProcessRecordNonNull(pid_t pid, std::string name);
    std::shared_ptr<ProcessRecord> FindProcessRecord(sptr<IRemoteObject> token_);

    inline uid_t GetUid() const
    {
        return uid_;
    }

    inline std::string GetName() const
    {
        return name_;
    }

    inline std::map<pid_t, std::shared_ptr<ProcessRecord>> GetPidsMap() const
    {
        return pidsMap_;
    }

    std::shared_ptr<ProcessRecord> focusedProcess_ = nullptr;
    SchedPolicy lastSchedGroup_ = SchedPolicy::SP_DEFAULT;
    SchedPolicy curSchedGroup_ = SchedPolicy::SP_DEFAULT;
    SchedPolicy setSchedGroup_ = SchedPolicy::SP_DEFAULT;
    int32_t state_ = static_cast<int32_t>(ApplicationState::APP_STATE_TERMINATED);

private:
    uid_t uid_;
    std::string name_;
    std::map<pid_t, std::shared_ptr<ProcessRecord>> pidsMap_;
};

class Supervisor {
public:
    std::shared_ptr<Application> GetAppRecord(int32_t uid);
    std::shared_ptr<Application> GetAppRecordNonNull(int32_t uid, std::string bundleName);
    std::shared_ptr<ProcessRecord> FindProcessRecord(pid_t pid);
    void SearchAbilityToken(std::shared_ptr<Application> &app, std::shared_ptr<ProcessRecord> &procRecord, sptr<IRemoteObject> token);

private:
    std::map<int32_t, std::shared_ptr<Application>> uidsMap_;
};
} // namespace ResourceSchedule
} // namespace OHOS
#endif // SUPERVISOR_H
