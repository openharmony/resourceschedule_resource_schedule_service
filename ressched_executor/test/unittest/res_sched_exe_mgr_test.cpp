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

#include "plugin_mgr.h"
#include "res_exe_type.h"
#include "res_sched_exe_constants.h"
#include "res_sched_exe_mgr.h"

namespace OHOS {
namespace ResourceSchedule {
using namespace std;
using namespace testing::ext;

class ResSchedExeMgrTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
    void MockProcess(int32_t uid);
};


void ResSchedExeMgrTest::SetUpTestCase(void)
{
    ResSchedExeMgr::GetInstance().Init();
}

void ResSchedExeMgrTest::TearDownTestCase()
{
    ResSchedExeMgr::GetInstance().Stop();
}

void ResSchedExeMgrTest::SetUp() {}

void ResSchedExeMgrTest::TearDown() {}

void ResSchedExeMgrTest::MockProcess(int32_t uid)
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
HWTEST_F(ResSchedExeMgrTest, SendRequestSync001, Function | MediumTest | Level0)
{
    nlohmann::json context;
    context["message"] = "test";
    nlohmann::json reply;
    int32_t ret = ResSchedExeMgr::GetInstance().SendRequestSync(ResExeType::RES_TYPE_DEBUG, 0, context, reply);
    EXPECT_EQ(ret, ResErrCode::RSSEXE_NO_ERR);
}

/**
 * @tc.name: SendRequestSync002
 * @tc.desc: send res request stable test
 * @tc.type: FUNC
 */
HWTEST_F(ResSchedExeMgrTest, SendRequestSync002, Function | MediumTest | Level0)
{
    nlohmann::json context;
    context["message"] = "test";
    nlohmann::json reply;
    int32_t ret = ResSchedExeMgr::GetInstance().SendRequestSync(
        ResExeType::RES_TYPE_EXECUTOR_PLUGIN_INIT, 0, context, reply);
    EXPECT_EQ(ret, ResErrCode::RSSEXE_NO_ERR);
}

/**
 * @tc.name: SendRequestSync003
 * @tc.desc: send res request stable test
 * @tc.type: FUNC
 */
HWTEST_F(ResSchedExeMgrTest, SendRequestSync003, Function | MediumTest | Level0)
{
    nlohmann::json context;
    context["message"] = "test";
    nlohmann::json reply;
    int32_t ret = ResSchedExeMgr::GetInstance().SendRequestSync(
        ResExeType::RES_TYPE_CGROUP_SYNC_EVENT, 0, context, reply);
    EXPECT_EQ(ret, ResErrCode::RSSEXE_NO_ERR);
}

/**
 * @tc.name: SendRequestSync004
 * @tc.desc: send res request stable test
 * @tc.type: FUNC
 */
HWTEST_F(ResSchedExeMgrTest, SendRequestSync004, Function | MediumTest | Level0)
{
    nlohmann::json context;
    context["message"] = "test";
    nlohmann::json reply;
    int32_t ret = ResSchedExeMgr::GetInstance().SendRequestSync(
        ResExeType::RES_TYPE_CGROUP_PROC_TASK_SYNC_EVENT, 0, context, reply);
    EXPECT_EQ(ret, ResErrCode::RSSEXE_NO_ERR);
}

/**
 * @tc.name: SendRequestSync005
 * @tc.desc: send res request stable test
 * @tc.type: FUNC
 */
HWTEST_F(ResSchedExeMgrTest, SendRequestSync005, Function | MediumTest | Level0)
{
    nlohmann::json context;
    context["message"] = "test";
    nlohmann::json reply;
    int32_t ret = ResSchedExeMgr::GetInstance().SendRequestSync(
        ResExeType::RES_TYPE_SET_THREAD_SCHED_POLICY_SYNC_EVENT, 0, context, reply);
    EXPECT_EQ(ret, ResErrCode::RSSEXE_NO_ERR);
}

/**
 * @tc.name: SendRequestSync006
 * @tc.desc: send res request stable test
 * @tc.type: FUNC
 */
HWTEST_F(ResSchedExeMgrTest, SendRequestSync006, Function | MediumTest | Level0)
{
    nlohmann::json context;
    context["tid"] = 1;
    context["policy"] = 1;
    nlohmann::json reply;
    int32_t ret = ResSchedExeMgr::GetInstance().SendRequestSync(
        ResExeType::RES_TYPE_SET_THREAD_SCHED_POLICY_SYNC_EVENT, 0, context, reply);
    EXPECT_EQ(ret, ResErrCode::RSSEXE_NO_ERR);
}

/**
 * @tc.name: SendRequestSync007
 * @tc.desc: send res request stable test
 * @tc.type: FUNC
 */
HWTEST_F(ResSchedExeMgrTest, SendRequestSync007, Function | MediumTest | Level0)
{
    nlohmann::json context;
    context["tid"] = 1;
    context["policy"] = 1;
    nlohmann::json reply;
    int32_t ret = ResSchedExeMgr::GetInstance().SendRequestSync(
        ResExeType::RES_TYPE_SET_THREAD_GROUP_SCHED_POLICY_SYNC_EVENT, 0, context, reply);
    EXPECT_EQ(ret, ResErrCode::RSSEXE_NO_ERR);
}

/**
 * @tc.name: SendRequestAsync001
 * @tc.desc: report data stable test
 * @tc.type: FUNC
 */
HWTEST_F(ResSchedExeMgrTest, SendRequestAsync001, Function | MediumTest | Level0)
{
    nlohmann::json context;
    context["message"] = "test";
    ResSchedExeMgr::GetInstance().SendRequestAsync(ResExeType::RES_TYPE_EXECUTOR_PLUGIN_INIT, 0, context);
    EXPECT_TRUE(PluginMgr::GetInstance().configReader_);
    ResSchedExeMgr::GetInstance().SendRequestAsync(ResExeType::RES_TYPE_DEBUG, 0, context);
}

/**
 * @tc.name: KillProcess001
 * @tc.desc: kill process stable test
 * @tc.type: FUNC
 */
HWTEST_F(ResSchedExeMgrTest, KillProcess001, Function | MediumTest | Level0)
{
    int32_t uid = 5555;
    MockProcess(uid);
    int32_t pid = 65535;
    int32_t ret = ResSchedExeMgr::GetInstance().KillProcess(pid);
    EXPECT_TRUE(ret <= ResErrCode::RSSEXE_NO_ERR);
}

/**
 * @tc.name: InitPluginMgr001
 * @tc.desc: Verify if InitExecutorPlugin is success
 * @tc.type: FUNC
 */
HWTEST_F(ResSchedExeMgrTest, InitPluginMgr001, TestSize.Level1)
{
    nlohmann::json context;
    context["config"] = "test_config";
    context["switch"] = "test_switch";
    ResSchedExeMgr::GetInstance().InitPluginMgr(context);
    EXPECT_TRUE(PluginMgr::GetInstance().configReader_);
}

/**
 * @tc.name: InitPluginMgrPretreatment001
 * @tc.desc: Verify if InitPluginMgrPretreatment is success
 * @tc.type: FUNC
 */
HWTEST_F(ResSchedExeMgrTest, InitPluginMgrPretreatment001, TestSize.Level1)
{
    nlohmann::json context;
    context["MESSAGE_INDEX"] = 0;
    context["MESSAGE_NUMBER"] = 1;
    context["IPC_MESSAGE"] = "test_switch";
    ResSchedExeMgr::GetInstance().InitPluginMgrPretreatment(context);
    EXPECT_TRUE(PluginMgr::GetInstance().configReader_);
}

/**
 * @tc.name: BuildTraceStr001
 * @tc.desc: BuildTraceStr test
 * @tc.type: FUNC
 */
HWTEST_F(ResSchedExeMgrTest, BuildTraceStr001, Function | MediumTest | Level0)
{
    std::string trace = ResSchedExeMgr::GetInstance().BuildTraceStr(__func__, ResExeType::RES_TYPE_DEBUG, 0);
    EXPECT_FALSE(trace.empty());
}

/**
 * @tc.name: Init001
 * @tc.desc: Init test
 * @tc.type: FUNC
 */
HWTEST_F(ResSchedExeMgrTest, Init001, Function | MediumTest | Level0)
{
    ResSchedExeMgr::GetInstance().Init();
    EXPECT_TRUE(PluginMgr::GetInstance().configReader_);
}

/**
 * @tc.name: Stop001
 * @tc.desc: Stop test
 * @tc.type: FUNC
 */
HWTEST_F(ResSchedExeMgrTest, Stop001, Function | MediumTest | Level0)
{
    ResSchedExeMgr::GetInstance().Stop();
    EXPECT_TRUE(nullptr == PluginMgr::GetInstance().configReader_);
}
} // namespace ResourceSchedule
} // namespace OHOS
