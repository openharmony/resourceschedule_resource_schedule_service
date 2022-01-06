/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: plugin switch unnittest
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
 * @tc.require: AR000001
 * @tc.author:xukuan
 */
HWTEST_F(PluginSwitchTest, Init001, TestSize.Level1)
{
    bool ret = pluginSwitch_->LoadFromConfigFile(TEST_PREFIX_SWITCH_PATH + "fileNotExist");
    EXPECT_TRUE(!ret);
}

}
}