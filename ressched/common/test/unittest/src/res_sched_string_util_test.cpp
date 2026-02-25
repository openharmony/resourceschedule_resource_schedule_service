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

#include "res_sched_string_util_test.h"

#include "res_sched_string_util.h"

using namespace std;
using namespace testing::ext;

namespace OHOS {
namespace ResourceSchedule {


void ResSchedStringUtilTest::SetUpTestCase() {}

void ResSchedStringUtilTest::TearDownTestCase() {}

void ResSchedStringUtilTest::SetUp() {}

void ResSchedStringUtilTest::TearDown() {}

/**
 * @tc.name: ResSchedStringUtilTest StrToFloat_001
 * @tc.desc: test StrToFloat
 * @tc.type: FUNC
 * @tc.require: issueIB8Y0E
 */
HWTEST_F(ResSchedStringUtilTest, StrToFloat_001, Function | MediumTest | Level0)
{
    float result = 0.0f;
    float fMax = std::numeric_limits<float>::max();
    EXPECT_TRUE(ResCommonUtil::StrToFloat(std::to_string(fMax), result));
    EXPECT_EQ(result, fMax);
    result = 0.0f;
    EXPECT_FALSE(ResCommonUtil::StrToFloat("1" + std::to_string(fMax), result));
    EXPECT_EQ(result, 0.0f);
    result = 0.0f;
    EXPECT_FALSE(ResCommonUtil::StrToFloat("abc", result));
    EXPECT_EQ(result, 0.0f);
    result = 0.0f;
    EXPECT_FALSE(ResCommonUtil::StrToFloat("1123abc", result));
    EXPECT_EQ(result, 0.0f);
}


/**
 * @tc.name: ResSchedStringUtilTest StrToInt32_001
 * @tc.desc: test StrToInt32
 * @tc.type: FUNC
 * @tc.require: issueIB8Y0E
 */
HWTEST_F(ResSchedStringUtilTest, StrToInt32_001, Function | MediumTest | Level0)
{
    int32_t result = -1;
    EXPECT_TRUE(ResCommonUtil::StrToInt32(std::to_string(INT32_MAX), result));
    EXPECT_EQ(result, INT32_MAX);
    result = -1;
    EXPECT_TRUE(ResCommonUtil::StrToInt32(std::to_string(INT32_MIN), result));
    EXPECT_EQ(result, INT32_MIN);
    result = -1;
    EXPECT_FALSE(ResCommonUtil::StrToInt32(std::to_string(LONG_MAX) + "1", result));
    EXPECT_EQ(result, -1);
    result = -1;
    EXPECT_FALSE(ResCommonUtil::StrToInt32("abc", result));
    EXPECT_EQ(result, -1);
    result = -1;
    EXPECT_FALSE(ResCommonUtil::StrToInt32("1123abc", result));
    EXPECT_EQ(result, -1);
}

/**
 * @tc.name: ResSchedStringUtilTest StrToInt64_001
 * @tc.desc: test StrToInt64
 * @tc.type: FUNC
 * @tc.require: issueIB8Y0E
 */
HWTEST_F(ResSchedStringUtilTest, StrToInt64_001, Function | MediumTest | Level0)
{
    int64_t result = -1;
    EXPECT_TRUE(ResCommonUtil::StrToInt64(std::to_string(INT64_MAX), result));
    EXPECT_EQ(result, INT64_MAX);
    result = -1;
    EXPECT_TRUE(ResCommonUtil::StrToInt64(std::to_string(INT64_MIN), result));
    EXPECT_EQ(result, INT64_MIN);
    result = -1;
    EXPECT_FALSE(ResCommonUtil::StrToInt64(std::to_string(INT64_MAX) + "1", result));
    EXPECT_EQ(result, -1);
    result = -1;
    EXPECT_FALSE(ResCommonUtil::StrToInt64("abc", result));
    EXPECT_EQ(result, -1);
    result = -1;
    EXPECT_FALSE(ResCommonUtil::StrToInt64("1123abc", result));
    EXPECT_EQ(result, -1);
}

/**
 * @tc.name: ResSchedStringUtilTest CheckBundleName_001
 * @tc.desc: test CheckBundleName
 * @tc.type: FUNC
 * @tc.require: issueIB8Y0E
 */
HWTEST_F(ResSchedStringUtilTest, CheckBundleName_001, Function | MediumTest | Level0)
{
    EXPECT_FALSE(ResCommonUtil::CheckBundleName(""));
    EXPECT_FALSE(ResCommonUtil::CheckBundleName("a"));
    EXPECT_FALSE(ResCommonUtil::CheckBundleName("aaaaaa"));
    std::string maxBundleName = "";
    for (int i = 0;i <= 127; ++i) {
        maxBundleName.append("a");
    }
    EXPECT_FALSE(ResCommonUtil::CheckBundleName(maxBundleName));
    EXPECT_FALSE(ResCommonUtil::CheckBundleName("11111111.1111"));
    EXPECT_FALSE(ResCommonUtil::CheckBundleName("com.test.**.**"));
    EXPECT_FALSE(ResCommonUtil::CheckBundleName("com.test.123_!@#"));
    EXPECT_TRUE(ResCommonUtil::CheckBundleName("com.test"));
    EXPECT_TRUE(ResCommonUtil::CheckBundleName("com.test_"));
}

/**
 * @tc.name: ResSchedStringUtilTest StrToUInt32_001
 * @tc.desc: test StrToUInt32
 * @tc.type: FUNC
 * @tc.require: issueIB8Y0E
 */
HWTEST_F(ResSchedStringUtilTest, StrToUInt32_001, Function | MediumTest | Level0)
{
    uint32_t result = 0;
    EXPECT_TRUE(ResCommonUtil::StrToUInt32(std::to_string(UINT_MAX), result));
    EXPECT_EQ(result, UINT_MAX);
    result = 0;
    EXPECT_FALSE(ResCommonUtil::StrToUInt32("abc", result));
    EXPECT_EQ(result, 0);
    result = 0;
    EXPECT_FALSE(ResCommonUtil::StrToUInt32("1123abc", result));
    EXPECT_EQ(result, 0);
}

/**
 * @tc.name: ResSchedStringUtilTest IsNumericString_001
 * @tc.desc: test IsNumericString
 * @tc.type: FUNC
 * @tc.require: issueIB8Y0E
 */
HWTEST_F(ResSchedStringUtilTest, IsNumericString_001, Function | MediumTest | Level0)
{
    EXPECT_FALSE(ResCommonUtil::IsNumericString("11111111111"));
    EXPECT_FALSE(ResCommonUtil::IsNumericString(""));
    EXPECT_FALSE(ResCommonUtil::IsNumericString("-"));
    EXPECT_FALSE(ResCommonUtil::IsNumericString("$"));
    EXPECT_FALSE(ResCommonUtil::IsNumericString("-111-1"));
    EXPECT_FALSE(ResCommonUtil::IsNumericString("-**123"));
    EXPECT_TRUE(ResCommonUtil::IsNumericString("-1"));
    EXPECT_TRUE(ResCommonUtil::IsNumericString("1"));
    EXPECT_TRUE(ResCommonUtil::IsNumericString("-123"));
    EXPECT_TRUE(ResCommonUtil::IsNumericString("0000012500"));
}

/**
 * @tc.name: ResSchedStringUtilTest StringTrim_001
 * @tc.desc: test StringTrim
 * @tc.type: FUNC
 * @tc.require: issueIB8Y0E
 */
HWTEST_F(ResSchedStringUtilTest, StringTrim_001, Function | MediumTest | Level0)
{
    EXPECT_EQ(ResCommonUtil::StringTrim("     test    "), "test");
    EXPECT_EQ(ResCommonUtil::StringTrim("test    "), "test");
    EXPECT_EQ(ResCommonUtil::StringTrim("     test"), "test");
    EXPECT_EQ(ResCommonUtil::StringTrim("\f\vtest    "), "test");
    EXPECT_EQ(ResCommonUtil::StringTrim("test\f\v"), "test");
    EXPECT_EQ(ResCommonUtil::StringTrim("\f\v\r\t\n     test"), "test");
    EXPECT_EQ(ResCommonUtil::StringTrim("t est    "), "t est");
    EXPECT_EQ(ResCommonUtil::StringTrim("   t\fest    "), "t\fest");
    EXPECT_EQ(ResCommonUtil::StringTrim("\f\v\r\t\n test \n\t\r\v\f"), "test");
}

/**
 * @tc.name: ResSchedStringUtilTest StringTrim_002
 * @tc.desc: test StringTrim with empty and all-blank inputs
 * @tc.type: FUNC
 * @tc.require: issueIB8Y0E
 */
HWTEST_F(ResSchedStringUtilTest, StringTrim_002, Function | MediumTest | Level0)
{
    EXPECT_EQ(ResCommonUtil::StringTrim(""), "");
    EXPECT_EQ(ResCommonUtil::StringTrim("     "), "");
    EXPECT_EQ(ResCommonUtil::StringTrim("\f\v\r\t\n"), "");
}

/**
 * @tc.name: ResSchedStringUtilTest StringTrimSpace_001
 * @tc.desc: test StringTrimSpace
 * @tc.type: FUNC
 * @tc.require: issueIB8Y0E
 */
HWTEST_F(ResSchedStringUtilTest, StringTrimSpace_001, Function | MediumTest | Level0)
{
    EXPECT_EQ(ResCommonUtil::StringTrimSpace("     test    "), "test");
    EXPECT_EQ(ResCommonUtil::StringTrimSpace("     test"), "test");
    EXPECT_EQ(ResCommonUtil::StringTrimSpace("test    "), "test");
    EXPECT_EQ(ResCommonUtil::StringTrimSpace("     t   est    "), "t   est");
}

/**
 * @tc.name: ResSchedStringUtilTest StringTrimSpace_002
 * @tc.desc: test StringTrimSpace with empty and all-space inputs
 * @tc.type: FUNC
 * @tc.require: issueIB8Y0E
 */
HWTEST_F(ResSchedStringUtilTest, StringTrimSpace_002, Function | MediumTest | Level0)
{
    EXPECT_EQ(ResCommonUtil::StringTrimSpace(""), "");
    EXPECT_EQ(ResCommonUtil::StringTrimSpace("     "), "");
}

/**
 * @tc.name: ResSchedStringUtilTest StringToJson_001
 * @tc.desc: test StringToJson
 * @tc.type: FUNC
 * @tc.require: issueIB8Y0E
 */
HWTEST_F(ResSchedStringUtilTest, StringToJson_001, Function | MediumTest | Level0)
{
    nlohmann::json testJson;
    EXPECT_FALSE(ResCommonUtil::StringToJson("", testJson));
    EXPECT_FALSE(ResCommonUtil::StringToJson("test{test}test", testJson));
    EXPECT_TRUE(ResCommonUtil::StringToJson("{ \"testKey1\": \"test1\", \"testKey2\": \"test2\" }",
        testJson));
}
} // namespace ResourceSchedule
} // namespace OHOS
