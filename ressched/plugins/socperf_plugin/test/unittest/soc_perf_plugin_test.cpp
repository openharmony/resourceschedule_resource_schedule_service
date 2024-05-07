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
    SUCCEED();
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
    SocPerfPlugin::GetInstance().HandleAppAbilityStart(coldStartData);
    SocPerfPlugin::GetInstance().HandleAppAbilityStart(coldStartData);
    const std::shared_ptr<ResData>& warmStartData = std::make_shared<ResData>(ResType::RES_TYPE_APP_ABILITY_START,
        ResType::AppStartType::APP_WARM_START, nullptr);
    SocPerfPlugin::GetInstance().HandleAppAbilityStart(warmStartData);
    const std::shared_ptr<ResData>& invalidData = std::make_shared<ResData>(-1, -1, nullptr);
    SocPerfPlugin::GetInstance().HandleAppAbilityStart(invalidData);
    SUCCEED();
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
    SUCCEED();
}

/*
 * @tc.name: SocPerfPluginTest_API_TEST_003
 * @tc.desc: test socperfplugin api
 * @tc.type FUNC
 * @tc.require: issueI78T3V
 */
HWTEST_F(SocPerfPluginTest, SocPerfPluginTest_API_TEST_003, Function | MediumTest | Level0)
{
    const std::shared_ptr<ResData>& touchDownData = std::make_shared<ResData>(ResType::RES_TYPE_CLICK_RECOGNIZE,
        ResType::ClickEventType::TOUCH_EVENT_DOWN, nullptr);
    SocPerfPlugin::GetInstance().HandleEventClick(touchDownData);
    const std::shared_ptr<ResData>& clickData = std::make_shared<ResData>(ResType::RES_TYPE_CLICK_RECOGNIZE,
        ResType::ClickEventType::CLICK_EVENT, nullptr);
    SocPerfPlugin::GetInstance().HandleEventClick(clickData);
    const std::shared_ptr<ResData>& touchUpData = std::make_shared<ResData>(ResType::RES_TYPE_CLICK_RECOGNIZE,
        ResType::ClickEventType::TOUCH_EVENT_UP, nullptr);
    SocPerfPlugin::GetInstance().HandleEventClick(touchUpData);
    const std::shared_ptr<ResData>& invalidData = std::make_shared<ResData>(-1, -1, nullptr);
    SocPerfPlugin::GetInstance().HandleEventClick(invalidData);
    SUCCEED();
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
    SocPerfPlugin::GetInstance().HandleLoadPage(loadPageStartData);
    SocPerfPlugin::GetInstance().HandleLoadPage(loadPageStartData);
    const std::shared_ptr<ResData>& loadPageEndData = std::make_shared<ResData>(ResType::RES_TYPE_LOAD_PAGE,
        ResType::LOAD_PAGE_COMPLETE, nullptr);
    SocPerfPlugin::GetInstance().HandleLoadPage(loadPageEndData);
    const std::shared_ptr<ResData>& invalidData = std::make_shared<ResData>(-1, -1, nullptr);
    SocPerfPlugin::GetInstance().HandleLoadPage(invalidData);
    SUCCEED();
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
    SUCCEED();
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
    SUCCEED();
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
    SUCCEED();
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
    SUCCEED();
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
    SocPerfPlugin::GetInstance().HandleRemoteAnimation(animationEndData);
    const std::shared_ptr<ResData>& animationBeginData = std::make_shared<ResData>(
        ResType::RES_TYPE_SHOW_REMOTE_ANIMATION, ResType::ShowRemoteAnimationStatus::ANIMATION_BEGIN, nullptr);
    SocPerfPlugin::GetInstance().HandleRemoteAnimation(animationBeginData);
    const std::shared_ptr<ResData>& unlockEndData = std::make_shared<ResData>(
        ResType::RES_TYPE_SHOW_REMOTE_ANIMATION, ResType::ShowRemoteAnimationStatus::ANIMATION_UNLOCK_END, nullptr);
    SocPerfPlugin::GetInstance().HandleRemoteAnimation(unlockEndData);
    const std::shared_ptr<ResData>& unlockBeginData = std::make_shared<ResData>(
        ResType::RES_TYPE_SHOW_REMOTE_ANIMATION, ResType::ShowRemoteAnimationStatus::ANIMATION_UNLOCK_BEGIN, nullptr);
    SocPerfPlugin::GetInstance().HandleRemoteAnimation(unlockBeginData);
    SocPerfPlugin::GetInstance().HandleRemoteAnimation(animationEndData);
    SocPerfPlugin::GetInstance().HandleRemoteAnimation(unlockEndData);
    const std::shared_ptr<ResData>& invalidData = std::make_shared<ResData>(-1, -1, nullptr);
    SocPerfPlugin::GetInstance().HandleRemoteAnimation(invalidData);
    SUCCEED();
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
    SUCCEED();
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
    SUCCEED();
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
    SUCCEED();
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
    SUCCEED();
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
    SUCCEED();
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
    SUCCEED();
}

