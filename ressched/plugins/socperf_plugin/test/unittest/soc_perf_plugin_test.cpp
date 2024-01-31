/*
 * Copyright (c) 2024-2024 Huawei Device Co., Ltd.
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

#define private public
#define protected public

#include <dlfcn.h>
#include <gtest/gtest.h>
#include <gtest/hwext/gtest-multithread.h>
#include "res_type.h"
#include "socperf_plugin.h"

using namespace testing::ext;
using namespace testing::mt;

namespace OHOS {
namespace ResourceSchedule {
namespace {
    std::string SOCPERF_PLUBIN_LIB_NAME = "libsocperf_plugin.z.so";
    std::string ERROR_LIB_NAME = "";
    using OnPluginInitFunc = bool (*)(std::string& libName);
    using OnPluginDisableFunc = void (*)();
    using OnDispatchResourceFunc = void (*)(const std::shared_ptr<ResData>& data);
}
class SocPerfPluginTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void SocPerfPluginTest::SetUpTestCase(void)
{
    SocPerfPlugin::GetInstance().Init();
}

void SocPerfPluginTest::TearDownTestCase(void)
{
    SocPerfPlugin::GetInstance().Disable();
}

void SocPerfPluginTest::SetUp(void)
{
}

void SocPerfPluginTest::TearDown(void)
{
}

/*
 * @tc.name: SocPerfPluginTest_LibTest_001
 * @tc.desc: test socperfplugin lib
 * @tc.type FUNC
 * @tc.require: issueI78T3V
 */
HWTEST_F(SocPerfPluginTest, SocPerfPluginTest_LibTest_001, Function | MediumTest | Level0)
{
    auto handle = dlopen(SOCPERF_PLUBIN_LIB_NAME.c_str(), RTLD_NOW);
    EXPECT_NE(handle, nullptr);
    OnPluginInitFunc onPluginInitFunc = reinterpret_cast<OnPluginInitFunc>(dlsym(handle, "OnPluginInit"));
    EXPECT_NE(onPluginInitFunc, nullptr);
    EXPECT_TRUE(onPluginInitFunc(SOCPERF_PLUBIN_LIB_NAME));
    EXPECT_FALSE(onPluginInitFunc(ERROR_LIB_NAME));
    OnPluginDisableFunc onPluginDisableFunc = reinterpret_cast<OnPluginDisableFunc>(dlsym(handle, "OnPluginDisable"));
    EXPECT_NE(onPluginDisableFunc, nullptr);
    onPluginDisableFunc();
    OnDispatchResourceFunc onDispatchResourceFunc = reinterpret_cast<OnDispatchResourceFunc>(dlsym(handle,
        "OnDispatchResource"));
    EXPECT_NE(onDispatchResourceFunc, nullptr);
    const std::shared_ptr<ResData>& data = std::make_shared<ResData>(ResType::RES_TYPE_APP_ABILITY_START,
        ResType::AppStartType::APP_COLD_START, nullptr);
    onDispatchResourceFunc(data);
    const std::shared_ptr<ResData>& dataInvalid = std::make_shared<ResData>(-1, -1, nullptr);
    onDispatchResourceFunc(dataInvalid);
}

/*
 * @tc.name: SocPerfPluginTest_API_TEST_001
 * @tc.desc: test socperfplugin api
 * @tc.type FUNC
 * @tc.require: issueI78T3V
 */
HWTEST_F(SocPerfPluginTest, SocPerfPluginTest_API_TEST_001, Function | MediumTest | Level0)
{
    const std::shared_ptr<ResData>& coldStartData = std::make_shared<ResData>(ResType::RES_TYPE_APP_ABILITY_START,
        ResType::AppStartType::APP_COLD_START, nullptr);
    SocPerfPlugin::GetInstance().socperfOnDemandSwitch_ = false;
    SocPerfPlugin::GetInstance().HandleAppAbilityStart(coldStartData);
    SocPerfPlugin::GetInstance().socperfOnDemandSwitch_ = true;
    SocPerfPlugin::GetInstance().HandleAppAbilityStart(coldStartData);
    const std::shared_ptr<ResData>& warmStartData = std::make_shared<ResData>(ResType::RES_TYPE_APP_ABILITY_START,
        ResType::AppStartType::APP_WARM_START, nullptr);
    SocPerfPlugin::GetInstance().HandleAppAbilityStart(warmStartData);
    const std::shared_ptr<ResData>& invalidData = std::make_shared<ResData>(-1, -1, nullptr);
    SocPerfPlugin::GetInstance().HandleAppAbilityStart(invalidData);
    EXPECT_TRUE(SocPerfPlugin::GetInstance().socperfOnDemandSwitch_);
}

