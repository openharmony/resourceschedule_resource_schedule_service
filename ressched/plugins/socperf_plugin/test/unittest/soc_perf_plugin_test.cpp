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
    EXPECT_NE(coldStartData, nullptr);
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
    EXPECT_NE(data, nullptr);
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
    EXPECT_NE(touchDownData, nullptr);
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
    EXPECT_NE(loadPageStartData, nullptr);
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
    EXPECT_NE(slideEventOnData, nullptr);
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
    EXPECT_NE(webGestureStartData, nullptr);
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
    EXPECT_NE(windowResizingData, nullptr);
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
    EXPECT_NE(windowMovingData, nullptr);
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
    EXPECT_NE(animationEndData, nullptr);
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
    EXPECT_NE(dragStartData, nullptr);
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
    EXPECT_NE(webGestureMoveStartData, nullptr);
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
    EXPECT_NE(webSlideNormalStartData, nullptr);
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
    EXPECT_NE(data, nullptr);
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
    EXPECT_NE(data, nullptr);
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
    EXPECT_NE(data, nullptr);
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
    const std::shared_ptr<ResData>& maxLenPayload = std::make_shared<ResData>(ResType::RES_TYPE_DEVICE_MODE_STATUS,
        ResType::DeviceModeStatus::MODE_ENTER, payload5);
    SocPerfPlugin::GetInstance().HandleDeviceModeStatusChange(maxLenPayload);
    EXPECT_NE(maxLenPayload, nullptr);
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

#ifdef RESSCHED_RESOURCESCHEDULE_CUST_SOC_PERF_ENABLE
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
#endif // RESSCHED_RESOURCESCHEDULE_CUST_SOC_PERF_ENABLE

/*
 * @tc.name: SocPerfPluginTest_API_TEST_022
 * @tc.desc: test socperfplugin api
 * @tc.type FUNC
 * @tc.require: issueI78T3V
 */
HWTEST_F(SocPerfPluginTest, SocPerfPluginTest_API_TEST_022, Function | MediumTest | Level0)
{
    const std::shared_ptr<ResData>& socPerfPowerModeChangedStartData = std::make_shared<ResData>(
        ResType::RES_TYPE_POWER_MODE_CHANGED, 601, nullptr);
    bool ret = SocPerfPlugin::GetInstance().HandlePowerModeChanged(socPerfPowerModeChangedStartData);
    EXPECT_TRUE(ret);
    const std::shared_ptr<ResData>& socPerfPowerModeChangedEndData = std::make_shared<ResData>(
        ResType::RES_TYPE_POWER_MODE_CHANGED, 602, nullptr);
    ret = SocPerfPlugin::GetInstance().HandlePowerModeChanged(socPerfPowerModeChangedEndData);
    EXPECT_TRUE(ret);
}

/*
 * @tc.name: SocPerfPluginTest_API_TEST_023
 * @tc.desc: test socperfplugin api
 * @tc.type FUNC
 * @tc.require: issueI78T3V
 */
HWTEST_F(SocPerfPluginTest, SocPerfPluginTest_API_TEST_023, Function | MediumTest | Level0)
{
    const std::shared_ptr<ResData>& socPerfBmmMoniterStartData = std::make_shared<ResData>(
        ResType::RES_TYPE_BMM_MONITER_CHANGE_EVENT, ResType::BmmMoniterStatus::BMM_BACKGROUND, nullptr);
    bool ret = SocPerfPlugin::GetInstance().HandleBmmMoniterStatus(socPerfBmmMoniterStartData);
    EXPECT_TRUE(ret);
    const std::shared_ptr<ResData>& socPerfBmmMoniterEndData = std::make_shared<ResData>(
        ResType::RES_TYPE_BMM_MONITER_CHANGE_EVENT, ResType::BmmMoniterStatus::BMM_CLOSE, nullptr);
    ret = SocPerfPlugin::GetInstance().HandleBmmMoniterStatus(socPerfBmmMoniterEndData);
    EXPECT_TRUE(ret);
    const std::shared_ptr<ResData>& socPerfBmmMoniterDataInvalid = std::make_shared<ResData>(
        ResType::RES_TYPE_BMM_MONITER_CHANGE_EVENT, -1, nullptr);
    ret = SocPerfPlugin::GetInstance().HandleBmmMoniterStatus(socPerfBmmMoniterDataInvalid);
    EXPECT_FALSE(ret);
}

