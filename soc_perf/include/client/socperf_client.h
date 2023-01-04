/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#ifndef SOC_PERF_INCLUDE_CLIENT_SOCPERF_CLIENT_H
#define SOC_PERF_INCLUDE_CLIENT_SOCPERF_CLIENT_H

#include <cstdint>             // for int32_t
#include <iosfwd>               // for string
#include <vector>               // for vector
#include "i_socperf_service.h"  // for ISocPerfService

namespace OHOS {
namespace SOCPERF {
class SocPerfClient {
public:
    void PerfRequest(int32_t cmdId, const std::string& msg);
    void PerfRequestEx(int32_t cmdId, bool onOffTag, const std::string& msg);
    void PowerLimitBoost(bool onOffTag, const std::string& msg);
    void ThermalLimitBoost(bool onOffTag, const std::string& msg);
    void LimitRequest(int32_t clientId,
        const std::vector<int32_t>& tags, const std::vector<int64_t>& configs, const std::string& msg);
    static SocPerfClient& GetInstance();
    void ResetClient();

private:
    SocPerfClient() {}
    ~SocPerfClient() {}

private:
    bool CheckClientValid();
    std::string AddPidAndTidInfo(const std::string& msg);

private:
    class SocPerfDeathRecipient : public IRemoteObject::DeathRecipient {
    public:
        explicit SocPerfDeathRecipient(SocPerfClient &socPerfClient);

        ~SocPerfDeathRecipient();

        void OnRemoteDied(const wptr<IRemoteObject> &object) override;

    private:
        SocPerfClient &socPerfClient_;
    };

private:
    std::mutex mutex_;
    sptr<ISocPerfService> client;
    sptr<SocPerfDeathRecipient> recipient_;
};
} // namespace SOCPERF
} // namespace OHOS

#endif // SOC_PERF_INCLUDE_CLIENT_SOCPERF_CLIENT_H
