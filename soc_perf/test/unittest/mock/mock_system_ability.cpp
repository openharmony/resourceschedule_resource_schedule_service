/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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

#include <gtest/gtest.h>
#include "system_ability.h"

using namespace OHOS;

SystemAbility::SystemAbility(const int32_t serviceId, bool runOnCreate)
{}
SystemAbility::~SystemAbility()
{}

bool SystemAbility::MakeAndRegisterAbility(SystemAbility* systemAbility)
{
    GTEST_LOG_(INFO) << "aams MOCK SystemAbility MakeAndRegisterAbility";
    (void)systemAbility;
    return true;
}

bool SystemAbility::AddSystemAbilityListener(int32_t systemAbilityId)
{
    GTEST_LOG_(INFO) << "aams MOCK SystemAbility MakeAndRegisterAbility";
    OnAddSystemAbility(systemAbilityId, "");
    return true;
}

bool SystemAbility::RemoveSystemAbilityListener(int32_t systemAbilityId)
{
    GTEST_LOG_(INFO) << "aams MOCK SystemAbility RemoveSystemAbilityListener";
    (void)systemAbilityId;
    return true;
}

bool SystemAbility::Publish(sptr<IRemoteObject> systemAbility)
{
    GTEST_LOG_(INFO) << "aams MOCK SystemAbility Publish";
    systemAbility.ForceSetRefPtr(nullptr);
    return true;
}

void SystemAbility::StopAbility(int32_t systemAbilityId)
{
    GTEST_LOG_(INFO) << "aams MOCK SystemAbility StopAbility";
    (void)systemAbilityId;
}

void SystemAbility::Start()
{
    GTEST_LOG_(INFO) << "aams MOCK SystemAbility Start";
}

void SystemAbility::Stop()
{
    GTEST_LOG_(INFO) << "aams MOCK SystemAbility Stop";
}

void SystemAbility::SADump()
{}

int32_t SystemAbility::GetSystemAbilitId() const
{
    GTEST_LOG_(INFO) << "aams MOCK SystemAbility GetSystemAbilitId";
    return 0;
}

void SystemAbility::SetLibPath(const std::string& libPath)
{
    GTEST_LOG_(INFO) << "aams MOCK SystemAbility SetLibPath";
    libPath_ = libPath;
}

const std::string& SystemAbility::GetLibPath() const
{
    GTEST_LOG_(INFO) << "aams MOCK SystemAbility GetLibPath";
    return libPath_;
}

void SystemAbility::SetDependSa(const std::vector<int32_t>& dependSa)
{
    GTEST_LOG_(INFO) << "aams MOCK SystemAbility SetDependSa";
    dependSa_ = dependSa;
}

const std::vector<int32_t>& SystemAbility::GetDependSa() const
{
    GTEST_LOG_(INFO) << "aams MOCK SystemAbility SetDependSa";
    return dependSa_;
}

void SystemAbility::SetRunOnCreate(bool isRunOnCreate)
{
    GTEST_LOG_(INFO) << "aams MOCK SystemAbility SetRunOnCreate";
    isRunOnCreate_ = isRunOnCreate;
}

bool SystemAbility::IsRunOnCreate() const
{
    GTEST_LOG_(INFO) << "aams MOCK SystemAbility IsRunOnCreate";
    return isRunOnCreate_;
}

void SystemAbility::SetDistributed(bool isDistributed)
{
    GTEST_LOG_(INFO) << "aams MOCK SystemAbility SetDistributed";
    isDistributed_ = isDistributed;
}

bool SystemAbility::GetDistributed() const
{
    GTEST_LOG_(INFO) << "aams MOCK SystemAbility GetDistributed";
    return isDistributed_;
}

bool SystemAbility::GetRunningStatus() const
{
    GTEST_LOG_(INFO) << "aams MOCK SystemAbility GetRunningStatus";
    return isRunning_;
}

void SystemAbility::SetDumpLevel(uint32_t dumpLevel)
{
    GTEST_LOG_(INFO) << "aams MOCK SystemAbility SetDumpLevel";
    dumpLevel_ = dumpLevel;
}

uint32_t SystemAbility::GetDumpLevel() const
{
    GTEST_LOG_(INFO) << "aams MOCK SystemAbility GetDumpLevel";
    return dumpLevel_;
}

void SystemAbility::SetDependTimeout(int32_t dependTimeout)
{
    GTEST_LOG_(INFO) << "aams MOCK SystemAbility SetDependTimeout";
    (void)dependTimeout;
}

int32_t SystemAbility::GetDependTimeout() const
{
    GTEST_LOG_(INFO) << "aams MOCK SystemAbility GetDependTimeout";
    return dependTimeout_;
}

// The details should be implemented by subclass
void SystemAbility::OnDump()
{
    GTEST_LOG_(INFO) << "aams MOCK SystemAbility OnDump";
}

// The details should be implemented by subclass
void SystemAbility::OnStart()
{
    GTEST_LOG_(INFO) << "aams MOCK SystemAbility OnStart";
}

void SystemAbility::OnStart(const SystemAbilityOnDemandReason& startReason)
{
    GTEST_LOG_(INFO) << "aams MOCK SystemAbility OnStart";
}

int32_t SystemAbility::OnIdle(const SystemAbilityOnDemandReason& idleReason)
{
    GTEST_LOG_(INFO) << "aams MOCK SystemAbility OnIdle";
    return 0;
}

void SystemAbility::OnActive(const SystemAbilityOnDemandReason& activeReason)
{
    GTEST_LOG_(INFO) << "aams MOCK SystemAbility OnActive";
}

// The details should be implemented by subclass
void SystemAbility::OnStop()
{
    GTEST_LOG_(INFO) << "aams MOCK SystemAbility OnStop";
}

void SystemAbility::OnStop(const SystemAbilityOnDemandReason& stopReason)
{
    GTEST_LOG_(INFO) << "aams MOCK SystemAbility OnStop";
}

// The details should be implemented by subclass
void SystemAbility::OnAddSystemAbility(int32_t systemAbilityId, const std::string& deviceId)
{
    GTEST_LOG_(INFO) << "aams MOCK SystemAbility OnAddSystemAbility";
    (void)systemAbilityId;
    (void)deviceId;
}

// The details should be implemented by subclass
void SystemAbility::OnRemoveSystemAbility(int32_t systemAbilityId, const std::string& deviceId)
{
    GTEST_LOG_(INFO) << "aams MOCK SystemAbility OnRemoveSystemAbility";
    (void)systemAbilityId;
    (void)deviceId;
}

sptr<IRemoteObject> SystemAbility::GetSystemAbility(int32_t systemAbilityId)
{
    GTEST_LOG_(INFO) << "aams MOCK SystemAbility GetSystemAbility";
    (void)systemAbilityId;
    return nullptr;
}

void SystemAbility::SetCapability(const std::u16string& capability)
{
    GTEST_LOG_(INFO) << "aams MOCK SystemAbility SetCapability";
    capability_ = capability;
}

const std::u16string& SystemAbility::GetCapability() const
{
    GTEST_LOG_(INFO) << "aams MOCK SystemAbility GetCapability";
    return capability_;
}

void SystemAbility::SetPermission(const std::u16string& permission)
{
    GTEST_LOG_(INFO) << "aams MOCK SystemAbility SetPermission";
    permission_ = permission;
}

// The details should be implemented by subclass
void SystemAbility::OnDeviceLevelChanged(int32_t type, int32_t level, std::string& action)
{
    GTEST_LOG_(INFO) << "aams MOCK SystemAbility OnDeviceLevelChanged";
    (void)type;
    (void)level;
    (void)action;
}
