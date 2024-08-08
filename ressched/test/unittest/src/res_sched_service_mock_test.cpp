/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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
#include <thread>

#include <vector>
#include "accesstoken_kit.h"
#include "ipc_skeleton.h"
#include "nativetoken_kit.h"
#include "notifier_mgr.h"
#include "plugin_mgr.h"
#include "res_sched_ipc_interface_code.h"
#include "res_sched_common_death_recipient.h"
#include "res_sched_service.h"
#include "res_sched_service_ability.h"
#include "res_sched_systemload_notifier_proxy.h"
#include "res_sched_systemload_notifier_stub.h"
#include "res_type.h"
#include "token_setproc.h"

namespace OHOS {
namespace system {
int32_t g_mockEngMode = 1;
template<typename T>
T GetIntParameter(const std::string& key, T def)
{
    return g_mockEngMode;
}
}
namespace Security::AccessToken {
int32_t g_mockDumpTokenKit = 1;
int32_t g_mockReportTokenKit = 1;
int AccessTokenKit::VerifyAccessToken(AccessTokenID tokenId, const std::string& permissionName)
{
    if (permissionName == "ohos.permission.DUMP") {
        return g_mockDumpTokenKit;
    }
    if (permissionName == "ohos.permission.REPORT_RESOURCE_SCHEDULE_EVENT") {
        return g_mockReportTokenKit;
    }
    return PermissionState::PERMISSION_GRANTED;
}

ATokenTypeEnum g_mockTokenFlag = TypeATokenTypeEnum::TOKEN_INVALID;
ATokenTypeEnum AccessTokenKit::GetTokenTypeFlag(AccessTokenID tokenId)
{
    return g_mockTokenFlag;
}

bool g_mockHapTokenInfo = false;
int AccessTokenKit::GetHapTokenInfo(AccessTokenID tokenId, HapTokenInfo& hapTokenInfoRes)
{
    if (g_mockHapTokenInfo) {
        hapTokenInfoRes.bundleName = "com.ohos.sceneboard";
    }
    return 1;
}
}
bool g_mockAddAbilityListener = true;
bool SystemAbility::AddSystemAbilityListener(int32_t systemAbilityId)
{
    return g_mockAddAbilityListener;
}
int g_mockUid = 0;
int IPCSkeleton::GetCallingUid()
{
    return g_mockUid;
}
namespace ResourceSchedule {
using namespace std;
using namespace testing::ext;
using namespace testing::mt;
using namespace Security::AccessToken;

class TestMockResSchedServiceStub : public ResSchedServiceStub {
public:
    TestMockResSchedServiceStub() : ResSchedServiceStub() {}

    void ReportData(uint32_t restype, int64_t value, const nlohmann::json& payload) override
    {
    }

    int32_t ReportSyncEvent(const uint32_t resType, const int64_t value, const nlohmann::json& payload,
        nlohmann::json& reply) override
    {
        return 0;
    }

    int32_t KillProcess(const nlohmann::json& payload) override
    {
        return 0;
    }

    void RegisterSystemloadNotifier(const sptr<IRemoteObject>& notifier) override
    {
    }

    void UnRegisterSystemloadNotifier() override
    {
    }

    void RegisterEventListener(const sptr<IRemoteObject>& listener, uint32_t eventType) override
    {
    }

    void UnRegisterEventListener(uint32_t eventType) override
    {
    }

    int32_t GetSystemloadLevel() override
    {
        return 0;
    }

    bool IsAllowedAppPreload(const std::string& bundleName, int32_t preloadMode) override
    {
        return true;
    }
};

class ResSchedServiceMockTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
protected:
    std::shared_ptr<ResSchedService> resSchedService_ = nullptr;
    std::shared_ptr<ResSchedServiceAbility> resSchedServiceAbility_ = nullptr;
    std::shared_ptr<TestMockResSchedServiceStub> resSchedServiceStub_ = nullptr;
};

class TestMockResSchedSystemloadListener : public ResSchedSystemloadNotifierStub {
public:
    TestMockResSchedSystemloadListener() = default;

    void OnSystemloadLevel(int32_t level)
    {
        testSystemloadLevel = level;
    }

