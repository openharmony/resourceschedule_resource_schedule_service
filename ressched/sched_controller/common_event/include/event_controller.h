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

#ifndef RESSCHED_SCHED_CONTROLLER_COMMON_EVENT_INCLUDE_EVENT_CONTROLLER_H
#define RESSCHED_SCHED_CONTROLLER_COMMON_EVENT_INCLUDE_EVENT_CONTROLLER_H

#include "common_event_subscriber.h"
#include "nlohmann/json.hpp"
#include "system_ability_status_change_stub.h"

#include "single_instance.h"

namespace OHOS {
namespace ResourceSchedule {
class EventController : public EventFwk::CommonEventSubscriber {
    DECLARE_SINGLE_INSTANCE_BASE(EventController);

public:
    EventController(const EventFwk::CommonEventSubscribeInfo &subscriberInfo)
        : CommonEventSubscriber(subscriberInfo) {}
    EventController() {}
    ~ EventController() {}

    virtual void OnReceiveEvent(const EventFwk::CommonEventData &data);
    void Init();
    void Stop();

    uint32_t resType_ = 0;
    int64_t value_ = 0;
    nlohmann::json payload_;

private:
class SystemAbilityStatusChangeListener : public OHOS::SystemAbilityStatusChangeStub {
public:
    SystemAbilityStatusChangeListener() {};
    virtual void OnAddSystemAbility(int32_t systemAbilityId, const std::string& deviceId) override;
    virtual void OnRemoveSystemAbility(int32_t systemAbilityId, const std::string& deviceId) override;
    void Stop();

private:
    std::shared_ptr<EventController> subscriber_ = nullptr;
};

    void HandleConnectivityChange(const EventFwk::Want &want, const int32_t &code, nlohmann::json &payload);
    void HandlePkgAddRemove(const EventFwk::Want &want, nlohmann::json &payload) const;
    int32_t GetUid(const int32_t &userId, const std::string &bundleName) const;
    void ReportDataInProcess(const uint32_t &resType, const int64_t &value, const nlohmann::json& payload);

    sptr<SystemAbilityStatusChangeListener> sysAbilityListener_ = nullptr;
};
}  // namespace ResourceSchedule
}  // namespace OHOS
#endif  // RESSCHED_SCHED_CONTROLLER_COMMON_EVENT_INCLUDE_EVENT_CONTROLLER_H