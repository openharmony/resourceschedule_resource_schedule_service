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

#include "event_listener_mgr_test.h"

#include "ipc_skeleton.h"
#include "event_listener_mgr.h"
#include "res_sched_common_death_recipient.h"
#include "res_sched_event_listener_proxy.h"
#include "res_sched_event_listener_stub.h"
#include "res_type.h"

using namespace std;
using namespace testing::ext;

namespace OHOS {
namespace ResourceSchedule {
static constexpr int32_t APP_STATE_EXIT = 4;

class TestEventListener : public ResSchedEventListenerStub {
public:
    TestEventListener() = default;

    void OnReceiveEvent(uint32_t eventType, uint32_t eventValue, uint32_t listenerGroup, const nlohmann::json& extInfo)
    {
        eventType_ = eventType;
        eventValue_ = eventValue;
        listenerGroup_ = listenerGroup;
    }

    static uint32_t eventType_;
    static uint32_t eventValue_;
    static uint32_t listenerGroup_;
};
uint32_t TestEventListener::eventType_ = 0;
uint32_t TestEventListener::eventValue_ = 0;
uint32_t TestEventListener::listenerGroup_ = 0;
void EventListenerMgrTest::SetUpTestCase()
{
    EventListenerMgr::GetInstance().Init();
}

void EventListenerMgrTest::TearDownTestCase() {}

void EventListenerMgrTest::SetUp() {}

void EventListenerMgrTest::TearDown()
{
    EventListenerMgr::GetInstance().UnRegisterEventListener(IPCSkeleton::GetCallingPid(),
        ResType::EventType::EVENT_DRAW_FRAME_REPORT,
        ResType::EventListenerGroup::LISTENER_GROUP_COMMON);
    TestEventListener::eventType_ = 0;
    TestEventListener::eventValue_ = 0;
}

/**
 * @tc.name: eventListener manager TestEventListener 001
 * @tc.desc: test the interface TestEventListener
 * @tc.type: FUNC
 * @tc.require: issueIA9UZ9
 * @tc.author:baiheng
 */
HWTEST_F(EventListenerMgrTest, TestEventListener001, Function | MediumTest | Level0)
{
    sptr<IRemoteObject> eventListener = new (std::nothrow) TestEventListener();
    EXPECT_TRUE(eventListener != nullptr);
    EventListenerMgr::GetInstance().RegisterEventListener(IPCSkeleton::GetCallingPid(), eventListener,
        ResType::EventType::EVENT_DRAW_FRAME_REPORT,
        ResType::EventListenerGroup::LISTENER_GROUP_COMMON);
    nlohmann::json extInfo;
    EventListenerMgr::GetInstance().SendEvent(ResType::EventType::EVENT_DRAW_FRAME_REPORT,
        ResType::EventValue::EVENT_VALUE_DRAW_FRAME_REPORT_START, extInfo);
    sleep(1);
    EXPECT_TRUE(TestEventListener::eventType_ ==
        ResType::EventType::EVENT_DRAW_FRAME_REPORT);
    EXPECT_TRUE(TestEventListener::eventValue_ ==
        ResType::EventValue::EVENT_VALUE_DRAW_FRAME_REPORT_START);
}

/**
 * @tc.name: eventListener manager TestEventListener 002
 * @tc.desc: test the interface TestEventListener
 * @tc.type: FUNC
 * @tc.require: issueIA9UZ9
 * @tc.author:baiheng
 */
HWTEST_F(EventListenerMgrTest, TestEventListener002, Function | MediumTest | Level0)
{
    sptr<IRemoteObject> eventListener = new (std::nothrow) TestEventListener();
    EXPECT_TRUE(eventListener != nullptr);
    nlohmann::json extInfo;
    EventListenerMgr::GetInstance().RegisterEventListener(IPCSkeleton::GetCallingPid(), eventListener,
        ResType::EventType::EVENT_DRAW_FRAME_REPORT,
        ResType::EventListenerGroup::LISTENER_GROUP_COMMON);
    sleep(2);
    EventListenerMgr::GetInstance().SendEvent(ResType::EventType::EVENT_DRAW_FRAME_REPORT,
        ResType::EventValue::EVENT_VALUE_DRAW_FRAME_REPORT_STOP, extInfo);
    sleep(2);
    EXPECT_TRUE(TestEventListener::eventType_ ==
        ResType::EventType::EVENT_DRAW_FRAME_REPORT);
    EXPECT_TRUE(TestEventListener::eventValue_ ==
        ResType::EventValue::EVENT_VALUE_DRAW_FRAME_REPORT_STOP);
}

/**
 * @tc.name: eventListener manager RegisterEventListener 001
 * @tc.desc: test the interface RegisterEventListener
 * @tc.type: FUNC
 * @tc.require: issueIA9UZ9
 * @tc.author:baiheng
 */
HWTEST_F(EventListenerMgrTest, RegisterEventListener001, Function | MediumTest | Level0)
{
    sptr<IRemoteObject> eventListener = new (std::nothrow) TestEventListener();
    EXPECT_TRUE(eventListener != nullptr);
    sptr<IRemoteObject> eventListener1 = new (std::nothrow) TestEventListener();
    EXPECT_TRUE(eventListener != nullptr);
    auto callingPid = IPCSkeleton::GetCallingPid();
    EventListenerMgr::GetInstance().RegisterEventListener(callingPid, eventListener,
        ResType::EventType::EVENT_DRAW_FRAME_REPORT,
        ResType::EventListenerGroup::LISTENER_GROUP_COMMON);
    EventListenerMgr::GetInstance().RegisterEventListener(callingPid, eventListener1,
        ResType::EventType::EVENT_DRAW_FRAME_REPORT,
        ResType::EventListenerGroup::LISTENER_GROUP_COMMON);
    auto realEventListener = EventListenerMgr::GetInstance().
        eventListenerMap_[ResType::EventType::EVENT_DRAW_FRAME_REPORT][callingPid].listener;
    EXPECT_TRUE(eventListener.GetRefPtr() == realEventListener.GetRefPtr());
}

/**
 * @tc.name: eventListener manager RegisterEventListener 002
 * @tc.desc: test the interface RegisterEventListener
 * @tc.type: FUNC
 * @tc.require: issueIA9UZ9
 * @tc.author:baiheng
 */
HWTEST_F(EventListenerMgrTest, RegisterEventListener002, Function | MediumTest | Level0)
{
    sptr<IRemoteObject> eventListener = new (std::nothrow) TestEventListener();
    EXPECT_TRUE(eventListener != nullptr);
    auto callingPid = IPCSkeleton::GetCallingPid();
    EventListenerMgr::GetInstance().RegisterEventListener(callingPid, eventListener,
        ResType::EventType::EVENT_DRAW_FRAME_REPORT,
        ResType::EventListenerGroup::LISTENER_GROUP_COMMON);
    EventListenerMgr::GetInstance().RegisterEventListener(callingPid, eventListener,
        ResType::EventType::EVENT_DRAW_FRAME_REPORT,
        ResType::EventListenerGroup::LISTENER_GROUP_COMMON);
    auto size = EventListenerMgr::GetInstance().eventListenerMap_.size();
    EXPECT_TRUE(size == 1);
}

/**
 * @tc.name: eventListener manager RegisterEventListener 003
 * @tc.desc: test the interface RegisterEventListener
 * @tc.type: FUNC
 * @tc.require: issueIA9UZ9
 * @tc.author:baiheng
 */
HWTEST_F(EventListenerMgrTest, RegisterEventListener003, Function | MediumTest | Level0)
{
    sptr<IRemoteObject> eventListener;
    auto callingPid = IPCSkeleton::GetCallingPid();
    EventListenerMgr::GetInstance().RegisterEventListener(callingPid, eventListener,
        ResType::EventType::EVENT_DRAW_FRAME_REPORT,
        ResType::EventListenerGroup::LISTENER_GROUP_COMMON);
    EXPECT_TRUE(eventListener == nullptr);
}

/**
 * @tc.name: eventListener manager RegisterEventListener 004
 * @tc.desc: test the interface RegisterEventListener
 * @tc.type: FUNC
 * @tc.require: issueIA9UZ9
 * @tc.author:baiheng
 */
HWTEST_F(EventListenerMgrTest, RegisterEventListener004, Function | MediumTest | Level0)
{
    sptr<IRemoteObject> eventListener = new (std::nothrow) TestEventListener();
    EXPECT_TRUE(eventListener != nullptr);
    auto callingPid = IPCSkeleton::GetCallingPid();
    EventListenerMgr::GetInstance().eventListenerDeathRecipient_ = nullptr;
    EventListenerMgr::GetInstance().RegisterEventListener(callingPid, eventListener,
        ResType::EventType::EVENT_DRAW_FRAME_REPORT,
        ResType::EventListenerGroup::LISTENER_GROUP_COMMON);
}

/**
 * @tc.name: eventListener manager UnRegisterEventListener 001
 * @tc.desc: test the interface UnRegisterEventListener
 * @tc.type: FUNC
 * @tc.require: issueIA9UZ9
 * @tc.author:baiheng
 */
HWTEST_F(EventListenerMgrTest, UnRegisterEventListener001, Function | MediumTest | Level0)
{
    sptr<IRemoteObject> eventListener = new (std::nothrow) TestEventListener();
    EXPECT_TRUE(eventListener != nullptr);
    EventListenerMgr::GetInstance().Init();
    auto callingPid = IPCSkeleton::GetCallingPid();
    EventListenerMgr::GetInstance().RegisterEventListener(callingPid, eventListener,
        ResType::EventType::EVENT_DRAW_FRAME_REPORT,
        ResType::EventListenerGroup::LISTENER_GROUP_COMMON);
    EventListenerMgr::GetInstance().UnRegisterEventListener(callingPid,
        ResType::EventType::EVENT_DRAW_FRAME_REPORT,
        ResType::EventListenerGroup::LISTENER_GROUP_COMMON);
    auto size = EventListenerMgr::GetInstance().eventListenerMap_.size();
    EXPECT_TRUE(size == 0);
}

/**
 * @tc.name: eventListener manager UnRegisterEventListener 002
 * @tc.desc: test the interface UnRegisterEventListener
 * @tc.type: FUNC
 * @tc.require: issueIA9UZ9
 * @tc.author:baiheng
 */
HWTEST_F(EventListenerMgrTest, UnRegisterEventListener002, Function | MediumTest | Level0)
{
    EventListenerMgr::GetInstance().UnRegisterEventListener(IPCSkeleton::GetCallingPid(),
        ResType::EventType::EVENT_DRAW_FRAME_REPORT,
        ResType::EventListenerGroup::LISTENER_GROUP_COMMON);
    auto size = EventListenerMgr::GetInstance().eventListenerMap_.size();
    EXPECT_TRUE(size == 0);
}


/**
 * @tc.name: eventListener manager dump 001
 * @tc.desc: test the interface dump
 * @tc.type: FUNC
 * @tc.require: issueIA9UZ9
 * @tc.author:baiheng
 */
HWTEST_F(EventListenerMgrTest, Dump001, Function | MediumTest | Level0)
{
    auto res = EventListenerMgr::GetInstance().DumpRegisterInfo();
    EXPECT_TRUE(res.size() == 0);
}

/**
 * @tc.name: eventListener manager dump 002
 * @tc.desc: test the interface dump
 * @tc.type: FUNC
 * @tc.require: issueIA9UZ9
 * @tc.author:baiheng
 */
HWTEST_F(EventListenerMgrTest, Dump002, Function | MediumTest | Level0)
{
    sptr<IRemoteObject> eventListener = new (std::nothrow) TestEventListener();
    EXPECT_TRUE(eventListener != nullptr);
    EventListenerMgr::GetInstance().initialized_ = false;
    EventListenerMgr::GetInstance().Init();
    EventListenerMgr::GetInstance().RegisterEventListener(IPCSkeleton::GetCallingPid(),
        eventListener, ResType::EventType::EVENT_DRAW_FRAME_REPORT,
        ResType::EventListenerGroup::LISTENER_GROUP_COMMON);
    auto res = EventListenerMgr::GetInstance().DumpRegisterInfo();
    EXPECT_TRUE(res.size() == 1);
    EventListenerMgr::GetInstance().UnRegisterEventListener(IPCSkeleton::GetCallingPid(),
         ResType::EventType::EVENT_DRAW_FRAME_REPORT,
         ResType::EventListenerGroup::LISTENER_GROUP_COMMON);
    res = EventListenerMgr::GetInstance().DumpRegisterInfo();
    EXPECT_TRUE(res.size() == 0);
}

/**
 * @tc.name: eventListener manager deinit 001
 * @tc.desc: test the interface Deinit
 * @tc.type: FUNC
 * @tc.require: issueIA9UZ9
 * @tc.author:baiheng
 */
HWTEST_F(EventListenerMgrTest, Deinit001, Function | MediumTest | Level0)
{
    EventListenerMgr::GetInstance().Init();
    EventListenerMgr::GetInstance().Deinit();
}

/**
 * @tc.name: eventListener manager OnRemoteeventListenerDied 001
 * @tc.desc: test the interface OnRemoteeventListenerDied
 * @tc.type: FUNC
 * @tc.require: issueIA9UZ9
 * @tc.author:baiheng
 */
HWTEST_F(EventListenerMgrTest, OnRemoteEventListenerDied001, Function | MediumTest | Level0)
{
    sptr<IRemoteObject> eventListener;
    EventListenerMgr::GetInstance().OnRemoteListenerDied(eventListener);
    EXPECT_EQ(EventListenerMgr::GetInstance().eventListenerMap_.size(), 0);
}

/**
 * @tc.name: eventListener manager OnRemoteeventListenerDied 002
 * @tc.desc: test the interface OnRemoteeventListenerDied
 * @tc.type: FUNC
 * @tc.require: issueIA9UZ9
 * @tc.author:baiheng
 */
HWTEST_F(EventListenerMgrTest, OnRemoteeventListenerDied002, Function | MediumTest | Level0)
{
    sptr<IRemoteObject> eventListener = new (std::nothrow) TestEventListener();
    EXPECT_TRUE(eventListener != nullptr);
    EventListenerMgr::GetInstance().OnRemoteListenerDied(eventListener);
    SUCCEED();
}

/**
 * @tc.name: eventListener manager SendEvent 001
 * @tc.desc: test the interface SendEvent
 * @tc.type: FUNC
 * @tc.require: issueIA9UZ9
 * @tc.author:baiheng
 */
HWTEST_F(EventListenerMgrTest, SendEvent001, Function | MediumTest | Level0)
{
    sptr<IRemoteObject> eventListener = nullptr;
    nlohmann::json extInfo;
    EventListenerMgr::GetInstance().SendEvent(ResType::EventType::EVENT_DRAW_FRAME_REPORT,
        ResType::EventValue::EVENT_VALUE_DRAW_FRAME_REPORT_STOP, extInfo);
    EXPECT_TRUE(eventListener == nullptr);
}
} // namespace ResourceSchedule
} // namespace OHOS