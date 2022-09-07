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

#include <file_ex.h>
#include <string_ex.h>
#include "i_socperf_service.h"
#include "singleton.h"
#include "socperf_stub.h"
#include "socperf.h"

namespace OHOS {
namespace SOCPERF {
class SocPerfServer : public SystemAbility, public SocPerfStub,
    public std::enable_shared_from_this<SocPerfServer> {
DISALLOW_COPY_AND_MOVE(SocPerfServer);
DECLARE_SYSTEM_ABILITY(SocPerfServer);
DECLARE_DELAYED_SINGLETON(SocPerfServer);

public:
    void PerfRequest(int32_t cmdId, const std::string& msg) override;
    void PerfRequestEx(int32_t cmdId, bool onOffTag, const std::string& msg) override;
    void PowerLimitBoost(bool onOffTag, const std::string& msg) override;
    void ThermalLimitBoost(bool onOffTag, const std::string& msg) override;
    void LimitRequest(int32_t clientId,
        const std::vector<int32_t>& tags, const std::vector<int64_t>& configs, const std::string& msg) override;
    int32_t Dump(int32_t fd, const std::vector<std::u16string>& args) override;

public:
    SocPerfServer(int32_t systemAbilityId, bool runOnCreate);

protected:
    void OnStart() override;
    void OnStop() override;

private:
    SocPerf socPerf;
};
} // namespace SOCPERF
} // namespace OHOS

#endif // SOC_PERF_INCLUDE_SERVER_SOCPERF_SERVER_H
