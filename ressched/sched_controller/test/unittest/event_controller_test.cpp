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

#include "application_info.h"
#include "common_event_data.h"
#include "common_event_manager.h"
#include "common_event_support.h"
#include "res_sched_log.h"
#include "net_supplier_info.h"
#include "want.h"

#include "event_controller.h"
#include "ressched_utils.h"
#include "res_type.h"

namespace OHOS {
namespace ResourceSchedule {
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
}
}