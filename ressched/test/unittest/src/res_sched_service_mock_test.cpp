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
#include "sched_controller.h"
#include "supervisor.h"

namespace OHOS {
namespace system {
int32_t g_mockEngMode = 1;
template<typename T>
T GetIntParameter(const std::string& key, T def)
{
    return g_mockEngMode;
}
bool g_mockAddAbilityListener = true;
bool SystemAbility::AddSystemAbilityListener(int32_t systemAbilityId)
{
    return g_mockAddAbilityListener;
}
namespace Security::AccessToken {
int32_t g_mockDumpTokenKit = 1;
int AccessTokenKit::VerifyAccessToken(AccessTokenId tokenId, const std::string& permissionName)
{
    if (permissionName == "ohos.permission.DUMP") {
        return g_mockDumpTokenKit;
    }
    if (permissionName == "ohos.permission.REPORT_RESOURCE_SCHEDULE_EVENT") {
        return g_mockReportTokenKit;
    }
    return PermissionState::PERMISSION_GRANTED;
}

int32_t g_mockTokenFlag = 1;
int AccessTokenKit::GetTokenTypeFlag(AccessTokenId tokenId)
{
    return g_mockTokenFlag;
}

bool g_mockHapTokenInfo = false;
int AccessTokenKit::GetHapTokenInfo(AccessTokenId tokenId, HapTokenInfo& hapTokenInfoRes)
{
    if (g_mockHapTokenInfo) {
        hapTokenInfoRes.bundleName = "com.ohos.sceneboard";
    }
    return 1;
}

}
}
namespace ResourceSchedule {
using namespace std;
using namespace testing::ext;
using namespace testing::mt;
using namespace Security::AccessToken;
class ResSchedServiceTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
protected:
    std::shared_ptr<ResSchedService> resSchedService_ = nullptr;
    std::shared_ptr<ResSchedServiceAbility> resSchedServiceAbility_ = nullptr;
};

class TestResSchedSystemloadListener : public ResSchedSystemloadNotifierStub {
public:
    TestResSchedSystemloadListener() = default;

    void OnSystemloadLevel(int32_t level)
    {
        testSystemloadLevel = level;
    }

    static int32_t testSystemloadLevel;
};

int32_t TestResSchedSystemloadListener::testSystemloadLevel = 0;

void ResSchedServiceTest::SetUpTestCase(void)
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
        .processName = "ResSchedServiceTest",
        .aplStr = "system_core",
    };
    tokenId = GetAccessTokenId(&infoInstance);
    SetSelfTokenID(tokenId);
    AccessTokenKit::ReloadNativeTokenInfo();
}

void ResSchedServiceTest::TearDownTestCase() {}

void ResSchedServiceTest::SetUp()
{
    /**
     * @tc.setup: initialize the member variable resSchedServiceAbility_
     */
    resSchedService_ = make_shared<ResSchedService>();
    resSchedServiceAbility_ = make_shared<ResSchedServiceAbility>();
}

void ResSchedServiceTest::TearDown()
{
    /**
     * @tc.teardown: clear resSchedServiceAbility_
     */
    resSchedService_ = nullptr;
    resSchedServiceAbility_ = nullptr;
}

/**
 * @tc.name: ressched service dump 001
 * @tc.desc: Verify if ressched service dump commonds is success.
 * @tc.type: FUNC
 * @tc.require: issueI5WWV3
 * @tc.author:lice
 */
HWTEST_F(ResSchedServiceTest, ServiceDump001, Function | MediumTest | Level0)
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
    resSchedServiceAbility_->Onstart();
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
 * @tc.require: issueI5WWV3
 * @tc.author:lice
 */
