/*
 * Copyright (c) 2023-2023 Huawei Device Co., Ltd.
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

#ifndef RESSCHED_SCHED_CONTROLLER_OBSERVER_INCLUDE_ABILITY_RUNTIME_CONNECTION_OBSERVER_H
#define RESSCHED_SCHED_CONTROLLER_OBSERVER_INCLUDE_ABILITY_RUNTIME_CONNECTION_OBSERVER_H

#include "nlohmann/json.hpp"
#include "connection_observer.h"

namespace OHOS {
namespace ResourceSchedule {
class ConnectionSubscriber : public AbilityRuntime::ConnectionObserver {
public:
    virtual void OnExtensionConnected(const AbilityRuntime::ConnectionData& data) override;
    virtual void OnExtensionDisconnected(const AbilityRuntime::ConnectionData& data) override;
    virtual void OnDlpAbilityOpened(const AbilityRuntime::DlpStateData& data) override;
    virtual void OnDlpAbilityClosed(const AbilityRuntime::DlpStateData& data) override;
    virtual void OnServiceDied() override;

private:
    void MarshallingConnectionData(const AbilityRuntime::ConnectionData& data, nlohmann::json &payload);
    void MarshallingDlpStateData(const AbilityRuntime::DlpStateData& data, nlohmann::json &payload);
};
} // namespace ResourceSchedule
} // namespace OHOS
#endif // RESSCHED_SCHED_CONTROLLER_OBSERVER_INCLUDE_ABILITY_RUNTIME_CONNECTION_OBSERVER_H