/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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
#define private public
#include "application_info.h"
#include "common_event_data.h"
#include "common_event_manager.h"
#include "common_event_support.h"
#include "res_sched_log.h"
#ifdef RESSCHED_COMMUNICATION_NETMANAGER_BASE_ENABLE
#include "net_supplier_info.h"
#endif
#include "want.h"

#include "event_controller.h"
#include "res_type.h"

namespace OHOS {
namespace ResourceSchedule {
using namespace EventFwk;
class EventControllerTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    static void AssertResType(uint32_t &actual, uint32_t &expect)
    {
        EXPECT_TRUE(actual == expect) << "Dispatch resType should be " + std::to_string(expect)
            + ", actually is " + std::to_string(actual);
    }
    static void AssertValue(int64_t &actual, int64_t &expect)
    {
        EXPECT_TRUE(actual == expect) << "Dispatch value should be " + std::to_string(expect)
            + ", actually is " + std::to_string(actual);
    }
    void SetUp() {}
    void TearDown() {}
};

/**
 * @tc.name: connectivityChange_00100
 * @tc.desc: test dispatching ResType::RES_TYPE_WIFI_CONNECT_STATE_CHANGE
 *           when recieve COMMON_EVENT_CONNECTIVITY_CHANGE.
 * @tc.type: FUNC
 * @tc.require: SR000H0H3C AR000HORSU
 */
HWTEST_F(EventControllerTest, connectivityChange_00100, testing::ext::TestSize.Level1)
{
#ifdef RESSCHED_COMMUNICATION_NETMANAGER_BASE_ENABLE
    AAFwk::Want want;
    EventFwk::CommonEventData data;
    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_CONNECTIVITY_CHANGE);
    want.SetParam("NetType", 1);
    data.SetWant(want);
    data.SetCode(NetManagerStandard::NetConnState::NET_CONN_STATE_CONNECTING);

    EventController::GetInstance().OnReceiveEvent(data);
    uint32_t expectResType = ResType::RES_TYPE_WIFI_CONNECT_STATE_CHANGE;
    int64_t expectValue = 2;
    EventControllerTest::AssertResType(EventController::GetInstance().resType_, expectResType);
    EventControllerTest::AssertValue(EventController::GetInstance().value_, expectValue);
#endif
}

/**
 * @tc.name: connectivityChange_00101
 * @tc.desc: test dispatching ResType::RES_TYPE_WIFI_CONNECT_STATE_CHANGE
 *           when receive COMMON_EVENT_CONNECTIVITY_CHANGE and code is NET_CONN_STATE_CONNECTED.
 * @tc.type: FUNC
 * @tc.require: SR000H0H3C AR000HORSU
 */
HWTEST_F(EventControllerTest, connectivityChange_00101, testing::ext::TestSize.Level1)
{
#ifdef RESSCHED_COMMUNICATION_NETMANAGER_BASE_ENABLE
    AAFwk::Want want;
    EventFwk::CommonEventData data;
    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_CONNECTIVITY_CHANGE);
    want.SetParam("NetType", 1);
    data.SetWant(want);
    data.SetCode(NetManagerStandard::NetConnState::NET_CONN_STATE_CONNECTED);

    EventController::GetInstance().OnReceiveEvent(data);
    uint32_t expectResType = ResType::RES_TYPE_WIFI_CONNECT_STATE_CHANGE;
    int64_t expectValue = 3;
    EventControllerTest::AssertResType(EventController::GetInstance().resType_, expectResType);
    EventControllerTest::AssertValue(EventController::GetInstance().value_, expectValue);
#endif
}

/**
 * @tc.name: connectivityChange_00102
 * @tc.desc: test dispatching ResType::RES_TYPE_APP_INSTALL_UNINSTALL when recieve COMMON_EVENT_PACKAGE_ADDED.
 * @tc.type: FUNC
 * @tc.require: SR000H0H3C AR000HORSU
 */
