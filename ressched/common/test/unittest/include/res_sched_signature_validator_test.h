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

#ifndef RES_SCHED_SIGNATURE_VALIDATOR_TEST_H
#define RES_SCHED_SIGNATURE_VALIDATOR_TEST_H

#include "gtest/gtest.h"
#include "bundle_mgr_helper.h"

namespace OHOS {
namespace ResourceSchedule {

class ResSchedSignatureValidatorTest : public testing::Test {
protected:
    void SetUp();
    void TearDown();
    void InitDefaultConfig();
};

class CommBundleMgrHelperTest : public BundleMgrHelper {
public:
    std::string GetBundleNameByUid(const int32_t uid) override;
    int32_t GetUidByBundleName(const std::string &bundleName, const int32_t userId) override;
    ErrCode GetSignatureInfoByUid(const int32_t uid, std::string &signatureInfo) override;
    void GetCurrentUserId(std::vector<int> &activatedOsAccountIds) override;
};
}  // namespace ResourceSchedule
}  // namespace OHOS

#endif  // RES_SCHED_SIGNATURE_VALIDATOR_TEST_H
