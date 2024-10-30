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

#include "batch_log_printer_test.h"
#include "batch_log_printer.h"


using namespace std;
using namespace testing::ext;

namespace OHOS {
namespace ResourceSchedule {
namespace {
    constexpr int32_t TEST_SUBMIT_NUM = 50;
    constexpr int32_t TEST_PRINT_NUM = 50;
    const string TEST_LOG = "test log";
} // namespace

void BatchLogPrinterTest::SetUpTestCase() {}

void BatchLogPrinterTest::TearDownTestCase() {}

void BatchLogPrinterTest::SetUp() {}

void BatchLogPrinterTest::TearDown() {}


/**
 * @tc.name: BatchLogPrinterTest SubmitLogTest_001
 * @tc.desc: test SubmitLog
 * @tc.type: FUNC
 * @tc.require: issueIAZWOS
 */
HWTEST_F(BatchLogPrinterTest, SubmitLogTest_001, Function | MediumTest | Level0)
{
    for (int i = 0;i < TEST_SUBMIT_NUM; ++i) {
        BatchLogPrinter::GetInstance().SubmitLog(TEST_LOG);
    }
    sleep(1);
    EXPECT_EQ(BatchLogPrinter::GetInstance().allLogs_.size(), TEST_SUBMIT_NUM);
    for (int i = 0;i < TEST_PRINT_NUM - TEST_SUBMIT_NUM; ++i) {
        BatchLogPrinter::GetInstance().SubmitLog(TEST_LOG);
    }
    sleep(1);
    EXPECT_EQ(BatchLogPrinter::GetInstance().allLogs_.size(), 0);
}
} // namespace ResourceSchedule
} // namespace OHOS
