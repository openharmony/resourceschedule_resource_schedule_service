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

#ifndef RESSCHED_INTERFACES_INNERKITS_SUSPEND_MANAGER_BASE_CLIENT_INCLUDE_SUSPEND_MANAGER_BASE_CLIENT_H
#define RESSCHED_INTERFACES_INNERKITS_SUSPEND_MANAGER_BASE_CLIENT_INCLUDE_SUSPEND_MANAGER_BASE_CLIENT_H

#include <cstdint>                                  // for int64_t, uint32_t
#include "errors.h"                                 // for ErrCode
#include <mutex>
#include <iremote_broker.h>
#include "isuspend_state_observer_base.h"
#include "ires_sched_service.h"

namespace OHOS {
namespace ResourceSchedule {
class SuspendManagerBaseClient {
public:
    /**
     * @brief Get one suspend manager client object.
     *
     * @return SuspendManagerBaseClient object.
     */
    static SuspendManagerBaseClient& GetInstance();

    /**
     * @brief Get application suspend state by uid.
     *
     * @param uid uid of application.
     * @param isFrozen suspend state. active is false, frozen is true.
     */
    ErrCode GetSuspendStateByUid(const int32_t uid, bool &isFrozen);

    /**
     * @brief Get process suspend state by pid.
     *
     * @param pid pid of application.
     * @param isFrozen suspend state. active is false, frozen is true.
     */
    ErrCode GetSuspendStateByPid(const int32_t pid, bool &isFrozen);

    /**
     * @brief Register suspend state observer.
     *
     * @param observer suspend state observer.When suspend state changed, observer will be called.
     * @return ERR_OK on success,
     *         other on err.
     */
    ErrCode RegisterSuspendObserver(const sptr<ISuspendStateObserverBase> &observer);

    /**
     * @brief Unregister suspend state observer.
     *
     * @param observer suspend state observer.
     * @return ERR_OK on success,
     *         other on err.
     */
    ErrCode UnregisterSuspendObserver(const sptr<ISuspendStateObserverBase> &observer);

private:
    SuspendManagerBaseClient() = default;
    ~SuspendManagerBaseClient();
    SuspendManagerBaseClient(const SuspendManagerBaseClient &suspendManagerBaseClient);
    const SuspendManagerBaseClient &operator=(const SuspendManagerBaseClient &suspendManagerBaseClient);

private:
    class SuspendManagerDeathRecipient : public IRemoteObject::DeathRecipient {
    public:
        explicit SuspendManagerDeathRecipient(SuspendManagerBaseClient &suspendManagerBaseClient);

        ~SuspendManagerDeathRecipient();

        void OnRemoteDied(const wptr<IRemoteObject> &object) override;

    private:
        SuspendManagerBaseClient &suspendManagerBaseClient_;
    };

private:
    bool GetSuspendManagerProxy();
    void ResetClient();
    sptr<IResSchedService> suspendManagerBaseProxy_;
    sptr<SuspendManagerDeathRecipient> recipient_;
    std::mutex mutex_;
};
} // namespace ResourceSchedule
} // namespace OHOS

#endif // RESSCHED_INTERFACES_INNERKITS_SUSPEND_MANAGER_BASE_CLIENT_INCLUDE_SUSPEND_MANAGER_BASE_CLIENT_H