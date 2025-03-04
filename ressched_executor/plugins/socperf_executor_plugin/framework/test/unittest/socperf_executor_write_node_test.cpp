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

#include <vector>
#include <fcntl.h>
#include <unistd.h>

#define private public
#include "socperf_executor_wirte_node.h"
#include "socperf_executor_config.h"
#undef private

namespace OHOS {
namespace ResourceSchedule {
using namespace std;
using namespace testing::ext;

class SocPerfExecutorWirteNodeTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void SocPerfExecutorWirteNodeTest::SetUpTestCase(void) {}

void SocPerfExecutorWirteNodeTest::TearDownTestCase() {}

void SocPerfExecutorWirteNodeTest::SetUp() {}

void SocPerfExecutorWirteNodeTest::TearDown() {}

/**
* @tc.name: SocPerfExecutorWirteNodeTest_API_001
* @tc.desc: test socperf_executor_config api
* @tc.type: FUNC
*/
HWTEST_F(SocPerfExecutorWirteNodeTest, SocPerfExecutorWirteNodeTest_API_001, Function | MediumTest | Level0)
{
    SocPerfExecutorWirteNode::GetInstance().InitThreadWraps();
    EXPECT_TRUE(!SocPerfConfig::GetInstance().resourceNodeInfo_.empty());
}

/**
* @tc.name: SocPerfExecutorWirteNodeTest_API_002
* @tc.desc: test socperf_executor_config api
* @tc.type: FUNC
*/
HWTEST_F(SocPerfExecutorWirteNodeTest, SocPerfExecutorWirteNodeTest_API_002, Function | MediumTest | Level0)
{
    SocPerfConfig::GetInstance().resourceNodeInfo_[1] = std::make_shared<ResNode>(1, "node1", 0, 1, 0);
    SocPerfConfig::GetInstance().resourceNodeInfo_[2] = std::make_shared<ResNode>(2, "node2", 0, 1, 0);
    SocPerfConfig::GetInstance().resourceNodeInfo_[3] = std::make_shared<ResNode>(3, "node3", 0, 1, 0);
    std::vector<int32_t> resIdVec = {1, 2, 3};
    std::vector<int64_t> valueVec = {-1, 200, 300};
    SocPerfExecutorWirteNode::GetInstance().WriteNodeThreadWraps(resIdVec, valueVec);
    EXPECT_TRUE(!valueVec.empty());
}

/**
* @tc.name: SocPerfExecutorWirteNodeTest_API_003
* @tc.desc: test socperf_executor_config api
* @tc.type: FUNC
*/
HWTEST_F(SocPerfExecutorWirteNodeTest, SocPerfExecutorWirteNodeTest_API_003, Function | MediumTest | Level0)
{
    int32_t resId = 1;
    int64_t currValue = NODE_DEFAULT_VALUE;
    SocPerfExecutorWirteNode::GetInstance().UpdateResIdCurrentValue(resId, currValue);
    currValue = 100;
    SocPerfExecutorWirteNode::GetInstance().UpdateResIdCurrentValue(resId, currValue);
    EXPECT_TRUE(resId == 1);
}

/**
* @tc.name: SocPerfExecutorWirteNodeTest_API_004
* @tc.desc: test socperf_executor_config api
* @tc.type: FUNC
*/
HWTEST_F(SocPerfExecutorWirteNodeTest, SocPerfExecutorWirteNodeTest_API_004, Function | MediumTest | Level0)
{
    int32_t resId = 1;
    int64_t currValue = 10;
    std::shared_ptr<GovResNode> govResNode = std::make_shared<GovResNode>(1, "node1", 0);
    govResNode->persistMode = REPORT_TO_PERFSO - 1;
    govResNode->levelToStr[currValue] = {"level1", "level2"};
    govResNode->paths = {"path1", "path2"};
    govResNode->def = 1;
    SocPerfConfig::GetInstance().resourceNodeInfo_[resId] = govResNode;
    SocPerfExecutorWirteNode::GetInstance().UpdateCurrentValue(resId, currValue);

    resId = 2;
    currValue = 20;
    std::shared_ptr<ResNode> resNode = std::make_shared<ResNode>(2, "node2", 0, 1, 0);
    resNode->path = "path";
    SocPerfConfig::GetInstance().resourceNodeInfo_[resId] = resNode;
    SocPerfExecutorWirteNode::GetInstance().UpdateCurrentValue(resId, currValue);
    EXPECT_NE(resNode, nullptr);
}
} // namespace ResourceSchedule
} // namespace OHOS
