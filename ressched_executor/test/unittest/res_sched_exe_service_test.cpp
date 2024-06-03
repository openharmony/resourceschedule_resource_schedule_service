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
#include "res_sched_exe_common_utils.h"
#include "res_sched_exe_constants.h"
#include "res_sched_exe_service.h"
#include "res_sched_exe_service_ability.h"

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
    std::shared_ptr<ResSchedExeServiceAbility> resSchedExeServiceAbility_ = nullptr;
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
    resSchedExeServiceAbility_ = make_shared<ResSchedExeServiceAbility>();
}

void ResSchedExeServiceTest::TearDown()
{
    /**
     * @tc.teardown: clear resSchedExeServiceAbility_
     */
    resSchedExeService_ = nullptr;
    resSchedExeServiceAbility_ = nullptr;
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
    nlohmann::json payload;
    nlohmann::json reply;
    EXPECT_TRUE(resSchedExeService_ != nullptr);
    resSchedExeService_->SendRequestSync(0, 0, payload, reply);
}

static void SendRequestSyncTask()
{
    std::shared_ptr<ResSchedExeService> resSchedExeService_ = make_shared<ResSchedExeService>();
    nlohmann::json payload;
    nlohmann::json reply;
    EXPECT_TRUE(resSchedExeService_ != nullptr);
    for (int i = 0; i < SYNC_THREAD_NUM; i++) {
        resSchedExeService_->SendRequestSync(0, 0, payload, reply);
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
    nlohmann::json payload;
    EXPECT_TRUE(resSchedExeService_ != nullptr);
    resSchedExeService_->SendRequestAsync(0, 0, payload);
}

static void SendRequestAsyncTask()
{
    std::shared_ptr<ResSchedExeService> resSchedExeService_ = make_shared<ResSchedExeService>();
    nlohmann::json payload;
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

/**
 * @tc.name: Start ResSchedExeServiceAbility OnStart001
 * @tc.desc: Verify if ResSchedExeServiceAbility OnStart is success.
 * @tc.type: FUNC
 */
HWTEST_F(ResSchedExeServiceTest, OnStart001, Function | MediumTest | Level0)
{
    resSchedExeServiceAbility_->OnStart();
    EXPECT_TRUE(resSchedExeServiceAbility_->service_ != nullptr);
}

static void OnStartTask()
{
    std::shared_ptr<ResSchedExeServiceAbility> resSchedExeServiceAbility_ = make_shared<ResSchedExeServiceAbility>();
    for (int i = 0; i < SYNC_THREAD_NUM; i++) {
        resSchedExeServiceAbility_->OnStart();
        EXPECT_TRUE(resSchedExeServiceAbility_->service_ != nullptr);
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

/**
 * @tc.name: Stop ResSchedExeServiceAbility OnStop001
 * @tc.desc: test the interface Onstop
 * @tc.type: FUNC
 */
HWTEST_F(ResSchedExeServiceTest, OnStop001, Function | MediumTest | Level0)
{
    resSchedExeServiceAbility_->OnStop();
    SUCCEED();
}

class TestResSchedExeServiceStub : public ResSchedExeServiceStub {
public:
    TestResSchedExeServiceStub() : ResSchedExeServiceStub() {}

    void SendRequestAsync(uint32_t restype, int64_t value, const nlohmann::json& context) override
    {
    }

    int32_t SendRequestSync(uint32_t restype, int64_t value,
        const nlohmann::json& context, nlohmann::json& reply) override
    {
        return 0;
    }

    int32_t KillProcess(pid_t pid) override
    {
        return 0;
    }
};

/**
 * @tc.name: ResSchedExeServicesStub ReportRequestInner001
 * @tc.desc: Verify if resschedexestub ReportRequestInner is success.
 * @tc.type: FUNC
 */
HWTEST_F(ResSchedExeServiceTest, ReportRequestInner001, Function | MediumTest | Level0)
{
    auto resSchedExeServiceStub_ = make_shared<TestResSchedExeServiceStub>();
    resSchedExeServiceStub_->Init();
    MessageParcel reply;
    MessageParcel emptyData;
    EXPECT_TRUE(resSchedExeServiceStub_->ReportRequestInner(emptyData, reply));

    MessageParcel reportData;
    reportData.WriteUint32(1);
    reportData.WriteInt64(1);
    reportData.WriteString("{ { \" uid \" : \" 1 \" } }");
    EXPECT_TRUE(!resSchedExeServiceStub_->ReportRequestInner(reportData, reply));
}

static void ReportRequestInnerTask()
{
    auto resSchedExeServiceStub_ = make_shared<TestResSchedExeServiceStub>();
    resSchedExeServiceStub_->Init();
    MessageParcel reply;
    for (int i = 0; i < SYNC_THREAD_NUM; i++) {
        MessageParcel emptyData;
        EXPECT_TRUE(resSchedExeServiceStub_->ReportRequestInner(emptyData, reply));

        MessageParcel reportData;
        reportData.WriteUint32(1);
        reportData.WriteInt64(1);
        reportData.WriteString("{ { \" uid \" : \" 1 \" } }");
        EXPECT_TRUE(!resSchedExeServiceStub_->ReportRequestInner(reportData, reply));
        usleep(SYNC_INTERNAL_TIME);
    }
}

/**
 * @tc.name: ResSchedExeServicesStub ReportRequestInner002
 * @tc.desc: Test resschedexestub ReportRequestInner in multithreading.
 * @tc.type: FUNC
 */
HWTEST_F(ResSchedExeServiceTest, ReportRequestInner002, Function | MediumTest | Level0)
{
    SET_THREAD_NUM(10);
    GTEST_RUN_TASK(ReportRequestInnerTask);
}

/**
 * @tc.name: ResSchedExeServicesStub ReportDebugInner001
 * @tc.desc: Verify if resschedexestub ReportDebugInner is success.
 * @tc.type: FUNC
 */
HWTEST_F(ResSchedExeServiceTest, ReportDebugInner001, Function | MediumTest | Level0)
{
    auto resSchedExeServiceStub_ = make_shared<TestResSchedExeServiceStub>();
    resSchedExeServiceStub_->Init();
    MessageParcel emptyData;
    EXPECT_TRUE(resSchedExeServiceStub_->ReportDebugInner(emptyData));

    MessageParcel reportData;
    reportData.WriteUint32(ResExeType::RES_TYPE_DEBUG);
    reportData.WriteUint64(ResSchedExeCommonUtils::GetCurrentTimestampUs());
    EXPECT_TRUE(!resSchedExeServiceStub_->ReportDebugInner(reportData));
}

static void ReportDebugInnerTask()
{
    auto resSchedExeServiceStub_ = make_shared<TestResSchedExeServiceStub>();
    resSchedExeServiceStub_->Init();
    for (int i = 0; i < SYNC_THREAD_NUM; i++) {
        MessageParcel emptyData;
        EXPECT_TRUE(resSchedExeServiceStub_->ReportDebugInner(emptyData));

        MessageParcel reportData;
        reportData.WriteUint32(ResExeType::RES_TYPE_DEBUG);
        reportData.WriteUint64(ResSchedExeCommonUtils::GetCurrentTimestampUs());
        EXPECT_TRUE(!resSchedExeServiceStub_->ReportDebugInner(reportData));
        usleep(SYNC_INTERNAL_TIME);
    }
}

/**
 * @tc.name: ResSchedExeServicesStub ReportDebugInner002
 * @tc.desc: Test resschedexestub ReportDebugInner in multithreading.
 * @tc.type: FUNC
 */
HWTEST_F(ResSchedExeServiceTest, ReportDebugInner002, Function | MediumTest | Level0)
{
    SET_THREAD_NUM(10);
    GTEST_RUN_TASK(ReportDebugInnerTask);
}

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

/**
 * @tc.name: ResSchedExeServicesStub ParseParcel001
 * @tc.desc: Verify if resschedexestub ParseParcel is success.
 * @tc.type: FUNC
 */
HWTEST_F(ResSchedExeServiceTest, ParseParcel001, Function | MediumTest | Level0)
{
    auto resSchedExeServiceStub_ = make_shared<TestResSchedExeServiceStub>();
    resSchedExeServiceStub_->Init();
    uint32_t resType = 0;
    int64_t value = 0;
    nlohmann::json context;
    MessageParcel emptyData;
    EXPECT_FALSE(resSchedExeServiceStub_->ParseParcel(emptyData, resType, value, context));

    MessageParcel reportData;
    reportData.WriteUint32(1);
    reportData.WriteInt64(1);
    reportData.WriteString("{ { \" uid \" : \" 1 \" } }");
    EXPECT_TRUE(resSchedExeServiceStub_->ParseParcel(reportData, resType, value, context));
}

static void ParseParcelTask()
{
    auto resSchedExeServiceStub_ = make_shared<TestResSchedExeServiceStub>();
    resSchedExeServiceStub_->Init();
    uint32_t resType = 0;
    int64_t value = 0;
    nlohmann::json context;
    for (int i = 0; i < SYNC_THREAD_NUM; i++) {
        MessageParcel emptyData;
        EXPECT_FALSE(resSchedExeServiceStub_->ParseParcel(emptyData, resType, value, context));

        MessageParcel reportData;
        reportData.WriteUint32(1);
        reportData.WriteInt64(1);
        reportData.WriteString("{ { \" uid \" : \" 1 \" } }");
        EXPECT_TRUE(resSchedExeServiceStub_->ParseParcel(reportData, resType, value, context));
        usleep(SYNC_INTERNAL_TIME);
    }
}

/**
 * @tc.name: ResSchedExeServicesStub ParseParcel002
 * @tc.desc: Test resschedexestub ParseParcel in multithreading.
 * @tc.type: FUNC
 */
HWTEST_F(ResSchedExeServiceTest, ParseParcel002, Function | MediumTest | Level0)
{
    SET_THREAD_NUM(10);
    GTEST_RUN_TASK(ParseParcelTask);
}
} // namespace ResourceSchedule
} // namespace OHOS
