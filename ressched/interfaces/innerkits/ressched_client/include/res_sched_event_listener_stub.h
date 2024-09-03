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

#ifndef RESSCHED_INTERFACES_INNERKITS_RESSCHED_CLIENT_INCLUDE_RES_SCHED_EVENT_LISTENER_STUB_H
#define RESSCHED_INTERFACES_INNERKITS_RESSCHED_CLIENT_INCLUDE_RES_SCHED_EVENT_LISTENER_STUB_H

#include "ires_sched_event_listener.h"

#include "iremote_stub.h"
#include "message_parcel.h"

namespace OHOS {
namespace ResourceSchedule {
class ResSchedEventListenerStub : public IRemoteStub<IResSchedEventListener> {
public:
    ResSchedEventListenerStub() = default;
    virtual ~ResSchedEventListenerStub() = default;

    virtual int32_t OnRemoteRequest(uint32_t code, MessageParcel& data,
        MessageParcel& reply, MessageOption& option) override;

private:
    DISALLOW_COPY_AND_MOVE(ResSchedEventListenerStub);
    int32_t OnReceiveEventInner(MessageParcel& data, MessageParcel& reply);
    int32_t ParseAndCheckEventParcel(MessageParcel& data, uint32_t& eventType,
        uint32_t& eventValue, uint32_t listenerGroup, std::string& extInfo);
};
} // namespace ResourceSchedule
} // namespace OHOS
#endif // RESSCHED_INTERFACES_INNERKITS_RESSCHED_CLIENT_INCLUDE_RES_SCHED_EVENT_LISTENER_STUB_H
