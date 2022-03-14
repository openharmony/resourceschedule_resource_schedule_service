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

#include "socperf_client.h"

namespace OHOS {
namespace SOCPERF {
SocPerfClient& SocPerfClient::GetInstance()
{
    static SocPerfClient instance;
    return instance;
}

bool SocPerfClient::CheckClientValid()
{
    if (client) {
        return true;
    }

    sptr<ISystemAbilityManager> samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (!samgr) {
        SOC_PERF_LOGE("Failed to get SystemAbilityManager.");
        return false;
    }

    sptr<IRemoteObject> object = samgr->GetSystemAbility(SOC_PERF_SERVICE_SA_ID);
    if (!object) {
        SOC_PERF_LOGE("Failed to get SystemAbility[1906] .");
        return false;
    }

    client = iface_cast<ISocPerfService>(object);
    if (!client || !client->AsObject()) {
        SOC_PERF_LOGE("Failed to get SocPerfClient.");
        return false;
    }

    recipient_ = new SocPerfDeathRecipient(*this);
    if (!recipient_) {
        return false;
    }
    client->AsObject()->AddDeathRecipient(recipient_);

    return true;
}

void SocPerfClient::ResetClient()
{
    if (client && client->AsObject()) {
        client->AsObject()->RemoveDeathRecipient(recipient_);
    }
    client = nullptr;
}

SocPerfClient::SocPerfDeathRecipient::SocPerfDeathRecipient(SocPerfClient &socPerfClient)
    : socPerfClient_(socPerfClient) {};

SocPerfClient::SocPerfDeathRecipient::~SocPerfDeathRecipient() {};

void SocPerfClient::SocPerfDeathRecipient::OnRemoteDied(const wptr<IRemoteObject> &remote)
{
    socPerfClient_.ResetClient();
}

std::string SocPerfClient::AddPidAndTidInfo(const std::string& msg)
{
    std::string str;
    int pid = getpid();
    int tid = gettid();
    str.append("pid=").append(std::to_string(pid)).append("|");
    str.append("tid=").append(std::to_string(tid));
    if (msg.size() > 0) {
        str.append("|").append(msg);
    }
    return str;
}

void SocPerfClient::PerfRequest(int cmdId, const std::string& msg)
{
    if (!CheckClientValid()) {
        return;
    }
    std::string newMsg = AddPidAndTidInfo(msg);
    client->PerfRequest(cmdId, newMsg);
}

void SocPerfClient::PerfRequestEx(int cmdId, bool onOffTag, const std::string& msg)
{
    if (!CheckClientValid()) {
        return;
    }
    std::string newMsg = AddPidAndTidInfo(msg);
    client->PerfRequestEx(cmdId, onOffTag, newMsg);
}

void SocPerfClient::PowerRequest(int cmdId, const std::string& msg)
{
    if (!CheckClientValid()) {
        return;
    }
    std::string newMsg = AddPidAndTidInfo(msg);
    client->PowerRequest(cmdId, newMsg);
}

void SocPerfClient::PowerRequestEx(int cmdId, bool onOffTag, const std::string& msg)
{
    if (!CheckClientValid()) {
        return;
    }
    std::string newMsg = AddPidAndTidInfo(msg);
    client->PowerRequestEx(cmdId, onOffTag, newMsg);
}

void SocPerfClient::PowerLimitBoost(bool onOffTag, const std::string& msg)
{
    if (!CheckClientValid()) {
        return;
    }
    std::string newMsg = AddPidAndTidInfo(msg);
    client->PowerLimitBoost(onOffTag, newMsg);
}

void SocPerfClient::ThermalRequest(int cmdId, const std::string& msg)
{
    if (!CheckClientValid()) {
        return;
    }
    std::string newMsg = AddPidAndTidInfo(msg);
    client->ThermalRequest(cmdId, newMsg);
}

void SocPerfClient::ThermalRequestEx(int cmdId, bool onOffTag, const std::string& msg)
{
    if (!CheckClientValid()) {
        return;
    }
    std::string newMsg = AddPidAndTidInfo(msg);
    client->ThermalRequestEx(cmdId, onOffTag, newMsg);
}

void SocPerfClient::ThermalLimitBoost(bool onOffTag, const std::string& msg)
{
    if (!CheckClientValid()) {
        return;
    }
    std::string newMsg = AddPidAndTidInfo(msg);
    client->ThermalLimitBoost(onOffTag, newMsg);
}
} // namespace SOCPERF
} // namespace OHOS
