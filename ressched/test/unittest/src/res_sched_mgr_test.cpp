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

#include "res_sched_mgr_test.h"

using namespace std;
using namespace testing::ext;

namespace OHOS {
namespace ResourceSchedule {
namespace {
    const string LIB_NAME = "libunittest_plugin.z.so";
}

void ResSchedMgrTest::SetUpTestCase() {}

void ResSchedMgrTest::TearDownTestCase() {}

void ResSchedMgrTest::SetUp() {}

void ResSchedMgrTest::TearDown() {}

/**
 * @tc.name: Init ressched mgr 001
 * @tc.desc: Verify if can Init the plugin correctly
 * @tc.type: FUNC
 * @tc.require: SR000GGUUN AR000GH00L
 * @tc.author:xukuan
 */
HWTEST_F(ResSchedMgrTest, Init001, TestSize.Level1)
{
    ResSchedMgr::GetInstance().Init();
    EXPECT_TRUE(ResSchedMgr::GetInstance().mainHandler_ != nullptr);
}

/**
 * @tc.name: Init ressched ReportData 002
 * @tc.desc: Verify if ReportData is success
 * @tc.type: FUNC
 * @tc.require: issueI5WWV3
 * @tc.author:lice
 */
HWTEST_F(ResSchedMgrTest, ReportData001, TestSize.Level1)
{
    nlohmann::json payload;
    ResSchedMgr::GetInstance().ReportData(0, 0, payload);
    EXPECT_TRUE(ResSchedMgr::GetInstance().mainHandler_ != nullptr);
    
    ResSchedMgr::GetInstance().DispatchResourceInner(0, 0, payload);
}
} // namespace ResourceSchedule
} // namespace OHOS
