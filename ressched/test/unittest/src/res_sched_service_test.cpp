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

    ErrCode OnSystemloadLevel(int32_t level)
    {
        testSystemloadLevel = level;
        return ERR_OK;
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
    resSchedService_->InitAllowIpcReportRes();
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

    std::vector<std::u16string> argsOnePlugin6 = {to_utf16("PluginConfig")};
    res = resSchedService_->Dump(correctFd, argsOnePlugin6);
}

static void ChangeAbilityTask()
{
    std::shared_ptr<ResSchedServiceAbility> resSchedServiceAbility_ = make_shared<ResSchedServiceAbility>();
    std::string deviceId;
    resSchedServiceAbility_->OnAddSystemAbility(-1, deviceId);
    resSchedServiceAbility_->OnRemoveSystemAbility(-1, deviceId);
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
    std::string payload;
    EXPECT_TRUE(resSchedService_ != nullptr);
    resSchedService_->ReportData(0, 0, payload);
    std::string deviceId;
    int32_t systemAbilityId = -1;
    resSchedServiceAbility_->OnAddSystemAbility(systemAbilityId, deviceId);
    resSchedServiceAbility_->OnRemoveSystemAbility(systemAbilityId, deviceId);
    SET_THREAD_NUM(10);
    GTEST_RUN_TASK(ChangeAbilityTask);
}

/**
 * @tc.name: Ressched service ReportData 002
 * @tc.desc: Verify if Ressched service ReportData is success.
 * @tc.type: FUNC
 * @tc.require: issueIC9NFW
 * @tc.author:baiheng
 */
HWTEST_F(ResSchedServiceTest, Report003, Function | MediumTest | Level0)
{
    std::string payload;
    EXPECT_TRUE(resSchedService_ != nullptr);
    resSchedService_->ReportData(ResType::RES_TYPE_CLICK_RECOGNIZE, ResType::ClickEventType::TOUCH_EVENT_DOWN, payload);
    resSchedService_->ReportData(ResType::RES_TYPE_PUSH_PAGE, ResType::PushPageType::PUSH_PAGE_START, payload);
    resSchedService_->ReportData(ResType::RES_TYPE_SLIDE_RECOGNIZE, 0, payload);
    resSchedService_->ReportData(ResType::RES_TYPE_POP_PAGE, 0, payload);
    resSchedService_->ReportData(ResType::RES_TYPE_LOAD_PAGE, 0, payload);
    resSchedService_->ReportData(ResType::RES_TYPE_KEY_EVENT, 0, payload);
}

/**
 * @tc.name: Ressched service ReportData 003
 * @tc.desc: Verify if Ressched service ReportData is success.
 * @tc.type: FUNC
 * @tc.require: issueIC9NFW
 * @tc.author:baiheng
 */
HWTEST_F(ResSchedServiceTest, Report004, Function | MediumTest | Level0)
{
    std::string payload;
    EXPECT_TRUE(resSchedService_ != nullptr);
    resSchedService_->ReportData(ResType::SYNC_RES_TYPE_THAW_ONE_APP, 0, payload);
    resSchedService_->ReportData(ResType::RES_TYPE_REPORT_SCENE_BOARD, 0, payload);
    resSchedService_->ReportData(ResType::RES_TYPE_SHOW_REMOTE_ANIMATION, 0, payload);
    resSchedService_->ReportData(ResType::RES_TYPE_MOVE_WINDOW, 0, payload);
    resSchedService_->ReportData(ResType::RES_TYPE_RESIZE_WINDOW, 0, payload);
    resSchedService_->ReportData(ResType::RES_TYPE_ONLY_PERF_APP_COLD_START, 0, payload);
    resSchedService_->ReportData(ResType::RES_TYPE_GESTURE_ANIMATION, 0, payload);
    resSchedService_->ReportData(ResType::RES_TYPE_SCENE_ROTATION, 0, payload);
}

