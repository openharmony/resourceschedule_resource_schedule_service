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

#ifndef RESSCHED_EXECUTOR_INTERFACES_INNERKITS_RESSCHED_EXECUTOR_CLIENT_INCLUDE_IRES_SCHED_EXE_SERVICE_H
#define RESSCHED_EXECUTOR_INTERFACES_INNERKITS_RESSCHED_EXECUTOR_CLIENT_INCLUDE_IRES_SCHED_EXE_SERVICE_H

#include "iremote_broker.h"
#include "nlohmann/json.hpp"

namespace OHOS {
namespace ResourceSchedule {
class IResSchedExeService : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.ResourceSchedule.ResSchedExeService");

    /**
     * @brief Send request sync to the ressched_executor through inter-process communication.
     *
     * @param resType Indicates the resource type, all of the type have listed in res_exe_type.h.
     * @param value Indicates the value of the resource type, defined by the developers.
     * @param payload Indicates the context info of the resource type event.
     * @param reply Indicates the context info of the ipc reply.
     * @return function result
     */
    virtual int32_t SendRequestSync(uint32_t resType, int64_t value,
        const nlohmann::json& payload, nlohmann::json& reply) = 0;

    /**
     * @brief Send request async to the ressched_executor through inter-process communication.
     *
     * @param resType Indicates the resource type, all of the type have listed in res_exe_type.h.
     * @param value Indicates the value of the resource type, defined by the developers.
     * @param payload Indicates the context info of the resource type event.
     */
    virtual void SendRequestAsync(uint32_t resType, int64_t value, const nlohmann::json& payload) = 0;

    /**
     * @brief Send kill process request async to the ressched_executor.
     *
     * @param pid the pid whiche will be killed.
     */
    virtual int32_t KillProcess(pid_t pid) = 0;
};
} // namespace ResourceSchedule
} // namespace OHOS

#endif // RESSCHED_EXECUTOR_INTERFACES_INNERKITS_RESSCHED_EXECUTOR_CLIENT_INCLUDE_IRES_SCHED_EXE_SERVICE_H
