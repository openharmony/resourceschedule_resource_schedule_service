/*
 * Copyright (c) 2024-2025 Huawei Device Co., Ltd.
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
#include <atomic>
#include <chrono>
#include <thread>

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
extern "C" {
void SetListFlingTimeoutTime(int64_t value);
void SetListFlingEndTime(int64_t value);
void SetListFlingSpeedLimit(float value);
}

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
    SceneRecognizerMgr::GetInstance().DispatchResource(std::make_shared<ResData>(ResType::RES_TYPE_SCREEN_STATUS,
        ResType::ScreenStatus::SCREEN_ON, payload));
    EXPECT_NE(SceneRecognizerMgr::GetInstance().sceneRecognizers_.size(), 0);
    SceneRecognizerMgr::GetInstance().DispatchResource(std::make_shared<ResData>(-1,
        -1, payload));
    EXPECT_NE(SceneRecognizerMgr::GetInstance().sceneRecognizers_.size(), 0);
    SceneRecognizerMgr::GetInstance().DispatchResource(std::make_shared<ResData>(
        ResType::RES_TYPE_APP_INSTALL_UNINSTALL, ResType::AppInstallStatus::APP_INSTALL_END, payload));
    EXPECT_NE(SceneRecognizerMgr::GetInstance().sceneRecognizers_.size(), 0);
    SceneRecognizerMgr::GetInstance().DispatchResource(std::make_shared<ResData>(
        ResType::RES_TYPE_APP_INSTALL_UNINSTALL, ResType::AppInstallStatus::APP_INSTALL_END, payload));
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
    EXPECT_FALSE(bgtaskRecognizer->isInBackgroundPerceivableScene_);
}

/**
 * @tc.name: SceneRecognizerMgr Bgtask test
 * @tc.desc: test the interface IsValidFilteredTypeIds
 * @tc.type: FUNC
 * @tc.require: IC9V05
 * @tc.author:fengshiyi
 */
HWTEST_F(SceneRecognizeTest, BgtaskTest002, Function | MediumTest | Level0)
{
    auto bgtaskRecognizer = std::make_shared<BackgroundSensitiveTaskOverlappingSceneRecognizer>();
    std::vector<uint32_t> filteredTypeIds;
    filteredTypeIds.push_back(0);
    EXPECT_FALSE(bgtaskRecognizer->IsValidFilteredTypeIds(filteredTypeIds));
    filteredTypeIds.clear();
    filteredTypeIds.push_back(1);
    EXPECT_TRUE(bgtaskRecognizer->IsValidFilteredTypeIds(filteredTypeIds));
    filteredTypeIds.clear();
    filteredTypeIds.push_back(100);
    EXPECT_FALSE(bgtaskRecognizer->IsValidFilteredTypeIds(filteredTypeIds));
    std::vector<uint32_t> typeIds(100, 1);
    EXPECT_FALSE(bgtaskRecognizer->IsValidFilteredTypeIds(typeIds));
}

/**
 * @tc.name: SceneRecognizerMgr Bgtask test
 * @tc.desc: test the interface IsValidPid
 * @tc.type: FUNC
 * @tc.require: IC9V05
 * @tc.author:fengshiyi
 */
