/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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

#include <dlfcn.h>
#include "plugin_mgr.h"
#include "res_type.h"
#include "plugin_mgr_test.h"
#include "socperf_plugin.h"
#include "mock_plugin_mgr.h"
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
 * @tc.require: issueI798UT
 * @tc.author:xukuan
 */
HWTEST_F(PluginMgrTest, Init001, TestSize.Level1)
{
    pluginMgr_->Init();
    EXPECT_TRUE(pluginMgr_->initStatus == pluginMgr_->INIT_SUCCESS);
    EXPECT_EQ(pluginMgr_->pluginLibMap_.size(), 0);
}

/**
 * @tc.name: Plugin mgr test Init 002
 * @tc.desc: Verify if can init fault.
 * @tc.type: FUNC
 * @tc.require: issueI8VZVN
 * @tc.author:z30053169
 */
HWTEST_F(PluginMgrTest, Init002, TestSize.Level1)
{
    PluginMgr::GetInstance().pluginSwitch_ = nullptr;
    pluginMgr_->Init();
    SUCCEED();
}

/**
 * @tc.name: Plugin mgr test Stop 001
 * @tc.desc: Verify if can stop success.
 * @tc.type: FUNC
 * @tc.require: issueI798UT
 * @tc.author:xukuan
 */