HWTEST_F(EventControllerTest, connectivityChange_00102, testing::ext::TestSize.Level1)
{
    AAFwk::Want want;
    EventFwk::CommonEventData data;
    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_ADDED);
    data.SetWant(want);

    EventController::GetInstance().OnReceiveEvent(data);
    uint32_t expectResType = ResType::RES_TYPE_APP_INSTALL_UNINSTALL;
    int64_t expectValue = 1;
    EventControllerTest::AssertResType(EventController::GetInstance().resType_, expectResType);
    EventControllerTest::AssertValue(EventController::GetInstance().value_, expectValue);
}

/**
 * @tc.name: connectivityChange_00103
 * @tc.desc: test dispatching ResType::RES_TYPE_APP_INSTALL_UNINSTALL when recieve COMMON_EVENT_PACKAGE_REMOVED.
 * @tc.type: FUNC
 * @tc.require: SR000H0H3C AR000HORSU
 */
HWTEST_F(EventControllerTest, connectivityChange_00103, testing::ext::TestSize.Level1)
{
    AAFwk::Want want;
    EventFwk::CommonEventData data;
    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_REMOVED);
    data.SetWant(want);

    EventController::GetInstance().OnReceiveEvent(data);
    uint32_t expectResType = ResType::RES_TYPE_APP_INSTALL_UNINSTALL;
    int64_t expectValue = 0;
    EventControllerTest::AssertResType(EventController::GetInstance().resType_, expectResType);
    EventControllerTest::AssertValue(EventController::GetInstance().value_, expectValue);
}

/**
 * @tc.name: connectivityChange_00104
 * @tc.desc: test dispatching ResType::RES_TYPE_SCREEN_STATUS when recieve COMMON_EVENT_SCREEN_ON.
 * @tc.type: FUNC
 * @tc.require: SR000H0H3C AR000HORSU
 */
HWTEST_F(EventControllerTest, connectivityChange_00104, testing::ext::TestSize.Level1)
{
    AAFwk::Want want;
    EventFwk::CommonEventData data;
    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_SCREEN_ON);
    data.SetWant(want);

    EventController::GetInstance().OnReceiveEvent(data);
    uint32_t expectResType = ResType::RES_TYPE_SCREEN_STATUS;
    int64_t expectValue = 1;
    EventControllerTest::AssertResType(EventController::GetInstance().resType_, expectResType);
    EventControllerTest::AssertValue(EventController::GetInstance().value_, expectValue);
}

/**
 * @tc.name: connectivityChange_00105
 * @tc.desc: test dispatching ResType::RES_TYPE_SCREEN_STATUS when recieve COMMON_EVENT_SCREEN_OFF.
 * @tc.type: FUNC
 * @tc.require: SR000H0H3C AR000HORSU
 */
HWTEST_F(EventControllerTest, connectivityChange_00105, testing::ext::TestSize.Level1)
{
    AAFwk::Want want;
    EventFwk::CommonEventData data;
    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_SCREEN_OFF);
    data.SetWant(want);

    EventController::GetInstance().OnReceiveEvent(data);
    uint32_t expectResType = ResType::RES_TYPE_SCREEN_STATUS;
    int64_t expectValue = 0;
    EventControllerTest::AssertResType(EventController::GetInstance().resType_, expectResType);
    EventControllerTest::AssertValue(EventController::GetInstance().value_, expectValue);
}

/**
 * @tc.name: connectivityChange_00106
 * @tc.desc: test the interface OnAddSystemAbility
 * @tc.type: FUNC
 * @tc.require: issueI8VZVN
 */
HWTEST_F(EventControllerTest, connectivityChange_00106, testing::ext::TestSize.Level1)
{
    EventController::GetInstance().Init();
    SUCCEED();
    int32_t systemAbilityId = 0;
    std::string deviceId;
    EventController::GetInstance().sysAbilityListener_->OnAddSystemAbility(systemAbilityId, deviceId);
    EXPECT_NE(EventController::GetInstance().sysAbilityListener_->subscriber_, nullptr);
}

/**
 * @tc.name: connectivityChange_00107
 * @tc.desc: test the interface
 * @tc.type: FUNC
 * @tc.require: issueI8VZVN
 */