    static int32_t testSystemloadLevel;
};

int32_t TestMockResSchedSystemloadListener::testSystemloadLevel = 0;

void ResSchedServiceMockTest::SetUpTestCase(void)
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
        .processName = "ResSchedServiceMockTest",
        .aplStr = "system_core",
    };
    tokenId = GetAccessTokenId(&infoInstance);
    SetSelfTokenID(tokenId);
    AccessTokenKit::ReloadNativeTokenInfo();
}

void ResSchedServiceMockTest::TearDownTestCase()
{
    int64_t sleepTime = 10;
    std::this_thread::sleep_for(std::chrono::seconds(sleepTime));
}

void ResSchedServiceMockTest::SetUp()
{
    /**
     * @tc.setup: initialize the member variable resSchedServiceAbility_
     */
    resSchedService_ = make_shared<ResSchedService>();
    resSchedServiceAbility_ = make_shared<ResSchedServiceAbility>();
    resSchedServiceStub_ = make_shared<TestMockResSchedServiceStub>();
}

void ResSchedServiceMockTest::TearDown()
{
    /**
     * @tc.teardown: clear resSchedServiceAbility_
     */
    resSchedService_ = nullptr;
    resSchedServiceAbility_ = nullptr;
    resSchedServiceStub_ = nullptr;
}

/**
 * @tc.name: ressched service dump 001
 * @tc.desc: Verify if ressched service dump commonds is success.
 * @tc.type: FUNC
 * @tc.require: issuesIAGHOC
 * @tc.author: fengyang
 */
HWTEST_F(ResSchedServiceMockTest, ServiceDump001, Function | MediumTest | Level0)
{
    Security::AccessToken::g_mockDumpTokenKit = 0;
    PluginMgr::GetInstance().Init();
    std::string result;
    resSchedService_->DumpAllInfo(result);
    EXPECT_TRUE(!result.empty());

    result = "";
    resSchedService_->DumpUsage(result);
    EXPECT_TRUE(!result.empty());

    int32_t wrongFd = -1;
    std::vector<std::u16string> argsNull;
    int res = resSchedService_->Dump(wrongFd, argsNull);
    EXPECT_EQ(res, ERR_OK);
    resSchedServiceAbility_->OnStart();
    int32_t correctFd = -1;
    res = resSchedService_->Dump(correctFd, argsNull);

    std::vector<std::u16string> argsHelp = {to_utf16("-h")};
    res = resSchedService_->Dump(correctFd, argsHelp);

    std::vector<std::u16string> argsAll = {to_utf16("-a")};
    res = resSchedService_->Dump(correctFd, argsAll);

    std::vector<std::u16string> argsError = {to_utf16("-e")};
    res = resSchedService_->Dump(correctFd, argsError);

    std::vector<std::u16string> argsPlugin = {to_utf16("-p")};
    res = resSchedService_->Dump(correctFd, argsPlugin);

    std::vector<std::u16string> argsOnePlugin = {to_utf16("-p"), to_utf16("1")};
    res = resSchedService_->Dump(correctFd, argsOnePlugin);

    std::vector<std::u16string> argsOnePlugin1 = {to_utf16("getRunningLockInfo")};
    res = resSchedService_->Dump(correctFd, argsOnePlugin1);

    std::vector<std::u16string> argsOnePlugin2 = {to_utf16("getProcessEventInfo")};
    res = resSchedService_->Dump(correctFd, argsOnePlugin2);

    std::vector<std::u16string> argsOnePlugin3 = {to_utf16("getProcessWindowInfo")};
    res = resSchedService_->Dump(correctFd, argsOnePlugin3);

    std::vector<std::u16string> argsOnePlugin4 = {to_utf16("getSystemloadInfo")};
    res = resSchedService_->Dump(correctFd, argsOnePlugin4);

    std::vector<std::u16string> argsOnePlugin5 = {to_utf16("sendDebugToExecutor")};
    res = resSchedService_->Dump(correctFd, argsOnePlugin5);
}

/**
 * @tc.name: ressched service dump 002
 * @tc.desc: Verify if ressched service dump commonds is success.
 * @tc.type: FUNC
 * @tc.require: issuesIAGHOC
 * @tc.author: fengyang
 */
