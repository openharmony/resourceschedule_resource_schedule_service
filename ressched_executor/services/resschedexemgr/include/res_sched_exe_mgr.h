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

#ifndef RESSCHED_EXECUTOR_SERVICES_RESSCHEDEXEMGR_INCLUDE_RES_SCHED_EXE_MGR_H
#define RESSCHED_EXECUTOR_SERVICES_RESSCHEDEXEMGR_INCLUDE_RES_SCHED_EXE_MGR_H

#include <cstdint>
#include <string>
#include <sys/types.h>

#include "event_handler.h"
#include "nlohmann/json.hpp"
#include "single_instance.h"

namespace OHOS {
namespace ResourceSchedule {
class ResSchedExeMgr {
    DECLARE_SINGLE_INSTANCE(ResSchedExeMgr);

public:
    /**
     * Init resource schedule manager.
     */
    void Init();

    /**
     * Stop resource schedule manager.
     */
    void Stop();

    /**
     * Send request sync inner, will report resource schedule executor data.
     *
     * @param resType Resource type.
     * @param value bit64 content.
     * @param payload Extra content.
     * @param reply Reply content.
     */
    int32_t SendRequestSync(uint32_t resType, int64_t value,
        const nlohmann::json& payload, nlohmann::json& reply);

    /**
     * Send request async inner, will report resource schedule executor data.
     *
     * @param resType Resource type.
     * @param value bit64 content.
     * @param payload Extra content.
     */
    void SendRequestAsync(uint32_t resType, int64_t value, const nlohmann::json& payload = nullptr);

    /**
     * @brief Send kill process request async to the ressched_executor.
     *
     * @param pid the pid whiche will be killed.
     */
    int32_t KillProcess(pid_t pid);
private:
    std::string BuildTraceStr(const std::string& func, uint32_t resType, int64_t value);
};
} // namespace ResourceSchedule
} // namespace OHOS

#endif // RESSCHED_EXECUTOR_SERVICES_RESSCHEDEXEMGR_INCLUDE_RES_SCHED_EXE_MGR_H