/*
 * @tc.name: SocPerfPluginTest_API_TEST_024
 * @tc.desc: test socperfplugin api
 * @tc.type FUNC
 * @tc.require: issueI78T3V
 */
HWTEST_F(SocPerfPluginTest, SocPerfPluginTest_API_TEST_024, Function | MediumTest | Level0)
{
    const std::shared_ptr<ResData>& socPerfSceneRotationStartData = std::make_shared<ResData>(
        ResType::RES_TYPE_SCENE_ROTATION, 0, nullptr);
    bool ret = SocPerfPlugin::GetInstance().HandleSceneRotation(socPerfSceneRotationStartData);
    EXPECT_TRUE(ret);
    const std::shared_ptr<ResData>& socPerfSceneRotationEndData = std::make_shared<ResData>(
        ResType::RES_TYPE_SCENE_ROTATION, 1, nullptr);
    ret = SocPerfPlugin::GetInstance().HandleSceneRotation(socPerfSceneRotationEndData);
    EXPECT_TRUE(ret);
}

/*
 * @tc.name: SocPerfPluginTest_API_TEST_025
 * @tc.desc: test socperfplugin api
 * @tc.type FUNC
 * @tc.require: issueI78T3V
 */
HWTEST_F(SocPerfPluginTest, SocPerfPluginTest_API_TEST_025, Function | MediumTest | Level0)
{
    const std::shared_ptr<ResData>& socPerfAppColdStartData = std::make_shared<ResData>(
        ResType::RES_TYPE_ONLY_PERF_APP_COLD_START, 0, nullptr);
    bool ret = SocPerfPlugin::GetInstance().HandleAppColdStartEx(socPerfAppColdStartData);
    EXPECT_TRUE(ret);
    const std::shared_ptr<ResData>& socPerfAppColdEndData = std::make_shared<ResData>(
        ResType::RES_TYPE_ONLY_PERF_APP_COLD_START, 1, nullptr);
    ret = SocPerfPlugin::GetInstance().HandleAppColdStartEx(socPerfAppColdEndData);
    EXPECT_TRUE(ret);
}

/*
 * @tc.name: SocPerfPluginTest_API_TEST_026
 * @tc.desc: test socperfplugin api
 * @tc.type FUNC
 * @tc.require: issueI78T3V
 */
HWTEST_F(SocPerfPluginTest, SocPerfPluginTest_API_TEST_026, Function | MediumTest | Level0)
{
    const std::shared_ptr<ResData>& socPerfRgmBootingStartData = std::make_shared<ResData>(
        SocPerfPlugin::GetInstance().RES_TYPE_RGM_BOOTING_STATUS, 0, nullptr);
    bool ret = SocPerfPlugin::GetInstance().HandleRgmBootingStatus(socPerfRgmBootingStartData);
    EXPECT_TRUE(ret);
    const std::shared_ptr<ResData>& socPerfRgmBootingEndData = std::make_shared<ResData>(
        SocPerfPlugin::GetInstance().RES_TYPE_RGM_BOOTING_STATUS, -1, nullptr);
    ret = SocPerfPlugin::GetInstance().HandleRgmBootingStatus(socPerfRgmBootingEndData);
    EXPECT_TRUE(ret);
}

/*
 * @tc.name: SocPerfPluginTest_API_TEST_027
 * @tc.desc: test socperfplugin api
 * @tc.type FUNC
 * @tc.require: issueI78T3V
 */
HWTEST_F(SocPerfPluginTest, SocPerfPluginTest_API_TEST_027, Function | MediumTest | Level0)
{
    const std::shared_ptr<ResData>& socPerfAccountActivatingData = std::make_shared<ResData>(
        ResType::RES_TYPE_ACCOUNT_ACTIVATING, ResType::AccountActivatingStatus::ACCOUNT_ACTIVATING_START, nullptr);
    bool ret = SocPerfPlugin::GetInstance().HandleSocperfAccountActivating(socPerfAccountActivatingData);
    EXPECT_TRUE(ret);
    const std::shared_ptr<ResData>& socPerfAccountActivatingInvalid = std::make_shared<ResData>(
        ResType::RES_TYPE_ACCOUNT_ACTIVATING, -1, nullptr);
    ret = SocPerfPlugin::GetInstance().HandleSocperfAccountActivating(socPerfAccountActivatingInvalid);
    EXPECT_TRUE(ret);
}