HWTEST_F(PluginMgrTest, Stop001, TestSize.Level1)
{
    pluginMgr_->Stop();
    EXPECT_EQ(pluginMgr_->pluginLibMap_.size(), 0);
    EXPECT_EQ(pluginMgr_->resTypeLibMap_.size(), 0);
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
 * @tc.name: Plugin mgr test GetConfig 002
 * @tc.desc: Verify if can get config with wrong env.
 * @tc.type: FUNC
 * @tc.require: issueI8VZVN
 * @tc.author:z30053169
 */
HWTEST_F(PluginMgrTest, GetConfig002, TestSize.Level1)
{
    PluginMgr::GetInstance().configReader_ = nullptr;
    PluginConfig config = pluginMgr_->GetConfig("", "");
    SUCCEED();
}

/**
 * @tc.name: Plugin mgr test SubscribeResource 002
 * @tc.desc: Verify if can SubscribeResource
 * @tc.type: FUNC
 * @tc.require: issueI8VZVN
 * @tc.author:z30053169
 */
HWTEST_F(PluginMgrTest, SubscribeResource002, TestSize.Level1)
{
    PluginMgr::GetInstance().SubscribeResource("", 0);
    SUCCEED();
    PluginMgr::GetInstance().SubscribeResource("test", 1);
    SUCCEED();
    PluginMgr::GetInstance().UnSubscribeResource("test", 1);
    SUCCEED();
}

/**
 * @tc.name: Plugin mgr test UnSubscribeResource 003
 * @tc.desc: Verify if can SubscribeResource
 * @tc.type: FUNC
 * @tc.require: issueI8VZVN
 * @tc.author:z30053169
 */
HWTEST_F(PluginMgrTest, UnSubscribeResource003, TestSize.Level1)
{
    PluginMgr::GetInstance().UnSubscribeResource("", 0);
    SUCCEED();
    PluginMgr::GetInstance().UnSubscribeResource("test", 0);
    SUCCEED();
    PluginMgr::GetInstance().SubscribeResource("test1", 1);
    PluginMgr::GetInstance().SubscribeResource("test2", 1);
    PluginMgr::GetInstance().UnSubscribeResource("test1", 1);
    SUCCEED();
    PluginMgr::GetInstance().UnSubscribeResource("test2", 1);
    SUCCEED();
}

/**
 * @tc.name: Plugin mgr test DispatchResource 001
 * @tc.desc: Verify if can DispatchResource
 * @tc.type: FUNC
 * @tc.require: issueI8VZVN
 * @tc.author:z30053169
 */
HWTEST_F(PluginMgrTest, DispatchResource001, TestSize.Level1)
{
    pluginMgr_->Init();
    nlohmann::json payload;
    auto data = std::make_shared<ResData>(ResType::RES_TYPE_APP_ABILITY_START,
        ResType::AppStartType::APP_COLD_START, payload);
    pluginMgr_->DispatchResource(data);
    pluginMgr_->DispatchResource(nullptr);
    SUCCEED();
}

/**
 * @tc.name: Plugin mgr test DispatchResource 002
 * @tc.desc: Verify if can DispatchResource
 * @tc.type: FUNC
 * @tc.require: issueI8VZVN
 * @tc.author:z30053169
 */
HWTEST_F(PluginMgrTest, DispatchResource002, TestSize.Level1)
{
    nlohmann::json payload;
    auto data = std::make_shared<ResData>(ResType::RES_TYPE_APP_ABILITY_START,
        ResType::AppStartType::APP_COLD_START, payload);
    PluginMgr::GetInstance().SubscribeResource("", 0);
    SUCCEED();
    PluginMgr::GetInstance().SubscribeResource("test", ResType::RES_TYPE_APP_ABILITY_START);
    SUCCEED();
    PluginMgr::GetInstance().DispatchResource(data);
    PluginMgr::GetInstance().UnSubscribeResource("", 0);
    SUCCEED();
}

/**
 * @tc.name: Plugin mgr test SubscribeResource 001
 * @tc.desc: Verify if can stop success.
 * @tc.type: FUNC
 * @tc.require: issueI798UT
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
 * @tc.name: Plugin mgr test SubscribeSyncResource 001
 * @tc.desc: Verify if can stop success.
 * @tc.type: FUNC
 */
HWTEST_F(PluginMgrTest, SubscribeSyncResource001, TestSize.Level1)
{
    pluginMgr_->Init();
    pluginMgr_->SubscribeSyncResource(LIB_NAME, ResType::RES_TYPE_SCREEN_STATUS);
    auto iter = pluginMgr_->resTypeLibSyncMap_.find(ResType::RES_TYPE_SCREEN_STATUS);
    string libName = iter->second;
    EXPECT_EQ(libName.compare(LIB_NAME), 0);
}

/**
 * @tc.name: Plugin mgr test UnSubscribeSyncResource 001
 * @tc.desc: Verify if can stop success.
 * @tc.type: FUNC
 */
HWTEST_F(PluginMgrTest, UnSubscribeSyncResource001, TestSize.Level1)
{
    pluginMgr_->Init();
    pluginMgr_->SubscribeSyncResource(LIB_NAME, ResType::RES_TYPE_SCREEN_STATUS);
    pluginMgr_->UnSubscribeSyncResource(LIB_NAME, ResType::RES_TYPE_SCREEN_STATUS);
    auto iter = pluginMgr_->resTypeLibSyncMap_.find(ResType::RES_TYPE_SCREEN_STATUS);
    EXPECT_TRUE(iter == pluginMgr_->resTypeLibSyncMap_.end());
}

/**
 * @tc.name: Plugin mgr test DeliverResource 001
 * @tc.desc: Verify if can DeliverResource
 * @tc.type: FUNC
 */
HWTEST_F(PluginMgrTest, DeliverResource001, TestSize.Level1)
{
    pluginMgr_->Init();
    nlohmann::json payload;
    nlohmann::json reply;
    auto data = std::make_shared<ResData>(ResType::RES_TYPE_APP_ABILITY_START,
        ResType::AppStartType::APP_COLD_START, payload, reply);
    pluginMgr_->DeliverResource(data);
    pluginMgr_->DeliverResource(nullptr);
    SUCCEED();
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
 * @tc.require: issueI798UT
 * @tc.author:xukuan
 */
HWTEST_F(PluginMgrTest, UnSubscribeResource001, TestSize.Level1)
{
    pluginMgr_->UnSubscribeResource(LIB_NAME, ResType::RES_TYPE_SCREEN_STATUS);
    auto iter = pluginMgr_->resTypeLibMap_.find(ResType::RES_TYPE_SCREEN_STATUS);
    EXPECT_TRUE(iter == pluginMgr_->resTypeLibMap_.end());
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
    data->value = ResType::ClickEventType::TOUCH_EVENT_DOWN;
    SocPerfPlugin::GetInstance().DispatchResource(data);
    data->value = ResType::ClickEventType::TOUCH_EVENT_UP;
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
    SUCCEED();
}

/*
 * @tc.name: SocPerfSubTest_DispatchResource_002
 * @tc.desc: DispatchResource Plugin
 * @tc.type FUNC
 * @tc.author:qiunaiguang
 * @tc.require: issueI6I9QS
 */
HWTEST_F(PluginMgrTest, PluginMgrTest_DispatchResource_002, Function | MediumTest | Level0)
{
    /* Init */
    SocPerfPlugin::GetInstance().Init();

    nlohmann::json payload;
    std::shared_ptr<ResData> resData =
        std::make_shared<ResData>(ResType::RES_TYPE_LOAD_PAGE, ResType::LoadPageType::LOAD_PAGE_START, payload);
    SocPerfPlugin::GetInstance().HandleLoadPage(resData);
    resData->value = ResType::LoadPageType::LOAD_PAGE_COMPLETE;
    SocPerfPlugin::GetInstance().HandleLoadPage(resData);
    /* DeInit */
    SocPerfPlugin::GetInstance().Disable();
    SUCCEED();
}

/*
 * @tc.name: SocPerfSubTest_DispatchResource_003
 * @tc.desc: DispatchResource Plugin
 * @tc.type FUNC
 * @tc.author:qiunaiguang
 * @tc.require: issueI6I9QS
 */
HWTEST_F(PluginMgrTest, PluginMgrTest_DispatchResource_003, Function | MediumTest | Level0)
{
    /* Init */
    SocPerfPlugin::GetInstance().Init();
    nlohmann::json payload;
    std::shared_ptr<ResData> resData =
        std::make_shared<ResData>(ResType::RES_TYPE_SHOW_REMOTE_ANIMATION,
        ResType::ShowRemoteAnimationStatus::ANIMATION_BEGIN, payload);
    SocPerfPlugin::GetInstance().HandleRemoteAnimation(resData);

    resData->value = ResType::ShowRemoteAnimationStatus::ANIMATION_END;
    SocPerfPlugin::GetInstance().HandleRemoteAnimation(resData);
    /* DeInit */
    SocPerfPlugin::GetInstance().Disable();
    SUCCEED();
}

/*
 * @tc.name: SocPerfSubTest_DispatchResource_004
 * @tc.desc: DispatchResource Plugin
 * @tc.type FUNC
 * @tc.author:qiunaiguang
 * @tc.require: issueI6I9QS
 */
HWTEST_F(PluginMgrTest, PluginMgrTest_DispatchResource_004, Function | MediumTest | Level0)
{
    /* Init */
    SocPerfPlugin::GetInstance().Init();
    SocPerfPlugin::GetInstance().InitFeatureSwitch("socperf_on_demand");
    SocPerfPlugin::GetInstance().InitFeatureSwitch("test");
    /* DeInit */
    SocPerfPlugin::GetInstance().Disable();
    SUCCEED();
}

/*
 * @tc.name: SocPerfSubTest_DispatchResource_005
 * @tc.desc: DispatchResource Plugin
 * @tc.type FUNC
 * @tc.author:fangdinggeng
 * @tc.require: issueI5VWUI
 */
HWTEST_F(PluginMgrTest, PluginMgrTest_DispatchResource_005, TestSize.Level1)
{
    nlohmann::json payload;
    auto data = std::make_shared<ResData>(ResType::RES_TYPE_DEVICE_MODE_STATUS,
        ResType::DeviceModeStatus::MODE_ENTER, payload);
    /* Init */
    SocPerfPlugin::GetInstance().Init();

    /* HandleDeviceModeStatusChange */
    data->payload["deviceMode"] = "test";
    SocPerfPlugin::GetInstance().DispatchResource(data);
    data->value = ResType::DeviceModeStatus::MODE_QUIT;
    SocPerfPlugin::GetInstance().DispatchResource(data);

    /* DeInit */
    SocPerfPlugin::GetInstance().Disable();
    SUCCEED();
}

/**
 * @tc.name: Plugin mgr test DumPluginInfoAppend_001
 * @tc.desc: test the interface DumpluginInfoAppend
 * @tc.type: FUNC
 * @tc.require: issueI8VZVN
 * @tc.author:z30053169
 */
HWTEST_F(PluginMgrTest, DumPluginInfoAppend_001, TestSize.Level1)
{
    string result;
    PluginInfo info;
    info.switchOn = false;
    PluginMgr::GetInstance().DumpPluginInfoAppend(result, info);
    SUCCEED();
}

/**
 * @tc.name: Plugin mgr test DispatchResource 003
 * @tc.desc: test the interface DispatchResource
 * @tc.type: FUNC
 * @tc.require: issueI8VZVN
 * @tc.author:z30053169
 */
HWTEST_F(PluginMgrTest, DispatchResource003, TestSize.Level1)
{
    nlohmann::json payload;
    PluginMgr::GetInstance().dispatcher_ = nullptr;
    auto data = std::make_shared<ResData>(ResType::RES_TYPE_APP_ABILITY_START,
        ResType::AppStartType::APP_COLD_START, payload);
    PluginMgr::GetInstance().UnSubscribeResource("test", ResType::RES_TYPE_APP_ABILITY_START);
    PluginMgr::GetInstance().DispatchResource(data);
    SUCCEED();
}

/**
 * @tc.name: Plugin mgr test DispatchResource 004
 * @tc.desc: Verify if can DispatchResource
 * @tc.type: FUNC
 * @tc.require: issueI91HSR
 * @tc.author:baiheng
 */
HWTEST_F(PluginMgrTest, DispatchResource004, TestSize.Level1)
{
    nlohmann::json payload;
    auto dataNoExtType = std::make_shared<ResData>(ResType::RES_TYPE_KEY_PERF_SCENE,
        ResType::KeyPerfStatus::ENTER_SCENE, payload);
    PluginMgr::GetInstance().SubscribeResource("test", 10000);
    SUCCEED();
    PluginMgr::GetInstance().DispatchResource(dataNoExtType);
    SUCCEED();
}

/**
 * @tc.name: Plugin mgr test DispatchResource 005
 * @tc.desc: Verify if can DispatchResource
 * @tc.type: FUNC
 * @tc.require: issueI91HSR
 * @tc.author:baiheng
 */
HWTEST_F(PluginMgrTest, DispatchResource005, TestSize.Level1)
{
    nlohmann::json payload;
    payload["extType"] = "10000";
    auto dataWithExtType = std::make_shared<ResData>(ResType::RES_TYPE_KEY_PERF_SCENE,
        ResType::KeyPerfStatus::ENTER_SCENE, payload);
    PluginMgr::GetInstance().SubscribeResource("test", 10000);
    SUCCEED();
    PluginMgr::GetInstance().DispatchResource(dataWithExtType);
    SUCCEED();
}

/**
 * @tc.name: Plugin mgr test GetPluginLib 001
 * @tc.desc: Verify if can get pluginlib with wrong env.
 * @tc.type: FUNC
 * @tc.require: issueI9C9JN
 * @tc.author:xiaoshun
 */
HWTEST_F(PluginMgrTest, GetPluginLib001, TestSize.Level0)
{
    std::shared_ptr<PluginLib> libInfoPtr = pluginMgr_->GetPluginLib("test");
    EXPECT_TRUE(libInfoPtr == nullptr);
}

/**
 * @tc.name: Plugin mgr test GetPluginLib 002
 * @tc.desc: Verify if can get pluginlib
 * @tc.type: FUNC
 * @tc.require: issueI9C9JN
 * @tc.author:xiaoshun
 */
HWTEST_F(PluginMgrTest, GetPluginLib002, TestSize.Level0)
{
    std::shared_ptr<PluginLib> libInfoPtr = pluginMgr_->GetPluginLib("libapp_preload_plugin.z.so");
    SUCCEED();
}
} // namespace ResourceSchedule
} // namespace OHOS