/*
 * @tc.name: SocPerfPluginTest_API_TEST_002
 * @tc.desc: test socperfplugin api
 * @tc.type FUNC
 * @tc.require: issueI78T3V
 */
HWTEST_F(SocPerfPluginTest, SocPerfPluginTest_API_TEST_002, Function | MediumTest | Level0)
{
    const std::shared_ptr<ResData>& data = std::make_shared<ResData>(ResType::RES_TYPE_WINDOW_FOCUS,
        ResType::WindowFocusStatus::WINDOW_FOCUS, nullptr);
    SocPerfPlugin::GetInstance().HandleWindowFocus(data);
    const std::shared_ptr<ResData>& invalidData = std::make_shared<ResData>(-1, -1, nullptr);
    SocPerfPlugin::GetInstance().HandleWindowFocus(invalidData);
    EXPECT_TRUE(SocPerfPlugin::GetInstance().socperfOnDemandSwitch_);
}

/*
 * @tc.name: SocPerfPluginTest_API_TEST_003
 * @tc.desc: test socperfplugin api
 * @tc.type FUNC
 * @tc.require: issueI78T3V
 */
HWTEST_F(SocPerfPluginTest, SocPerfPluginTest_API_TEST_003, Function | MediumTest | Level0)
{
    const std::shared_ptr<ResData>& touchData = std::make_shared<ResData>(ResType::RES_TYPE_CLICK_RECOGNIZE,
        ResType::ClickEventType::TOUCH_EVENT, nullptr);
    SocPerfPlugin::GetInstance().HandleEventClick(touchData);
    const std::shared_ptr<ResData>& clickData = std::make_shared<ResData>(ResType::RES_TYPE_CLICK_RECOGNIZE,
        ResType::ClickEventType::CLICK_EVENT, nullptr);
    SocPerfPlugin::GetInstance().HandleEventClick(clickData);
    const std::shared_ptr<ResData>& invalidData = std::make_shared<ResData>(-1, -1, nullptr);
    SocPerfPlugin::GetInstance().HandleEventClick(invalidData);
    EXPECT_TRUE(SocPerfPlugin::GetInstance().socperfOnDemandSwitch_);
}

/*
 * @tc.name: SocPerfPluginTest_API_TEST_004
 * @tc.desc: test socperfplugin api
 * @tc.type FUNC
 * @tc.require: issueI78T3V
 */
HWTEST_F(SocPerfPluginTest, SocPerfPluginTest_API_TEST_004, Function | MediumTest | Level0)
{
    const std::shared_ptr<ResData>& loadPageStartData = std::make_shared<ResData>(ResType::RES_TYPE_LOAD_PAGE,
        ResType::LOAD_PAGE_START, nullptr);
    SocPerfPlugin::GetInstance().socperfOnDemandSwitch_ = false;
    SocPerfPlugin::GetInstance().HandleLoadPage(loadPageStartData);
    SocPerfPlugin::GetInstance().socperfOnDemandSwitch_ = true;
    SocPerfPlugin::GetInstance().HandleLoadPage(loadPageStartData);
    const std::shared_ptr<ResData>& loadPageEndData = std::make_shared<ResData>(ResType::RES_TYPE_LOAD_PAGE,
        ResType::LOAD_PAGE_COMPLETE, nullptr);
    SocPerfPlugin::GetInstance().HandleLoadPage(loadPageEndData);
    const std::shared_ptr<ResData>& invalidData = std::make_shared<ResData>(-1, -1, nullptr);
    SocPerfPlugin::GetInstance().HandleLoadPage(invalidData);
    EXPECT_TRUE(SocPerfPlugin::GetInstance().socperfOnDemandSwitch_);
}

/*
 * @tc.name: SocPerfPluginTest_API_TEST_005
 * @tc.desc: test socperfplugin api
 * @tc.type FUNC
 * @tc.require: issueI78T3V
 */
