/*
 * Copyright (c) 2025-2026 Huawei Device Co., Ltd.
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
constexpr int32_t DEFAULT_USER_ID = 100;
}  // namespace

void ResSchedSignatureValidatorTest::SetUp()
{
    ResSchedSignatureValidator testSignatureValidator;
    std::shared_ptr<ResourceSchedule::CommBundleMgrHelperTest> commBundleMgrHelperTest =
        std::make_shared<ResourceSchedule::CommBundleMgrHelperTest>();
    testSignatureValidator.InitSignatureDependencyInterface(commBundleMgrHelperTest);
    testSignatureValidator.validCache_.clear();
    testSignatureValidator.signatureConfig_.clear();
}

void ResSchedSignatureValidatorTest::TearDown()
{}

/*
void ResSchedSignatureValidatorTest::InitDefaultConfig()
{
    ResSchedSignatureValidator testSignatureValidator;
    std::shared_ptr<ResourceSchedule::CommBundleMgrHelperTest> commBundleMgrHelperTest =
        std::make_shared<ResourceSchedule::CommBundleMgrHelperTest>();
    testSignatureValidator.InitSignatureDependencyInterface(commBundleMgrHelperTest);
    std::unordered_map<std::string, std::string> config = {{BUNDLE_VALID, SIGNATURE_VALID}};
    testSignatureValidator.AddSignatureConfig(config);
}*/

// OVERRIDE CommBundleMgrHelperTest METHOD

std::string CommBundleMgrHelperTest::GetBundleNameByUid(int32_t uid)
{
    if (uid == UID_VALID) {
        return BUNDLE_VALID;
    } else {
        return BUNDLE_INVALID;
    }
}

int32_t CommBundleMgrHelperTest::GetUidByBundleName(const std::string &bundleName, const int32_t userId)
{
    if (bundleName == BUNDLE_VALID) {
        return UID_VALID;
    } else {
        return UID_INVALID;
    }
}

ErrCode CommBundleMgrHelperTest::GetSignatureInfoByUid(const int32_t uid, std::string &signatureInfo)
{
    if (uid == UID_VALID) {
        signatureInfo = SIGNATURE_VALID;
        return 0;
    } else if (uid == UID_NO_MATCH) {
        signatureInfo = SIGNATURE_NO_MATCH;
        return 0;
    } else {
        signatureInfo = SIGNATURE_INVALID;
        return 1;
    }
}

void CommBundleMgrHelperTest::GetCurrentUserId(std::vector<int> &activatedOsAccountIds)
{
    activatedOsAccountIds.push_back(DEFAULT_USER_ID);
}

// TESTCASE

/**
 * @tc.name: ResSchedSignatureValidatorTest CheckSignatureByUid_001
 * @tc.desc: test for CheckSignatureByUid
 * @tc.type: FUNC
 * @tc.require: issues/1527
 */
HWTEST_F(ResSchedSignatureValidatorTest, CheckSignatureByUid_001, Function | MediumTest | Level0)
{
    ResSchedSignatureValidator testSignatureValidator;
    std::shared_ptr<ResourceSchedule::CommBundleMgrHelperTest> commBundleMgrHelperTest =
        std::make_shared<ResourceSchedule::CommBundleMgrHelperTest>();
    testSignatureValidator.InitSignatureDependencyInterface(commBundleMgrHelperTest);
    std::unordered_map<std::string, std::string> config = {{BUNDLE_VALID, SIGNATURE_VALID}};
    testSignatureValidator.AddSignatureConfig(config);
    EXPECT_EQ(testSignatureValidator.CheckSignatureByUid(UID_VALID),
        SignatureCheckResult::CHECK_OK);
    EXPECT_EQ(testSignatureValidator.CheckSignatureByUid(UID_INVALID),
        SignatureCheckResult::ERR_INTERNAL_ERROR);
}

/**
 * @tc.name: ResSchedSignatureValidatorTest CheckSignatureByBundleName_001
 * @tc.desc: test for CheckSignatureByBundleName normal
 * @tc.type: FUNC
 * @tc.require: issues/1527
 */
HWTEST_F(ResSchedSignatureValidatorTest, CheckSignatureByBundleName_001, Function | MediumTest | Level0)
{
    ResSchedSignatureValidator testSignatureValidator;
    std::shared_ptr<ResourceSchedule::CommBundleMgrHelperTest> commBundleMgrHelperTest =
        std::make_shared<ResourceSchedule::CommBundleMgrHelperTest>();
    testSignatureValidator.InitSignatureDependencyInterface(commBundleMgrHelperTest);
    std::unordered_map<std::string, std::string> config = {{BUNDLE_VALID, SIGNATURE_VALID}};
    testSignatureValidator.AddSignatureConfig(config);
    EXPECT_EQ(testSignatureValidator.CheckSignatureByBundleName(BUNDLE_VALID),
        SignatureCheckResult::CHECK_OK);
    EXPECT_EQ(testSignatureValidator.CheckSignatureByBundleName(BUNDLE_NOT_EXIST),
        SignatureCheckResult::ERR_INTERNAL_ERROR);
    EXPECT_EQ(testSignatureValidator.CheckSignatureByBundleName(BUNDLE_INVALID),
        SignatureCheckResult::ERR_PARAM_INVALID);
}

/**
 * @tc.name: ResSchedSignatureValidatorTest CheckSignatureByBundleName_002
 * @tc.desc: test for CheckSignatureByBundleName install changed
 * @tc.type: FUNC
 * @tc.require: issues/1527
 */