/**
 * @tc.name: Ressched service ReportData 004
 * @tc.desc: Verify if Ressched service ReportData is success.
 * @tc.type: FUNC
 * @tc.require: issueIC9NFW
 * @tc.author:baiheng
 */
HWTEST_F(ResSchedServiceTest, Report005, Function | MediumTest | Level0)
{
    std::string payload;
    EXPECT_TRUE(resSchedService_ != nullptr);
    resSchedService_->ReportData(ResType::RES_TYPE_SCREEN_STATUS, 0, payload);
    resSchedService_->ReportData(ResType::RES_TYPE_EXTENSION_STATE_CHANGE, 0, payload);
    resSchedService_->ReportData(ResType::RES_TYPE_PROCESS_STATE_CHANGE, 0, payload);
    resSchedService_->ReportData(ResType::RES_TYPE_GET_GAME_SCENE_INFO, 0, payload);
    resSchedService_->ReportData(ResType::RES_TYPE_GAME_INFO_NOTIFY, 0, payload);
    resSchedService_->ReportData(ResType::SYNC_RES_TYPE_THAW_ONE_APP, 0, payload);
}

/**
 * @tc.name: Ressched service StringToJsonObj001
 * @tc.desc: test StringToJsonObj.
 * @tc.type: FUNC
 * @tc.require: issueIC9NFW
 * @tc.author:baiheng
 */
HWTEST_F(ResSchedServiceTest, StringToJsonObj001, Function | MediumTest | Level0)
{
    std::string payload;
    EXPECT_TRUE(resSchedService_ != nullptr);
    resSchedService_->StringToJsonObj("");
    resSchedService_->StringToJsonObj("{}");
    resSchedService_->StringToJsonObj("{\"key\":\"value\"}");
    resSchedService_->StringToJsonObj("aaa");
}

#ifdef RESOURCE_SCHEDULE_SERVICE_WITH_EXT_RES_ENABLE
/**
 * @tc.name: Ressched service GetExtTypeByResPayload001
 * @tc.desc: test GetExtTypeByResPayload.
 * @tc.type: FUNC
 * @tc.require: issueIC9NFW
 * @tc.author:baiheng
 */
HWTEST_F(ResSchedServiceTest, GetExtTypeByResPayload001, Function | MediumTest | Level0)
{
    std::string payload;
    EXPECT_TRUE(resSchedService_ != nullptr);
    resSchedService_->GetExtTypeByResPayload(payload);
    payload = "{\"extType\": 1}";
    resSchedService_->GetExtTypeByResPayload(payload);
    payload = "{\"extType\": \"-100.00\"}";
    resSchedService_->GetExtTypeByResPayload(payload);
    payload = "{\"extType\": \"10000\"}";
    resSchedService_->GetExtTypeByResPayload(payload);
}
#endif

/**
 * @tc.name: Ressched service IsAllowedAppPreload001
 * @tc.desc: test IsAllowedAppPreload.
 * @tc.type: FUNC
 * @tc.require: issueIC9NFW
 * @tc.author:baiheng
 */
HWTEST_F(ResSchedServiceTest, IsAllowedAppPreload001, Function | MediumTest | Level0)
{
    std::string payload;
    EXPECT_TRUE(resSchedService_ != nullptr);
    bool res1 = false;
    int32_t res2 = -1;
    resSchedService_->IsAllowedAppPreload("test", 0, res1);
    resSchedService_->LoadAppPreloadPlugin();
    resSchedService_->IsAllowedLinkJump(false, res2);
    resSchedService_->IsAllowedLinkJump(true, res2);
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
    std::string reply;
    int32_t ret;
    resSchedService_->ReportSyncEvent(ResType::SYNC_RES_TYPE_THAW_ONE_APP, 0,
        payload.dump(-1, ' ', false, nlohmann::detail::error_handler_t::replace),
        reply, ret);
    // 事件分发失败，返回err
    EXPECT_NE(ret, 0);
}

