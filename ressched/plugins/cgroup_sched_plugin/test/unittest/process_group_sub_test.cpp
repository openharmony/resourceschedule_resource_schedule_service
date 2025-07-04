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
#include "supervisor.h"
#include "cgroup_adjuster.h"
#include "cgroup_log.h"
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
 * @tc.require: issueI798UT
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
 * @tc.require: issueI798UT
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
 * @tc.require: issueI798UT
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
 * @tc.require: issueI798UT
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
 * @tc.require: issueI798UT
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
 * @tc.require: issueI798UT
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
 * @tc.require: issueI798UT
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
 * @tc.require: issueI798UT
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
 * @tc.require: issueI798UT
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
 * @tc.require: issueI798UT
 * @tc.desc:
 */
HWTEST_F(ProcessGroupSubTest, ProcessGroupSubTest_SchedPolicyType_010, Function | MediumTest | Level0)
{
    const char* policyName = GetSchedPolicyShortenedName(SP_BACKGROUND);
    EXPECT_TRUE(!strcmp(policyName, "bg"));
}

/*
 * @tc.number: ProcessGroupSubTest_AdjustProcess_001
 * @tc.name: Adjust Fork Process Schedule
 * @tc.type: FUNC
 * @tc.require: issueI9TPCH
 * @tc.desc:
 */
HWTEST_F(ProcessGroupSubTest, ProcessGroupSubTest_AdjustProcess_001, Function | MediumTest | Level1)
{
    auto application = new Application(20150011);
    EXPECT_TRUE(application != nullptr);
    auto pr = new ProcessRecord(20150011, 9050);
    EXPECT_TRUE(pr != nullptr);
    CgroupAdjuster::GetInstance().AdjustForkProcessGroup(*application, *pr);
    SUCCEED();
}

/*
 * @tc.number: ProcessGroupSubTest_AdjustProcess_001
 * @tc.name: Adjust Fork Process Schedule
 * @tc.type: FUNC
 */
HWTEST_F(ProcessGroupSubTest, ProcessGroupSubTest_CgroupLog_001, Function | MediumTest | Level1)
{
    auto application = new Application(20150011);
    EXPECT_TRUE(application != nullptr);
    CgsLogD("test logD!");
    CgsLogI("test logI!");
    CgsLogW("test logW!");
    CgsLogE("test logE!");
    CgsLogF("test logF!");
    SUCCEED();
}
} // namespace CgroupSetting
} // namespace ResourceSchedule
} // namespace OHOS
