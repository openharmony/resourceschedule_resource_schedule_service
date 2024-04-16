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

#include "res_sched_exe_common_utils.h"

#include <chrono>

#include <parameters.h>

#include "accesstoken_kit.h"
#include "ipc_skeleton.h"
#include "tokenid_kit.h"

#include "res_sched_exe_log.h"

namespace OHOS {
namespace ResourceSchedule {
uint64_t ResSchedExeCommonUtils::GetCurrentTimestampMs()
{
    std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()
    );
    return static_cast<uint64_t>(ms.count());
}

uint64_t ResSchedExeCommonUtils::GetCurrentTimestampUs()
{
    std::chrono::microseconds ms = std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::system_clock::now().time_since_epoch()
    );
    return static_cast<uint64_t>(ms.count());
}

bool ResSchedExeCommonUtils::IsDebugMode()
{
    return OHOS::system::GetIntParameter("const.debuggable", 0) != 0;
}

bool ResSchedExeCommonUtils::CheckPermission(const std::string& permission)
{
    Security::AccessToken::AccessTokenID tokenId = IPCSkeleton::GetCallingTokenID();
    auto tokenType = Security::AccessToken::AccessTokenKit::GetTokenTypeFlag(tokenId);
    if (tokenType != Security::AccessToken::ATokenTypeEnum::TOKEN_NATIVE) {
        RSSEXE_LOGE("not native sa");
        return false;
    }
    int32_t ret = Security::AccessToken::AccessTokenKit::VerifyAccessToken(tokenId, permission);
    return ret == Security::AccessToken::PermissionState::PERMISSION_GRANTED;
}

void ResSchedExeCommonUtils::StringToJson(const std::string& str, nlohmann::json& payload)
{
    if (str.empty()) {
        return;
    }

    auto jsonObj = nlohmann::json::parse(str, nullptr, false);
    if (jsonObj.is_discarded()) {
        RSSEXE_LOGE("parse string to json failed.");
        RSSEXE_LOGD("string: %{private}s.", str.c_str());
        return;
    }
    if (!jsonObj.is_object()) {
        RSSEXE_LOGE("convert string result is not a jsonobj.");
        RSSEXE_LOGD("string: %{private}s.", str.c_str());
        return;
    }

    for (auto& [key, value] : jsonObj.items()) {
        payload[key] = value;
    }
}
} // namespace ResourceSchedule
} // namespace OHOS
