/*
 * Copyright (c) 2025-2025 Huawei Device Co., Ltd.
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
#include "suspend_manager_base_client.h"
#include "nlohmann/json.hpp"
#include "iservice_registry.h"
#include "system_ability_definition.h"
#include "suspend_manager_base_log.h"

namespace OHOS {
namespace ResourceSchedule {
SuspendManagerBaseClient &SuspendManagerBaseClient::GetInstance()
{
    static SuspendManagerBaseClient instance;
    return instance;
}

ErrCode SuspendManagerBaseClient::GetSuspendStateByUid(const int32_t uid, bool &isFrozen)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!GetSuspendManagerProxy()) {
        SUSPEND_MSG_LOGE("Failed to register suspend observer, get proxy err.");
        return ERR_INVALID_OPERATION;
    }
    int32_t funcResult = ERR_OK;
    suspendManagerBaseProxy_->GetSuspendStateByUid(uid, isFrozen, funcResult);
    return funcResult;
}

ErrCode SuspendManagerBaseClient::GetSuspendStateByPid(const int32_t pid, bool &isFrozen)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!GetSuspendManagerProxy()) {
        SUSPEND_MSG_LOGE("Failed to register suspend observer, get proxy err.");
        return ERR_INVALID_OPERATION;
    }
    int32_t funcResult = ERR_OK;
    suspendManagerBaseProxy_->GetSuspendStateByPid(pid, isFrozen, funcResult);
    return funcResult;
}

__attribute__((no_sanitize("cfi"))) void SuspendManagerBaseClient::ResetClient()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (suspendManagerBaseProxy_ && suspendManagerBaseProxy_->AsObject()) {
        suspendManagerBaseProxy_->AsObject()->RemoveDeathRecipient(recipient_);
    }
    suspendManagerBaseProxy_ = nullptr;
    recipient_ = nullptr;
}

SuspendManagerBaseClient::~SuspendManagerBaseClient()
{
    SUSPEND_MSG_LOGI("libsuspend_manager_base_client.z.so will remove, clear death recipient!");
    ResetClient();
}

SuspendManagerBaseClient::SuspendManagerDeathRecipient::SuspendManagerDeathRecipient(
    SuspendManagerBaseClient &suspendManagerBaseClient) : suspendManagerBaseClient_(suspendManagerBaseClient) {}

SuspendManagerBaseClient::SuspendManagerDeathRecipient::~SuspendManagerDeathRecipient() {}

void SuspendManagerBaseClient::SuspendManagerDeathRecipient::OnRemoteDied(const wptr<IRemoteObject> &remote)
{
    suspendManagerBaseClient_.ResetClient();
}

bool SuspendManagerBaseClient::GetSuspendManagerProxy()
{
    if (suspendManagerBaseProxy_) {
        return true;
    }
    sptr<ISystemAbilityManager> samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (!samgr) {
        SUSPEND_MSG_LOGE("Failed to get SystemAbilityManager.");
        return false;
    }
    sptr<IRemoteObject> object = samgr->CheckSystemAbility(RES_SCHED_SYS_ABILITY_ID);
    if (!object) {
        SUSPEND_MSG_LOGE("Failed to get SystemAbility[1901].");
        return false;
    }

    suspendManagerBaseProxy_ = iface_cast<IResSchedService>(object);
    if (!suspendManagerBaseProxy_ || !suspendManagerBaseProxy_->AsObject()) {
        SUSPEND_MSG_LOGE("Failed to get SystemAbility.");
        return false;
    }

    recipient_ = new (std::nothrow) SuspendManagerDeathRecipient(*this);
    if (!recipient_) {
        return false;
    }
    suspendManagerBaseProxy_->AsObject()->AddDeathRecipient(recipient_);

    return true;
}

ErrCode SuspendManagerBaseClient::RegisterSuspendObserver(const sptr<ISuspendStateObserverBase> &observer)
{
    if (observer == nullptr) {
        SUSPEND_MSG_LOGE("Failed to register suspend observer, observer is null.");
        return ERR_INVALID_VALUE;
    }

    std::lock_guard<std::mutex> lock(mutex_);
    if (!GetSuspendManagerProxy()) {
        SUSPEND_MSG_LOGE("Failed to register suspend observer, get proxy err.");
        return ERR_INVALID_OPERATION;
    }
    int32_t funcResult = ERR_OK;
    suspendManagerBaseProxy_->RegisterSuspendObserver(observer, funcResult);
    return funcResult;
}

ErrCode SuspendManagerBaseClient::UnregisterSuspendObserver(const sptr<ISuspendStateObserverBase> &observer)
{
    if (observer == nullptr) {
        SUSPEND_MSG_LOGE("Failed to unregister suspend observer, observer is null.");
        return ERR_INVALID_VALUE;
    }

    std::lock_guard<std::mutex> lock(mutex_);
    if (!GetSuspendManagerProxy()) {
        SUSPEND_MSG_LOGE("Failed to unregister suspend observer, get proxy err.");
        return ERR_INVALID_OPERATION;
    }
    int32_t funcResult = ERR_OK;
    suspendManagerBaseProxy_->UnregisterSuspendObserver(observer, funcResult);
    return funcResult;
}
} // namespace ResourceSchedule
} // namespace OHOS