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

#include "device_movement_observer.h"

#include "ressched_utils.h"
#include "res_sched_log.h"
#include "res_type.h"

namespace OHOS {
namespace ResourceSchedule {
int32_t DeviceMovementObserver::OnRemoteRequest(uint32_t code, MessageParcel &data,
    MessageParcel &reply, MessageOption &option)
{
    std::u16string descriptor = DeviceMovementObserver::GetDescriptor();
    std::u16string remoteDescriptor = data.ReadInterfaceToken();
    if (descriptor != remoteDescriptor) {
        RESSCHED_LOGE("DeviceMovementObserver::OnRemoteRequest failed, descriptor mismatch");
        return -1;
    }

    switch (code) {
        case static_cast<int32_t>(Msdp::ImovementCallback::MOVEMENT_CHANGE): {
            return OnReceiveDeviceMovementEvent(data);
        }
        default:
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
    return ERR_OK;
}

int32_t DeviceMovementObserver::OnReceiveDeviceMovementEvent(MessageParcel &data)
{
    int32_t type = data.ReadInt32();
    int32_t value = data.ReadInt32();
    Msdp::MovementDataUtils::MovementData movementData = {
        static_cast<Msdp::MovementDataUtils::MovementType>(type),
        static_cast<Msdp::MovementDataUtils::MovementValue>(value)
    };
    OnMovementChanged(movementData);
    return ERR_OK;
}

void DeviceMovementObserver::OnMovementChanged(const Msdp::MovementDataUtils::MovementData &movementData)
{
    RESSCHED_LOGD("enter DeviceMovementObserver::OnMovementChanged");
    nlohmann::json payload;
    payload["type"] = std::to_string(movementData.type);
    payload["value"] = std::to_string(movementData.value);
    ResSchedUtils::GetInstance().ReportDataInProcess(ResType::RES_TYPE_DEVICE_STILL_STATE_CHANGE,
        movementData.value, payload);
}
}
}
