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

#include "socperf_proxy.h"

namespace OHOS {
namespace SOCPERF {
void SocPerfProxy::PerfRequest(int cmdId, const std::string& msg)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInt32(cmdId);
    data.WriteString(msg);
    Remote()->SendRequest(TRANS_IPC_ID_PERF_REQUEST, data, reply, option);
}

void SocPerfProxy::PerfRequestEx(int cmdId, bool onOffTag, const std::string& msg)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInt32(cmdId);
    data.WriteBool(onOffTag);
    data.WriteString(msg);
    Remote()->SendRequest(TRANS_IPC_ID_PERF_REQUEST_EX, data, reply, option);
}

void SocPerfProxy::PowerRequest(int cmdId, const std::string& msg)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInt32(cmdId);
    data.WriteString(msg);
    Remote()->SendRequest(TRANS_IPC_ID_POWER_REQUEST, data, reply, option);
}

void SocPerfProxy::PowerRequestEx(int cmdId, bool onOffTag, const std::string& msg)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInt32(cmdId);
    data.WriteBool(onOffTag);
    data.WriteString(msg);
    Remote()->SendRequest(TRANS_IPC_ID_POWER_REQUEST_EX, data, reply, option);
}

void SocPerfProxy::PowerLimitBoost(bool onOffTag, const std::string& msg)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteBool(onOffTag);
    data.WriteString(msg);
    Remote()->SendRequest(TRANS_IPC_ID_POWER_LIMIT_BOOST_FREQ, data, reply, option);
}

void SocPerfProxy::ThermalRequest(int cmdId, const std::string& msg)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInt32(cmdId);
    data.WriteString(msg);
    Remote()->SendRequest(TRANS_IPC_ID_THERMAL_REQUEST, data, reply, option);
}

void SocPerfProxy::ThermalRequestEx(int cmdId, bool onOffTag, const std::string& msg)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInt32(cmdId);
    data.WriteBool(onOffTag);
    data.WriteString(msg);
    Remote()->SendRequest(TRANS_IPC_ID_THERMAL_REQUEST_EX, data, reply, option);
}

void SocPerfProxy::ThermalLimitBoost(bool onOffTag, const std::string& msg)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteBool(onOffTag);
    data.WriteString(msg);
    Remote()->SendRequest(TRANS_IPC_ID_THERMAL_LIMIT_BOOST_FREQ, data, reply, option);
}
} // namespace SOCPERF
} // namespace OHOS