HWTEST_F(SceneRecognizeTest, BgtaskTest003, Function | MediumTest | Level0)
{
    auto bgtaskRecognizer = std::make_shared<BackgroundSensitiveTaskOverlappingSceneRecognizer>();
    pid_t pid = -2;
    EXPECT_FALSE(bgtaskRecognizer->IsValidPid(pid));
    pid = INT32_MAX;
    EXPECT_TRUE(bgtaskRecognizer->IsValidPid(pid));
    pid = 100;
    EXPECT_TRUE(bgtaskRecognizer->IsValidPid(pid));
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
    SceneRecognizerMgr::GetInstance().DispatchResource(std::make_shared<ResData>(ResType::RES_TYPE_SLIDE_RECOGNIZE,
        ResType::SlideEventStatus::SLIDE_EVENT_DETECTING, payload));
    EXPECT_NE(SceneRecognizerMgr::GetInstance().sceneRecognizers_.size(), 0);
    SceneRecognizerMgr::GetInstance().DispatchResource(std::make_shared<ResData>(ResType::RES_TYPE_SEND_FRAME_EVENT,
        0, payload));
    EXPECT_NE(SceneRecognizerMgr::GetInstance().sceneRecognizers_.size(), 0);
    SceneRecognizerMgr::GetInstance().DispatchResource(std::make_shared<ResData>(ResType::RES_TYPE_CLICK_RECOGNIZE,
        ResType::ClickEventType::TOUCH_EVENT_UP, payload));
    EXPECT_NE(SceneRecognizerMgr::GetInstance().sceneRecognizers_.size(), 0);
    SceneRecognizerMgr::GetInstance().DispatchResource(std::make_shared<ResData>(ResType::RES_TYPE_CLICK_RECOGNIZE,
        ResType::ClickEventType::TOUCH_EVENT_PULL_UP, payload));
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
    int64_t value = 0;
    nlohmann::json payload;
    systemUpgradeSceneRecognizer->OnDispatchResource(ResType::RES_TYPE_BOOT_COMPLETED, value, payload);
    EXPECT_EQ(systemUpgradeSceneRecognizer->isSystemUpgraded_, false);
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
    auto slideRecognizer = std::make_shared<SlideRecognizer>();
    int64_t value = ResType::SlideEventStatus::SLIDE_EVENT_ON;
    nlohmann::json payload;
    payload["clientPid"] = "2000";
    slideRecognizer->HandleSlideEvent(value, payload);
    EXPECT_EQ(g_slideState, SlideRecognizeStat::IDLE);
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
    sleep(1);
    EXPECT_EQ(slideRecognizer->slidePid_, testPid);
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
    value = ResType::ClickEventType::TOUCH_EVENT_DOWN;
    slideRecognizer->HandleClickEvent(value, payload);
    slideRecognizer->HandleClickEvent(value, payload);
    EXPECT_EQ(slideRecognizer->isInTouching_, true);
}

/**
 * @tc.name: SceneRecognizer FillRealPidAndUid_001
 * @tc.desc: test the interface FillRealPidAndUid
 * @tc.type: FUNC
 * @tc.require: issuesIAJZVI
 * @tc.author: fengyang
 */
