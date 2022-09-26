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
 * @tc.name: SocPerfSubTest_ActionType_001
 * @tc.desc: action type perf
 * @tc.type FUNC
 * @tc.require SR000GGTLG AR000GH38A
 */
HWTEST_F(SocPerfSubTest, SocPerfSubTest_ActionType_001, Function | MediumTest | Level0)
{
    EXPECT_EQ(OHOS::SOCPERF::ActionType::ACTION_TYPE_PERF, 0);
}

/*
 * @tc.name: SocPerfSubTest_ActionType_002
 * @tc.desc: action type power
 * @tc.type FUNC
 * @tc.require SR000GGTLG AR000GH38A
 */
HWTEST_F(SocPerfSubTest, SocPerfSubTest_ActionType_002, Function | MediumTest | Level0)
{
    EXPECT_EQ(OHOS::SOCPERF::ActionType::ACTION_TYPE_POWER, 1);
}

/*
 * @tc.name: SocPerfSubTest_ActionType_003
 * @tc.desc: action type THERMAL
 * @tc.type FUNC
 * @tc.require SR000GGTLG AR000GH38A
 */
HWTEST_F(SocPerfSubTest, SocPerfSubTest_ActionType_003, Function | MediumTest | Level0)
{
    EXPECT_EQ(OHOS::SOCPERF::ActionType::ACTION_TYPE_THERMAL, 2);
}

/*
 * @tc.name: SocPerfSubTest_ActionType_004
 * @tc.desc: action type max
 * @tc.type FUNC
 * @tc.require SR000GGTLG AR000GH38A
 */
HWTEST_F(SocPerfSubTest, SocPerfSubTest_ActionType_004, Function | MediumTest | Level0)
{
    EXPECT_EQ(OHOS::SOCPERF::ActionType::ACTION_TYPE_MAX, 3);
}

/*
 * @tc.name: SocPerfSubTest_EventType_001
 * @tc.desc: event type off
 * @tc.type FUNC
 * @tc.require SR000GGTLG AR000GH38A
 */
HWTEST_F(SocPerfSubTest, SocPerfSubTest_EventType_001, Function | MediumTest | Level0)
{
    EXPECT_EQ(OHOS::SOCPERF::EventType::EVENT_OFF, 0);
}

/*
 * @tc.name: SocPerfSubTest_EventType_002
 * @tc.desc: event type on
 * @tc.type FUNC
 * @tc.require SR000GGTLG AR000GH38A
 */
HWTEST_F(SocPerfSubTest, SocPerfSubTest_EventType_002, Function | MediumTest | Level0)
{
    EXPECT_EQ(OHOS::SOCPERF::EventType::EVENT_ON, 1);
}

/*
 * @tc.name: SocPerfSubTest_EventType_003
 * @tc.desc: event type invalid
 * @tc.type FUNC
 * @tc.require SR000GGTLG AR000GH38A
 */
HWTEST_F(SocPerfSubTest, SocPerfSubTest_EventType_003, Function | MediumTest | Level0)
{
    EXPECT_EQ(OHOS::SOCPERF::EventType::EVENT_INVALID, -1);
}

/*
 * @tc.name: SocPerfSubTest_InnerEventId_001
 * @tc.desc: init res node info
 * @tc.type FUNC
 * @tc.require SR000GGTLG AR000GH37V
 */
HWTEST_F(SocPerfSubTest, SocPerfSubTest_InnerEventId_001, Function | MediumTest | Level0)
{
    EXPECT_EQ(INNER_EVENT_ID_INIT_RES_NODE_INFO, 0);
}

/*
 * @tc.name: SocPerfSubTest_InnerEventId_002
 * @tc.desc: init gov res node info
 * @tc.type FUNC
 * @tc.require SR000GGTLG AR000GH37V
 */
HWTEST_F(SocPerfSubTest, SocPerfSubTest_InnerEventId_002, Function | MediumTest | Level0)
{
    EXPECT_EQ(INNER_EVENT_ID_INIT_GOV_RES_NODE_INFO, 1);
}

/*
 * @tc.name: SocPerfSubTest_InnerEventId_003
 * @tc.desc: do freq action
 * @tc.type FUNC
 * @tc.require SR000GGTLG AR000GH380
 */
HWTEST_F(SocPerfSubTest, SocPerfSubTest_InnerEventId_003, Function | MediumTest | Level0)
{
    EXPECT_EQ(INNER_EVENT_ID_DO_FREQ_ACTION, 2);
}

