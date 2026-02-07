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

#include "res_sched_file_util_test.h"

#include "res_sched_file_util.h"

using namespace std;
using namespace testing::ext;

namespace OHOS {
namespace ResourceSchedule {
namespace {
    const std::string TEST_DIR = "/data/resschedutiltest";
    const std::string TEST_COPY_DIR = "/data/resschedutiltest/copy";
    const std::string TEST_FILE_PATH = "/data/resschedutiltest/test.txt";
    const std::string TEST_COPY_FILE_PATH = "/data/resschedutiltest/copy/test_copy.txt";
    const std::string TEST_CONFIG_DIR = "etc/ressched/res_sched_config.xml";
    const std::string TEST_REAL_CONFIG_FILE_PATH = "/sys_prod/etc/ressched/res_sched_config.xml";
    const std::string TEST_FILE_NAME = "test.txt";
    const std::string TEST_CONTENT = "ressched util test";
    const std::string DATA_DIR = "/data";
}

void ResSchedFileUtilTest::SetUpTestCase() {}

void ResSchedFileUtilTest::TearDownTestCase() {}

void ResSchedFileUtilTest::SetUp() {}

void ResSchedFileUtilTest::TearDown() {}

/**
 * @tc.name: ResSchedFileUtilTest WriteFileReclaim_001
 * @tc.desc: test WriteFileReclaim
 * @tc.type: FUNC
 * @tc.require: issueIB8Y0E
 */
HWTEST_F(ResSchedFileUtilTest, WriteFileReclaim_001, Function | MediumTest | Level0)
{
    ResCommonUtil::WriteFileReclaim(getpid());
    std::string path = "/proc/" + std::to_string(getpid()) + "/reclaim";
    if (ResCommonUtil::PathOrFileExists(path)) {
        EXPECT_TRUE(open(path.c_str(), O_WRONLY) >= 0);
    } else {
        EXPECT_FALSE(open(path.c_str(), O_WRONLY) >= 0);
    }
}

/**
 * @tc.name: ResSchedFileUtilTest GetRealPath_001
 * @tc.desc: test GetRealPath
 * @tc.type: FUNC
 * @tc.require: issueIB8Y0E
 */
HWTEST_F(ResSchedFileUtilTest, GetRealPath_001, Function | MediumTest | Level0)
{
    std::string path = "./";
    std::string realPath;
    EXPECT_TRUE(ResCommonUtil::GetRealPath(path, realPath));
    path = "000/0/00\\00";
    EXPECT_FALSE(ResCommonUtil::GetRealPath(path, realPath));
}

/**
 * @tc.name: ResSchedFileUtilTest PathOrFileExists_001
 * @tc.desc: test PathOrFileExists
 * @tc.type: FUNC
 * @tc.require: issueIB8Y0E
 */
HWTEST_F(ResSchedFileUtilTest, PathOrFileExists_001, Function | MediumTest | Level0)
{
    std::string path = "./";
    EXPECT_TRUE(ResCommonUtil::PathOrFileExists(path));
}

/**
 * @tc.name: ResSchedFileUtilTest DirIterator_001
 * @tc.desc: test DirIterator
 * @tc.type: FUNC
 * @tc.require: issueIB8Y0E
 */
HWTEST_F(ResSchedFileUtilTest, DirIterator_001, Function | MediumTest | Level0)
{
    std::string path = "./";
    std::vector<std::string> result;
    EXPECT_TRUE(ResCommonUtil::DirIterator(path, result));
    path = "000/0/00\\00";
    result.clear();
    EXPECT_FALSE(ResCommonUtil::DirIterator(path, result));
}

/**
 * @tc.name: ResSchedFileUtilTest IsEmptyDir_001
 * @tc.desc: test IsEmptyDir
 * @tc.type: FUNC
 * @tc.require: issueIB8Y0E
 */
HWTEST_F(ResSchedFileUtilTest, IsEmptyDir_001, Function | MediumTest | Level0)
{
    ResCommonUtil::CreateDir(TEST_DIR, S_IXUSR | S_IWUSR | S_IRUSR);
    EXPECT_TRUE(ResCommonUtil::IsEmptyDir(TEST_DIR));
    EXPECT_FALSE(ResCommonUtil::IsEmptyDir(DATA_DIR));
    ResCommonUtil::RemoveDirs(TEST_DIR);
    SUCCEED();
}

/**
 * @tc.name: ResSchedFileUtilTest IsDir_001
 * @tc.desc: test IsDir
 * @tc.type: FUNC
 * @tc.require: issueIB8Y0E
 */
HWTEST_F(ResSchedFileUtilTest, IsDir_001, Function | MediumTest | Level0)
{
    EXPECT_FALSE(ResCommonUtil::IsDir(TEST_DIR));
    ResCommonUtil::CreateDir(TEST_DIR, S_IXUSR | S_IWUSR | S_IRUSR);
    EXPECT_TRUE(ResCommonUtil::IsDir(TEST_DIR));
    ResCommonUtil::RemoveDirs(TEST_DIR);
    SUCCEED();
}

/**
 * @tc.name: ResSchedFileUtilTest CreateDir_001
 * @tc.desc: test CreateDir
 * @tc.type: FUNC
 * @tc.require: issueIB8Y0E
 */
HWTEST_F(ResSchedFileUtilTest, CreateDir_001, Function | MediumTest | Level0)
{
    EXPECT_TRUE(ResCommonUtil::CreateDir(TEST_DIR, S_IXUSR | S_IWUSR | S_IRUSR));
    ResCommonUtil::RemoveDirs(TEST_DIR);
    EXPECT_TRUE(ResCommonUtil::CreateDir(TEST_COPY_DIR, S_IXUSR | S_IWUSR | S_IRUSR));
}

/**
 * @tc.name: ResSchedFileUtilTest RemoveDirs_001
 * @tc.desc: test RemoveDirs
 * @tc.type: FUNC
 * @tc.require: issueIB8Y0E
 */
HWTEST_F(ResSchedFileUtilTest, RemoveDirs_001, Function | MediumTest | Level0)
{
    EXPECT_TRUE(ResCommonUtil::CreateDir(TEST_DIR, S_IXUSR | S_IWUSR | S_IRUSR));
    EXPECT_TRUE(ResCommonUtil::RemoveDirs(TEST_DIR));
    EXPECT_FALSE(ResCommonUtil::RemoveDirs(TEST_DIR));
}

/**
 * @tc.name: ResSchedFileUtilTest RemoveFile_001
 * @tc.desc: test RemoveFile
 * @tc.type: FUNC
 * @tc.require: issueIB8Y0E
 */
HWTEST_F(ResSchedFileUtilTest, RemoveFile_001, Function | MediumTest | Level0)
{
    EXPECT_TRUE(ResCommonUtil::RemoveFile(TEST_FILE_PATH));
    ResCommonUtil::CreateDir(TEST_DIR, S_IXUSR | S_IWUSR | S_IRUSR);
    ResCommonUtil::CreateFile(TEST_FILE_PATH, S_IXUSR | S_IWUSR | S_IRUSR);
    EXPECT_TRUE(ResCommonUtil::RemoveFile(TEST_FILE_PATH));
    ResCommonUtil::RemoveDirs(TEST_DIR);
    SUCCEED();
}

/**
 * @tc.name: ResSchedFileUtilTest CreateFile_001
 * @tc.desc: test CreateFile
 * @tc.type: FUNC
 * @tc.require: issueIB8Y0E
 */
HWTEST_F(ResSchedFileUtilTest, CreateFile_001, Function | MediumTest | Level0)
{
    EXPECT_FALSE(ResCommonUtil::CreateFile(TEST_FILE_PATH, S_IXUSR | S_IWUSR | S_IRUSR));
    ResCommonUtil::CreateDir(TEST_DIR, S_IXUSR | S_IWUSR | S_IRUSR);
    EXPECT_TRUE(ResCommonUtil::CreateFile(TEST_FILE_PATH, S_IXUSR | S_IWUSR | S_IRUSR));
    ResCommonUtil::RemoveFile(TEST_FILE_PATH);
    ResCommonUtil::RemoveDirs(TEST_DIR);
    SUCCEED();
}

/**
 * @tc.name: ResSchedFileUtilTest ExtractFileName_001
 * @tc.desc: test ExtractFileName
 * @tc.type: FUNC
 * @tc.require: issueIB8Y0E
 */
HWTEST_F(ResSchedFileUtilTest, ExtractFileName_001, Function | MediumTest | Level0)
{
    EXPECT_EQ(ResCommonUtil::ExtractFileName(TEST_FILE_PATH), TEST_FILE_NAME);
    EXPECT_EQ(ResCommonUtil::ExtractFileName(TEST_DIR), "resschedutiltest");
}

/**
 * @tc.name: ResSchedFileUtilTest IsBLKPath_001
 * @tc.desc: test IsBLKPath
 * @tc.type: FUNC
 * @tc.require: issueIB8Y0E
 */
HWTEST_F(ResSchedFileUtilTest, IsBLKPath_001, Function | MediumTest | Level0)
{
    std::string path = "/dev/block/sda";
    if (ResCommonUtil::PathOrFileExists(path)) {
        EXPECT_EQ(ResCommonUtil::IsBLKPath(path), true);
    }
    path = "/sys/block/dm-0";
    EXPECT_EQ(ResCommonUtil::IsBLKPath(path), false);
    path = "";
    EXPECT_EQ(ResCommonUtil::IsBLKPath(path), false);
    path = "/sys/block/fo_test";
    EXPECT_EQ(ResCommonUtil::IsBLKPath(path), false);
}

/**
 * @tc.name: ResSchedFileUtilTest SaveStringToFile_001
 * @tc.desc: test SaveStringToFile
 * @tc.type: FUNC
 * @tc.require: issueIB8Y0E
 */
HWTEST_F(ResSchedFileUtilTest, SaveStringToFile_001, Function | MediumTest | Level0)
{
    EXPECT_FALSE(ResCommonUtil::SaveStringToFile(TEST_FILE_PATH, TEST_CONTENT));
    ResCommonUtil::CreateDir(TEST_DIR, S_IXUSR | S_IWUSR | S_IRUSR);
    ResCommonUtil::CreateFile(TEST_FILE_PATH, S_IXUSR | S_IWUSR | S_IRUSR);
    EXPECT_TRUE(ResCommonUtil::SaveStringToFile(TEST_FILE_PATH, TEST_CONTENT));
    ResCommonUtil::RemoveFile(TEST_FILE_PATH);
    ResCommonUtil::RemoveDirs(TEST_DIR);
    SUCCEED();
}

/**
 * @tc.name: ResSchedFileUtilTest ReadLinesFromFile_001
 * @tc.desc: test ReadLinesFromFile
 * @tc.type: FUNC
 * @tc.require: issueIB8Y0E
 */
HWTEST_F(ResSchedFileUtilTest, ReadLinesFromFile_001, Function | MediumTest | Level0)
{
    ResCommonUtil::CreateDir(TEST_DIR, S_IXUSR | S_IWUSR | S_IRUSR);
    ResCommonUtil::CreateFile(TEST_FILE_PATH, S_IXUSR | S_IWUSR | S_IRUSR);
    std::vector<std::string> texts = {"ressched", "util", "test"};
    std::string content = "";
    for (auto& text : texts) {
        ResCommonUtil::SaveStringToFile(TEST_FILE_PATH, text, false);
        ResCommonUtil::SaveStringToFile(TEST_FILE_PATH, "\n", false);
    }
    std::vector<std::string> lines;
    EXPECT_TRUE(ResCommonUtil::ReadLinesFromFile(TEST_FILE_PATH, lines));
    for (std::size_t i = 0; i < lines.size(); ++i) {
        EXPECT_EQ(lines[i], texts[i]);
    }
    ResCommonUtil::RemoveFile(TEST_FILE_PATH);
    ResCommonUtil::RemoveDirs(TEST_DIR);
}

/**
 * @tc.name: ResSchedFileUtilTest CopyFile_001
 * @tc.desc: test CopyFile
 * @tc.type: FUNC
 * @tc.require: issueIB8Y0E
 */
HWTEST_F(ResSchedFileUtilTest, CopyFile_001, Function | MediumTest | Level0)
{
    EXPECT_FALSE(ResCommonUtil::CopyFile(TEST_FILE_PATH, TEST_COPY_FILE_PATH));
    ResCommonUtil::CreateDir(TEST_DIR, S_IXUSR | S_IWUSR | S_IRUSR);
    ResCommonUtil::CreateFile(TEST_FILE_PATH, S_IXUSR | S_IWUSR | S_IRUSR);
    EXPECT_TRUE(ResCommonUtil::CopyFile(TEST_FILE_PATH, TEST_COPY_DIR));
    ResCommonUtil::RemoveFile(TEST_FILE_PATH);
    ResCommonUtil::RemoveFile(TEST_COPY_FILE_PATH);
    ResCommonUtil::RemoveDirs(TEST_DIR);
    EXPECT_FALSE(ResCommonUtil::CopyFile(TEST_FILE_PATH, TEST_COPY_DIR));
}

/**
 * @tc.name: ResSchedFileUtilTest GetRealConfigPath_001
 * @tc.desc: test GetRealConfigPath
 * @tc.type: FUNC
 * @tc.require: issueIB8Y0E
 */
HWTEST_F(ResSchedFileUtilTest, GetRealConfigPath_001, Function | MediumTest | Level0)
{
    std::string configPath = "";
    std::string realConfigPath;
    EXPECT_FALSE(ResCommonUtil::GetRealConfigPath(configPath, realConfigPath));
    EXPECT_TRUE(ResCommonUtil::GetRealConfigPath(TEST_CONFIG_DIR, realConfigPath));
}

/**
 * @tc.name: ResSchedFileUtilTest RemoveFileOrDirIfExist_001
 * @tc.desc: test RemoveFileOrDirIfExist
 * @tc.type: FUNC
 */
HWTEST_F(ResSchedFileUtilTest, RemoveFileOrDirIfExist_001, Function | MediumTest | Level0)
{
    EXPECT_TRUE(ResCommonUtil::RemoveFileOrDirIfExist(TEST_COPY_FILE_PATH));
    ResCommonUtil::CreateDir(TEST_DIR, S_IXUSR | S_IWUSR | S_IRUSR);
    EXPECT_TRUE(ResCommonUtil::RemoveFileOrDirIfExist(TEST_DIR));
    ResCommonUtil::CreateFile(TEST_FILE_PATH, S_IXUSR | S_IWUSR | S_IRUSR);
    EXPECT_TRUE(ResCommonUtil::RemoveFileOrDirIfExist(TEST_FILE_PATH));
}
} // namespace ResourceSchedule
} // namespace OHOS
