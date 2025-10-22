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

#include "bundle_mgr_helper.h"
#include "res_sched_signature_validator_test.h"
#include "res_sched_signature_validator.h"

using namespace std;
using namespace testing::ext;
using namespace testing;

namespace OHOS {
namespace ResourceSchedule {

namespace {
const std::string BUNDLE_VALID = "com.example.test";
const std::string BUNDLE_NOT_EXIST = "com.example.not_exist";
const std::string BUNDLE_INVALID = "";
const std::string SIGNATURE_VALID = "123456789abcdefg";
const std::string SIGNATURE_NO_MATCH = "abcdefg";
const std::string SIGNATURE_INVALID = "";
constexpr int32_t UID_VALID = 111;
constexpr int32_t UID_NOT_EXIST = 222;
constexpr int32_t UID_NO_MATCH = 333;
constexpr int32_t UID_INVALID = -1;
}  // namespace

void ResSchedSignatureValidatorTest::SetUp()
{
    ResSchedSignatureValidator::GetInstance().validCache_.clear();
    ResSchedSignatureValidator::GetInstance().signatureConfig_.clear();
}

void ResSchedSignatureValidatorTest::TearDown()
{}

void ResSchedSignatureValidatorTest::InitDefaultConfig()
{
    std::unordered_map<std::string, std::string> config = {{BUNDLE_VALID, SIGNATURE_VALID}};
    ResSchedSignatureValidator::GetInstance().SetSignatureConfig(config);
}

// OVERRIDE BundleMgrHelper METHOD

std::string BundleMgrHelper::GetBundleNameByUid(int32_t uid)
{
    if (uid == UID_VALID) {
        return BUNDLE_VALID;
    } else {
        return BUNDLE_INVALID;
    }
}

int32_t BundleMgrHelper::GetUidByBundleName(const std::string &bundleName, const int32_t userId)
{
    if (bundleName == BUNDLE_VALID) {
        return UID_VALID;
    } else {
        return UID_INVALID;
    }
}

ErrCode BundleMgrHelper::GetSignatureInfoByUid(const int32_t uid, AppExecFwk::SignatureInfo &signatureInfo)
{
    if (uid == UID_VALID) {
        signatureInfo.appIdentifier = SIGNATURE_VALID;
        return 0;
    } else if (uid == UID_NO_MATCH) {
        signatureInfo.appIdentifier = SIGNATURE_NO_MATCH;
        return 0;
    } else {
        signatureInfo.appIdentifier = SIGNATURE_INVALID;
        return 1;
    }
}

// TESTCASE

HWTEST_F(ResSchedSignatureValidatorTest, CheckSignatureByUid_001, Function | MediumTest | Level0)
{
    auto &instance = ResSchedSignatureValidator::GetInstance();
    InitDefaultConfig();
    EXPECT_EQ(instance.CheckSignatureByUid(UID_VALID), SignatureCheckResult::CHECK_OK);
    EXPECT_EQ(instance.CheckSignatureByUid(UID_INVALID), SignatureCheckResult::ERR_INTERNAL_ERROR);
}

HWTEST_F(ResSchedSignatureValidatorTest, CheckSignatureByBundleName_001, Function | MediumTest | Level0)
{
    auto &instance = ResSchedSignatureValidator::GetInstance();
    InitDefaultConfig();
    EXPECT_EQ(instance.CheckSignatureByBundleName(BUNDLE_VALID), SignatureCheckResult::CHECK_OK);
    EXPECT_EQ(instance.CheckSignatureByBundleName(BUNDLE_NOT_EXIST), SignatureCheckResult::ERR_INTERNAL_ERROR);
    EXPECT_EQ(instance.CheckSignatureByBundleName(BUNDLE_INVALID), SignatureCheckResult::ERR_PARAM_INVALID);
}

HWTEST_F(ResSchedSignatureValidatorTest, CheckBundleInList_001, Function | MediumTest | Level0)
{
    auto &instance = ResSchedSignatureValidator::GetInstance();
    InitDefaultConfig();
    std::vector<std::string> list{BUNDLE_VALID};
    EXPECT_EQ(instance.CheckBundleInList(list, BUNDLE_VALID), SignatureCheckResult::CHECK_OK);
    EXPECT_EQ(instance.CheckBundleInList(list, BUNDLE_NOT_EXIST), SignatureCheckResult::ERR_NOT_IN_LIST);
}

HWTEST_F(ResSchedSignatureValidatorTest, CheckSignature_001, Function | MediumTest | Level0)
{
    auto &instance = ResSchedSignatureValidator::GetInstance();
    EXPECT_EQ(instance.CheckSignature(222, "test"), SignatureCheckResult::ERR_NO_SIGNATURE_CONFIG);
    InitDefaultConfig();
    EXPECT_EQ(instance.CheckSignature(UID_NO_MATCH, BUNDLE_VALID), SignatureCheckResult::ERR_SIGNATURE_NO_MATCH);
    // Check With Cache
    EXPECT_EQ(instance.CheckSignature(UID_NO_MATCH, BUNDLE_VALID), SignatureCheckResult::ERR_SIGNATURE_NO_MATCH);
    EXPECT_EQ(instance.CheckSignature(UID_NOT_EXIST, BUNDLE_VALID), SignatureCheckResult::ERR_INTERNAL_ERROR);
}

}  // namespace ResourceSchedule
}  // namespace OHOS
