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

#include "socperf_server.h"

namespace OHOS {
namespace SOCPERF {
REGISTER_SYSTEM_ABILITY_BY_ID(SocPerfServer, RESSCHEDD_SA_ID, true);

void SocPerfServer::OnStart()
{
    if (!Publish(this)) {
        SOC_PERF_LOGE("Register SystemAbility for SocPerf FAILED.");
        return;
    }
    SOC_PERF_LOGI("Register SystemAbility for SocPerf SUCCESS.");

    if (!socPerf.Init()) {
        SOC_PERF_LOGE("SocPerf Init FAILED");
        return;
    }
    SOC_PERF_LOGI("SocPerf Init SUCCESS.");
}

void SocPerfServer::OnStop()
{
}

void SocPerfServer::PerfRequest(int cmdId, const std::string& msg)
{
    socPerf.PerfRequest(cmdId, msg);
}

void SocPerfServer::PerfRequestEx(int cmdId, bool onOffTag, const std::string& msg)
{
    socPerf.PerfRequestEx(cmdId, onOffTag, msg);
}

void SocPerfServer::PowerRequest(int cmdId, const std::string& msg)
{
    socPerf.PowerRequest(cmdId, msg);
}

void SocPerfServer::PowerRequestEx(int cmdId, bool onOffTag, const std::string& msg)
{
    socPerf.PowerRequestEx(cmdId, onOffTag, msg);
}

void SocPerfServer::PowerLimitBoost(bool onOffTag, const std::string& msg)
{
    socPerf.PowerLimitBoost(onOffTag, msg);
}

void SocPerfServer::ThermalRequest(int cmdId, const std::string& msg)
{
    socPerf.ThermalRequest(cmdId, msg);
}

void SocPerfServer::ThermalRequestEx(int cmdId, bool onOffTag, const std::string& msg)
{
    socPerf.ThermalRequestEx(cmdId, onOffTag, msg);
}

void SocPerfServer::ThermalLimitBoost(bool onOffTag, const std::string& msg)
{
    socPerf.ThermalLimitBoost(onOffTag, msg);
}
} // namespace SOCPERF
} // namespace OHOS
