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

#include "res_bundle_mgr_helper.h"
#include "res_bundle_mgr_helper_test.h"

using namespace std;
using namespace testing::ext;
using namespace testing;

namespace OHOS {
namespace ResourceSchedule {

void ResBundleMgrHelperTest::SetUp()
{
    resBundleMgrHelper_ = make_shared<ResBundleMgrHelper>();
}

void ResBundleMgrHelperTest::TearDown()
{
    resBundleMgrHelper_ = nullptr;
}

HWTEST_F(ResBundleMgrHelperTest, GetBundleNameByUid_001, Function | MediumTest | Level0)
{
    EXPECT_TRUE(resBundleMgrHelper_->GetBundleNameByUid(111).empty());
}

HWTEST_F(ResBundleMgrHelperTest, GetUidByBundleName_001, Function | MediumTest | Level0)
{
    EXPECT_TRUE(resBundleMgrHelper_->GetUidByBundleName("test", 0) != 0);
}

HWTEST_F(ResBundleMgrHelperTest, GetSignatureInfoByUid_001, Function | MediumTest | Level0)
{
    std::string signatureInfo;
    EXPECT_TRUE(resBundleMgrHelper_->GetSignatureInfoByUid(111, signatureInfo) != 0);
}

HWTEST_F(ResBundleMgrHelperTest, GetCurrentUserId_001, Function | MediumTest | Level0)
{
    std::vector<int> activatedOsAccountIds;
    resBundleMgrHelper_->GetCurrentUserId(activatedOsAccountIds);
    EXPECT_TRUE(activatedOsAccountIds.size() != 0);
}
}  // namespace ResourceSchedule
}  // namespace OHOS