/*
 * @tc.name: SocPerfPluginTest_API_TEST_016
 * @tc.desc: test socperfplugin api
 * @tc.type FUNC
 * @tc.require: issueI78T3V
 */
HWTEST_F(SocPerfPluginTest, SocPerfPluginTest_API_TEST_016, Function | MediumTest | Level0)
{
    std::shared_ptr<ResData> invalidValuedata = std::make_shared<ResData>(ResType::RES_TYPE_APP_STATE_CHANGE,
        ResType::ProcessStatus::PROCESS_READY, nullptr);
    bool ret = SocPerfPlugin::GetInstance().HandleAppStateChange(invalidValuedata);
    EXPECT_FALSE(ret);

    nlohmann::json invalidPayload;
    invalidPayload["extensionType"] = 0;
    std::shared_ptr<ResData> invalidPayloadTypedata = std::make_shared<ResData>(ResType::RES_TYPE_APP_STATE_CHANGE,
       ResType::ProcessStatus::PROCESS_CREATED, invalidPayload);
    ret = SocPerfPlugin::GetInstance().HandleAppStateChange(invalidPayloadTypedata);
    EXPECT_FALSE(ret);

    nlohmann::json payload;
    payload["extensionType"] = 2;
    std::shared_ptr<ResData> data = std::make_shared<ResData>(ResType::RES_TYPE_APP_STATE_CHANGE,
        ResType::ProcessStatus::PROCESS_CREATED, payload);
    ret = SocPerfPlugin::GetInstance().HandleAppStateChange(data);
    EXPECT_TRUE(ret);

    payload["extensionType"] = 255;
    data->payload = payload;
    ret = SocPerfPlugin::GetInstance().HandleAppStateChange(data);
    EXPECT_TRUE(ret);

    payload["extensionType"] = 256;
    data->payload = payload;
    ret = SocPerfPlugin::GetInstance().HandleAppStateChange(data);
    EXPECT_TRUE(ret);

    payload["extensionType"] = 500;
    data->payload = payload;
    ret = SocPerfPlugin::GetInstance().HandleAppStateChange(data);
    EXPECT_TRUE(ret);

    payload["extensionType"] = 600;
    data->payload = payload;
    ret = SocPerfPlugin::GetInstance().HandleAppStateChange(data);
    EXPECT_FALSE(ret);
}

/*
 * @tc.name: SocPerfPluginTest_API_TEST_017
 * @tc.desc: test socperfplugin api
 * @tc.type FUNC
 * @tc.require: issueI78T3V
 */
HWTEST_F(SocPerfPluginTest, SocPerfPluginTest_API_TEST_017, Function | MediumTest | Level0)
{
    nlohmann::json payload;
    payload["deviceMode"] = "test";
    std::shared_ptr<ResData> normalValuedata = std::make_shared<ResData>(ResType::RES_TYPE_DEVICE_MODE_STATUS,
        ResType::DeviceModeStatus::MODE_ENTER, payload);
    SocPerfPlugin::GetInstance().HandleDeviceModeStatusChange(normalValuedata);
    SUCCEED();

    nlohmann::json payload2;
    payload["test"] = "test";
    std::shared_ptr<ResData> invalidPayload = std::make_shared<ResData>(ResType::RES_TYPE_DEVICE_MODE_STATUS,
        ResType::DeviceModeStatus::MODE_ENTER, payload2);
    SocPerfPlugin::GetInstance().HandleDeviceModeStatusChange(invalidPayload);
    SUCCEED();

    nlohmann::json payload3;
    payload["test"] = "test";
    int64_t invalidType = 3;
    std::shared_ptr<ResData> invalidValue = std::make_shared<ResData>(ResType::RES_TYPE_DEVICE_MODE_STATUS,
        invalidType, payload3);
    SocPerfPlugin::GetInstance().HandleDeviceModeStatusChange(invalidValue);
    SUCCEED();

    nlohmann::json payload4;
    payload["deviceMode"] = "";
    std::shared_ptr<ResData> emptyPayload = std::make_shared<ResData>(ResType::RES_TYPE_DEVICE_MODE_STATUS,
        ResType::DeviceModeStatus::MODE_ENTER, payload4);
    SocPerfPlugin::GetInstance().HandleDeviceModeStatusChange(emptyPayload);
    SUCCEED();

    nlohmann::json payload5;
    payload["deviceMode"] = "ABCEDFGHABCEDFGHABCEDFGHABCEDFGHABCEDFGHABCEDFGHABCEDFGHABCEDFGHZ";
    std::shared_ptr<ResData> maxLenPayload = std::make_shared<ResData>(ResType::RES_TYPE_DEVICE_MODE_STATUS,
        ResType::DeviceModeStatus::MODE_ENTER, payload5);
    SocPerfPlugin::GetInstance().HandleDeviceModeStatusChange(maxLenPayload);
    SUCCEED();
}

