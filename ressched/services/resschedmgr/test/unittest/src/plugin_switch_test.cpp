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
#include "plugin_switch_test.h"
#include "res_type.h"
#include "plugin_switch.h"
#undef private
#include "mock.h"

using namespace std;
using namespace testing::ext;

namespace OHOS {
    namespace ResourceSchedule {
        namespace {
            const string LIB_NAME = "libunittestplugin_cplusplus.z.so";
        }

        void PluginSwitchTest::SetUpTestCase() {}

        void PluginSwitchTest::TearDownTestCase() {}

        void PluginSwitchTest::SetUp()
        {
            /**
             * @tc.setup: initialize the member variable pluginSwitch_
             */
            pluginSwitch_ = make_shared<PluginSwitch>();
        }

        void PluginSwitchTest::TearDown()
        {
            /**
             * @tc.teardown: clear pluginSwitch_
             */
            pluginSwitch_ = nullptr;
        }

/**
 * @tc.name: Init plugin Switch 001
 * @tc.desc: Verify if can Init the plugin correctly
 * @tc.type: FUNC
 * @tc.require: I4PY59
 * @tc.author:xukuan
 */
HWTEST_F(PluginSwitchTest, Init001, TestSize.Level1)
{
    bool ret = pluginSwitch_->LoadFromConfigFile(TEST_PREFIX_SWITCH_PATH + "fileNotExist");
    EXPECT_TRUE(!ret);
}
} // namespace ResourceSchedule
} // namespace OHOS