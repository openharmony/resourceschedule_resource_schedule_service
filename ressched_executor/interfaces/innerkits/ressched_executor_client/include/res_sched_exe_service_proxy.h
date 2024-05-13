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

#ifndef RESSCHED_EXECUTOR_INTERFACES_INNERKITS_RESSCHED_EXECUTOR_CLIENT_INCLUDE_RES_SCHED_EXE_SERVICE_PROXY_H
#define RESSCHED_EXECUTOR_INTERFACES_INNERKITS_RESSCHED_EXECUTOR_CLIENT_INCLUDE_RES_SCHED_EXE_SERVICE_PROXY_H

#include <cstdint>

#include "iremote_proxy.h"
#include "message_parcel.h"
#include "message_option.h"
#include "nocopyable.h"

#include "ires_sched_exe_service.h"

namespace OHOS { class IRemoteObject; }

namespace OHOS {
namespace ResourceSchedule {
class ResSchedExeServiceProxy : public IRemoteProxy<IResSchedExeService> {
public:
    /**
     * @brief Send request sync to the ressched_executor through inter-process communication.
     *
     * @param resType Indicates the resource type, all of the type have listed in res_exe_type.h.
     * @param value Indicates the value of the resource type, defined by the developers.
     * @param context Indicates the context info of the resource type event.
     * @param reply Indicates the context info of the ipc reply.
     * @return function result
     */
    int32_t SendRequestSync(uint32_t resType, int64_t value,
        const nlohmann::json& context, nlohmann::json& reply) override;

    /**
     * @brief Send request async to the ressched_executor through inter-process communication.
     *
     * @param resType Indicates the resource type, all of the type have listed in res_exe_type.h.
     * @param value Indicates the value of the resource type, defined by the developers.
     * @param context Indicates the context info of the resource type event.
     */
    void SendRequestAsync(uint32_t resType, int64_t value, const nlohmann::json& context) override;

    /**
     * @brief Send kill process request async to the ressched_executor.
     *
     * @param pid the pid whiche will be killed.
     */
    int32_t KillProcess(pid_t pid) override;

public:
    /**
     * @brief Construct a new ResSchedExeServiceProxy object.
     *
     * @param impl RemoteObject.
     */
    explicit ResSchedExeServiceProxy(const sptr<IRemoteObject> &impl) : IRemoteProxy<IResSchedExeService>(impl) {}

    /**
     * @brief Destroy the ResSchedExeServiceProxy object
     */
    virtual ~ResSchedExeServiceProxy() {}

private:
    int32_t MakeUpParcel(MessageParcel& data, uint32_t resType, int64_t value, const nlohmann::json& context);
    int32_t SendDebugCommand(MessageOption& option);

    static inline BrokerDelegator<ResSchedExeServiceProxy> delegator_;
    DISALLOW_COPY_AND_MOVE(ResSchedExeServiceProxy);
};
} // namespace ResourceSchedule
} // namespace OHOS

#endif // RESSCHED_EXECUTOR_INTERFACES_INNERKITS_RESSCHED_EXECUTOR_CLIENT_INCLUDE_RES_SCHED_EXE_SERVICE_PROXY_H
