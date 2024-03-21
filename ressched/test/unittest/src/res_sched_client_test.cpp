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

#define private public
#define protected public
#include <unordered_map>
#include <vector>
#include "nativetoken_kit.h"
#include "res_sched_client.h"
#include "token_setproc.h"

namespace OHOS {
namespace ResourceSchedule {
using namespace std;
using namespace testing::ext;
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
 * @tc.name: RegisterSystemloadNotifier
 * @tc.desc: Register systemload notifier
 * @tc.type: FUNC
 * @tc.require: I97M6C
 * @tc.author: shanhaiyang
 */
HWTEST_F(ResSchedClientTest, RegisterSystemloadNotifier, Function | MediumTest | Level0)
{
    sptr<IRemoteObject> notifier = nullptr;
    ResSchedClient::GetInstance().RegisterSystemloadNotifier(notifier);
    EXPECT_TRUE(ResSchedClient::GetInstance().rss_);
}

/**
 * @tc.name: UnRegisterSystemloadNotifier
 * @tc.desc: UnRegister systemload notifier
 * @tc.type: FUNC
 * @tc.require: I97M6C
 * @tc.author: shanhaiyang
 */
HWTEST_F(ResSchedClientTest, UnRegisterSystemloadNotifier, Function | MediumTest | Level0)
{
    ResSchedClient::GetInstance().UnRegisterSystemloadNotifier();
    EXPECT_TRUE(ResSchedClient::GetInstance().rss_);
}

/**
 * @tc.name: GetSystemloadLevel
 * @tc.desc: Get systemload level
 * @tc.type: FUNC
 * @tc.require: I97M6C
 * @tc.author: shanhaiyang
 */
HWTEST_F(ResSchedClientTest, GetSystemloadLevel, Function | MediumTest | Level0)
{
    int32_t res = ResSchedClient::GetInstance().GetSystemloadLevel();
    EXPECT_TRUE(!(res < 0 || res > 7));
}
#undef private
#undef protected
} // namespace ResourceSchedule
} // namespace OHOS
