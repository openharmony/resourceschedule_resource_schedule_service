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

#include "supervisor.h"
#include "cgroup_sched_common.h"

namespace OHOS {
namespace ResourceSchedule {
std::shared_ptr<AbilityInfo> ProcessRecord::GetAbilityInfoNonNull(sptr<IRemoteObject> token)
{
    for (auto a : abilities_) {
        if (a->token_ == token) {
            return a;
        }
    }
    std::shared_ptr<AbilityInfo> abi = std::make_shared<AbilityInfo>(token);
    abilities_.push_back(abi);
    return abi;
}

bool ProcessRecord::HasAbility(sptr<IRemoteObject> token) const
{
    for (auto abi : abilities_) {
        if (abi->token_ == token)
            return true;
    }
    return false;
}

std::shared_ptr<ProcessRecord> Application::AddProcessRecord(std::shared_ptr<ProcessRecord> pr)
{
    if (pr != nullptr) {
        pidsMap_[pr->GetPid()] = pr;
    }
    return pr;
}

void Application::RemoveProcessRecord(pid_t pid)
{
    auto iter = pidsMap_.find(pid);
    if (iter != pidsMap_.end()) {
        pidsMap_.erase(iter);
    }
}

std::shared_ptr<ProcessRecord> Application::GetProcessRecord(pid_t pid)
{
    if (pidsMap_.find(pid) == pidsMap_.end()) {
        return nullptr;
    }
    return pidsMap_[pid];
}

std::shared_ptr<ProcessRecord> Application::GetProcessRecordNonNull(pid_t pid, std::string name)
{
    if (pidsMap_.find(pid) == pidsMap_.end()) {
        auto pr = std::make_shared<ProcessRecord>(this->GetUid(), pid, name);
        this->AddProcessRecord(pr);
        return pr;
    }
    return pidsMap_[pid];
}

std::shared_ptr<ProcessRecord> Application::FindProcessRecord(sptr<IRemoteObject> token)
{
    for (auto iter = pidsMap_.begin(); iter != pidsMap_.end(); iter++) {
        auto pr = iter->second;
        if (pr->HasAbility(token)) {
            return pr;
        }
    }
    return nullptr;
}

std::shared_ptr<Application> Supervisor::GetAppRecord(int32_t uid)
{
    if (uidsMap_.find(uid) == uidsMap_.end()) {
        return nullptr;
    }
    return uidsMap_[uid];
}

std::shared_ptr<Application> Supervisor::GetAppRecordNonNull(int32_t uid, std::string bundleName)
{
    if (uidsMap_.find(uid) == uidsMap_.end()) {
        auto app = std::make_shared<Application>(uid, bundleName);
        uidsMap_[uid] = app;
        return app;
    }
    return uidsMap_[uid];
}

std::shared_ptr<ProcessRecord> Supervisor::FindProcessRecord(pid_t pid)
{
    std::shared_ptr<ProcessRecord> pr = nullptr;
    for (auto iter = uidsMap_.begin(); iter != uidsMap_.end(); iter++) {
        auto app = iter->second;
        pr = app->GetProcessRecord(pid);
        if (pr != nullptr) {
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

void Supervisor::SearchAbilityToken(std::shared_ptr<Application> &application,
    std::shared_ptr<ProcessRecord> &procRecord, sptr<IRemoteObject> token)
{
    std::shared_ptr<ProcessRecord> pr = nullptr;
    for (auto iter = uidsMap_.begin(); iter != uidsMap_.end(); iter++) {
        auto app = iter->second;
        pr = app->FindProcessRecord(token);
        if (pr != nullptr) {
            application = app;
            procRecord = pr;
            break;
        }
    }
}
} // namespace ResourceSchedule
} // namespace OHOS
