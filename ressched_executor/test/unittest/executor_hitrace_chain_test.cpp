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

#include "executor_hitrace_chain_test.h"

#include "executor_hitrace_chain.h"

using namespace std;
using namespace testing::ext;

namespace OHOS {
namespace ResourceSchedule {
namespace {
}

void ExecutorHiTraceChainTest::SetUpTestCase() {}

void ExecutorHiTraceChainTest::TearDownTestCase() {}

void ExecutorHiTraceChainTest::SetUp() {}

void ExecutorHiTraceChainTest::TearDown() {}
/**
 * @tc.name: ExecutorHiTraceChainTest ExecutorHiTraceChainTest_001
 * @tc.desc: test ExecutorHiTraceChainTest_001
 * @tc.type: FUNC
 * @tc.require: add tracechain
 */
HWTEST_F(ExecutorHiTraceChainTest, ExecutorHiTraceChainTest_001, Function | MediumTest | Level0)
{
    ExecutorHiTraceChain traceChain(__func__);
    EXPECT_TRUE(traceChain.isBegin_);
    EXPECT_TRUE(HiTraceChainIsValid(&traceChain.traceId_));
}

} // namespace ResourceSchedule
} // namespace OHOS