/*
 * @tc.name: SocPerfPluginTest_API_TEST_028
 * @tc.desc: test socperfplugin api
 * @tc.type FUNC
 * @tc.require: issueI78T3V
 */
HWTEST_F(SocPerfPluginTest, SocPerfPluginTest_API_TEST_028, Function | MediumTest | Level0)
{
    nlohmann::json payload;
    payload["deviceMode"] = "displayMain";
    std::shared_ptr<ResData> invalidPayload = std::make_shared<ResData>(ResType::RES_TYPE_DEVICE_MODE_STATUS,
        ResType::DeviceModeStatus::MODE_ENTER, payload);
    SocPerfPlugin::GetInstance().HandleDeviceModeStatusChange(invalidPayload);

    std::shared_ptr<ResData> normalValuedata = std::make_shared<ResData>(ResType::RES_TYPE_SCREEN_STATUS,
        ResType::ScreenStatus::SCREEN_ON, nullptr);
    bool ret = SocPerfPlugin::GetInstance().HandleScreenStatusAnalysis(normalValuedata);
    EXPECT_TRUE(ret);

    std::shared_ptr<ResData> normalValuedata2 = std::make_shared<ResData>(ResType::RES_TYPE_SCREEN_STATUS,
        ResType::ScreenStatus::SCREEN_OFF, nullptr);
    ret = SocPerfPlugin::GetInstance().HandleScreenStatusAnalysis(normalValuedata2);
    EXPECT_TRUE(ret);

    nlohmann::json payload1;
    payload1["deviceMode"] = "displayFull";
    std::shared_ptr<ResData> invalidPayload1 = std::make_shared<ResData>(ResType::RES_TYPE_DEVICE_MODE_STATUS,
        ResType::DeviceModeStatus::MODE_ENTER, payload1);
    SocPerfPlugin::GetInstance().HandleDeviceModeStatusChange(invalidPayload1);

    std::shared_ptr<ResData> normalValuedata3 = std::make_shared<ResData>(ResType::RES_TYPE_SCREEN_STATUS,
        ResType::ScreenStatus::SCREEN_ON, nullptr);
    ret = SocPerfPlugin::GetInstance().HandleScreenStatusAnalysis(normalValuedata3);
    EXPECT_TRUE(ret);

    std::shared_ptr<ResData> normalValuedata4 = std::make_shared<ResData>(ResType::RES_TYPE_SCREEN_STATUS,
        ResType::ScreenStatus::SCREEN_OFF, nullptr);
    ret = SocPerfPlugin::GetInstance().HandleScreenStatusAnalysis(normalValuedata4);
    EXPECT_TRUE(ret);

    ret = SocPerfPlugin::GetInstance().HandleScreenStatusAnalysis(nullptr);
    EXPECT_FALSE(ret);
}

/*
 * @tc.name: SocPerfPluginTest_API_TEST_029
 * @tc.desc: test socperfplugin api
 * @tc.type FUNC
 * @tc.require: issueI78T3V
 */
HWTEST_F(SocPerfPluginTest, SocPerfPluginTest_API_TEST_029, Function | MediumTest | Level0)
{
    bool ret = SocPerfPlugin::GetInstance().HandleSubValue("");
    EXPECT_FALSE(ret);
    ret = SocPerfPlugin::GetInstance().HandleSubValue("tencent,alipay");
    EXPECT_TRUE(ret);
}

/*
 * @tc.name: SocPerfPluginTest_API_TEST_030
 * @tc.desc: test socperfplugin api
 * @tc.type FUNC
 * @tc.require: issueI78T3V
 */
