/*
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
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
#include "res_sched_client.h"
#include "res_sched_systemload_notifier_client.h"
#include "token_setproc.h"

namespace OHOS {
namespace ResourceSchedule {
using namespace std;
using namespace testing::ext;
static constexpr int32_t RSS_SA_ID = 1901;
static constexpr int32_t OTHER_SA_ID = 1900;
class ResSchedClientTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
    void MockProcess(int32_t uid);
};


void ResSchedClientTest::SetUpTestCase(void) {}

void ResSchedClientTest::TearDownTestCase() {}

void ResSchedClientTest::SetUp() {}

void ResSchedClientTest::TearDown() {}

void ResSchedClientTest::MockProcess(int32_t uid)
{
    static const char *perms[] = {
        "ohos.permission.DISTRIBUTED_DATASYNC"
    };
    uint64_t tokenId;
    NativeTokenInfoParams infoInstance = {
        .dcapsNum = 0,
        .permsNum = 1,
        .aclsNum = 0,
        .dcaps = nullptr,
        .perms = perms,
        .acls = nullptr,
        .processName = "samgr",
        .aplStr = "system_core",
    };
    tokenId = GetAccessTokenId(&infoInstance);
    SetSelfTokenID(tokenId);
    setuid(uid);
}

class ResSchedSystemloadNotifierClientMock : public ResSchedSystemloadNotifierClient {
public:
    ResSchedSystemloadNotifierClientMock() = default;
    ~ResSchedSystemloadNotifierClientMock() = default;
    void OnSystemloadLevel(int32_t level) override
    {
        levels = level;
    }
    static int32_t levels;
};

int32_t ResSchedSystemloadNotifierClientMock::levels = 0;

/**
 * @tc.name: KillProcess001
 * @tc.desc: kill process stable test
 * @tc.type: FUNC
 * @tc.require: I6EEJI
 * @tc.author: qiunaiguang
 */
HWTEST_F(ResSchedClientTest, KillProcess001, Function | MediumTest | Level0)
{
    int32_t uid = 5555;
    MockProcess(uid);
    std::unordered_map<std::string, std::string> mapPayload;
    mapPayload["pid"] = "65535";
    mapPayload["processName"] = "test";
    for (int i = 0; i < 100; i++) {
        ResSchedClient::GetInstance().KillProcess(mapPayload);
    }
    EXPECT_TRUE(ResSchedClient::GetInstance().rss_);
}

/**
 * @tc.name: KillProcess002
 * @tc.desc: kill process error test
 * @tc.type: FUNC
 * @tc.require: I6EEJI
 * @tc.author: qiunaiguang
 */
HWTEST_F(ResSchedClientTest, KillProcess002, Function | MediumTest | Level0)
{
    int32_t uid = 5555;
    MockProcess(uid);
    std::unordered_map<std::string, std::string> mapPayload;
    ResSchedClient::GetInstance().KillProcess(mapPayload);
    EXPECT_TRUE(ResSchedClient::GetInstance().rss_);

    mapPayload["pid"] = "TEST";
    ResSchedClient::GetInstance().KillProcess(mapPayload);
    EXPECT_TRUE(ResSchedClient::GetInstance().rss_);

    mapPayload["pid"] = "65535";
    mapPayload["processName"] = "test";
    ResSchedClient::GetInstance().KillProcess(mapPayload);
    EXPECT_TRUE(ResSchedClient::GetInstance().rss_);

    uid = 6666;
    MockProcess(uid);
    ResSchedClient::GetInstance().KillProcess(mapPayload);
    EXPECT_TRUE(ResSchedClient::GetInstance().rss_);
}

/**
 * @tc.name: StopRemoteObject
 * @tc.desc: Stop Remote Object
 * @tc.type: FUNC
 * @tc.require: I78O6Y
 * @tc.author: lujunchao
 */
HWTEST_F(ResSchedClientTest, StopRemoteObject, Function | MediumTest | Level0)
{
    ResSchedClient::GetInstance().StopRemoteObject();
    EXPECT_TRUE(nullptr == ResSchedClient::GetInstance().rss_);
}

/**
 * @tc.name: RegisterSystemloadNotifier001
 * @tc.desc: Register systemload notifier
 * @tc.type: FUNC
 * @tc.require: issueI9G149
 * @tc.author: shanhaiyang
 */
HWTEST_F(ResSchedClientTest, RegisterSystemloadNotifier001, Function | MediumTest | Level0)
{
    sptr<ResSchedSystemloadNotifierClient> notifier =
        new (std::nothrow) ResSchedSystemloadNotifierClientMock;
    EXPECT_TRUE(notifier != nullptr);
    ResSchedClient::GetInstance().RegisterSystemloadNotifier(notifier);
    ResSchedClient::GetInstance().RegisterSystemloadNotifier(notifier);
    ResSchedClient::GetInstance().systemloadLevelListener_->OnSystemloadLevel(2);
    EXPECT_TRUE(ResSchedSystemloadNotifierClientMock::levels == 2);
    ResSchedSystemloadNotifierClientMock::levels = 0;
    ResSchedClient::GetInstance().UnRegisterSystemloadNotifier(notifier);
}

