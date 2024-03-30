/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include <gtest/gtest.h>
#include <iostream>
#include <memory>
#define private public
#define protected public
#include "sched_controller.h"
#include "window_state_observer.h"
#include "window_manager.h"
#include "wm_common.h"
#include "sched_policy.h"

#undef private
#undef protected

using namespace testing::ext;
using namespace OHOS::ResourceSchedule::CgroupSetting;
using OHOS::Rosen::WindowModeType;
using OHOS::Rosen::IWindowModeChangedListener;

namespace OHOS {
namespace ResourceSchedule {
namespace CgroupSetting {
class CGroupSchedTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void CGroupSchedTest::SetUpTestCase(void)
{
}

void CGroupSchedTest::TearDownTestCase(void)
{
}

void CGroupSchedTest::SetUp(void)
{
}

void CGroupSchedTest::TearDown(void)
{
}

/**
 * @tc.name: CGroupSchedTest_WindowStateObserver_001
 * @tc.desc: Window Mode Observer Test
 * @tc.type: FUNC
 * @tc.require: issuesI9BU37
 * @tc.desc:
 */
HWTEST_F(CGroupSchedTest, CGroupSchedTest_WindowStateObserver_001, Function | MediumTest | Level1)
{
    std::shared_ptr<WindowModeObserver> windowModeObserver_ =
        std::make_shared<WindowModeObserver>();
    windowModeObserver_->OnWindowModeUpdate(Rosen::WindowModeType::WINDOW_MODE_SPLIT);
    EXPECT_NE(windowModeObserver_, nullptr);

    windowModeObserver_->OnWindowModeUpdate(Rosen::WindowModeType::WINDOW_MODE_FLOATING);
    EXPECT_NE(windowModeObserver_, nullptr);

    windowModeObserver_->OnWindowModeUpdate(Rosen::WindowModeType::WINDOW_MODE_SPLIT);
    EXPECT_NE(windowModeObserver_, nullptr);

    windowModeObserver_->OnWindowModeUpdate(Rosen::WindowModeType::WINDOW_MODE_SPLIT_FLOATING);
    EXPECT_NE(windowModeObserver_, nullptr);

    windowModeObserver_->OnWindowModeUpdate(Rosen::WindowModeType::WINDOW_MODE_SPLIT);
    EXPECT_NE(windowModeObserver_, nullptr);

    windowModeObserver_->OnWindowModeUpdate(Rosen::WindowModeType::WINDOW_MODE_OTHER);
    EXPECT_NE(windowModeObserver_, nullptr);

    windowModeObserver_->OnWindowModeUpdate(Rosen::WindowModeType::WINDOW_MODE_FLOATING);
    EXPECT_NE(windowModeObserver_, nullptr);

    windowModeObserver_->OnWindowModeUpdate(Rosen::WindowModeType::WINDOW_MODE_SPLIT_FLOATING);
    EXPECT_NE(windowModeObserver_, nullptr);

    windowModeObserver_->OnWindowModeUpdate(Rosen::WindowModeType::WINDOW_MODE_FLOATING);
    EXPECT_NE(windowModeObserver_, nullptr);

    windowModeObserver_->OnWindowModeUpdate(Rosen::WindowModeType::WINDOW_MODE_OTHER);
    EXPECT_NE(windowModeObserver_, nullptr);

    windowModeObserver_->OnWindowModeUpdate(Rosen::WindowModeType::WINDOW_MODE_SPLIT_FLOATING);
    EXPECT_NE(windowModeObserver_, nullptr);

    windowModeObserver_->OnWindowModeUpdate(Rosen::WindowModeType::WINDOW_MODE_OTHER);
    EXPECT_NE(windowModeObserver_, nullptr);

    windowModeObserver_->OnWindowModeUpdate(Rosen::WindowModeType::WINDOW_MODE_SPLIT);
    EXPECT_NE(windowModeObserver_, nullptr);

    windowModeObserver_->OnWindowModeUpdate(Rosen::WindowModeType::WINDOW_MODE_SPLIT);

    windowModeObserver_ = nullptr;
}

/**
 * @tc.name: CGroupSchedTest_WindowStateObserver_002
 * @tc.desc: Window Mode Observer Test
 * @tc.type: FUNC
 * @tc.require: issuesI9BU37
 * @tc.desc:
 */
HWTEST_F(CGroupSchedTest, CGroupSchedTest_WindowStateObserver_002, Function | MediumTest | Level1)
{
    std::shared_ptr<WindowModeObserver> windowModeObserver_ =
        std::make_shared<WindowModeObserver>();
    uint8_t nowWindowMode = windowModeObserver_->MarshallingWindowModeType(
        Rosen::WindowModeType::WINDOW_MODE_SPLIT_FLOATING);
    EXPECT_NQ(nowWindowMode, RSSWindowMode::WINDOW_MODE_SPLIT_FLOATING);

    uint8_t nowWindowMode = windowModeObserver_->MarshallingWindowModeType(Rosen::WindowModeType::WINDOW_MODE_SPLIT);
    EXPECT_NQ(nowWindowMode, RSSWindowMode::WINDOW_MODE_SPLIT);

    uint8_t nowWindowMode = windowModeObserver_->MarshallingWindowModeType(Rosen::WindowModeType::WINDOW_MODE_FLOATING);
    EXPECT_NQ(nowWindowMode, RSSWindowMode::WINDOW_MODE_FLOATING);

    uint8_t nowWindowMode = windowModeObserver_->MarshallingWindowModeType(Rosen::WindowModeType::WINDOW_MODE_OTHER);
    EXPECT_NQ(nowWindowMode, RSSWindowMode::WINDOW_MODE_OTHER);
    windowModeObserver_ = nullptr;
}

/**
 * @tc.name: CGroupSchedTest_WindowStateObserver_002
 * @tc.desc: Window Mode Observer Test
 * @tc.type: FUNC
 * @tc.require: issuesI9BU37
 * @tc.desc:
 */
HWTEST_F(CGroupSchedTest, CGroupSchedTest_SchedController_001, Function | MediumTest | Level1)
{
    auto schedController = SchedController::GetInstance();
    SUCCEED();
    schedController.SubscribeWindowState();
    SUCCEED();
    schedController.UnsubscribeWindowState();
    SUCCEED();
}


} // namespace CgroupSetting
} // namespace ResourceSchedule
} // namespace OHOS
