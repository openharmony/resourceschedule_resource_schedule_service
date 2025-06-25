/*
 * Copyright (c) 2025-2025 Huawei Device Co., Ltd.
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
#include "gtest/hwext/gtest-multithread.h"

#include "nativetoken_kit.h"
#include "suspend_manager_base_client.h"
#include "token_setproc.h"

namespace OHOS {
namespace ResourceSchedule {
using namespace std;
using namespace testing::ext;
using namespace testing::mt;
class SuspendManagerBaseClientTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
    void MockProcess(int32_t uid);
};


void SuspendManagerBaseClientTest::SetUpTestCase(void) {}

void SuspendManagerBaseClientTest::TearDownTestCase() {}

void SuspendManagerBaseClientTest::SetUp() {}

void SuspendManagerBaseClientTest::TearDown() {}

void SuspendManagerBaseClientTest::MockProcess(int32_t uid)
{
    static const char *perms[] = {
        "ohos.permission.GET_SUSPEND_STATE"
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
 * @tc.name: GetSuspendStateByUid
 * @tc.desc: Get application suspend state by uid test.
 * @tc.type: FUNC
 * @tc.require: #IC49DE
 * @tc.author: wujinghui
 */
HWTEST_F(SuspendManagerBaseClientTest, GetSuspendStateByUid, Function | MediumTest | Level0)
{
    int32_t uid = 7777;
    MockProcess(uid);
    int32_t appUid = 20020021;
    bool isFrozen;
    SuspendManagerBaseClient::GetInstance().GetSuspendStateByUid(appUid, isFrozen);
    if (SuspendManagerBaseClient::GetInstance().GetSuspendManagerProxy()) {
        EXPECT_NE(SuspendManagerBaseClient::GetInstance().suspendManagerBaseProxy_, nullptr);
    } else {
        EXPECT_EQ(SuspendManagerBaseClient::GetInstance().suspendManagerBaseProxy_, nullptr);
    }
}

/**
 * @tc.name: GetSuspendStateByPid
 * @tc.desc: Get application suspend state by pid test.
 * @tc.type: FUNC
 * @tc.require: #IC49DE
 * @tc.author: wujinghui
 */
HWTEST_F(SuspendManagerBaseClientTest, GetSuspendStateByPid, Function | MediumTest | Level0)
{
    int32_t uid = 7777;
    MockProcess(uid);
    int32_t appPid = 6666;
    bool isFrozen;
    SuspendManagerBaseClient::GetInstance().GetSuspendStateByPid(appPid, isFrozen);
    if (SuspendManagerBaseClient::GetInstance().GetSuspendManagerProxy()) {
        EXPECT_NE(SuspendManagerBaseClient::GetInstance().suspendManagerBaseProxy_, nullptr);
    } else {
        EXPECT_EQ(SuspendManagerBaseClient::GetInstance().suspendManagerBaseProxy_, nullptr);
    }
}
} // namespace ResourceSchedule
} // namespace OHOS