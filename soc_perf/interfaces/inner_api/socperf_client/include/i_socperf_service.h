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

#ifndef SOC_PERF_INTERFACES_INNER_API_SOCPERF_CLIENT_INCLUDE_I_SOCPERF_SERVICE_H
#define SOC_PERF_INTERFACES_INNER_API_SOCPERF_CLIENT_INCLUDE_I_SOCPERF_SERVICE_H

#include <string>
#include "iremote_broker.h"
#include "iremote_stub.h"
#include "iremote_proxy.h"
#include "iremote_object.h"
#include "system_ability.h"
#include "system_ability_definition.h"
#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "socperf_action_type.h"

namespace OHOS {
namespace SOCPERF {
class ISocPerfService : public IRemoteBroker {
public:
    /**
     * @brief Sending a performance request.
     *
     * @param cmdId scene id defined in config file.
     * @param msg Additional string info, which is used for other extensions.
     */
    virtual void PerfRequest(int32_t cmdId, const std::string& msg) = 0;

    /**
     * @brief Sending a performance request.
     *
     * @param cmdId Scene id defined in config file.
     * @param onOffTag Indicates the start of end of a long-term frequency increase event.
     * @param msg Additional string info, which is used for other extensions.
     */
    virtual void PerfRequestEx(int32_t cmdId, bool onOffTag, const std::string& msg) = 0;

    /**
     * @brief Sending a power limit boost request.
     *
     * @param onOffTag Indicates the start of end of a power limit boost event.
     * @param msg Additional string info, which is used for other extensions.
     */
    virtual void PowerLimitBoost(bool onOffTag, const std::string& msg) = 0;

    /**
     * @brief Sending a thermal limit boost request.
     *
     * @param onOffTag Indicates the start of end of a thermal limit boost event.
     * @param msg Additional string info, which is used for other extensions.
     */
    virtual void ThermalLimitBoost(bool onOffTag, const std::string& msg) = 0;

    /**
     * @brief Sending a limit request.
     *
     * @param clientId Used to indentify the caller of frequency limiting, such as
     * the thermal module or power consumption module.
     * @param configs Indicates the specific value to be limited.
     * @param msg Additional string info, which is used for other extensions.
     */
    virtual void LimitRequest(int32_t clientId,
        const std::vector<int32_t>& tags, const std::vector<int64_t>& configs, const std::string& msg) = 0;

public:
    enum {
        TRANS_IPC_ID_PERF_REQUEST             = 0x0001,
        TRANS_IPC_ID_PERF_REQUEST_EX          = 0x0002,
        TRANS_IPC_ID_POWER_LIMIT_BOOST_FREQ   = 0x0005,
        TRANS_IPC_ID_THERMAL_LIMIT_BOOST_FREQ = 0x0008,
        TRANS_IPC_ID_LIMIT_REQUEST            = 0x0009,
    };

public:
    DECLARE_INTERFACE_DESCRIPTOR(u"Resourceschedule.ISocPerfService");
};
} // namespace SOCPERF
} // namespace OHOS

#endif // SOC_PERF_INTERFACES_INNER_API_SOCPERF_CLIENT_INCLUDE_I_SOCPERF_SERVICE_H
