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

#include "socperf_stub.h"

namespace OHOS {
namespace SOCPERF {
int SocPerfStub::OnRemoteRequest(uint32_t code, MessageParcel &data,
    MessageParcel &reply, MessageOption &option)
{
    auto remoteDescriptor = data.ReadInterfaceToken();
    if (GetDescriptor() != remoteDescriptor) {
        return ERR_INVALID_STATE;
    }
    switch (code) {
        case TRANS_IPC_ID_PERF_REQUEST: {
            int cmdId = data.ReadInt32();
            std::string msg = data.ReadString();
            PerfRequest(cmdId, msg);
            return 0;
        }
        case TRANS_IPC_ID_PERF_REQUEST_EX: {
            int cmdId = data.ReadInt32();
            bool onOffTag = data.ReadBool();
            std::string msg = data.ReadString();
            PerfRequestEx(cmdId, onOffTag, msg);
            return 0;
        }
        case TRANS_IPC_ID_POWER_REQUEST: {
            int cmdId = data.ReadInt32();
            std::string msg = data.ReadString();
            PowerRequest(cmdId, msg);
            return 0;
        }
        case TRANS_IPC_ID_POWER_REQUEST_EX: {
            int cmdId = data.ReadInt32();
            bool onOffTag = data.ReadBool();
            std::string msg = data.ReadString();
            PowerRequestEx(cmdId, onOffTag, msg);
            return 0;
        }
        case TRANS_IPC_ID_POWER_LIMIT_BOOST_FREQ: {
            bool onOffTag = data.ReadBool();
            std::string msg = data.ReadString();
            PowerLimitBoost(onOffTag, msg);
            return 0;
        }
        case TRANS_IPC_ID_THERMAL_REQUEST: {
            int cmdId = data.ReadInt32();
            std::string msg = data.ReadString();
            ThermalRequest(cmdId, msg);
            return 0;
        }
        case TRANS_IPC_ID_THERMAL_REQUEST_EX: {
            int cmdId = data.ReadInt32();
            bool onOffTag = data.ReadBool();
            std::string msg = data.ReadString();
            ThermalRequestEx(cmdId, onOffTag, msg);
            return 0;
        }
        case TRANS_IPC_ID_THERMAL_LIMIT_BOOST_FREQ: {
            bool onOffTag = data.ReadBool();
            std::string msg = data.ReadString();
            ThermalLimitBoost(onOffTag, msg);
            return 0;
        }
        default:
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
}
} // namespace SOCPERF
} // namespace OHOS