static void ReportTask()
{
    std::shared_ptr<ResSchedService> resSchedService_ = make_shared<ResSchedService>();
    std::string payload;
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
    std::string payload;
    int32_t t;
    resSchedService_->KillProcess(payload, t);
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
    int32_t res;
    resSchedService_->GetSystemloadLevel(res);
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
    int32_t res;
    resSchedService_->GetSystemloadLevel(res);
    EXPECT_TRUE(res == 2);
}

class TestResSchedServiceStub : public ResSchedServiceStub {
public:
    TestResSchedServiceStub() : ResSchedServiceStub() {}

    ErrCode ReportData(uint32_t restype, int64_t value, const std::string& payload) override
    {
        return ERR_OK;
    }

    ErrCode ReportSyncEvent(uint32_t resType, int64_t value, const std::string& payload,
        std::string& reply, int32_t& resultValue) override
    {
        return ERR_OK;
    }

    ErrCode KillProcess(const std::string& payload, int32_t& resultValue) override
    {
        return ERR_OK;
    }

    ErrCode RegisterSystemloadNotifier(const sptr<IRemoteObject>& notifier) override
    {
        return ERR_OK;
    }

    ErrCode UnRegisterSystemloadNotifier() override
    {
        return ERR_OK;
    }

    ErrCode RegisterEventListener(const sptr<IRemoteObject>& listener, uint32_t eventType,
        uint32_t listenerGroup) override
    {
        return ERR_OK;
    }

    ErrCode UnRegisterEventListener(uint32_t eventType,
        uint32_t listenerGroup) override
    {
        return ERR_OK;
    }

    ErrCode GetSystemloadLevel(int32_t& resultValue) override
    {
        return ERR_OK;
    }

    ErrCode IsAllowedAppPreload(const std::string& bundleName, int32_t preloadMode, bool& resultValue) override
    {
        return ERR_OK;
    }

    ErrCode IsAllowedLinkJump(bool isAllowedLinkJump, int32_t& resultValue) override
    {
        return ERR_OK;
    }

    ErrCode GetResTypeList(std::set<uint32_t>& resTypeList) override
    {
        return ERR_OK;
    }

    ErrCode RegisterSuspendObserver(const sptr<ISuspendStateObserverBase> &observer, int32_t &funcResult) override
    {
        return ERR_OK;
    }

    ErrCode UnregisterSuspendObserver(const sptr<ISuspendStateObserverBase> &observer, int32_t &funcResult) override
    {
        return ERR_OK;
    }

    ErrCode GetSuspendStateByUid(const int32_t uid, bool &isFrozen, int32_t &funcResult) override
    {
        return ERR_OK;
    }

    ErrCode GetSuspendStateByPid(const int32_t pid, bool &isFrozen, int32_t &funcResult) override
    {
        return ERR_OK;
    }
};

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
        static_cast<uint32_t>(IResSchedServiceIpcCode::COMMAND_REPORT_DATA), reply, reply, option);
    EXPECT_TRUE(res);
    res = resSchedServiceStub_->OnRemoteRequest(
        static_cast<uint32_t>(IResSchedServiceIpcCode::COMMAND_REPORT_SYNC_EVENT), reply, reply, option);
    EXPECT_TRUE(res);
    res = resSchedServiceStub_->OnRemoteRequest(
        static_cast<uint32_t>(IResSchedServiceIpcCode::COMMAND_KILL_PROCESS), reply, reply, option);
    EXPECT_TRUE(res);
    res = resSchedServiceStub_->OnRemoteRequest(0, reply, reply, option);
    EXPECT_TRUE(res);
    res = resSchedServiceStub_->OnRemoteRequest(
        static_cast<uint32_t>(IResSchedServiceIpcCode::COMMAND_REGISTER_SYSTEMLOAD_NOTIFIER), reply, reply, option);
    EXPECT_TRUE(res);
    res = resSchedServiceStub_->OnRemoteRequest(
        static_cast<uint32_t>(IResSchedServiceIpcCode::COMMAND_UN_REGISTER_SYSTEMLOAD_NOTIFIER), reply, reply, option);
    EXPECT_TRUE(res);
    res = resSchedServiceStub_->OnRemoteRequest(
        static_cast<uint32_t>(IResSchedServiceIpcCode::COMMAND_GET_SYSTEMLOAD_LEVEL), reply, reply, option);
    EXPECT_TRUE(res);
}