HWTEST_F(ResSchedServiceMockTest, ServiceDump002, Function | MediumTest | Level0)
{
    Security::AccessToken::g_mockDumpTokenKit = 0;
    resSchedServiceAbility_->OnStart();
    std::shared_ptr<ResSchedService> resSchedService = make_shared<ResSchedService>();
    int32_t correctFd = -1;
    std::vector<std::u16string> argsNull;
    std::vector<std::u16string> argsOnePlugin1 = {to_utf16("getRunningLockInfo")};
    int res = resSchedService->Dump(correctFd, argsOnePlugin1);
    EXPECT_EQ(res, ERR_OK);
    std::vector<std::u16string> argsOnePlugin2 = {to_utf16("getProcessEventInfo")};
    res = resSchedService->Dump(correctFd, argsOnePlugin2);
    EXPECT_EQ(res, ERR_OK);
    std::vector<std::u16string> argsOnePlugin3 = {to_utf16("getProcessWindowInfo")};
    res = resSchedService->Dump(correctFd, argsOnePlugin3);
    EXPECT_EQ(res, ERR_OK);
}

/**
 * @tc.name: ressched service dump 003
 * @tc.desc: Verify if ressched service dump commonds is success.
 * @tc.type: FUNC
 * @tc.require: issuesIAGHOC
 * @tc.author: fengyang
 */
HWTEST_F(ResSchedServiceMockTest, ServiceDump003, Function | MediumTest | Level0)
{
    Security::AccessToken::g_mockDumpTokenKit = 0;
    PluginMgr::GetInstance().Init();
    auto notifier = new (std::nothrow) TestMockResSchedSystemloadListener();
    EXPECT_TRUE(notifier != nullptr);
    resSchedService_->RegisterSystemloadNotifier(notifier);
    int32_t correctFd = -1;
    std::vector<std::u16string> argsOnePlugin = {to_utf16("getSystemloadInfo")};
    int res = resSchedService_->Dump(correctFd, argsOnePlugin);
    EXPECT_EQ(res, ERR_OK);
    std::vector<std::u16string> argsOnePlugin2 = {to_utf16("sendDebugToExecutor"), to_utf16("1")};
    res = resSchedService_->Dump(correctFd, argsOnePlugin2);
    EXPECT_EQ(res, ERR_OK);
}

/**
 * @tc.name: ressched service dump 004
 * @tc.desc: Verify if ressched service dump commonds is success.
 * @tc.type: FUNC
 * @tc.require: issuesIAGHOC
 * @tc.author: fengyang
 */
HWTEST_F(ResSchedServiceMockTest, ServiceDump004, Function | MediumTest | Level0)
{
    Security::AccessToken::g_mockDumpTokenKit = 1;
    int32_t correctFd = -1;
    std::vector<std::u16string> argsOnePlugin = {to_utf16("-h")};
    int res = resSchedService_->Dump(correctFd, argsOnePlugin);
    EXPECT_NE(res, ERR_OK);
    system::g_mockEngMode = 0;
    res = resSchedService_->Dump(correctFd, argsOnePlugin);
    EXPECT_NE(res, ERR_OK);
}

/**
 * @tc.name: ressched service dump 005
 * @tc.desc: Verify if ressched service dump commonds is success.
 * @tc.type: FUNC
 * @tc.require: issuesIAGHOC
 * @tc.author: fengyang
 */
HWTEST_F(ResSchedServiceMockTest, ServiceDump005, Function | MediumTest | Level0)
{
    std::vector<std::string> args;
    std::string result;
    resSchedService_->DumpSystemLoadInfo(result);
    EXPECT_NE(result, "");
    resSchedService_->DumpExecutorDebugCommand(args, result);
    EXPECT_NE(result, "");
    resSchedService_->DumpAllPluginConfig(result);
    EXPECT_NE(result, "");
}

/**
 * @tc.name: Start ResSchedServiceAbility 001
 * @tc.desc: Verify if ResSchedServiceAbility OnStart is success.
 * @tc.type: FUNC
 * @tc.require: issuesIAGHOC
 * @tc.author: fengyang
 */
