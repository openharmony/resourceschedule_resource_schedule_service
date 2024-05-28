/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "res_exe_type.h"
#include "res_sched_exe_client.h"
#include "res_sched_exe_constants.h"

namespace OHOS {
namespace ResourceSchedule {
using namespace std;
using namespace testing::ext;

namespace {
    constexpr int32_t SYNC_THREAD_NUM = 100;
    constexpr int32_t SYNC_INTERNAL_TIME = 200;
}

class ResSchedExeClientTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
    void MockProcess(int32_t uid);
};


void ResSchedExeClientTest::SetUpTestCase(void) {}

void ResSchedExeClientTest::TearDownTestCase() {}

void ResSchedExeClientTest::SetUp() {}

void ResSchedExeClientTest::TearDown() {}

void ResSchedExeClientTest::MockProcess(int32_t uid)
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
 * @tc.name: SendRequestSync001
 * @tc.desc: send res request stable test
 * @tc.type: FUNC
 */
HWTEST_F(ResSchedExeClientTest, SendRequestSync001, Function | MediumTest | Level0)
{
    nlohmann::json context;
    context["message"] = "test";
    nlohmann::json reply;
    for (int i = 0; i < SYNC_THREAD_NUM; i++) {
        ResSchedExeClient::GetInstance().SendRequestSync(ResExeType::RES_TYPE_DEBUG, 0, context, reply);
        usleep(SYNC_INTERNAL_TIME);
    }
    EXPECT_TRUE(ResSchedExeClient::GetInstance().resSchedExe_);
}

/**
 * @tc.name: SendRequestAsync001
 * @tc.desc: report data stable test
 * @tc.type: FUNC
 */
HWTEST_F(ResSchedExeClientTest, SendRequestAsync001, Function | MediumTest | Level0)
{
    nlohmann::json context;
    context["message"] = "test";
    for (int i = 0; i < SYNC_THREAD_NUM; i++) {
        ResSchedExeClient::GetInstance().SendRequestAsync(ResExeType::RES_TYPE_DEBUG, 0, context);
        usleep(SYNC_INTERNAL_TIME);
    }
    EXPECT_TRUE(ResSchedExeClient::GetInstance().resSchedExe_);
}

/**
 * @tc.name: KillProcess001
 * @tc.desc: kill process stable test
 * @tc.type: FUNC
 */
HWTEST_F(ResSchedExeClientTest, KillProcess001, Function | MediumTest | Level0)
{
    int32_t uid = 5555;
    MockProcess(uid);
    int32_t pid = 65535;
    for (int i = 0; i < 100; i++) {
        ResSchedExeClient::GetInstance().KillProcess(pid);
        usleep(SYNC_INTERNAL_TIME);
    }
    EXPECT_TRUE(ResSchedExeClient::GetInstance().resSchedExe_);
}

/**
 * @tc.name: KillProcess002
 * @tc.desc: kill process error test
 * @tc.type: FUNC
 */
HWTEST_F(ResSchedExeClientTest, KillProcess002, Function | MediumTest | Level0)
{
    int32_t uid = 5555;
    MockProcess(uid);

    int32_t pid = -1;
    std::unordered_map<std::string, std::string> mapPayload;
    ResSchedExeClient::GetInstance().KillProcess(pid);
    EXPECT_TRUE(ResSchedExeClient::GetInstance().resSchedExe_);

    pid = 65535;
    ResSchedExeClient::GetInstance().KillProcess(pid);
    EXPECT_TRUE(ResSchedExeClient::GetInstance().resSchedExe_);
}

/**
 * @tc.name: SendDebugCommand001
 * @tc.desc: send debug command stable test
 * @tc.type: FUNC
 */
HWTEST_F(ResSchedExeClientTest, SendDebugCommand001, Function | MediumTest | Level0)
{
    for (int i = 0; i < SYNC_THREAD_NUM; i++) {
        ResSchedExeClient::GetInstance().SendDebugCommand(true);
        usleep(SYNC_INTERNAL_TIME);
    }
    EXPECT_TRUE(ResSchedExeClient::GetInstance().resSchedExe_);
}

/**
 * @tc.name: SendDebugCommand002
 * @tc.desc: send debug command stable test
 * @tc.type: FUNC
 */
HWTEST_F(ResSchedExeClientTest, SendDebugCommand002, Function | MediumTest | Level0)
{
    for (int i = 0; i < SYNC_THREAD_NUM; i++) {
        ResSchedExeClient::GetInstance().SendDebugCommand(false);
        usleep(SYNC_INTERNAL_TIME);
    }
    EXPECT_TRUE(ResSchedExeClient::GetInstance().resSchedExe_);
}

/**
 * @tc.name: StopRemoteObject001
 * @tc.desc: Stop Remote Object
 * @tc.type: FUNC
 */
HWTEST_F(ResSchedExeClientTest, StopRemoteObject001, Function | MediumTest | Level0)
{
    ResSchedExeClient::GetInstance().StopRemoteObject();
    EXPECT_TRUE(nullptr == ResSchedExeClient::GetInstance().resSchedExe_);
}
} // namespace ResourceSchedule
} // namespace OHOS