HWTEST_F(ResSchedSignatureValidatorTest, CheckSignatureByBundleName_002, Function | MediumTest | Level0)
{
    ResSchedSignatureValidator testSignatureValidator;
    std::shared_ptr<ResourceSchedule::CommBundleMgrHelperTest> commBundleMgrHelperTest =
        std::make_shared<ResourceSchedule::CommBundleMgrHelperTest>();
    testSignatureValidator.InitSignatureDependencyInterface(commBundleMgrHelperTest);
    std::unordered_map<std::string, std::string> config = {{BUNDLE_VALID, SIGNATURE_VALID}};
    testSignatureValidator.AddSignatureConfig(config);
    EXPECT_EQ(testSignatureValidator.CheckSignatureByBundleName(BUNDLE_VALID),
        SignatureCheckResult::CHECK_OK);
    EXPECT_EQ(testSignatureValidator.CheckSignatureByBundleName(BUNDLE_VALID),
        SignatureCheckResult::CHECK_OK);
    testSignatureValidator.OnAppInstallChanged(BUNDLE_VALID);
    EXPECT_EQ(testSignatureValidator.CheckSignatureByBundleName(BUNDLE_VALID),
        SignatureCheckResult::CHECK_OK);
}

/**
 * @tc.name: ResSchedSignatureValidatorTest CheckBundleInList_001
 * @tc.desc: test for CheckBundleInList
 * @tc.type: FUNC
 * @tc.require: issues/1527
 */
HWTEST_F(ResSchedSignatureValidatorTest, CheckBundleInList_001, Function | MediumTest | Level0)
{
    ResSchedSignatureValidator testSignatureValidator;
    std::shared_ptr<ResourceSchedule::CommBundleMgrHelperTest> commBundleMgrHelperTest =
        std::make_shared<ResourceSchedule::CommBundleMgrHelperTest>();
    testSignatureValidator.InitSignatureDependencyInterface(commBundleMgrHelperTest);
    std::unordered_map<std::string, std::string> config = {{BUNDLE_VALID, SIGNATURE_VALID}};
    testSignatureValidator.AddSignatureConfig(config);
    std::vector<std::string> list{BUNDLE_VALID};
    EXPECT_EQ(testSignatureValidator.CheckBundleInList(list, BUNDLE_VALID),
        SignatureCheckResult::CHECK_OK);
    EXPECT_EQ(testSignatureValidator.CheckBundleInList(list, BUNDLE_NOT_EXIST),
        SignatureCheckResult::ERR_NOT_IN_LIST);
}

/**
 * @tc.name: ResSchedSignatureValidatorTest CheckSignature_001
 * @tc.desc: test for CheckSignature
 * @tc.type: FUNC
 * @tc.require: issues/1527
 */
HWTEST_F(ResSchedSignatureValidatorTest, CheckSignature_001, Function | MediumTest | Level0)
{
    ResSchedSignatureValidator testSignatureValidator;
    std::shared_ptr<ResourceSchedule::CommBundleMgrHelperTest> commBundleMgrHelperTest =
        std::make_shared<ResourceSchedule::CommBundleMgrHelperTest>();
    testSignatureValidator.InitSignatureDependencyInterface(commBundleMgrHelperTest);
    EXPECT_EQ(testSignatureValidator.CheckSignature(222, "test"),
        SignatureCheckResult::ERR_NO_SIGNATURE_CONFIG);
    std::unordered_map<std::string, std::string> config = {{BUNDLE_VALID, SIGNATURE_VALID}};
    testSignatureValidator.AddSignatureConfig(config);
    EXPECT_EQ(testSignatureValidator.CheckSignature(UID_NO_MATCH, BUNDLE_VALID),
        SignatureCheckResult::ERR_SIGNATURE_NO_MATCH);
    // Check With Cache
    EXPECT_EQ(testSignatureValidator.CheckSignature(UID_NO_MATCH, BUNDLE_VALID),
        SignatureCheckResult::ERR_SIGNATURE_NO_MATCH);
    EXPECT_EQ(testSignatureValidator.CheckSignature(UID_NOT_EXIST, BUNDLE_VALID),
        SignatureCheckResult::ERR_INTERNAL_ERROR);
}

/**
 * @tc.name: ResSchedSignatureValidatorTest AddSignatureConfig_001
 * @tc.desc: test for AddSignatureConfig
 * @tc.type: FUNC
 * @tc.require: issues/1527
 */
HWTEST_F(ResSchedSignatureValidatorTest, AddSignatureConfig_001, Function | MediumTest | Level0)
{
    ResSchedSignatureValidator testSignatureValidator;
    std::shared_ptr<ResourceSchedule::CommBundleMgrHelperTest> commBundleMgrHelperTest =
        std::make_shared<ResourceSchedule::CommBundleMgrHelperTest>();
    testSignatureValidator.InitSignatureDependencyInterface(commBundleMgrHelperTest);
    testSignatureValidator.signatureConfig_.clear();
    std::unordered_map<std::string, std::string> config1 = {{BUNDLE_VALID, SIGNATURE_VALID}};
    testSignatureValidator.AddSignatureConfig(config1);
    EXPECT_EQ(testSignatureValidator.signatureConfig_.size(), 1);
    std::unordered_map<std::string, std::string> config = {{"test_add_bundle", "test"}};
    testSignatureValidator.AddSignatureConfig(config);
    EXPECT_EQ(testSignatureValidator.signatureConfig_.size(), 2);
}
}  // namespace ResourceSchedule
}  // namespace OHOS