HWTEST_F(SocPerfPluginTest, SocPerfPluginTest_API_TEST_030, Function | MediumTest | Level0)
{
    nlohmann::json payload1;
    payload1["size"] = "1";
    std::shared_ptr<ResData> normalData1 = std::make_shared<ResData>(111, 0, payload1);
    bool ret = SocPerfPlugin::GetInstance().ParsePayload(normalData1, "notSize") == -1;
    EXPECT_TRUE(ret);
    ret = SocPerfPlugin::GetInstance().ParsePayload(normalData1, "size") == 1;
    EXPECT_TRUE(ret);

    nlohmann::json payload2;
    payload2["size"] = 1;
    std::shared_ptr<ResData> normalData2 = std::make_shared<ResData>(111, 0, payload2);
    ret = SocPerfPlugin::GetInstance().ParsePayload(normalData2, "size") == 1;
    EXPECT_TRUE(ret);
}

/*
 * @tc.name: SocPerfPluginTest_API_TEST_031
 * @tc.desc: test socperfplugin api
 * @tc.type FUNC
 * @tc.require: issueI78T3V
 */
HWTEST_F(SocPerfPluginTest, SocPerfPluginTest_API_TEST_031, Function | MediumTest | Level0)
{
    nlohmann::json payload1;
    payload1["uid"] = "100111";
    std::shared_ptr<ResData> normalData1 = std::make_shared<ResData>(111, 0, payload1);
    int32_t appType = -1;
    bool ret = SocPerfPlugin::GetInstance().UpdateUidToAppMsgMap(normalData1, appType, "qq");
    EXPECT_FALSE(ret);

    appType = 0;
    ret = SocPerfPlugin::GetInstance().UpdateUidToAppMsgMap(normalData1, appType, "qq");
    EXPECT_FALSE(ret);

    appType = 3;
    ret = SocPerfPlugin::GetInstance().UpdateUidToAppMsgMap(normalData1, appType, "qq");
    EXPECT_TRUE(ret);

    nlohmann::json payload2;
    payload2["uid"] = "-1";
    std::shared_ptr<ResData> normalData2 = std::make_shared<ResData>(111, 0, payload2);
    ret = SocPerfPlugin::GetInstance().UpdateUidToAppMsgMap(normalData2, appType, "qq");
    EXPECT_FALSE(ret);
}

/*
 * @tc.name: SocPerfPluginTest_API_TEST_032
 * @tc.desc: test socperfplugin api
 * @tc.type FUNC
 * @tc.require: issueI78T3V
 */
HWTEST_F(SocPerfPluginTest, SocPerfPluginTest_API_TEST_032, Function | MediumTest | Level0)
{
    nlohmann::json payload1;
    payload1["uid"] = "-1";
    std::shared_ptr<ResData> invalidData1 = std::make_shared<ResData>(111, 0, payload1);
    bool ret = SocPerfPlugin::GetInstance().UpdateFocusAppType(invalidData1, true);
    EXPECT_FALSE(ret);

    nlohmann::json payload2;
    payload2["uid"] = "10011";
    std::shared_ptr<ResData> validData2 = std::make_shared<ResData>(111, 0, payload2);
    ret = SocPerfPlugin::GetInstance().UpdateFocusAppType(validData2, false);
    EXPECT_TRUE(ret);

    payload2["pid"] = "2025";
    AppKeyMessage appMsg(3, "qq");
    uidToAppMsgMap_[10011] = appMsg;
    ret = SocPerfPlugin::GetInstance().UpdateFocusAppType(validData2, true);
    EXPECT_TRUE(ret);

    nlohmann::json payload3;
    payload3["uid"] = "10012";
    payload3["pid"] = "2025";
    std::shared_ptr<ResData> validData3 = std::make_shared<ResData>(111, 0, payload3);
    ret = SocPerfPlugin::GetInstance().UpdateFocusAppType(validData3, true);
    EXPECT_TRUE(ret);
}

/*
 * @tc.name: SocPerfPluginTest_API_TEST_033
 * @tc.desc: test socperfplugin api
 * @tc.type FUNC
 * @tc.require: issueI78T3V
 */
