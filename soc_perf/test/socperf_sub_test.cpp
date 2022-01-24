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

#define private public
#define protected public
#undef private
#undef protected

#include <gtest/gtest.h>
#include "socperf_client.h"
#include "socperf.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"

using namespace testing::ext;

namespace OHOS {
namespace SOCPERF {
class SocPerfSubTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void SocPerfSubTest::SetUpTestCase(void)
{
}

void SocPerfSubTest::TearDownTestCase(void)
{
}

void SocPerfSubTest::SetUp(void)
{
}

void SocPerfSubTest::TearDown(void)
{
}

/*
 * @tc.number: SocPerfSubTest_ActionType_001
 * @tc.name: Get ActionType
 * @tc.desc:
 */
HWTEST_F(SocPerfSubTest, SocPerfSubTest_ActionType_001, Function | MediumTest | Level0)
{
    EXPECT_EQ(OHOS::SOCPERF::ActionType::ACTION_TYPE_PERF, 0);
}

/*
 * @tc.number: SocPerfSubTest_ActionType_002
 * @tc.name: Get ActionType
 * @tc.desc:
 */
HWTEST_F(SocPerfSubTest, SocPerfSubTest_ActionType_002, Function | MediumTest | Level0)
{
    EXPECT_EQ(OHOS::SOCPERF::ActionType::ACTION_TYPE_POWER, 1);
}

/*
 * @tc.number: SocPerfSubTest_ActionType_003
 * @tc.name: Get ActionType
 * @tc.desc:
 */
HWTEST_F(SocPerfSubTest, SocPerfSubTest_ActionType_003, Function | MediumTest | Level0)
{
    EXPECT_EQ(OHOS::SOCPERF::ActionType::ACTION_TYPE_THERMAL, 2);
}

/*
 * @tc.number: SocPerfSubTest_ActionType_004
 * @tc.name: Get ActionType
 * @tc.desc:
 */
HWTEST_F(SocPerfSubTest, SocPerfSubTest_ActionType_004, Function | MediumTest | Level0)
{
    EXPECT_EQ(OHOS::SOCPERF::ActionType::ACTION_TYPE_MAX, 3);
}

/*
 * @tc.number: SocPerfSubTest_EventType_001
 * @tc.name: Get EventType
 * @tc.desc:
 */
HWTEST_F(SocPerfSubTest, SocPerfSubTest_EventType_001, Function | MediumTest | Level0)
{
    EXPECT_EQ(OHOS::SOCPERF::EventType::EVENT_OFF, 0);
}

/*
 * @tc.number: SocPerfSubTest_EventType_002
 * @tc.name: Get EventType
 * @tc.desc:
 */
HWTEST_F(SocPerfSubTest, SocPerfSubTest_EventType_002, Function | MediumTest | Level0)
{
    EXPECT_EQ(OHOS::SOCPERF::EventType::EVENT_ON, 1);
}

/*
 * @tc.number: SocPerfSubTest_EventType_003
 * @tc.name: Get EventType
 * @tc.desc:
 */
HWTEST_F(SocPerfSubTest, SocPerfSubTest_EventType_003, Function | MediumTest | Level0)
{
    EXPECT_EQ(OHOS::SOCPERF::EventType::EVENT_INVALID, -1);
}

/*
 * @tc.number: SocPerfSubTest_InnerEventId_001
 * @tc.name: Get InnerEventId
 * @tc.desc:
 */
HWTEST_F(SocPerfSubTest, SocPerfSubTest_InnerEventId_001, Function | MediumTest | Level0)
{
    EXPECT_EQ(INNER_EVENT_ID_INIT_RES_NODE_INFO, 0);
}

/*
 * @tc.number: SocPerfSubTest_InnerEventId_002
 * @tc.name: Get InnerEventId
 * @tc.desc:
 */
HWTEST_F(SocPerfSubTest, SocPerfSubTest_InnerEventId_002, Function | MediumTest | Level0)
{
    EXPECT_EQ(INNER_EVENT_ID_INIT_GOV_RES_NODE_INFO, 1);
}

/*
 * @tc.number: SocPerfSubTest_InnerEventId_003
 * @tc.name: Get InnerEventId
 * @tc.desc:
 */
HWTEST_F(SocPerfSubTest, SocPerfSubTest_InnerEventId_003, Function | MediumTest | Level0)
{
    EXPECT_EQ(INNER_EVENT_ID_DO_FREQ_ACTION, 2);
}

/*
 * @tc.number: SocPerfSubTest_InnerEventId_004
 * @tc.name: Get InnerEventId
 * @tc.desc:
 */
HWTEST_F(SocPerfSubTest, SocPerfSubTest_InnerEventId_004, Function | MediumTest | Level0)
{
    EXPECT_EQ(INNER_EVENT_ID_DO_FREQ_ACTION_DELAYED, 3);
}

/*
 * @tc.number: SocPerfSubTest_InnerEventId_005
 * @tc.name: Get InnerEventId
 * @tc.desc:
 */
HWTEST_F(SocPerfSubTest, SocPerfSubTest_InnerEventId_005, Function | MediumTest | Level0)
{
    EXPECT_EQ(INNER_EVENT_ID_POWER_LIMIT_BOOST_FREQ, 4);
}

/*
 * @tc.number: SocPerfSubTest_InnerEventId_006
 * @tc.name: Get InnerEventId
 * @tc.desc:
 */
HWTEST_F(SocPerfSubTest, SocPerfSubTest_InnerEventId_006, Function | MediumTest | Level0)
{
    EXPECT_EQ(INNER_EVENT_ID_THERMAL_LIMIT_BOOST_FREQ, 5);
}

/*
 * @tc.number: SocPerfSubTest_GetService_001
 * @tc.name: Get Service
 * @tc.desc:
 */
HWTEST_F(SocPerfSubTest, SocPerfSubTest_GetService_001, Function | MediumTest | Level0)
{
    sptr<ISystemAbilityManager> samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    EXPECT_NE(samgr, nullptr);

    sptr<IRemoteObject> object = samgr->GetSystemAbility(SOC_PERF_SERVICE_SA_ID);
    EXPECT_NE(object, nullptr);
}

/*
 * @tc.number: SocPerfSubTest_PerfRequest_001
 * @tc.name: PerfRequest
 * @tc.desc:
 */
HWTEST_F(SocPerfSubTest, SocPerfSubTest_PerfRequest_001, Function | MediumTest | Level0)
{
    OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequest(10000, "");
}

/*
 * @tc.number: SocPerfSubTest_PerfRequest_002
 * @tc.name: PerfRequest
 * @tc.desc:
 */
HWTEST_F(SocPerfSubTest, SocPerfSubTest_PerfRequest_002, Function | MediumTest | Level0)
{
    OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(10000, true, "");
}

/*
 * @tc.number: SocPerfSubTest_PerfRequest_003
 * @tc.name: PerfRequest
 * @tc.desc:
 */
HWTEST_F(SocPerfSubTest, SocPerfSubTest_PerfRequest_003, Function | MediumTest | Level0)
{
    OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(10000, false, "");
}

/*
 * @tc.number: SocPerfSubTest_PowerRequest_001
 * @tc.name: PowerRequest
 * @tc.desc:
 */
HWTEST_F(SocPerfSubTest, SocPerfSubTest_PowerRequest_001, Function | MediumTest | Level0)
{
    OHOS::SOCPERF::SocPerfClient::GetInstance().PowerRequest(20000, "");
}

/*
 * @tc.number: SocPerfSubTest_PowerRequest_002
 * @tc.name: PowerRequest
 * @tc.desc:
 */
HWTEST_F(SocPerfSubTest, SocPerfSubTest_PowerRequest_002, Function | MediumTest | Level0)
{
    OHOS::SOCPERF::SocPerfClient::GetInstance().PowerRequestEx(20000, true, "");
}

/*
 * @tc.number: SocPerfSubTest_PowerRequest_003
 * @tc.name: PowerRequest
 * @tc.desc:
 */
HWTEST_F(SocPerfSubTest, SocPerfSubTest_PowerRequest_003, Function | MediumTest | Level0)
{
    OHOS::SOCPERF::SocPerfClient::GetInstance().PowerRequestEx(20000, false, "");
}

/*
 * @tc.number: SocPerfSubTest_ThermalRequest_001
 * @tc.name: ThermalRequest
 * @tc.desc:
 */
HWTEST_F(SocPerfSubTest, SocPerfSubTest_ThermalRequest_001, Function | MediumTest | Level0)
{
    OHOS::SOCPERF::SocPerfClient::GetInstance().ThermalRequest(30000, "");
}

/*
 * @tc.number: SocPerfSubTest_ThermalRequest_002
 * @tc.name: ThermalRequest
 * @tc.desc:
 */
HWTEST_F(SocPerfSubTest, SocPerfSubTest_ThermalRequest_002, Function | MediumTest | Level0)
{
    OHOS::SOCPERF::SocPerfClient::GetInstance().ThermalRequestEx(30000, true, "");
}

/*
 * @tc.number: SocPerfSubTest_ThermalRequest_003
 * @tc.name: ThermalRequest
 * @tc.desc:
 */
HWTEST_F(SocPerfSubTest, SocPerfSubTest_ThermalRequest_003, Function | MediumTest | Level0)
{
    OHOS::SOCPERF::SocPerfClient::GetInstance().ThermalRequestEx(30000, false, "");
}

/*
 * @tc.number: SocPerfSubTest_PowerLimitBoost_001
 * @tc.name: PowerLimitBoost
 * @tc.desc:
 */
HWTEST_F(SocPerfSubTest, SocPerfSubTest_PowerLimitBoost_001, Function | MediumTest | Level0)
{
    OHOS::SOCPERF::SocPerfClient::GetInstance().PowerLimitBoost(true, "");
}

/*
 * @tc.number: SocPerfSubTest_PowerLimitBoost_002
 * @tc.name: PowerLimitBoost
 * @tc.desc:
 */
HWTEST_F(SocPerfSubTest, SocPerfSubTest_PowerLimitBoost_002, Function | MediumTest | Level0)
{
    OHOS::SOCPERF::SocPerfClient::GetInstance().PowerLimitBoost(false, "");
}

/*
 * @tc.number: SocPerfSubTest_ThermalLimitBoost_001
 * @tc.name: ThermalLimitBoost
 * @tc.desc:
 */
HWTEST_F(SocPerfSubTest, SocPerfSubTest_ThermalLimitBoost_001, Function | MediumTest | Level0)
{
    OHOS::SOCPERF::SocPerfClient::GetInstance().ThermalLimitBoost(true, "");
}

/*
 * @tc.number: SocPerfSubTest_ThermalLimitBoost_002
 * @tc.name: ThermalLimitBoost
 * @tc.desc:
 */
HWTEST_F(SocPerfSubTest, SocPerfSubTest_ThermalLimitBoost_002, Function | MediumTest | Level0)
{
    OHOS::SOCPERF::SocPerfClient::GetInstance().ThermalLimitBoost(false, "");
}
} // namespace SOCPERF
} // namespace OHOS