/*
 * @tc.name: SocPerfSubTest_InnerEventId_004
 * @tc.desc: do freq action delayed
 * @tc.type FUNC
 * @tc.require SR000GGTLG AR000GH380
 */
HWTEST_F(SocPerfSubTest, SocPerfSubTest_InnerEventId_004, Function | MediumTest | Level0)
{
    EXPECT_EQ(INNER_EVENT_ID_DO_FREQ_ACTION_DELAYED, 3);
}

/*
 * @tc.name: SocPerfSubTest_InnerEventId_005
 * @tc.desc: power limit boost freq
 * @tc.type FUNC
 * @tc.require SR000GGTLG AR000GH380
 */
HWTEST_F(SocPerfSubTest, SocPerfSubTest_InnerEventId_005, Function | MediumTest | Level0)
{
    EXPECT_EQ(INNER_EVENT_ID_POWER_LIMIT_BOOST_FREQ, 4);
}

/*
 * @tc.name: SocPerfSubTest_InnerEventId_006
 * @tc.desc: thermal limit boost freq
 * @tc.type FUNC
 * @tc.require SR000GGTLG AR000GH380
 */
HWTEST_F(SocPerfSubTest, SocPerfSubTest_InnerEventId_006, Function | MediumTest | Level0)
{
    EXPECT_EQ(INNER_EVENT_ID_THERMAL_LIMIT_BOOST_FREQ, 5);
}

/*
 * @tc.name: SocPerfSubTest_GetService_001
 * @tc.desc: get socperf service
 * @tc.type FUNC
 * @tc.require SR000GGTLG AR000GH384 AR000GH385
 */
HWTEST_F(SocPerfSubTest, SocPerfSubTest_GetService_001, Function | MediumTest | Level0)
{
    sptr<ISystemAbilityManager> samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    EXPECT_NE(samgr, nullptr);

    sptr<IRemoteObject> object = samgr->GetSystemAbility(SOC_PERF_SERVICE_SA_ID);
    EXPECT_NE(object, nullptr);
}

/*
 * @tc.name: SocPerfSubTest_GetService_002
 * @tc.desc: get socperf service
 * @tc.type FUNC
 * @tc.require SR000GGTLG AR000GH38B AR000GH38C AR000GH38D AR000GH38E AR000GH38F AR000GH386 AR000GH387 AR000GH388
 */
HWTEST_F(SocPerfSubTest, SocPerfSubTest_GetService_002, Function | MediumTest | Level0)
{
    sptr<ISystemAbilityManager> samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    EXPECT_NE(samgr, nullptr);

    sptr<IRemoteObject> object = samgr->GetSystemAbility(RES_SCHED_SYS_ABILITY_ID);
    EXPECT_NE(object, nullptr);
}

/*
 * @tc.name: SocPerfSubTest_PerfRequest_001
 * @tc.desc: PerfRequest
 * @tc.type FUNC
 * @tc.require SR000GGTLG AR000GH381 AR000GH382 AR000GH389 AR000GH37U
 */
HWTEST_F(SocPerfSubTest, SocPerfSubTest_PerfRequest_001, Function | MediumTest | Level0)
{
    OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequest(10000, "");
}

/*
 * @tc.name: SocPerfSubTest_PerfRequest_002
 * @tc.desc: PerfRequestEx ON
 * @tc.type FUNC
 * @tc.require SR000GGTLG AR000GH381 AR000GH382 AR000GH389 AR000GH37U
 */
HWTEST_F(SocPerfSubTest, SocPerfSubTest_PerfRequest_002, Function | MediumTest | Level0)
{
    OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(10000, true, "");
}

/*
 * @tc.name: SocPerfSubTest_PerfRequest_003
 * @tc.desc: PerfRequestEx OFF
 * @tc.type FUNC
 * @tc.require SR000GGTLG AR000GH381 AR000GH382 AR000GH389 AR000GH37U
 */
HWTEST_F(SocPerfSubTest, SocPerfSubTest_PerfRequest_003, Function | MediumTest | Level0)
{
    OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(10000, false, "");
}

/*
 * @tc.name: SocPerfSubTest_PowerLimitBoost_001
 * @tc.desc: PowerLimitBoost ON
 * @tc.type FUNC
 * @tc.require SR000GGTLG AR000GH381 AR000GH382 AR000GH389 AR000GH37U
 */
