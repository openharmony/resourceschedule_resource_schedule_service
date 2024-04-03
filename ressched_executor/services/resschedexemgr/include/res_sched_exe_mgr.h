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
     * Report data inner, will report resource data.
     *
     * @param resType Resource type.
     * @param value bit64 content.
     * @param reply Reply content.
     * @param payload Extra content.
     */
    int32_t SendResRequest(uint32_t resType, int64_t value = 0,
        nlohmann::json& reply, const nlohmann::json& payload = nullptr);
};
} // namespace ResourceSchedule
} // namespace OHOS

#endif // RESSCHED_EXECUTOR_SERVICES_RESSCHEDEXEMGR_INCLUDE_RES_SCHED_EXE_MGR_H
