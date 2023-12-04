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

#include <gtest/gtest.h>
#include <gtest/hwext/gtest-multithread.h>
#include "socperf_server.h"

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
 * @tc.name: SocPerfServerTest_LogEnable_001
 * @tc.desc: test log switch func
 * @tc.type FUNC
 * @tc.require: issueI78T3V
 */
HWTEST_F(SocPerfSubTest, SocPerfSubTest_PerfRequest_001, Function | MediumTest | Level0)
{
    SocPerfLog::EnableDebugLog();
    bool isEnable = SocPerfLog::IsDebugLogEnables();
    EXPECT_TRUE(isEnable);
    SocPerfLog::DisableDebugLog();
    isEnable = SocPerfLog::IsDebugLogEnables();
    EXPECT_FALSE(isEnable);
}

/*
 * @tc.name: SocPerfServerTest_LogEnable_002
 * @tc.desc: test socperf server api
 * @tc.type FUNC
 * @tc.require: issueI78T3V
 */
HWTEST_F(SocPerfSubTest, SocPerfSubTest_PerfRequest_002, Function | MediumTest | Level0)
{
    std::string msg = "testBoost";
    socPerfServer_->PerfRequest(10000, msg);
    socPerfServer_->PerfRequestEx(10000, true, msg);
    socPerfServer_->PerfRequestex(10000, false, msg);
    socPerfServer_->LimitRequest(ActionType::ACTION_TYPE_POWER, {1005}, {1364000}, msg);
    socPerfServer_->LimitRequest(ActionType::ACTION_TYPE_THERMAL, {1005}, {1364000}, msg);
    socPerfServer_->PowerLimitBoost(true, msg);
    socPerfServer_->ThermalLimitBoost(true, msg);
    socPerfServer_->Dump(-1, {});
    EXPECT_EQ(msg, "testBoost");
} // namespace SOCPERF
} // namespace OHOS