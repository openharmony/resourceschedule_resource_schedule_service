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

#ifndef SOC_PERF_INCLUDE_SERVER_SOCPERF_SERVER_H
#define SOC_PERF_INCLUDE_SERVER_SOCPERF_SERVER_H

#include "i_socperf_service.h"
#include "socperf_stub.h"
#include "socperf.h"

namespace OHOS {
namespace SOCPERF {
class SocPerfServer : public SystemAbility, public SocPerfStub {
DECLARE_SYSTEM_ABILITY(SocPerfServer);

public:
    void PerfRequest(int32_t cmdId, const std::string& msg) override;
    void PerfRequestEx(int32_t cmdId, bool onOffTag, const std::string& msg) override;
    void PowerRequest(int32_t cmdId, const std::string& msg) override;
    void PowerRequestEx(int32_t cmdId, bool onOffTag, const std::string& msg) override;
    void PowerLimitBoost(bool onOffTag, const std::string& msg) override;
    void ThermalRequest(int32_t cmdId, const std::string& msg) override;
    void ThermalRequestEx(int32_t cmdId, bool onOffTag, const std::string& msg) override;
    void ThermalLimitBoost(bool onOffTag, const std::string& msg) override;

public:
    SocPerfServer(int32_t systemAbilityId, bool runOnCreate)
        : SystemAbility(systemAbilityId, runOnCreate) {}
    ~SocPerfServer() {}

protected:
    void OnStart() override;
    void OnStop() override;

private:
    SocPerf socPerf;
};
} // namespace SOCPERF
} // namespace OHOS

#endif // SOC_PERF_INCLUDE_SERVER_SOCPERF_SERVER_H