HWTEST_F(SocPerfPluginTest, SocPerfPluginTest_API_TEST_033, Function | MediumTest | Level0)
{
    focusAppUids_.clear();
    uidToAppMsgMap_.clear();
    bool ret = SocPerfPlugin::GetInstance().IsFocusAppsAllGame();
    EXPECT_FALSE(ret);

    AppKeyMessage appMsg1(2, "qq");
    uidToAppMsgMap_[10010] = appMsg1;
    AppKeyMessage appMsg2(3, "alipy");
    uidToAppMsgMap_[10011] = appMsg2;
    AppKeyMessage appMsg3(2, "game");
    uidToAppMsgMap_[10012] = appMsg2;
    focusAppUids_ = {10010, 10011};
    ret = SocPerfPlugin::GetInstance().IsFocusAppsAllGame();
    EXPECT_FALSE(ret);

    focusAppUids_.insert(10012);
    focusAppUids_.erase(10011);
    ret = SocPerfPlugin::GetInstance().IsFocusAppsAllGame();
    EXPECT_TRUE(ret);
    focusAppUids_.clear();
    uidToAppMsgMap_.clear();
}

/*
 * @tc.name: SocPerfPluginTest_API_TEST_034
 * @tc.desc: test socperfplugin api
 * @tc.type FUNC
 * @tc.require: issueI78T3V
 */
HWTEST_F(SocPerfPluginTest, SocPerfPluginTest_API_TEST_034, Function | MediumTest | Level0)
{
    focusAppUids_.clear();
    uidToAppMsgMap_.clear();
    focusAppUids_ = {10010};
    bool ret = SocPerfPlugin::GetInstance().UpdatesFocusAppsType(2);
    EXPECT_TRUE(ret);

    ret = SocPerfPlugin::GetInstance().UpdatesFocusAppsType(3);
    EXPECT_TRUE(ret);

    focusAppUids_insert(10011);
    isFocusAppsGameType_ = false;
    ret = SocPerfPlugin::GetInstance().UpdatesFocusAppsType(2);
    EXPECT_FALSE(ret);

    ret = SocPerfPlugin::GetInstance().UpdatesFocusAppsType(3);
    EXPECT_FALSE(ret);

    isFocusAppsGameType_ = true;
    ret = SocPerfPlugin::GetInstance().UpdatesFocusAppsType(2);
    EXPECT_TRUE(ret);

    ret = SocPerfPlugin::GetInstance().UpdatesFocusAppsType(3);
    EXPECT_FALSE(ret);

    focusAppUids_.clear();
    uidToAppMsgMap_.clear();
}

/*
 * @tc.name: SocPerfPluginTest_API_TEST_035
 * @tc.desc: test socperfplugin api
 * @tc.type FUNC
 * @tc.require: issueI78T3V
 */
HWTEST_F(SocPerfPluginTest, SocPerfPluginTest_API_TEST_035, Function | MediumTest | Level0)
{
    socperfGameBoostSwitch_ = false;
    bool ret = SocPerfPlugin::GetInstance().HandleGameBoost(nullptr);
    EXPECT_FALSE(ret);

    socperfGameBoostSwitch_ = true;
    ret = SocPerfPlugin::GetInstance().HandleGameBoost(nullptr);
    EXPECT_FALSE(ret);

    std::shared_ptr<ResData> validData1 = std::make_shared<ResData>(ResType::RES_TYPE_APP_GAME_BOOST_EVENT,
        ResType::GameBoostState::BOOST_START, nullptr);
    ret = SocPerfPlugin::GetInstance().HandleGameBoost(validData1);
    EXPECT_TRUE(ret);

    std::shared_ptr<ResData> validData2 = std::make_shared<ResData>(ResType::RES_TYPE_APP_GAME_BOOST_EVENT,
        ResType::GameBoostState::BOOST_END, nullptr);
    ret = SocPerfPlugin::GetInstance().HandleGameBoost(validData2);
    EXPECT_TRUE(ret);
}

/*
 * @tc.name: SocPerfPluginTest_API_TEST_036
 * @tc.desc: test socperfplugin api
 * @tc.type FUNC
 * @tc.require: issueI78T3V
 */
