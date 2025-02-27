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
    
}
} // namespace ResourceSchedule
} // namespace OHOS
