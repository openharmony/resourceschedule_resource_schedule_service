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

#include "supervisor.h"
#include "ability_info.h"
#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"
#include "cgroup_sched_log.h"

namespace OHOS {
namespace ResourceSchedule {
using OHOS::AppExecFwk::AbilityType;

void ProcessRecord::SetName(const std::string& name)
{
    if (processName_.empty()) {
        processName_ = name;
    }
}

std::shared_ptr<AbilityInfo> ProcessRecord::GetAbilityInfoNonNull(int32_t recordId)
{
    auto a = std::find_if(abilities_.begin(), abilities_.end(), [ recordId ] (const auto& a) {
        return a->recordId_ == recordId;
    });
    if (a != abilities_.end()) {
        return *a;
    }
    std::shared_ptr<AbilityInfo> abi = std::make_shared<AbilityInfo>(recordId);
    abilities_.push_back(abi);
    return abi;
}

std::shared_ptr<AbilityInfo> ProcessRecord::GetAbilityInfo(int32_t recordId)
{
    auto a = std::find_if(abilities_.begin(), abilities_.end(), [ recordId ] (const auto& a) {
        return a->recordId_ == recordId;
    });
    if (a != abilities_.end()) {
        return *a;
    }
    return nullptr;
}

std::shared_ptr<WindowInfo> ProcessRecord::GetWindowInfoNonNull(uint32_t windowId)
{
    auto w = std::find_if(windows_.begin(), windows_.end(), [ windowId ] (const auto& w) {
        return w->windowId_ == windowId;
    });
    if (w != windows_.end()) {
        return *w;
    }
    std::shared_ptr<WindowInfo> win = std::make_shared<WindowInfo>(windowId);
    windows_.push_back(win);
    return win;
}

void ProcessRecord::RemoveAbilityByRecordId(int32_t recordId)
{
    for (auto iter = abilities_.begin(); iter != abilities_.end(); ++iter) {
        if ((*iter)->recordId_ == recordId) {
            abilities_.erase(iter);
            break;
        }
    }
}

bool ProcessRecord::HasAbility(int32_t recordId) const
{
    return std::any_of(abilities_.begin(), abilities_.end(), [ recordId ] (const auto& abi) {
        return abi->recordId_ == recordId;
    });
}

bool ProcessRecord::HasServiceExtension() const
{
    return std::any_of(abilities_.begin(), abilities_.end(), [] (const auto& abi) {
        return abi->type_ == (int32_t)(AbilityType::SERVICE)
            || abi->type_ == (int32_t)(AbilityType::EXTENSION)
            || abi->type_ == (int32_t)(AbilityType::DATA);
    });
}

bool ProcessRecord::IsVisible() const
{
    return std::any_of(windows_.begin(), windows_.end(), [] (const auto& w) {
        return w->isVisible_;
    });
}

std::set<int32_t> ProcessRecord::GetKeyTidSetByRole(int64_t role)
{
    std::set<int32_t> tids {};
    for (const auto [tid, tidRole] : keyThreadRoleMap_) {
        if (tidRole != role) {
            continue;
        }
        tids.insert(tid);
    }
    return tids;
}

std::shared_ptr<ProcessRecord> Application::AddProcessRecord(std::shared_ptr<ProcessRecord> pr)
{
    if (pr) {
        pidsMap_[pr->GetPid()] = pr;
    }
    return pr;
}

void Application::RemoveProcessRecord(pid_t pid)
{
    auto iter = pidsMap_.find(pid);
    if (iter != pidsMap_.end()) {
        if (focusedProcess_ == iter->second) {
            focusedProcess_ = nullptr;
        }
        if (IsHostProcess(pid)) {
            hostPidsSet_.erase(pid);
        }
        pidsMap_.erase(iter);
    }
}

std::shared_ptr<ProcessRecord> Application::GetProcessRecord(pid_t pid)
{
    auto iter = pidsMap_.find(pid);
    if (iter != pidsMap_.end()) {
        return iter->second;
    }
    return nullptr;
}

std::shared_ptr<ProcessRecord> Application::GetProcessRecordNonNull(pid_t pid)
{
    auto iter = pidsMap_.find(pid);
    if (iter != pidsMap_.end()) {
        return iter->second;
    }
    auto pr = std::make_shared<ProcessRecord>(this->GetUid(), pid);
    pidsMap_[pid] = pr;
    return pr;
}

std::shared_ptr<ProcessRecord> Application::FindProcessRecordByRecordId(int32_t recordId)
{
    for (auto iter = pidsMap_.begin(); iter != pidsMap_.end(); iter++) {
        auto pr = iter->second;
        if (pr->HasAbility(recordId)) {
            return pr;
        }
    }
    return nullptr;
}

void Application::SetName(const std::string& name)
{
    if (name_.empty()) {
        name_ = name;
    }
}

std::shared_ptr<ProcessRecord> Application::FindProcessRecordByWindowId(uint32_t windowId)
{
    for (auto iter = pidsMap_.begin(); iter != pidsMap_.end(); iter++) {
        auto pr = iter->second;
        if (std::any_of(pr->windows_.begin(), pr->windows_.end(),
            [ windowId ] (const auto& w) {
                return w->windowId_ == windowId;
            })) {
            return pr;
        }
    }
    return nullptr;
}

void Application::AddHostProcess(int32_t hostPid)
{
    hostPidsSet_.insert(hostPid);
}

bool Application::IsHostProcess(int32_t hostPid) const
{
    if (hostPidsSet_.find(hostPid) != hostPidsSet_.end()) {
        return true;
    } else {
        return false;
    }
}

std::shared_ptr<Application> Supervisor::GetAppRecord(int32_t uid)
{
    auto iter = uidsMap_.find(uid);
    if (iter != uidsMap_.end()) {
        return iter->second;
    }
    return nullptr;
}

std::shared_ptr<Application> Supervisor::GetAppRecordNonNull(int32_t uid)
{
    auto iter = uidsMap_.find(uid);
    if (iter != uidsMap_.end()) {
        return iter->second;
    }
    auto app = std::make_shared<Application>(uid);
    uidsMap_[uid] = app;
    return app;
}

std::shared_ptr<ProcessRecord> Supervisor::FindProcessRecord(pid_t pid)
{
    std::shared_ptr<ProcessRecord> pr = nullptr;
    for (auto iter = uidsMap_.begin(); iter != uidsMap_.end(); iter++) {
        auto app = iter->second;
        pr = app->GetProcessRecord(pid);
        if (pr) {
            break;
        }
    }
    return pr;
}

void Supervisor::RemoveApplication(int32_t uid)
{
    auto iter = uidsMap_.find(uid);
    if (iter != uidsMap_.end()) {
        uidsMap_.erase(iter);
    }
}

void Supervisor::SearchAbilityRecordId(std::shared_ptr<Application> &application,
    std::shared_ptr<ProcessRecord> &procRecord, int32_t recordId)
{
    std::shared_ptr<ProcessRecord> pr = nullptr;
    for (auto iter = uidsMap_.begin(); iter != uidsMap_.end(); iter++) {
        auto app = iter->second;
        pr = app->FindProcessRecordByRecordId(recordId);
        if (pr) {
            application = app;
            procRecord = pr;
            break;
        }
    }
}

void Supervisor::SearchWindowId(std::shared_ptr<Application> &application,
    std::shared_ptr<ProcessRecord> &procRecord, uint32_t windowId)
{
    std::shared_ptr<ProcessRecord> pr = nullptr;
    for (auto iter = uidsMap_.begin(); iter != uidsMap_.end(); iter++) {
        auto app = iter->second;
        pr = app->FindProcessRecordByWindowId(windowId);
        if (pr) {
            application = app;
            procRecord = pr;
            break;
        }
    }
}

void Supervisor::SetSystemLoadLevelState(int32_t level)
{
    systemLoadLevel_ = level;
}

int32_t Supervisor::GetSystemLoadLevel()
{
    return systemLoadLevel_;
}

void Supervisor::ConnectAppManagerService()
{
    sptr<OHOS::ISystemAbilityManager> systemAbilityManager =
        OHOS::SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    sptr<OHOS::IRemoteObject> object = systemAbilityManager->GetSystemAbility(OHOS::APP_MGR_SERVICE_ID);
    appManager_ = OHOS::iface_cast<OHOS::AppExecFwk::IAppMgr>(object);
}

void Supervisor::ReloadApplication()
{
    if (appManager_ == nullptr) {
        return;
    }
    std::vector<AppExecFwk::RunningProcessInfo> runningProcess;
    appManager_->GetAllRunningProcesses(runningProcess);
    for (const auto& process : runningProcess) {
        std::shared_ptr<Application> app = GetAppRecordNonNull(process.uid_);
        if (process.bundleNames.size() != 0) {
            app->SetName(process.bundleNames[0]);
        }
        app->GetProcessRecordNonNull(process.pid_);
        CGS_LOGI("reload application cache uid:%{public}d pid:%{public}d bundleName:%{public}s isFocused:%{public}d",
            process.uid_, process.pid_, app->GetName().c_str(), process.isFocused);
    }
}

#ifdef SUPPORT_CHILD_PROCESS
void Supervisor::ReloadChildProcess()
{
    if (appManager_ == nullptr) {
        return;
    }
    std::vector<AppExecFwk::ChildProcessInfo> childProcess;
    appManager_->GetAllChildrenProcesses(childProcess);
    for (const auto& process : childProcess) {
        std::shared_ptr<Application> app = GetAppRecordNonNull(process.hostUid);
        app->AddHostProcess(process.hostPid);
        auto procRecord = app->GetProcessRecordNonNull(process.pid);
        procRecord->processType_ = ProcRecordType::CHILD;
        procRecord->hostPid_ = process.hostPid;
        procRecord->isReload_ = true;
        CGS_LOGI("reload child process bundleName:%{public}s processName:%{public}s pid:%{public}d \
            uid:%{public}d hostUid:%{public}d hostPid:%{public}d",
            process.bundleName.c_str(), process.processName.c_str(), process.pid,
            process.uid, process.hostUid, process.hostPid);
    }
}
#endif // SUPPORT_CHILD_PROCESS

void Supervisor::InitSuperVisorContent()
{
    ConnectAppManagerService();
    /* reload application info */
    ReloadApplication();
#ifdef SUPPORT_CHILD_PROCESS
    /* reload child process */
    ReloadChildProcess();
#endif // SUPPORT_CHILD_PROCESS
}
} // namespace ResourceSchedule
} // namespace OHOS