/**
 * @tc.name: UnRegisterSystemloadNotifier001
 * @tc.desc: UnRegister systemload notifier
 * @tc.type: FUNC
 * @tc.require: issueI9G149
 * @tc.author: shanhaiyang
 */
HWTEST_F(ResSchedClientTest, UnRegisterSystemloadNotifier001, Function | MediumTest | Level0)
{
    sptr<ResSchedSystemloadNotifierClient> notifier =
        new (std::nothrow) ResSchedSystemloadNotifierClientMock;
    EXPECT_TRUE(notifier != nullptr);
    ResSchedClient::GetInstance().RegisterSystemloadNotifier(notifier);
    ResSchedClient::GetInstance().UnRegisterSystemloadNotifier(notifier);
    ResSchedClient::GetInstance().systemloadLevelListener_->OnSystemloadLevel(2);
    EXPECT_TRUE(ResSchedSystemloadNotifierClientMock::levels == 0);
}

/**
 * @tc.name: GetSystemloadLevel001
 * @tc.desc: Get systemload level
 * @tc.type: FUNC
 * @tc.require: issueI9G149
 * @tc.author: shanhaiyang
 */
HWTEST_F(ResSchedClientTest, GetSystemloadLevel001, Function | MediumTest | Level0)
{
    int32_t res = ResSchedClient::GetInstance().GetSystemloadLevel();
    EXPECT_TRUE(ResSchedClient::GetInstance().rss_);
}

/**
 * @tc.name: ResSchedSvcStatusChange add 001
 * @tc.desc: ResSchedSvcStatusChange OnAddSystemAbility
 * @tc.type: FUNC
 * @tc.require: issueI9G149
 * @tc.author: shanhaiyang
 */
HWTEST_F(ResSchedClientTest, OnAddSystemAbility001, Function | MediumTest | Level0)
{
    ASSERT_TRUE(ResSchedClient::GetInstance().resSchedSvcStatusListener_);
    std::string empty;
    ResSchedClient::GetInstance().resSchedSvcStatusListener_->OnAddSystemAbility(RSS_SA_ID, empty);
    sptr<ResSchedSystemloadNotifierClient> notifier =
        new (std::nothrow) ResSchedSystemloadNotifierClientMock;
    EXPECT_TRUE(notifier != nullptr);
    ResSchedClient::GetInstance().RegisterSystemloadNotifier(notifier);
    ResSchedClient::GetInstance().UnRegisterSystemloadNotifier(notifier);
    SUCCEED();
}

/**
 * @tc.name: ResSchedSvcStatusChange add 002
 * @tc.desc: ResSchedSvcStatusChange OnAddSystemAbility
 * @tc.type: FUNC
 * @tc.require: issueI9G149
 * @tc.author: shanhaiyang
 */
HWTEST_F(ResSchedClientTest, OnAddSystemAbility002, Function | MediumTest | Level0)
{
    ASSERT_TRUE(ResSchedClient::GetInstance().resSchedSvcStatusListener_);
    std::string empty;
    ResSchedClient::GetInstance().resSchedSvcStatusListener_->OnAddSystemAbility(RSS_SA_ID, empty);
    sptr<ResSchedSystemloadNotifierClient> notifier =
        new (std::nothrow) ResSchedSystemloadNotifierClientMock;
    EXPECT_TRUE(notifier != nullptr);
    ResSchedClient::GetInstance().RegisterSystemloadNotifier(notifier);
    ResSchedClient::GetInstance().UnRegisterSystemloadNotifier(notifier);
    SUCCEED();
}

/**
 * @tc.name: ResSchedSvcStatusChange remove 001
 * @tc.desc: ResSchedSvcStatusChange OnRemoveSystemAbility
 * @tc.type: FUNC
 * @tc.require: issueI9G149
 * @tc.author: shanhaiyang
 */
HWTEST_F(ResSchedClientTest, OnRemoveSystemAbility001, Function | MediumTest | Level0)
{
    sptr<ResSchedSystemloadNotifierClient> notifier =
        new (std::nothrow) ResSchedSystemloadNotifierClientMock;
    EXPECT_TRUE(notifier != nullptr);
    ResSchedClient::GetInstance().RegisterSystemloadNotifier(notifier);
    ASSERT_TRUE(ResSchedClient::GetInstance().resSchedSvcStatusListener_);
    std::string empty;
    ResSchedClient::GetInstance().resSchedSvcStatusListener_->OnRemoveSystemAbility(OTHER_SA_ID, empty);
    ResSchedClient::GetInstance().UnRegisterSystemloadNotifier(notifier);
    SUCCEED();
}

/**
 * @tc.name: IsAllowedAppPreload
 * @tc.desc: Is allowed application preload
 * @tc.type: FUNC
 * @tc.require: issueI9C9JN
 * @tc.author: xiaoshun
 */
HWTEST_F(ResSchedClientTest, IsAllowedAppPreload, Function | MediumTest | Level0)
{
    std::string bundleName = "com.ohos.test";
    EXPECT_TRUE(ResSchedClient::GetInstance().rss_);
    EXPECT_TRUE(!ResSchedClient::GetInstance().IsAllowedAppPreload(bundleName, 0));
}
} // namespace ResourceSchedule
} // namespace OHOS
