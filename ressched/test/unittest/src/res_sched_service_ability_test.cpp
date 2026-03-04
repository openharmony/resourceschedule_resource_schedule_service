/*
 * Copyright (c) 2026-2026 Huawei Device Co., Ltd.
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

#include "gtest/gtest.h"
#include "res_sched_service_ability.h"

namespace OHOS {
namespace ResourceSchedule {
using namespace std;
using namespace testing::ext;
class ResSchedServiceAbilityTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void ResSchedServiceAbilityTest::SetUpTestCase(void) {}

void ResSchedServiceAbilityTest::TearDownTestCase() {}

void ResSchedServiceAbilityTest::SetUp() {}

void ResSchedServiceAbilityTest::TearDown() {}

/**
 * @tc.name: ResSchedServiceAbilityTest_SystemAbilityListenerInit_001
 * @tc.desc: Test for SystemAbilityListenerInit
 * @tc.type: FUNC
 * @tc.require: issue1658
 */
HWTEST_F(ResSchedServiceAbilityTest, SystemAbilityListenerInit_001, TestSize.Level1)
{
    ResSchedServiceAbility ability;
    ability.SystemAbilityListenerInit();
    std::vector<std::string> noErr;
    EXPECT_TRUE(ability.ReportSAListenerResult(noErr));
    std::vector<std::string> errIds = {"111", "222", "333"};
    EXPECT_FALSE(ability.ReportSAListenerResult(errIds));
}
} // namespace ResourceSchedule
} // namespace OHOS