HWTEST_F(SocPerfPluginTest, SocPerfPluginTest_API_TEST_005, Function | MediumTest | Level0)
{
    const std::shared_ptr<ResData>& slideEventOnData = std::make_shared<ResData>(ResType::RES_TYPE_SLIDE_RECOGNIZE,
        ResType::SlideEventStatus::SLIDE_EVENT_ON, nullptr);
    SocPerfPlugin::GetInstance().HandleEventSlide(slideEventOnData);
    const std::shared_ptr<ResData>& slideEventOffData = std::make_shared<ResData>(ResType::RES_TYPE_SLIDE_RECOGNIZE,
        ResType::SlideEventStatus::SLIDE_EVENT_OFF, nullptr);
    SocPerfPlugin::GetInstance().HandleEventSlide(slideEventOffData);
    const std::shared_ptr<ResData>& slideNormalBeginData = std::make_shared<ResData>(ResType::RES_TYPE_SLIDE_RECOGNIZE,
        ResType::SlideEventStatus::SLIDE_NORMAL_BEGIN, nullptr);
    SocPerfPlugin::GetInstance().HandleEventSlide(slideNormalBeginData);
    const std::shared_ptr<ResData>& slideNormalEndData = std::make_shared<ResData>(ResType::RES_TYPE_SLIDE_RECOGNIZE,
        ResType::SlideEventStatus::SLIDE_NORMAL_END, nullptr);
    SocPerfPlugin::GetInstance().HandleEventSlide(slideNormalEndData);
    const std::shared_ptr<ResData>& invalidData = std::make_shared<ResData>(-1, -1, nullptr);
    SocPerfPlugin::GetInstance().HandleEventSlide(invalidData);
    EXPECT_TRUE(SocPerfPlugin::GetInstance().socperfOnDemandSwitch_);
}

/*
 * @tc.name: SocPerfPluginTest_API_TEST_006
 * @tc.desc: test socperfplugin api
 * @tc.type FUNC
 * @tc.require: issueI78T3V
 */
HWTEST_F(SocPerfPluginTest, SocPerfPluginTest_API_TEST_006, Function | MediumTest | Level0)
{
    const std::shared_ptr<ResData>& webGestureStartData = std::make_shared<ResData>(ResType::RES_TYPE_WEB_GESTURE,
        ResType::WebGesture::WEB_GESTURE_START, nullptr);
    SocPerfPlugin::GetInstance().HandleEventWebGesture(webGestureStartData);
    const std::shared_ptr<ResData>& webGestureEndData = std::make_shared<ResData>(ResType::RES_TYPE_WEB_GESTURE,
        ResType::WebGesture::WEB_GESTURE_END, nullptr);
    SocPerfPlugin::GetInstance().HandleEventWebGesture(webGestureEndData);
    SocPerfPlugin::GetInstance().HandleEventWebGesture(nullptr);
    const std::shared_ptr<ResData>& invalidData = std::make_shared<ResData>(-1, -1, nullptr);
    SocPerfPlugin::GetInstance().HandleEventWebGesture(invalidData);
    EXPECT_TRUE(SocPerfPlugin::GetInstance().socperfOnDemandSwitch_);
}

/*
 * @tc.name: SocPerfPluginTest_API_TEST_007
 * @tc.desc: test socperfplugin api
 * @tc.type FUNC
 * @tc.require: issueI78T3V
 */
HWTEST_F(SocPerfPluginTest, SocPerfPluginTest_API_TEST_007, Function | MediumTest | Level0)
{
    const std::shared_ptr<ResData>& windowResizingData = std::make_shared<ResData>(ResType::RES_TYPE_RESIZE_WINDOW,
        ResType::WindowResizeType::WINDOW_RESIZING, nullptr);
    SocPerfPlugin::GetInstance().HandleResizeWindow(windowResizingData);
    const std::shared_ptr<ResData>& webGestureEndData = std::make_shared<ResData>(ResType::RES_TYPE_WEB_GESTURE,
        ResType::WindowResizeType::WINDOW_RESIZE_STOP, nullptr);
    SocPerfPlugin::GetInstance().HandleResizeWindow(webGestureEndData);
    SocPerfPlugin::GetInstance().HandleResizeWindow(nullptr);
    const std::shared_ptr<ResData>& invalidData = std::make_shared<ResData>(-1, -1, nullptr);
    SocPerfPlugin::GetInstance().HandleResizeWindow(invalidData);
    EXPECT_TRUE(SocPerfPlugin::GetInstance().socperfOnDemandSwitch_);
}

/*
 * @tc.name: SocPerfPluginTest_API_TEST_008
 * @tc.desc: test socperfplugin api
 * @tc.type FUNC
 * @tc.require: issueI78T3V
 */