static void RemoteRequestTask()
{
    auto resSchedServiceStub_ = make_shared<TestResSchedServiceStub>();
    MessageOption option;
    MessageParcel reply;
    int32_t res = resSchedServiceStub_->OnRemoteRequest(
        static_cast<uint32_t>(IResSchedServiceIpcCode::COMMAND_REPORT_DATA), reply, reply, option);
    EXPECT_TRUE(res);
    res = resSchedServiceStub_->OnRemoteRequest(
        static_cast<uint32_t>(IResSchedServiceIpcCode::COMMAND_KILL_PROCESS), reply, reply, option);
    EXPECT_TRUE(res);
    res = resSchedServiceStub_->OnRemoteRequest(0, reply, reply, option);
    EXPECT_TRUE(res);
    res = resSchedServiceStub_->OnRemoteRequest(
        static_cast<uint32_t>(IResSchedServiceIpcCode::COMMAND_REGISTER_SYSTEMLOAD_NOTIFIER), reply, reply, option);
    EXPECT_TRUE(res);
    res = resSchedServiceStub_->OnRemoteRequest(
        static_cast<uint32_t>(IResSchedServiceIpcCode::COMMAND_UN_REGISTER_SYSTEMLOAD_NOTIFIER), reply, reply, option);
    EXPECT_TRUE(res);
    res = resSchedServiceStub_->OnRemoteRequest(
        static_cast<uint32_t>(IResSchedServiceIpcCode::COMMAND_GET_SYSTEMLOAD_LEVEL), reply, reply, option);
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
 * @tc.name: Ressched service GetSystemLoadLevel debug 001
 * @tc.desc: Test resschedService DumpSetSystemLoad Err Param Num
 * @tc.type: FUNC
 * @tc.require: issue#ICQCY1
 * @tc.author: jiangfeng
 */
HWTEST_F(ResSchedServiceTest, DumpDebugSystemLoad001, Function | MediumTest | Level0)
{
    std::shared_ptr<ResSchedService> resSchedService_ = make_shared<ResSchedService>();
    EXPECT_TRUE(resSchedService_ != nullptr);
    sptr<IRemoteObject> notifier = new (std::nothrow) TestResSchedSystemloadListener();
    EXPECT_TRUE(notifier != nullptr);
    NotifierMgr::GetInstance().Init();
    // Err Param Num
    std::vector<std::string> args = {};
    std::string result_;
    resSchedService_->DumpSetSystemLoad(args, result_);
    EXPECT_TRUE(result_.find("Err setSystemLoadLevel param num") != std::string::npos);
}

/**
 * @tc.name: Ressched service GetSystemLoadLevel debug 002
 * @tc.desc: Test resschedService DumpSetSystemLoad Close Debug
 * @tc.type: FUNC
 * @tc.require: issue#ICQCY1
 * @tc.author: jiangfeng
 */
HWTEST_F(ResSchedServiceTest, DumpDebugSystemLoad002, Function | MediumTest | Level0)
{
    std::shared_ptr<ResSchedService> resSchedService_ = make_shared<ResSchedService>();
    EXPECT_TRUE(resSchedService_ != nullptr);
    sptr<IRemoteObject> notifier = new (std::nothrow) TestResSchedSystemloadListener();
    EXPECT_TRUE(notifier != nullptr);
    NotifierMgr::GetInstance().Init();
    // Close Debug
    std::vector<std::string> args = {"0", "reset"};
    std::string result_;
    resSchedService_->DumpSetSystemLoad(args, result_);
    EXPECT_FALSE(resSchedService_->systemLoadLevelDebugEnable_);
    EXPECT_EQ(resSchedService_->debugSystemLoadLevel_, 0);
}

/**
 * @tc.name: Ressched service GetSystemLoadLevel debug 003
 * @tc.desc: Test resschedService DumpSetSystemLoad over Maximum
 * @tc.type: FUNC
 * @tc.require: issue#ICQCY1
 * @tc.author: jiangfeng
 */
HWTEST_F(ResSchedServiceTest, DumpDebugSystemLoad003, Function | MediumTest | Level0)
{
    std::shared_ptr<ResSchedService> resSchedService_ = make_shared<ResSchedService>();
    EXPECT_TRUE(resSchedService_ != nullptr);
    sptr<IRemoteObject> notifier = new (std::nothrow) TestResSchedSystemloadListener();
    EXPECT_TRUE(notifier != nullptr);
    NotifierMgr::GetInstance().Init();
    // Close Debug
    std::vector<std::string> args = {"0", "8"};
    std::string result_;
    resSchedService_->DumpSetSystemLoad(args, result_);
    EXPECT_FALSE(resSchedService_->systemLoadLevelDebugEnable_);
    EXPECT_EQ(resSchedService_->debugSystemLoadLevel_, 0);
}

/**
 * @tc.name: Ressched service GetSystemLoadLevel debug 004
 * @tc.desc: Test resschedService DumpSetSystemLoad err num
 * @tc.type: FUNC
 * @tc.require: issue#ICQCY1
 * @tc.author: jiangfeng
 */
HWTEST_F(ResSchedServiceTest, DumpDebugSystemLoad004, Function | MediumTest | Level0)
{
    std::shared_ptr<ResSchedService> resSchedService_ = make_shared<ResSchedService>();
    EXPECT_TRUE(resSchedService_ != nullptr);
    sptr<IRemoteObject> notifier = new (std::nothrow) TestResSchedSystemloadListener();
    EXPECT_TRUE(notifier != nullptr);
    NotifierMgr::GetInstance().Init();
    // Close Debug
    std::vector<std::string> args = {"0", "-2"};
    std::string result_;
    resSchedService_->DumpSetSystemLoad(args, result_);
    EXPECT_FALSE(resSchedService_->systemLoadLevelDebugEnable_);
    EXPECT_EQ(resSchedService_->debugSystemLoadLevel_, 0);
}

/**
 * @tc.name: Ressched service GetSystemLoadLevel debug 005
 * @tc.desc: Test resschedService DumpSetSystemLoad valid value(3) and start debug
 * @tc.type: FUNC
 * @tc.require: issue#ICQCY1
 * @tc.author: jiangfeng
 */
HWTEST_F(ResSchedServiceTest, DumpDebugSystemLoad005, Function | MediumTest | Level0)
{
    std::shared_ptr<ResSchedService> resSchedService_ = make_shared<ResSchedService>();
    EXPECT_TRUE(resSchedService_ != nullptr);
    sptr<IRemoteObject> notifier = new (std::nothrow) TestResSchedSystemloadListener();
    EXPECT_TRUE(notifier != nullptr);
    NotifierMgr::GetInstance().Init();
    // Close Debug
    std::vector<std::string> args = {"0", "3"};
    std::string result_;
    resSchedService_->DumpSetSystemLoad(args, result_);
    EXPECT_TRUE(resSchedService_->systemLoadLevelDebugEnable_);
    EXPECT_EQ(resSchedService_->debugSystemLoadLevel_, 3);
}

/**
 * @tc.name: Ressched service GetSystemLoadLevel debug 006
 * @tc.desc: Test resschedService DumpSetSystemLoad reset
 * @tc.type: FUNC
 * @tc.require: issue#ICQCY1
 * @tc.author: jiangfeng
 */
HWTEST_F(ResSchedServiceTest, DumpDebugSystemLoad006, Function | MediumTest | Level0)
{
    std::shared_ptr<ResSchedService> resSchedService_ = make_shared<ResSchedService>();
    EXPECT_TRUE(resSchedService_ != nullptr);
    sptr<IRemoteObject> notifier = new (std::nothrow) TestResSchedSystemloadListener();
    EXPECT_TRUE(notifier != nullptr);
    NotifierMgr::GetInstance().Init();
    // Close Debug
    std::vector<std::string> args = {"0", "reset"};
    std::string result_;
    resSchedService_->DumpSetSystemLoad(args, result_);
    EXPECT_FALSE(resSchedService_->systemLoadLevelDebugEnable_);
    EXPECT_EQ(resSchedService_->debugSystemLoadLevel_, 0);
}

/**
 * @tc.name: Ressched service GetSystemLoadLevel debug 007
 * @tc.desc: Pass resschedService Dump func strToIntErr Branch
 * @tc.type: FUNC
 * @tc.require: issue#ICQCY1
 * @tc.author: jiangfeng
 */
HWTEST_F(ResSchedServiceTest, DumpDebugSystemLoad007, Function | MediumTest | Level0)
{
    std::shared_ptr<ResSchedService> resSchedService_ = make_shared<ResSchedService>();
    EXPECT_TRUE(resSchedService_ != nullptr);
    sptr<IRemoteObject> notifier = new (std::nothrow) TestResSchedSystemloadListener();
    EXPECT_TRUE(notifier != nullptr);
    NotifierMgr::GetInstance().Init();

    std::vector<std::string> args = {"setSystemLoadLevel", "++"};
    std::string result_;
    resSchedService_->DumpSetSystemLoad(args, result_);
    EXPECT_FALSE(resSchedService_->systemLoadLevelDebugEnable_);
}

/**
 * @tc.name: Ressched service OnDeviceLevelChanged Debug 001
 * @tc.desc: Test OnDeviceLevelChanged when debugReport is false and debugEnable is close
 * @tc.type: FUNC
 * @tc.require: issue#ICQCY1
 * @tc.author: jiangfeng
 */
HWTEST_F(ResSchedServiceTest, OnDeviceLevelChangedDebug001, Function | MediumTest | Level0)
{
    std::shared_ptr<ResSchedService> resSchedService_ = make_shared<ResSchedService>();
    EXPECT_TRUE(resSchedService_ != nullptr);
    sptr<IRemoteObject> notifier = new (std::nothrow) TestResSchedSystemloadListener();
    EXPECT_TRUE(notifier != nullptr);
    NotifierMgr::GetInstance().Init();
    int32_t type = static_cast<int32_t>(ResType::DeviceStatus::SYSTEMLOAD_LEVEL);
    int32_t level = 5;
    bool debugReport = false;

    resSchedService_->OnDeviceLevelChanged(type, level, debugReport);
    
    EXPECT_EQ(resSchedService_->actualSystemLoadLevel_, level);
    EXPECT_EQ(resSchedService_->debugSystemLoadLevel_, 0);
    EXPECT_FALSE(resSchedService_->systemLoadLevelDebugEnable_);
}

/**
 * @tc.name: Ressched service OnDeviceLevelChanged Debug 002
 * @tc.desc: Test OnDeviceLevelChanged when debugReport is false and debugEnable is open
 * @tc.type: FUNC
 * @tc.require: issue#ICQCY1
 * @tc.author: jiangfeng
 */
HWTEST_F(ResSchedServiceTest, OnDeviceLevelChangedDebug002, Function | MediumTest | Level0)
{
    std::shared_ptr<ResSchedService> resSchedService_ = make_shared<ResSchedService>();
    EXPECT_TRUE(resSchedService_ != nullptr);
    sptr<IRemoteObject> notifier = new (std::nothrow) TestResSchedSystemloadListener();
    EXPECT_TRUE(notifier != nullptr);
    NotifierMgr::GetInstance().Init();
    int32_t type = static_cast<int32_t>(ResType::DeviceStatus::SYSTEMLOAD_LEVEL);
    int32_t level = 5;
    bool debugReport = false;
    resSchedService_->debugSystemLoadLevel_ = 3;
    resSchedService_->systemLoadLevelDebugEnable_ = true;

    resSchedService_->OnDeviceLevelChanged(type, level, debugReport);
    
    EXPECT_EQ(resSchedService_->actualSystemLoadLevel_, level);
    EXPECT_EQ(resSchedService_->debugSystemLoadLevel_, 3);
    EXPECT_TRUE(resSchedService_->systemLoadLevelDebugEnable_);
}

/**
 * @tc.name: Ressched service OnDeviceLevelChanged Debug 003
 * @tc.desc: Test OnDeviceLevelChanged when debugReport is TRUE and debugEnable is open
 * @tc.type: FUNC
 * @tc.require: issue#ICQCY1
 * @tc.author: jiangfeng
 */
HWTEST_F(ResSchedServiceTest, OnDeviceLevelChangedDebug003, Function | MediumTest | Level0)
{
    std::shared_ptr<ResSchedService> resSchedService_ = make_shared<ResSchedService>();
    EXPECT_TRUE(resSchedService_ != nullptr);
    sptr<IRemoteObject> notifier = new (std::nothrow) TestResSchedSystemloadListener();
    EXPECT_TRUE(notifier != nullptr);
    NotifierMgr::GetInstance().Init();
    int32_t type = static_cast<int32_t>(ResType::DeviceStatus::SYSTEMLOAD_LEVEL);
    int32_t level = 5;
    bool debugReport = true;
    resSchedService_->debugSystemLoadLevel_ = 3;
    resSchedService_->systemLoadLevelDebugEnable_ = true;

    resSchedService_->OnDeviceLevelChanged(type, level, debugReport);
    EXPECT_EQ(resSchedService_->actualSystemLoadLevel_, 0); // if debugReport is true, cur variable will not change
    EXPECT_EQ(resSchedService_->debugSystemLoadLevel_, 3);
    EXPECT_TRUE(resSchedService_->systemLoadLevelDebugEnable_);
}

/**
 * @tc.name: Ressched service OnDeviceLevelChanged Debug 004
 * @tc.desc: Test other level, Forward directly
 * @tc.type: FUNC
 * @tc.require: issue#ICQCY1
 * @tc.author: jiangfeng
 */
HWTEST_F(ResSchedServiceTest, OnDeviceLevelChangedDebug004, Function | MediumTest | Level0)
{
    std::shared_ptr<ResSchedService> resSchedService_ = make_shared<ResSchedService>();
    EXPECT_TRUE(resSchedService_ != nullptr);
    sptr<IRemoteObject> notifier = new (std::nothrow) TestResSchedSystemloadListener();
    EXPECT_TRUE(notifier != nullptr);
    NotifierMgr::GetInstance().Init();
    int32_t type = 100;
    int32_t level = 5;
    bool debugReport = false;

    resSchedService_->OnDeviceLevelChanged(type, level, debugReport);
    EXPECT_EQ(resSchedService_->actualSystemLoadLevel_, 0); // if debugReport is true, cur variable will not change
    EXPECT_EQ(resSchedService_->debugSystemLoadLevel_, 0);
    EXPECT_FALSE(resSchedService_->systemLoadLevelDebugEnable_);
}

/**
 * @tc.name: ResSchedServiceTest GetSystemloadLevelDebug001
 * @tc.desc: Test other level, Forward directly
 * @tc.type: FUNC
 * @tc.require: issue#ICQCY1
 * @tc.author: jiangfeng
 */
HWTEST_F(ResSchedServiceTest, GetSystemloadLevelDebug001, Function | MediumTest | Level0)
{
    std::shared_ptr<ResSchedService> resSchedService_ = make_shared<ResSchedService>();
    EXPECT_TRUE(resSchedService_ != nullptr);
    sptr<IRemoteObject> notifier = new (std::nothrow) TestResSchedSystemloadListener();
    EXPECT_TRUE(notifier != nullptr);
    NotifierMgr::GetInstance().Init();

    resSchedService_->systemLoadLevelDebugEnable_ = true;
    resSchedService_->debugSystemLoadLevel_ = 2;
    int32_t resultValue = 0;

    resSchedService_->GetSystemloadLevel(resultValue);
    EXPECT_EQ(resSchedService_->debugSystemLoadLevel_, resultValue);
}

/**
 * @tc.name: Ressched service DumpUsage2D 001
 * @tc.desc: Test DumpUsage2D function when SET_SYSTEM_LOAD_LEVEL_2D_ENABLE is defined
 * @tc.type: FUNC
 * @tc.require: issue#ICQCY1
 * @tc.author: jiangfeng
 */
HWTEST_F(ResSchedServiceTest, DumpUsage2D001, Function | MediumTest | Level0)
{
    std::shared_ptr<ResSchedService> resSchedService_ = make_shared<ResSchedService>();
    EXPECT_TRUE(resSchedService_ != nullptr);

    std::string result;
#ifdef SET_SYSTEM_LOAD_LEVEL_2D_ENABLE
    resSchedService_->DumpUsage2D(result);
    EXPECT_TRUE(result.find("setSystemLoadLevel") != std::string::npos);
    EXPECT_TRUE(result.find("getSystemloadInfo") != std::string::npos);
    // DumpUsage2D should not include basic help info
    EXPECT_TRUE(result.find("-h: show the help") == std::string::npos);
#else
    // If SET_SYSTEM_LOAD_LEVEL_2D_ENABLE is not defined, this function should not be available
    GTEST_SKIP() << "SET_SYSTEM_LOAD_LEVEL_2D_ENABLE is not defined, skipping test";
#endif //SET_SYSTEM_LOAD_LEVEL_2D_ENABLE
}

/**
 * @tc.name: Ressched service DumpUsage 001
 * @tc.desc: Test DumpUsage function
 * @tc.type: FUNC
 * @tc.require: issue#ICQCY1
 * @tc.author: jiangfeng
 */
HWTEST_F(ResSchedServiceTest, DumpUsage001, Function | MediumTest | Level0)
{
    std::shared_ptr<ResSchedService> resSchedService_ = make_shared<ResSchedService>();
    EXPECT_TRUE(resSchedService_ != nullptr);

    std::string result;
    resSchedService_->DumpUsage(result);
    EXPECT_TRUE(result.find("-h: show the help") != std::string::npos);
    EXPECT_TRUE(result.find("-a: show all info") != std::string::npos);
    EXPECT_TRUE(result.find("-p: show the all plugin info") != std::string::npos);
    EXPECT_TRUE(result.find("-p (plugin name): show one plugin info") != std::string::npos);
    // Now DumpUsage always includes system load level related help info
    EXPECT_TRUE(result.find("setSystemLoadLevel") != std::string::npos);
    EXPECT_TRUE(result.find("getSystemloadInfo") != std::string::npos);
}

/**
 * @tc.name: Ressched service DumpExt Help 001
 * @tc.desc: Test DumpExt function with -h option
 * @tc.type: FUNC
 * @tc.require: issue#ICQCY1
 * @tc.author: jiangfeng
 */
HWTEST_F(ResSchedServiceTest, DumpExtHelp001, Function | MediumTest | Level0)
{
    std::shared_ptr<ResSchedService> resSchedService_ = make_shared<ResSchedService>();
    EXPECT_TRUE(resSchedService_ != nullptr);

    std::vector<std::string> argsInStr = {"-h"};
    std::string result;
    resSchedService_->DumpExt(argsInStr, result);
    EXPECT_TRUE(result.find("-h: show the help") != std::string::npos);
    // DumpExt with -h always includes system load level related help info
    EXPECT_TRUE(result.find("setSystemLoadLevel") != std::string::npos);
    EXPECT_TRUE(result.find("getSystemloadInfo") != std::string::npos);
}
} // namespace ResourceSchedule
} // namespace OHOS
