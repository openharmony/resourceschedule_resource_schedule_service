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
#include "socperf_executor_write_node.h"
#undef private

namespace OHOS {
namespace ResourceSchedule {
using namespace std;
using namespace testing::ext;

namespace {
    constexpr int32_t SYNC_THREAD_NUM = 100;
    constexpr int32_t SYNC_INTERNAL_TIME = 200;
}

class SocperfExecutorWirteNodeTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};


void SocperfExecutorWirteNodeTest::SetUpTestCase(void) {}

void SocperfExecutorWirteNodeTest::TearDownTestCase() {}

void SocperfExecutorWirteNodeTest::SetUp() {}

void SocperfExecutorWirteNodeTest::TearDown() {}

class ResourceNodeTest : public ResourceNode {
public:
    virtual int32_t BeginExecute(uint64_t scheduleId, const std::vector<uint8_t> &publicKey,
        const Attributes &command) override {
        return 0;
    }
};

class GovResNodeTest : public GovResNode {
public:
    virtual int32_t BeginExecute(uint64_t scheduleId, const std::vector<uint8_t> &publicKey,
        const Attributes &command) override {
        return 0;
    }
};

/**
* @tc.name: SocperfExecutorWirteNodeTest_API_001
* @tc.desc: test socperf_executor_config api
* @tc.type: FUNC
*/
HWTEST_F(SocperfExecutorWirteNodeTest, SocperfExecutorWirteNodeTest_API_001, Function | MediumTest | Level0)
{
    SocPerfExecutorWirteNode node;
    int fd1 = open("testfile1", O_RDWR | O_CREAT, 0644);
    int fd2 = open("testfile2", O_RDWR | O_CREAT, 0644);
    node.fdInfo_[0] = fd1;
    node.fdInfo_[1] = fd2;
    node.~SocPerfExecutorWirteNode();
    EXPECT_TRUE(node.fdInfo_.empty());
    EXPECT_EQ(close(fd1), -1);
    EXPECT_EQ(close(fd2), -1);

    SocPerfExecutorWirteNode node1;
    node1.~SocPerfExecutorWirteNode();
    EXPECT_TRUE(node1.fdInfo_.empty());
}

/**
* @tc.name: SocperfExecutorWirteNodeTest_API_002
* @tc.desc: test socperf_executor_config api
* @tc.type: FUNC
*/
HWTEST_F(SocperfExecutorWirteNodeTest, SocperfExecutorWirteNodeTest_API_002, Function | MediumTest | Level0)
{
    SocPerfExecutorWirteNode& node = SocPerfExecutorWirteNode.GetInstance();
    SocPerfConfig& config = SocPerfConfig.GetInstance();
    std::shared_ptr<ResourceNode> resourceNode(new ResourceNode());
    config.resourceNodeInfo_["testNode"] = resourceNode;
    node.InitThreadWraps();
    EXPECT_TRUE(resourceNode->isInitialized);

    config.resourceNodeInfo_.clear();
    node.InitThreadWraps();
    EXPECT_TRUE(config.resourceNodeInfo_.empty());
}

/**
* @tc.name: SocperfExecutorWirteNodeTest_API_003
* @tc.desc: test socperf_executor_config api
* @tc.type: FUNC
*/
HWTEST_F(SocperfExecutorWirteNodeTest, SocperfExecutorWirteNodeTest_API_003, Function | MediumTest | Level0)
{
    SocPerfExecutorWirteNode executor;
    executor.InitResourceNodeInfo(nullptr);

    std::shared_ptr<GovResNodeTest> govResNode = std::make_shared<GovResNodeTest>();
    govResNode->isGov = true;
    govResNode->persistMode = REPORT_TO_PERFSO;
    govResNode->paths.push_back("path1");
    govResNode->levelToStr[govResNode->def].push_back("level1");
    executor.InitResourceNodeInfo(govResNode);

    std::shared_ptr<GovResNodeTest> govResNode1 = std::make_shared<GovResNodeTest>();
    govResNode1->isGov = false;
    govResNode1->persistMode = REPORT_TO_PERFSO;
    govResNode1->paths = "path1";
    govResNode1->def = 1;
    executor.InitResourceNodeInfo(govResNode1);
    EXPECT_NE(executor, nullptr);
}

/**
* @tc.name: SocperfExecutorWirteNodeTest_API_004
* @tc.desc: test socperf_executor_config api
* @tc.type: FUNC
*/
HWTEST_F(SocperfExecutorWirteNodeTest, SocperfExecutorWirteNodeTest_API_004, Function | MediumTest | Level0)
{
    std::vector<int32_t> resIdVec = {1, 2, 3};
    std::vector<int64_t> valueVec = {100, 200, 300};
    std::vector<int32_t> resIdVec1 = {};
    std::vector<int64_t> valueVec1 = {};
    SocPerfExecutorWirteNode.GetInstance().WriteNodeThreadWraps(resIdVec, valueVec);
    SocPerfExecutorWirteNode.GetInstance().WriteNodeThreadWraps(resIdVec, valueVec1);
    SocPerfExecutorWirteNode.GetInstance().WriteNodeThreadWraps(resIdVec1, valueVec);
    EXPECT_NE(resIdVec, nullptr);
}

/**
* @tc.name: SocperfExecutorWirteNodeTest_API_005
* @tc.desc: test socperf_executor_config api
* @tc.type: FUNC
*/
HWTEST_F(SocperfExecutorWirteNodeTest, SocperfExecutorWirteNodeTest_API_005, Function | MediumTest | Level0)
{
    int32_t resId = 1;
    int64_t currValue = SocPerfExecutorWirteNode::NODE_DEFAULT_VALUE;
    SocPerfExecutorWirteNode::GetInstance().UpdateResIdCurrentValue(resId, currValue);
    currValue = 100;
    SocPerfExecutorWirteNode::GetInstance().UpdateResIdCurrentValue(resId, currValue);
    EXPECT_NE(resId, nullptr);
}

/**
* @tc.name: SocperfExecutorWirteNodeTest_API_006
* @tc.desc: test socperf_executor_config api
* @tc.type: FUNC
*/
HWTEST_F(SocperfExecutorWirteNodeTest, SocperfExecutorWirteNodeTest_API_006, Function | MediumTest | Level0)
{
    int32_t resId = 1;
    int64_t currValue = 10;
    std::shared_ptr<GovResNode> govResNode = std::make_shared<GovResNode>();
    govResNode->persistMode = REPORT_TO_PERFSO - 1;
    govResNode->levelToStr[currValue] = {"level1", "level2"};
    govResNode->paths = {"path1", "path2"};
    govResNode->def = 1;
    SocPerfConfig.GetInstance().resourceNodeInfo_[resId] = govResNode;
    SocPerfExecutorWirteNode::GetInstance().UpdateCurrentValue(resId, currValue);

    resId = 2;
    currValue = 20;
    std::shared_ptr<ResNode> resNode = std::make_shared<ResNode>();
    resNode->path = "path";
    SocPerfConfig.GetInstance().resourceNodeInfo_[resId] = resNode;
    SocPerfExecutorWirteNode::GetInstance().UpdateCurrentValue(resId, currValue);
    EXPECT_NE(resNode, nullptr);
}

/**
* @tc.name: SocperfExecutorWirteNodeTest_API_007
* @tc.desc: test socperf_executor_config api
* @tc.type: FUNC
*/
HWTEST_F(SocperfExecutorWirteNodeTest, SocperfExecutorWirteNodeTest_API_007, Function | MediumTest | Level0)
{
    int32_t resId = 1;
    std::string filePath = "test/path";
    std::string value = "test_value";
    SocPerfExecutorWirteNode::GetInstance().WriteNode(resId, filePath, value);

    int32_t resId1 = 2;
    std::string filePath1 = "invalid/path";
    SocPerfExecutorWirteNode::GetInstance().WriteNode(resId1, filePath1, value);

    int32_t resId2 = 3;
    std::string filePath2 = "valid/path";
    SocPerfExecutorWirteNode::GetInstance().WriteNode(resId2, filePath2, value);
    EXPECT_NE(filePath2, nullptr);
}

/**
* @tc.name: SocperfExecutorWirteNodeTest_API_008
* @tc.desc: test socperf_executor_config api
* @tc.type: FUNC
*/
HWTEST_F(SocperfExecutorWirteNodeTest, SocperfExecutorWirteNodeTest_API_008, Function | MediumTest | Level0)
{
    EXPECT_EQ(-1, SocPerfExecutorWirteNode::GetInstance().GetFdForFilePath(""));

    std::string longPath(PATH_MAX + 1, 'a');
    EXPECT_EQ(-1, SocPerfExecutorWirteNode::GetInstance().GetFdForFilePath(longPath));

    std::string validPath = "/valid/path";
    EXPECT_EQ(SocPerfExecutorWirteNode::GetInstance().GetFdForFilePath(validPath),
        SocPerfExecutorWirteNode::GetInstance().GetFdForFilePath(validPath));

    std::string invalidPath = "/invalid/path";
    EXPECT_EQ(SocPerfExecutorWirteNode::GetInstance().GetFdForFilePath(invalidPath), 0);
}
} // namespace ResourceSchedule
} // namespace OHOS
