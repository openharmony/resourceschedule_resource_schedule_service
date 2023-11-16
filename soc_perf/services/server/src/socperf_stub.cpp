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
#include <cstdint>              // for int32_t
#include <iosfwd>               // for string
#include <string>               // for basic_string, operator!=, u16string
#include <vector>               // for vector
#include "accesstoken_kit.h"
#include "ipc_skeleton.h"
#include "tokenid_kit.h"
#include "socperf_ipc_interface_code.h"
#include "socperf_log.h"

namespace OHOS {
namespace SOCPERF {
int32_t SocPerfStub::OnRemoteRequest(uint32_t code, MessageParcel &data,
    MessageParcel &reply, MessageOption &option)
{
    auto remoteDescriptor = data.ReadInterfaceToken();
    if (GetDescriptor() != remoteDescriptor || !HasPerfPermission()) {
        return ERR_INVALID_STATE;
    }
    switch (code) {
        case static_cast<uint32_t>(SocPerfInterfaceCode::TRANS_IPC_ID_PERF_REQUEST): {
            int32_t cmdId = data.ReadInt32();
            std::string msg = data.ReadString();
            PerfRequest(cmdId, msg);
            break;
        }
        case static_cast<uint32_t>(SocPerfInterfaceCode::TRANS_IPC_ID_PERF_REQUEST_EX): {
            int32_t cmdId = data.ReadInt32();
            bool onOffTag = data.ReadBool();
            std::string msg = data.ReadString();
            PerfRequestEx(cmdId, onOffTag, msg);
            break;
        }
        case static_cast<uint32_t>(SocPerfInterfaceCode::TRANS_IPC_ID_POWER_LIMIT_BOOST_FREQ): {
            bool onOffTag = data.ReadBool();
            std::string msg = data.ReadString();
            PowerLimitBoost(onOffTag, msg);
            break;
        }
        case static_cast<uint32_t>(SocPerfInterfaceCode::TRANS_IPC_ID_THERMAL_LIMIT_BOOST_FREQ): {
            bool onOffTag = data.ReadBool();
            std::string msg = data.ReadString();
            ThermalLimitBoost(onOffTag, msg);
            break;
        }
        case static_cast<uint32_t>(SocPerfInterfaceCode::TRANS_IPC_ID_LIMIT_REQUEST): {
            int32_t clientId = data.ReadInt32();
            std::vector<int32_t> tags;
            data.ReadInt32Vector(&tags);
            std::vector<int64_t> configs;
            data.ReadInt64Vector(&configs);
            std::string msg = data.ReadString();
            LimitRequest(clientId, tags, configs, msg);
            break;
        }
        default:
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
    return ERR_OK;
}

bool SocPerfStub::HasPerfPermission()
{
    uint32_t accessToken = IPCSkeleton::GetCallingTokenID();
    auto tokenType = Security::AccessToken::AccessTokenKit::GetTokenTypeFlag(accessToken);
    if (int(tokenType) == OHOS::Security::AccessToken::ATokenTypeEnum::TOKEN_HAP) {
        uint64_t fullTokenId = IPCSkeleton::GetCallingFullTokenID();
        if (!Security::AccessToken::TokenIdKit::IsSystemAppByFullTokenID(fullTokenId)) {
            SOC_PERF_LOGE("Invalid Permission to SocPerf, token[%{public}u] tokenType[%{public}d]",
                accessToken, (int)tokenType);
            return false;
        }
    }
    return true;
}
} // namespace SOCPERF
} // namespace OHOS