HWTEST_F(SocPerfPluginTest, SocPerfPluginTest_API_TEST_008, Function | MediumTest | Level0)
{
    const std::shared_ptr<ResData>& windowMovingData = std::make_shared<ResData>(ResType::RES_TYPE_MOVE_WINDOW,
        ResType::WindowMoveType::WINDOW_MOVING, nullptr);
    SocPerfPlugin::GetInstance().HandleMoveWindow(windowMovingData);
    const std::shared_ptr<ResData>& windowMoveStop = std::make_shared<ResData>(ResType::RES_TYPE_MOVE_WINDOW,
        ResType::WindowMoveType::WINDOW_MOVE_STOP, nullptr);
    SocPerfPlugin::GetInstance().HandleMoveWindow(windowMoveStop);
    SocPerfPlugin::GetInstance().HandleMoveWindow(nullptr);
    const std::shared_ptr<ResData>& invalidData = std::make_shared<ResData>(-1, -1, nullptr);
    SocPerfPlugin::GetInstance().HandleMoveWindow(invalidData);
    EXPECT_TRUE(SocPerfPlugin::GetInstance().socperfOnDemandSwitch_);
}

/*
 * @tc.name: SocPerfPluginTest_API_TEST_009
 * @tc.desc: test socperfplugin api
 * @tc.type FUNC
 * @tc.require: issueI78T3V
 */
HWTEST_F(SocPerfPluginTest, SocPerfPluginTest_API_TEST_009, Function | MediumTest | Level0)
{
    const std::shared_ptr<ResData>& animationEndData = std::make_shared<ResData>(
        ResType::RES_TYPE_SHOW_REMOTE_ANIMATION, ResType::ShowRemoteAnimationStatus::ANIMATION_END, nullptr);
    SocPerfPlugin::GetInstance().socperfOnDemandSwitch_ = false;
    SocPerfPlugin::GetInstance().HandleRemoteAnimation(animationEndData);
    SocPerfPlugin::GetInstance().socperfOnDemandSwitch_ = true;
    const std::shared_ptr<ResData>& animationBeginData = std::make_shared<ResData>(
        ResType::RES_TYPE_SHOW_REMOTE_ANIMATION, ResType::ShowRemoteAnimationStatus::ANIMATION_BEGIN, nullptr);
    SocPerfPlugin::GetInstance().HandleRemoteAnimation(animationBeginData);
    SocPerfPlugin::GetInstance().HandleRemoteAnimation(animationEndData);
    const std::shared_ptr<ResData>& invalidData = std::make_shared<ResData>(-1, -1, nullptr);
    SocPerfPlugin::GetInstance().HandleRemoteAnimation(invalidData);
    EXPECT_TRUE(SocPerfPlugin::GetInstance().socperfOnDemandSwitch_);
}

/*
 * @tc.name: SocPerfPluginTest_API_TEST_010
 * @tc.desc: test socperfplugin api
 * @tc.type FUNC
 * @tc.require: issueI78T3V
 */
HWTEST_F(SocPerfPluginTest, SocPerfPluginTest_API_TEST_010, Function | MediumTest | Level0)
{
    const std::shared_ptr<ResData>& dragStartData = std::make_shared<ResData>(ResType::RES_TYPE_DRAG_STATUS_BAR,
        ResType::StatusBarDragStatus::DRAG_START, nullptr);
    SocPerfPlugin::GetInstance().HandleDragStatusBar(dragStartData);
    const std::shared_ptr<ResData>& dragEndData = std::make_shared<ResData>(ResType::RES_TYPE_DRAG_STATUS_BAR,
        ResType::StatusBarDragStatus::DRAG_END, nullptr);
    SocPerfPlugin::GetInstance().HandleDragStatusBar(dragEndData);
    SocPerfPlugin::GetInstance().HandleDragStatusBar(nullptr);
    const std::shared_ptr<ResData>& invalidData = std::make_shared<ResData>(-1, -1, nullptr);
    SocPerfPlugin::GetInstance().HandleDragStatusBar(invalidData);
    EXPECT_TRUE(SocPerfPlugin::GetInstance().socperfOnDemandSwitch_);
}

/*
 * @tc.name: SocPerfPluginTest_API_TEST_011
 * @tc.desc: test socperfplugin api
 * @tc.type FUNC
 * @tc.require: issueI78T3V
 */
