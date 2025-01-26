/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "res_sched_system_util_test.h"

#include "res_sched_system_util.h"

using namespace std;
using namespace testing::ext;

namespace OHOS {
namespace ResourceSchedule {


void ResSchedSystemUtilTest::SetUpTestCase() {}

void ResSchedSystemUtilTest::TearDownTestCase() {}

void ResSchedSystemUtilTest::SetUp() {}

void ResSchedSystemUtilTest::TearDown() {}

/**
 * @tc.name: ResSchedSystemUtilTest GetTotalRamSize_001
 * @tc.desc: test GetTotalRamSize
 * @tc.type: FUNC
 * @tc.require: issueIB8Y0E
 */
HWTEST_F(ResSchedSystemUtilTest, GetTotalRamSize_001, Function | MediumTest | Level0)
{
    EXPECT_NE(ResCommonUtil::GetTotalRamSize("test"), 0);
    EXPECT_NE(ResCommonUtil::GetTotalRamSize(), 0);
}

/**
 * @tc.name: ResSchedSystemUtilTest GetTotalRomSize_001
 * @tc.desc: test GetTotalRomSize
 * @tc.type: FUNC
 * @tc.require: issueIB8Y0E
 */
HWTEST_F(ResSchedSystemUtilTest, GetTotalRomSize_001, Function | MediumTest | Level0)
{
    EXPECT_NE(ResCommonUtil::GetTotalRomSize(), 0);
}

/**
 * @tc.name: ResSchedSystemUtilTest GetSystemProperties_001
 * @tc.desc: test GetSystemProperties
 * @tc.type: FUNC
 * @tc.require: issueIB8Y0E
 */
HWTEST_F(ResSchedSystemUtilTest, GetSystemProperties_001, Function | MediumTest | Level0)
{
    EXPECT_EQ(ResCommonUtil::GetSystemProperties("", ""), "");
}
} // namespace ResourceSchedule
} // namespace OHOS
