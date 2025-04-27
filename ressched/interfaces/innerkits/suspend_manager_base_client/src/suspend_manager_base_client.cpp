/*
 * Copyright (c) 2025-2025 Huawei Device Co., Ltd.
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
#include "suspend_manager_base_client.h"
#include "res_sched_client.h"
#include "res_sched_errors.h"           // for GET_RES_SCHED_SERVICE_FAILED
#include "res_type.h"                   // for ResType
#include "nlohmann/json.hpp"

namespace OHOS {
namespace ResourceSchedule {
ErrCode SuspendManagerBaseClient::GetSuspendStateByUid(const int32_t uid, bool &isFrozen)
{
    nlohmann::json payload;
    payload["uid"] = uid;
    nlohmann::json reply;
    ResSchedClient::GetInstance().ReportSyncEvent(
        ResType::SYNC_RES_TYPE_GET_SUSPEND_STATE_BY_UID, 0, payload, reply);
    if (!reply.is_null() && reply.contains("isFrozen") && reply.at("isFrozen").is_boolean()) {
        isFrozen = reply["isFrozen"].get<bool>();
        return ERR_OK;
    }
    return RES_SCHED_ACCESS_TOKEN_FAIL;
}

ErrCode SuspendManagerBaseClient::GetSuspendStateByPid(const int32_t pid, bool &isFrozen)
{
    nlohmann::json payload;
    payload["pid"] = pid;
    nlohmann::json reply;
    ResSchedClient::GetInstance().ReportSyncEvent(
        ResType::SYNC_RES_TYPE_GET_SUSPEND_STATE_BY_PID, 0, payload, reply);
    if (!reply.is_null() && reply.contains("isFrozen") && reply.at("isFrozen").is_boolean()) {
        isFrozen = reply["isFrozen"].get<bool>();
        return ERR_OK;
    }
    return RES_SCHED_ACCESS_TOKEN_FAIL;
}
} // namespace ResourceSchedule
} // namespace OHOS