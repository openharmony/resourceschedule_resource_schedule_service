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

using namespace std;
using namespace testing::ext;

namespace OHOS {
namespace ResourceSchedule {

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
    SUCCEED();
    SceneRecognizerMgr::GetInstance().DispatchResource(-1, -1, payload);
    SUCCEED();
    SceneRecognizerMgr::GetInstance().DispatchResource(ResType::RES_TYPE_APP_INSTALL_UNINSTALL,
        ResType::AppInstallStatus::APP_INSTALL_END, payload);
    SUCCEED();
    SceneRecognizerMgr::GetInstance().DispatchResource(ResType::RES_TYPE_APP_INSTALL_UNINSTALL,
        ResType::AppInstallStatus::APP_INSTALL_END, payload);
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
}

/**
 * @tc.name: SceneRecognizerMgr Slide test
 * @tc.desc: test the interface DispatchResource
 * @tc.type: FUNC
 * @tc.require: issueIA9UZ9
 * @tc.author:baiheng
 */
HWTEST_F(SceneRecognizeTest, AppInstallTest001, Function | MediumTest | Level0)
{
    nlohmann::json payload;
    SceneRecognizerMgr::GetInstance().DispatchResource(ResType::RES_TYPE_SLIDE_RECOGNIZE,
        ResType::SlideEventStatus::SLIDE_NORMAL_DETECTING, payload);
    SUCCEED();
    SceneRecognizerMgr::GetInstance().DispatchResource(ResType::RES_TYPE_SEND_FRAME_EVENT, 0, payload);
    SUCCEED();
    SceneRecognizerMgr::GetInstance().DispatchResource(ResType::RES_TYPE_CLICK_RECOGNIZE,
        ResType::ClickEventType::TOUCH_EVENT_UP, payload);
    SUCCEED();
    SceneRecognizerMgr::GetInstance().DispatchResource(ResType::RES_TYPE_CLICK_RECOGNIZE,
        ResType::ClickEventType::TOUCH_EVENT_PULL_UP, payload);
    SUCCEED();
}
} // namespace ResourceSchedule
} // namespace OHOS
