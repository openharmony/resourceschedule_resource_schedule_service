/*
 * Copyright (c) 2025-2025 Huawei Device Co., Ltd.
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
#include <thread>

#include "res_type.h"
#include "res_sched_mgr.h"
#include "scene_recognizer_mgr.h"
#include "slide_recognizer.h"

namespace OHOS {
namespace ResourceSchedule {
using namespace std;
using namespace testing::ext;
class SlideRecognizeTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
    static std::shared_ptr<SlideRecognizer> slideRecognizer_;
};
std::shared_ptr<SlideRecognizer> SlideRecognizeTest::slideRecognizer_ = nullptr;

void SlideRecognizeTest::SetUpTestCase(void)
{
    slideRecognizer_ = std::static_pointer_cast<SlideRecognizer>(SceneRecognizerMgr::GetInstance().
        sceneRecognizers_[RecognizeType::SLIDE_RECOGNIZER]);
}

void SlideRecognizeTest::TearDownTestCase() {}

void SlideRecognizeTest::SetUp() {}

void SlideRecognizeTest::TearDown() {}

/**
* @tc.name: NormalSlideTest001
* @tc.desc: test normal slide scene.
* @tc.type: FUNC
* @tc.require: #IC5T7D
* @tc.author: baiheng
*/
HWTEST_F(SlideRecognizeTest, NormalSlideTest001, Function | MediumTest | Level0)
{
    nlohmann::json payload;
    payload["clientPid"] = "1000";
    payload["callingUid"] = "2000";
    //初始状态 IDLE
    EXPECT_EQ(slideRecognizer_->GetSlideStatus(), SlideRecognizeStat::IDLE);
    ResSchedMgr::GetInstance().ReportData(ResType::RES_TYPE_SLIDE_RECOGNIZE,
        ResType::SlideEventStatus::SLIDE_EVENT_DETECTING, payload);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    //收到滑动探测事件 状态变化为DETECTING
    EXPECT_EQ(slideRecognizer_->GetSlideStatus(), SlideRecognizeStat::SLIDE_NORMAL_DETECTING);
    ResSchedMgr::GetInstance().ReportData(ResType::RES_TYPE_SEND_FRAME_EVENT, 0, payload);
    std::this_thread::sleep_for(std::chrono::milliseconds(2));
    //收到送帧上屏事件，且未离手，识别到此次为拖滑，状态变化为拖滑状态
    EXPECT_EQ(slideRecognizer_->GetSlideStatus(), SlideRecognizeStat::SLIDE_NORMAL);
    payload["up_speed"] = "0";
    ResSchedMgr::GetInstance().ReportData(ResType::RES_TYPE_CLICK_RECOGNIZE,
        ResType::ClickEventType::TOUCH_EVENT_UP, payload);
    ResSchedMgr::GetInstance().ReportData(ResType::RES_TYPE_CLICK_RECOGNIZE,
        ResType::ClickEventType::TOUCH_EVENT_DOWN, payload);
    std::this_thread::sleep_for(std::chrono::milliseconds(2));
    //离手速度为0，拖滑结束，状态变化为IDLE
    EXPECT_EQ(slideRecognizer_->GetSlideStatus(), SlideRecognizeStat::IDLE);
}

/**
* @tc.name: NormalSlideTest002
* @tc.desc: test normal slide scene.
* @tc.type: FUNC
* @tc.require: #IC5T7D
* @tc.author: baiheng
*/
HWTEST_F(SlideRecognizeTest, NormalSlideTest002, Function | MediumTest | Level0)
{
    nlohmann::json payload;
    payload["clientPid"] = "1000";
    payload["callingUid"] = "2000";
    //初始状态 IDLE
    EXPECT_EQ(slideRecognizer_->GetSlideStatus(), SlideRecognizeStat::IDLE);
    ResSchedMgr::GetInstance().ReportData(ResType::RES_TYPE_SLIDE_RECOGNIZE,
        ResType::SlideEventStatus::SLIDE_EVENT_DETECTING, payload);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    //收到滑动探测事件 状态变化为DETECTING
    EXPECT_EQ(slideRecognizer_->GetSlideStatus(), SlideRecognizeStat::SLIDE_NORMAL_DETECTING);
    //离手
    payload["up_speed"] = "0";
    ResSchedMgr::GetInstance().ReportData(ResType::RES_TYPE_CLICK_RECOGNIZE,
        ResType::ClickEventType::TOUCH_EVENT_UP, payload);
    ResSchedMgr::GetInstance().ReportData(ResType::RES_TYPE_SEND_FRAME_EVENT, 0, payload);
    std::this_thread::sleep_for(std::chrono::milliseconds(2));
    //收到送帧上屏事件，但已离手，不识别为拖滑，状态变化为IDLE
    EXPECT_EQ(slideRecognizer_->GetSlideStatus(), SlideRecognizeStat::IDLE);
}