HWTEST_F(SocPerfPluginTest, SocPerfPluginTest_API_TEST_036, Function | MediumTest | Level0)
{
    pidToAppTypeMap_.clear();
    nlohmann::json payload1;
    payload1["clientPid"] = "-1";
    std::shared_ptr<ResData> invalidData1 = std::make_shared<ResData>(-1, -1, payload1);
    bool ret = SocPerfPlugin::GetInstance().IsGameEvent(invalidData1);
    EXPECT_FALSE(ret);

    payload1["clientPid"] = "2025";
    pidToAppTypeMap_[2025] = 3;
    std::shared_ptr<ResData> invalidData2 = std::make_shared<ResData>(-1, -1, payload1);
    ret = SocPerfPlugin::GetInstance().IsGameEvent(invalidData2);
    EXPECT_FALSE(ret);

    pidToAppTypeMap_[2025] = 2;
    std::shared_ptr<ResData> validData = std::make_shared<ResData>(-1, -1, payload1);
    ret = SocPerfPlugin::GetInstance().IsGameEvent(validData);
    EXPECT_TRUE(ret);
}

/*
 * @tc.name: SocPerfPluginTest_API_TEST_037
 * @tc.desc: test socperfplugin api
 * @tc.type FUNC
 * @tc.require: issueI78T3V
 */
HWTEST_F(SocPerfPluginTest, SocPerfPluginTest_API_TEST_037, Function | MediumTest | Level0)
{
    bool ret = SocPerfPlugin::GetInstance().HandleUninstallEvent(nullptr);
    EXPECT_FALSE(ret);

    std::shared_ptr<ResData> invalidData1 = std::make_shared<ResData>(-1, -1, nullptr);
    ret = SocPerfPlugin::GetInstance().HandleUninstallEvent(invalidData1);
    EXPECT_FALSE(ret);
    
    std::shared_ptr<ResData> invalidData2 = std::make_shared<ResData>(ResType::RES_TYPE_APP_INSTALL_UNINSTALL,
        ResType::AppInstallStatus::APP_UNINSTALL, nullptr);
    ret = SocPerfPlugin::GetInstance().HandleUninstallEvent(invalidData2);
    EXPECT_FALSE(ret);

    nlohmann::json payload1;
    std::shared_ptr<ResData> invalidData3 = std::make_shared<ResData>(ResType::RES_TYPE_APP_INSTALL_UNINSTALL,
        ResType::AppInstallStatus::APP_UNINSTALL, payload1);
    ret = SocPerfPlugin::GetInstance().HandleUninstallEvent(invalidData3);
    EXPECT_FALSE(ret);

    payload1["uid"] = "str";
    ret = SocPerfPlugin::GetInstance().HandleUninstallEvent(invalidData3);
    EXPECT_FALSE(ret);

    nlohmann::json payload2;
    payload2["uid"] = -1;
    std::shared_ptr<ResData> invalidData4 = std::make_shared<ResData>(ResType::RES_TYPE_APP_INSTALL_UNINSTALL,
        ResType::AppInstallStatus::APP_UNINSTALL, payload2);
    ret = SocPerfPlugin::GetInstance().HandleUninstallEvent(invalidData4);
    EXPECT_FALSE(ret);

    nlohmann::json payload3;
    payload3["uid"] = 10010;
    std::shared_ptr<ResData> invalidData5 = std::make_shared<ResData>(ResType::RES_TYPE_APP_INSTALL_UNINSTALL,
        ResType::AppInstallStatus::APP_UNINSTALL, payload3);
    ret = SocPerfPlugin::GetInstance().HandleUninstallEvent(invalidData5);
    EXPECT_TRUE(ret);
}

/*
 * @tc.name: SocPerfPluginTest_API_TEST_038
 * @tc.desc: test socperfplugin api
 * @tc.type FUNC
 * @tc.require: issueI78T3V
 */
