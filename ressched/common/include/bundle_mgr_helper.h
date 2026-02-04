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

#ifndef RESSCHED_COMMON_INCLUDE_BUNDLE_MGR_HELPER_H
#define RESSCHED_COMMON_INCLUDE_BUNDLE_MGR_HELPER_H

#include <string>
#include "ipc_skeleton.h"

namespace OHOS {
namespace ResourceSchedule {
class BundleMgrHelper {
public:
    virtual std::string GetBundleNameByUid(const int32_t uid) = 0;
    virtual int32_t GetUidByBundleName(const std::string &bundleName, const int32_t userId) = 0;
    virtual ErrCode GetSignatureInfoByUid(const int32_t uid, std::string &signatureInfo) = 0;
    virtual void GetCurrentUserId(std::vector<int> &activatedOsAccountIds) = 0;
    virtual ~BundleMgrHelper() = default;
};
}  // namespace ResourceSchedule
}  // namespace OHOS
#endif  // RESSCHED_COMMON_INCLUDE_BUNDLE_MGR_HELPER_H