/**
* @tc.name: NormalSlideTest003
* @tc.desc: test normal slide scene.
* @tc.type: FUNC
* @tc.require: #IC5T7D
* @tc.author: baiheng
*/
HWTEST_F(SlideRecognizeTest, NormalSlideTest003, Function | MediumTest | Level0)
{
    nlohmann::json payload;
    payload["clientPid"] = "1000";
    payload["callingUid"] = "2000";
    //初始状态 IDLE
    EXPECT_EQ(slideRecognizer_->GetSlideStatus(), SlideRecognizeStat::IDLE);
    ResSchedMgr::GetInstance().ReportData(ResType::RES_TYPE_SLIDE_RECOGNIZE,
        ResType::SlideEventStatus::SLIDE_EVENT_DETECTING, payload);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    //收到滑动探测事件 状态变化为DETECTING
    EXPECT_EQ(slideRecognizer_->GetSlideStatus(), SlideRecognizeStat::SLIDE_NORMAL_DETECTING);
    payload["up_speed"] = "0";
    ResSchedMgr::GetInstance().ReportData(ResType::RES_TYPE_CLICK_RECOGNIZE,
        ResType::ClickEventType::TOUCH_EVENT_UP, payload);
    ResSchedMgr::GetInstance().ReportData(ResType::RES_TYPE_CLICK_RECOGNIZE,
        ResType::ClickEventType::TOUCH_EVENT_DOWN, payload);
    std::this_thread::sleep_for(std::chrono::milliseconds(2));
    //离手速度为0，中间未收到送帧上屏事件，拖滑结束，状态变化为IDLE
    EXPECT_EQ(slideRecognizer_->GetSlideStatus(), SlideRecognizeStat::IDLE);
}

/**
* @tc.name: ListFlingTest001
* @tc.desc: test list fling scene.
* @tc.type: FUNC
* @tc.require: #IC5T7D
* @tc.author: baiheng
*/
HWTEST_F(SlideRecognizeTest, ListFlingTest001, Function | MediumTest | Level0)
{
    nlohmann::json payload;
    payload["clientPid"] = "1000";
    payload["callingUid"] = "2000";
    //初始状态 IDLE
    EXPECT_EQ(slideRecognizer_->GetSlideStatus(), SlideRecognizeStat::IDLE);
    ResSchedMgr::GetInstance().ReportData(ResType::RES_TYPE_SLIDE_RECOGNIZE,
        ResType::SlideEventStatus::SLIDE_EVENT_DETECTING, payload);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    //收到滑动探测事件 状态变化为DETECTING
    EXPECT_EQ(slideRecognizer_->GetSlideStatus(), SlideRecognizeStat::SLIDE_NORMAL_DETECTING);
    ResSchedMgr::GetInstance().ReportData(ResType::RES_TYPE_SEND_FRAME_EVENT, 0, payload);
    std::this_thread::sleep_for(std::chrono::milliseconds(2));
    //收到送帧上屏事件，且未离手，识别到此次为拖滑，状态变化为拖滑状态
    EXPECT_EQ(slideRecognizer_->GetSlideStatus(), SlideRecognizeStat::SLIDE_NORMAL);
    payload["up_speed"] = "10.0";
    ResSchedMgr::GetInstance().ReportData(ResType::RES_TYPE_CLICK_RECOGNIZE,
        ResType::ClickEventType::TOUCH_EVENT_UP, payload);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    //离手速度大于检测阈值，识别为抛滑，状态变化为LIST_FLING
    EXPECT_EQ(slideRecognizer_->GetSlideStatus(), SlideRecognizeStat::LIST_FLING);
    //模拟 2s内一直产生送帧事件
    for (int i = 0; i < 100; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        ResSchedMgr::GetInstance().ReportData(ResType::RES_TYPE_SEND_FRAME_EVENT, 0, payload);
        EXPECT_EQ(slideRecognizer_->GetSlideStatus(), SlideRecognizeStat::LIST_FLING);
    }
    //100ms内未收到送帧事件 识别未抛滑结束
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    EXPECT_EQ(slideRecognizer_->GetSlideStatus(), SlideRecognizeStat::IDLE);
}

