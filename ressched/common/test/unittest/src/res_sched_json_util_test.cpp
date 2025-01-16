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

#include "res_sched_json_util_test.h"

#include "res_sched_json_util.h"
#include "res_sched_file_util.h"

using namespace std;
using namespace testing::ext;

namespace OHOS {
namespace ResourceSchedule {
namespace {
    const std::string TEST_INT_KEY = "intKey";
    const std::string TEST_STRING_KEY = "stringKey";
    const std::string TEST_BOOL_KEY = "boolKey";
    const std::string TEST_ARRAY_KEY = "arrayKey";
    const std::string TEST_OBJ_KEY = "objKey";
    const std::string TEST_KEY = "testKey";
    const std::string TEST_VALID_JSON = "etc/ressched/test_valid_json.json";
    const std::string TEST_INVALID_JSON = "etc/ressched/test_invalid_json.json";
    const std::string TEST_JSON_FILE_NAME = "test.json";

}

void ResSchedJsonUtilTest::SetUpTestCase() {}

void ResSchedJsonUtilTest::TearDownTestCase() {}

void ResSchedJsonUtilTest::SetUp() {}

void ResSchedJsonUtilTest::TearDown() {}

/**
 * @tc.name: ResSchedJsonUtilTest ParseIntParameterFromJson_001
 * @tc.desc: test ParseIntParameterFromJson
 * @tc.type: FUNC
 * @tc.require: issueIB8Y0E
 */
HWTEST_F(ResSchedJsonUtilTest, ParseIntParameterFromJson_001, Function | MediumTest | Level0)
{
    nlohmann::json testObj;
    int32_t value = -1;
    EXPECT_FALSE(ResCommonUtil::ParseIntParameterFromJson(TEST_INT_KEY, value, testObj));
    EXPECT_EQ(value, -1);
    testObj[TEST_KEY] = "test";
    EXPECT_FALSE(ResCommonUtil::ParseIntParameterFromJson(TEST_INT_KEY, value, testObj));
    EXPECT_EQ(value, -1);
    EXPECT_FALSE(ResCommonUtil::ParseIntParameterFromJson("", value, testObj));
    EXPECT_EQ(value, -1);
    testObj[TEST_INT_KEY] = "test";
    EXPECT_FALSE(ResCommonUtil::ParseIntParameterFromJson(TEST_INT_KEY, value, testObj));
    EXPECT_EQ(value, -1);
    testObj[TEST_INT_KEY] = "100";
    EXPECT_TRUE(ResCommonUtil::ParseIntParameterFromJson(TEST_INT_KEY, value, testObj));
    EXPECT_EQ(value, 100);
    testObj[TEST_INT_KEY] = 100;
    EXPECT_TRUE(ResCommonUtil::ParseIntParameterFromJson(TEST_INT_KEY, value, testObj));
    EXPECT_EQ(value, 100);
}

/**
 * @tc.name: ResSchedJsonUtilTest ParseStringParameterFromJson_001
 * @tc.desc: test ParseStringParameterFromJson
 * @tc.type: FUNC
 * @tc.require: issueIB8Y0E
 */
HWTEST_F(ResSchedJsonUtilTest, ParseStringParameterFromJson_001, Function | MediumTest | Level0)
{
    nlohmann::json testObj;
    std::string value = "";
    EXPECT_FALSE(ResCommonUtil::ParseStringParameterFromJson(TEST_STRING_KEY, value, testObj));
    EXPECT_EQ(value, "");
    testObj[TEST_KEY] = "test";
    EXPECT_FALSE(ResCommonUtil::ParseStringParameterFromJson("", value, testObj));
    EXPECT_EQ(value, "");
    EXPECT_FALSE(ResCommonUtil::ParseStringParameterFromJson(TEST_STRING_KEY, value, testObj));
    EXPECT_EQ(value, "");
    testObj[TEST_STRING_KEY] = 100;
    EXPECT_FALSE(ResCommonUtil::ParseStringParameterFromJson(TEST_STRING_KEY, value, testObj));
    EXPECT_EQ(value, "");
    testObj[TEST_STRING_KEY] = "test";
    EXPECT_TRUE(ResCommonUtil::ParseStringParameterFromJson(TEST_STRING_KEY, value, testObj));
    EXPECT_EQ(value, "test");
}

/**
 * @tc.name: ResSchedJsonUtilTest ParseBoolParameterFromJson_001
 * @tc.desc: test ParseBoolParameterFromJson
 * @tc.type: FUNC
 * @tc.require: issueIB8Y0E
 */
HWTEST_F(ResSchedJsonUtilTest, ParseBoolParameterFromJson_001, Function | MediumTest | Level0)
{
    nlohmann::json testObj;
    bool value = false;
    EXPECT_FALSE(ResCommonUtil::ParseBoolParameterFromJson(TEST_BOOL_KEY, value, testObj));
    EXPECT_FALSE(value);
    testObj[TEST_KEY] = "test";
    EXPECT_FALSE(ResCommonUtil::ParseBoolParameterFromJson("", value, testObj));
    EXPECT_FALSE(value);
    EXPECT_FALSE(ResCommonUtil::ParseBoolParameterFromJson(TEST_BOOL_KEY, value, testObj));
    EXPECT_FALSE(value);
    testObj[TEST_BOOL_KEY] = 100;
    EXPECT_FALSE(ResCommonUtil::ParseBoolParameterFromJson(TEST_BOOL_KEY, value, testObj));
    EXPECT_FALSE(value);
    testObj[TEST_BOOL_KEY] = true;
    EXPECT_TRUE(ResCommonUtil::ParseBoolParameterFromJson(TEST_BOOL_KEY, value, testObj));
    EXPECT_TRUE(value);
}

/**
 * @tc.name: ResSchedJsonUtilTest LoadFileToJsonObj_001
 * @tc.desc: test LoadFileToJsonObj
 * @tc.type: FUNC
 * @tc.require: issueIB8Y0E
 */
HWTEST_F(ResSchedJsonUtilTest, LoadFileToJsonObj_001, Function | MediumTest | Level0)
{
    nlohmann::json testJsonObj;
    EXPECT_FALSE(ResCommonUtil::LoadOneCfgFileToJsonObj("", testJsonObj));
    EXPECT_FALSE(ResCommonUtil::LoadOneCfgFileToJsonObj(TEST_INVALID_JSON, testJsonObj));
    EXPECT_TRUE(ResCommonUtil::LoadOneCfgFileToJsonObj(TEST_VALID_JSON, testJsonObj));
}

/**
 * @tc.name: ResSchedJsonUtilTest GetArrayFromJson_001
 * @tc.desc: test GetArrayFromJson
 * @tc.type: FUNC
 * @tc.require: issueIB8Y0E
 */
HWTEST_F(ResSchedJsonUtilTest, GetArrayFromJson_001, Function | MediumTest | Level0)
{
    nlohmann::json testObj;
    nlohmann::json value;
    EXPECT_FALSE(ResCommonUtil::GetArrayFromJson(testObj, TEST_ARRAY_KEY, value));
    testObj[TEST_KEY] = "test";
    EXPECT_FALSE(ResCommonUtil::GetArrayFromJson(testObj, "", value));
    EXPECT_FALSE(ResCommonUtil::GetArrayFromJson(testObj, TEST_ARRAY_KEY, value));
    testObj[TEST_ARRAY_KEY] = 100;
    EXPECT_FALSE(ResCommonUtil::GetArrayFromJson(testObj, TEST_ARRAY_KEY, value));
    nlohmann::json array = nlohmann::json::array();
    array.push_back(1);
    array.push_back(2);
    array.push_back(3);
    testObj[TEST_ARRAY_KEY] = array;
    EXPECT_TRUE(ResCommonUtil::GetArrayFromJson(testObj, TEST_ARRAY_KEY, value));
}

/**
 * @tc.name: ResSchedJsonUtilTest GetObjFromJson_001
 * @tc.desc: test GetObjFromJson
 * @tc.type: FUNC
 * @tc.require: issueIB8Y0E
 */
HWTEST_F(ResSchedJsonUtilTest, GetObjFromJson_001, Function | MediumTest | Level0)
{
    nlohmann::json testObj;
    nlohmann::json value;
    EXPECT_FALSE(ResCommonUtil::GetObjFromJson(testObj, TEST_OBJ_KEY, value));
    testObj[TEST_KEY] = "test";
    EXPECT_FALSE(ResCommonUtil::GetObjFromJson(testObj, "", value));
    EXPECT_FALSE(ResCommonUtil::GetObjFromJson(testObj, TEST_OBJ_KEY, value));
    testObj[TEST_BOOL_KEY] = 100;
    EXPECT_FALSE(ResCommonUtil::GetObjFromJson(testObj, TEST_OBJ_KEY, value));
    nlohmann::json obj;
    obj[TEST_KEY] = "test";
    testObj[TEST_OBJ_KEY] = obj;
    EXPECT_TRUE(ResCommonUtil::GetObjFromJson(testObj, TEST_OBJ_KEY, value));
}

/**
 * @tc.name: ResSchedJsonUtilTest LoadContentToJsonObj_001
 * @tc.desc: test LoadContentToJsonObj
 * @tc.type: FUNC
 * @tc.require: issueIB8Y0E
 */
HWTEST_F(ResSchedJsonUtilTest, LoadContentToJsonObj_001, Function | MediumTest | Level0)
{
    nlohmann::json testJson;
    EXPECT_FALSE(ResCommonUtil::LoadContentToJsonObj("", testJson));
    EXPECT_FALSE(ResCommonUtil::LoadContentToJsonObj("test{test}test", testJson));
    EXPECT_TRUE(ResCommonUtil::LoadContentToJsonObj("{ \"testKey1\": \"test1\", \"testKey2\": \"test2\" }",
        testJson));
}

/**
 * @tc.name: ResSchedJsonUtilTest DumpJsonToString_001
 * @tc.desc: test DumpJsonToString
 * @tc.type: FUNC
 * @tc.require: issueIB8Y0E
 */
HWTEST_F(ResSchedJsonUtilTest, DumpJsonToString_001, Function | MediumTest | Level0)
{
    nlohmann::json testJson;
    EXPECT_TRUE(ResCommonUtil::LoadContentToJsonObj("{ \"testKey1\": \"test1\", \"testKey2\": \"test2\" }", testJson));
    std::string jsonStr;
    ResCommonUtil::DumpJsonToString(testJson, jsonStr);
    EXPECT_EQ(jsonStr, "{ \"testKey1\": \"test1\", \"testKey2\": \"test2\" }");
}
} // namespace ResourceSchedule
} // namespace OHOS
