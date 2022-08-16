#include <iostream>
#include <memory>
#include <string>

#include <gtest/gtest.h>

#include <inetwork_latency_switcher.h>
#include <network_latency_controller.h>

struct SwitchCounter {
    int onCount = 0;
    int offCount = 0;
};

struct MockSwitcher : OHOS::ResourceSchedule::INetworkLatencySwitcher {
    MockSwitcher(std::shared_ptr<SwitchCounter> &counter)
        : counter(counter)
    { }

    virtual void LowLatencyOn() override {
        ++counter->onCount;
    }

    virtual void LowLatencyOff() override {
        ++counter->offCount;
    }

private:
    std::shared_ptr<SwitchCounter> counter;
};

class NetworkLatencyControllerTest : public testing::Test {
public:
    void SetUp() {
        counter = std::make_shared<SwitchCounter>();
        switcher = std::make_unique<MockSwitcher>(counter);
        ctrl.Init(std::move(switcher));
    }

    void TearDown() { }

protected:
    std::shared_ptr<SwitchCounter> counter;
    std::unique_ptr<MockSwitcher> switcher;
    OHOS::ResourceSchedule::NetworkLatencyController ctrl;

    static const long long LOW_LATENCY_VALUE = 0;
    static const long long DEFAULT_LATENCY_VALUE = 1;
};

HWTEST_F(NetworkLatencyControllerTest, singleUser_001, testing::ext::TestSize.Level1)
{
    const std::string identity("test.application.1");

    // enable low latency
    ctrl.HandleRequest(LOW_LATENCY_VALUE, identity);
    EXPECT_EQ(counter->onCount, 1);
    EXPECT_EQ(counter->offCount, 0);

    // disable low latency
    ctrl.HandleRequest(DEFAULT_LATENCY_VALUE, identity);
    EXPECT_EQ(counter->onCount, 1);
    EXPECT_EQ(counter->offCount, 1);
}

HWTEST_F(NetworkLatencyControllerTest, multiUser_002, testing::ext::TestSize.Level1)
{
    const std::string identity1("test.application.1");
    const std::string identity2("test.application.2");

    // enable low latency from the first user
    ctrl.HandleRequest(LOW_LATENCY_VALUE, identity1);
    EXPECT_EQ(counter->onCount, 1);
    EXPECT_EQ(counter->offCount, 0);

    // enable low latency from the second user
    ctrl.HandleRequest(LOW_LATENCY_VALUE, identity2);
    EXPECT_EQ(counter->onCount, 1); // already activated
    EXPECT_EQ(counter->offCount, 0);

    // try reset to normal latency from the first user
    ctrl.HandleRequest(DEFAULT_LATENCY_VALUE, identity1);
    EXPECT_EQ(counter->onCount, 1);
    EXPECT_EQ(counter->offCount, 0); // there is a second user alive

    ctrl.HandleRequest(DEFAULT_LATENCY_VALUE, identity2);
    EXPECT_EQ(counter->onCount, 1);
    EXPECT_EQ(counter->offCount, 1); // finally disable
}

HWTEST_F(NetworkLatencyControllerTest, errorEmptyIdentity_003, testing::ext::TestSize.Level1)
{
    const std::string empty;

    // cannot create latency request with empty identity
    ctrl.HandleRequest(LOW_LATENCY_VALUE, empty);
    EXPECT_EQ(counter->onCount, 0);
    EXPECT_EQ(counter->offCount, 0);
}

HWTEST_F(NetworkLatencyControllerTest, errorDuplicateRequests_004, testing::ext::TestSize.Level1)
{
    const std::string identity("my.test.application");

    // send initial latency request
    ctrl.HandleRequest(LOW_LATENCY_VALUE, identity);
    EXPECT_EQ(counter->onCount, 1);
    EXPECT_EQ(counter->offCount, 0);

    // create another latency request shouldn't change anything
    ctrl.HandleRequest(LOW_LATENCY_VALUE, identity);
    EXPECT_EQ(counter->onCount, 1);
    EXPECT_EQ(counter->offCount, 0);
}

HWTEST_F(NetworkLatencyControllerTest, errorCancelNonExistentRequest_005, testing::ext::TestSize.Level1)
{
    const std::string identity("my.test.application");

    // cancelling a non-existing latency request should not do anything
    ctrl.HandleRequest(DEFAULT_LATENCY_VALUE, identity);
    EXPECT_EQ(counter->onCount, 0);
    EXPECT_EQ(counter->offCount, 0); // nothing changed
}

HWTEST_F(NetworkLatencyControllerTest, errorCancelForeignRequest_006, testing::ext::TestSize.Level1)
{
    const std::string identity1("my.test.application1");
    const std::string identity2("my.test.application2");

    // register a latency request from the first user
    ctrl.HandleRequest(LOW_LATENCY_VALUE, identity1);
    EXPECT_EQ(counter->onCount, 1);
    EXPECT_EQ(counter->offCount, 0);

    // cannot cancel the request using another identity
    ctrl.HandleRequest(DEFAULT_LATENCY_VALUE, identity2);
    EXPECT_EQ(counter->onCount, 1); // nothing changed
    EXPECT_EQ(counter->offCount, 0);
}

HWTEST_F(NetworkLatencyControllerTest, errorInvalidLatencyValue_007, testing::ext::TestSize.Level1)
{
    const std::string identity("my.test.application");
    long long badValue = 9999;

    // try to register non existing request value
    ctrl.HandleRequest(badValue, identity);
    EXPECT_EQ(counter->onCount, 0); // should not activate
    EXPECT_EQ(counter->offCount, 0);
}