/**
* @tc.name: ListFlingTest002
* @tc.desc: test list fling scene.
* @tc.type: FUNC
* @tc.require: #IC5T7D
* @tc.author: baiheng
*/
HWTEST_F(SlideRecognizeTest, ListFlingTest002, Function | MediumTest | Level0)
{
    nlohmann::json payload;
    payload["clientPid"] = "1000";
    payload["callingUid"] = "2000";
    //初始状态 IDLE
    EXPECT_EQ(slideRecognizer_->GetSlideStatus(), SlideRecognizeStat::IDLE);
    ResSchedMgr::GetInstance().ReportData(ResType::RES_TYPE_SLIDE_RECOGNIZE,
        ResType::SlideEventStatus::SLIDE_EVENT_DETECTING, payload);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    //收到滑动探测事件 状态变化为DETECTING
    EXPECT_EQ(slideRecognizer_->GetSlideStatus(), SlideRecognizeStat::SLIDE_NORMAL_DETECTING);
    ResSchedMgr::GetInstance().ReportData(ResType::RES_TYPE_SEND_FRAME_EVENT, 0, payload);
    std::this_thread::sleep_for(std::chrono::milliseconds(2));
    //收到送帧上屏事件，且未离手，识别到此次为拖滑，状态变化为拖滑状态
    EXPECT_EQ(slideRecognizer_->GetSlideStatus(), SlideRecognizeStat::SLIDE_NORMAL);
    payload["up_speed"] = "10.0";
    ResSchedMgr::GetInstance().ReportData(ResType::RES_TYPE_CLICK_RECOGNIZE,
        ResType::ClickEventType::TOUCH_EVENT_UP, payload);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    //离手速度大于检测阈值，识别为抛滑，状态变化为LIST_FLING
    EXPECT_EQ(slideRecognizer_->GetSlideStatus(), SlideRecognizeStat::LIST_FLING);
    //模拟 4s内一直产生送帧事件
    for (int i = 0; i < 400; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        ResSchedMgr::GetInstance().ReportData(ResType::RES_TYPE_SEND_FRAME_EVENT, 0, payload);
    }
    //单次抛滑超过3s，超时结束
    EXPECT_EQ(slideRecognizer_->GetSlideStatus(), SlideRecognizeStat::IDLE);
}