HWTEST_F(ResSchedServiceMockTest, OnStart001, Function | MediumTest | Level0)
{
    g_mockAddAbilityListener = false;
    resSchedServiceAbility_->OnStart();
    EXPECT_TRUE(resSchedServiceAbility_->service_ != nullptr);
    std::string action = "test";
    resSchedServiceAbility_->OnDeviceLevelChanged(0, 2, action);
    g_mockAddAbilityListener = true;
}

/**
 * @tc.name: ResSchedServicesStub ReportDataInner 001
 * @tc.desc: Verify if resschedstub reportdatainner is success.
 * @tc.type: FUNC
 * @tc.require: issuesIAGHOC
 * @tc.author: fengyang
 */
HWTEST_F(ResSchedServiceMockTest, ReportDataInner001, Function | MediumTest | Level0)
{
    Security::AccessToken::g_mockReportTokenKit = 0;
    Security::AccessToken::g_mockTokenFlag = TypeATokenTypeEnum::TOKEN_NATIVE;
    Security::AccessToken::g_mockHapTokenInfo = true;
    resSchedServiceStub_->Init();
    MessageParcel reply;
    MessageParcel emptyData;
    EXPECT_NE(resSchedServiceStub_->ReportDataInner(emptyData, reply), ERR_OK);
    MessageParcel reportData;
    reportData.WriteInterfaceToken(resSchedServiceStub_->GetDescriptor());
    reportData.WriteUint32(1);
    reportData.WriteInt64(1);
    reportData.WriteString("{ { \" uid \" : \" 1 \" } }");
    EXPECT_EQ(resSchedServiceStub_->ReportDataInner(reportData, reply), ERR_OK);
    MessageParcel reportData2;
    reportData2.WriteInterfaceToken(resSchedServiceStub_->GetDescriptor());
    reportData2.WriteUint32(-1);
    reportData2.WriteInt64(1);
    reportData2.WriteString("{ { \" uid \" : \" 1 \" } }");
    EXPECT_NE(resSchedServiceStub_->ReportDataInner(reportData2, reply), ERR_OK);
}

/**
 * @tc.name: ResSchedServicesStub ReportDataInner002
 * @tc.desc: ReportDataInner002 IsSBDResType
 * @tc.type: FUNC
 * @tc.require: issuesIAGHOC
 * @tc.author: fengyang
 */
HWTEST_F(ResSchedServiceMockTest, ReportDataInner002, Function | MediumTest | Level0)
{
    Security::AccessToken::g_mockTokenFlag = TypeATokenTypeEnum::TOKEN_HAP;
    Security::AccessToken::g_mockHapTokenInfo = true;
    MessageParcel reportData;
    MessageParcel reply;
    reportData.WriteInterfaceToken(resSchedServiceStub_->GetDescriptor());
    reportData.WriteUint32(38);
    reportData.WriteInt64(1);
    reportData.WriteString("{ { \" uid \" : \" 1 \" } }");
    EXPECT_EQ(resSchedServiceStub_->ReportDataInner(reportData, reply), ERR_OK);
    Security::AccessToken::g_mockHapTokenInfo = false;
    EXPECT_NE(resSchedServiceStub_->ReportDataInner(reportData, reply), ERR_OK);
    Security::AccessToken::g_mockTokenFlag = TypeATokenTypeEnum::TOKEN_INVALID;
    EXPECT_NE(resSchedServiceStub_->ReportDataInner(reportData, reply), ERR_OK);
}

/**
 * @tc.name: ResSchedServicesStub ReportDataInner 003
 * @tc.desc: ReportDataInner003 IsThirdPartType
 * @tc.type: FUNC
 * @tc.require: issuesIAGHOC
 * @tc.author: fengyang
 */
HWTEST_F(ResSchedServiceMockTest, ReportDataInner003, Function | MediumTest | Level0)
{
    Security::AccessToken::g_mockReportTokenKit = 0;
    Security::AccessToken::g_mockTokenFlag = TypeATokenTypeEnum::TOKEN_HAP;
    MessageParcel reportData;
    MessageParcel reply;
    reportData.WriteInterfaceToken(resSchedServiceStub_->GetDescriptor());
    reportData.WriteUint32(9);
    reportData.WriteInt64(1);
    reportData.WriteString("{ { \" uid \" : \" 1 \" } }");
    EXPECT_EQ(resSchedServiceStub_->ReportDataInner(reportData, reply), ERR_OK);
    Security::AccessToken::g_mockTokenFlag = TypeATokenTypeEnum::TOKEN_INVALID;
    EXPECT_NE(resSchedServiceStub_->ReportDataInner(reportData, reply), ERR_OK);
}

