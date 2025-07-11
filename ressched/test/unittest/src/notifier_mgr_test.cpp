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

#include "notifier_mgr_test.h"

#include "ipc_skeleton.h"
#include "notifier_mgr.h"
#include "res_sched_common_death_recipient.h"
#include "res_sched_systemload_notifier_proxy.h"
#include "res_sched_systemload_notifier_stub.h"

using namespace std;
using namespace testing::ext;

namespace OHOS {
namespace AppExecFwk {
    int32_t AppMgrClient::GetBundleNameByPid(const int pid, std::string &bundleName, int32_t &uid)
    {
        if (pid == 1) {
            bundleName = "";
        } else {
            bundleName ="com.example.myapp";
        }
        return ERR_OK;
    }
}
namespace ResourceSchedule {
static constexpr int32_t APP_STATE_EXIT = 4;

class TestNotifierSystemloadListener : public ResSchedSystemloadNotifierStub {
public:
    TestNotifierSystemloadListener() = default;

    ErrCode OnSystemloadLevel(int32_t level)
    {
        testSystemloadLevel = level;
        return ERR_OK;
    }

    static int32_t testSystemloadLevel;
};

int32_t TestNotifierSystemloadListener::testSystemloadLevel = 0;

void NotifierMgrTest::SetUpTestCase()
{
    NotifierMgr::GetInstance().Init();
}

void NotifierMgrTest::TearDownTestCase() {}

void NotifierMgrTest::SetUp() {}

void NotifierMgrTest::TearDown()
{
    NotifierMgr::GetInstance().UnRegisterNotifier(IPCSkeleton::GetCallingPid());
    NotifierMgr::GetInstance().OnApplicationStateChange(APP_STATE_EXIT, IPCSkeleton::GetCallingPid());
    TestNotifierSystemloadListener::testSystemloadLevel = 0;
}

/**
 * @tc.name: notifier manager TestNotifierSystemloadListener 001
 * @tc.desc: test the interface TestNotifierSystemloadListener
 * @tc.type: FUNC
 * @tc.require: issueI97M6C
 * @tc.author:shanhaiyang
 */
HWTEST_F(NotifierMgrTest, TestNotifierSystemloadListener001, Function | MediumTest | Level0)
{
    sptr<IRemoteObject> notifier = new (std::nothrow) TestNotifierSystemloadListener();
    EXPECT_TRUE(notifier != nullptr);
    NotifierMgr::GetInstance().RegisterNotifier(IPCSkeleton::GetCallingPid(), notifier);
    NotifierMgr::GetInstance().OnApplicationStateChange(2, IPCSkeleton::GetCallingPid());
    NotifierMgr::GetInstance().OnDeviceLevelChanged(0, 2);
    sleep(1);
    EXPECT_TRUE(TestNotifierSystemloadListener::testSystemloadLevel == 2);
}

/**
 * @tc.name: notifier manager TestNotifierSystemloadListener 002
 * @tc.desc: test the interface TestNotifierSystemloadListener
 * @tc.type: FUNC
 * @tc.require: issueI97M6C
 * @tc.author:shanhaiyang
 */
HWTEST_F(NotifierMgrTest, TestNotifierSystemloadListener002, Function | MediumTest | Level0)
{
    sptr<IRemoteObject> notifier = new (std::nothrow) TestNotifierSystemloadListener();
    EXPECT_TRUE(notifier != nullptr);
    NotifierMgr::GetInstance().RegisterNotifier(IPCSkeleton::GetCallingPid(), notifier);
    NotifierMgr::GetInstance().OnDeviceLevelChanged(0, 2);
    sleep(1);
    EXPECT_TRUE(TestNotifierSystemloadListener::testSystemloadLevel == 2);
}

/**
 * @tc.name: notifier manager TestNotifierSystemloadListener 003
 * @tc.desc: test the interface RegisterNotifier
 * @tc.type: FUNC
 * @tc.require: issueI97M6C
 * @tc.author:shanhaiyang
 */
HWTEST_F(NotifierMgrTest, TestNotifierSystemloadListener003, Function | MediumTest | Level0)
{
    sptr<IRemoteObject> notifier = new (std::nothrow) TestNotifierSystemloadListener();
    EXPECT_TRUE(notifier != nullptr);
    NotifierMgr::GetInstance().RegisterNotifier(IPCSkeleton::GetCallingPid(), notifier);
    NotifierMgr::GetInstance().OnApplicationStateChange(2, IPCSkeleton::GetCallingPid());
    NotifierMgr::GetInstance().OnDeviceLevelChanged(1, 2);
    sleep(1);
    EXPECT_TRUE(TestNotifierSystemloadListener::testSystemloadLevel == 0);
}

/**
 * @tc.name: notifier manager TestNotifierSystemloadListener 004
 * @tc.desc: test the interface TestNotifierSystemloadListener
 * @tc.type: FUNC
 * @tc.require: issueI97M6C
 * @tc.author:shanhaiyang
 */
HWTEST_F(NotifierMgrTest, TestNotifierSystemloadListener004, Function | MediumTest | Level0)
{
    sptr<IRemoteObject> notifier = new (std::nothrow) TestNotifierSystemloadListener();
    EXPECT_TRUE(notifier != nullptr);
    NotifierMgr::GetInstance().RegisterNotifier(IPCSkeleton::GetCallingPid(), notifier);
    NotifierMgr::GetInstance().OnApplicationStateChange(2, 111111);
    NotifierMgr::GetInstance().OnDeviceLevelChanged(0, 2);
    sleep(1);
    EXPECT_TRUE(TestNotifierSystemloadListener::testSystemloadLevel == 2);
    NotifierMgr::GetInstance().OnApplicationStateChange(APP_STATE_EXIT, 111111);
}

/**
 * @tc.name: notifier manager TestNotifierSystemloadListener 005
 * @tc.desc: test the interface TestNotifierSystemloadListener
 * @tc.type: FUNC
 * @tc.require: issueI97M6C
 * @tc.author:shanhaiyang
 */
HWTEST_F(NotifierMgrTest, TestNotifierSystemloadListener005, Function | MediumTest | Level0)
{
    sptr<IRemoteObject> notifier = new (std::nothrow) TestNotifierSystemloadListener();
    EXPECT_TRUE(notifier != nullptr);
    NotifierMgr::GetInstance().RegisterNotifier(IPCSkeleton::GetCallingPid(), notifier);
    NotifierMgr::GetInstance().OnApplicationStateChange(2, IPCSkeleton::GetCallingPid());
    NotifierMgr::GetInstance().OnRemoteNotifierDied(notifier);
    NotifierMgr::GetInstance().OnDeviceLevelChanged(0, 2);
    sleep(1);
    EXPECT_TRUE(TestNotifierSystemloadListener::testSystemloadLevel == 0);
}

/**
 * @tc.name: notifier manager TestNotifierSystemloadListener 006
 * @tc.desc: test the interface TestNotifierSystemloadListener
 * @tc.type: FUNC
 * @tc.require: issueI9G149
 * @tc.author:shanhaiyang
 */
HWTEST_F(NotifierMgrTest, TestNotifierSystemloadListener006, Function | MediumTest | Level0)
{
    sptr<IRemoteObject> notifier = new (std::nothrow) TestNotifierSystemloadListener();
    EXPECT_TRUE(notifier != nullptr);
    auto callingPid = IPCSkeleton::GetCallingPid();
    NotifierMgr::GetInstance().RegisterNotifier(callingPid, notifier);
    auto& info = NotifierMgr::GetInstance().notifierMap_[callingPid];
    info.hapApp = true;
    NotifierMgr::GetInstance().OnApplicationStateChange(APP_STATE_EXIT, callingPid);
    NotifierMgr::GetInstance().OnDeviceLevelChanged(0, 2);
    NotifierMgr::GetInstance().OnApplicationStateChange(2, callingPid);
    sleep(1);
    EXPECT_TRUE(TestNotifierSystemloadListener::testSystemloadLevel == 2);
}

/**
 * @tc.name: notifier manager RegisterNotifier 001
 * @tc.desc: test the interface RegisterNotifier
 * @tc.type: FUNC
 * @tc.require: issueI97M6C
 * @tc.author:shanhaiyang
 */
HWTEST_F(NotifierMgrTest, RegisterNotifier001, Function | MediumTest | Level0)
{
    sptr<IRemoteObject> notifier = new (std::nothrow) TestNotifierSystemloadListener();
    EXPECT_TRUE(notifier != nullptr);
    sptr<IRemoteObject> notifier1 = new (std::nothrow) TestNotifierSystemloadListener();
    EXPECT_TRUE(notifier != nullptr);
    auto callingPid = IPCSkeleton::GetCallingPid();
    NotifierMgr::GetInstance().RegisterNotifier(callingPid, notifier);
    NotifierMgr::GetInstance().RegisterNotifier(callingPid, notifier1);
    auto realNotifier = NotifierMgr::GetInstance().notifierMap_[callingPid].notifier;
    EXPECT_TRUE(notifier.GetRefPtr() == realNotifier.GetRefPtr());
}

/**
 * @tc.name: notifier manager RegisterNotifier 002
 * @tc.desc: test the interface RegisterNotifier
 * @tc.type: FUNC
 * @tc.require: issueI97M6C
 * @tc.author:shanhaiyang
 */
HWTEST_F(NotifierMgrTest, RegisterNotifier002, Function | MediumTest | Level0)
{
    sptr<IRemoteObject> notifier = new (std::nothrow) TestNotifierSystemloadListener();
    EXPECT_TRUE(notifier != nullptr);
    auto callingPid = IPCSkeleton::GetCallingPid();
    NotifierMgr::GetInstance().RegisterNotifier(callingPid, notifier);
    NotifierMgr::GetInstance().RegisterNotifier(callingPid, notifier);
    auto size = NotifierMgr::GetInstance().notifierMap_.size();
    EXPECT_TRUE(size == 1);
}

/**
 * @tc.name: notifier manager RegisterNotifier 003
 * @tc.desc: test the interface RegisterNotifier
 * @tc.type: FUNC
 * @tc.require: issueI97M6C
 * @tc.author:luolu
 */
HWTEST_F(NotifierMgrTest, RegisterNotifier003, Function | MediumTest | Level0)
{
    sptr<IRemoteObject> notifier;
    auto callingPid = IPCSkeleton::GetCallingPid();
    NotifierMgr::GetInstance().RegisterNotifier(callingPid, notifier);
    EXPECT_EQ(NotifierMgr::GetInstance().notifierMap_.size(), 0);
}

/**
 * @tc.name: notifier manager RegisterNotifier 004
 * @tc.desc: test the interface RegisterNotifier
 * @tc.type: FUNC
 * @tc.require: issueI97M6C
 * @tc.author:luolu
 */
HWTEST_F(NotifierMgrTest, RegisterNotifier004, Function | MediumTest | Level0)
{
    sptr<IRemoteObject> notifier = new (std::nothrow) TestNotifierSystemloadListener();
    EXPECT_TRUE(notifier != nullptr);
    auto callingPid = IPCSkeleton::GetCallingPid();
    NotifierMgr::GetInstance().notifierDeathRecipient_ = nullptr;
    NotifierMgr::GetInstance().RegisterNotifier(callingPid, notifier);
}

/**
 * @tc.name: notifier manager RegisterNotifier 005
 * @tc.desc: test the interface RegisterNotifier
 * @tc.type: FUNC
 * @tc.require: issueI97M6C
 * @tc.author:zhuxiaofei
 */
HWTEST_F(NotifierMgrTest, RegisterNotifier005, Function | MediumTest | Level0)
{
    NotifierMgr::GetInstance().isTaskSubmit_ = false;
    NotifierMgr::GetInstance().ReportPidToHisysevent(1);
    EXPECT_TRUE(!NotifierMgr::GetInstance().isTaskSubmit_);
}

/**
 * @tc.name: notifier manager RegisterNotifier 006
 * @tc.desc: test the interface RegisterNotifier
 * @tc.type: FUNC
 * @tc.require: issueI97M6C
 * @tc.author:zhuxiaofei
 */
HWTEST_F(NotifierMgrTest, RegisterNotifier006, Function | MediumTest | Level0)
{
    NotifierMgr::GetInstance().isTaskSubmit_ = true;
    NotifierMgr::GetInstance().hisyseventBundleNames_.clear();
    for (int i = 1; i < 100; i++) {
        NotifierMgr::GetInstance().hisyseventBundleNames_.insert(std::to_string(i));
    }
    NotifierMgr::GetInstance().ReportPidToHisysevent(100);
    EXPECT_TRUE(NotifierMgr::GetInstance().hisyseventBundleNames_.size() == 0);
}

/**
 * @tc.name: notifier manager RegisterNotifier 007
 * @tc.desc: test the interface RegisterNotifier
 * @tc.type: FUNC
 * @tc.require: issueI97M6C
 * @tc.author:zhuxiaofei
 */
HWTEST_F(NotifierMgrTest, RegisterNotifier007, Function | MediumTest | Level0)
{
    NotifierMgr::GetInstance().isTaskSubmit_ = false;
    NotifierMgr::GetInstance().hisyseventBundleNames_.clear();
    NotifierMgr::GetInstance().hisyseventBundleNames_.insert("1");
    NotifierMgr::GetInstance().NotifierEventReportPeriod();
    EXPECT_TRUE(NotifierMgr::GetInstance().hisyseventBundleNames_.size() == 0);
}

/**
 * @tc.name: notifier manager RegisterNotifier 008
 * @tc.desc: test the interface RegisterNotifier
 * @tc.type: FUNC
 * @tc.require: issueI97M6C
 * @tc.author:zhuxiaofei
 */
HWTEST_F(NotifierMgrTest, RegisterNotifier008, Function | MediumTest | Level0)
{
    NotifierMgr::GetInstance().isTaskSubmit_ = false;
    NotifierMgr::GetInstance().hisyseventBundleNames_.clear();
    NotifierMgr::GetInstance().ReportPidToHisysevent(100);
    EXPECT_TRUE(NotifierMgr::GetInstance().isTaskSubmit_);
}

/**
 * @tc.name: notifier manager UnRegisterNotifier 001
 * @tc.desc: test the interface UnRegisterNotifier
 * @tc.type: FUNC
 * @tc.require: issueI97M6C
 * @tc.author:shanhaiyang
 */
HWTEST_F(NotifierMgrTest, UnRegisterNotifier001, Function | MediumTest | Level0)
{
    sptr<IRemoteObject> notifier = new (std::nothrow) TestNotifierSystemloadListener();
    EXPECT_TRUE(notifier != nullptr);
    NotifierMgr::GetInstance().Init();
    auto callingPid = IPCSkeleton::GetCallingPid();
    NotifierMgr::GetInstance().RegisterNotifier(callingPid, notifier);
    NotifierMgr::GetInstance().UnRegisterNotifier(callingPid);
    auto size = NotifierMgr::GetInstance().notifierMap_.size();
    EXPECT_TRUE(size == 0);
}

/**
 * @tc.name: notifier manager UnRegisterNotifier 002
 * @tc.desc: test the interface UnRegisterNotifier
 * @tc.type: FUNC
 * @tc.require: issueI97M6C
 * @tc.author:shanhaiyang
 */
HWTEST_F(NotifierMgrTest, UnRegisterNotifier002, Function | MediumTest | Level0)
{
    NotifierMgr::GetInstance().UnRegisterNotifier(IPCSkeleton::GetCallingPid());
    auto size = NotifierMgr::GetInstance().notifierMap_.size();
    EXPECT_TRUE(size == 0);
}

/**
 * @tc.name: notifier manager GetSystemloadLevel 001
 * @tc.desc: test the interface GetSystemloadLevel
 * @tc.type: FUNC
 * @tc.require: issueI97M6C
 * @tc.author:shanhaiyang
 */
HWTEST_F(NotifierMgrTest, GetSystemloadLevel001, Function | MediumTest | Level0)
{
    NotifierMgr::GetInstance().OnDeviceLevelChanged(0, 0);
    int32_t res = NotifierMgr::GetInstance().GetSystemloadLevel();
    EXPECT_TRUE(res == 0);
}

/**
 * @tc.name: notifier manager GetSystemloadLevel 002
 * @tc.desc: test the interface GetSystemloadLevel
 * @tc.type: FUNC
 * @tc.require: issueI97M6C
 * @tc.author:shanhaiyang
 */
HWTEST_F(NotifierMgrTest, GetSystemloadLevel002, Function | MediumTest | Level0)
{
    NotifierMgr::GetInstance().OnDeviceLevelChanged(0, 2);
    NotifierMgr::GetInstance().OnDeviceLevelChanged(1, 5);
    int32_t res = NotifierMgr::GetInstance().GetSystemloadLevel();
    EXPECT_TRUE(res == 2);
}

/**
 * @tc.name: notifier manager dump 001
 * @tc.desc: test the interface dump
 * @tc.type: FUNC
 * @tc.require: issueI97M6C
 * @tc.author:shanhaiyang
 */
HWTEST_F(NotifierMgrTest, Dump001, Function | MediumTest | Level0)
{
    auto res = NotifierMgr::GetInstance().DumpRegisterInfo();
    EXPECT_TRUE(res.size() == 0);
}

/**
 * @tc.name: notifier manager dump 002
 * @tc.desc: test the interface dump
 * @tc.type: FUNC
 * @tc.require: issueI97M6C
 * @tc.author:shanhaiyang
 */
HWTEST_F(NotifierMgrTest, Dump002, Function | MediumTest | Level0)
{
    sptr<IRemoteObject> notifier = new (std::nothrow) TestNotifierSystemloadListener();
    EXPECT_TRUE(notifier != nullptr);
    NotifierMgr::GetInstance().initialized_ = false;
    NotifierMgr::GetInstance().Init();
    NotifierMgr::GetInstance().RegisterNotifier(IPCSkeleton::GetCallingPid(), notifier);
    auto res = NotifierMgr::GetInstance().DumpRegisterInfo();
    EXPECT_TRUE(res.size() == 1);
    NotifierMgr::GetInstance().UnRegisterNotifier(IPCSkeleton::GetCallingPid());
    res = NotifierMgr::GetInstance().DumpRegisterInfo();
    EXPECT_TRUE(res.size() == 0);
}

/**
 * @tc.name: notifier manager deinit 001
 * @tc.desc: test the interface Deinit
 * @tc.type: FUNC
 * @tc.require: issueI97M6C
 * @tc.author:luolu
 */
HWTEST_F(NotifierMgrTest, Deinit001, Function | MediumTest | Level0)
{
    NotifierMgr::GetInstance().Init();
    EXPECT_TRUE(NotifierMgr::GetInstance().initialized_);
    NotifierMgr::GetInstance().Deinit();
}

/**
 * @tc.name: notifier manager OnRemoteNotifierDied 001
 * @tc.desc: test the interface OnRemoteNotifierDied
 * @tc.type: FUNC
 * @tc.require: issueI97M6C
 * @tc.author:luolu
 */
HWTEST_F(NotifierMgrTest, OnRemoteNotifierDied001, Function | MediumTest | Level0)
{
    sptr<IRemoteObject> notifier;
    NotifierMgr::GetInstance().OnRemoteNotifierDied(notifier);
    EXPECT_TRUE(NotifierMgr::GetInstance().notifierMap_.size() == 0);
}

/**
 * @tc.name: notifier manager OnRemoteNotifierDied 002
 * @tc.desc: test the interface OnRemoteNotifierDied
 * @tc.type: FUNC
 * @tc.require: issueI97M6C
 * @tc.author:luolu
 */
HWTEST_F(NotifierMgrTest, OnRemoteNotifierDied002, Function | MediumTest | Level0)
{
    sptr<IRemoteObject> notifier = new (std::nothrow) TestNotifierSystemloadListener();
    EXPECT_TRUE(notifier != nullptr);
    NotifierMgr::GetInstance().OnRemoteNotifierDied(notifier);
}

/**
 * @tc.name: notifier manager OnDeviceLevelChanged 001
 * @tc.desc: test the interface OnDeviceLevelChanged
 * @tc.type: FUNC
 * @tc.require: issueI97M6C
 * @tc.author:luolu
 */
HWTEST_F(NotifierMgrTest, OnDeviceLevelChanged001, Function | MediumTest | Level0)
{
    sptr<IRemoteObject> notifier = nullptr;
    NotifierMgr::GetInstance().OnDeviceLevelChanged(0, 2);
    EXPECT_EQ(NotifierMgr::GetInstance().systemloadLevel_, 2);
}

/**
 * @tc.name: notifier manager OnApplicationStateChange 001
 * @tc.desc: test the interface OnApplicationStateChange
 * @tc.type: FUNC
 * @tc.require: issueI97M6C
 * @tc.author:luolu
 */
HWTEST_F(NotifierMgrTest, OnApplicationStateChange001, Function | MediumTest | Level0)
{
    NotifierMgr::GetInstance().Init();
    NotifierMgr::GetInstance().OnApplicationStateChange(2, IPCSkeleton::GetCallingPid());
    EXPECT_TRUE(NotifierMgr::GetInstance().initialized_);
}

/**
 * @tc.name: notifier manager RemoveNotifierLock 001
 * @tc.desc: test the interface RemoveNotifierLock
 * @tc.type: FUNC
 * @tc.require: issueI97M6C
 * @tc.author:luolu
 */
HWTEST_F(NotifierMgrTest, RemoveNotifierLock001, Function | MediumTest | Level0)
{
    sptr<IRemoteObject> notifier = new (std::nothrow) TestNotifierSystemloadListener();
    EXPECT_TRUE(notifier != nullptr);
    NotifierMgr::GetInstance().RemoveNotifierLock(notifier);
}
} // namespace ResourceSchedule
} // namespace OHOS
