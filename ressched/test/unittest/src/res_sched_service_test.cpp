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
#include "res_common_util.h"
#include "res_sched_ipc_interface_code.h"
#include "res_sched_common_death_recipient.h"
#include "res_sched_service.h"
#include "res_sched_service_ability.h"
#include "res_sched_systemload_notifier_proxy.h"
#include "res_sched_systemload_notifier_stub.h"
#include "res_type.h"
#include "token_setproc.h"

namespace OHOS {
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
    EXPECT_NE(res, ERR_OK);

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
 * @tc.name: Ressched service ReportData 001
 * @tc.desc: Verify if Ressched service ReportData is success.
 * @tc.type: FUNC
 * @tc.require: issueI5WWV3
 * @tc.author:lice
 */
HWTEST_F(ResSchedServiceTest, Report001, Function | MediumTest | Level0)
{
    nlohmann::json payload;
    EXPECT_TRUE(resSchedService_ != nullptr);
    resSchedService_->ReportData(0, 0, payload);
}

/**
 * @tc.name: ReportSyncEvent
 * @tc.desc: test func ReportSyncEvent.
 * @tc.type: FUNC
 * @tc.require: I9QN9E
 */
HWTEST_F(ResSchedServiceTest, ReportSyncEvent, Function | MediumTest | Level0)
{
    EXPECT_NE(resSchedService_, nullptr);
    nlohmann::json payload({{"pid", 100}});
    nlohmann::json reply;
    int32_t ret = resSchedService_->ReportSyncEvent(ResType::SYNC_RES_TYPE_THAW_ONE_APP, 0, payload, reply);
    // 事件分发失败，返回err
    EXPECT_NE(ret, 0);
}

static void ReportTask()
{
    std::shared_ptr<ResSchedService> resSchedService_ = make_shared<ResSchedService>();
    nlohmann::json payload;
    EXPECT_TRUE(resSchedService_ != nullptr);
    resSchedService_->ReportData(0, 0, payload);
}

/**
 * @tc.name: Ressched service ReportData 002
 * @tc.desc: Test Ressched service ReportData in multithreading.
 * @tc.type: FUNC
 * @tc.require: issueI7G8VT
 * @tc.author: nizihao
 */
HWTEST_F(ResSchedServiceTest, Report002, Function | MediumTest | Level0)
{
    SET_THREAD_NUM(10);
    GTEST_RUN_TASK(ReportTask);
}

/**
 * @tc.name: Ressched service KillProcess 001
 * @tc.desc: test the interface service KillProcess
 * @tc.type: FUNC
 * @tc.require: issueI8VZVN
 * @tc.author:z30053169
 */
HWTEST_F(ResSchedServiceTest, KillProcess001, Function | MediumTest | Level0)
{
    nlohmann::json payload;
    int32_t t = resSchedService_->KillProcess(payload);
    EXPECT_EQ(t, -1);
}

/**
 * @tc.name: Ressched service TestResSchedSystemloadListener 001
 * @tc.desc: test the interface service TestResSchedSystemloadListener
 * @tc.type: FUNC
 * @tc.require: issueI97M6C
 * @tc.author:shanhaiyang
 */
HWTEST_F(ResSchedServiceTest, TestResSchedSystemloadListener001, Function | MediumTest | Level0)
{
    std::shared_ptr<ResSchedService> resSchedService_ = make_shared<ResSchedService>();
    EXPECT_TRUE(resSchedService_ != nullptr);
    sptr<IRemoteObject> notifier = new (std::nothrow) TestResSchedSystemloadListener();
    EXPECT_TRUE(notifier != nullptr);
    NotifierMgr::GetInstance().Init();
    resSchedService_->RegisterSystemloadNotifier(notifier);
    NotifierMgr::GetInstance().OnApplicationStateChange(2, IPCSkeleton::GetCallingPid());
    resSchedService_->OnDeviceLevelChanged(0, 2);
    sleep(1);
    EXPECT_TRUE(TestResSchedSystemloadListener::testSystemloadLevel == 2);
    resSchedService_->UnRegisterSystemloadNotifier();
    NotifierMgr::GetInstance().OnApplicationStateChange(4, IPCSkeleton::GetCallingPid());
    TestResSchedSystemloadListener::testSystemloadLevel = 0;
}

/**
 * @tc.name: Ressched service TestResSchedSystemloadListener 002
 * @tc.desc: test the interface service TestResSchedSystemloadListener
 * @tc.type: FUNC
 * @tc.require: issueI97M6C
 * @tc.author:shanhaiyang
 */
HWTEST_F(ResSchedServiceTest, TestResSchedSystemloadListener002, Function | MediumTest | Level0)
{
    std::shared_ptr<ResSchedService> resSchedService_ = make_shared<ResSchedService>();
    EXPECT_TRUE(resSchedService_ != nullptr);
    sptr<IRemoteObject> notifier = new (std::nothrow) TestResSchedSystemloadListener();
    EXPECT_TRUE(notifier != nullptr);
    NotifierMgr::GetInstance().Init();
    resSchedService_->RegisterSystemloadNotifier(notifier);
    resSchedService_->OnDeviceLevelChanged(0, 2);
    sleep(1);
    EXPECT_TRUE(TestResSchedSystemloadListener::testSystemloadLevel == 2);
    resSchedService_->UnRegisterSystemloadNotifier();
    TestResSchedSystemloadListener::testSystemloadLevel = 0;
}

/**
 * @tc.name: Ressched service TestResSchedSystemloadListener 003
 * @tc.desc: test the interface service TestResSchedSystemloadListener
 * @tc.type: FUNC
 * @tc.require: issueI97M6C
 * @tc.author:shanhaiyang
 */
HWTEST_F(ResSchedServiceTest, TestResSchedSystemloadListener003, Function | MediumTest | Level0)
{
    std::shared_ptr<ResSchedService> resSchedService_ = make_shared<ResSchedService>();
    EXPECT_TRUE(resSchedService_ != nullptr);
    sptr<IRemoteObject> notifier = new (std::nothrow) TestResSchedSystemloadListener();
    EXPECT_TRUE(notifier != nullptr);
    NotifierMgr::GetInstance().Init();
    resSchedService_->RegisterSystemloadNotifier(notifier);
    NotifierMgr::GetInstance().OnApplicationStateChange(2, 111111);
    resSchedService_->OnDeviceLevelChanged(0, 2);
    sleep(1);
    EXPECT_TRUE(TestResSchedSystemloadListener::testSystemloadLevel == 2);
    resSchedService_->UnRegisterSystemloadNotifier();
    NotifierMgr::GetInstance().OnApplicationStateChange(4, 111111);
    TestResSchedSystemloadListener::testSystemloadLevel = 0;
}

/**
 * @tc.name: Ressched service TestResSchedSystemloadListener 004
 * @tc.desc: test the interface service TestResSchedSystemloadListener
 * @tc.type: FUNC
 * @tc.require: issueI97M6C
 * @tc.author:shanhaiyang
 */
HWTEST_F(ResSchedServiceTest, TestResSchedSystemloadListener004, Function | MediumTest | Level0)
{
    std::shared_ptr<ResSchedService> resSchedService_ = make_shared<ResSchedService>();
    EXPECT_TRUE(resSchedService_ != nullptr);
    sptr<IRemoteObject> notifier = new (std::nothrow) TestResSchedSystemloadListener();
    EXPECT_TRUE(notifier != nullptr);
    NotifierMgr::GetInstance().Init();
    std::string cbType = "systemLoadChange";
    resSchedService_->RegisterSystemloadNotifier(notifier);
    NotifierMgr::GetInstance().OnApplicationStateChange(2, IPCSkeleton::GetCallingPid());
    NotifierMgr::GetInstance().OnRemoteNotifierDied(notifier);
    resSchedService_->OnDeviceLevelChanged(0, 2);
    sleep(1);
    EXPECT_TRUE(TestResSchedSystemloadListener::testSystemloadLevel == 0);
    resSchedService_->UnRegisterSystemloadNotifier();
    NotifierMgr::GetInstance().OnApplicationStateChange(4, IPCSkeleton::GetCallingPid());
    TestResSchedSystemloadListener::testSystemloadLevel = 0;
}

/**
 * @tc.name: Ressched service RegisterSystemloadNotifier 001
 * @tc.desc: test the interface service RegisterSystemloadNotifier
 * @tc.type: FUNC
 * @tc.require: issueI97M6C
 * @tc.author:shanhaiyang
 */
HWTEST_F(ResSchedServiceTest, RegisterSystemloadNotifier001, Function | MediumTest | Level0)
{
    std::shared_ptr<ResSchedService> resSchedService_ = make_shared<ResSchedService>();
    EXPECT_TRUE(resSchedService_ != nullptr);
    sptr<IRemoteObject> notifier = new (std::nothrow) TestResSchedSystemloadListener();
    EXPECT_TRUE(notifier != nullptr);
    NotifierMgr::GetInstance().Init();
    resSchedService_->RegisterSystemloadNotifier(notifier);
}

/**
 * @tc.name: Ressched service RegisterSystemloadNotifier 002
 * @tc.desc: test the interface service RegisterSystemloadNotifier
 * @tc.type: FUNC
 * @tc.require: issueI97M6C
 * @tc.author:shanhaiyang
 */
HWTEST_F(ResSchedServiceTest, RegisterSystemloadNotifier002, Function | MediumTest | Level0)
{
    std::shared_ptr<ResSchedService> resSchedService_ = make_shared<ResSchedService>();
    EXPECT_TRUE(resSchedService_ != nullptr);
    sptr<IRemoteObject> notifier = new (std::nothrow) TestResSchedSystemloadListener();
    EXPECT_TRUE(notifier != nullptr);
    NotifierMgr::GetInstance().Init();
    resSchedService_->RegisterSystemloadNotifier(notifier);
    resSchedService_->RegisterSystemloadNotifier(notifier);
}

/**
 * @tc.name: Ressched service UnRegisterSystemloadNotifier 001
 * @tc.desc: test the interface service UnRegisterSystemloadNotifier
 * @tc.type: FUNC
 * @tc.require: issueI97M6C
 * @tc.author:shanhaiyang
 */
HWTEST_F(ResSchedServiceTest, UnRegisterSystemloadNotifier001, Function | MediumTest | Level0)
{
    std::shared_ptr<ResSchedService> resSchedService_ = make_shared<ResSchedService>();
    EXPECT_TRUE(resSchedService_ != nullptr);
    sptr<IRemoteObject> notifier = new (std::nothrow) TestResSchedSystemloadListener();
    EXPECT_TRUE(notifier != nullptr);
    NotifierMgr::GetInstance().Init();
    resSchedService_->RegisterSystemloadNotifier(notifier);
    resSchedService_->UnRegisterSystemloadNotifier();
}

/**
 * @tc.name: Ressched service UnRegisterSystemloadNotifier 002
 * @tc.desc: test the interface service UnRegisterSystemloadNotifier
 * @tc.type: FUNC
 * @tc.require: issueI97M6C
 * @tc.author:shanhaiyang
 */
HWTEST_F(ResSchedServiceTest, UnRegisterSystemloadNotifier002, Function | MediumTest | Level0)
{
    std::shared_ptr<ResSchedService> resSchedService_ = make_shared<ResSchedService>();
    EXPECT_TRUE(resSchedService_ != nullptr);
    sptr<IRemoteObject> notifier = new (std::nothrow) TestResSchedSystemloadListener();
    EXPECT_TRUE(notifier != nullptr);
    NotifierMgr::GetInstance().Init();
    resSchedService_->UnRegisterSystemloadNotifier();
}

/**
 * @tc.name: Ressched service GetSystemloadLevel 001
 * @tc.desc: test the interface service GetSystemloadLevel
 * @tc.type: FUNC
 * @tc.require: issueI97M6C
 * @tc.author:shanhaiyang
 */
HWTEST_F(ResSchedServiceTest, GetSystemloadLevel001, Function | MediumTest | Level0)
{
    std::shared_ptr<ResSchedService> resSchedService_ = make_shared<ResSchedService>();
    EXPECT_TRUE(resSchedService_ != nullptr);
    sptr<IRemoteObject> notifier = new (std::nothrow) TestResSchedSystemloadListener();
    EXPECT_TRUE(notifier != nullptr);
    NotifierMgr::GetInstance().Init();
    resSchedService_->OnDeviceLevelChanged(0, 0);
    int32_t res = resSchedService_->GetSystemloadLevel();
    EXPECT_TRUE(res == 0);
}

/**
 * @tc.name: Ressched service GetSystemloadLevel 002
 * @tc.desc: test the interface service GetSystemloadLevel
 * @tc.type: FUNC
 * @tc.require: issueI97M6C
 * @tc.author:shanhaiyang
 */
HWTEST_F(ResSchedServiceTest, GetSystemloadLevel002, Function | MediumTest | Level0)
{
    std::shared_ptr<ResSchedService> resSchedService_ = make_shared<ResSchedService>();
    EXPECT_TRUE(resSchedService_ != nullptr);
    sptr<IRemoteObject> notifier = new (std::nothrow) TestResSchedSystemloadListener();
    EXPECT_TRUE(notifier != nullptr);
    NotifierMgr::GetInstance().Init();
    resSchedService_->OnDeviceLevelChanged(0, 2);
    resSchedService_->OnDeviceLevelChanged(1, 5);
    int32_t res = resSchedService_->GetSystemloadLevel();
    EXPECT_TRUE(res == 2);
}

/**
 * @tc.name: Start ResSchedServiceAbility 001
 * @tc.desc: Verify if ResSchedServiceAbility OnStart is success.
 * @tc.type: FUNC
 * @tc.require: issueI5WWV3
 * @tc.author:lice
 */
HWTEST_F(ResSchedServiceTest, OnStart001, Function | MediumTest | Level0)
{
    resSchedServiceAbility_->OnStart();
    EXPECT_TRUE(resSchedServiceAbility_->service_ != nullptr);
}

static void OnStartTask()
{
    std::shared_ptr<ResSchedServiceAbility> resSchedServiceAbility_ = make_shared<ResSchedServiceAbility>();
    resSchedServiceAbility_->OnStart();
    EXPECT_TRUE(resSchedServiceAbility_->service_ != nullptr);
}

/**
 * @tc.name: Start ResSchedServiceAbility 002
 * @tc.desc: Test ResSchedServiceAbility OnStart in multithreading.
 * @tc.type: FUNC
 * @tc.require: issueI7G8VT
 * @tc.author: nizihao
 */
HWTEST_F(ResSchedServiceTest, OnStart002, Function | MediumTest | Level0)
{
    SET_THREAD_NUM(10);
    GTEST_RUN_TASK(OnStartTask);
}

/**
 * @tc.name: ResSchedServiceAbility ChangeAbility 001
 * @tc.desc: Verify if add and remove system ability is success.
 * @tc.type: FUNC
 * @tc.require: issueI5WWV3
 * @tc.author:lice
 */
HWTEST_F(ResSchedServiceTest, ChangeAbility001, Function | MediumTest | Level0)
{
    std::string deviceId;
    int32_t systemAbilityId = -1;
    resSchedServiceAbility_->OnAddSystemAbility(systemAbilityId, deviceId);
    resSchedServiceAbility_->OnRemoveSystemAbility(systemAbilityId, deviceId);
    EXPECT_EQ(systemAbilityId, -1);
}

static void ChangeAbilityTask()
{
    std::shared_ptr<ResSchedServiceAbility> resSchedServiceAbility_ = make_shared<ResSchedServiceAbility>();
    std::string deviceId;
    resSchedServiceAbility_->OnAddSystemAbility(-1, deviceId);
    resSchedServiceAbility_->OnRemoveSystemAbility(-1, deviceId);
}

/**
 * @tc.name: ResSchedServiceAbility ChangeAbility 002
 * @tc.desc: Test add and remove system ability in multithreading.
 * @tc.type: FUNC
 * @tc.require: issueI7G8VT
 * @tc.author: nizihao
 */
HWTEST_F(ResSchedServiceTest, ChangeAbility002, Function | MediumTest | Level0)
{
    SET_THREAD_NUM(10);
    GTEST_RUN_TASK(ChangeAbilityTask);
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

    void RegisterEventListener(const sptr<IRemoteObject>& listener, uint32_t eventType,
        uint32_t listenerGroup = ResType::EventListenerGroup::LISTENER_GROUP_COMMON) override
    {
    }

    void UnRegisterEventListener(uint32_t eventType,
        uint32_t listenerGroup = ResType::EventListenerGroup::LISTENER_GROUP_COMMON) override
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

    int32_t IsAllowedLinkJump(bool& isAllowedLinkJump) override
    {
        return 0;
    }
};

/**
 * @tc.name: ResSchedServicesStub ReportDataInner 001
 * @tc.desc: Verify if resschedstub reportdatainner is success.
 * @tc.type: FUNC
 * @tc.require: issueI5WWV3
 * @tc.author:lice
 */
HWTEST_F(ResSchedServiceTest, ReportDataInner001, Function | MediumTest | Level0)
{
    auto resSchedServiceStub_ = make_shared<TestResSchedServiceStub>();
    resSchedServiceStub_->Init();
    MessageParcel reply;
    MessageParcel emptyData;
    EXPECT_TRUE(resSchedServiceStub_->ReportDataInner(emptyData, reply));

    MessageParcel reportData;
    reportData.WriteInterfaceToken(ResSchedServiceStub::GetDescriptor());
    reportData.WriteUint32(1);
    reportData.WriteInt64(1);
    reportData.WriteString("{ { \" uid \" : \" 1 \" } }");
    SUCCEED();
}

/**
 * @tc.name: ReportSyncEventInner
 * @tc.desc: test func ReportSyncEventInner.
 * @tc.type: FUNC
 * @tc.require: I9QN9E
 */
HWTEST_F(ResSchedServiceTest, ReportSyncEventInner, Function | MediumTest | Level0)
{
    auto serviceStub = make_shared<TestResSchedServiceStub>();
    EXPECT_NE(serviceStub, nullptr);
    serviceStub->Init();
    MessageParcel data;
    data.WriteInterfaceToken(ResSchedServiceStub::GetDescriptor());
    data.WriteUint32(ResType::SYNC_RES_TYPE_THAW_ONE_APP);
    data.WriteInt64(0);
    data.WriteString(R"({"pid": 100})");
    MessageParcel reply;
    EXPECT_NE(serviceStub->ReportSyncEventInner(data, reply), 0);
}

static void ReportDataInnerTask()
{
    auto resSchedServiceStub_ = make_shared<TestResSchedServiceStub>();
    resSchedServiceStub_->Init();
    MessageParcel reply;
    MessageParcel emptyData;
    EXPECT_TRUE(resSchedServiceStub_->ReportDataInner(emptyData, reply));

    MessageParcel reportData;
    reportData.WriteInterfaceToken(ResSchedServiceStub::GetDescriptor());
    reportData.WriteUint32(1);
    reportData.WriteInt64(1);
    reportData.WriteString("{ { \" uid \" : \" 1 \" } }");
    SUCCEED();
}

/**
 * @tc.name: ResSchedServicesStub ReportDataInner 002
 * @tc.desc: Test resschedstub reportdatainner in multithreading.
 * @tc.type: FUNC
 * @tc.require: issueI7G8VT
 * @tc.author: nizihao
 */
HWTEST_F(ResSchedServiceTest, ReportDataInner002, Function | MediumTest | Level0)
{
    SET_THREAD_NUM(10);
    GTEST_RUN_TASK(ReportDataInnerTask);
}

/**
 * @tc.name: ResSchedServicesStub StringToJson 001
 * @tc.desc: Verify if resschedstub StringToJson is success.
 * @tc.type: FUNC
 * @tc.require: issueI5WWV3
 * @tc.author:lice
 */
HWTEST_F(ResSchedServiceTest, StringToJson001, Function | MediumTest | Level0)
{
    auto resSchedServiceStub_ = make_shared<TestResSchedServiceStub>();
    nlohmann::json res = resSchedServiceStub_->StringToJsonObj("");
    EXPECT_TRUE(!res.dump().empty());
}

static void StringToJsonTask()
{
    auto resSchedServiceStub_ = make_shared<TestResSchedServiceStub>();
    nlohmann::json res = resSchedServiceStub_->StringToJsonObj("");
    EXPECT_TRUE(!res.dump().empty());
}

/**
 * @tc.name: ResSchedServicesStub StringToJson 002
 * @tc.desc: Test resschedstub StringToJson in multithreading.
 * @tc.type: FUNC
 * @tc.require: issueI7G8VT
 * @tc.author: nizihao
 */
HWTEST_F(ResSchedServiceTest, StringToJson002, Function | MediumTest | Level0)
{
    SET_THREAD_NUM(10);
    GTEST_RUN_TASK(StringToJsonTask);
}

/**
 * @tc.name: ResSchedServicesStub RemoteRequest 001
 * @tc.desc: Verify if resschedstub RemoteRequest is success.
 * @tc.type: FUNC
 * @tc.require: issueI5WWV3 issueI6D6BM
 * @tc.author:lice
 */
HWTEST_F(ResSchedServiceTest, RemoteRequest001, Function | MediumTest | Level0)
{
    auto resSchedServiceStub_ = make_shared<TestResSchedServiceStub>();
    MessageOption option;
    MessageParcel reply;
    int32_t res = resSchedServiceStub_->OnRemoteRequest(
        static_cast<uint32_t>(ResourceScheduleInterfaceCode::REPORT_DATA), reply, reply, option);
    EXPECT_TRUE(res);
    res = resSchedServiceStub_->OnRemoteRequest(
        static_cast<uint32_t>(ResourceScheduleInterfaceCode::REPORT_SYNC_EVENT), reply, reply, option);
    EXPECT_TRUE(res);
    res = resSchedServiceStub_->OnRemoteRequest(
        static_cast<uint32_t>(ResourceScheduleInterfaceCode::KILL_PROCESS), reply, reply, option);
    EXPECT_TRUE(res);
    res = resSchedServiceStub_->OnRemoteRequest(0, reply, reply, option);
    EXPECT_TRUE(res);
    res = resSchedServiceStub_->OnRemoteRequest(
        static_cast<uint32_t>(ResourceScheduleInterfaceCode::REGISTER_SYSTEMLOAD_NOTIFIER), reply, reply, option);
    EXPECT_TRUE(!res);
    res = resSchedServiceStub_->OnRemoteRequest(
        static_cast<uint32_t>(ResourceScheduleInterfaceCode::UNREGISTER_SYSTEMLOAD_NOTIFIER), reply, reply, option);
    EXPECT_TRUE(!res);
    res = resSchedServiceStub_->OnRemoteRequest(
        static_cast<uint32_t>(ResourceScheduleInterfaceCode::GET_SYSTEMLOAD_LEVEL), reply, reply, option);
    EXPECT_TRUE(res);
}

static void RemoteRequestTask()
{
    auto resSchedServiceStub_ = make_shared<TestResSchedServiceStub>();
    MessageOption option;
    MessageParcel reply;
    int32_t res = resSchedServiceStub_->OnRemoteRequest(
        static_cast<uint32_t>(ResourceScheduleInterfaceCode::REPORT_DATA), reply, reply, option);
    EXPECT_TRUE(res);
    res = resSchedServiceStub_->OnRemoteRequest(
        static_cast<uint32_t>(ResourceScheduleInterfaceCode::KILL_PROCESS), reply, reply, option);
    EXPECT_TRUE(res);
    res = resSchedServiceStub_->OnRemoteRequest(0, reply, reply, option);
    EXPECT_TRUE(res);
    res = resSchedServiceStub_->OnRemoteRequest(
        static_cast<uint32_t>(ResourceScheduleInterfaceCode::REGISTER_SYSTEMLOAD_NOTIFIER), reply, reply, option);
    EXPECT_TRUE(!res);
    res = resSchedServiceStub_->OnRemoteRequest(
        static_cast<uint32_t>(ResourceScheduleInterfaceCode::UNREGISTER_SYSTEMLOAD_NOTIFIER), reply, reply, option);
    EXPECT_TRUE(!res);
    res = resSchedServiceStub_->OnRemoteRequest(
        static_cast<uint32_t>(ResourceScheduleInterfaceCode::GET_SYSTEMLOAD_LEVEL), reply, reply, option);
    EXPECT_TRUE(res);
}

/**
 * @tc.name: ResSchedServicesStub RemoteRequest 002
 * @tc.desc: Test resschedstub RemoteRequest in multithreading.
 * @tc.type: FUNC
 * @tc.require: issueI7G8VT
 * @tc.author: nizihao
 */
HWTEST_F(ResSchedServiceTest, RemoteRequest002, Function | MediumTest | Level0)
{
    SET_THREAD_NUM(10);
    GTEST_RUN_TASK(RemoteRequestTask);
}

/**
 * @tc.name: ResSchedServicesStub RegisterSystemloadNotifier 001
 * @tc.desc: Verify if resschedstub RegisterSystemloadNotifier is success.
 * @tc.type: FUNC
 * @tc.require: issueI97M6C
 * @tc.author:shanhaiyang
 */
HWTEST_F(ResSchedServiceTest, StubRegisterSystemloadNotifier001, Function | MediumTest | Level0)
{
    auto resSchedServiceStub_ = make_shared<TestResSchedServiceStub>();
    MessageParcel reply;
    MessageParcel emptyData;
    EXPECT_TRUE(resSchedServiceStub_ != nullptr);
    resSchedServiceStub_->RegisterSystemloadNotifierInner(emptyData, reply);
}

/**
 * @tc.name: ResSchedServicesStub UnRegisterSystemloadNotifier 001
 * @tc.desc: Verify if resschedstub UnRegisterSystemloadNotifier is success.
 * @tc.type: FUNC
 * @tc.require: issueI97M6C
 * @tc.author:shanhaiyang
 */
HWTEST_F(ResSchedServiceTest, StubUnRegisterSystemloadNotifier001, Function | MediumTest | Level0)
{
    auto resSchedServiceStub_ = make_shared<TestResSchedServiceStub>();
    MessageParcel reply;
    MessageParcel emptyData;
    EXPECT_TRUE(resSchedServiceStub_ != nullptr);
    resSchedServiceStub_->UnRegisterSystemloadNotifierInner(emptyData, reply);
}

/**
 * @tc.name: ResSchedServicesStub GetSystemloadLevel 001
 * @tc.desc: Verify if resschedstub GetSystemloadLevel is success.
 * @tc.type: FUNC
 * @tc.require: issueI97M6C
 * @tc.author:shanhaiyang
 */
HWTEST_F(ResSchedServiceTest, StubGetSystemloadLevel001, Function | MediumTest | Level0)
{
    auto resSchedServiceStub_ = make_shared<TestResSchedServiceStub>();
    MessageParcel reply;
    MessageParcel emptyData;
    EXPECT_TRUE(resSchedServiceStub_->GetSystemloadLevelInner(emptyData, reply));
}

/**
 * @tc.name: ResSchedServicesStub IsAllowedAppPreloadInner 001
 * @tc.desc: Verify resschedstub allowedAppPreloadInner.
 * @tc.type: FUNC
 * @tc.require: issueI9C9JN
 * @tc.author:xiaoshun
 */
HWTEST_F(ResSchedServiceTest, IsAllowedAppPreloadInner001, Function | MediumTest | Level0)
{
    auto resSchedServiceStub_ = make_shared<TestResSchedServiceStub>();
    resSchedServiceStub_->Init();
    MessageParcel reply;
    MessageParcel emptyData;
    EXPECT_TRUE(!resSchedServiceStub_->IsAllowedAppPreloadInner(emptyData, reply));
}

/**
 * @tc.name: ResSchedServicesStub IsLimitRequest 001
 * @tc.desc: IsLimitRequestTest
 * @tc.type: FUNC
 * @tc.require: issueI9U0YF
 * @tc.author:fengyang
 */
HWTEST_F(ResSchedServiceTest, IsLimitRequest001, Function | MediumTest | Level0)
{
    auto resSchedServiceStub_ = make_shared<TestResSchedServiceStub>();
    resSchedServiceStub_->Init();
    int32_t uid = 0;
    EXPECT_EQ(resSchedServiceStub_->IsLimitRequest(uid), false);
    resSchedServiceStub_->appRequestCountMap_[uid] = 300;
    EXPECT_EQ(resSchedServiceStub_->IsLimitRequest(uid), true);
    resSchedServiceStub_->allRequestCount_.store(800);
    EXPECT_EQ(resSchedServiceStub_->IsLimitRequest(uid), true);
}

/**
 * @tc.name: ResSchedServicesStub PrintLimitLog 001
 * @tc.desc: PrintLimitLog
 * @tc.type: FUNC
 * @tc.require: issuesIAGHOC
 * @tc.author:fengyang
 */
HWTEST_F(ResSchedServiceTest, PrintLimitLog001, Function | MediumTest | Level0)
{
    auto resSchedServiceStub_ = make_shared<TestResSchedServiceStub>();
    resSchedServiceStub_->Init();
    int32_t uid = 0;
    resSchedServiceStub_->isPrintLimitLog_.store(true);
    resSchedServiceStub_->PrintLimitLog(uid);
    EXPECT_EQ(resSchedServiceStub_->isPrintLimitLog_.load(), false);
}

/**
 * @tc.name: ResSchedServicesStub ReportBigData 001
 * @tc.desc: ReportBigData
 * @tc.type: FUNC
 * @tc.require: issuesIAGHOC
 * @tc.author:fengyang
 */
HWTEST_F(ResSchedServiceTest, ReportBigData001, Function | MediumTest | Level0)
{
    auto resSchedServiceStub_ = make_shared<TestResSchedServiceStub>();
    resSchedServiceStub_->Init();
    resSchedServiceStub_->isReportBigData_.store(false);
    resSchedServiceStub_->ReportBigData();
    resSchedServiceStub_->isReportBigData_.store(true);
    resSchedServiceStub_->ReportBigData();
    resSchedServiceStub_->nextReportBigDataTime_ = ResCommonUtil::GetNowMillTime();
    resSchedServiceStub_->ReportBigData();
    EXPECT_EQ(resSchedServiceStub_->isReportBigData_.load(), false);
}

/**
 * @tc.name: ResSchedServicesStub InreaseBigDataCount 001
 * @tc.desc: InreaseBigDataCount
 * @tc.type: FUNC
 * @tc.require: issuesIAGHOC
 * @tc.author:fengyang
 */
HWTEST_F(ResSchedServiceTest, InreaseBigDataCount001, Function | MediumTest | Level0)
{
    auto resSchedServiceStub_ = make_shared<TestResSchedServiceStub>();
    resSchedServiceStub_->Init();
    resSchedServiceStub_->isReportBigData_.store(false);
    resSchedServiceStub_->InreaseBigDataCount();
    resSchedServiceStub_->isReportBigData_.store(true);
    resSchedServiceStub_->InreaseBigDataCount();
    EXPECT_EQ(resSchedServiceStub_->isReportBigData_.load(), true);
}

} // namespace ResourceSchedule
} // namespace OHOS
