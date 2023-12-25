/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include <gtest/gtest.h>
#include <gtest/hwext/gtest-multithread.h>
#include "socperf_server.h"
#include "socperf.h"

using namespace testing::ext;
using namespace testing::mt;

namespace OHOS {
namespace SOCPERF {
class SocPerfServerTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
private:
    std::shared_ptr<SocPerfServer> socPerfServer_;
    SocPerf socPerf_;
};

void SocPerfServerTest::SetUpTestCase(void)
{
}

void SocPerfServerTest::TearDownTestCase(void)
{
}

void SocPerfServerTest::SetUp(void)
{
    socPerfServer_ = DelayedSingleton<SocPerfServer>::GetInstance();
    socPerfServer_->OnStart();

    socPerf_.Init();
}

void SocPerfServerTest::TearDown(void)
{
    socPerfServer_ = nullptr;
}

/*
 * @tc.name: SocPerfServerTest_LogEnable_001
 * @tc.desc: test log switch func
 * @tc.type FUNC
 * @tc.require: issueI78T3V
 */
HWTEST_F(SocPerfServerTest, SocPerfServerTest_LogEnable_001, Function | MediumTest | Level0)
{
    SocPerfLog::EnableDebugLog();
    bool isEnable = SocPerfLog::IsDebugLogEnabled();
    EXPECT_TRUE(isEnable);
    SocPerfLog::DisableDebugLog();
    isEnable = SocPerfLog::IsDebugLogEnabled();
    EXPECT_FALSE(isEnable);
}

/*
 * @tc.name: SocPerfServerTest_SocPerfServerAPI_001
 * @tc.desc: test socperf server api
 * @tc.type FUNC
 * @tc.require: issueI78T3V
 */
HWTEST_F(SocPerfServerTest, SocPerfServerTest_SocPerfServerAPI_001, Function | MediumTest | Level0)
{
    std::string msg = "testBoost";
    socPerf_.PerfRequest(10000, msg);
    socPerf_.PerfRequestEx(10000, true, msg);
    socPerf_.PerfRequestEx(10000, false, msg);
    socPerf_.PerfRequestEx(10028, true, msg);
    socPerf_.PerfRequestEx(10028, false, msg);
    socPerf_.LimitRequest(ActionType::ACTION_TYPE_POWER, {1001}, {999000}, msg);
    socPerf_.LimitRequest(ActionType::ACTION_TYPE_THERMAL, {1001}, {999000}, msg);
    socPerf_.LimitRequest(ActionType::ACTION_TYPE_POWER, {1001}, {1325000}, msg);
    socPerf_.LimitRequest(ActionType::ACTION_TYPE_THERMAL, {1001}, {1325000}, msg);
    socPerf_.PowerLimitBoost(true, msg);
    socPerf_.ThermalLimitBoost(true, msg);

    socPerfServer_->PerfRequest(10000, msg);
    socPerfServer_->PerfRequestEx(10000, true, msg);
    socPerfServer_->PerfRequestEx(10000, false, msg);
    socPerfServer_->LimitRequest(ActionType::ACTION_TYPE_POWER, {1001}, {1364000}, msg);
    socPerfServer_->PowerLimitBoost(true, msg);
    socPerfServer_->LimitRequest(ActionType::ACTION_TYPE_THERMAL, {1001}, {1364000}, msg);
    socPerfServer_->ThermalLimitBoost(true, msg);
    socPerfServer_->PowerLimitBoost(false, msg);
    socPerfServer_->ThermalLimitBoost(false, msg);
    socPerfServer_->OnStop();
    EXPECT_EQ(msg, "testBoost");
}
} // namespace SOCPERF
} // namespace OHOS