HWTEST_F(EventControllerTest, connectivityChange_00107, testing::ext::TestSize.Level1)
{
    int32_t userId = 0;
    std::string bundleName = "test";
    int32_t result = EventController::GetInstance().GetUid(userId, bundleName);
    EXPECT_EQ(result, -1);
    EventController::GetInstance().Stop();
}

/**
 * @tc.name: connectivityChange_00108
 * @tc.desc: test the interface Init
 * @tc.type: FUNC
 * @tc.require: issueI8VZVN
 */
HWTEST_F(EventControllerTest, connectivityChange_00108, testing::ext::TestSize.Level1)
{
    EventController::GetInstance().Init();
    EXPECT_NE(EventController::GetInstance().sysAbilityListener_, nullptr);
}

/**
 * @tc.name: connectivityChange_00110
 * @tc.desc: test the interface
 * @tc.type: FUNC
 * @tc.require: issueI8VZVN
 */
HWTEST_F(EventControllerTest, connectivityChange_00110, testing::ext::TestSize.Level1)
{
    EventFwk::CommonEventData data;
    nlohmann::json payload;
    EventFwk::Want want = data.GetWant();
    std::string action = EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_CHANGED;
    bool b1 = EventController::GetInstance().HandlePkgCommonEvent(action, want, payload);
    EXPECT_EQ(b1, true);

    std::string action1 = EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_REPLACED;
    bool b2 = EventController::GetInstance().HandlePkgCommonEvent(action1, want, payload);
    EXPECT_EQ(b2, true);

    std::string action2 = EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_FULLY_REMOVED;
    bool b3 = EventController::GetInstance().HandlePkgCommonEvent(action2, want, payload);
    EXPECT_EQ(b3, true);

    std::string action3 = EventFwk::CommonEventSupport::COMMON_EVENT_BUNDLE_REMOVED;
    bool b4 = EventController::GetInstance().HandlePkgCommonEvent(action3, want, payload);
    EXPECT_EQ(b4, true);
}

/**
 * @tc.name: bootCompleted_001
 * @tc.desc: test the boot bootCompleted_001
 * @tc.type: FUNC
 * @tc.require: issuesI9IR2I
 */
HWTEST_F(EventControllerTest, bootCompleted_001, testing::ext::TestSize.Level1)
{
    AAFwk::Want want;
    EventFwk::CommonEventData data;
    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_BOOT_COMPLETED);
    data.SetWant(want);
    EventController::GetInstance().OnReceiveEvent(data);
    EXPECT_NE(want.GetAction(), "");
}

/**
 * @tc.name: userSwitch_001
 * @tc.desc: test the userSwitch_001
 * @tc.type: FUNC
 * @tc.require: issuesICOFZ6
 */
HWTEST_F(EventControllerTest, userSwitch_001, testing::ext::TestSize.Level1)
{
    AAFwk::Want want;
    EventFwk::CommonEventData data;
    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_USER_SWITCHED);
    data.SetWant(want);
    EventController::GetInstance().OnReceiveEvent(data);
    EXPECT_NE(want.GetAction(), "");
}

/**
 * @tc.name: userRemove_001
 * @tc.desc: test the userRemove_001
 * @tc.type: FUNC
 * @tc.require: issuesICOFZ6
 */
HWTEST_F(EventControllerTest, userRemove_001, testing::ext::TestSize.Level1)
{
    AAFwk::Want want;
    EventFwk::CommonEventData data;
    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_USER_REMOVED);
    data.SetWant(want);
    EventController::GetInstance().OnReceiveEvent(data);
    EXPECT_NE(want.GetAction(), "");
}

/**
 * @tc.name: unlockScreen_001
 * @tc.desc: test the unlockScreen_001
 * @tc.type: FUNC
 * @tc.require: issuesICOFZ6
 */
HWTEST_F(EventControllerTest, unlockScreen_001, testing::ext::TestSize.Level1)
{
    AAFwk::Want want;
    EventFwk::CommonEventData data;
    want.SetAction("common.event.UNLOCK_SCREEN");
    data.SetWant(want);
    EventController::GetInstance().OnReceiveEvent(data);
    EXPECT_NE(want.GetAction(), "");
}