HWTEST_F(SocPerfPluginTest, SocPerfPluginTest_API_TEST_038, Function | MediumTest | Level0)
{
    std::shared_ptr<ResData> invalidData1 = std::make_shared<ResData>(-1, -1, nullptr);
    bool ret = SocPerfPlugin::GetInstance().GetUidByData(invalidData1) == -1;
    EXPECT_TRUE(ret);

    nlohmann::json payload1;
    std::shared_ptr<ResData> invalidData2 = std::make_shared<ResData>(-1, -1, payload1);
    ret = SocPerfPlugin::GetInstance().GetUidByData(invalidData2) == -1;
    EXPECT_TRUE(ret);

    payload1["uid"] = 10010;
    std::shared_ptr<ResData> invalidData3 = std::make_shared<ResData>(-1, -1, payload1);
    ret = SocPerfPlugin::GetInstance().GetUidByData(invalidData3) == -1;
    EXPECT_TRUE(ret);

    nlohmann::json payload2;
    payload2["uid"] = "10010";
    std::shared_ptr<ResData> invalidData4 = std::make_shared<ResData>(-1, -1, payload2);
    ret = SocPerfPlugin::GetInstance().GetUidByData(invalidData4) == 10010;
    EXPECT_TRUE(ret);
}

/*
 * @tc.name: SocPerfPluginTest_API_TEST_038
 * @tc.desc: test socperfplugin api
 * @tc.type FUNC
 * @tc.require: issueI78T3V
 */
HWTEST_F(SocPerfPluginTest, SocPerfPluginTest_API_TEST_038, Function | MediumTest | Level0)
{
    uidToAppMsgMap_.clear();
    nlohmann::json payload1;
    payload1["uid"] = "-1";
    std::shared_ptr<ResData> invalidData1 = std::make_shared<ResData>(-1, -1, payload1);
    bool ret = SocPerfPlugin::GetInstance().UpdateUidToAppMsgMap(invalidData1);
    EXPECT_FALSE(ret);

    nlohmann::json payload2;
    payload2["uid"] = "10010";
    std::shared_ptr<ResData> validData1 = std::make_shared<ResData>(-1, -1, payload2);
    AppKeyMessage appMsg(2, "qq");
    uidToAppMsgMap_[10010] = appMsg;
    ret = SocPerfPlugin::GetInstance().UpdateUidToAppMsgMap(validData1);
    EXPECT_TRUE(ret);

    nlohmann::json payload3;
    payload3["uid"] = "10011";
    std::shared_ptr<ResData> invalidData2 = std::make_shared<ResData>(-1, -1, payload3);
    ret = SocPerfPlugin::GetInstance().UpdateUidToAppMsgMap(invalidData2);
    EXPECT_FALSE(ret);

    payload3["bundleName"] = "wechat";
    std::shared_ptr<ResData> validData2 = std::make_shared<ResData>(-1, -1, payload3);
    ret = SocPerfPlugin::GetInstance().UpdateUidToAppMsgMap(validData2);
    EXPECT_TRUE(ret);
    uidToAppMsgMap_.clear();
}

/*
 * @tc.name: SocPerfPluginTest_API_TEST_039
 * @tc.desc: test socperfplugin api
 * @tc.type FUNC
 * @tc.require: issueI78T3V
 */
HWTEST_F(SocPerfPluginTest, SocPerfPluginTest_API_TEST_039, Function | MediumTest | Level0)
{
    appNameUseCamera_.clear();
    uidToAppMsgMap_.clear();
    focusAppUids_.clear();
    bool ret = SocPerfPlugin::GetInstance().HandleCameraStateChange(nullptr);
    EXPECT_FALSE(ret);

    appNameUseCamera_.insert("qqlive");
    std::shared_ptr<ResData> validData1 = std::make_shared<ResData>(ResType::RES_TYPE_APP_INSTALL_UNINSTALL,
        ResType::CameraState::CAMERA_DISCONNECT, nullptr);
    ret = SocPerfPlugin::GetInstance().HandleCameraStateChange(validData1);
    EXPECT_TRUE(ret);

    std::shared_ptr<ResData> validData2 = std::make_shared<ResData>(ResType::RES_TYPE_APP_INSTALL_UNINSTALL,
        ResType::CameraState::CAMERA_CONNECT, nullptr);
    ret = SocPerfPlugin::GetInstance().HandleCameraStateChange(validData2);
    EXPECT_TRUE(ret);

    focusAppUids_ = {10010, 10011};
    AppKeyMessage appMsg(2, "qqlive");
    uidToAppMsgMap_[10010] = appMsg;
    std::shared_ptr<ResData> validData2 = std::make_shared<ResData>(ResType::RES_TYPE_APP_INSTALL_UNINSTALL,
        ResType::CameraState::CAMERA_CONNECT, nullptr);
    EXPECT_TRUE(ret);
    appNameUseCamera_.clear();
    uidToAppMsgMap_.clear();
    focusAppUids_.clear();
}