HWTEST_F(SocPerfPluginTest, SocPerfPluginTest_API_TEST_011, Function | MediumTest | Level0)
{
    const std::shared_ptr<ResData>& webGestureMoveStartData = std::make_shared<ResData>(
        ResType::RES_TYPE_WEB_GESTURE_MOVE, ResType::WebGestureMove::WEB_GESTURE_MOVE_START, nullptr);
    SocPerfPlugin::GetInstance().HandleWebGestureMove(webGestureMoveStartData);
    const std::shared_ptr<ResData>& webGestureMoveEndData = std::make_shared<ResData>(
        ResType::RES_TYPE_WEB_GESTURE_MOVE, ResType::WebGestureMove::WEB_GESTURE_MOVE_END, nullptr);
    SocPerfPlugin::GetInstance().HandleWebGestureMove(webGestureMoveEndData);
    SocPerfPlugin::GetInstance().HandleWebGestureMove(nullptr);
    const std::shared_ptr<ResData>& invalidData = std::make_shared<ResData>(-1, -1, nullptr);
    SocPerfPlugin::GetInstance().HandleWebGestureMove(invalidData);
    EXPECT_TRUE(SocPerfPlugin::GetInstance().socperfOnDemandSwitch_);
}

/*
 * @tc.name: SocPerfPluginTest_API_TEST_012
 * @tc.desc: test socperfplugin api
 * @tc.type FUNC
 * @tc.require: issueI78T3V
 */
HWTEST_F(SocPerfPluginTest, SocPerfPluginTest_API_TEST_012, Function | MediumTest | Level0)
{
    const std::shared_ptr<ResData>& webSlideNormalStartData = std::make_shared<ResData>(
        ResType::RES_TYPE_WEB_SLIDE_NORMAL, ResType::WebSlideNormal::WEB_SLIDE_NORMAL_START, nullptr);
    SocPerfPlugin::GetInstance().HandleWebSlideNormal(webSlideNormalStartData);
    const std::shared_ptr<ResData>& webSlideNormalEndData = std::make_shared<ResData>(
        ResType::RES_TYPE_WEB_SLIDE_NORMAL, ResType::WebSlideNormal::WEB_SLIDE_NORMAL_END, nullptr);
    SocPerfPlugin::GetInstance().HandleWebSlideNormal(webSlideNormalEndData);
    SocPerfPlugin::GetInstance().HandleWebSlideNormal(nullptr);
    const std::shared_ptr<ResData>& invalidData = std::make_shared<ResData>(-1, -1, nullptr);
    SocPerfPlugin::GetInstance().HandleWebSlideNormal(invalidData);
    EXPECT_TRUE(SocPerfPlugin::GetInstance().socperfOnDemandSwitch_);
}

/*
 * @tc.name: SocPerfPluginTest_API_TEST_013
 * @tc.desc: test socperfplugin api
 * @tc.type FUNC
 * @tc.require: issueI78T3V
 */
HWTEST_F(SocPerfPluginTest, SocPerfPluginTest_API_TEST_013, Function | MediumTest | Level0)
{
    const std::shared_ptr<ResData>& data = std::make_shared<ResData>(ResType::RES_TYPE_LOAD_URL, -1, nullptr);
    SocPerfPlugin::GetInstance().HandleLoadUrl(data);
    EXPECT_TRUE(SocPerfPlugin::GetInstance().socperfOnDemandSwitch_);
}

/*
 * @tc.name: SocPerfPluginTest_API_TEST_014
 * @tc.desc: test socperfplugin api
 * @tc.type FUNC
 * @tc.require: issueI78T3V
 */
HWTEST_F(SocPerfPluginTest, SocPerfPluginTest_API_TEST_014, Function | MediumTest | Level0)
{
    const std::shared_ptr<ResData>& data = std::make_shared<ResData>(ResType::RES_TYPE_MOUSEWHEEL, -1, nullptr);
    SocPerfPlugin::GetInstance().HandleMousewheel(data);
    EXPECT_TRUE(SocPerfPlugin::GetInstance().socperfOnDemandSwitch_);
}

/*
 * @tc.name: SocPerfPluginTest_API_TEST_015
 * @tc.desc: test socperfplugin api
 * @tc.type FUNC
 * @tc.require: issueI78T3V
 */
HWTEST_F(SocPerfPluginTest, SocPerfPluginTest_API_TEST_015, Function | MediumTest | Level0)
{
    const std::shared_ptr<ResData>& data = std::make_shared<ResData>(ResType::RES_TYPE_POP_PAGE, -1, nullptr);
    SocPerfPlugin::GetInstance().HandlePopPage(data);
    EXPECT_TRUE(SocPerfPlugin::GetInstance().socperfOnDemandSwitch_);
}

} // namespace SOCPERF
} // namespace OHOS