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

#include "background_sensitive_task_overlapping_scene_recognizer.h"
#include "gtest/gtest.h"
#include "res_type.h"
#include "scene_recognize_test.h"
#include "scene_recognizer_mgr.h"
#include "slide_recognizer.h"
#include "continuous_app_install_recognizer.h"
#include "system_upgrade_scene_recognizer.h"

using namespace std;
using namespace testing::ext;

namespace OHOS {
namespace ResourceSchedule {
static uint32_t g_slideState = SlideRecognizeStat::IDLE;
void SceneRecognizeTest::SetUpTestCase() {}

void SceneRecognizeTest::TearDownTestCase() {}

void SceneRecognizeTest::SetUp() {}

void SceneRecognizeTest::TearDown() {}

/**
 * @tc.name: SceneRecognizerMgr AppInstallTest test
 * @tc.desc: test the interface DispatchResource
 * @tc.type: FUNC
 * @tc.require: issueI9S1FA
 * @tc.author:baiheng
 */
HWTEST_F(SceneRecognizeTest, AppInstallTest001, Function | MediumTest | Level0)
{
    nlohmann::json payload;
    SceneRecognizerMgr::GetInstance().DispatchResource(ResType::RES_TYPE_SCREEN_STATUS,
        ResType::ScreenStatus::SCREEN_ON, payload);
    EXPECT_NE(SceneRecognizerMgr::GetInstance().sceneRecognizers_.size(), 0);
    SceneRecognizerMgr::GetInstance().DispatchResource(-1, -1, payload);
    EXPECT_NE(SceneRecognizerMgr::GetInstance().sceneRecognizers_.size(), 0);
    SceneRecognizerMgr::GetInstance().DispatchResource(ResType::RES_TYPE_APP_INSTALL_UNINSTALL,
        ResType::AppInstallStatus::APP_INSTALL_END, payload);
    EXPECT_NE(SceneRecognizerMgr::GetInstance().sceneRecognizers_.size(), 0);
    SceneRecognizerMgr::GetInstance().DispatchResource(ResType::RES_TYPE_APP_INSTALL_UNINSTALL,
        ResType::AppInstallStatus::APP_INSTALL_END, payload);
    EXPECT_NE(SceneRecognizerMgr::GetInstance().sceneRecognizers_.size(), 0);
}

/**
 * @tc.name: ContinuousAppInstallRecognizer AppInstallTest test
 * @tc.desc: test the interface OnDispatchResource
 * @tc.type: FUNC
 * @tc.require: issuesIAJZVI
 * @tc.author: fengyang
 */
HWTEST_F(SceneRecognizeTest, AppInstallTest002, Function | MediumTest | Level0)
{
    auto continuousAppInstallRecognizer = std::make_shared<ContinuousAppInstallRecognizer>();
    nlohmann::json payload;
    continuousAppInstallRecognizer->OnDispatchResource(ResType::RES_TYPE_APP_INSTALL_UNINSTALL,
        ResType::AppInstallStatus::APP_INSTALL_END, payload);
    continuousAppInstallRecognizer->OnDispatchResource(ResType::RES_TYPE_APP_INSTALL_UNINSTALL,
        ResType::AppInstallStatus::APP_INSTALL_END, payload);
    continuousAppInstallRecognizer->OnDispatchResource(ResType::RES_TYPE_APP_INSTALL_UNINSTALL,
        ResType::AppInstallStatus::APP_INSTALL_START, payload);
    EXPECT_NE(continuousAppInstallRecognizer, nullptr);
}

/**
 * @tc.name: SceneRecognizerMgr Bgtask test
 * @tc.desc: test the interface DispatchResource
 * @tc.type: FUNC
 * @tc.require: issueI9S1FA
 * @tc.author:baiheng
 */
HWTEST_F(SceneRecognizeTest, BgtaskTest001, Function | MediumTest | Level0)
{
    auto bgtaskRecognizer = std::make_shared<BackgroundSensitiveTaskOverlappingSceneRecognizer>();
    nlohmann::json payload;
    payload["pid"] = "2000";
    bgtaskRecognizer->OnDispatchResource(ResType::RES_TYPE_REPORT_SCENE_BOARD,
        0, payload);
    EXPECT_EQ(bgtaskRecognizer->sceneboardPid_, 2000);
    EXPECT_EQ(bgtaskRecognizer->isInBackgroundPerceivableScene_, false);
    payload["pid"] = "3000";
    bgtaskRecognizer->OnDispatchResource(ResType::RES_TYPE_APP_STATE_CHANGE,
        ResType::ProcessStatus::PROCESS_FOREGROUND, payload);
    EXPECT_EQ(bgtaskRecognizer->foregroundPid_, 3000);
    EXPECT_EQ(bgtaskRecognizer->isInBackgroundPerceivableScene_, false);
    payload["pid"] = "4000";
    payload["typeIds"] = { 2 };
    bgtaskRecognizer->OnDispatchResource(ResType::RES_TYPE_CONTINUOUS_TASK,
        ResType::ContinuousTaskStatus::CONTINUOUS_TASK_START, payload);
    EXPECT_EQ(bgtaskRecognizer->isInBackgroundPerceivableScene_, true);
    payload["pid"] = "2000";
    bgtaskRecognizer->OnDispatchResource(ResType::RES_TYPE_APP_STATE_CHANGE,
        ResType::ProcessStatus::PROCESS_FOREGROUND, payload);
    EXPECT_EQ(bgtaskRecognizer->isInBackgroundPerceivableScene_, false);
    payload["pid"] = "3000";
    bgtaskRecognizer->OnDispatchResource(ResType::RES_TYPE_APP_STATE_CHANGE,
        ResType::ProcessStatus::PROCESS_FOREGROUND, payload);
    EXPECT_EQ(bgtaskRecognizer->isInBackgroundPerceivableScene_, true);
    payload["pid"] = "4000";
    payload["typeIds"] = { 1 };
    bgtaskRecognizer->OnDispatchResource(ResType::RES_TYPE_CONTINUOUS_TASK,
        ResType::ContinuousTaskStatus::CONTINUOUS_TASK_UPDATE, payload);
    EXPECT_EQ(bgtaskRecognizer->isInBackgroundPerceivableScene_, false);
    payload["typeIds"] = { 2 };
    bgtaskRecognizer->OnDispatchResource(ResType::RES_TYPE_CONTINUOUS_TASK,
        ResType::ContinuousTaskStatus::CONTINUOUS_TASK_UPDATE, payload);
    EXPECT_EQ(bgtaskRecognizer->isInBackgroundPerceivableScene_, true);
    bgtaskRecognizer->OnDispatchResource(ResType::RES_TYPE_CONTINUOUS_TASK,
        ResType::ContinuousTaskStatus::CONTINUOUS_TASK_END, payload);
    EXPECT_EQ(bgtaskRecognizer->isInBackgroundPerceivableScene_, false);
    payload["typeIds"] = { 1, 2 };
    bgtaskRecognizer->OnDispatchResource(ResType::RES_TYPE_CONTINUOUS_TASK,
        ResType::ContinuousTaskStatus::CONTINUOUS_TASK_END, payload);
    bgtaskRecognizer->OnDispatchResource(ResType::RES_TYPE_CONTINUOUS_TASK,
        -1, payload);
    bgtaskRecognizer->HandleForeground(ResType::RES_TYPE_CONTINUOUS_TASK,
        ResType::ContinuousTaskStatus::CONTINUOUS_TASK_END, payload);
    SUCCEED();
}

/**
 * @tc.name: SceneRecognizerMgr Slide test
 * @tc.desc: test the interface DispatchResource
 * @tc.type: FUNC
 * @tc.require: issuesIAJZVI
 * @tc.author: fengyang
 */
HWTEST_F(SceneRecognizeTest, SlideTest001, Function | MediumTest | Level0)
{
    nlohmann::json payload;
    payload["clientPid"] = "2000";
    SceneRecognizerMgr::GetInstance().DispatchResource(ResType::RES_TYPE_SLIDE_RECOGNIZE,
        ResType::SlideEventStatus::SLIDE_EVENT_DETECTING, payload);
    EXPECT_NE(SceneRecognizerMgr::GetInstance().sceneRecognizers_.size(), 0);
    SceneRecognizerMgr::GetInstance().DispatchResource(ResType::RES_TYPE_SEND_FRAME_EVENT, 0, payload);
    EXPECT_NE(SceneRecognizerMgr::GetInstance().sceneRecognizers_.size(), 0);
    SceneRecognizerMgr::GetInstance().DispatchResource(ResType::RES_TYPE_CLICK_RECOGNIZE,
        ResType::ClickEventType::TOUCH_EVENT_UP, payload);
    EXPECT_NE(SceneRecognizerMgr::GetInstance().sceneRecognizers_.size(), 0);
    SceneRecognizerMgr::GetInstance().DispatchResource(ResType::RES_TYPE_CLICK_RECOGNIZE,
        ResType::ClickEventType::TOUCH_EVENT_PULL_UP, payload);
    EXPECT_NE(SceneRecognizerMgr::GetInstance().sceneRecognizers_.size(), 0);
}

/**
 * @tc.name: SystemUpgradeSceneRecognizer Slide test
 * @tc.desc: test the interface OnDispatchResource
 * @tc.type: FUNC
 * @tc.require: issuesIAJZVI
 * @tc.author: fengyang
 */
HWTEST_F(SceneRecognizeTest, SystemUpgradeSceneRecognizer_001, Function | MediumTest | Level0)
{
    auto systemUpgradeSceneRecognizer = std::make_shared<SystemUpgradeSceneRecognizer>();
    systemUpgradeSceneRecognizer->isSystemUpgraded_ = true;
    int64_t value = 0;
    nlohmann::json payload;
    systemUpgradeSceneRecognizer->OnDispatchResource(ResType::RES_TYPE_BOOT_COMPLETED, value, payload);
    EXPECT_EQ(systemUpgradeSceneRecognizer->isSystemUpgraded_, true);
}

/**
 * @tc.name: SceneRecognizer HandleSlideEvent
 * @tc.desc: test the interface HandleSlideEvent
 * @tc.type: FUNC
 * @tc.require: issuesIAJZVI
 * @tc.author: fengyang
 */
HWTEST_F(SceneRecognizeTest, slideRecognizer_001, Function | MediumTest | Level0)
{
    g_slideState = SlideRecognizeStat::LIST_FLING;
    auto slideRecognizer = std::make_shared<SlideRecognizer>();
    int64_t value = ResType::SlideEventStatus::SLIDE_EVENT_ON;
    nlohmann::json payload;
    payload["clientPid"] = "2000";
    slideRecognizer->HandleSlideEvent(value, payload);
    EXPECT_EQ(g_slideState, SlideRecognizeStat::LIST_FLING);
}

/**
 * @tc.name: SceneRecognizer HandleSlideDetecting_001
 * @tc.desc: test the interface HandleSlideDetecting
 * @tc.type: FUNC
 * @tc.require: issuesIAJZVI
 * @tc.author: fengyang
 */
HWTEST_F(SceneRecognizeTest, HandleSlideDetecting_001, Function | MediumTest | Level0)
{
    auto slideRecognizer = std::make_shared<SlideRecognizer>();
    nlohmann::json payload;
    slideRecognizer->HandleSlideDetecting(payload);
    std::string testPid = "2000";
    payload["clientPid"] = testPid;
    slideRecognizer->HandleListFlingStart(payload);
    g_slideState = SlideRecognizeStat::LIST_FLING;
    slideRecognizer->HandleSlideDetecting(payload);
    EXPECT_EQ(slideRecognizer->slidePid_, testPid);
}

/**
 * @tc.name: SceneRecognizer HandleListFlingStart_001
 * @tc.desc: test the interface HandleListFlingStart
 * @tc.type: FUNC
 * @tc.require: issuesIAJZVI
 * @tc.author: fengyang
 */
HWTEST_F(SceneRecognizeTest, HandleListFlingStart_001, Function | MediumTest | Level0)
{
    auto slideRecognizer = std::make_shared<SlideRecognizer>();
    nlohmann::json payload;
    std::string testPid = "2000";
    payload["clientPid"] = testPid;
    g_slideState = SlideRecognizeStat::LIST_FLING;
    slideRecognizer->HandleListFlingStart(payload);
    slideRecognizer->HandleListFlingStart(payload);
    EXPECT_EQ(g_slideState, SlideRecognizeStat::LIST_FLING);
}

/**
 * @tc.name: SceneRecognizer HandleSendFrameEvent_001
 * @tc.desc: test the interface HandleSendFrameEvent
 * @tc.type: FUNC
 * @tc.require: issuesIAJZVI
 * @tc.author: fengyang
 */
HWTEST_F(SceneRecognizeTest, HandleSendFrameEvent_001, Function | MediumTest | Level0)
{
    auto slideRecognizer = std::make_shared<SlideRecognizer>();
    nlohmann::json payload;
    payload["clientPid"] = "2000";
    g_slideState = SlideRecognizeStat::SLIDE_NORMAL_DETECTING;
    slideRecognizer->HandleSendFrameEvent(payload);
    g_slideState = SlideRecognizeStat::LIST_FLING;
    slideRecognizer->HandleSendFrameEvent(payload);
    EXPECT_EQ(g_slideState, SlideRecognizeStat::LIST_FLING);
}

/**
 * @tc.name: SceneRecognizer HandleClickEvent_001
 * @tc.desc: test the interface HandleClickEvent
 * @tc.type: FUNC
 * @tc.require: issuesIAJZVI
 * @tc.author: fengyang
 */
HWTEST_F(SceneRecognizeTest, HandleClickEvent_001, Function | MediumTest | Level0)
{
    auto slideRecognizer = std::make_shared<SlideRecognizer>();
    std::string testPid = "2000";
    slideRecognizer->slidePid_ = testPid;
    int64_t value = ResType::SlideEventStatus::SLIDE_EVENT_ON;
    nlohmann::json payload;
    g_slideState = SlideRecognizeStat::SLIDE_NORMAL_DETECTING;
    slideRecognizer->HandleClickEvent(value, payload);
    g_slideState = SlideRecognizeStat::SLIDE_NORMAL;
    slideRecognizer->HandleClickEvent(value, payload);
    payload["clientPid"] = testPid;
    payload["up_speed"] = std::to_string(slideRecognizer->listFlingTimeOutTime_ + 1);
    slideRecognizer->HandleClickEvent(value, payload);
    slideRecognizer->HandleClickEvent(value, payload);
    EXPECT_EQ(slideRecognizer->slidePid_, testPid);
}

/**
 * @tc.name: SceneRecognizer FillRealPid_001
 * @tc.desc: test the interface FillRealPid
 * @tc.type: FUNC
 * @tc.require: issuesIAJZVI
 * @tc.author: fengyang
 */
HWTEST_F(SceneRecognizeTest, FillRealPid_001, Function | MediumTest | Level0)
{
    auto slideRecognizer = std::make_shared<SlideRecognizer>();
    nlohmann::json payload;
    auto result = slideRecognizer->FillRealPid(payload);
    EXPECT_EQ(payload, result);
}

/**
 * @tc.name: SceneRecognizer SetListFlingTimeoutTime_001
 * @tc.desc: test the interface SetListFlingTimeoutTime
 * @tc.type: FUNC
 * @tc.require: issuesIAJZVI
 * @tc.author: fengyang
 */
HWTEST_F(SceneRecognizeTest, SetListFlingTimeoutTime_001, Function | MediumTest | Level0)
{
    auto slideRecognizer = std::make_shared<SlideRecognizer>();
    int64_t value = 100;
    slideRecognizer->SetListFlingTimeoutTime(value);
    EXPECT_EQ(slideRecognizer->listFlingTimeOutTime_, value);
}

/**
 * @tc.name: SceneRecognizer SetListFlingEndTime_001
 * @tc.desc: test the interface SetListFlingEndTime
 * @tc.type: FUNC
 * @tc.require: issuesIAJZVI
 * @tc.author: fengyang
 */
HWTEST_F(SceneRecognizeTest, SetListFlingEndTime_001, Function | MediumTest | Level0)
{
    auto slideRecognizer = std::make_shared<SlideRecognizer>();
    int64_t value = 100;
    slideRecognizer->SetListFlingEndTime(value);
    EXPECT_EQ(slideRecognizer->listFlingEndTime_, value);
}

/**
 * @tc.name: SceneRecognizer SetListFlingSpeedLimit_001
 * @tc.desc: test the interface SetListFlingSpeedLimit
 * @tc.type: FUNC
 * @tc.require: issuesIAJZVI
 * @tc.author: fengyang
 */
HWTEST_F(SceneRecognizeTest, SetListFlingSpeedLimit_001, Function | MediumTest | Level0)
{
    auto slideRecognizer = std::make_shared<SlideRecognizer>();
    int64_t value = 100;
    slideRecognizer->SetListFlingSpeedLimit(value);
    EXPECT_EQ(slideRecognizer->listFlingSpeedLimit_, value);
}

/**
 * @tc.name: SceneRecognizer SetSlideNormalDetectingTime_001
 * @tc.desc: test the interface SetSlideNormalDetectingTime
 * @tc.type: FUNC
 * @tc.require: issuesIAJZVI
 * @tc.author: fengyang
 */
HWTEST_F(SceneRecognizeTest, SetSlideNormalDetectingTime_001, Function | MediumTest | Level0)
{
    auto slideRecognizer = std::make_shared<SlideRecognizer>();
    int64_t value = 100;
    slideRecognizer->SetSlideNormalDetectingTime(value);
    EXPECT_EQ(slideRecognizer->slideNormalDecectingTime_, value);
}

} // namespace ResourceSchedule
} // namespace OHOS
