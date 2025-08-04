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
#include "res_sched_service.h"
#include "res_sched_errors.h"
#include "res_type.h"

#include "accesstoken_kit.h"
#include "ipc_skeleton.h"
#include "suspend_manager_base_observer.h"
#include "suspend_manager_base_log.h"

namespace OHOS {
namespace ResourceSchedule {
namespace {
    const std::string GET_SUSPEND_STATE_PERMISSION = "ohos.permission.GET_SUSPEND_STATE";
}

ErrCode ResSchedService::GetSuspendStateByUid(const int32_t uid, bool &isFrozen, int32_t &funcResult)
{
    funcResult = ERR_OK;
    nlohmann::json payload;
    payload["uid"] = uid;
    std::string payloadValue = payload.dump(-1, ' ', false, nlohmann::detail::error_handler_t::replace);

    std::string replyValue = "";
    int32_t ret = ERR_OK;
    ReportSyncEvent(
        ResType::SYNC_RES_TYPE_GET_SUSPEND_STATE_BY_UID, 0, payloadValue, replyValue, ret);

    nlohmann::json reply {};
    reply = StringToJsonObj(replyValue);
    if (ret == ERR_OK && !reply.is_null() && reply.contains("isFrozen") && reply.at("isFrozen").is_boolean()) {
        isFrozen = reply["isFrozen"].get<bool>();
        return ERR_OK;
    }
    funcResult = RES_SCHED_ACCESS_TOKEN_FAIL;
    return ERR_OK;
}

ErrCode ResSchedService::GetSuspendStateByPid(const int32_t pid, bool &isFrozen, int32_t &funcResult)
{
    funcResult = ERR_OK;
    nlohmann::json payload;
    payload["pid"] = pid;
    std::string payloadValue = payload.dump(-1, ' ', false, nlohmann::detail::error_handler_t::replace);

    std::string replyValue = "";
    int32_t ret = ERR_OK;
    ReportSyncEvent(
        ResType::SYNC_RES_TYPE_GET_SUSPEND_STATE_BY_PID, 0, payloadValue, replyValue, ret);

    nlohmann::json reply {};
    reply = StringToJsonObj(replyValue);
    if (ret == ERR_OK && !reply.is_null() && reply.contains("isFrozen") && reply.at("isFrozen").is_boolean()) {
        isFrozen = reply["isFrozen"].get<bool>();
        return ERR_OK;
    }
    funcResult = RES_SCHED_ACCESS_TOKEN_FAIL;
    return ERR_OK;
}

ErrCode ResSchedService::RegisterSuspendObserver(const sptr<ISuspendStateObserverBase> &observer, int32_t &funcResult)
{
    SUSPEND_MSG_LOGI("called");
    Security::AccessToken::AccessTokenID tokenId = OHOS::IPCSkeleton::GetCallingTokenID();
    auto tokenFlag = Security::AccessToken::AccessTokenKit::GetTokenTypeFlag(tokenId);
    if (tokenFlag != Security::AccessToken::TypeATokenTypeEnum::TOKEN_NATIVE) {
        SUSPEND_MSG_LOGE("Invalid calling token!");
        funcResult = ERR_INVALID_OPERATION;
        return ERR_OK;
    }

    int32_t status = Security::AccessToken::AccessTokenKit::VerifyAccessToken(tokenId, GET_SUSPEND_STATE_PERMISSION);
    if (status != Security::AccessToken::PermissionState::PERMISSION_GRANTED) {
        SUSPEND_MSG_LOGE("Verify permission denied!");
        funcResult = ERR_INVALID_OPERATION;
        return ERR_OK;
    }

    funcResult = SuspendManagerBaseObserver::GetInstance().RegisterSuspendObserver(observer);
    return ERR_OK;
}

ErrCode ResSchedService::UnregisterSuspendObserver(const sptr<ISuspendStateObserverBase> &observer, int32_t &funcResult)
{
    SUSPEND_MSG_LOGI("called");
    Security::AccessToken::AccessTokenID tokenId = OHOS::IPCSkeleton::GetCallingTokenID();
    auto tokenFlag = Security::AccessToken::AccessTokenKit::GetTokenTypeFlag(tokenId);
    if (tokenFlag != Security::AccessToken::TypeATokenTypeEnum::TOKEN_NATIVE) {
        SUSPEND_MSG_LOGE("Invalid calling token!");
        funcResult = ERR_INVALID_OPERATION;
        return ERR_OK;
    }

    int32_t status = Security::AccessToken::AccessTokenKit::VerifyAccessToken(tokenId, GET_SUSPEND_STATE_PERMISSION);
    if (status != Security::AccessToken::PermissionState::PERMISSION_GRANTED) {
        SUSPEND_MSG_LOGE("Verify permission denied!");
        funcResult = ERR_INVALID_OPERATION;
        return ERR_OK;
    }

    funcResult = SuspendManagerBaseObserver::GetInstance().UnregisterSuspendObserver(observer);
    return ERR_OK;
}
} // namespace ResourceSchedule
} // namespace OHOS