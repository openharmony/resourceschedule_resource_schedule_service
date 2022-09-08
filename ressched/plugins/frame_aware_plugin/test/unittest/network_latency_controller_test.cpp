/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include <iostream>
#include <memory>
#include <string>

#include <gtest/gtest.h>

#include <inetwork_latency_switcher.h>
#include <network_latency_controller.h>

namespace OHOS {
namespace ResourceSchedule {
struct MockSwitcher : INetworkLatencySwitcher {
    struct Counter {
        int onCount = 0;
        int offCount = 0;
    };

    explicit MockSwitcher(std::shared_ptr<Counter> &counter)
        : counter(counter)
    { }

    void LowLatencyOn() override
    {
        ++counter->onCount;
    }

    void LowLatencyOff() override
    {
        ++counter->offCount;
    }

private:
    std::shared_ptr<Counter> counter;
};

class NetworkLatencyControllerTest : public testing::Test {
public:
    void SetUp()
    {
        counter = std::make_shared<MockSwitcher::Counter>();
        switcher = std::make_unique<MockSwitcher>(counter);
        ctrl.Init(std::move(switcher));
    }

    void TearDown()
    { }

protected:
    std::shared_ptr<MockSwitcher::Counter> counter;
    std::unique_ptr<MockSwitcher> switcher;
    NetworkLatencyController ctrl;
};

/**
 * @tc.name: singleUser_001
 * @tc.desc: test latency switching with a single user
 * @tc.type: FUNC
 * @tc.require: SR000H029E SR000GVT7U
 */
HWTEST_F(NetworkLatencyControllerTest, singleUser_001, testing::ext::TestSize.Level1)
{
    const std::string identity("test.application.1");

    // enable low latency
    ctrl.HandleRequest(NetworkLatencyController::NETWORK_LATENCY_REQUEST_LOW, identity);
    EXPECT_EQ(counter->onCount, 1);
    EXPECT_EQ(counter->offCount, 0);

    // disable low latency
    ctrl.HandleRequest(NetworkLatencyController::NETWORK_LATENCY_REQUEST_NORMAL, identity);
    EXPECT_EQ(counter->onCount, 1);
    EXPECT_EQ(counter->offCount, 1);
}

/**
 * @tc.name: multiUser_002
 * @tc.desc: test latency switching with multiple users
 * @tc.type: FUNC
 * @tc.require: SR000H029E SR000GVT7U
 */
HWTEST_F(NetworkLatencyControllerTest, multiUser_002, testing::ext::TestSize.Level1)
{
    const std::string identity1("test.application.1");
    const std::string identity2("test.application.2");

    // enable low latency from the first user
    ctrl.HandleRequest(NetworkLatencyController::NETWORK_LATENCY_REQUEST_LOW, identity1);
    EXPECT_EQ(counter->onCount, 1);
    EXPECT_EQ(counter->offCount, 0);

    // enable low latency from the second user
    ctrl.HandleRequest(NetworkLatencyController::NETWORK_LATENCY_REQUEST_LOW, identity2);
    EXPECT_EQ(counter->onCount, 1); // already activated
    EXPECT_EQ(counter->offCount, 0);

    // try reset to normal latency from the first user
    ctrl.HandleRequest(NetworkLatencyController::NETWORK_LATENCY_REQUEST_NORMAL, identity1);
    EXPECT_EQ(counter->onCount, 1);
    EXPECT_EQ(counter->offCount, 0); // there is a second user alive

    // finally reset from the second user
    ctrl.HandleRequest(NetworkLatencyController::NETWORK_LATENCY_REQUEST_NORMAL, identity2);
    EXPECT_EQ(counter->onCount, 1);
    EXPECT_EQ(counter->offCount, 1); // finally disable
}

/**
 * @tc.name: errorEmptyIdentity_003
 * @tc.desc: test that empty identities are not accepted
 * @tc.type: FUNC
 * @tc.require: SR000H029E SR000GVT7U
 */
HWTEST_F(NetworkLatencyControllerTest, errorEmptyIdentity_003, testing::ext::TestSize.Level1)
{
    const std::string empty;

    // cannot create latency request with empty identity
    ctrl.HandleRequest(NetworkLatencyController::NETWORK_LATENCY_REQUEST_LOW, empty);
    EXPECT_EQ(counter->onCount, 0);
    EXPECT_EQ(counter->offCount, 0);
}

/**
 * @tc.name: errorDuplicateRequests_004
 * @tc.desc: test that duplicate requests are ignored
 * @tc.type: FUNC
 * @tc.require: SR000H029E SR000GVT7U
 */
HWTEST_F(NetworkLatencyControllerTest, errorDuplicateRequests_004, testing::ext::TestSize.Level1)
{
    const std::string identity("my.test.application");

    // send initial latency request
    ctrl.HandleRequest(NetworkLatencyController::NETWORK_LATENCY_REQUEST_LOW, identity);
    EXPECT_EQ(counter->onCount, 1);
    EXPECT_EQ(counter->offCount, 0);

    // create another latency request shouldn't change anything
    ctrl.HandleRequest(NetworkLatencyController::NETWORK_LATENCY_REQUEST_LOW, identity);
    EXPECT_EQ(counter->onCount, 1);
    EXPECT_EQ(counter->offCount, 0);
}

/**
 * @tc.name: errorCancelNonExistentRequest_005
 * @tc.desc: test that it is impossible to cancel non-existent request
 * @tc.type: FUNC
 * @tc.require: SR000H029E SR000GVT7U
 */
HWTEST_F(NetworkLatencyControllerTest, errorCancelNonExistentRequest_005, testing::ext::TestSize.Level1)
{
    const std::string identity("my.test.application");

    // cancelling a non-existing latency request should not do anything
    ctrl.HandleRequest(NetworkLatencyController::NETWORK_LATENCY_REQUEST_NORMAL, identity);
    EXPECT_EQ(counter->onCount, 0);
    EXPECT_EQ(counter->offCount, 0); // nothing changed
}

/**
 * @tc.name: errorCancelForeignRequest_006
 * @tc.desc: test that it is impossible to cancel another user's request
 * @tc.type: FUNC
 * @tc.require: SR000H029E SR000GVT7U
 */
HWTEST_F(NetworkLatencyControllerTest, errorCancelForeignRequest_006, testing::ext::TestSize.Level1)
{
    const std::string identity1("my.test.application1");
    const std::string identity2("my.test.application2");

    // register a latency request from the first user
    ctrl.HandleRequest(NetworkLatencyController::NETWORK_LATENCY_REQUEST_LOW, identity1);
    EXPECT_EQ(counter->onCount, 1);
    EXPECT_EQ(counter->offCount, 0);

    // cannot cancel the request using another identity
    ctrl.HandleRequest(NetworkLatencyController::NETWORK_LATENCY_REQUEST_NORMAL, identity2);
    EXPECT_EQ(counter->onCount, 1); // nothing changed
    EXPECT_EQ(counter->offCount, 0);
}

/**
 * @tc.name: errorInvalidLatencyValue_007
 * @tc.desc: test that invalid request values are ignored
 * @tc.type: FUNC
 * @tc.require: SR000H029E SR000GVT7U
 */
HWTEST_F(NetworkLatencyControllerTest, errorInvalidLatencyValue_007, testing::ext::TestSize.Level1)
{
    const std::string identity("my.test.application");
    long long badValue = 9999;

    // try to register non existing request value
    ctrl.HandleRequest(badValue, identity);
    EXPECT_EQ(counter->onCount, 0); // should not activate
    EXPECT_EQ(counter->offCount, 0);
}
} // namespace ResourceSchedule
} // namespace OHOS
