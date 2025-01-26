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

#include "res_sched_time_util_test.h"

#include "res_sched_time_util.h"

using namespace std;
using namespace testing::ext;

namespace OHOS {
namespace ResourceSchedule {
namespace {
    const int64_t MAX_DIFF_MS = 200;
    const int64_t ONE_MS_TO_US = 1000;
    const int64_t ONE_S_TO_MS = 1000;
}

void ResSchedTimeUtilTest::SetUpTestCase() {}

void ResSchedTimeUtilTest::TearDownTestCase() {}

void ResSchedTimeUtilTest::SetUp() {}

void ResSchedTimeUtilTest::TearDown() {}

/**
 * @tc.name: ResSchedTimeUtilTest GetNowMillTime_001
 * @tc.desc: test GetNowMillTime
 * @tc.type: FUNC
 * @tc.require: issueIB8Y0E
 */
HWTEST_F(ResSchedTimeUtilTest, GetNowMillTime_001, Function | MediumTest | Level0)
{
    int64_t start = ResCommonUtil::GetNowMillTime();
    int64_t end = ResCommonUtil::GetNowMillTime();
    int64_t diff = end - start;
    EXPECT_TRUE(diff >= 0 && diff <= MAX_DIFF_MS);
    start = ResCommonUtil::GetNowMillTime(true);
    sleep(1);
    end = ResCommonUtil::GetNowMillTime(true);
    diff = end - start - ONE_S_TO_MS;
    EXPECT_TRUE(diff >= 0 && diff <= MAX_DIFF_MS);
}

/**
 * @tc.name: ResSchedTimeUtilTest GetNowMicroTime_001
 * @tc.desc: test GetNowMicroTime
 * @tc.type: FUNC
 * @tc.require: issueIB8Y0E
 */
HWTEST_F(ResSchedTimeUtilTest, GetNowMicroTime_001, Function | MediumTest | Level0)
{
    int64_t start = ResCommonUtil::GetNowMicroTime();
    int64_t end = ResCommonUtil::GetNowMicroTime();
    int64_t diff = end - start;
    EXPECT_TRUE(diff >= 0 && diff <= (MAX_DIFF_MS * ONE_MS_TO_US));
    start = ResCommonUtil::GetNowMicroTime(true);
    sleep(1);
    end = ResCommonUtil::GetNowMicroTime(true);
    diff = end - start - (ONE_S_TO_MS * ONE_MS_TO_US);
    EXPECT_TRUE(diff >= 0 && diff <= (MAX_DIFF_MS * ONE_MS_TO_US));
}

/**
 * @tc.name: ResSchedTimeUtilTest ConvertTimestampToStr_001
 * @tc.desc: test ConvertTimestampToStr
 * @tc.type: FUNC
 * @tc.require: issueIB8Y0E
 */
HWTEST_F(ResSchedTimeUtilTest, ConvertTimestampToStr_001, Function | MediumTest | Level0)
{
    EXPECT_NE(ResCommonUtil::ConvertTimestampToStr(ResCommonUtil::GetCurrentTimestamp()), "");
}

/**
 * @tc.name: ResSchedTimeUtilTest GetNextMillTimeStamp_001
 * @tc.desc: test GetNextMillTimeStamp
 * @tc.type: FUNC
 * @tc.require: issueIB8Y0E
 */
HWTEST_F(ResSchedTimeUtilTest, GetNextMillTimeStamp_001, Function | MediumTest | Level0)
{
    int64_t currentTime = ResCommonUtil::GetNowMillTime(true);
    int64_t nextTime = ResCommonUtil::GetNextMillTimeStamp(100, true);
    int64_t diff = nextTime - currentTime - 100;
    EXPECT_TRUE(diff >= 0 && diff <= MAX_DIFF_MS);
}
} // namespace ResourceSchedule
} // namespace OHOS
