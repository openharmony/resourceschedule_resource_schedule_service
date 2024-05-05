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

#ifndef RESSCHED_EXECUTOR_SERVICES_RESSCHEDEXESERVICE_INCLUDE_RES_SCHED_EXE_SERVICE_H
#define RESSCHED_EXECUTOR_SERVICES_RESSCHEDEXESERVICE_INCLUDE_RES_SCHED_EXE_SERVICE_H

#include "res_sched_exe_service_stub.h"

namespace OHOS {
namespace ResourceSchedule {
class ResSchedExeService : public ResSchedExeServiceStub {
public:
    /**
     * @brief Construct a new ResSchedExeService object.
     */
    ResSchedExeService() = default;

    /**
     * @brief Destroy the ResSchedExeService object.
     */
    ~ResSchedExeService() override = default;

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

    /**
     * @brief Support dump option.
     *
     * @param fd Save dump result to the file.
     * @param args Dump option arguments.
     */
    int32_t Dump(int32_t fd, const std::vector<std::u16string>& args) override;

private:
    bool AllowDump();
    void DumpAllInfo(std::string &result);
    void DumpUsage(std::string &result);
    DISALLOW_COPY_AND_MOVE(ResSchedExeService);
};
} // namespace ResourceSchedule
} // namespace OHOS

#endif // RESSCHED_EXECUTOR_SERVICES_RESSCHEDEXESERVICE_INCLUDE_RES_SCHED_EXE_SERVICE_H
