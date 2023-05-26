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
 * @tc.name: SocPerfSubTest_PerfRequest_001
 * @tc.desc: PerfRequest
 * @tc.type FUNC
 * @tc.require SR000GGTLG AR000GH381 AR000GH382 AR000GH389 AR000GH37U
 */
HWTEST_F(SocPerfSubTest, SocPerfSubTest_PerfRequest_001, Function | MediumTest | Level0)
{
    OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequest(10000, "");
    OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequest(10000, "123");
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
    OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(10000, true, "123");
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
    OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(10000, false, "123");
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
    OHOS::SOCPERF::SocPerfClient::GetInstance().PowerLimitBoost(true, "123");
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
    OHOS::SOCPERF::SocPerfClient::GetInstance().PowerLimitBoost(false, "123");
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
    OHOS::SOCPERF::SocPerfClient::GetInstance().ThermalLimitBoost(true, "123");
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
    OHOS::SOCPERF::SocPerfClient::GetInstance().ThermalLimitBoost(false, "123");
}
} // namespace SOCPERF
} // namespace OHOS