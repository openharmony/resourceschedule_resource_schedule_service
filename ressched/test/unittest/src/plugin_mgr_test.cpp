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
#include "socperf_plugin.h"
#include "res_data.h"
#include "res_type.h"

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
 * @tc.name: Plugin mgr test GetConfig 001
 * @tc.desc: Verify if can get config with wrong env.
 * @tc.type: FUNC
 * @tc.require: issueI5WWV3
 * @tc.author:lice
 */
HWTEST_F(PluginMgrTest, GetConfig001, TestSize.Level1)
{
    PluginConfig config = pluginMgr_->GetConfig("", "");
    EXPECT_TRUE(config.itemList.empty());
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
 * @tc.name: Plugin mgr test Dump 001
 * @tc.desc: Verify if dump commands is success.
 * @tc.type: FUNC
 * @tc.require: issueI5WWV3
 * @tc.author:lice
 */
HWTEST_F(PluginMgrTest, Dump001, TestSize.Level1)
{
    std::string res;
    pluginMgr_->Init();
    pluginMgr_->DumpAllPlugin(res);
    EXPECT_TRUE(!res.empty());
    res = "";

    pluginMgr_->LoadPlugin();
    pluginMgr_->DumpHelpFromPlugin(res);
    EXPECT_TRUE(res.empty());
    res = "";

    std::vector<std::string> args;
    pluginMgr_->DumpOnePlugin(res, LIB_NAME, args);
    EXPECT_TRUE(!res.empty());
    res = "";

    args.emplace_back("-h");
    pluginMgr_->DumpOnePlugin(res, LIB_NAME, args);
    EXPECT_TRUE(!res.empty());
}

/**
 * @tc.name: Plugin mgr test RepairPlugin 001
 * @tc.desc: Verify if RepairPlugin is success.
 * @tc.type: FUNC
 * @tc.require: issueI5WWV3
 * @tc.author:lice
 */
HWTEST_F(PluginMgrTest, RepairPlugin001, TestSize.Level1)
{
    PluginLib libInfo = pluginMgr_->pluginLibMap_.find(LIB_NAME)->second;
    pluginMgr_->RepairPlugin(Clock::now(), LIB_NAME, libInfo);
    EXPECT_TRUE(true);
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

/**
 * @tc.name: Plugin mgr test Remove 001
 * @tc.desc: Verify if RemoveDisablePluginHandler is success.
 * @tc.type: FUNC
 * @tc.require: issueI5WWV3
 * @tc.author:lice
 */
HWTEST_F(PluginMgrTest, Remove001, TestSize.Level1)
{
    pluginMgr_->RemoveDisablePluginHandler();
    EXPECT_TRUE(!pluginMgr_->disablePlugins_.size());

    pluginMgr_->LoadPlugin();
}

/*
 * @tc.name: SocPerfSubTest_DispatchResource_001
 * @tc.desc: DispatchResource Plugin
 * @tc.type FUNC
 * @tc.author:zoujie
 * @tc.require: issueI5VWUI
 */
HWTEST_F(PluginMgrTest, PluginMgrTest_DispatchResource_001, TestSize.Level1)
{
    nlohmann::json payload;
    auto data = std::make_shared<ResData>(ResType::RES_TYPE_APP_ABILITY_START,
        ResType::AppStartType::APP_COLD_START, payload);
    /* Init */
    SocPerfPlugin::GetInstance().Init();
    /* HandleAppAbilityStart */
    SocPerfPlugin::GetInstance().DispatchResource(data);
    data->value = ResType::AppStartType::APP_WARM_START;
    SocPerfPlugin::GetInstance().DispatchResource(data);

    /* HandleWindowFocus */
    data->resType = ResType::RES_TYPE_WINDOW_FOCUS;
    data->value = ResType::WindowFocusStatus::WINDOW_FOCUS;
    SocPerfPlugin::GetInstance().DispatchResource(data);

    /* HandleEventClick */
    data->resType = ResType::RES_TYPE_CLICK_RECOGNIZE;
    data->value = ResType::ClickEventType::TOUCH_EVENT;
    SocPerfPlugin::GetInstance().DispatchResource(data);
    data->value = ResType::ClickEventType::CLICK_EVENT;
    SocPerfPlugin::GetInstance().DispatchResource(data);

    /* HandlePushPage */
    data->resType = ResType::RES_TYPE_PUSH_PAGE;
    data->value = ResType::PushPageType::PUSH_PAGE_START;
    SocPerfPlugin::GetInstance().DispatchResource(data);
    data->value = ResType::PushPageType::PUSH_PAGE_COMPLETE;
    SocPerfPlugin::GetInstance().DispatchResource(data);

    /* HandlePopPage */
    data->resType = ResType::RES_TYPE_POP_PAGE;
    data->value = 0;
    SocPerfPlugin::GetInstance().DispatchResource(data);

    /* HandleEventSlide */
    data->resType = ResType::RES_TYPE_SLIDE_RECOGNIZE;
    data->value = ResType::SlideEventStatus::SLIDE_EVENT_ON;
    SocPerfPlugin::GetInstance().DispatchResource(data);
    data->value = ResType::SlideEventStatus::SLIDE_EVENT_OFF;
    SocPerfPlugin::GetInstance().DispatchResource(data);

    /* HandleEventWebGesture */
    data->resType = ResType::RES_TYPE_WEB_GESTURE;
    data->value = 0;
    SocPerfPlugin::GetInstance().DispatchResource(data);

    /* HandleResizeWindow */
    data->resType = ResType::RES_TYPE_RESIZE_WINDOW;
    data->value = ResType::WindowResizeType::WINDOW_RESIZING;
    SocPerfPlugin::GetInstance().DispatchResource(data);
    data->value = ResType::WindowResizeType::WINDOW_RESIZE_STOP;
    SocPerfPlugin::GetInstance().DispatchResource(data);

    /* HandleMoveWindow */
    data->resType = ResType::RES_TYPE_MOVE_WINDOW;
    data->value = ResType::WindowMoveType::WINDOW_MOVING;
    SocPerfPlugin::GetInstance().DispatchResource(data);
    data->value = ResType::WindowMoveType::WINDOW_MOVE_STOP;
    SocPerfPlugin::GetInstance().DispatchResource(data);

    /* DeInit */
    SocPerfPlugin::GetInstance().Disable();
}
} // namespace ResourceSchedule
} // namespace OHOS
