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

#ifndef RESSCHED_INTERFACES_INNERKITS_RESSCHED_CLIENT_INCLUDE_RES_SCHED_SERVICE_PROXY_H
#define RESSCHED_INTERFACES_INNERKITS_RESSCHED_CLIENT_INCLUDE_RES_SCHED_SERVICE_PROXY_H

#include <cstdint>               // for int64_t, uint32_t
#include <functional>            // for __base
#include "iremote_broker.h"      // for BrokerDelegator
#include "iremote_proxy.h"       // for IRemoteProxy
#include "ires_sched_service.h"  // for IResSchedService
#include "nlohmann/json.hpp"          // for Value
#include "nocopyable.h"          // for DISALLOW_COPY_AND_MOVE
#include "refbase.h"             // for sptr
namespace OHOS { class IRemoteObject; }

namespace OHOS {
namespace ResourceSchedule {
class ResSchedServiceProxy : public IRemoteProxy<IResSchedService> {
public:
    explicit ResSchedServiceProxy(const sptr<IRemoteObject> &impl) : IRemoteProxy<IResSchedService>(impl) {}
    virtual ~ResSchedServiceProxy() {}

    void ReportData(uint32_t resType, int64_t value, const nlohmann::json& payload) override;

private:
    DISALLOW_COPY_AND_MOVE(ResSchedServiceProxy);
    static inline BrokerDelegator<ResSchedServiceProxy> delegator_;
};
} // namespace ResourceSchedule
} // namespace OHOS

#endif // RESSCHED_INTERFACES_INNERKITS_RESSCHED_CLIENT_INCLUDE_RES_SCHED_SERVICE_PROXY_H
