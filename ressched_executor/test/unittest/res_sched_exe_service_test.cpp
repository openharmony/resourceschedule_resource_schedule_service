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
#include "gtest/hwext/gtest-multithread.h"

#include <vector>

#include "accesstoken_kit.h"
#include "ipc_skeleton.h"
#include "nativetoken_kit.h"
#include "token_setproc.h"

#include "plugin_mgr.h"
#include "res_exe_type.h"
#include "res_common_util.h"
#include "res_json_type.h"
#include "res_sched_exe_constants.h"
#include "res_sched_exe_service.h"

namespace OHOS {
namespace ResourceSchedule {
using namespace std;
using namespace testing::ext;
using namespace testing::mt;
using namespace Security::AccessToken;

namespace {
    constexpr int32_t SYNC_THREAD_NUM = 100;
    constexpr int32_t SYNC_INTERNAL_TIME = 10000;
}

class ResSchedExeServiceTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
protected:
    std::shared_ptr<ResSchedExeService> resSchedExeService_ = nullptr;
};

void ResSchedExeServiceTest::SetUpTestCase(void)
{
    static const char *perms[] = {
        "ohos.permission.REPORT_RESOURCE_SCHEDULE_EVENT",
        "ohos.permission.DUMP",
    };
    uint64_t tokenId;
    NativeTokenInfoParams infoInstance = {
        .dcapsNum = 0,
        .permsNum = 2,
        .aclsNum = 0,
        .dcaps = nullptr,
        .perms = perms,
        .acls = nullptr,
        .processName = "ResSchedExeServiceTest",
        .aplStr = "system_core",
    };
    tokenId = GetAccessTokenId(&infoInstance);
    SetSelfTokenID(tokenId);
    AccessTokenKit::ReloadNativeTokenInfo();
}

void ResSchedExeServiceTest::TearDownTestCase() {}

void ResSchedExeServiceTest::SetUp()
{
    /**
     * @tc.setup: initialize the member variable resSchedExeServiceAbility_
     */
    resSchedExeService_ = make_shared<ResSchedExeService>();
}

void ResSchedExeServiceTest::TearDown()
{
    /**
     * @tc.teardown: clear resSchedExeServiceAbility_
     */
    resSchedExeService_ = nullptr;
}

/**
 * @tc.name: resschedexe service dump 001
 * @tc.desc: Verify if resschedexe service dump commonds is success.
 * @tc.type: FUNC
 */
HWTEST_F(ResSchedExeServiceTest, ServiceDump001, Function | MediumTest | Level0)
{
    PluginMgr::GetInstance().Init(true);
    std::string result;
    resSchedExeService_->DumpAllInfo(result);
    EXPECT_TRUE(!result.empty());

    result = "";
    resSchedExeService_->DumpUsage(result);
    EXPECT_TRUE(!result.empty());

    int32_t wrongFd = -1;
    std::vector<std::u16string> argsNull;
    int res = resSchedExeService_->Dump(wrongFd, argsNull);
    EXPECT_NE(res, ResErrCode::RSSEXE_NO_ERR);

    int32_t correctFd = 0;
    res = resSchedExeService_->Dump(correctFd, argsNull);

    std::vector<std::u16string> argsHelp = {to_utf16("-h")};
    res = resSchedExeService_->Dump(correctFd, argsHelp);

    std::vector<std::u16string> argsAll = {to_utf16("-a")};
    res = resSchedExeService_->Dump(correctFd, argsAll);

    std::vector<std::u16string> argsError = {to_utf16("-e")};
    res = resSchedExeService_->Dump(correctFd, argsError);

    std::vector<std::u16string> argsPlugin = {to_utf16("-p")};
    res = resSchedExeService_->Dump(correctFd, argsPlugin);

    std::vector<std::u16string> argsOnePlugin = {to_utf16("-p"), to_utf16("1")};
    res = resSchedExeService_->Dump(correctFd, argsOnePlugin);
}

/**
 * @tc.name: Ressched_executor service SendRequestSync001
 * @tc.desc: Verify if Ressched_executor service SendRequestSync is success.
 * @tc.type: FUNC
 */
HWTEST_F(ResSchedExeServiceTest, SendRequestSync001, Function | MediumTest | Level0)
{
    ResJsonType payload;
    ResJsonType reply;
    int32_t result = -1;

    EXPECT_TRUE(resSchedExeService_ != nullptr);
    resSchedExeService_->SendRequestSync(0, 0, payload, reply, result);
}

static void SendRequestSyncTask()
{
    std::shared_ptr<ResSchedExeService> resSchedExeService_ = make_shared<ResSchedExeService>();
    ResJsonType payload;
    ResJsonType reply;
    int32_t result = -1;

    EXPECT_TRUE(resSchedExeService_ != nullptr);
    for (int i = 0; i < SYNC_THREAD_NUM; i++) {
        resSchedExeService_->SendRequestSync(0, 0, payload, reply, result);
        usleep(SYNC_INTERNAL_TIME);
    }
}

/**
 * @tc.name: Ressched_executor service SendRequestSync002
 * @tc.desc: Test Ressched_executor service SendRequestSync in multithreading.
 * @tc.type: FUNC
 */
HWTEST_F(ResSchedExeServiceTest, SendRequestSync002, Function | MediumTest | Level0)
{
    SET_THREAD_NUM(10);
    GTEST_RUN_TASK(SendRequestSyncTask);
}

/**
 * @tc.name: Ressched_executor service SendRequestAsync001
 * @tc.desc: Verify if Ressched_executor service SendRequestAsync is success.
 * @tc.type: FUNC
 */
HWTEST_F(ResSchedExeServiceTest, SendRequestAsync001, Function | MediumTest | Level0)
{
    ResJsonType payload;
    EXPECT_TRUE(resSchedExeService_ != nullptr);
    resSchedExeService_->SendRequestAsync(0, 0, payload);
}

static void SendRequestAsyncTask()
{
    std::shared_ptr<ResSchedExeService> resSchedExeService_ = make_shared<ResSchedExeService>();
    ResJsonType payload;
    for (int i = 0; i < SYNC_THREAD_NUM; i++) {
        EXPECT_TRUE(resSchedExeService_ != nullptr);
        resSchedExeService_->SendRequestAsync(0, 0, payload);
        usleep(SYNC_INTERNAL_TIME);
    }
}

/**
 * @tc.name: Ressched_executor service SendRequestAsync002
 * @tc.desc: Test Ressched_executor service SendRequestAsync in multithreading.
 * @tc.type: FUNC
 */
HWTEST_F(ResSchedExeServiceTest, SendRequestAsync002, Function | MediumTest | Level0)
{
    SET_THREAD_NUM(10);
    GTEST_RUN_TASK(SendRequestAsyncTask);
}

static void OnStartTask()
{
    std::shared_ptr<ResSchedExeService> resSchedExeService_ = make_shared<ResSchedExeService>();
    for (int i = 0; i < SYNC_THREAD_NUM; i++) {
        resSchedExeService_->OnStart();
        EXPECT_TRUE(resSchedExeService_ != nullptr);
        usleep(SYNC_INTERNAL_TIME);
    }
}

/**
 * @tc.name: Start ResSchedExeServiceAbility OnStart002
 * @tc.desc: Test ResSchedExeServiceAbility OnStart in multithreading.
 * @tc.type: FUNC
 */
HWTEST_F(ResSchedExeServiceTest, OnStart002, Function | MediumTest | Level0)
{
    SET_THREAD_NUM(10);
    GTEST_RUN_TASK(OnStartTask);
}

class TestResSchedExeServiceStub : public ResSchedExeServiceStub {
public:
    TestResSchedExeServiceStub() : ResSchedExeServiceStub() {}

