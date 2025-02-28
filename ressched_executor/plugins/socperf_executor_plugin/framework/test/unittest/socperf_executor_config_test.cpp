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

#include "nativetoken_kit.h"
#include "token_setproc.h"
#define private public
#include "socperf_executor_config.h"
#undef private

namespace OHOS {
namespace ResourceSchedule {
using namespace std;
using namespace testing::ext;

namespace {
    constexpr int32_t SYNC_THREAD_NUM = 100;
    constexpr int32_t SYNC_INTERNAL_TIME = 200;
}

class SocperfExecutorConfigTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};


void SocperfExecutorConfigTest::SetUpTestCase(void) {}

void SocperfExecutorConfigTest::TearDownTestCase() {}

void SocperfExecutorConfigTest::SetUp() {}

void SocperfExecutorConfigTest::TearDown() {}

/**
* @tc.name: SocperfExecutorConfigTest_API_001
* @tc.desc: test socperf_executor_config api
* @tc.type: FUNC
*/
HWTEST_F(SocperfExecutorConfigTest, SocperfExecutorConfigTest_API_001, Function | MediumTest | Level0)
{
    SocPerfConfig& config = SocPerfConfig::GetInstance();
    ExPECT_CALL(config, LoadAllConfigXmlFile(SOCPERF_RESOURCE_CONFIG_XML).WillOnce(Return(false)));
    bool ret = config.Init();
    EXPECT_FALSE(ret);

    ExPECT_CALL(config, LoadAllConfigXmlFile(SOCPERF_RESOURCE_CONFIG_XML).WillOnce(Return(true)));
    ret = config.Init();
    EXPECT_TRUE(ret);
}

/**
* @tc.name: SocperfExecutorConfigTest_API_002
* @tc.desc: test socperf_executor_config api
* @tc.type: FUNC
*/
HWTEST_F(SocperfExecutorConfigTest, SocperfExecutorConfigTest_API_002, Function | MediumTest | Level0)
{
    SocPerfConfig::GetInstance().resourceNodeInfo_[1] = std::make_shared<ResourceNode>;
    SocPerfConfig::GetInstance().resourceNodeInfo_[1]->isGov = true;
    ret = SocPerfConfig::GetInstance().IsGovResId(1);
    EXPECT_TRUE(ret);

    SocPerfConfig::GetInstance().resourceNodeInfo_[2] = std::make_shared<ResourceNode>;
    SocPerfConfig::GetInstance().resourceNodeInfo_[2]->isGov = false;
    ret = SocPerfConfig::GetInstance().IsGovResId(1);
    EXPECT_FALSE(ret);

    ret = SocPerfConfig::GetInstance().IsGovResId(3);
    EXPECT_FALSE(ret);
}

/**
* @tc.name: SocperfExecutorConfigTest_API_003
* @tc.desc: test socperf_executor_config api
* @tc.type: FUNC
*/
HWTEST_F(SocperfExecutorConfigTest, SocperfExecutorConfigTest_API_003, Function | MediumTest | Level0)
{
    SocPerfConfig& config = SocPerfConfig::GetInstance();
    int32_t resId = 100;
    EXPECT_FALSE(config.IsValidResId(resId));

    resId = 101;
    config.resourceNodeInfo_[resId] = std::make_shared<ResourceNode>;
    EXPECT_TRUE(config.IsValidResId(resId));
}

/**
* @tc.name: SocperfExecutorConfigTest_API_004
* @tc.desc: test socperf_executor_config api
* @tc.type: FUNC
*/
HWTEST_F(SocperfExecutorConfigTest, SocperfExecutorConfigTest_API_004, Function | MediumTest | Level0)
{
    std::string invalidConfigFile = "invalid_config_file";
    SocPerfConfig& config = SocPerfConfig::GetInstance();
    EXPECT_EQ(config.GetRealConfigPath(invalidConfigFile), "");

    std::string validConfigFile = "valid_config_file";
    EXPECT_NE(config.GetRealConfigPath(invalidConfigFile), "");
}

/**
* @tc.name: SocperfExecutorConfigTest_API_005
* @tc.desc: test socperf_executor_config api
* @tc.type: FUNC
*/
HWTEST_F(SocperfExecutorConfigTest, SocperfExecutorConfigTest_API_005, Function | MediumTest | Level0)
{
    std::string configFile = "test_config_file";
    auto ret = SocPerfConfig::GetInstance().GetAllRealConfigPath(configFile);
    EXPECT_TRUE(ret.empty());

    auto cfgFiles = new std::vector<std::string>{"path1", "path2", "path3"};
    ExPECT_CALL(SocPerfConfig::GetInstance(), GetCfgFiles(configFile.c_str())).WillOnce(Return(cfgFiles));
    ret = SocPerfConfig::GetInstance().GetAllRealConfigPath(configFile);
    EXPECT_EQ(ret, std::vector<std::string>{"path3", "path2", "path1"});

    auto cfgFiles1 = new std::vector<std::string>{"path1", nullptr, "path3"};
    ExPECT_CALL(SocPerfConfig::GetInstance(), GetCfgFiles(configFile.c_str())).WillOnce(Return(cfgFiles));
    ret = SocPerfConfig::GetInstance().GetAllRealConfigPath(configFile);
    EXPECT_EQ(ret, std::vector<std::string>{"path3", "path1"});
}

/**
* @tc.name: SocperfExecutorConfigTest_API_006
* @tc.desc: test socperf_executor_config api
* @tc.type: FUNC
*/
HWTEST_F(SocperfExecutorConfigTest, SocperfExecutorConfigTest_API_006, Function | MediumTest | Level0)
{
    std::string configFile = "";
    bool ret = SocPerfConfig::GetInstance().LoadAllConfigXmlFile(configFile);
    EXPECT_FALSE(ret);

    std::string configFile = "invalid_config_file.xml";
    ret = SocPerfConfig::GetInstance().LoadAllConfigXmlFile(configFile);
    EXPECT_FALSE(ret);

    std::string configFile = "valid_config_file.xml";
    ret = SocPerfConfig::GetInstance().LoadAllConfigXmlFile(configFile);
    EXPECT_TRUE(ret);
}

/**
* @tc.name: SocperfExecutorConfigTest_API_007
* @tc.desc: test socperf_executor_config api
* @tc.type: FUNC
*/
HWTEST_F(SocperfExecutorConfigTest, SocperfExecutorConfigTest_API_007, Function | MediumTest | Level0)
{
    bool ret = SocPerfConfig::GetInstance().LoadConfigXmlFile("");
    EXPECT_FALSE(ret);

    ret = SocPerfConfig::GetInstance().LoadConfigXmlFile("non_existent_file.xml");
    EXPECT_FALSE(ret);

    ret = SocPerfConfig::GetInstance().LoadConfigXmlFile("empty_file.xml");
    EXPECT_FALSE(ret);

    ret = SocPerfConfig::GetInstance().LoadConfigXmlFile("wrong_root_node.xml");
    EXPECT_FALSE(ret);

    ret = SocPerfConfig::GetInstance().LoadConfigXmlFile("valid_file.xml");
    EXPECT_TRUE(ret);
}

/**
* @tc.name: SocperfExecutorConfigTest_API_008
* @tc.desc: test socperf_executor_config api
* @tc.type: FUNC
*/
HWTEST_F(SocperfExecutorConfigTest, SocperfExecutorConfigTest_API_008, Function | MediumTest | Level0)
{
    xmlDoc* doc = nullptr;
    std::string realConfigFile = "test_config_file.xml";
    EXPECT_FALSE(SocPerfConfig::GetInstance().ParseResourceXmlFile(nullptr, realConfigFile, doc));

    xmlNode rootNode;
    rootNode.children = nullptr;
    EXPECT_TRUE(SocPerfConfig::GetInstance().ParseResourceXmlFile(&rootNode, realConfigFile, doc));

    xmlNode child;
    child.name = reinterpret_cast<const xmlChar*>("Resource");
    rootNode.children = &child;
    ExPECT_CALL(SocPerfConfig::GetInstance(), LoadResource(_, _, _)).WillOnce(Return(false));
    EXPECT_FALSE(SocPerfConfig::GetInstance().ParseResourceXmlFile(&rootNode, realConfigFile, doc));

    child.name = reinterpret_cast<const xmlChar*>("GovResource");
    rootNode.children = &child;
    ExPECT_CALL(SocPerfConfig::GetInstance(), LoadResource(_, _, _)).WillOnce(Return(false));
    EXPECT_FALSE(SocPerfConfig::GetInstance().ParseResourceXmlFile(&rootNode, realConfigFile, doc));
}

/**
* @tc.name: SocperfExecutorConfigTest_API_009
* @tc.desc: test socperf_executor_config api
* @tc.type: FUNC
*/
HWTEST_F(SocperfExecutorConfigTest, SocperfExecutorConfigTest_API_009, Function | MediumTest | Level0)
{
    xmlNode child;
    std::string configFile = "test_config_file";
    ON_CALL(SocPerfConfig::GetInstance(), TraversalFreqResource(_, _)).WillByDefault(Return(false));
    EXPECT_FALSE(SocPerfConfig::GetInstance().LoadResource(&child, configFile));

    ON_CALL(SocPerfConfig::GetInstance(), CheckPairResIdValid()).WillByDefault(Return(false));
    EXPECT_FALSE(SocPerfConfig::GetInstance().LoadResource(&child, configFile));

    ON_CALL(SocPerfConfig::GetInstance(), CheckDefValid()).WillByDefault(Return(false));
    EXPECT_FALSE(SocPerfConfig::GetInstance().LoadResource(&child, configFile));

    ON_CALL(SocPerfConfig::GetInstance(), TraversalFreqResource(_, _)).WillByDefault(Return(true));
    ON_CALL(SocPerfConfig::GetInstance(), CheckPairResIdValid()).WillByDefault(Return(true));
    ON_CALL(SocPerfConfig::GetInstance(), CheckDefValid()).WillByDefault(Return(true));
    EXPECT_TRUE(SocPerfConfig::GetInstance().LoadResource(&child, configFile));
}

/**
* @tc.name: SocperfExecutorConfigTest_API_010
* @tc.desc: test socperf_executor_config api
* @tc.type: FUNC
*/
HWTEST_F(SocperfExecutorConfigTest, SocperfExecutorConfigTest_API_010, Function | MediumTest | Level0)
{
    xmlNode grandson;
    std::string configFile = "test_config.xml";
    ExPECT_CALL(grandson, xmlGetProp(_, _)).WillRepeatedly(Return(nullptr));
    bool ret = SocPerfConfig::GetInstance().TraversalFreqResource(&grandson, configFile);
    EXPECT_FALSE(ret);

    ExPECT_CALL(grandson, xmlGetProp(_, _)).WillRepeatedly(Return("1"));
    SocPerfConfig::GetInstance().resourceNodeInfo_[1] = std::make_shared<ResourceNode>();
    ret = SocPerfConfig::GetInstance()..TraversalFreqResource(&grandson, configFile);
    EXPECT_TRUE(ret);

    ExPECT_CALL(grandson, xmlGetProp(_, _)).WillRepeatedly(Return("1"));
    ExPECT_CALL(SocPerfConfig::GetInstance(), LoadFreqResourceContent(_, _, _, _)).WillOnce(Return(false));
    ret = SocPerfConfig::GetInstance().TraversalFreqResource(&grandson, configFile);
    EXPECT_FALSE(ret);
}

/**
* @tc.name: SocperfExecutorConfigTest_API_011
* @tc.desc: test socperf_executor_config api
* @tc.type: FUNC
*/
HWTEST_F(SocperfExecutorConfigTest, SocperfExecutorConfigTest_API_011, Function | MediumTest | Level0)
{
    xmlNode xmlNode;
    const char* propName = "testProp";
    char propValue[] = "123";
    xmlNode.properties = propValue;
    int32_t ret = SocPerfConfig::GetInstance().GetXmlIntProp(&xmlNode, propName);
    EXPECT_EQ(ret, 123);

    char propValue1[] = "abc";
    xmlNode.properties = propValue1;
    int32_t ret = SocPerfConfig::GetInstance().GetXmlIntProp(&xmlNode, propName);
    EXPECT_EQ(ret, -1);

    xmlNode.properties = nullptr;
    int32_t ret = SocPerfConfig::GetInstance().GetXmlIntProp(&xmlNode, propName);
    EXPECT_EQ(ret, -1);
}

/**
* @tc.name: SocperfExecutorConfigTest_API_012
* @tc.desc: test socperf_executor_config api
* @tc.type: FUNC
*/
HWTEST_F(SocperfExecutorConfigTest, SocperfExecutorConfigTest_API_012, Function | MediumTest | Level0)
{
    int32_t resId = 100;
    int32_t expected = RES_ID_NUMS_PER_TYPE;
    int32_t actual = SocPerfConfig::GetInstance().GetResIdNumsPerType(resId);
    EXPECT_EQ(expected, actual);

    SocPerfConfig::GetInstance().resourceNodeInfo_[resId] = std::make_shared<ResourceNode>();
    SocPerfConfig::GetInstance().resourceNodeInfo_[resId]->persistMode = REPORT_TO_PERFSO;
    expected = RES_ID_NUMS_PER_TYPE_EXT;
    actual = SocPerfConfig::GetInstance().GetResIdNumsPerType(resId);
    EXPECT_EQ(expected, actual);
}

/**
* @tc.name: SocperfExecutorConfigTest_API_013
* @tc.desc: test socperf_executor_config api
* @tc.type: FUNC
*/
HWTEST_F(SocperfExecutorConfigTest, SocperfExecutorConfigTest_API_013, Function | MediumTest | Level0)
{
    bool ret = SocPerfConfig::GetInstance().CheckResourceTag(
        "invalid", "name", "pair", "mode", "persistMode", "configFile");
    EXPECT_FALSE(ret);

    ret = SocPerfConfig::GetInstance().CheckResourceTag(
        "1", nullptr, "pair", "mode", "persistMode", "configFile");
    EXPECT_FALSE(ret);

    ret = SocPerfConfig::GetInstance().CheckResourceTag(
        "1", "name", "invalid", "mode", "persistMode", "configFile");
    EXPECT_FALSE(ret);

    ret = SocPerfConfig::GetInstance().CheckResourceTag(
        "1", "name", "pair", "invalid", "persistMode", "configFile");
    EXPECT_FALSE(ret);

    ret = SocPerfConfig::GetInstance().CheckResourceTag(
        "1", "name", "pair",  "mode", "persistMode", "configFile");
    EXPECT_TRUE(ret);
}

/**
* @tc.name: SocperfExecutorConfigTest_API_013
* @tc.desc: test socperf_executor_config api
* @tc.type: FUNC
*/
HWTEST_F(SocperfExecutorConfigTest, SocperfExecutorConfigTest_API_013, Function | MediumTest | Level0)
{
    const char* persistMode = "1";
    std::string configFile = "test_config_file";
    EXPECT_TRUE(SocPerfConfig::GetInstance().CheckResourcePersistMode(persistMode, configFile));

    persistMode = "invalid_mode";
    EXPECT_FALSE(SocPerfConfig::GetInstance().CheckResourcePersistMode(persistMode, configFile));

    persistMode = nullptr;
    EXPECT_FALSE(SocPerfConfig::GetInstance().CheckResourcePersistMode(persistMode, configFile));
}

/**
* @tc.name: SocperfExecutorConfigTest_API_014
* @tc.desc: test socperf_executor_config api
* @tc.type: FUNC
*/
HWTEST_F(SocperfExecutorConfigTest, SocperfExecutorConfigTest_API_014, Function | MediumTest | Level0)
{
    const char* def1 = nullptr;
    const char* def2 = "123";
    const char* path1 = nullptr;
    const char* path2 = "test_path";
    std::string configFile = "test_config_file";
    bool ret = SocPerfConfig::GetInstance().CheckResourceTag(1, def1, path2, configFile);
    EXPECT_FALSE(ret);

    ret = SocPerfConfig::GetInstance().CheckResourceTag(1, def2, path1, configFile);
    EXPECT_FALSE(ret);

    ret = SocPerfConfig::GetInstance().CheckResourceTag(1, def2, path2, configFile);
    EXPECT_TRUE(ret);
}

/**
* @tc.name: SocperfExecutorConfigTest_API_015
* @tc.desc: test socperf_executor_config api
* @tc.type: FUNC
*/
HWTEST_F(SocperfExecutorConfigTest, SocperfExecutorConfigTest_API_015, Function | MediumTest | Level0)
{
    auto resNode = std::make_shared<ResNode>(1, "test", 1, 1, 1);
    const char* node = "123 456 789";
    EXPECT_TRUE(SocPerfConfig::GetInstance().LoadResourceAvailable(resNode, node));
    EXPECT_EQ(resNode->available.size(), 3);

    const char* node1 = "123 abc 789";
    EXPECT_TRUE(SocPerfConfig::GetInstance().LoadResourceAvailable(resNode, node1));

    const char* node2 = "";
    EXPECT_TRUE(SocPerfConfig::GetInstance().LoadResourceAvailable(resNode, node2));
}

/**
* @tc.name: SocperfExecutorConfigTest_API_016
* @tc.desc: test socperf_executor_config api
* @tc.type: FUNC
*/
HWTEST_F(SocperfExecutorConfigTest, SocperfExecutorConfigTest_API_016, Function | MediumTest | Level0)
{
    SocPerfConfig::GetInstance().resourceNodeInfo_[1] = std::make_shared<ResourceNode>();
    SocPerfConfig::GetInstance().resourceNodeInfo_[1]->pair = 2;
    EXPECT_FALSE(SocPerfConfig::GetInstance().CheckPairResIdValid());

    SocPerfConfig::GetInstance().resourceNodeInfo_[2] = std::make_shared<ResourceNode>();
    EXPECT_TRUE(SocPerfConfig::GetInstance().CheckPairResIdValid());
}

/**
* @tc.name: SocperfExecutorConfigTest_API_017
* @tc.desc: test socperf_executor_config api
* @tc.type: FUNC
*/
HWTEST_F(SocperfExecutorConfigTest, SocperfExecutorConfigTest_API_017, Function | MediumTest | Level0)
{
    SocPerfConfig::GetInstance().resourceNodeInfo_[1] = std::make_shared<ResourceNode>();
    SocPerfConfig::GetInstance().resourceNodeInfo_[1]->isGov = true;
    EXPECT_TRUE(SocPerfConfig::GetInstance().CheckPairResIdValid());
}

/**
* @tc.name: SocperfExecutorConfigTest_API_018
* @tc.desc: test socperf_executor_config api
* @tc.type: FUNC
*/
HWTEST_F(SocperfExecutorConfigTest, SocperfExecutorConfigTest_API_018, Function | MediumTest | Level0)
{
    std::shared_ptr<ResourceNode> node = std::make_shared<ResourceNode>();
    node->def = 1;
    node->available.insert(2);
    SocPerfConfig::GetInstance().resourceNodeInfo_.insert(std::make_pair(1, node));
    EXPECT_FALSE(SocPerfConfig::GetInstance().CheckDefValid());

    std::shared_ptr<ResourceNode> node1 = std::make_shared<ResourceNode>();
    node1->def = 1;
    node1->available.insert(1);
    SocPerfConfig::GetInstance().resourceNodeInfo_.insert(std::make_pair(1, node1));
    EXPECT_TRUE(SocPerfConfig::GetInstance().CheckDefValid());

    std::shared_ptr<ResourceNode> node2 = std::make_shared<ResourceNode>();
    node2->def = 1;
    SocPerfConfig::GetInstance().resourceNodeInfo_.insert(std::make_pair(1, node2));
    EXPECT_TRUE(SocPerfConfig::GetInstance().CheckDefValid());
}

/**
* @tc.name: SocperfExecutorConfigTest_API_019
* @tc.desc: test socperf_executor_config api
* @tc.type: FUNC
*/
HWTEST_F(SocperfExecutorConfigTest, SocperfExecutorConfigTest_API_019, Function | MediumTest | Level0)
{
    EXPECT_TRUE(SocPerfConfig::GetInstance().CheckGovResourceTag("1", "testName", "1", "testConfigFile"));
    EXPECT_FALSE(SocPerfConfig::GetInstance().CheckGovResourceTag("invalid", "testName", "1", "testConfigFile"));
    EXPECT_FALSE(SocPerfConfig::GetInstance().CheckGovResourceTag("1", nullptr, "1", "testConfigFile"));
    EXPECT_FALSE(SocPerfConfig::GetInstance().CheckGovResourceTag("1", "testName", "invalid", "testConfigFile"));
}

/**
* @tc.name: SocperfExecutorConfigTest_API_019
* @tc.desc: test socperf_executor_config api
* @tc.type: FUNC
*/
HWTEST_F(SocperfExecutorConfigTest, SocperfExecutorConfigTest_API_019, Function | MediumTest | Level0)
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
    EXPECT_EQ(govResNode->levelToStr.size(), 0);
}

/**
* @tc.name: SocperfExecutorConfigTest_API_020
* @tc.desc: test socperf_executor_config api
* @tc.type: FUNC
*/
HWTEST_F(SocperfExecutorConfigTest, SocperfExecutorConfigTest_API_020, Function | MediumTest | Level0)
{
    SocPerfConfig::GetInstance().resourceNodeInfo_.clear();
    SocPerfConfig::GetInstance().PrintCachedInfo();
    EXPECT_TRUE(SocPerfConfig::GetInstance().resourceNodeInfo_.empty());

    SocPerfConfig::GetInstance().resourceNodeInfo_.clear();
    SocPerfConfig::GetInstance().resourceNodeInfo_[1] = std::make_shared<ResourceNode>();
    SocPerfConfig::GetInstance().PrintCachedInfo();
    EXPECT_FALSE(SocPerfConfig::GetInstance().resourceNodeInfo_.empty());
}
} // namespace ResourceSchedule
} // namespace OHOS
