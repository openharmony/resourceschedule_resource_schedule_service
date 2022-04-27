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
#include <iostream>

#define private public
#define protected public
#include "sched_policy.h"
#undef private
#undef protected

using namespace testing::ext;
using namespace  OHOS::ResourceSchedule::CgroupSetting;

namespace OHOS {
namespace ResourceSchedule {
namespace CgroupSetting {
class ProcessGroupSubTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void ProcessGroupSubTest::SetUpTestCase(void)
{
}

void ProcessGroupSubTest::TearDownTestCase(void)
{
}

void ProcessGroupSubTest::SetUp(void)
{
}

void ProcessGroupSubTest::TearDown(void)
{
}

/**
 * @tc.name: ProcessGroupSubTest_SchedPolicyType_001
 * @tc.desc: Get SchedPolicy Type
 * @tc.type: FUNC
 * @tc.require: SR000GGT7N AR000GH37H
 * @tc.desc:
 */
HWTEST_F(ProcessGroupSubTest, ProcessGroupSubTest_SchedPolicyType_001, Function | MediumTest | Level0)
{
    EXPECT_EQ(SP_BACKGROUND, 1);
}

/*
 * @tc.number: ProcessGroupSubTest_SchedPolicyType_002
 * @tc.name: Get SchedPolicy Type
 * @tc.type: FUNC
 * @tc.require: SR000GGT7N AR000GH37I
 * @tc.desc:
 */
HWTEST_F(ProcessGroupSubTest, ProcessGroupSubTest_SchedPolicyType_002, Function | MediumTest | Level0)
{
    EXPECT_EQ(SP_FOREGROUND, 2);
}

/*
 * @tc.number: ProcessGroupSubTest_SchedPolicyType_003
 * @tc.name: Get SchedPolicy Full Name
 * @tc.type: FUNC
 * @tc.require: SR000GGT7N AR000GH37J
 * @tc.desc:
 */
HWTEST_F(ProcessGroupSubTest, ProcessGroupSubTest_SchedPolicyType_003, Function | MediumTest | Level0)
{
    const char* policyName = GetSchedPolicyFullName(SP_DEFAULT);
    EXPECT_TRUE(!strcmp(policyName, "sp_default"));
}

/*
 * @tc.number: ProcessGroupSubTest_SchedPolicyType_004
 * @tc.name: Get SchedPolicy Full Name
 * @tc.type: FUNC
 * @tc.require: SR000GGT7N AR000GH37K
 * @tc.desc:
 */
HWTEST_F(ProcessGroupSubTest, ProcessGroupSubTest_SchedPolicyType_004, Function | MediumTest | Level0)
{
    const char* policyName = GetSchedPolicyFullName(SP_BACKGROUND);
    EXPECT_TRUE(!strcmp(policyName, "sp_background"));
}

/*
 * @tc.number: ProcessGroupSubTest_SchedPolicyType_005
 * @tc.name: Get SchedPolicy Full Name
 * @tc.type: FUNC
 * @tc.require: SR000GGT7N AR000GH37L
 * @tc.desc:
 */
HWTEST_F(ProcessGroupSubTest, ProcessGroupSubTest_SchedPolicyType_005, Function | MediumTest | Level0)
{
    const char* policyName = GetSchedPolicyFullName(SP_FOREGROUND);
    EXPECT_TRUE(!strcmp(policyName, "sp_foreground"));
}

/*
 * @tc.number: ProcessGroupSubTest_SchedPolicyType_006
 * @tc.name: Get SchedPolicy Full Name
 * @tc.type: FUNC
 * @tc.require: SR000GGT7N AR000GH37M AR000GH37L AR000GH37N AR000GH37Q
 * @tc.desc:
 */
HWTEST_F(ProcessGroupSubTest, ProcessGroupSubTest_SchedPolicyType_006, Function | MediumTest | Level0)
{
    const char* policyName = GetSchedPolicyFullName(SP_SYSTEM_BACKGROUND);
    EXPECT_TRUE(!strcmp(policyName, "sp_system_background"));
}

/*
 * @tc.number: ProcessGroupSubTest_SchedPolicyType_007
 * @tc.name: Get SchedPolicy Full Name
 * @tc.type: FUNC
 * @tc.require: SR000GGT7N AR000GH37N AR000GH37J AR000GH37K AR000GH37R
 * @tc.desc:
 */
HWTEST_F(ProcessGroupSubTest, ProcessGroupSubTest_SchedPolicyType_007, Function | MediumTest | Level0)
{
    const char* policyName = GetSchedPolicyFullName(SP_TOP_APP);
    EXPECT_TRUE(!strcmp(policyName, "sp_top_app"));
}

/*
 * @tc.number: ProcessGroupSubTest_SchedPolicyType_008
 * @tc.name: Get SchedPolicy Shortened Name
 * @tc.type: FUNC
 * @tc.require: SR000GGT7N AR000GH37O AR000GH37I AR000GH37M AR000GH37S
 * @tc.desc:
 */
HWTEST_F(ProcessGroupSubTest, ProcessGroupSubTest_SchedPolicyType_008, Function | MediumTest | Level0)
{
    const char* policyName = GetSchedPolicyShortenedName(SP_DEFAULT);
    EXPECT_TRUE(!strcmp(policyName, "df"));
}

/*
 * @tc.number: ProcessGroupSubTest_SchedPolicyType_009
 * @tc.name: Get SchedPolicy Shortened Name
 * @tc.type: FUNC
 * @tc.require: SR000GGT7N AR000GH37P AR000GH37H AR000GH37N AR000GH37T
 * @tc.desc:
 */
HWTEST_F(ProcessGroupSubTest, ProcessGroupSubTest_SchedPolicyType_009, Function | MediumTest | Level0)
{
    const char* policyName = GetSchedPolicyShortenedName(SP_FOREGROUND);
    EXPECT_TRUE(!strcmp(policyName, "fg"));
}

/*
 * @tc.number: ProcessGroupSubTest_SchedPolicyType_010
 * @tc.name: Get SchedPolicy Shortened Name
 * @tc.type: FUNC
 * @tc.require: SR000GGT7N AR000GH37Q AR000GH37R AR000GH37S AR000GH37T
 * @tc.desc:
 */
HWTEST_F(ProcessGroupSubTest, ProcessGroupSubTest_SchedPolicyType_010, Function | MediumTest | Level0)
{
    const char* policyName = GetSchedPolicyShortenedName(SP_BACKGROUND);
    EXPECT_TRUE(!strcmp(policyName, "bg"));
}
} // namespace CgroupSetting
} // namespace ResourceSchedule
} // namespace OHOS