/**
 * @tc.name: lockScreen_001
 * @tc.desc: test the lockScreen_001
 * @tc.type: FUNC
 * @tc.require: issuesICOFZ6
 */
HWTEST_F(EventControllerTest, lockScreen_001, testing::ext::TestSize.Level1)
{
    AAFwk::Want want;
    EventFwk::CommonEventData data;
    want.SetAction("common.event.LOCK_SCREEN");
    data.SetWant(want);
    EventController::GetInstance().OnReceiveEvent(data);
    EXPECT_NE(want.GetAction(), "");
}

/**
 * @tc.name: callStateChange_001
 * @tc.desc: test the callStateChange_001
 * @tc.type: FUNC
 * @tc.require: issuesICOFZ6
 */
HWTEST_F(EventControllerTest, callStateChange_001, testing::ext::TestSize.Level1)
{
    AAFwk::Want want;
    EventFwk::CommonEventData data;
    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_CALL_STATE_CHANGED);
    data.SetWant(want);
    EventController::GetInstance().OnReceiveEvent(data);
    EXPECT_NE(want.GetAction(), "");
}

/**
 * @tc.name: timezoneChange_001
 * @tc.desc: test the timezoneChange_001
 * @tc.type: FUNC
 * @tc.require: issuesICOFZ6
 */
HWTEST_F(EventControllerTest, timezoneChange_001, testing::ext::TestSize.Level1)
{
    AAFwk::Want want;
    EventFwk::CommonEventData data;
    want.SetAction(CommonEventSupport::COMMON_EVENT_TIMEZONE_CHANGED);
    data.SetWant(want);
    EventController::GetInstance().OnReceiveEvent(data);
    EXPECT_NE(want.GetAction(), "");
}

/**
 * @tc.name: timeChange_001
 * @tc.desc: test the timeChange_001
 * @tc.type: FUNC
 * @tc.require: issuesICOFZ6
 */
HWTEST_F(EventControllerTest, timeChange_001, testing::ext::TestSize.Level1)
{
    AAFwk::Want want;
    EventFwk::CommonEventData data;
    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_TIME_CHANGED);
    data.SetWant(want);
    EventController::GetInstance().OnReceiveEvent(data);
    EXPECT_NE(want.GetAction(), "");
}

/**
 * @tc.name: nitzTimeChange_001
 * @tc.desc: test the nitzTimeChange_001
 * @tc.type: FUNC
 * @tc.require: issuesICOFZ6
 */
HWTEST_F(EventControllerTest, nitzTimeChange_001, testing::ext::TestSize.Level1)
{
    AAFwk::Want want;
    EventFwk::CommonEventData data;
    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_NITZ_TIME_CHANGED);
    data.SetWant(want);
    EventController::GetInstance().OnReceiveEvent(data);
    EXPECT_NE(want.GetAction(), "");
}


/**
 * @tc.name: nitzTimezoneChange_001
 * @tc.desc: test the nitzTimezoneChange_001
 * @tc.type: FUNC
 * @tc.require: issuesICOFZ6
 */
HWTEST_F(EventControllerTest, nitzTimezoneChange_001, testing::ext::TestSize.Level1)
{
    AAFwk::Want want;
    EventFwk::CommonEventData data;
    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_NITZ_TIMEZONE_CHANGED);
    data.SetWant(want);
    EventController::GetInstance().OnReceiveEvent(data);
    EXPECT_NE(want.GetAction(), "");
}

/**
 * @tc.name: charging_001
 * @tc.desc: test the charging_001
 * @tc.type: FUNC
 * @tc.require: issuesICOFZ6
 */
HWTEST_F(EventControllerTest, charging_001, testing::ext::TestSize.Level1)
{
    AAFwk::Want want;
    EventFwk::CommonEventData data;
    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_CHARGING);
    data.SetWant(want);
    EventController::GetInstance().OnReceiveEvent(data);
    EXPECT_NE(want.GetAction(), "");
}

