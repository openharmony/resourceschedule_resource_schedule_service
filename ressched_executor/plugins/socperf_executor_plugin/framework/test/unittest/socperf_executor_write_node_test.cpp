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



/**
* @tc.name: SocperfExecutorWirteNodeTest_API_001
* @tc.desc: test socperf_executor_config api
* @tc.type: FUNC
*/
HWTEST_F(SocperfExecutorWirteNodeTest, SocperfExecutorWirteNodeTest_API_001, Function | MediumTest | Level0)
{
    SocPerfExecutorWirteNode.GetInstance().InitThreadWraps();
    EXPECT_TRUE(!SocPerfConfig.GetInstance().resourceNodeInfo_.empty());
}

/**
* @tc.name: SocperfExecutorWirteNodeTest_API_002
* @tc.desc: test socperf_executor_config api
* @tc.type: FUNC
*/
HWTEST_F(SocperfExecutorWirteNodeTest, SocperfExecutorWirteNodeTest_API_002, Function | MediumTest | Level0)
{
    SocPerfConfig::GetInstance().resourceNodeInfo_[1] = std::make_shared<ResNode>(1, "node1", 0, 1, 0);
    SocPerfConfig::GetInstance().resourceNodeInfo_[2] = std::make_shared<ResNode>(2, "node2", 0, 1, 0);
    SocPerfConfig::GetInstance().resourceNodeInfo_[3] = std::make_shared<ResNode>(3, "node3", 0, 1, 0);
    std::vector<int32_t> resIdVec = {1, 2, 3};
    std::vector<int64_t> valueVec = {100, 200, 300};
    SocPerfExecutorWirteNode.GetInstance().WriteNodeThreadWraps(resIdVec, valueVec);
    EXPECT_TRUE(!valueVec.empty());
}

/**
* @tc.name: SocperfExecutorWirteNodeTest_API_003
* @tc.desc: test socperf_executor_config api
* @tc.type: FUNC
*/
HWTEST_F(SocperfExecutorWirteNodeTest, SocperfExecutorWirteNodeTest_API_003, Function | MediumTest | Level0)
{
    int32_t resId = 1;
    int64_t currValue = NODE_DEFAULT_VALUE;
    SocPerfExecutorWirteNode::GetInstance().UpdateResIdCurrentValue(resId, currValue);
    currValue = 100;
    SocPerfExecutorWirteNode::GetInstance().UpdateResIdCurrentValue(resId, currValue);
    EXPECT_TRUE(resId == 1);
}

/**
* @tc.name: SocperfExecutorWirteNodeTest_API_004
* @tc.desc: test socperf_executor_config api
* @tc.type: FUNC
*/
HWTEST_F(SocperfExecutorWirteNodeTest, SocperfExecutorWirteNodeTest_API_004, Function | MediumTest | Level0)
{
    int32_t resId = 1;
    int64_t currValue = 10;
    std::shared_ptr<GovResNode> govResNode = std::make_shared<GovResNode>(1, "node1", 0);
    govResNode->persistMode = REPORT_TO_PERFSO - 1;
    govResNode->levelToStr[currValue] = {"level1", "level2"};
    govResNode->paths = {"path1", "path2"};
    govResNode->def = 1;
    SocPerfConfig.GetInstance().resourceNodeInfo_[resId] = govResNode;
    SocPerfExecutorWirteNode::GetInstance().UpdateCurrentValue(resId, currValue);

    resId = 2;
    currValue = 20;
    std::shared_ptr<ResNode> resNode = std::make_shared<ResNode>(2, "node2", 0, 1, 0);
    resNode->path = "path";
    SocPerfConfig.GetInstance().resourceNodeInfo_[resId] = resNode;
    SocPerfExecutorWirteNode::GetInstance().UpdateCurrentValue(resId, currValue);
    EXPECT_NE(resNode, nullptr);
}

/**
* @tc.name: SocperfExecutorWirteNodeTest_API_005
* @tc.desc: test socperf_executor_config api
* @tc.type: FUNC
*/
HWTEST_F(SocperfExecutorWirteNodeTest, SocperfExecutorWirteNodeTest_API_005, Function | MediumTest | Level0)
{
    SocPerfExecutorWirteNode node;
    int fd1 = open("testfile1", O_RDWR | O_CREAT, 0644);
    int fd2 = open("testfile2", O_RDWR | O_CREAT, 0644);
    SocPerfExecutorWirteNode::GetInstance().fdInfo_[0] = fd1;
    SocPerfExecutorWirteNode::GetInstance().fdInfo_[1] = fd2;
    SocPerfExecutorWirteNode::GetInstance().~SocPerfExecutorWirteNode();
    EXPECT_TRUE(SocPerfExecutorWirteNode::GetInstance().fdInfo_.empty());
    EXPECT_EQ(close(fd1), -1);
    EXPECT_EQ(close(fd2), -1);

    SocPerfExecutorWirteNode::GetInstance().~SocPerfExecutorWirteNode();
    EXPECT_TRUE(SocPerfExecutorWirteNode::GetInstance().fdInfo_.empty());
}
} // namespace ResourceSchedule
} // namespace OHOS
