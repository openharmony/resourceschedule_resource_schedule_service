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
#include "plugin_mgr.h"
#include "res_type.h"
#include "plugin_mgr_test.h"
#undef private
#include "mock_plugin_mgr.h"

using namespace std;
using namespace testing::ext;

namespace OHOS {
namespace ResourceSchedule {
namespace {
    const string LIB_NAME = "libunittest_plugin.z.so";
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

/**
 * @tc.name: Plugin mgr test Init 001
 * @tc.desc: Verify if can init success.
 * @tc.type: FUNC
 * @tc.require: SR000GGUUN AR000GH37C AR000GH37D AR000GH37E AR000GH37F AR000GH37G AR000GH1JU AR000GH1JO
 * @tc.author:xukuan
 */
HWTEST_F(PluginMgrTest, Init001, TestSize.Level1)
{
    pluginMgr_->Init();
    EXPECT_TRUE(pluginMgr_->initStatus == pluginMgr_->INIT_SUCCESS);
    EXPECT_EQ(pluginMgr_->pluginLibMap_.size(), 0);
}

/**
 * @tc.name: Plugin mgr test Stop 001
 * @tc.desc: Verify if can stop success.
 * @tc.type: FUNC
 * @tc.require: SR000GGUUN AR000GH37C AR000GH37D AR000GH37E AR000GH37F AR000GH37G AR000GH1JU AR000GH1JO
 * @tc.author:xukuan
 */
HWTEST_F(PluginMgrTest, Stop001, TestSize.Level1)
{
    pluginMgr_->Stop();
    EXPECT_EQ(pluginMgr_->pluginLibMap_.size(), 0);
    EXPECT_EQ(pluginMgr_->resTypeLibMap_.size(), 0);
    EXPECT_EQ(pluginMgr_->dispatcherHandlerMap_.size(), 0);
    EXPECT_TRUE(pluginMgr_->configReader_ == nullptr);
}

/**
 * @tc.name: Plugin mgr test SubscribeResource 001
 * @tc.desc: Verify if can stop success.
 * @tc.type: FUNC
 * @tc.require: SR000GGUUN AR000GH1JP AR000GH1JR AR000GH1JS AR000GH1JT AR000GH1JV AR000GH1K0
 * @tc.author:xukuan
 */
HWTEST_F(PluginMgrTest, SubscribeResource001, TestSize.Level1)
{
    pluginMgr_->Init();
    pluginMgr_->SubscribeResource(LIB_NAME, ResType::RES_TYPE_SCREEN_STATUS);
    auto iter = pluginMgr_->resTypeLibMap_.find(ResType::RES_TYPE_SCREEN_STATUS);
    string libName = iter->second.back();
    EXPECT_EQ(libName.compare(LIB_NAME), 0);
}

/**
 * @tc.name: Plugin mgr test UnSubscribeResource 001
 * @tc.desc: Verify if can stop success.
 * @tc.type: FUNC
 * @tc.require: SR000GGUUN AR000GH1JP AR000GH1JR AR000GH1JS AR000GH1JT AR000GH1JV AR000GH1K0
 * @tc.author:xukuan
 */
HWTEST_F(PluginMgrTest, UnSubscribeResource001, TestSize.Level1)
{
    pluginMgr_->UnSubscribeResource(LIB_NAME, ResType::RES_TYPE_SCREEN_STATUS);
    auto iter = pluginMgr_->resTypeLibMap_.find(ResType::RES_TYPE_SCREEN_STATUS);
    EXPECT_TRUE(iter == pluginMgr_->resTypeLibMap_.end());
}
} // namespace ResourceSchedule
} // namespace OHOS
