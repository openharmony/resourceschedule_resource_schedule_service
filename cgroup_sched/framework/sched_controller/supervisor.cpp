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
#include "cgroup_sched_common.h"

namespace OHOS {
namespace ResourceSchedule {
using OHOS::AppExecFwk::AbilityType;

std::shared_ptr<AbilityInfo> ProcessRecord::GetAbilityInfoNonNull(uintptr_t token)
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

std::shared_ptr<AbilityInfo> ProcessRecord::GetAbilityInfo(uintptr_t token)
{
    for (auto a : abilities_) {
        if (a->token_ == token) {
            return a;
        }
    }
    return nullptr;
}

std::shared_ptr<WindowInfo> ProcessRecord::GetWindowInfoNonNull(uint32_t windowId)
{
    for (auto w : windows_) {
        if (w->windowId_ == windowId) {
            return w;
        }
    }
    std::shared_ptr<WindowInfo> win = std::make_shared<WindowInfo>(windowId);
    windows_.push_back(win);
    return win;
}

void ProcessRecord::RemoveAbilityByToken(uintptr_t token)
{
    for (auto iter = abilities_.begin(); iter != abilities_.end(); ++iter) {
        if ((*iter)->token_ == token) {
            abilities_.erase(iter);
            break;
        }
    }
}

bool ProcessRecord::HasAbility(uintptr_t token) const
{
    for (auto abi : abilities_) {
        if (abi->token_ == token)
            return true;
    }
    return false;
}

bool ProcessRecord::HasServiceExtension() const
{
    for (auto abi : abilities_) {
        if (abi->type_ == (int32_t)(AbilityType::SERVICE)
            || abi->type_ == (int32_t)(AbilityType::EXTENSION)
            || abi->type_ == (int32_t)(AbilityType::DATA)) {
            return true;
        }
    }
    return false;
}

bool ProcessRecord::IsVisible() const
{
    for (auto w : windows_) {
        if (w->isVisible_) {
            return true;
        }
    }
    return false;
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

std::shared_ptr<ProcessRecord> Application::GetProcessRecordNonNull(pid_t pid)
{
    if (pidsMap_.find(pid) == pidsMap_.end()) {
        auto pr = std::make_shared<ProcessRecord>(this->GetUid(), pid);
        this->AddProcessRecord(pr);
        return pr;
    }
    return pidsMap_[pid];
}

std::shared_ptr<ProcessRecord> Application::FindProcessRecordByToken(uintptr_t token)
{
    for (auto iter = pidsMap_.begin(); iter != pidsMap_.end(); iter++) {
        auto pr = iter->second;
        if (pr->HasAbility(token)) {
            return pr;
        }
    }
    return nullptr;
}

std::shared_ptr<ProcessRecord> Application::FindProcessRecordByWindowId(uint32_t windowId)
{
    for (auto iter = pidsMap_.begin(); iter != pidsMap_.end(); iter++) {
        auto pr = iter->second;
        for (auto& w : pr->windows_) {
            if (w->windowId_ == windowId) {
                return pr;
            }
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

std::shared_ptr<Application> Supervisor::GetAppRecordNonNull(int32_t uid)
{
    if (uidsMap_.find(uid) == uidsMap_.end()) {
        auto app = std::make_shared<Application>(uid);
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

void Supervisor::SearchAbilityToken(std::shared_ptr<Application> &application,
    std::shared_ptr<ProcessRecord> &procRecord, uintptr_t token)
{
    std::shared_ptr<ProcessRecord> pr = nullptr;
    for (auto iter = uidsMap_.begin(); iter != uidsMap_.end(); iter++) {
        auto app = iter->second;
        pr = app->FindProcessRecordByToken(token);
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
} // namespace ResourceSchedule
} // namespace OHOS