/**
 * @tc.name: ResSchedServicesStub ReportDataInner004
 * @tc.desc: ReportDataInner IsHasPermission
 * @tc.type: FUNC
 * @tc.require: issuesIAGHOC
 * @tc.author: fengyang
 */
HWTEST_F(ResSchedServiceMockTest, ReportDataInner004, Function | MediumTest | Level0)
{
    Security::AccessToken::g_mockReportTokenKit = 0;
    Security::AccessToken::g_mockTokenFlag = TypeATokenTypeEnum::TOKEN_NATIVE;
    MessageParcel reportData;
    MessageParcel reply;
    reportData.WriteInterfaceToken(resSchedServiceStub_->GetDescriptor());
    reportData.WriteUint32(0);
    reportData.WriteInt64(1);
    reportData.WriteString("{ { \" uid \" : \" 1 \" } }");
    EXPECT_EQ(resSchedServiceStub_->ReportDataInner(reportData, reply), ERR_OK);
    Security::AccessToken::g_mockDumpTokenKit = 1;
    EXPECT_NE(resSchedServiceStub_->ReportDataInner(reportData, reply), ERR_OK);
    Security::AccessToken::g_mockTokenFlag = TypeATokenTypeEnum::TOKEN_INVALID;
    EXPECT_NE(resSchedServiceStub_->ReportDataInner(reportData, reply), ERR_OK);
}

/**
 * @tc.name: ResSchedServicesStub ReportSyncEventInner001
 * @tc.desc: ReportSyncEventInner
 * @tc.type: FUNC
 * @tc.require: issuesIAGHOC
 * @tc.author: fengyang
 */
HWTEST_F(ResSchedServiceMockTest, ReportSyncEventInner001, Function | MediumTest | Level0)
{
    Security::AccessToken::g_mockReportTokenKit = 0;
    Security::AccessToken::g_mockTokenFlag = TypeATokenTypeEnum::TOKEN_NATIVE;
    MessageParcel reportData;
    MessageParcel reply;
    reportData.WriteInterfaceToken(resSchedServiceStub_->GetDescriptor());
    reportData.WriteUint32(0);
    reportData.WriteInt64(1);
    reportData.WriteString("{ { \" uid \" : \" 1 \" } }");
    EXPECT_EQ(resSchedServiceStub_->ReportSyncEventInner(reportData, reply), ERR_OK);
}

/**
 * @tc.name: ResSchedServicesStub KillProcessInner001
 * @tc.desc: KillProcessInner
 * @tc.type: FUNC
 * @tc.require: issuesIAGHOC
 * @tc.author: fengyang
 */
HWTEST_F(ResSchedServiceMockTest, KillProcessInner001, Function | MediumTest | Level0)
{
    Security::AccessToken::g_mockReportTokenKit = 0;
    Security::AccessToken::g_mockTokenFlag = TypeATokenTypeEnum::TOKEN_NATIVE;
    g_mockUid = 1111;
    MessageParcel reportData;
    MessageParcel reply;
    reportData.WriteInterfaceToken(resSchedServiceStub_->GetDescriptor());
    reportData.WriteString("{ { \" uid \" : \" 1 \" } }");
    EXPECT_EQ(resSchedServiceStub_->KillProcessInner(reportData, reply), ERR_OK);
    Security::AccessToken::g_mockTokenFlag = TypeATokenTypeEnum::TOKEN_INVALID;
    EXPECT_NE(resSchedServiceStub_->KillProcessInner(reportData, reply), ERR_OK);
    g_mockUid = 0;
    EXPECT_NE(resSchedServiceStub_->KillProcessInner(reportData, reply), ERR_OK);
}

} // namespace ResourceSchedule
} // namespace OHOS
