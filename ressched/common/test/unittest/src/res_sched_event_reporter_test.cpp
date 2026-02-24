/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "res_sched_event_reporter_test.h"

#include "res_sched_event_reporter.h"

using namespace std;
using namespace testing::ext;

namespace OHOS {
namespace ResourceSchedule {

void ResSchedEventReporterTest::SetUpTestCase() {}

void ResSchedEventReporterTest::TearDownTestCase() {}

void ResSchedEventReporterTest::SetUp() {}

void ResSchedEventReporterTest::TearDown() {}

/**
 * @tc.name: ResSchedEventReporterTest ReportFileSizeEvent_001
 * @tc.desc: test ReportFileSizeEvent
 * @tc.type: FUNC
 * @tc.require: issueIC1ZAF
 */
HWTEST_F(ResSchedEventReporterTest, ReportFileSizeEvent_001, Function | MediumTest | Level0)
{
    EXPECT_FALSE(ResschedEventReporter::GetInstance().isTaskSubmit_);
    std::vector<std::string> filePaths = {
        "/data/test/test1.txt",
        "/data/test/test2.txt",
    };
    ResschedEventReporter::GetInstance().ReportFileSizeEvent(filePaths);
    EXPECT_TRUE(ResschedEventReporter::GetInstance().isTaskSubmit_);
    EXPECT_TRUE(ResschedEventReporter::GetInstance().allFilePaths_.find("/data/test/test1.txt") !=
        ResschedEventReporter::GetInstance().allFilePaths_.end());
    EXPECT_TRUE(ResschedEventReporter::GetInstance().allFilePaths_.find("/data/test/test2.txt") !=
        ResschedEventReporter::GetInstance().allFilePaths_.end());
    EXPECT_FALSE(ResschedEventReporter::GetInstance().allFilePaths_.find("") !=
        ResschedEventReporter::GetInstance().allFilePaths_.end());
    ResschedEventReporter::GetInstance().allFilePaths_.clear();
    ResschedEventReporter::GetInstance().isTaskSubmit_ = false;
}

/**
 * @tc.name: ResSchedEventReporterTest ReportFileSizeEvent_002
 * @tc.desc: test ReportFileSizeEvent
 * @tc.type: FUNC
 * @tc.require: issueIC1ZAF
 */
HWTEST_F(ResSchedEventReporterTest, ReportFileSizeEvent_002, Function | MediumTest | Level0)
{
    EXPECT_FALSE(ResschedEventReporter::GetInstance().isTaskSubmit_);
    ResschedEventReporter::GetInstance().ReportFileSizeEvent("/data/test/test1.txt");
    EXPECT_TRUE(ResschedEventReporter::GetInstance().isTaskSubmit_);
    EXPECT_TRUE(ResschedEventReporter::GetInstance().allFilePaths_.find("/data/test/test1.txt") !=
        ResschedEventReporter::GetInstance().allFilePaths_.end());
    EXPECT_FALSE(ResschedEventReporter::GetInstance().allFilePaths_.find("/data/test/test2.txt") !=
        ResschedEventReporter::GetInstance().allFilePaths_.end());
    ResschedEventReporter::GetInstance().allFilePaths_.clear();
    ResschedEventReporter::GetInstance().isTaskSubmit_ = false;
}

/**
 * @tc.name: ResSchedEventReporterTest ValidReportTime_001
 * @tc.desc: test ValidReportTime
 * @tc.type: FUNC
 */
HWTEST_F(ResSchedEventReporterTest, ValidReportTime_001, Function | MediumTest | Level0)
{
    EXPECT_EQ(0, ResschedEventReporter::GetInstance().ValidReportTime(-100));
    EXPECT_EQ(86400000000, ResschedEventReporter::GetInstance().ValidReportTime(186400000000));
    EXPECT_EQ(1000, ResschedEventReporter::GetInstance().ValidReportTime(1000));
}
} // namespace ResourceSchedule
} // namespace OHOS
