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
#include "ressched_utils.h"

using namespace testing::ext;
namespace OHOS {
namespace ResourceSchedule {
class ResschedUtilsTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void ResschedUtilsTest::SetUpTestCase(void)
{
}

void ResschedUtilsTest::TearDownTestCase(void)
{
}

void ResschedUtilsTest::SetUp(void)
{
}

void ResschedUtilsTest::TearDown(void)
{
}

/**
 * @tc.name: ResschedUtilsTest_CheckTidIsInPid_001
 * @tc.desc: check tid is in pid or not
 * @tc.type: FUNC
 * @tc.require: issueI9BPMB
 * @tc.desc:z30053169
 */
HWTEST_F(ResschedUtilsTest, ProcessGroupSubTest_SchedPolicyType_001, Function | MediumTest | Level0)
{
    EXPECT_EQ(ResSchedUtils::GetInstance().CheckTidIsInPid(0, 0), false);
}

/**
 * @tc.name: ResschedUtilsTest_CheckTidIsInPid_002
 * @tc.desc: check tid is in pid or not
 * @tc.type: FUNC
 * @tc.require: issueI9BPMB
 * @tc.desc:z30053169
 */
HWTEST_F(ResschedUtilsTest, ProcessGroupSubTest_SchedPolicyType_002, Function | MediumTest | Level0)
{
    EXPECT_EQ(ResSchedUtils::GetInstance().CheckTidIsInPid(1, 1), true);
}
}
}