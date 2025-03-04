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

#include "gtest/gtest.h"

#include <unordered_map>
#include <vector>

#define private public
#include "socperf_executor_config.h"
#undef private

namespace OHOS {
namespace ResourceSchedule {
using namespace std;
using namespace testing::ext;

namespace {
}

class SocPerfExecutorConfigTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void SocPerfExecutorConfigTest::SetUpTestCase(void) {}

void SocPerfExecutorConfigTest::TearDownTestCase() {}

void SocPerfExecutorConfigTest::SetUp() {}

void SocPerfExecutorConfigTest::TearDown() {}

/**
* @tc.name: SocPerfExecutorConfigTest_API_001
* @tc.desc: test socperf_executor_config api
* @tc.type: FUNC
*/
HWTEST_F(SocPerfExecutorConfigTest, SocPerfExecutorConfigTest_API_001, Function | MediumTest | Level0)
{
    bool ret = SocPerfConfig::GetInstance().Init();
    EXPECT_TRUE(ret);
}

/**
* @tc.name: SocPerfExecutorConfigTest_API_002
* @tc.desc: test socperf_executor_config api
* @tc.type: FUNC
*/
HWTEST_F(SocPerfExecutorConfigTest, SocPerfExecutorConfigTest_API_002, Function | MediumTest | Level0)
{
    SocPerfConfig::GetInstance().resourceNodeInfo_[1] = std::make_shared<ResourceNode>(1, "node1", 0, 1, 0);
    bool ret = SocPerfConfig::GetInstance().IsGovResId(1);
    EXPECT_TRUE(ret);

    SocPerfConfig::GetInstance().resourceNodeInfo_[2] = std::make_shared<ResourceNode>(2, "node2", 0, 0, 0);
    ret = SocPerfConfig::GetInstance().IsGovResId(2);
    EXPECT_FALSE(ret);
}

/**
* @tc.name: SocPerfExecutorConfigTest_API_003
* @tc.desc: test socperf_executor_config api
* @tc.type: FUNC
*/
HWTEST_F(SocPerfExecutorConfigTest, SocPerfExecutorConfigTest_API_003, Function | MediumTest | Level0)
{
    SocPerfConfig& config = SocPerfConfig::GetInstance();
    int32_t resId = 100;
    EXPECT_FALSE(config.IsValidResId(resId));

    resId = 101;
    config.resourceNodeInfo_[resId] = std::make_shared<ResNode>(1, "node1", 0, 2, 0);
    EXPECT_TRUE(config.IsValidResId(resId));
}

/**
* @tc.name: SocPerfExecutorConfigTest_API_004
* @tc.desc: test socperf_executor_config api
* @tc.type: FUNC
*/
HWTEST_F(SocPerfExecutorConfigTest, SocPerfExecutorConfigTest_API_004, Function | MediumTest | Level0)
{
    std::string invalidConfigFile = "invalid_config_file";
    SocPerfConfig& config = SocPerfConfig::GetInstance();
    EXPECT_EQ(config.GetRealConfigPath(invalidConfigFile), "");

    std::string validConfigFile = SOCPERF_RESOURCE_CONFIG_XML;
    EXPECT_NE(config.GetRealConfigPath(invalidConfigFile), "");
}

/**
* @tc.name: SocPerfExecutorConfigTest_API_005
* @tc.desc: test socperf_executor_config api
* @tc.type: FUNC
*/
HWTEST_F(SocPerfExecutorConfigTest, SocPerfExecutorConfigTest_API_005, Function | MediumTest | Level0)
{
    std::string configFile = SOCPERF_RESOURCE_CONFIG_XML;
    auto ret = SocPerfConfig::GetInstance().GetAllRealConfigPath(configFile);
    EXPECT_TRUE(ret.size() > 0);
}

/**
* @tc.name: SocPerfExecutorConfigTest_API_006
* @tc.desc: test socperf_executor_config api
* @tc.type: FUNC
*/
HWTEST_F(SocPerfExecutorConfigTest, SocPerfExecutorConfigTest_API_006, Function | MediumTest | Level0)
{
    int32_t resId = 100;
    int32_t expected = RES_ID_NUMS_PER_TYPE;
    int32_t actual = SocPerfConfig::GetInstance().GetResIdNumsPerType(resId);
    EXPECT_EQ(expected, actual);

    SocPerfConfig::GetInstance().resourceNodeInfo_[resId] =
        std::make_shared<ResourceNode>(3, "gov_node", REPORT_TO_PERFSO, true, false);
    expected = RES_ID_NUMS_PER_TYPE_EXT;
    actual = SocPerfConfig::GetInstance().GetResIdNumsPerType(resId);
    EXPECT_EQ(expected, actual);
}

/**
* @tc.name: SocPerfExecutorConfigTest_API_007
* @tc.desc: test socperf_executor_config api
* @tc.type: FUNC
*/
HWTEST_F(SocPerfExecutorConfigTest, SocPerfExecutorConfigTest_API_007, Function | MediumTest | Level0)
{
    std::string configFile = "/sys_prod/etc/soc_perf/socperf_resource_config.xml";
    bool ret = SocPerfConfig::GetInstance().CheckResourceTag(
        "invalid", "name", "pair", "mode", "persistMode", configFile);
    EXPECT_FALSE(ret);

    ret = SocPerfConfig::GetInstance().CheckResourceTag(
        "1001", nullptr, "pair", "mode", "persistMode", configFile);
    EXPECT_FALSE(ret);

    ret = SocPerfConfig::GetInstance().CheckResourceTag(
        "1001", "name", "invalid", "mode", "persistMode", configFile);
    EXPECT_FALSE(ret);

    ret = SocPerfConfig::GetInstance().CheckResourceTag(
        "1001", "name", "1002", "invalid", "persistMode", configFile);
    EXPECT_FALSE(ret);

    ret = SocPerfConfig::GetInstance().CheckResourceTag(
        "1001", "name", "pair",  "123", "persistMode", configFile);
    EXPECT_FALSE(ret);

    ret = SocPerfConfig::GetInstance().CheckResourceTag(
        "1001", "name", "1002",  "123", "0", configFile);
    EXPECT_TRUE(ret);
}

/**
* @tc.name: SocPerfExecutorConfigTest_API_008
* @tc.desc: test socperf_executor_config api
* @tc.type: FUNC
*/
HWTEST_F(SocPerfExecutorConfigTest, SocPerfExecutorConfigTest_API_008, Function | MediumTest | Level0)
{
    std::string configFile = "/sys_prod/etc/soc_perf/socperf_resource_config.xml";
    EXPECT_FALSE(SocPerfConfig::GetInstance().CheckResourcePersistMode("3", configFile));
    EXPECT_TRUE(SocPerfConfig::GetInstance().CheckResourcePersistMode(nullptr, configFile));
}

/**
* @tc.name: SocPerfExecutorConfigTest_API_009
* @tc.desc: test socperf_executor_config api
* @tc.type: FUNC
*/
HWTEST_F(SocPerfExecutorConfigTest, SocPerfExecutorConfigTest_API_009, Function | MediumTest | Level0)
{
    const char* def1 = nullptr;
    const char* def2 = "123";
    const char* path1 = nullptr;
    const char* path2 = "test_path";
    std::string configFile = "/sys_prod/etc/soc_perf/socperf_resource_config.xml";
    bool ret = SocPerfConfig::GetInstance().CheckResourceTag(REPORT_TO_PERFSO, def1, path2, configFile);
    EXPECT_FALSE(ret);

    ret = SocPerfConfig::GetInstance().CheckResourceTag(0, def2, path1, configFile);
    EXPECT_FALSE(ret);

    ret = SocPerfConfig::GetInstance().CheckResourceTag(REPORT_TO_PERFSO, def2, path2, configFile);
    EXPECT_TRUE(ret);
}

/**
* @tc.name: SocPerfExecutorConfigTest_API_010
* @tc.desc: test socperf_executor_config api
* @tc.type: FUNC
*/
HWTEST_F(SocPerfExecutorConfigTest, SocPerfExecutorConfigTest_API_010, Function | MediumTest | Level0)
{
    auto resNode = std::make_shared<ResNode>(1, "test", 1, 1, 1);
    const char* node = "123 456 789";
    EXPECT_TRUE(SocPerfConfig::GetInstance().LoadResourceAvailable(resNode, node));
    EXPECT_EQ(resNode->available.size(), 3);

    const char* node1 = "123 abc 789";
    EXPECT_FALSE(SocPerfConfig::GetInstance().LoadResourceAvailable(resNode, node1));

    const char* node2 = "";
    EXPECT_TRUE(SocPerfConfig::GetInstance().LoadResourceAvailable(resNode, node2));
}

/**
* @tc.name: SocPerfExecutorConfigTest_API_011
* @tc.desc: test socperf_executor_config api
* @tc.type: FUNC
*/
HWTEST_F(SocPerfExecutorConfigTest, SocPerfExecutorConfigTest_API_011, Function | MediumTest | Level0)
{
    SocPerfConfig::GetInstance().resourceNodeInfo_[1] = std::make_shared<ResNode>(1, "node1", 0, 2, 0);
    SocPerfConfig::GetInstance().resourceNodeInfo_[2] = std::make_shared<ResNode>(2, "node2", 0, 1, 0);
    EXPECT_TRUE(SocPerfConfig::GetInstance().CheckPairResIdValid());

    SocPerfConfig::GetInstance().resourceNodeInfo_[2] = std::make_shared<ResNode>(2, "node2", 0, 3, 0);
    EXPECT_FALSE(SocPerfConfig::GetInstance().CheckPairResIdValid());

    SocPerfConfig::GetInstance().resourceNodeInfo_[2] = std::make_shared<ResNode>(2, "node2", 0, 1, 0);
    SocPerfConfig::GetInstance().resourceNodeInfo_[3] = std::make_shared<ResourceNode>(3, "gov_node", 0, true, false);
    EXPECT_TRUE(SocPerfConfig::GetInstance().CheckPairResIdValid());
}

/**
* @tc.name: SocPerfExecutorConfigTest_API_012
* @tc.desc: test socperf_executor_config api
* @tc.type: FUNC
*/
HWTEST_F(SocPerfExecutorConfigTest, SocPerfExecutorConfigTest_API_012, Function | MediumTest | Level0)
{
    EXPECT_TRUE(SocPerfConfig::GetInstance().CheckGovResourceTag("1001", "testName", "1", "testConfigFile"));
    EXPECT_FALSE(SocPerfConfig::GetInstance().CheckGovResourceTag("invalid", "testName", "1", "testConfigFile"));
    EXPECT_FALSE(SocPerfConfig::GetInstance().CheckGovResourceTag("1001", nullptr, "1", "testConfigFile"));
    EXPECT_FALSE(SocPerfConfig::GetInstance().CheckGovResourceTag("1001", "testName", "invalid", "testConfigFile"));
}

/**
* @tc.name: SocPerfExecutorConfigTest_API_013
* @tc.desc: test socperf_executor_config api
* @tc.type: FUNC
*/
HWTEST_F(SocPerfExecutorConfigTest, SocPerfExecutorConfigTest_API_013, Function | MediumTest | Level0)
{
    auto govResNode = std::make_shared<GovResNode>(1, "test", 1);
    govResNode->paths.push_back("path1");
    govResNode->paths.push_back("path2");
    const char* level = "1";
    const char* node = "path1|path2";
    bool ret = SocPerfConfig::GetInstance().LoadGovResourceAvailable(govResNode, level, node);
    EXPECT_TRUE(ret);
    EXPECT_EQ(govResNode->levelToStr.size(), 1);
    EXPECT_EQ(govResNode->levelToStr[1][0], "path1");
    EXPECT_EQ(govResNode->levelToStr[1][1], "path2");

    node = "path1";
    ret = SocPerfConfig::GetInstance().LoadGovResourceAvailable(govResNode, level, node);
    EXPECT_FALSE(ret);
    EXPECT_EQ(govResNode->levelToStr.size(), 1);
}

/**
* @tc.name: SocPerfExecutorConfigTest_API_014
* @tc.desc: test socperf_executor_config api
* @tc.type: FUNC
*/
HWTEST_F(SocPerfExecutorConfigTest, SocPerfExecutorConfigTest_API_014, Function | MediumTest | Level0)
{
    SocPerfConfig::GetInstance().resourceNodeInfo_.clear();
    SocPerfConfig::GetInstance().PrintCachedInfo();
    EXPECT_TRUE(SocPerfConfig::GetInstance().resourceNodeInfo_.empty());

    SocPerfConfig::GetInstance().resourceNodeInfo_.clear();
    SocPerfConfig::GetInstance().resourceNodeInfo_[1] = std::make_shared<ResNode>(1, "node1", 0, 2, 0);
    SocPerfConfig::GetInstance().PrintCachedInfo();
    EXPECT_FALSE(SocPerfConfig::GetInstance().resourceNodeInfo_.empty());
}
} // namespace ResourceSchedule
} // namespace OHOS