    ErrCode SendRequestAsync(uint32_t resType, int64_t value, const ResJsonType& context) override
    {
        return ERR_OK;
    }

    ErrCode SendRequestSync(uint32_t resType, int64_t value,
        const ResJsonType& context, ResJsonType& response, int32_t& funcResult) override
    {
        return ERR_OK;
    }

    ErrCode KillProcess(uint32_t pid, int32_t& funcResult) override
    {
        return ERR_OK;
    }
};

/**
 * @tc.name: ResSchedExeServicesStub RemoteRequest001
 * @tc.desc: Verify if resschedexestub RemoteRequest is success.
 * @tc.type: FUNC
 */
HWTEST_F(ResSchedExeServiceTest, RemoteRequest001, Function | MediumTest | Level0)
{
    auto resSchedExeServiceStub_ = make_shared<TestResSchedExeServiceStub>();
    MessageOption option;
    MessageParcel reply;
    int32_t res = resSchedExeServiceStub_->OnRemoteRequest(ResIpcType::REQUEST_SEND_SYNC, reply, reply, option);
    EXPECT_TRUE(res);
    res = resSchedExeServiceStub_->OnRemoteRequest(ResIpcType::REQUEST_SEND_ASYNC, reply, reply, option);
    EXPECT_TRUE(res);
    res = resSchedExeServiceStub_->OnRemoteRequest(ResIpcType::REQUEST_SEND_DEBUG, reply, reply, option);
    EXPECT_TRUE(res);
}

static void RemoteRequestTask()
{
    auto resSchedExeServiceStub_ = make_shared<TestResSchedExeServiceStub>();
    MessageOption option;
    MessageParcel reply;
    for (int i = 0; i < SYNC_THREAD_NUM; i++) {
        int32_t res = resSchedExeServiceStub_->OnRemoteRequest(ResIpcType::REQUEST_SEND_SYNC, reply, reply, option);
        EXPECT_TRUE(res);
        res = resSchedExeServiceStub_->OnRemoteRequest(ResIpcType::REQUEST_SEND_ASYNC, reply, reply, option);
        EXPECT_TRUE(res);
        res = resSchedExeServiceStub_->OnRemoteRequest(ResIpcType::REQUEST_SEND_DEBUG, reply, reply, option);
        EXPECT_TRUE(res);
        usleep(SYNC_INTERNAL_TIME);
    }
}

/**
 * @tc.name: ResSchedExeServicesStub RemoteRequest002
 * @tc.desc: Test resschedexestub RemoteRequest in multithreading.
 * @tc.type: FUNC
 */
HWTEST_F(ResSchedExeServiceTest, RemoteRequest002, Function | MediumTest | Level0)
{
    SET_THREAD_NUM(10);
    GTEST_RUN_TASK(RemoteRequestTask);
}
} // namespace ResourceSchedule
} // namespace OHOS
