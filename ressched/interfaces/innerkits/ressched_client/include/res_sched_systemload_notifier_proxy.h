/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef RESSCHED_INTERFACES_INNERKITS_RESSCHED_CLIENT_INCLUDE_RES_SCHED_SYSTEMLOAD_NOTIFIER_PROXY_H
#define RESSCHED_INTERFACES_INNERKITS_RESSCHED_CLIENT_INCLUDE_RES_SCHED_SYSTEMLOAD_NOTIFIER_PROXY_H

#include "ires_sched_systemload_notifier.h"

#include "iremote_proxy.h"

namespace OHOS {
namespace ResourceSchedule {
class ResSchedSystemloadNotifierProxy : public IRemoteProxy<IResSchedSystemloadNotifier> {
public:
    explicit ResSchedSystemloadNotifierProxy(const sptr<IRemoteObject>& impl)
        : IRemoteProxy<IResSchedSystemloadNotifier>(impl) {}
    virtual ~ResSchedSystemloadNotifierProxy() = default;

    void OnSystemloadLevel(int32_t level) override;

private:
    DISALLOW_COPY_AND_MOVE(ResSchedSystemloadNotifierProxy);
    static inline BrokerDelegator<ResSchedSystemloadNotifierProxy> delegator_;
};
} // namespace ResourceSchedule
} // namespace OHOS
#endif // RESSCHED_INTERFACES_INNERKITS_RESSCHED_CLIENT_INCLUDE_RES_SCHED_SYSTEMLOAD_NOTIFIER_PROXY_H
