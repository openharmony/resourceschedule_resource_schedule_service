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

#include "plugin_switch_test.h"
#include "plugin_mgr.h"

using namespace std;
using namespace testing::ext;

namespace OHOS {
namespace ResourceSchedule {
const string TEST_PREFIX_SWITCH_PATH = "/data/test/resource/resschedfwk/parseswitch/";

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
 * @tc.name: Init plugin Switch LoadConfig001
 * @tc.desc: Verify if can Init the plugin correctly
 * @tc.type: FUNC
 * @tc.require: issueI798UT
 * @tc.author:xukuan
 */
HWTEST_F(PluginSwitchTest, LoadConfig001, TestSize.Level1)
{
    pluginSwitch_ = make_unique<PluginSwitch>();
    std::vector<std::string> contents;
    PluginMgr::GetInstance().GetConfigContent(-1, TEST_PREFIX_SWITCH_PATH + "fileNotExist", contents);
    bool ret = false;
    for (auto content : contents) {
        ret = pluginSwitch_->LoadFromConfigContent(content);
    }
    EXPECT_TRUE(!ret);
}

/**
 * @tc.name: Init plugin Switch LoadConfig002
 * @tc.desc: Verify if can load file while xml does not have pluginlist node
 * @tc.type: FUNC
 * @tc.require: issueI798UT
 * @tc.author:xukuan
 */
HWTEST_F(PluginSwitchTest, LoadConfig002, TestSize.Level1)
{
    pluginSwitch_ = make_unique<PluginSwitch>();
    std::vector<std::string> contents;
    PluginMgr::GetInstance().GetConfigContent(-1, TEST_PREFIX_SWITCH_PATH + "not_exist_plugin.xml", contents);
    bool ret = false;
    for (auto content : contents) {
        ret = pluginSwitch_->LoadFromConfigContent(content);
    }
    EXPECT_TRUE(!ret);
}

/**
 * @tc.name: Init plugin Switch LoadConfig003
 * @tc.desc: Verify if can load file invalid format config file
 * @tc.type: FUNC
 * @tc.require: issueI798UT
 * @tc.author:xukuan
 */
HWTEST_F(PluginSwitchTest, LoadConfig003, TestSize.Level1)
{
    std::vector<std::string> contents;
    PluginMgr::GetInstance().GetConfigContent(-1, TEST_PREFIX_SWITCH_PATH + "invalid_format.xml", contents);
    bool ret = false;
    for (auto content : contents) {
        ret = pluginSwitch_->LoadFromConfigContent(content);
    }
    EXPECT_TRUE(!ret);
}

/**
 * @tc.name: Init plugin Switch LoadConfig004
 * @tc.desc: Verify if can load wrong root element config
 * @tc.type: FUNC
 * @tc.require: issueI798UT
 * @tc.author:xukuan
 */
HWTEST_F(PluginSwitchTest, LoadConfig004, TestSize.Level1)
{
    std::vector<std::string> contents;
    PluginMgr::GetInstance().GetConfigContent(-1, TEST_PREFIX_SWITCH_PATH + "root_element_wrong.xml", contents);
    bool ret = false;
    for (auto content : contents) {
        ret = pluginSwitch_->LoadFromConfigContent(content);
    }
    EXPECT_TRUE(!ret);
}

/**
 * @tc.name: Init plugin Switch IsInvalidNode001
 * @tc.desc: IsInvalidNode
 * @tc.type: FUNC
 * @tc.require: issueI798UT
 * @tc.author:luolu
 */
HWTEST_F(PluginSwitchTest, IsInvalidNode001, TestSize.Level1)
{
    xmlNode currNode;
    EXPECT_TRUE(pluginSwitch_->IsInvalidNode(currNode));
}
} // namespace ResourceSchedule
} // namespace OHOS