/**
 * @tc.name: discharging_001
 * @tc.desc: test the discharging_001
 * @tc.type: FUNC
 * @tc.require: issuesICOFZ6
 */
HWTEST_F(EventControllerTest, discharging_001, testing::ext::TestSize.Level1)
{
    AAFwk::Want want;
    EventFwk::CommonEventData data;
    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_DISCHARGING);
    data.SetWant(want);
    EventController::GetInstance().OnReceiveEvent(data);
    EXPECT_NE(want.GetAction(), "");
}

/**
 * @tc.name: usbDeviceAttached_001
 * @tc.desc: test the usbDeviceAttached_001
 * @tc.type: FUNC
 * @tc.require: issuesICOFZ6
 */
HWTEST_F(EventControllerTest, usbDeviceAttached_001, testing::ext::TestSize.Level1)
{
    AAFwk::Want want;
    EventFwk::CommonEventData data;
    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_USB_DEVICE_ATTACHED);
    data.SetWant(want);
    EventController::GetInstance().OnReceiveEvent(data);
    EXPECT_NE(want.GetAction(), "");
}

/**
 * @tc.name: usbDeviceDetached_001
 * @tc.desc: test the usbDeviceDetached_001
 * @tc.type: FUNC
 * @tc.require: issuesICOFZ6
 */
HWTEST_F(EventControllerTest, usbDeviceDetached_001, testing::ext::TestSize.Level1)
{
    AAFwk::Want want;
    EventFwk::CommonEventData data;
    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_USB_DEVICE_DETACHED);
    data.SetWant(want);
    EventController::GetInstance().OnReceiveEvent(data);
    EXPECT_NE(want.GetAction(), "");
}

/**
 * @tc.name: wifiP2PStateChange_001
 * @tc.desc: test the wifiP2PStateChange_001
 * @tc.type: FUNC
 * @tc.require: issuesICOFZ6
 */
HWTEST_F(EventControllerTest, wifiP2PStateChange_001, testing::ext::TestSize.Level1)
{
    AAFwk::Want want;
    EventFwk::CommonEventData data;
    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_WIFI_P2P_STATE_CHANGED);
    data.SetWant(want);
    EventController::GetInstance().OnReceiveEvent(data);
    EXPECT_NE(want.GetAction(), "");
}

/**
 * @tc.name: powerSaveModeChange_001
 * @tc.desc: test the powerSaveModeChange_001
 * @tc.type: FUNC
 * @tc.require: issuesICOFZ6
 */
HWTEST_F(EventControllerTest, powerSaveModeChange_001, testing::ext::TestSize.Level1)
{
    AAFwk::Want want;
    EventFwk::CommonEventData data;
    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_POWER_SAVE_MODE_CHANGED);
    data.SetWant(want);
    EventController::GetInstance().OnReceiveEvent(data);
    EXPECT_NE(want.GetAction(), "");
}

/**
 * @tc.name: wifiPowerState_001
 * @tc.desc: test the wifiPowerState_001
 * @tc.type: FUNC
 * @tc.require: issuesICOFZ6
 */
HWTEST_F(EventControllerTest, wifiPowerState_001, testing::ext::TestSize.Level1)
{
    AAFwk::Want want;
    EventFwk::CommonEventData data;
    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_WIFI_POWER_STATE);
    data.SetWant(want);
    EventController::GetInstance().OnReceiveEvent(data);
    EXPECT_NE(want.GetAction(), "");
}

/**
 * @tc.name: powerConnected_001
 * @tc.desc: test the powerConnected_001
 * @tc.type: FUNC
 * @tc.require: issuesICOFZ6
 */
HWTEST_F(EventControllerTest, powerConnected_001, testing::ext::TestSize.Level1)
{
    AAFwk::Want want;
    EventFwk::CommonEventData data;
    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_POWER_CONNECTED);
    data.SetWant(want);
    EventController::GetInstance().OnReceiveEvent(data);
    EXPECT_NE(want.GetAction(), "");
}

