/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
    MockProcess(std::string processName);
};


void ResSchedClientTest::SetUpTestCase(void) {}

void ResSchedClientTest::TearDownTestCase() {}

void ResSchedClientTest::SetUp() {}

void ResSchedClientTest::TearDown() {}

void ResSchedClientTest::MockProcess(std::string processName)
{
    static const char *PERMS[] = {
        "ohos.permission.DISTRIBUTED_DATASYNC"
    };
    uint64_t tokenId;
    NativeTokenInfoParams infoInstance = {
        .dcapsNum = 0,
        .permsNum = 1,
        .aclsNum = 0,
        .dcaps = nullptr,
        .perms = PERMS,
        .acls = nullptr,
        .processName = processName.c_str(),
        .aplStr = "system_core",
    };
    tokenId = GetAccessTokenId(&infoInstance);
    SetSelfTokenID(tokenId);
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
    std::string processName = "samgr";
    MockProcess(processName);
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
    std::string processName = "samgr";
    MockProcess(processName);
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

    processName = "resource_schedule_service";
    MockProcess(processName);
    ResSchedClient::GetInstance().KillProcess(mapPayload);
    EXPECT_TRUE(ResSchedClient::GetInstance().rss_);
}
#undef private
#undef protected
} // namespace ResourceSchedule
} // namespace OHOS