HWTEST_F(ResSchedServiceTest, ServiceDump002, Function | MediumTest | Level0)
{
    Security::AccessToken::g_mockDumpTokenKit = 0；
    resSchedServiceAbility_->Onstart();
    std::shared_ptr<ResSchedService> resSchedService = make_shared<ResSchedService>();
    int32_t uid = 100;
    int32_t pid = 10000;
    auto app = SchedController::GetInstance().GetSupervisor()->GetAppRecordNonNull(uid);
    auto processRecord = app->GetProcessRecordNonNull(pid);
    processRecord->GetAbilityInfoNonNull(1000);
    processRecord->GetWindowInfoNonNull(1000);
    processRecord->runningLockState_(1000) = false;
    int32_t wrongFd = -1;
    int32_t correctFd = -1;
    std::vector<std::u16string> argsNull;
    std::vector<std::u16string> argsOnePlugin1 = {to_utf16("getRunningLockInfo")};
    res = resSchedService->Dump(correctFd, argsOnePlugin1);
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
 * @tc.require: issueI5WWV3
 * @tc.author:lice
 */
HWTEST_F(ResSchedServiceTest, ServiceDump003, Function | MediumTest | Level0)
{
    Security::AccessToken::g_mockDumpTokenKit = 0；
    PluginMgr::GetInstance().Init();
    auto notifier = new (std::nothrow) TestResSchedSystemloadListener();
    EXPECT_TRUE(notifier != nullptr);
    resSchedService_->RegisterSystemLoadNotifier(notifier);
    std::vector<std::u16string> argsOnePlugin = {to_utf16("getSystemloadInfo")};
    res = resSchedService_->Dump(correctFd, argsOnePlugin);
    EXPECT_EQ(res, ERR_OK);
    td::vector<std::u16string> argsOnePlugi2 = {to_utf16("sendDebugToExecutor"), to_utf16("1")};
    res = resSchedService_->Dump(correctFd, argsOnePlugi2);
    EXPECT_EQ(res, ERR_OK);
}

/**
 * @tc.name: ressched service dump 004
 * @tc.desc: Verify if ressched service dump commonds is success.
 * @tc.type: FUNC
 * @tc.require: issueI5WWV3
 * @tc.author:lice
 */
HWTEST_F(ResSchedServiceTest, ServiceDump004, Function | MediumTest | Level0)
{
    Security::AccessToken::g_mockDumpTokenKit = 1；
    std::shared_ptr<ResSchedService> resSchedService = make_shared<ResSchedService>();
    int32_t wrongFd = -1;
    std:;vector<std::u16string> argsNull;
    res = resSchedService->Dump(correctFd, argsOnePlugi2);
    EXPECT_NE(res, ERR_OK);
    system::g_mockEngMode = 0;
    res = resSchedService->Dump(correctFd, argsOnePlugi2);
    EXPECT_NE(res, ERR_OK);
}

/**
 * @tc.name: ressched service dump 005
 * @tc.desc: Verify if ressched service dump commonds is success.
 * @tc.type: FUNC
 * @tc.require: issueI5WWV3
 * @tc.author:lice
 */
HWTEST_F(ResSchedServiceTest, ServiceDump005, Function | MediumTest | Level0)
{
    std::shared_ptr<ResSchedService> resSchedService = make_shared<ResSchedService>();
    std::string result;
    resSchedService->DumpProcessRunningLock(result);
    EXPECT_NE(result, "");
    resSchedService->DumpProcessWindowInfo(result);
    EXPECT_NE(result, "");
    resSchedService->DumpProcessEventState(result);
    EXPECT_NE(result, "");
    resSchedService->DumpSystemLoadInfo(result);
    EXPECT_NE(result, "");
    resSchedService->DumpExecutorDebugCommand(result);
    EXPECT_NE(result, "");
    resSchedService->DumpAllPluginConfig(result);
    EXPECT_NE(result, "");
}

class TestResSchedServiceStub : public ResSchedServiceStub {
public:
    TestResSchedServiceStub() : ResSchedServiceStub() {}

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

/**
 * @tc.name: ResSchedServicesStub IsHasPermission 001
 * @tc.desc: IsHasPermission
 * @tc.type: FUNC
 * @tc.require: issueI9U0YF
 * @tc.author:fengyang
 */
HWTEST_F(ResSchedServiceTest, IsHasPermission001, Function | MediumTest | Level0)
{
    Security::AccessToken::g_mockDumpTokenKit = 0；
    Security::AccessToken::g_mockTokenFlag = 0；
    auto resSchedServiceStub = make_shared<TestResSchedServiceStub>();
    resSchedServiceStub_->Init();
    uint32_t type = 0;
    std::unordered_set<uint32_t> saRes;
    saRes.insert(0);
    EXPECT_TRUE(resSchedServiceStub->IsHasPermission(type, saRes));
    EXPECT_FALSE(resSchedServiceStub->IsHasPermission(1, saRes));
    Security::AccessToken::g_mockDumpTokenKit = 1；
    EXPECT_FALSE(resSchedServiceStub->IsHasPermission(0, saRes));
    Security::AccessToken::g_mockTokenFlag = 1；
    EXPECT_FALSE(resSchedServiceStub->IsHasPermission(type, saRes));
}

/**
 * @tc.name: ResSchedServicesStub IsThirdPartType 001
 * @tc.desc: IsThirdPartType
 * @tc.type: FUNC
 * @tc.require: issueI9U0YF
 * @tc.author:fengyang
 */
HWTEST_F(ResSchedServiceTest, IsThirdPartType001, Function | MediumTest | Level0)
{
    Security::AccessToken::g_mockReportTokenKit = 0；
    Security::AccessToken::g_mockTokenFlag = 0；
    auto resSchedServiceStub = make_shared<TestResSchedServiceStub>();
    resSchedServiceStub_->Init();
    uint32_t type = 0;
    std::unordered_set<uint32_t> thirdPartRes;
    thirdPartRes.insert(0);
    EXPECT_TRUE(resSchedServiceStub->IsThirdPartType(type, saRes));
    EXPECT_FALSE(resSchedServiceStub->IsThirdPartType(1, saRes));
    Security::AccessToken::g_mockTokenFlag = 1；
    EXPECT_FALSE(resSchedServiceStub->IsThirdPartType(type, saRes));
}

/**
 * @tc.name: ResSchedServicesStub IsSBDResType 001
 * @tc.desc: IsSBDResType
 * @tc.type: FUNC
 * @tc.require: issueI9U0YF
 * @tc.author:fengyang
 */
HWTEST_F(ResSchedServiceTest, IsSBDResType001, Function | MediumTest | Level0)
{
    Security::AccessToken::g_mockTokenFlag = 0；
    Security::AccessToken::g_mockHapTokenInfo = true;
    auto resSchedServiceStub = make_shared<TestResSchedServiceStub>();
    resSchedServiceStub_->Init();
    uint32_t type = 38;
    EXPECT_FALSE(resSchedServiceStub->IsSBDResType(0));
    EXPECT_TRUE(resSchedServiceStub->IsSBDResType(type));
    Security::AccessToken::g_mockTokenFlag = 1；
    EXPECT_FALSE(resSchedServiceStub->IsSBDResType(type));
    Security::AccessToken::g_mockHapTokenInfo = false;
    EXPECT_FALSE(resSchedServiceStub->IsSBDResType(type));
}
} // namespace ResourceSchedule
} // namespace OHOS
