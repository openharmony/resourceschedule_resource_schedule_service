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
#ifndef EFFICIENCY_MANAGER_SUSPEND_BASE_OBSERVER_H
#define EFFICIENCY_MANAGER_SUSPEND_BASE_OBSERVER_H

#include <unordered_map>
#include <vector>
#include <mutex>
#include <map>
#include "single_instance.h"

#include "suspend_state_observer_base_stub.h"

namespace OHOS {
namespace ResourceSchedule {
enum class States : int32_t {
    ACTIVE_STATE = 0,
    DOZE_STATE,
    FROZEN_STATE,
};

class SuspendManagerBaseObserver : public std::enable_shared_from_this<SuspendManagerBaseObserver> {
DECLARE_SINGLE_INSTANCE(SuspendManagerBaseObserver)
public:
    int32_t RegisterSuspendObserverInner(const sptr<ISuspendStateObserverBase> &observer);
    int32_t UnregisterSuspendObserverInner(const sptr<ISuspendStateObserverBase> &observer);
    int32_t UpdateSuspendObserver(const States state, const int32_t uid, const std::vector<int32_t> &pidList);
    int32_t RegisterSuspendObserver(const sptr<ISuspendStateObserverBase> &observer);
    int32_t UnregisterSuspendObserver(const sptr<ISuspendStateObserverBase> &observer);

private:
    void OnObserverDied(const wptr<IRemoteObject> &remote);
    int32_t AddObserverDeathRecipient(const sptr<ISuspendStateObserverBase> &observer);
    int32_t RemoveObserverDeathRecipient(const sptr<ISuspendStateObserverBase> &observer);

private:
    std::mutex suspendObserverLock_;
    std::mutex recipientMapLock_;
    std::vector<sptr<ISuspendStateObserverBase>> suspendObservers_ {};
    std::map<sptr<IRemoteObject>, sptr<IRemoteObject::DeathRecipient>> recipientMap_ {};
    static std::unordered_map<uint32_t, std::function<void(std::vector<sptr<ISuspendStateObserverBase>>::iterator,
        const int32_t, const std::vector<int32_t>&)>> updateSuspendCbMap_;
};
} // namespace ResourceSchedule
} // namespace OHOS
#endif // EFFICIENCY_MANAGER_SUSPEND_BASE_OBSERVER_H