/**
* @tc.name: ListFlingTest003
* @tc.desc: test list fling scene.
* @tc.type: FUNC
* @tc.require: #IC5T7D
* @tc.author: baiheng
*/
HWTEST_F(SlideRecognizeTest, ListFlingTest003, Function | MediumTest | Level0)
{
    nlohmann::json payload;
    payload["clientPid"] = "1000";
    payload["callingUid"] = "2000";
    //初始状态 IDLE
    EXPECT_EQ(slideRecognizer_->GetSlideStatus(), SlideRecognizeStat::IDLE);
    ResSchedMgr::GetInstance().ReportData(ResType::RES_TYPE_SLIDE_RECOGNIZE,
        ResType::SlideEventStatus::SLIDE_EVENT_DETECTING, payload);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    //收到滑动探测事件 状态变化为DETECTING
    EXPECT_EQ(slideRecognizer_->GetSlideStatus(), SlideRecognizeStat::SLIDE_NORMAL_DETECTING);
    ResSchedMgr::GetInstance().ReportData(ResType::RES_TYPE_SEND_FRAME_EVENT, 0, payload);
    std::this_thread::sleep_for(std::chrono::milliseconds(2));
    //收到送帧上屏事件，且未离手，识别到此次为拖滑，状态变化为拖滑状态
    EXPECT_EQ(slideRecognizer_->GetSlideStatus(), SlideRecognizeStat::SLIDE_NORMAL);
    payload["up_speed"] = "10.0";
    ResSchedMgr::GetInstance().ReportData(ResType::RES_TYPE_CLICK_RECOGNIZE,
        ResType::ClickEventType::TOUCH_EVENT_UP, payload);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    //离手速度大于检测阈值，识别为抛滑，状态变化为LIST_FLING
    EXPECT_EQ(slideRecognizer_->GetSlideStatus(), SlideRecognizeStat::LIST_FLING);
    //模拟 1s内一直产生送帧事件
    for (int i = 0; i < 100; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        ResSchedMgr::GetInstance().ReportData(ResType::RES_TYPE_SEND_FRAME_EVENT, 0, payload);
        EXPECT_EQ(slideRecognizer_->GetSlideStatus(), SlideRecognizeStat::LIST_FLING);
    }
    //模拟连续抛滑场景
    ResSchedMgr::GetInstance().ReportData(ResType::TOUCH_EVENT_DOWN,
        ResType::ClickEventType::TOUCH_EVENT_UP, payload);
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
    ResSchedMgr::GetInstance().ReportData(ResType::RES_TYPE_SLIDE_RECOGNIZE,
        ResType::SlideEventStatus::SLIDE_EVENT_DETECTING, payload);
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
    EXPECT_EQ(slideRecognizer_->GetSlideStatus(), SlideRecognizeStat::SLIDE_NORMAL_DETECTING);
    ResSchedMgr::GetInstance().ReportData(ResType::RES_TYPE_SEND_FRAME_EVENT, 0, payload);
    std::this_thread::sleep_for(std::chrono::milliseconds(2));
    //收到送帧上屏事件，且未离手，识别到此次为拖滑，状态变化为拖滑状态
    EXPECT_EQ(slideRecognizer_->GetSlideStatus(), SlideRecognizeStat::SLIDE_NORMAL);
    payload["up_speed"] = "10.0";
    ResSchedMgr::GetInstance().ReportData(ResType::RES_TYPE_CLICK_RECOGNIZE,
        ResType::ClickEventType::TOUCH_EVENT_UP, payload);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    //离手速度大于检测阈值，识别为抛滑，状态变化为LIST_FLING
    EXPECT_EQ(slideRecognizer_->GetSlideStatus(), SlideRecognizeStat::LIST_FLING);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    //超过100ms未收到送帧，识别抛滑结束
    EXPECT_EQ(slideRecognizer_->GetSlideStatus(), SlideRecognizeStat::IDLE);
}

/**
* @tc.name: ListFlingTest004
* @tc.desc: test list fling scene.
* @tc.type: FUNC
* @tc.require: #IC5T7D
* @tc.author: baiheng
*/
HWTEST_F(SlideRecognizeTest, ListFlingTest004, Function | MediumTest | Level0)
{
    nlohmann::json payload;
    payload["clientPid"] = "1000";
    payload["callingUid"] = "2000";
    //初始状态 IDLE
    EXPECT_EQ(slideRecognizer_->GetSlideStatus(), SlideRecognizeStat::IDLE);
    ResSchedMgr::GetInstance().ReportData(ResType::RES_TYPE_SLIDE_RECOGNIZE,
        ResType::SlideEventStatus::SLIDE_EVENT_DETECTING, payload);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    //收到滑动探测事件 状态变化为DETECTING
    EXPECT_EQ(slideRecognizer_->GetSlideStatus(), SlideRecognizeStat::SLIDE_NORMAL_DETECTING);
    std::this_thread::sleep_for(std::chrono::milliseconds(2));
    payload["up_speed"] = "10.0";
    ResSchedMgr::GetInstance().ReportData(ResType::RES_TYPE_CLICK_RECOGNIZE,
        ResType::ClickEventType::TOUCH_EVENT_UP, payload);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    //离手速度大于检测阈值，且未收到送帧，但仍处于滑动探测状态，识别为抛滑，状态变化为LIST_FLING
    EXPECT_EQ(slideRecognizer_->GetSlideStatus(), SlideRecognizeStat::LIST_FLING);
    //模拟 4s内一直产生送帧事件
    for (int i = 0; i < 400; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        ResSchedMgr::GetInstance().ReportData(ResType::RES_TYPE_SEND_FRAME_EVENT, 0, payload);
    }
    //单次抛滑超过3s，超时结束
    EXPECT_EQ(slideRecognizer_->GetSlideStatus(), SlideRecognizeStat::IDLE);
}

