/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef RESSCHED_INTERFACES_INNERKITS_RESSCHED_CLIENT_INCLUDE_RES_SCHED_EVENT_LISTENER_PROXY_H
#define RESSCHED_INTERFACES_INNERKITS_RESSCHED_CLIENT_INCLUDE_RES_SCHED_EVENT_LISTENER_PROXY_H

#include "ires_sched_event_listener.h"

#include "iremote_proxy.h"

namespace OHOS {
namespace ResourceSchedule {
class ResSchedEventListenerProxy : public IRemoteProxy<IResSchedEventListener> {
public:
    explicit ResSchedEventListenerProxy(const sptr<IRemoteObject>& impl)
        : IRemoteProxy<IResSchedEventListener>(impl) {}
    virtual ~ResSchedEventListenerProxy() = default;

    void OnReceiveEvent(uint32_t eventType, uint32_t eventValue, uint32_t listenerGroup,
        const nlohmann::json& extInfo) override;

private:
    int32_t WriteParcelForReceiveEvent(const uint32_t eventType,
        const uint32_t eventValue, const uint32_t listenerGroup, const nlohmann::json& extInfo, MessageParcel& data);
    DISALLOW_COPY_AND_MOVE(ResSchedEventListenerProxy);
    static inline BrokerDelegator<ResSchedEventListenerProxy> delegator_;
};
} // namespace ResourceSchedule
} // namespace OHOS
#endif // RESSCHED_INTERFACES_INNERKITS_RESSCHED_CLIENT_INCLUDE_RES_SCHED_EVENT_LISTENER_PROXY_H
