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

#ifndef RESSCHED_INTERFACES_INNERKITS_RESSCHED_CLIENT_INCLUDE_IRES_SCHED_SERVICE_H
#define RESSCHED_INTERFACES_INNERKITS_RESSCHED_CLIENT_INCLUDE_IRES_SCHED_SERVICE_H

#include "iremote_broker.h"
#include "nlohmann/json.hpp"

namespace OHOS {
namespace ResourceSchedule {
class IResSchedService : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.ResourceSchedule.ResSchedService");

    /**
     * @brief Report resource data to the resource schedule service through inter-process communication.
     *
     * @param resType Indicates the resource type, all of the type have listed in res_type.h.
     * @param value Indicates the value of the resource type, defined by the developers.
     * @param payload Indicates the context info of the resource type event.
     */
    virtual void ReportData(uint32_t resType, int64_t value, const nlohmann::json& payload) = 0;

    /**
     * @brief Kill process with pid.
     *
     * @param payload Indicates the context info of the kill message.
     */
    virtual int32_t KillProcess(const nlohmann::json& payload) = 0;
    enum : uint32_t {
        REPORT_DATA = 1,
        KILL_PROCESS = 2,
    };
};
} // namespace ResourceSchedule
} // namespace OHOS

#endif // RESSCHED_INTERFACES_INNERKITS_RESSCHED_CLIENT_INCLUDE_IRES_SCHED_SERVICE_H