/**
* @tc.name: ListFlingTest005
* @tc.desc: test list fling scene.
* @tc.type: FUNC
* @tc.require: #IC5T7D
* @tc.author: baiheng
*/
HWTEST_F(SlideRecognizeTest, ListFlingTest005, Function | MediumTest | Level0)
{
    nlohmann::json payload;
    payload["clientPid"] = "1000";
    payload["calling_uid"] = "2000";
    //初始状态 IDLE
    EXPECT_EQ(slideRecognizer_->GetSlideStatus(), SlideRecognizeStat::IDLE);
    ResSchedMgr::GetInstance().ReportData(ResType::RES_TYPE_AXIS_EVENT,
        ResType::AxisEventStatus::AXIS_EVENT_END, payload);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    EXPECT_EQ(slideRecognizer_->isInTouching_, false);
}

/**
* @tc.name: ListFlingTest006
* @tc.desc: test list fling scene.
* @tc.type: FUNC
* @tc.require: #IC5T7D
* @tc.author: baiheng
*/
HWTEST_F(SlideRecognizeTest, ListFlingTest006, Function | MediumTest | Level0)
{
    nlohmann::json payload;
    payload["clientPid"] = "1000";
    payload["calling_uid"] = "2000";
    //初始状态 IDLE
    EXPECT_EQ(slideRecognizer_->GetSlideStatus(), SlideRecognizeStat::IDLE);
    ResSchedMgr::GetInstance().ReportData(ResType::RES_TYPE_CLICK_RECOGNIZE,
        ResType::ClickEventType::TOUCH_EVENT_PULL_UP, payload);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    EXPECT_EQ(slideRecognizer_->isInTouching_, false);
}

/**
* @tc.name: NormalSlideTest004
* @tc.desc: test normal slide scene.
* @tc.type: FUNC
* @tc.require: #IC5T7D
* @tc.author: baiheng
*/
HWTEST_F(SlideRecognizeTest, NormalSlideTest004, Function | MediumTest | Level0)
{
    nlohmann::json payload;
    //初始状态 IDLE
    EXPECT_EQ(slideRecognizer_->GetSlideStatus(), SlideRecognizeStat::IDLE);
    ResSchedMgr::GetInstance().ReportData(ResType::RES_TYPE_SLIDE_RECOGNIZE,
        ResType::SlideEventStatus::SLIDE_EVENT_DETECTING, payload);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    ResSchedMgr::GetInstance().ReportData(ResType::RES_TYPE_SEND_FRAME_EVENT, 0, payload);
    std::this_thread::sleep_for(std::chrono::milliseconds(2));
    payload["up_speed"] = "0";
    ResSchedMgr::GetInstance().ReportData(ResType::RES_TYPE_CLICK_RECOGNIZE,
        ResType::ClickEventType::TOUCH_EVENT_UP, payload);
    ResSchedMgr::GetInstance().ReportData(ResType::RES_TYPE_CLICK_RECOGNIZE,
        ResType::ClickEventType::TOUCH_EVENT_DOWN, payload);
    std::this_thread::sleep_for(std::chrono::milliseconds(2));
}

/**
* @tc.name: NormalSlideTest005
* @tc.desc: test normal slide scene.
* @tc.type: FUNC
* @tc.require: #IC5T7D
* @tc.author: baiheng
*/
HWTEST_F(SlideRecognizeTest, NormalSlideTest005, Function | MediumTest | Level0)
{
    nlohmann::json payload;
    payload["clientPid"] = 1000;
    payload["calling_uid"] = 2000;
    //初始状态 IDLE
    EXPECT_EQ(slideRecognizer_->GetSlideStatus(), SlideRecognizeStat::IDLE);
    ResSchedMgr::GetInstance().ReportData(ResType::RES_TYPE_SLIDE_RECOGNIZE,
        ResType::SlideEventStatus::SLIDE_EVENT_DETECTING, payload);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    ResSchedMgr::GetInstance().ReportData(ResType::RES_TYPE_SEND_FRAME_EVENT, 0, payload);
    std::this_thread::sleep_for(std::chrono::milliseconds(2));
    payload["up_speed"] = "0";
    ResSchedMgr::GetInstance().ReportData(ResType::RES_TYPE_CLICK_RECOGNIZE,
        ResType::ClickEventType::TOUCH_EVENT_UP, payload);
    ResSchedMgr::GetInstance().ReportData(ResType::RES_TYPE_CLICK_RECOGNIZE,
        ResType::ClickEventType::TOUCH_EVENT_DOWN, payload);
    std::this_thread::sleep_for(std::chrono::milliseconds(2));
}
} // namespace ResourceSchedule
} // namespace OHOS