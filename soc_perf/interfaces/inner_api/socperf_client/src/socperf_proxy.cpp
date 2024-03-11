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

#include "socperf_ipc_interface_code.h"
#include "socperf_proxy.h"
#include "socperf_log.h"

namespace OHOS {
namespace SOCPERF {
void SocPerfProxy::PerfRequest(int32_t cmdId, const std::string& msg)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option = { MessageOption::TF_ASYNC };
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        return;
    }
    data.WriteInt32(cmdId);
    data.WriteString(msg);
    Remote()->SendRequest(static_cast<uint32_t>(SocPerfInterfaceCode::TRANS_IPC_ID_PERF_REQUEST),
        data, reply, option);
}

void SocPerfProxy::PerfRequestEx(int32_t cmdId, bool onOffTag, const std::string& msg)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option = { MessageOption::TF_ASYNC };
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        return;
    }
    data.WriteInt32(cmdId);
    data.WriteBool(onOffTag);
    data.WriteString(msg);
    Remote()->SendRequest(static_cast<uint32_t>(SocPerfInterfaceCode::TRANS_IPC_ID_PERF_REQUEST_EX),
        data, reply, option);
}

void SocPerfProxy::PowerLimitBoost(bool onOffTag, const std::string& msg)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option = { MessageOption::TF_ASYNC };
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        return;
    }
    data.WriteBool(onOffTag);
    data.WriteString(msg);
    Remote()->SendRequest(static_cast<uint32_t>(SocPerfInterfaceCode::TRANS_IPC_ID_POWER_LIMIT_BOOST_FREQ),
        data, reply, option);
}

void SocPerfProxy::ThermalLimitBoost(bool onOffTag, const std::string& msg)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option = { MessageOption::TF_ASYNC };
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        return;
    }
    data.WriteBool(onOffTag);
    data.WriteString(msg);
    Remote()->SendRequest(static_cast<uint32_t>(SocPerfInterfaceCode::TRANS_IPC_ID_THERMAL_LIMIT_BOOST_FREQ),
        data, reply, option);
}

void SocPerfProxy::LimitRequest(int32_t clientId,
    const std::vector<int32_t>& tags, const std::vector<int64_t>& configs, const std::string& msg)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option = { MessageOption::TF_ASYNC };
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        return;
    }
    data.WriteInt32(clientId);
    data.WriteInt32Vector(tags);
    data.WriteInt64Vector(configs);
    data.WriteString(msg);
    Remote()->SendRequest(static_cast<uint32_t>(SocPerfInterfaceCode::TRANS_IPC_ID_LIMIT_REQUEST),
        data, reply, option);
}

void SocPerfProxy::SetRequestStatus(bool status, const std::string &msg)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option = { MessageOption::TF_ASYNC };
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        return;
    }
    if (!data.WriteBool(status)) {
        SOC_PERF_LOGE("Failed to write status: %{public}d", status);
        return;
    }
    if (!data.WriteString(msg)) {
        SOC_PERF_LOGE("Failed to write msg: %{public}s", msg.c_str());
        return;
    }
    Remote()->SendRequest(static_cast<uint32_t>(SocPerfInterfaceCode::TRANS_IPC_ID_SET_STATUS), data, reply, option);
}

void SocPerfProxy::SetThermalLevel(int32_t level)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option = { MessageOption::TF_ASYNC };
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        SOC_PERF_LOGE("Failed to write descriptor");
        return;
    }
    if (!data.WriteInt32(level)) {
        SOC_PERF_LOGE("Failed to write level: %{public}d", level);
        return;
    }
    Remote()->SendRequest(static_cast<uint32_t>(SocPerfInterfaceCode::TRANS_IPC_ID_SET_THERMAL_LEVEL),
        data, reply, option);
}
} // namespace SOCPERF
} // namespace OHOS