/**
 * @tc.name: powerDisconnected_001
 * @tc.desc: test the powerDisconnected_001
 * @tc.type: FUNC
 * @tc.require: issuesICOFZ6
 */
HWTEST_F(EventControllerTest, powerDisconnected_001, testing::ext::TestSize.Level1)
{
    AAFwk::Want want;
    EventFwk::CommonEventData data;
    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_POWER_DISCONNECTED);
    data.SetWant(want);
    EventController::GetInstance().OnReceiveEvent(data);
    EXPECT_NE(want.GetAction(), "");
}

/**
 * @tc.name: cameraStatus_001
 * @tc.desc: test the cameraStatus_001
 * @tc.type: FUNC
 * @tc.require: issuesICOFZ6
 */
HWTEST_F(EventControllerTest, cameraStatus_001, testing::ext::TestSize.Level1)
{
    AAFwk::Want want;
    EventFwk::CommonEventData data;
    want.SetAction("usual.event.CAMERA_STATUS");
    data.SetWant(want);
    EventController::GetInstance().OnReceiveEvent(data);
    EXPECT_NE(want.GetAction(), "");
}

/**
 * @tc.name: configUpdate_001
 * @tc.desc: test the configUpdate_001
 * @tc.type: FUNC
 * @tc.require: issuesICOFZ6
 */
HWTEST_F(EventControllerTest, configUpdate_001, testing::ext::TestSize.Level1)
{
    AAFwk::Want want;
    EventFwk::CommonEventData data;
    want.SetAction("usual.event.DUA_SA_CFG_UPDATED");
    data.SetWant(want);
    EventController::GetInstance().OnReceiveEvent(data);
    EXPECT_NE(want.GetAction(), "");
}

/**
 * @tc.name: gameStatusChange_001
 * @tc.desc: test the gameStatusChange_001
 * @tc.type: FUNC
 * @tc.require: issuesICOFZ6
 */
HWTEST_F(EventControllerTest, gameStatusChange_001, testing::ext::TestSize.Level1)
{
    AAFwk::Want want;
    EventFwk::CommonEventData data;
    want.SetAction("usual.event.gameservice.GAME_STATUS_CHANGE_UNI");
    data.SetWant(want);
    EventController::GetInstance().OnReceiveEvent(data);
    EXPECT_NE(want.GetAction(), "");
}

/**
 * @tc.name: userSleepStateChange_001
 * @tc.desc: test the userSleepStateChange_001
 * @tc.type: FUNC
 * @tc.require: issuesICOFZ6
 */
HWTEST_F(EventControllerTest, userSleepStateChange_001, testing::ext::TestSize.Level1)
{
    AAFwk::Want want;
    EventFwk::CommonEventData data;
    want.SetAction("common.event.USER_NOT_CARE_CHARGE_SLEEP");
    data.SetWant(want);
    EventController::GetInstance().OnReceiveEvent(data);
    EXPECT_NE(want.GetAction(), "");
}

/**
 * @tc.name: batteryChange_001
 * @tc.desc: test the batteryChange_001
 * @tc.type: FUNC
 * @tc.require: issuesICOFZ6
 */
HWTEST_F(EventControllerTest, batteryChange_001, testing::ext::TestSize.Level1)
{
    AAFwk::Want want;
    EventFwk::CommonEventData data;
    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_BATTERY_CHANGED);
    data.SetWant(want);
    EventController::GetInstance().OnReceiveEvent(data);
    EXPECT_NE(want.GetAction(), "");
}

/**
 * @tc.name: packageInstallStarted_001
 * @tc.desc: test the packageInstallStarted_001
 * @tc.type: FUNC
 * @tc.require: issuesICOFZ6
 */
HWTEST_F(EventControllerTest, packageInstallStarted_001, testing::ext::TestSize.Level1)
{
    AAFwk::Want want;
    EventFwk::CommonEventData data;
    want.SetAction(CommonEventSupport::COMMON_EVENT_PACKAGE_INSTALLATION_STARTED);
    data.SetWant(want);
    EventController::GetInstance().OnReceiveEvent(data);
    EXPECT_NE(want.GetAction(), "");
}
}
}