HWTEST_F(SocPerfSubTest, SocPerfSubTest_PowerLimitBoost_001, Function | MediumTest | Level0)
{
    OHOS::SOCPERF::SocPerfClient::GetInstance().PowerLimitBoost(true, "");
}

/*
 * @tc.name: SocPerfSubTest_PowerLimitBoost_001
 * @tc.desc: PowerLimitBoost OFF
 * @tc.type FUNC
 * @tc.require SR000GGTLG AR000GH381 AR000GH382 AR000GH389 AR000GH37U
 */
HWTEST_F(SocPerfSubTest, SocPerfSubTest_PowerLimitBoost_002, Function | MediumTest | Level0)
{
    OHOS::SOCPERF::SocPerfClient::GetInstance().PowerLimitBoost(false, "");
}

/*
 * @tc.name: SocPerfSubTest_ThermalLimitBoost_001
 * @tc.desc: ThermalLimitBoost ON
 * @tc.type FUNC
 * @tc.require SR000GGTLG AR000GH381 AR000GH382 AR000GH389 AR000GH37U
 */
HWTEST_F(SocPerfSubTest, SocPerfSubTest_ThermalLimitBoost_001, Function | MediumTest | Level0)
{
    OHOS::SOCPERF::SocPerfClient::GetInstance().ThermalLimitBoost(true, "");
}

/*
 * @tc.name: SocPerfSubTest_ThermalLimitBoost_002
 * @tc.desc: ThermalLimitBoost OFF
 * @tc.type FUNC
 * @tc.require SR000GGTLG AR000GH381 AR000GH382 AR000GH389 AR000GH37U
 */
HWTEST_F(SocPerfSubTest, SocPerfSubTest_ThermalLimitBoost_002, Function | MediumTest | Level0)
{
    OHOS::SOCPERF::SocPerfClient::GetInstance().ThermalLimitBoost(false, "");
}

/*
 * @tc.name: SocPerfSubTest_LimitRequest_001
 * @tc.desc: LimitRequest thermal
 * @tc.type FUNC
 */
HWTEST_F(SocPerfSubTest, SocPerfSubTest_LimitRequest_001, Function | MediumTest | Level0)
{
    int32_t clientId_power = ACTION_TYPE_POWER;
    std::vector<int32_t> tags;
    tags.push_back(1001);
    std::vector<int64_t> configs;
    EXPECT_NE(tags.size(), configs.size());
    configs.push_back(1608000);
    EXPECT_EQ(tags.size(), configs.size());
    OHOS::SOCPERF::SocPerfClient::GetInstance().LimitRequest(clientId_power, tags, configs, "");
}

/*
 * @tc.name: SocPerfSubTest_LimitRequest_002
 * @tc.desc: LimitRequest thermal
 * @tc.type FUNC
 */
HWTEST_F(SocPerfSubTest, SocPerfSubTest_LimitRequest_002, Function | MediumTest | Level0)
{
    int32_t clientId_thermal = ACTION_TYPE_THERMAL;
    std::vector<int32_t> tags;
    tags.push_back(1000);
    tags.push_back(1001);
    std::vector<int64_t> configs;
    EXPECT_NE(tags.size(), configs.size());
    configs.push_back(1800000);
    configs.push_back(1992000);
    EXPECT_EQ(tags.size(), configs.size());
    OHOS::SOCPERF::SocPerfClient::GetInstance().LimitRequest(clientId_thermal, tags, configs, "");
}

/*
 * @tc.name: SocPerfSubTest_ResetClient_001
 * @tc.desc: ResetClient
 * @tc.type FUNC
 */
HWTEST_F(SocPerfSubTest, SocPerfSubTest_ResetClient_001, Function | MediumTest | Level0)
{
    OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequest(10000, "");
    OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(10000, true, "");
    OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(10000, false, "");
    OHOS::SOCPERF::SocPerfClient::GetInstance().PowerLimitBoost(true, "");
    OHOS::SOCPERF::SocPerfClient::GetInstance().PowerLimitBoost(false, "");
    OHOS::SOCPERF::SocPerfClient::GetInstance().ThermalLimitBoost(true, "");
    OHOS::SOCPERF::SocPerfClient::GetInstance().ThermalLimitBoost(false, "");
    OHOS::SOCPERF::SocPerfClient::GetInstance().ResetClient();
}
} // namespace SOCPERF
} // namespace OHOS
