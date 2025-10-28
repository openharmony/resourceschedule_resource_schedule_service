/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef RESSCHED_COMMON_INCLUDE_RES_SCHED_SIGNATURE_VALIDATOR_H
#define RESSCHED_COMMON_INCLUDE_RES_SCHED_SIGNATURE_VALIDATOR_H

#include <unordered_map>
#include <ctime>
#include <utility>
#include "ffrt.h"

namespace OHOS {
namespace ResourceSchedule {

enum SignatureCheckResult : int8_t {
    CHECK_OK = 0,
    ERR_SIGNATURE_NO_MATCH = 1,
    ERR_NO_SIGNATURE_CONFIG = 2,
    ERR_PARAM_INVALID = 3,
    ERR_INTERNAL_ERROR = 4,
    ERR_NOT_IN_LIST = 5,
};

class ResSchedSignatureValidator {
public:
    static ResSchedSignatureValidator &GetInstance();

    SignatureCheckResult CheckSignatureByBundleName(const std::string &bundleName);

    SignatureCheckResult CheckSignatureByUid(const int32_t uid);

    SignatureCheckResult CheckBundleInList(const std::vector<std::string> &whiteList, const std::string &bundleName);

    void SetSignatureConfig(std::unordered_map<std::string, std::string> &config);

    void AddSignatureConfig(std::unordered_map<std::string, std::string> &config);
private:
    ResSchedSignatureValidator() = default;

    ~ResSchedSignatureValidator() = default;

    SignatureCheckResult CheckSignature(const int32_t uid, const std::string &bundleName);

    int32_t GetCurrentUserId();

    ffrt::mutex mutex_;

    std::unordered_map<std::string, std::pair<int32_t, bool>> validCache_;

    std::unordered_map<std::string, std::string> signatureConfig_;
};
}  // namespace ResourceSchedule
}  // namespace OHOS

#endif  // RESSCHED_COMMON_INCLUDE_RES_SCHED_SIGNATURE_VALIDATOR_H