/*
 * @tc.name: SocPerfPluginTest_API_TEST_018
 * @tc.desc: test socperfplugin api
 * @tc.type FUNC
 * @tc.require: issueI78T3V
 */
HWTEST_F(SocPerfPluginTest, SocPerfPluginTest_API_TEST_018, Function | MediumTest | Level0)
{
    const std::shared_ptr<ResData>& scbAnimationEndData = std::make_shared<ResData>(
        SocPerfPlugin::GetInstance().RES_TYPE_SCENE_BOARD_ID,
        ResType::ShowRemoteAnimationStatus::ANIMATION_END, nullptr);
    bool ret = SocPerfPlugin::GetInstance().HandleSocperfSceneBoard(scbAnimationEndData);
    EXPECT_TRUE(ret);
    const std::shared_ptr<ResData>& scbAnimationBeginData = std::make_shared<ResData>(
        SocPerfPlugin::GetInstance().RES_TYPE_SCENE_BOARD_ID,
        ResType::ShowRemoteAnimationStatus::ANIMATION_BEGIN, nullptr);
    ret = SocPerfPlugin::GetInstance().HandleSocperfSceneBoard(scbAnimationBeginData);
    EXPECT_TRUE(ret);
    ret = SocPerfPlugin::GetInstance().HandleSocperfSceneBoard(nullptr);
    EXPECT_FALSE(ret);
}

/*
 * @tc.name: SocPerfPluginTest_API_TEST_019
 * @tc.desc: test socperfplugin api
 * @tc.type FUNC
 * @tc.require: issueI78T3V
 */
HWTEST_F(SocPerfPluginTest, SocPerfPluginTest_API_TEST_019, Function | MediumTest | Level0)
{
    const std::shared_ptr<ResData>& ancoEvnetData = std::make_shared<ResData>(
        ResType::RES_TYPE_ANCO_CUST, 1001, nullptr);
    bool ret = SocPerfPlugin::GetInstance().HandleCustEvent(ancoEvnetData);
    EXPECT_TRUE(ret);
    const std::shared_ptr<ResData>& ancoEvnetDataInvalid = std::make_shared<ResData>(
        ResType::RES_TYPE_ANCO_CUST, -1, nullptr);
    ret = SocPerfPlugin::GetInstance().HandleCustEvent(ancoEvnetDataInvalid);
    EXPECT_FALSE(ret);
}

/*
 * @tc.name: SocPerfPluginTest_API_TEST_020
 * @tc.desc: test socperfplugin api
 * @tc.type FUNC
 * @tc.require: issueI78T3V
 */
HWTEST_F(SocPerfPluginTest, SocPerfPluginTest_API_TEST_020, Function | MediumTest | Level0)
{
    const std::shared_ptr<ResData>& socPerfCustEventBeginData = std::make_shared<ResData>(
        ResType::RES_TYPE_SOCPERF_CUST_EVENT_BEGIN, 1001, nullptr);
    bool ret = SocPerfPlugin::GetInstance().HandleCustEventBegin(socPerfCustEventBeginData);
    EXPECT_TRUE(ret);
    const std::shared_ptr<ResData>& socPerfCustEventBeginDataInvalid = std::make_shared<ResData>(
        ResType::RES_TYPE_SOCPERF_CUST_EVENT_BEGIN, -1, nullptr);
    ret = SocPerfPlugin::GetInstance().HandleCustEventBegin(socPerfCustEventBeginDataInvalid);
    EXPECT_FALSE(ret);
}

/*
 * @tc.name: SocPerfPluginTest_API_TEST_021
 * @tc.desc: test socperfplugin api
 * @tc.type FUNC
 * @tc.require: issueI78T3V
 */
HWTEST_F(SocPerfPluginTest, SocPerfPluginTest_API_TEST_021, Function | MediumTest | Level0)
{
    const std::shared_ptr<ResData>& socPerfCustEventEndData = std::make_shared<ResData>(
        ResType::RES_TYPE_SOCPERF_CUST_EVENT_END, 1001, nullptr);
    bool ret = SocPerfPlugin::GetInstance().HandleCustEventEnd(socPerfCustEventEndData);
    EXPECT_TRUE(ret);
    const std::shared_ptr<ResData>& socPerfCustEventEndDataInvalid = std::make_shared<ResData>(
        ResType::RES_TYPE_SOCPERF_CUST_EVENT_END, -1, nullptr);
    ret = SocPerfPlugin::GetInstance().HandleCustEventEnd(socPerfCustEventEndDataInvalid);
    EXPECT_FALSE(ret);
}
} // namespace SOCPERF
} // namespace OHOS