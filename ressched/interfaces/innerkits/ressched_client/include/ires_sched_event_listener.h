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

#ifndef RESSCHED_INTERFACES_INNERKITS_RESSCHED_CLIENT_INCLUDE_IRES_SCHED_EVENT_LISTENER_H
#define RESSCHED_INTERFACES_INNERKITS_RESSCHED_CLIENT_INCLUDE_IRES_SCHED_EVENT_LISTENER_H

#include "iremote_broker.h"

namespace OHOS {
namespace ResourceSchedule {
class IResSchedEventListener : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.ResourceSchedule.IResSchedEventListener");

    /**
     * @brief receive event frm rss.
     *
     * @param eventType Indicates the event type.
     * @param eventValue Indicates the event value.
     * @param extInfo Indicates the extra info.
     */
    virtual void OnReceiveEvent(uint32_t eventType, uint32_t eventValue, const nlohmann::json& extInfo) = 0;
};
} // namespace ResourceSchedule
} // namespace OHOS

#endif // RESSCHED_INTERFACES_INNERKITS_RESSCHED_CLIENT_INCLUDE_IRES_SCHED_EVENT_LISTENER_H