/*
 * @tc.name: SocPerfPluginTest_API_TEST_040
 * @tc.desc: test socperfplugin api
 * @tc.type FUNC
 * @tc.require: issueI78T3V
 */
HWTEST_F(SocPerfPluginTest, SocPerfPluginTest_API_TEST_040, Function | MediumTest | Level0)
{
    appNameUseCamera_.clear();
    uidToAppMsgMap_.clear();
    focusAppUids_.clear();
    bool ret = SocPerfPlugin::GetInstance().IsAllowBoostScene();
    EXPECT_FALSE(ret);

    appNameUseCamera_.insert("qqlive");
    focusAppUids_ = {10010, 10011};
    AppKeyMessage appMsg(2, "qqlive");
    uidToAppMsgMap_[10010] = appMsg;
    ret = SocPerfPlugin::GetInstance().IsAllowBoostScene();
    EXPECT_TRUE(ret);
    appNameUseCamera_.clear();
    uidToAppMsgMap_.clear();
    focusAppUids_.clear();
}

/*
 * @tc.name: SocPerfPluginTest_API_TEST_041
 * @tc.desc: test socperfplugin api
 * @tc.type FUNC
 * @tc.require: issueI78T3V
 */
HWTEST_F(SocPerfPluginTest, SocPerfPluginTest_API_TEST_041, Function | MediumTest | Level0)
{
    bool ret = SocPerfPlugin::GetInstance().HandleProcessStateChange(nullptr);
    EXPECT_TRUE(ret);

    std::shared_ptr<ResData> validData1 = std::make_shared<ResData>(ResType::RES_TYPE_PROCESS_STATE_CHANGE,
        ResType::ProcessStatus::PROCESS_DIED, nullptr);
    ret = SocPerfPlugin::GetInstance().HandleProcessStateChange(validData1);
    EXPECT_TRUE(ret);
}

/*
 * @tc.name: SocPerfPluginTest_API_TEST_042
 * @tc.desc: test socperfplugin api
 * @tc.type FUNC
 * @tc.require: issueI78T3V
 */
HWTEST_F(SocPerfPluginTest, SocPerfPluginTest_API_TEST_042, Function | MediumTest | Level0)
{
    nlohmann::json payload1;
    payload1["pid"] = "-1";
    std::shared_ptr<ResData> validData1 = std::make_shared<ResData>(-1, -1, payload1);
    bool ret = SocPerfPlugin::GetInstance().HandleProcessStateChange(validData1);
    EXPECT_FALSE(ret);

    payload1["pid"] = "2025";
    std::shared_ptr<ResData> validData2 = std::make_shared<ResData>(-1, -1, payload1);
    ret = SocPerfPlugin::GetInstance().HandleProcessStateChange(validData2);
    EXPECT_TRUE(ret);
}

HWTEST_F(SocPerfPluginTest, SocPerfPluginTest_API_TEST_043, Function | MediumTest | Level0)
{
    std::shared_ptr<ResData> invalidData1 = std::make_shared<ResData>(-1, -1, nullptr);
    bool ret = SocPerfPlugin::GetInstance().GetPidByData(invalidData1, "notKey") == -1;
    EXPECT_TRUE(ret);

    nlohmann::json payload1;
    payload1["pid"] = "2025";
    std::shared_ptr<ResData> invalidData2 = std::make_shared<ResData>(-1, -1, payload1);
    ret = SocPerfPlugin::GetInstance().GetPidByData(invalidData1, "notPid") == -1;
    EXPECT_TRUE(ret);

    ret = SocPerfPlugin::GetInstance().GetPidByData(invalidData1, "pid") == 2025;
    EXPECT_TRUE(ret);
}
} // namespace SOCPERF
} // namespace OHOS