/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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
#include "iremote_proxy.h"       // for IRemoteProxy
#include "ires_sched_service.h"  // for IResSchedService
#include "nlohmann/json.hpp"     // for Value
#include "nocopyable.h"          // for DISALLOW_COPY_AND_MOVE

namespace OHOS { class IRemoteObject; }

namespace OHOS {
namespace ResourceSchedule {
class ResSchedServiceProxy : public IRemoteProxy<IResSchedService> {
public:
    /**
     * @brief Report resource data to the resource schedule service through inter-process communication.
     *
     * @param resType Indicates the resource type, all of the type have listed in res_type.h.
     * @param value Indicates the value of the resource type, defined by the developers.
     * @param payload Indicates the context info of the resource type event.
     */
    void ReportData(uint32_t resType, int64_t value, const nlohmann::json& payload) override;

    /**
     * @brief Kill process with pid.
     *
     * @param payload Indicates the context info of the kill message.
     */
    int32_t KillProcess(const nlohmann::json& payload) override;

    /**
     * @brief Register systemload remote listener.
     *
     * @param notifier remote listener object
     */
    void RegisterSystemloadNotifier(const sptr<IRemoteObject>& notifier) override;

    /**
     * @brief UnRegister systemload remote listener.
     */
    void UnRegisterSystemloadNotifier() override;

    /**
     * @brief client get systemload level.
     */
    int32_t GetSystemloadLevel() override;

    /**
     * @brief is allowed appliacation preload through resource scheduling services.
     *
     * @param bundleName bundleName of the application.
     */
    bool IsAllowedAppPreload(const std::string& bundleName, int32_t preloadMode) override;
public:
    /**
     * @brief Construct a new ResSchedServiceProxy object.
     *
     * @param impl RemoteObject.
     */
    explicit ResSchedServiceProxy(const sptr<IRemoteObject> &impl) : IRemoteProxy<IResSchedService>(impl) {}

    /**
     * @brief Destroy the ResSchedServiceProxy object
     */
    virtual ~ResSchedServiceProxy() {}

private:
    DISALLOW_COPY_AND_MOVE(ResSchedServiceProxy);
    static inline BrokerDelegator<ResSchedServiceProxy> delegator_;
};
} // namespace ResourceSchedule
} // namespace OHOS

#endif // RESSCHED_INTERFACES_INNERKITS_RESSCHED_CLIENT_INCLUDE_RES_SCHED_SERVICE_PROXY_H
