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
#include "res_sched_notifier_death_recipient.h"
#include "res_sched_systemload_notifier_proxy.h"
#include "res_sched_systemload_notifier_stub.h"

using namespace std;
using namespace testing::ext;

namespace OHOS {
namespace ResourceSchedule {
static constexpr int32_t APP_STATE_EXIT = 4;

class TestNotifierSystemloadListener : public ResSchedSystemloadNotifierStub {
public:
    TestNotifierSystemloadListener() = default;

    void OnSystemloadLevel(int32_t level)
    {
        testSystemloadLevel = level;
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
    NotifierMgr::GetInstance().Init();
    NotifierMgr::GetInstance().RegisterNotifier(IPCSkeleton::GetCallingPid(), notifier);
    auto res = NotifierMgr::GetInstance().DumpRegisterInfo();
    EXPECT_TRUE(res.size() == 1);
    NotifierMgr::GetInstance().UnRegisterNotifier(IPCSkeleton::GetCallingPid());
    res = NotifierMgr::GetInstance().DumpRegisterInfo();
    EXPECT_TRUE(res.size() == 0);
}
} // namespace ResourceSchedule
} // namespace OHOS
