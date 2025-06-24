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
#include "suspend_manager_base_observer.h"
#include "suspend_state_observer_base_recipient.h"
#include "suspend_manager_base_log.h"

namespace OHOS {
namespace ResourceSchedule {
IMPLEMENT_SINGLE_INSTANCE(SuspendManagerBaseObserver)

void SuspendManagerBaseObserver::OnObserverDied(const wptr<IRemoteObject> &remote)
{
    SUSPEND_MSG_LOGI("Suspend manager OnObserverDied called");
    auto object = remote.promote();
    if (object == nullptr) {
        SUSPEND_MSG_LOGE("observer nullptr.");
        return;
    }
    sptr<ISuspendStateObserverBase> observer = iface_cast<ISuspendStateObserverBase>(object);
    int32_t ret = UnregisterSuspendObserverInner(observer);
    if (ret != ERR_OK) {
        SUSPEND_MSG_LOGE("Unregister suspend observer failed in OnObserverDied");
    }

    ret = RemoveObserverDeathRecipient(observer);
    if (ret != ERR_OK) {
        SUSPEND_MSG_LOGE("Remove observer death recipient failed.");
    }
}

int32_t SuspendManagerBaseObserver::AddObserverDeathRecipient(const sptr<ISuspendStateObserverBase> &observer)
{
    SUSPEND_MSG_LOGD("Add observer death recipient begin.");
    if (observer == nullptr || observer->AsObject() == nullptr) {
        SUSPEND_MSG_LOGE("The param observer is nullptr.");
        return ERR_INVALID_VALUE;
    }

    std::lock_guard<std::mutex> lock(recipientMapLock_);
    auto it = recipientMap_.find(observer->AsObject());
    if (it != recipientMap_.end()) {
        SUSPEND_MSG_LOGI("This death recipient has been added.");
        return ERR_OK;
    }

    sptr<IRemoteObject::DeathRecipient> deathRecipient =
        new SuspendStateObserverBaseRecipient([this](const wptr<IRemoteObject> &remote) {
            this->OnObserverDied(remote);
        });
    if (deathRecipient == nullptr) {
        SUSPEND_MSG_LOGE("create death recipient ptr failed");
        return ERR_INVALID_VALUE;
    }
    if (!observer->AsObject()->AddDeathRecipient(deathRecipient)) {
        SUSPEND_MSG_LOGE("AddDeathRecipient failed.");
        return ERR_INVALID_OPERATION;
    }

    recipientMap_.emplace(observer->AsObject(), deathRecipient);
    SUSPEND_MSG_LOGI("Add death recipient succeed.");
    return ERR_OK;
}

int32_t SuspendManagerBaseObserver::RemoveObserverDeathRecipient(const sptr<ISuspendStateObserverBase> &observer)
{
    SUSPEND_MSG_LOGD("Remove observer death recipient begin.");
    if (observer == nullptr || observer->AsObject() == nullptr) {
        SUSPEND_MSG_LOGE("The param observer is nullptr.");
        return ERR_INVALID_VALUE;
    }

    std::lock_guard<std::mutex> lock(recipientMapLock_);
    auto it = recipientMap_.find(observer->AsObject());
    if (it != recipientMap_.end()) {
        it->first->RemoveDeathRecipient(it->second);
        recipientMap_.erase(it);
        SUSPEND_MSG_LOGI("Remove death recipient succeed.");
        return ERR_OK;
    }

    SUSPEND_MSG_LOGW("Observer not found in observer death recipient.");
    return ERR_OK;
}

int32_t SuspendManagerBaseObserver::RegisterSuspendObserver(const sptr<ISuspendStateObserverBase> &observer)
{
    int32_t ret = RegisterSuspendObserverInner(observer);
    if (ret != ERR_OK) {
        SUSPEND_MSG_LOGE("Register suspend observer inner error.");
        return ret;
    }

    ret = AddObserverDeathRecipient(observer);
    if (ret != ERR_OK) {
        SUSPEND_MSG_LOGE("Add observer death recipient failed.");
        UnregisterSuspendObserverInner(observer);
        return ret;
    }
    SUSPEND_MSG_LOGI("BaseObserver register succ, size:%{public}u.", (uint32_t)suspendObservers_.size());
    return ERR_OK;
}

int32_t SuspendManagerBaseObserver::UnregisterSuspendObserver(const sptr<ISuspendStateObserverBase> &observer)
{
    int32_t ret = UnregisterSuspendObserverInner(observer);
    if (ret != ERR_OK) {
        SUSPEND_MSG_LOGE("Unregister suspend observer inner error.");
        return ret;
    }

    ret = RemoveObserverDeathRecipient(observer);
    if (ret != ERR_OK) {
        SUSPEND_MSG_LOGE("Remove observer death recipient failed.");
        RegisterSuspendObserverInner(observer);
        return ret;
    }
    SUSPEND_MSG_LOGI("BaseObserver unregister succ, size:%{public}u.", (uint32_t)suspendObservers_.size());
    return ERR_OK;
}

int32_t SuspendManagerBaseObserver::RegisterSuspendObserverInner(const sptr<ISuspendStateObserverBase> &observer)
{
    SUSPEND_MSG_LOGI("BaseObserver RegisterSuspendObserverInner.");
    if (observer == nullptr) {
        SUSPEND_MSG_LOGE("Register suspend observer error: observer is null.");
        return ERR_INVALID_VALUE;
    }

    std::lock_guard<std::mutex> lock(suspendObserverLock_);
    auto it = std::find_if(suspendObservers_.begin(), suspendObservers_.end(),
        [&observer](const sptr<ISuspendStateObserverBase> &item) {
            return (item && item->AsObject() == observer->AsObject());
        });
    if (it != suspendObservers_.end()) {
        SUSPEND_MSG_LOGW("Register error: observer already exist.");
        return ERR_OK;
    }
    suspendObservers_.push_back(observer);
    return ERR_OK;
}

int32_t SuspendManagerBaseObserver::UnregisterSuspendObserverInner(const sptr<ISuspendStateObserverBase> &observer)
{
    SUSPEND_MSG_LOGI("BaseObserver UnregisterSuspendObserverInner.");
    if (observer == nullptr) {
        SUSPEND_MSG_LOGE("Unregister suspend observer error: observer is null.");
        return ERR_INVALID_VALUE;
    }

    std::lock_guard<std::mutex> lock(suspendObserverLock_);
    auto it = std::find_if(suspendObservers_.begin(), suspendObservers_.end(),
        [&observer](const sptr<ISuspendStateObserverBase> &item) {
            return (item && item->AsObject() == observer->AsObject());
        });
    if (it == suspendObservers_.end()) {
        SUSPEND_MSG_LOGW("Suspend observer not register.");
        return ERR_OK;
    }
    suspendObservers_.erase(it);
    return ERR_OK;
}

std::unordered_map<uint32_t, std::function<void(std::vector<sptr<ISuspendStateObserverBase>>::iterator,
    const int32_t, const std::vector<int32_t>&)>> SuspendManagerBaseObserver::updateSuspendCbMap_ = {
    {
        static_cast<uint32_t>(States::ACTIVE_STATE),
        [](std::vector<sptr<ISuspendStateObserverBase>>::iterator iter, const int32_t uid,
            const std::vector<int32_t> &pidList) {
            return (*iter)->OnActive(pidList, uid);
        }
    },
    {
        static_cast<uint32_t>(States::DOZE_STATE),
        [](std::vector<sptr<ISuspendStateObserverBase>>::iterator iter, const int32_t uid,
            const std::vector<int32_t> &pidList) {
            return (*iter)->OnDoze(pidList, uid);
        }
    },
    {
        static_cast<uint32_t>(States::FROZEN_STATE),
        [](std::vector<sptr<ISuspendStateObserverBase>>::iterator iter, const int32_t uid,
            const std::vector<int32_t> &pidList) {
            return (*iter)->OnFrozen(pidList, uid);
        }
    },
};

int32_t SuspendManagerBaseObserver::UpdateSuspendObserver(const States state, const int32_t uid,
    const std::vector<int32_t> &pidList)
{
    auto cbIt = updateSuspendCbMap_.find(static_cast<uint32_t>(state));
    if (cbIt == updateSuspendCbMap_.end()) {
        SUSPEND_MSG_LOGE("Update suspend observer state error: %{public}u.", static_cast<uint32_t>(state));
        return ERR_INVALID_VALUE;
    }

    std::lock_guard<std::mutex> lock(suspendObserverLock_);
    if (suspendObservers_.empty()) {
        SUSPEND_MSG_LOGD("BaseObserver Update observer null.");
        return ERR_OK;
    }
    for (auto iter = suspendObservers_.begin(); iter != suspendObservers_.end(); ++iter) {
        cbIt->second(iter, uid, pidList);
    }

    SUSPEND_MSG_LOGD("BaseObserver UpdateSuspendObserver for state: %{public}u, uid: %{public}d.", cbIt->first, uid);
    return ERR_OK;
}
} // namespace ResourceSchedule
} // namespace OHOS