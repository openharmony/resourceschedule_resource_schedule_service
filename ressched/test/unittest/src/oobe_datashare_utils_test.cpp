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

#include "oobe_datashare_utils_test.h"

#include "errors.h"
#include "datashare_predicates.h"
#include "datashare_result_set.h"
#include "oobe_datashare_utils.h"
#include "datashare_values_bucket.h"
#include "oobe_manager.h"
#include "uri.h"

using namespace std;
using namespace testing::ext;

namespace OHOS {
namespace ResourceSchedule {
namespace {
const std::string KEYWORD = "basic_statement_agreed";
} // namespace

void OOBEDataShareUtilsTest::SetUpTestCase() {}

void OOBEDataShareUtilsTest::TearDownTestCase() {}

void OOBEDataShareUtilsTest::SetUp() {}

void OOBEDataShareUtilsTest::TearDown() {}

void OOBEDataShareUtilsTest::QueryDatabaseBeforeAssertions(int32_t res)
{
    std::string stringValue;
    DataShareUtils::GetInstance().GetStringValue(KEYWORD, stringValue);
    if (stringValue.empty()) {
        EXPECT_NE(res, ERR_OK);
    } else {
        EXPECT_EQ(res, ERR_OK);
    }
}

/**
 * @tc.name: oobe dataShareUtils TestDataShareUtils_001
 * @tc.desc: test the interface GetValue of dataShareUtils
 * @tc.type: FUNC
 * @tc.require: issueI97493
 * @tc.author:zhumingjie
 */
HWTEST_F(OOBEDataShareUtilsTest, TestDataShareUtils_001, Function | MediumTest | Level0)
{
    int32_t result = 0;
    int32_t res = DataShareUtils::GetInstance().GetValue(KEYWORD, result);
    QueryDatabaseBeforeAssertions(res);
}

/**
 * @tc.name: oobe dataShareUtils TestDataShareUtils_002
 * @tc.desc: test the interface GetValue of dataShareUtils
 * @tc.type: FUNC
 * @tc.require: issueI97493
 * @tc.author:zhumingjie
 */
HWTEST_F(OOBEDataShareUtilsTest, TestDataShareUtils_002, Function | MediumTest | Level0)
{
    int64_t result = 0;
    int32_t res = DataShareUtils::GetInstance().GetValue(KEYWORD, result);
    QueryDatabaseBeforeAssertions(res);
}

/**
 * @tc.name: oobe dataShareUtils TestDataShareUtils_003
 * @tc.desc: test the interface GetValue of dataShareUtils
 * @tc.type: FUNC
 * @tc.require: issueI97493
 * @tc.author:zhumingjie
 */
HWTEST_F(OOBEDataShareUtilsTest, TestDataShareUtils_003, Function | MediumTest | Level0)
{
    std::string result;
    int32_t res = DataShareUtils::GetInstance().GetValue(KEYWORD, result);
    QueryDatabaseBeforeAssertions(res);
}

/**
 * @tc.name: oobe dataShareUtils TestDataShareUtils_004
 * @tc.desc: test the interface GetValue of dataShareUtils
 * @tc.type: FUNC
 * @tc.require: issueI97493
 * @tc.author:zhumingjie
 */
HWTEST_F(OOBEDataShareUtilsTest, TestDataShareUtils_004, Function | MediumTest | Level0)
{
    bool result;
    std::string stringValue;
    DataShareUtils& dataShareUtils = DataShareUtils::GetInstance();
    dataShareUtils.GetStringValue(KEYWORD, stringValue);
    if (stringValue.empty()) {
        int32_t res = dataShareUtils.GetValue(KEYWORD, result);
        EXPECT_NE(res, ERR_OK);
    } else {
        int32_t res1 = dataShareUtils.GetValue(KEYWORD, result);
        EXPECT_EQ(res1, ERR_INVALID_OPERATION);
    }
}

/**
 * @tc.name: oobe dataShareUtils TestDataShareUtils_005
 * @tc.desc: test the interface GetStringValue of dataShareUtils
 * @tc.type: FUNC
 * @tc.require: issueI97493
 * @tc.author:zhumingjie
 */
HWTEST_F(OOBEDataShareUtilsTest, TestDataShareUtils_005, Function | MediumTest | Level0)
{
    std::string result;
    int32_t ret1 = ResourceSchedule::DataShareUtils::GetInstance().GetStringValue(KEYWORD, result);
    if (result.empty()) {
        EXPECT_NE(ret1, ERR_OK);
    } else {
        EXPECT_EQ(ret1, ERR_OK);
    }

    int32_t ret2 = ResourceSchedule::DataShareUtils::GetInstance().GetStringValue("test", result);
    EXPECT_EQ(ret2, ERR_NAME_NOT_FOUND);
}

/**
 * @tc.name: oobe dataShareUtils TestDataShareUtils_006
 * @tc.desc: test the interface CreateDataShareHelper of dataShareUtils
 * @tc.type: FUNC
 * @tc.require: issueI97493
 * @tc.author:zhumingjie
 */
HWTEST_F(OOBEDataShareUtilsTest, TestDataShareUtils_006, Function | MediumTest | Level0)
{
    DataShareUtils& dataShareUtils = DataShareUtils::GetInstance();
    dataShareUtils.remoteObj_ = nullptr;
    std::shared_ptr<DataShare::DataShareHelper> helper = dataShareUtils.CreateDataShareHelper();
    EXPECT_EQ(helper, nullptr);
}

/**
 * @tc.name: oobe dataShareUtils TestDataShareUtils_007
 * @tc.desc: test the interface CreateDataShareHelper of dataShareUtils
 * @tc.type: FUNC
 * @tc.require: issueI97493
 * @tc.author:zhumingjie
 */
HWTEST_F(OOBEDataShareUtilsTest, TestDataShareUtils_007, Function | MediumTest | Level0)
{
    DataShareUtils& dataShareUtils = DataShareUtils::GetInstance();
    dataShareUtils.InitSystemAbilityManager();
    std::shared_ptr<DataShare::DataShareHelper> helper = dataShareUtils.CreateDataShareHelper();
    EXPECT_NE(helper, nullptr);
}

/**
 * @tc.name: oobe dataShareUtils TestDataShareUtils_008
 * @tc.desc: test the interface ReleaseDataShareHelper of dataShareUtils
 * @tc.type: FUNC
 * @tc.require: issueI97493
 * @tc.author:zhumingjie
 */
HWTEST_F(OOBEDataShareUtilsTest, TestDataShareUtils_008, Function | MediumTest | Level0)
{
    DataShareUtils& dataShareUtils = DataShareUtils::GetInstance();
    dataShareUtils.InitSystemAbilityManager();
    std::shared_ptr<DataShare::DataShareHelper> helper = dataShareUtils.CreateDataShareHelper();
    bool flag = dataShareUtils.ReleaseDataShareHelper(helper);
    EXPECT_TRUE(flag);
}

/**
 * @tc.name: oobe dataShareUtils TestDataShareUtils_009
 * @tc.desc: test the interface ReleaseDataShareHelper of dataShareUtils
 * @tc.type: FUNC
 * @tc.require: issueI97493
 * @tc.author:zhumingjie
 */
HWTEST_F(OOBEDataShareUtilsTest, TestDataShareUtils_009, Function | MediumTest | Level0)
{
    DataShareUtils& dataShareUtils = DataShareUtils::GetInstance();
    dataShareUtils.InitSystemAbilityManager();
    std::shared_ptr<DataShare::DataShareHelper> helper = dataShareUtils.CreateDataShareHelper();
    helper = nullptr;
    bool flag = dataShareUtils.ReleaseDataShareHelper(helper);
    EXPECT_EQ(flag, false);
}

/**
 * @tc.name: oobe dataShareUtils TestDataShareUtils_010
 * @tc.desc: test the interface AssembleUri of dataShareUtils
 * @tc.type: FUNC
 * @tc.require: issueI97493
 * @tc.author:zhumingjie
 */
HWTEST_F(OOBEDataShareUtilsTest, TestDataShareUtils_010, Function | MediumTest | Level0)
{
    DataShareUtils& dataShareUtils = DataShareUtils::GetInstance();
    dataShareUtils.AssembleUri(KEYWORD);
    SUCCEED();
}
#undef private
#undef protected
} // namespace ResourceSchedule
} // namespace OHOS
