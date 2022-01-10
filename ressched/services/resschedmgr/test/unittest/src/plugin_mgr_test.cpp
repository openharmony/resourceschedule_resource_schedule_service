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

#define private public
#include <dlfcn.h>
#include "plugin_mgr_test.h"
#include "res_type.h"
#include "plugin_mgr.h"
#undef private
#include "mock.h"

using namespace std;
using namespace testing::ext;

namespace OHOS {
namespace ResourceSchedule {
namespace {
    const string LIB_NAME = "libunittestplugin_cplusplus.z.so";
}

void PluginMgrTest::SetUpTestCase() {}

void PluginMgrTest::TearDownTestCase() {}

void PluginMgrTest::SetUp()
{
    /**
     * @tc.setup: initialize the member variable pluginMgr_
     */
    pluginMgr_ = make_shared<MockPluginMgr>();
}

void PluginMgrTest::TearDown()
{
    /**
     * @tc.teardown: clear pluginMgr_
     */
    pluginMgr_ = nullptr;
}

/*
 * Feature: PluginMgr
 * Function: Init
 * SubFunction: NA
 * FunctionPoints: PluginMgr Init
 * EnvConditions: code is start PluginMgr
 * CaseDescription: Verify that on remote request is normal and abnormal
 */
HWTEST_F(PluginMgrTest, PluginMgrTest_001, TestSize.Level1)
{
    pluginMgr_->Init();
    EXPECT_TRUE(!pluginMgr_->initStatus == pluginMgr_->INIT_SUCCESS);
    EXPECT_TRUE(!pluginMgr_->pluginLibMap_.size() == 1);
}
} // namespace ResourceSchedule
} // namespace OHOS