HWTEST_F(SceneRecognizeTest, FillRealPidAndUid_001, Function | MediumTest | Level0)
{
    auto slideRecognizer = std::make_shared<SlideRecognizer>();
    nlohmann::json payload;
    auto result = slideRecognizer->FillRealPidAndUid(payload);
    EXPECT_EQ(result["scrTid"], "");
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
 * @tc.name: SceneRecognizerBase_Accept_001
 * @tc.desc: test SceneRecognizerBase accept resource type branches
 * @tc.type: FUNC
 * @tc.require: issue1663
 * @tc.author: zhumingjie
 */
HWTEST_F(SceneRecognizeTest, SceneRecognizerBase_Accept_001, Function | MediumTest | Level0)
{
    SlideRecognizer recognizer;
    recognizer.AddAcceptResTypes({ ResType::RES_TYPE_SCREEN_STATUS, ResType::RES_TYPE_BOOT_COMPLETED });
    EXPECT_TRUE(recognizer.Accept(ResType::RES_TYPE_SCREEN_STATUS));
    EXPECT_FALSE(recognizer.Accept(ResType::RES_TYPE_CLICK_RECOGNIZE));

    recognizer.AddAcceptResTypes({ ResType::RES_TYPE_CLICK_RECOGNIZE });
    EXPECT_FALSE(recognizer.Accept(ResType::RES_TYPE_SCREEN_STATUS));
    EXPECT_TRUE(recognizer.Accept(ResType::RES_TYPE_CLICK_RECOGNIZE));
}

/**
 * @tc.name: SceneRecognizerMgr_SubmitTask_002
 * @tc.desc: test SceneRecognizerMgr::SubmitTask when queue is null
 * @tc.type: FUNC
 * @tc.require: issue1663
 * @tc.author: zhumingjie
 */
HWTEST_F(SceneRecognizeTest, SceneRecognizerMgr_SubmitTask_002, Function | MediumTest | Level0)
{
    SceneRecognizerMgr localMgr;
    localMgr.ffrtQueue_ = nullptr;
    std::atomic<bool> taskExecuted(false);
    localMgr.SubmitTask([&taskExecuted]() {
        taskExecuted.store(true);
    });
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    EXPECT_FALSE(taskExecuted.load());
}

/**
 * @tc.name: SceneRecognizerMgr_SetSlideConfig_001
 * @tc.desc: test SceneRecognizerMgr slide config setter wrappers
 * @tc.type: FUNC
 * @tc.require: issue1663
 * @tc.author: zhumingjie
 */
HWTEST_F(SceneRecognizeTest, SceneRecognizerMgr_SetSlideConfig_001, Function | MediumTest | Level0)
{
    SceneRecognizerMgr localMgr;
    auto slideRecognizer = std::static_pointer_cast<SlideRecognizer>(
        localMgr.sceneRecognizers_[RecognizeType::SLIDE_RECOGNIZER]);
    ASSERT_NE(slideRecognizer, nullptr);

    localMgr.SetListFlingTimeoutTime(111);
    localMgr.SetListFlingEndTime(222);
    localMgr.SetListFlingSpeedLimit(9.0);
    EXPECT_EQ(slideRecognizer->listFlingTimeOutTime_, 111);
    EXPECT_EQ(slideRecognizer->listFlingEndTime_, 222);
    EXPECT_FLOAT_EQ(slideRecognizer->listFlingSpeedLimit_, 9.0);
}

/**
 * @tc.name: SceneRecognizerMgr_CApiSetSlideConfig_001
 * @tc.desc: test C interface wrappers for slide config
 * @tc.type: FUNC
 * @tc.require: issue1663
 * @tc.author: zhumingjie
 */
HWTEST_F(SceneRecognizeTest, SceneRecognizerMgr_CApiSetSlideConfig_001, Function | MediumTest | Level0)
{
    auto slideRecognizer = std::static_pointer_cast<SlideRecognizer>(
        SceneRecognizerMgr::GetInstance().sceneRecognizers_[RecognizeType::SLIDE_RECOGNIZER]);
    ASSERT_NE(slideRecognizer, nullptr);

    SetListFlingTimeoutTime(123);
    SetListFlingEndTime(234);
    SetListFlingSpeedLimit(8.0);
    EXPECT_EQ(slideRecognizer->listFlingTimeOutTime_, 123);
    EXPECT_EQ(slideRecognizer->listFlingEndTime_, 234);
    EXPECT_FLOAT_EQ(slideRecognizer->listFlingSpeedLimit_, 8.0);

    SetListFlingTimeoutTime(DEFAULT_LIST_FLINT_TIME_OUT_TIME);
    SetListFlingEndTime(DEFAULT_LIST_FLINT_END_TIME);
    SetListFlingSpeedLimit(DEFAULT_LIST_FLING_SPEED_LIMIT);
}

/**
 * @tc.name: ContinuousAppInstallRecognizer_Branch_001
 * @tc.desc: test uncovered branches in ContinuousAppInstallRecognizer
 * @tc.type: FUNC
 * @tc.require: issue1663
 * @tc.author: zhumingjie
 */
HWTEST_F(SceneRecognizeTest, ContinuousAppInstallRecognizer_Branch_001, Function | MediumTest | Level0)
{
    auto recognizer = std::make_shared<ContinuousAppInstallRecognizer>();
    nlohmann::json payload;

    recognizer->OnDispatchResource(
        ResType::RES_TYPE_SCREEN_STATUS, ResType::AppInstallStatus::APP_INSTALL_START, payload);
    EXPECT_FALSE(recognizer->isInContinuousInstall_.load());
    EXPECT_EQ(recognizer->exitAppInstall_, nullptr);

    recognizer->OnDispatchResource(
        ResType::RES_TYPE_APP_INSTALL_UNINSTALL, ResType::AppInstallStatus::APP_CHANGED, payload);
    EXPECT_NE(recognizer->exitAppInstall_, nullptr);

    recognizer->isInContinuousInstall_.store(true);
    recognizer->OnDispatchResource(ResType::RES_TYPE_APP_INSTALL_UNINSTALL,
        ResType::AppInstallStatus::APP_INSTALL_START, payload);
    EXPECT_TRUE(recognizer->isInContinuousInstall_.load());
}

/**
 * @tc.name: BgtaskPayloadCheck_001
 * @tc.desc: test payload validation and default branch in bgtask recognizer
 * @tc.type: FUNC
 * @tc.require: issue1663
 * @tc.author: zhumingjie
 */
HWTEST_F(SceneRecognizeTest, BgtaskPayloadCheck_001, Function | MediumTest | Level0)
{
    auto recognizer = std::make_shared<BackgroundSensitiveTaskOverlappingSceneRecognizer>();
    nlohmann::json payload;
    recognizer->OnDispatchResource(
        ResType::RES_TYPE_APP_STATE_CHANGE, ResType::ProcessStatus::PROCESS_FOREGROUND, payload);
    EXPECT_EQ(recognizer->foregroundPid_, -1);

    payload["pid"] = 1000;
    recognizer->OnDispatchResource(ResType::RES_TYPE_REPORT_SCENE_BOARD, 0, payload);
    EXPECT_EQ(recognizer->sceneboardPid_, -1);

    payload["pid"] = "3000";
    recognizer->OnDispatchResource(ResType::RES_TYPE_SCREEN_STATUS, 0, payload);
    EXPECT_EQ(recognizer->foregroundPid_, -1);
    EXPECT_TRUE(recognizer->perceivableTasks_.empty());
}

/**
 * @tc.name: BgtaskCheckEnterScene_001
 * @tc.desc: test multi-task and guard branches in CheckEnterScene
 * @tc.type: FUNC
 * @tc.require: issue1663
 * @tc.author: zhumingjie
 */
HWTEST_F(SceneRecognizeTest, BgtaskCheckEnterScene_001, Function | MediumTest | Level0)
{
    auto recognizer = std::make_shared<BackgroundSensitiveTaskOverlappingSceneRecognizer>();
    recognizer->foregroundPid_ = 100;
    recognizer->sceneboardPid_ = 200;
    recognizer->perceivableTasks_[300] = { 2 };
    recognizer->perceivableTasks_[400] = { 2 };
    EXPECT_TRUE(recognizer->CheckEnterScene());

    recognizer->isInBackgroundPerceivableScene_ = true;
    EXPECT_FALSE(recognizer->CheckEnterScene());

    recognizer->isInBackgroundPerceivableScene_ = false;
    recognizer->foregroundPid_ = recognizer->sceneboardPid_;
    EXPECT_FALSE(recognizer->CheckEnterScene());
}

/**
 * @tc.name: BgtaskHandleTaskUpdate_001
 * @tc.desc: test HandleTaskUpdate and HandleTaskStop branches for pid miss/hit
 * @tc.type: FUNC
 * @tc.require: issue1663
 * @tc.author: zhumingjie
 */
HWTEST_F(SceneRecognizeTest, BgtaskHandleTaskUpdate_001, Function | MediumTest | Level0)
{
    auto recognizer = std::make_shared<BackgroundSensitiveTaskOverlappingSceneRecognizer>();
    recognizer->foregroundPid_ = 100;
    recognizer->sceneboardPid_ = 200;
    recognizer->isInBackgroundPerceivableScene_ = true;
    recognizer->perceivableTasks_[300] = { 2 };

    recognizer->HandleTaskUpdate(999, {});
    EXPECT_EQ(recognizer->perceivableTasks_.size(), 1);
    EXPECT_FALSE(recognizer->isInBackgroundPerceivableScene_);

    recognizer->HandleTaskStop(999);
    EXPECT_EQ(recognizer->perceivableTasks_.size(), 1);
    EXPECT_FALSE(recognizer->isInBackgroundPerceivableScene_);

    recognizer->HandleTaskUpdate(300, {});
    EXPECT_TRUE(recognizer->perceivableTasks_.empty());
    EXPECT_FALSE(recognizer->isInBackgroundPerceivableScene_);
}

/**
 * @tc.name: BgtaskIsValidFilteredTypeIds_004
 * @tc.desc: test empty filtered typeIds branch
 * @tc.type: FUNC
 * @tc.require: issue1663
 * @tc.author: zhumingjie
 */
HWTEST_F(SceneRecognizeTest, BgtaskIsValidFilteredTypeIds_004, Function | MediumTest | Level0)
{
    auto recognizer = std::make_shared<BackgroundSensitiveTaskOverlappingSceneRecognizer>();
    std::vector<uint32_t> filteredTypeIds;
    EXPECT_TRUE(recognizer->IsValidFilteredTypeIds(filteredTypeIds));
}

/**
 * @tc.name: SystemUpgradeSceneRecognizer_002
 * @tc.desc: test uncovered branches in SystemUpgradeSceneRecognizer::OnDispatchResource
 * @tc.type: FUNC
 * @tc.require: issue1663
 * @tc.author: zhumingjie
 */
HWTEST_F(SceneRecognizeTest, SystemUpgradeSceneRecognizer_002, Function | MediumTest | Level0)
{
    auto recognizer = std::make_shared<SystemUpgradeSceneRecognizer>();
    nlohmann::json payload;

    recognizer->isSystemUpgraded_ = true;
    recognizer->OnDispatchResource(ResType::RES_TYPE_SCREEN_STATUS, 0, payload);
    EXPECT_TRUE(recognizer->isSystemUpgraded_);

    recognizer->OnDispatchResource(ResType::RES_TYPE_BOOT_COMPLETED, 0, payload);
    EXPECT_TRUE(recognizer->isSystemUpgraded_);
}

/**
 * @tc.name: SlideRecognizer_OnDispatchResource_002
 * @tc.desc: test axis non-end and default branches in OnDispatchResource
 * @tc.type: FUNC
 * @tc.require: issue1663
 * @tc.author: zhumingjie
 */
HWTEST_F(SceneRecognizeTest, SlideRecognizer_OnDispatchResource_002, Function | MediumTest | Level0)
{
    auto slideRecognizer = std::make_shared<SlideRecognizer>();
    slideRecognizer->HandleSlideOFFEvent();

    nlohmann::json payload;
    payload["clientPid"] = "1000";
    payload["callingUid"] = "2000";
    slideRecognizer->OnDispatchResource(
        ResType::RES_TYPE_AXIS_EVENT, ResType::AxisEventStatus::AXIS_EVENT_BEGIN, payload);
    EXPECT_EQ(slideRecognizer->GetSlideStatus(), SlideRecognizeStat::IDLE);

    slideRecognizer->OnDispatchResource(ResType::RES_TYPE_SCREEN_STATUS, 0, payload);
    EXPECT_EQ(slideRecognizer->GetSlideStatus(), SlideRecognizeStat::IDLE);
}

/**
 * @tc.name: SlideRecognizer_StartDetecting_002
 * @tc.desc: test missing callingUid branch in StartDetecting
 * @tc.type: FUNC
 * @tc.require: issue1663
 * @tc.author: zhumingjie
 */
HWTEST_F(SceneRecognizeTest, SlideRecognizer_StartDetecting_002, Function | MediumTest | Level0)
{
    auto slideRecognizer = std::make_shared<SlideRecognizer>();
    slideRecognizer->HandleSlideOFFEvent();

    nlohmann::json payload;
    payload["clientPid"] = "1000";
    slideRecognizer->StartDetecting(payload);
    EXPECT_EQ(slideRecognizer->GetSlideStatus(), SlideRecognizeStat::SLIDE_NORMAL_DETECTING);
    EXPECT_EQ(slideRecognizer->slidePid_, "1000");
    EXPECT_EQ(slideRecognizer->slideUid_, "");

    slideRecognizer->HandleSlideOFFEvent();
}

/**
 * @tc.name: SlideRecognizer_HandleSlideOffEvent_001
 * @tc.desc: test SLIDE_EVENT_OFF branch
 * @tc.type: FUNC
 * @tc.require: issue1663
 * @tc.author: zhumingjie
 */
HWTEST_F(SceneRecognizeTest, SlideRecognizer_HandleSlideOffEvent_001, Function | MediumTest | Level0)
{
    auto slideRecognizer = std::make_shared<SlideRecognizer>();
    slideRecognizer->HandleSlideOFFEvent();

    nlohmann::json payload;
    payload["clientPid"] = "1000";
    payload["callingUid"] = "2000";
    slideRecognizer->StartDetecting(payload);
    slideRecognizer->isInTouching_ = true;
    slideRecognizer->HandleSendFrameEvent(payload);
    EXPECT_EQ(slideRecognizer->GetSlideStatus(), SlideRecognizeStat::SLIDE_NORMAL);

    payload["up_speed"] = "10.0";
    slideRecognizer->HandleClickEvent(ResType::ClickEventType::TOUCH_EVENT_UP, payload);
    EXPECT_EQ(slideRecognizer->GetSlideStatus(), SlideRecognizeStat::LIST_FLING);

    slideRecognizer->HandleSlideEvent(ResType::SlideEventStatus::SLIDE_EVENT_OFF, payload);
    EXPECT_EQ(slideRecognizer->GetSlideStatus(), SlideRecognizeStat::IDLE);
}

/**
 * @tc.name: SlideRecognizer_HandleClickEvent_002
 * @tc.desc: test click payload validation branches
 * @tc.type: FUNC
 * @tc.require: issue1663
 * @tc.author: zhumingjie
 */
HWTEST_F(SceneRecognizeTest, SlideRecognizer_HandleClickEvent_002, Function | MediumTest | Level0)
{
    auto slideRecognizer = std::make_shared<SlideRecognizer>();
    slideRecognizer->HandleSlideOFFEvent();

    nlohmann::json payload;
    payload["clientPid"] = "1000";
    payload["callingUid"] = "2000";
    slideRecognizer->StartDetecting(payload);
    slideRecognizer->isInTouching_ = true;
    slideRecognizer->HandleSendFrameEvent(payload);
    EXPECT_EQ(slideRecognizer->GetSlideStatus(), SlideRecognizeStat::SLIDE_NORMAL);

    nlohmann::json missPid;
    missPid["up_speed"] = "10.0";
    slideRecognizer->HandleClickEvent(ResType::ClickEventType::TOUCH_EVENT_UP, missPid);
    EXPECT_EQ(slideRecognizer->GetSlideStatus(), SlideRecognizeStat::SLIDE_NORMAL);

    nlohmann::json missSpeed;
    missSpeed["clientPid"] = "1000";
    slideRecognizer->HandleClickEvent(ResType::ClickEventType::TOUCH_EVENT_UP, missSpeed);
    EXPECT_EQ(slideRecognizer->GetSlideStatus(), SlideRecognizeStat::SLIDE_NORMAL);

    nlohmann::json invalidSpeed;
    invalidSpeed["clientPid"] = "1000";
    invalidSpeed["up_speed"] = "bad_speed";
    slideRecognizer->HandleClickEvent(ResType::ClickEventType::TOUCH_EVENT_UP, invalidSpeed);
    EXPECT_EQ(slideRecognizer->GetSlideStatus(), SlideRecognizeStat::SLIDE_NORMAL);

    slideRecognizer->HandleClickEvent(ResType::ClickEventType::TOUCH_EVENT_DOWN, invalidSpeed);
    EXPECT_EQ(slideRecognizer->GetSlideStatus(), SlideRecognizeStat::IDLE);
}

/**
 * @tc.name: SlideRecognizer_AxisSpeedFactor_001
 * @tc.desc: test axis_event_type speed factor branch
 * @tc.type: FUNC
 * @tc.require: issue1663
 * @tc.author: zhumingjie
 */
HWTEST_F(SceneRecognizeTest, SlideRecognizer_AxisSpeedFactor_001, Function | MediumTest | Level0)
{
    auto slideRecognizer = std::make_shared<SlideRecognizer>();
    slideRecognizer->HandleSlideOFFEvent();

    nlohmann::json payload;
    payload["clientPid"] = "1000";
    payload["callingUid"] = "2000";
    slideRecognizer->StartDetecting(payload);
    slideRecognizer->isInTouching_ = true;
    slideRecognizer->HandleSendFrameEvent(payload);
    EXPECT_EQ(slideRecognizer->GetSlideStatus(), SlideRecognizeStat::SLIDE_NORMAL);

    nlohmann::json upPayload;
    upPayload["clientPid"] = "1000";
    upPayload["up_speed"] = "1.0";
    upPayload["axis_event_type"] = "axis_event_type";
    slideRecognizer->HandleClickEvent(ResType::ClickEventType::TOUCH_EVENT_UP, upPayload);
    EXPECT_EQ(slideRecognizer->GetSlideStatus(), SlideRecognizeStat::LIST_FLING);

    slideRecognizer->HandleSlideOFFEvent();
    EXPECT_EQ(slideRecognizer->GetSlideStatus(), SlideRecognizeStat::IDLE);
}

/**
 * @tc.name: FillRealPidAndUid_002
 * @tc.desc: test FillRealPidAndUid replacement branch
 * @tc.type: FUNC
 * @tc.require: issue1663
 * @tc.author: zhumingjie
 */
HWTEST_F(SceneRecognizeTest, FillRealPidAndUid_002, Function | MediumTest | Level0)
{
    auto slideRecognizer = std::make_shared<SlideRecognizer>();
    slideRecognizer->slidePid_ = "3000";
    slideRecognizer->slideUid_ = "4000";
    slideRecognizer->scrTid_ = "5000";

    nlohmann::json payload;
    payload["clientPid"] = "1000";
    payload["callingUid"] = "2000";
    auto result = slideRecognizer->FillRealPidAndUid(payload);
    EXPECT_EQ(result["clientPid"], "3000");
    EXPECT_EQ(result["callingUid"], "4000");
    EXPECT_EQ(result["scrTid"], "5000");
}

/**
 * @tc.name: SceneRecognizer DispatchResource_001
 * @tc.desc: test the interface DispatchResource
 * @tc.type: FUNC
 * @tc.require: issuesIC5T7D
 * @tc.author: baiheng
 */
HWTEST_F(SceneRecognizeTest, DispatchResource_001, Function | MediumTest | Level0)
{
    SceneRecognizerMgr::GetInstance().DispatchResource(nullptr);
    SceneRecognizerMgr::GetInstance().ffrtQueue_ = nullptr;
    nlohmann::json payload;
    auto resData = make_shared<ResData>(0, 0, payload);
    SceneRecognizerMgr::GetInstance().DispatchResource(resData);
    EXPECT_EQ(SceneRecognizerMgr::GetInstance().ffrtQueue_, nullptr);
}
} // namespace ResourceSchedule
} // namespace OHOS
