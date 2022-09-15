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

#ifndef RESSCHED_SCHED_CONTROLLER_OBSERVER_INCLUDE_DEVICE_MOVEMENT_OBSERVER_H
#define RESSCHED_SCHED_CONTROLLER_OBSERVER_INCLUDE_DEVICE_MOVEMENT_OBSERVER_H

#include "iremote_stub.h"
#include "imovement_callback.h"
#include "movement_data_utils.h"

namespace OHOS {
namespace ResourceSchedule {
class DeviceMovementObserver : public IRemoteStub<Msdp::ImovementCallback> {
public:
    DISALLOW_COPY_AND_MOVE(DeviceMovementObserver);
    DeviceMovementObserver() = default;
    virtual ~DeviceMovementObserver() = default;
    int32_t OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;
    void OnMovementChanged(const Msdp::MovementDataUtils::MovementData &movementData) override;

private:
    int32_t OnReceiveDeviceMovementEvent(MessageParcel &data);
};
} // namespace ResourceSchedule
} // namespace OHOS
#endif // RESSCHED_SCHED_CONTROLLER_OBSERVER_INCLUDE_DEVICE_MOVEMENT_OBSERVER_H
