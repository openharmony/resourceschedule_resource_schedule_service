/*
 * Copyright (c) 2026-2026 Huawei Device Co., Ltd.
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

#include "res_sched_json_storage_util_test.h"

#include <unistd.h>
#include <fcntl.h>
#include <fstream>
#include <sstream>

#include "errors.h"
#include "res_sched_json_storage_util.h"
#include "res_sched_file_util.h"

using namespace std;
using namespace testing::ext;

namespace OHOS {
namespace ResourceSchedule {
using namespace ResCommonUtil;

namespace {
const std::string TEST_DIR = "/data/resschedjsonstoragetest";
const std::string TEST_FILE_PATH = "/data/resschedjsonstoragetest/test.json";
const std::string TEST_SUBDIR = "/data/resschedjsonstoragetest/subdir";
const std::string TEST_SUBFILE_PATH = "/data/resschedjsonstoragetest/subdir/test.json";
const std::string TEST_VALID_JSON = R"({"key1":"value1", "key2":123, "key3":true})";
const std::string TEST_INVALID_JSON = R"({"key1":"value1", "key2":123,})";
}

void ResSchedJsonStorageUtilTest::SetUpTestCase() {}

void ResSchedJsonStorageUtilTest::TearDownTestCase() {}

void ResSchedJsonStorageUtilTest::SetUp()
{
    ResCommonUtil::RemoveFileOrDirIfExist(TEST_DIR);
}

void ResSchedJsonStorageUtilTest::TearDown()
{
    ResCommonUtil::RemoveFileOrDirIfExist(TEST_DIR);
}

/**
 * @tc.name: ResSchedJsonStorageUtilTest RefreshJsonFileInfo_001
 * @tc.desc: test RefreshJsonFileInfo with valid JSON
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(ResSchedJsonStorageUtilTest, RefreshJsonFileInfo_001, Function | MediumTest | Level0)
{
    ResCommonUtil::CreateDir(TEST_DIR, S_IXUSR | S_IWUSR | S_IRUSR);
    int32_t result = ResCommonUtil::ResSchedJsonStorage::RefreshJsonFileInfo(TEST_VALID_JSON, TEST_FILE_PATH);
    EXPECT_EQ(result, ERR_OK);

    nlohmann::json value;
    result = ResCommonUtil::ResSchedJsonStorage::RestoreJsonFileInfo(value, TEST_FILE_PATH);
    EXPECT_EQ(result, ERR_OK);
    EXPECT_EQ(value["key1"], "value1");
    EXPECT_EQ(value["key2"], 123);
    EXPECT_EQ(value["key3"], true);
}

/**
 * @tc.name: ResSchedJsonStorageUtilTest RefreshJsonFileInfo_002
 * @tc.desc: test RefreshJsonFileInfo with invalid file path
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(ResSchedJsonStorageUtilTest, RefreshJsonFileInfo_002, Function | MediumTest | Level0)
{
    int32_t result = ResCommonUtil::ResSchedJsonStorage::RefreshJsonFileInfo(TEST_VALID_JSON, "");
    EXPECT_EQ(result, -1);
}

/**
 * @tc.name: ResSchedJsonStorageUtilTest RefreshJsonFileInfo_003
 * @tc.desc: test RefreshJsonFileInfo with non-existent directory
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(ResSchedJsonStorageUtilTest, RefreshJsonFileInfo_003, Function | MediumTest | Level0)
{
    int32_t result = ResCommonUtil::ResSchedJsonStorage::RefreshJsonFileInfo(TEST_VALID_JSON, TEST_FILE_PATH);
    EXPECT_NE(result, ERR_OK);
}

/**
 * @tc.name: ResSchedJsonStorageUtilTest RefreshJsonFileInfo_004
 * @tc.desc: test RefreshJsonFileInfo with subdirectory path
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(ResSchedJsonStorageUtilTest, RefreshJsonFileInfo_004, Function | MediumTest | Level0)
{
    int32_t result = ResCommonUtil::ResSchedJsonStorage::RefreshJsonFileInfo(TEST_VALID_JSON, TEST_SUBFILE_PATH);
    EXPECT_NE(result, ERR_OK);
}

/**
 * @tc.name: ResSchedJsonStorageUtilTest RestoreJsonFileInfo_001
 * @tc.desc: test RestoreJsonFileInfo with valid JSON file
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(ResSchedJsonStorageUtilTest, RestoreJsonFileInfo_001, Function | MediumTest | Level0)
{
    ResCommonUtil::CreateDir(TEST_DIR, S_IXUSR | S_IWUSR | S_IRUSR);
    ResCommonUtil::ResSchedJsonStorage::RefreshJsonFileInfo(TEST_VALID_JSON, TEST_FILE_PATH);

    nlohmann::json value;
    int32_t result = ResCommonUtil::ResSchedJsonStorage::RestoreJsonFileInfo(value, TEST_FILE_PATH);
    EXPECT_EQ(result, ERR_OK);
    EXPECT_FALSE(value.is_discarded());
    EXPECT_EQ(value["key1"], "value1");
    EXPECT_EQ(value["key2"], 123);
    EXPECT_EQ(value["key3"], true);
}

/**
 * @tc.name: ResSchedJsonStorageUtilTest RestoreJsonFileInfo_002
 * @tc.desc: test RestoreJsonFileInfo with non-existent file
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(ResSchedJsonStorageUtilTest, RestoreJsonFileInfo_002, Function | MediumTest | Level0)
{
    nlohmann::json value;
    int32_t result = ResCommonUtil::ResSchedJsonStorage::RestoreJsonFileInfo(value, TEST_FILE_PATH);
    EXPECT_EQ(result, -1);
}

/**
 * @tc.name: ResSchedJsonStorageUtilTest RestoreJsonFileInfo_003
 * @tc.desc: test RestoreJsonFileInfo with invalid JSON content
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(ResSchedJsonStorageUtilTest, RestoreJsonFileInfo_003, Function | MediumTest | Level0)
{
    ResCommonUtil::CreateDir(TEST_DIR, S_IXUSR | S_IWUSR | S_IRUSR);
    ResCommonUtil::ResSchedJsonStorage::RefreshJsonFileInfo(TEST_INVALID_JSON, TEST_FILE_PATH);

    nlohmann::json value;
    int32_t result = ResCommonUtil::ResSchedJsonStorage::RestoreJsonFileInfo(value, TEST_FILE_PATH);
    EXPECT_EQ(result, -1);
}

/**
 * @tc.name: ResSchedJsonStorageUtilTest RestoreJsonFileInfo_004
 * @tc.desc: test RestoreJsonFileInfo with empty file path
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(ResSchedJsonStorageUtilTest, RestoreJsonFileInfo_004, Function | MediumTest | Level0)
{
    nlohmann::json value;
    int32_t result = ResCommonUtil::ResSchedJsonStorage::RestoreJsonFileInfo(value, "");
    EXPECT_EQ(result, -1);
}

/**
 * @tc.name: ResSchedJsonStorageUtilTest RestoreJsonFileInfo_005
 * @tc.desc: test RestoreJsonFileInfo with empty JSON file
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(ResSchedJsonStorageUtilTest, RestoreJsonFileInfo_005, Function | MediumTest | Level0)
{
    ResCommonUtil::CreateDir(TEST_DIR, S_IXUSR | S_IWUSR | S_IRUSR);
    ResCommonUtil::ResSchedJsonStorage::RefreshJsonFileInfo("", TEST_FILE_PATH);

    nlohmann::json value;
    int32_t result = ResCommonUtil::ResSchedJsonStorage::RestoreJsonFileInfo(value, TEST_FILE_PATH);
    EXPECT_NE(result, ERR_OK);
}

/**
 * @tc.name: ResSchedJsonStorageUtilTest RestoreJsonFileInfo_006
 * @tc.desc: test RestoreJsonFileInfo with complex JSON structure
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(ResSchedJsonStorageUtilTest, RestoreJsonFileInfo_006, Function | MediumTest | Level0)
{
    ResCommonUtil::CreateDir(TEST_DIR, S_IXUSR | S_IWUSR | S_IRUSR);
    std::string complexJson = R"({"array":[1,2,3], "nested":{"key":"value"}, "number":42.5})";
    ResCommonUtil::ResSchedJsonStorage::RefreshJsonFileInfo(complexJson, TEST_FILE_PATH);

    nlohmann::json value;
    int32_t result = ResCommonUtil::ResSchedJsonStorage::RestoreJsonFileInfo(value, TEST_FILE_PATH);
    EXPECT_EQ(result, ERR_OK);
    EXPECT_EQ(value["array"].size(), 3);
    EXPECT_EQ(value["nested"]["key"], "value");
    EXPECT_EQ(value["number"], 42.5);
}

/**
 * @tc.name: ResSchedJsonStorageUtilTest SaveJsonValueToFile_001
 * @tc.desc: test SaveJsonValueToFile with valid JSON string
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(ResSchedJsonStorageUtilTest, SaveJsonValueToFile_001, Function | MediumTest | Level0)
{
    ResCommonUtil::CreateDir(TEST_DIR, S_IXUSR | S_IWUSR | S_IRUSR);
    int32_t result = ResCommonUtil::ResSchedJsonStorage::RefreshJsonFileInfo(TEST_VALID_JSON, TEST_FILE_PATH);
    EXPECT_EQ(result, ERR_OK);

    std::ifstream fin(TEST_FILE_PATH);
    EXPECT_TRUE(fin.is_open());
    std::string content((std::istreambuf_iterator<char>(fin)), std::istreambuf_iterator<char>());
    fin.close();
    EXPECT_EQ(content, TEST_VALID_JSON + "\n");
}

/**
 * @tc.name: ResSchedJsonStorageUtilTest SaveJsonValueToFile_002
 * @tc.desc: test SaveJsonValueToFile with empty JSON string
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(ResSchedJsonStorageUtilTest, SaveJsonValueToFile_002, Function | MediumTest | Level0)
{
    ResCommonUtil::CreateDir(TEST_DIR, S_IXUSR | S_IWUSR | S_IRUSR);
    int32_t result = ResCommonUtil::ResSchedJsonStorage::RefreshJsonFileInfo("", TEST_FILE_PATH);
    EXPECT_EQ(result, ERR_OK);
}

/**
 * @tc.name: ResSchedJsonStorageUtilTest SaveJsonValueToFile_003
 * @tc.desc: test SaveJsonValueToFile with existing file
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(ResSchedJsonStorageUtilTest, SaveJsonValueToFile_003, Function | MediumTest | Level0)
{
    ResCommonUtil::CreateDir(TEST_DIR, S_IXUSR | S_IWUSR | S_IRUSR);
    ResCommonUtil::ResSchedJsonStorage::RefreshJsonFileInfo(TEST_VALID_JSON, TEST_FILE_PATH);

    std::string newJson = R"({"newKey":"newValue"})";
    int32_t result = ResCommonUtil::ResSchedJsonStorage::RefreshJsonFileInfo(newJson, TEST_FILE_PATH);
    EXPECT_EQ(result, ERR_OK);

    nlohmann::json value;
    ResCommonUtil::ResSchedJsonStorage::RestoreJsonFileInfo(value, TEST_FILE_PATH);
    EXPECT_EQ(value["newKey"], "newValue");
}

/**
 * @tc.name: ResSchedJsonStorageUtilTest SaveJsonValueToFile_004
 * @tc.desc: test SaveJsonValueToFile with long JSON string
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(ResSchedJsonStorageUtilTest, SaveJsonValueToFile_004, Function | MediumTest | Level0)
{
    ResCommonUtil::CreateDir(TEST_DIR, S_IXUSR | S_IWUSR | S_IRUSR);
    std::string longJson = R"({)";
    for (int32_t i = 0; i < 100; i++) {
        longJson += "\"key" + std::to_string(i) + "\":\"value" + std::to_string(i) + "\",";
    }
    longJson.back() = '}';

    int32_t result = ResCommonUtil::ResSchedJsonStorage::RefreshJsonFileInfo(longJson, TEST_FILE_PATH);
    EXPECT_EQ(result, ERR_OK);
}

/**
 * @tc.name: ResSchedJsonStorageUtilTest ParseJsonValueFromFile_001
 * @tc.desc: test ParseJsonValueFromFile with valid JSON file
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(ResSchedJsonStorageUtilTest, ParseJsonValueFromFile_001, Function | MediumTest | Level0)
{
    ResCommonUtil::CreateDir(TEST_DIR, S_IXUSR | S_IWUSR | S_IRUSR);
    ResCommonUtil::ResSchedJsonStorage::RefreshJsonFileInfo(TEST_VALID_JSON, TEST_FILE_PATH);

    nlohmann::json value;
    int32_t result = ResCommonUtil::ResSchedJsonStorage::RestoreJsonFileInfo(value, TEST_FILE_PATH);
    EXPECT_EQ(result, ERR_OK);
    EXPECT_FALSE(value.is_discarded());
}

/**
 * @tc.name: ResSchedJsonStorageUtilTest ParseJsonValueFromFile_002
 * @tc.desc: test ParseJsonValueFromFile with multi-line JSON
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(ResSchedJsonStorageUtilTest, ParseJsonValueFromFile_002, Function | MediumTest | Level0)
{
    ResCommonUtil::CreateDir(TEST_DIR, S_IXUSR | S_IWUSR | S_IRUSR);
    std::string multiLineJson = R"({
        "key1": "value1",
        "key2": 123,
        "key3": true
    })";
    ResCommonUtil::ResSchedJsonStorage::RefreshJsonFileInfo(multiLineJson, TEST_FILE_PATH);

    nlohmann::json value;
    int32_t result = ResCommonUtil::ResSchedJsonStorage::RestoreJsonFileInfo(value, TEST_FILE_PATH);
    EXPECT_EQ(result, ERR_OK);
    EXPECT_EQ(value["key1"], "value1");
    EXPECT_EQ(value["key2"], 123);
    EXPECT_EQ(value["key3"], true);
}

/**
 * @tc.name: ResSchedJsonStorageUtilTest CreateNodeFile_001
 * @tc.desc: test CreateNodeFile with new file
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(ResSchedJsonStorageUtilTest, CreateNodeFile_001, Function | MediumTest | Level0)
{
    ResCommonUtil::CreateDir(TEST_DIR, S_IXUSR | S_IWUSR | S_IRUSR);
    int32_t result = ResCommonUtil::ResSchedJsonStorage::RefreshJsonFileInfo(TEST_VALID_JSON, TEST_FILE_PATH);
    EXPECT_EQ(result, ERR_OK);
    EXPECT_TRUE(ResCommonUtil::PathOrFileExists(TEST_FILE_PATH));
}

/**
 * @tc.name: ResSchedJsonStorageUtilTest CreateNodeFile_002
 * @tc.desc: test CreateNodeFile with existing file
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(ResSchedJsonStorageUtilTest, CreateNodeFile_002, Function | MediumTest | Level0)
{
    ResCommonUtil::CreateDir(TEST_DIR, S_IXUSR | S_IWUSR | S_IRUSR);
    ResCommonUtil::ResSchedJsonStorage::RefreshJsonFileInfo(TEST_VALID_JSON, TEST_FILE_PATH);

    int32_t result = ResCommonUtil::ResSchedJsonStorage::RefreshJsonFileInfo(TEST_VALID_JSON, TEST_FILE_PATH);
    EXPECT_EQ(result, ERR_OK);
}

/**
 * @tc.name: ResSchedJsonStorageUtilTest CreateNodeFile_003
 * @tc.desc: test CreateNodeFile with invalid path
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(ResSchedJsonStorageUtilTest, CreateNodeFile_003, Function | MediumTest | Level0)
{
    int32_t result = ResCommonUtil::ResSchedJsonStorage::RefreshJsonFileInfo(TEST_VALID_JSON, "");
    EXPECT_EQ(result, -1);
}

/**
 * @tc.name: ResSchedJsonStorageUtilTest CreateNodeFile_004
 * @tc.desc: test CreateNodeFile with path without separator
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(ResSchedJsonStorageUtilTest, CreateNodeFile_004, Function | MediumTest | Level0)
{
    std::string invalidPath = "invalidfilename.json";
    int32_t result = ResCommonUtil::ResSchedJsonStorage::RefreshJsonFileInfo(TEST_VALID_JSON, invalidPath);
    EXPECT_EQ(result, -1);
}

/**
 * @tc.name: ResSchedJsonStorageUtilTest ConvertFullPath_001
 * @tc.desc: test ConvertFullPath with valid path
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(ResSchedJsonStorageUtilTest, ConvertFullPath_001, Function | MediumTest | Level0)
{
    ResCommonUtil::CreateDir(TEST_DIR, S_IXUSR | S_IWUSR | S_IRUSR);
    int32_t result = ResCommonUtil::ResSchedJsonStorage::RefreshJsonFileInfo(TEST_VALID_JSON, TEST_FILE_PATH);
    EXPECT_EQ(result, ERR_OK);
}

/**
 * @tc.name: ResSchedJsonStorageUtilTest ConvertFullPath_002
 * @tc.desc: test ConvertFullPath with empty path
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(ResSchedJsonStorageUtilTest, ConvertFullPath_002, Function | MediumTest | Level0)
{
    int32_t result = ResCommonUtil::ResSchedJsonStorage::RefreshJsonFileInfo(TEST_VALID_JSON, "");
    EXPECT_EQ(result, -1);
}

/**
 * @tc.name: ResSchedJsonStorageUtilTest ConvertFullPath_003
 * @tc.desc: test ConvertFullPath with path exceeding PATH_MAX
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(ResSchedJsonStorageUtilTest, ConvertFullPath_003, Function | MediumTest | Level0)
{
    std::string longPath(PATH_MAX + 10, 'a');
    int32_t result = ResCommonUtil::ResSchedJsonStorage::RefreshJsonFileInfo(TEST_VALID_JSON, longPath);
    EXPECT_EQ(result, -1);
}

/**
 * @tc.name: ResSchedJsonStorageUtilTest ConvertFullPath_004
 * @tc.desc: test ConvertFullPath with non-existent path
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(ResSchedJsonStorageUtilTest, ConvertFullPath_004, Function | MediumTest | Level0)
{
    std::string nonExistentPath = "/non/existent/path/test.json";
    int32_t result = ResCommonUtil::ResSchedJsonStorage::RefreshJsonFileInfo(TEST_VALID_JSON, nonExistentPath);
    EXPECT_NE(result, ERR_OK);
}

/**
 * @tc.name: ResSchedJsonStorageUtilTest RefreshRestoreCycle_001
 * @tc.desc: test complete refresh and restore cycle
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(ResSchedJsonStorageUtilTest, RefreshRestoreCycle_001, Function | MediumTest | Level0)
{
    ResCommonUtil::CreateDir(TEST_DIR, S_IXUSR | S_IWUSR | S_IRUSR);

    std::string originalJson = R"({"test":"data", "number":42})";
    int32_t saveResult = ResCommonUtil::ResSchedJsonStorage::RefreshJsonFileInfo(originalJson, TEST_FILE_PATH);
    EXPECT_EQ(saveResult, ERR_OK);

    nlohmann::json restoredValue;
    int32_t loadResult = ResCommonUtil::ResSchedJsonStorage::RestoreJsonFileInfo(restoredValue, TEST_FILE_PATH);
    EXPECT_EQ(loadResult, ERR_OK);
    EXPECT_EQ(restoredValue["test"], "data");
    EXPECT_EQ(restoredValue["number"], 42);
}

/**
 * @tc.name: ResSchedJsonStorageUtilTest RefreshRestoreCycle_002
 * @tc.desc: test multiple refresh and restore cycles
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(ResSchedJsonStorageUtilTest, RefreshRestoreCycle_002, Function | MediumTest | Level0)
{
    ResCommonUtil::CreateDir(TEST_DIR, S_IXUSR | S_IWUSR | S_IRUSR);

    for (int32_t i = 0; i < 3; i++) {
        std::string json = R"({"iteration":)" + std::to_string(i) + R"(})";
        int32_t result = ResCommonUtil::ResSchedJsonStorage::RefreshJsonFileInfo(json, TEST_FILE_PATH);
        EXPECT_EQ(result, ERR_OK);

        nlohmann::json value;
        result = ResCommonUtil::ResSchedJsonStorage::RestoreJsonFileInfo(value, TEST_FILE_PATH);
        EXPECT_EQ(result, ERR_OK);
        EXPECT_EQ(value["iteration"], i);
    }
}

/**
 * @tc.name: ResSchedJsonStorageUtilTest RefreshRestoreCycle_003
 * @tc.desc: test refresh and restore with nested JSON
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(ResSchedJsonStorageUtilTest, RefreshRestoreCycle_003, Function | MediumTest | Level0)
{
    ResCommonUtil::CreateDir(TEST_DIR, S_IXUSR | S_IWUSR | S_IRUSR);

    std::string nestedJson = R"({
        "level1": {
            "level2": {
                "level3": {
                    "value": "deep"
                }
            }
        },
        "array": [1, 2, {"nested": "object"}]
    })";

    int32_t saveResult = ResCommonUtil::ResSchedJsonStorage::RefreshJsonFileInfo(nestedJson, TEST_FILE_PATH);
    EXPECT_EQ(saveResult, ERR_OK);

    nlohmann::json restoredValue;
    int32_t loadResult = ResCommonUtil::ResSchedJsonStorage::RestoreJsonFileInfo(restoredValue, TEST_FILE_PATH);
    EXPECT_EQ(loadResult, ERR_OK);
    EXPECT_EQ(restoredValue["level1"]["level2"]["level3"]["value"], "deep");
    EXPECT_EQ(restoredValue["array"][2]["nested"], "object");
}

} // namespace ResourceSchedule
} // namespace OHOS