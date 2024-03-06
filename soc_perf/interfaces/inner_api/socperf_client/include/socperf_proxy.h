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

#ifndef SOC_PERF_INTERFACES_INNER_API_SOCPERF_CLIENT_INCLUDE_SOCPERF_PROXY_H
#define SOC_PERF_INTERFACES_INNER_API_SOCPERF_CLIENT_INCLUDE_SOCPERF_PROXY_H

#include "i_socperf_service.h"

namespace OHOS {
namespace SOCPERF {
class SocPerfProxy : public IRemoteProxy<ISocPerfService> {
public:
    /**
     * @brief Sending a performance request.
     *
     * @param cmdId Scene id defined in config file.
     * @param msg Additional string info, which is used for other extensions.
     */
    void PerfRequest(int32_t cmdId, const std::string& msg) override;

    /**
     * @brief Sending a performance request.
     *
     * @param cmdId Scene id defined in config file.
     * @param onOffTag Indicates the start of end of a long-term frequency increase event.
     * @param msg Additional string info, which is used for other extensions.
     */
    void PerfRequestEx(int32_t cmdId, bool onOffTag, const std::string& msg) override;

    /**
     * @brief Sending a power limit boost request.
     *
     * @param onOffTag Indicates the start of end of a power limit boost event.
     * @param msg Additional string info, which is used for other extensions.
     */
    void PowerLimitBoost(bool onOffTag, const std::string& msg) override;

    /**
     * @brief Sending a thermal limit boost request.
     *
     * @param onOffTag Indicates the start of end of a thermal limit boost event.
     * @param msg Additional string info, which is used for other extensions.
     */
    void ThermalLimitBoost(bool onOffTag, const std::string& msg) override;

    /**
     * @brief Sending a limit request.
     *
     * @param clientId Used to indentify the caller of frequency limiting, such as
     * the thermal module or power consumption module.
     * @param configs Indicates the specific value to be limited.
     * @param msg Additional string info, which is used for other extensions.
     */
    void LimitRequest(int32_t clientId,
        const std::vector<int32_t>& tags, const std::vector<int64_t>& configs, const std::string& msg) override;

    /**
     * @brief set socperf server status, enable or disable
     *
     * @param status true means enable socperfserver, false means disable socperfserver
     * @param msg the reason why we need change socperfserver status
     */
    void SetRequestStatus(bool status, const std::string& msg) override;

    /**
     * @brief set thermal level intermal for perfquest
     *
     * @param level thermal level
     */
    void SetThermalLevel(int32_t level) override;

public:
    /**
     * @brief Construct a new SocPerfProxy object.
     *
     * @param impl RemoteObject.
     */
    explicit SocPerfProxy(const sptr<IRemoteObject> &impl)
        : IRemoteProxy<ISocPerfService>(impl) {}

    /**
     * @brief Destroy the SocPerfProxy object.
     *
     */
    virtual ~SocPerfProxy() {}

private:
    static inline BrokerDelegator<SocPerfProxy> delegator_;
};
} // namespace SOCPERF
} // namespace OHOS

#endif // SOC_PERF_INTERFACES_INNER_API_SOCPERF_CLIENT_INCLUDE_SOCPERF_PROXY_H
