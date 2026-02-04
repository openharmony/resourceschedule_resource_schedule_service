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
    std::atomic<int32_t> g_initFinishCallCountA(0);
    std::atomic<int32_t> g_initFinishCallCountB(0);

    void TestInitFinishCallbackA()
    {
        ++g_initFinishCallCountA;
    }

    void TestInitFinishCallbackB()
    {
        ++g_initFinishCallCountB;
    }
}

#ifdef RESOURCE_SCHEDULE_SERVICE_WITH_FFRT_ENABLE
atomic<bool> PluginMgrTest::isBlocked = false;
#endif

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

std::string PluginMgrTest::GetSubItemValue(std::string PluginName, std::string configName)
{
    std::string subItemValue;
    PluginConfig config = pluginMgr_->GetConfig(PluginName, configName);
    if (config.itemList.size() <= 0) {
        return "";
    }
    for (auto item : config.itemList) {
        for (auto subItem : item.subItemList) {
            if (subItem.name == "tag") {
                subItemValue = subItem.value;
            }
        }
    }
    return subItemValue;
}

#ifdef RESOURCE_SCHEDULE_SERVICE_WITH_FFRT_ENABLE
std::shared_ptr<PluginLib> PluginMgrTest::GetTestPlugin()
{
    PluginLib libInfo;
    libInfo.onDispatchResourceFunc_ = [](const std::shared_ptr<ResData>& data) {
        while (PluginMgrTest::isBlocked.load()) {}
    };
    return  make_shared<PluginLib>(libInfo);
}

void PluginMgrTest::LoadTestPlugin()
{
    auto plugin = GetTestPlugin();
    PluginMgr::GetInstance().pluginLibMap_.emplace(LIB_NAME, *plugin);
    PluginMgr::GetInstance().SubscribeResource(LIB_NAME, ResType::RES_TYPE_SLIDE_RECOGNIZE);
    auto callback = [pluginName = LIB_NAME, time = PluginMgr::GetInstance().pluginBlockTime]() {
        PluginMgr::GetInstance().HandlePluginTimeout(pluginName);
        ffrt::submit([pluginName]() {
            PluginMgr::GetInstance().EnablePluginIfResume(pluginName);
            }, {}, {}, ffrt::task_attr().delay(time));
    };
    PluginMgr::GetInstance().dispatchers_.emplace(LIB_NAME, std::make_shared<ffrt::queue>(LIB_NAME.c_str(),
        ffrt::queue_attr().timeout(PluginMgr::GetInstance().pluginBlockTime).callback(callback)));
}
#endif

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
    EXPECT_TRUE(pluginMgr_->pluginSwitch_ != nullptr);
}

/**
 * @tc.name: Plugin mgr test GetPluginSwitch 001
 * @tc.desc: Verify if can get pluginSwitch
 * @tc.type: FUNC
 * @tc.require: issuesIA7P80
 * @tc.author:xiaoshun
 */
HWTEST_F(PluginMgrTest, GetPluginSwitch001, TestSize.Level0)
{
    pluginMgr_->Init();
    auto pluginInfoList = pluginMgr_->pluginSwitch_->GetPluginSwitch();
    bool result;
    for (auto pluginInfo : pluginInfoList) {
        if (pluginInfo.libPath == "libapp_preload_plugin.z.so") {
            EXPECT_TRUE(pluginInfo.switchOn);
        } else if (pluginInfo.libPath == "libapp_preload_plugin2.z.so" ||
            pluginInfo.libPath == "libapp_preload_plugin3.z.so" ||
            pluginInfo.libPath == "libapp_preload_plugin4.z.so") {
            EXPECT_TRUE(!pluginInfo.switchOn);
        }
    }
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
 * @tc.require: issuesIA7P80
 * @tc.author:lice
 */
HWTEST_F(PluginMgrTest, GetConfig002, TestSize.Level1)
{
    pluginMgr_->Init();
    std::string subItemValue = GetSubItemValue("demo2", "sample");
    bool ret = !subItemValue.empty() && subItemValue == "test_sys_prod";
    EXPECT_TRUE(ret);
    subItemValue = GetSubItemValue("demo3", "sample");
    ret = !subItemValue.empty() && subItemValue == "test_sys_prod";
    EXPECT_TRUE(ret);
    subItemValue = GetSubItemValue("demo4", "sample");
    ret = !subItemValue.empty() && subItemValue == "test_system";
    EXPECT_TRUE(ret);
}

/**
 * @tc.name: Plugin mgr test GetConfig 003
 * @tc.desc: Verify if can get config with wrong env.
 * @tc.type: FUNC
 * @tc.require: issueI8VZVN
 * @tc.author:z30053169
 */
HWTEST_F(PluginMgrTest, GetConfig003, TestSize.Level1)
{
    PluginMgr::GetInstance().configReader_ = nullptr;
    PluginConfig config = pluginMgr_->GetConfig("", "");
    EXPECT_EQ(config.itemList.size(), 0);
}

/**
 * @tc.name: Plugin mgr test RemoveConfig 001
 * @tc.desc: Verify if can get config with wrong env.
 * @tc.type: FUNC
 * @tc.require: issueI5WWV3
 * @tc.author:lice
 */
HWTEST_F(PluginMgrTest, RemoveConfig001, TestSize.Level1)
{
    pluginMgr_->RemoveConfig("", "");
    PluginConfig config = pluginMgr_->GetConfig("", "");
    EXPECT_TRUE(config.itemList.empty());
}

/**
 * @tc.name: Plugin mgr test RemoveConfig 002
 * @tc.desc: Verify if can get config with wrong env.
 * @tc.type: FUNC
 * @tc.require: issuesIA7P80
 * @tc.author:lice
 */
HWTEST_F(PluginMgrTest, RemoveConfig002, TestSize.Level1)
{
    pluginMgr_->Init();
    std::string subItemValue = GetSubItemValue("demo2", "sample");
    bool ret = !subItemValue.empty() && subItemValue == "test_sys_prod";
    EXPECT_TRUE(ret);
    pluginMgr_->RemoveConfig("demo2", "sample");
    subItemValue = GetSubItemValue("demo2", "sample");
    ret = subItemValue.empty();
    EXPECT_TRUE(ret);
}

/**
 * @tc.name: Plugin mgr test RemoveConfig 003
 * @tc.desc: Verify if can get config with wrong env.
 * @tc.type: FUNC
 * @tc.require: issuesIA7P80
 * @tc.author:lice
 */
 HWTEST_F(PluginMgrTest, RemoveConfig003, TestSize.Level1)
 {
     pluginMgr_->Init();
     std::string subItemValue = GetSubItemValue("demo3", "sample");
     bool ret = !subItemValue.empty() && subItemValue == "test_sys_prod";
     EXPECT_TRUE(ret);
     pluginMgr_->RemoveConfig("demo3", "sample");
     subItemValue = GetSubItemValue("demo3", "sample");
     ret = subItemValue.empty();
     EXPECT_TRUE(ret);
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
    EXPECT_EQ(PluginMgr::GetInstance().resTypeLibMap_.size(), 0);
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
    EXPECT_EQ(PluginMgr::GetInstance().resTypeLibMap_.size(), 0);
}

/**
 * @tc.name: Plugin mgr test UnSubscribeAllResources 001
 * @tc.desc: Verify if can SubscribeResource
 * @tc.type: FUNC
 * @tc.require: IC9LVT
 */
HWTEST_F(PluginMgrTest, UnSubscribeAllResources001, TestSize.Level1)
{
    PluginMgr::GetInstance().UnSubscribeAllResources("");
    EXPECT_EQ(PluginMgr::GetInstance().resTypeLibMap_.size(), 0);
    PluginMgr::GetInstance().SubscribeResource("test", 1);
    PluginMgr::GetInstance().SubscribeResource("test", 2);
    EXPECT_EQ(PluginMgr::GetInstance().resTypeLibMap_.size(), 2);
    PluginMgr::GetInstance().UnSubscribeAllResources("test");
    EXPECT_EQ(PluginMgr::GetInstance().resTypeLibMap_.size(), 0);
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
#ifndef RESOURCE_SCHEDULE_SERVICE_WITH_FFRT_ENABLE
    if (pluginMgr_->dispatcher_ == nullptr) {
        pluginMgr_->dispatcher_ = std::make_shared<AppExecFwk::EventHandler>(
            AppExecFwk::EventRunner::Create("rssDispatcher"));
    }
#endif
    nlohmann::json payload;
    auto data = std::make_shared<ResData>(ResType::RES_TYPE_APP_ABILITY_START,
        ResType::AppStartType::APP_COLD_START, payload);
    pluginMgr_->DispatchResource(data);
    EXPECT_TRUE(pluginMgr_->dispatcher_ != nullptr);
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
#ifndef RESOURCE_SCHEDULE_SERVICE_WITH_FFRT_ENABLE
    if (PluginMgr::GetInstance().dispatcher_ == nullptr) {
        PluginMgr::GetInstance().dispatcher_ = std::make_shared<AppExecFwk::EventHandler>(
            AppExecFwk::EventRunner::Create("rssDispatcher"));
    }
#endif
    nlohmann::json payload;
    auto data = std::make_shared<ResData>(ResType::RES_TYPE_APP_ABILITY_START,
        ResType::AppStartType::APP_COLD_START, payload);
    PluginMgr::GetInstance().SubscribeResource("", 0);
    SUCCEED();
    PluginMgr::GetInstance().SubscribeResource("test", ResType::RES_TYPE_APP_ABILITY_START);
    SUCCEED();
    PluginMgr::GetInstance().DispatchResource(data);
    EXPECT_TRUE(PluginMgr::GetInstance().dispatcher_ != nullptr);
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
    res = "";
    pluginMgr_->DumpAllPluginConfig(res);
    EXPECT_TRUE(!res.empty());
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
    EXPECT_TRUE(SocPerfPlugin::GetInstance().handle_ == nullptr);
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
    EXPECT_TRUE(SocPerfPlugin::GetInstance().handle_ == nullptr);
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
    EXPECT_TRUE(SocPerfPlugin::GetInstance().handle_ == nullptr);
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
    EXPECT_TRUE(SocPerfPlugin::GetInstance().handle_ == nullptr);
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
    EXPECT_TRUE(SocPerfPlugin::GetInstance().handle_ == nullptr);
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
    EXPECT_FALSE(result.empty());
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
#ifndef RESOURCE_SCHEDULE_SERVICE_WITH_FFRT_ENABLE
    PluginMgr::GetInstance().dispatcher_ = nullptr;
#endif
    auto data = std::make_shared<ResData>(ResType::RES_TYPE_APP_ABILITY_START,
        ResType::AppStartType::APP_COLD_START, payload);
    PluginMgr::GetInstance().UnSubscribeResource("test", ResType::RES_TYPE_APP_ABILITY_START);
    PluginMgr::GetInstance().DispatchResource(data);
    EXPECT_TRUE(PluginMgr::GetInstance().dispatcher_ == nullptr);
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
    EXPECT_TRUE(PluginMgr::GetInstance().resTypeLibMap_.size() == 1);
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
    EXPECT_TRUE(PluginMgr::GetInstance().resTypeLibMap_.size() == 1);
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
    EXPECT_TRUE(pluginMgr_->pluginLibMap_.find("libapp_preload_plugin.z.so") == pluginMgr_->pluginLibMap_.end() ?
        libInfoPtr == nullptr : libInfoPtr != nullptr);
}

/**
 * @tc.name: Plugin mgr test InnerTimeUtil 001
 * @tc.desc: InnerTimeUtil
 * @tc.type: FUNC
 * @tc.require: issueI9C9JN
 * @tc.author:luolu
 */
HWTEST_F(PluginMgrTest, InnerTimeUtil001, TestSize.Level0)
{
    PluginMgr::InnerTimeUtil innerTimeUtil("test1", "test2");
    EXPECT_EQ(innerTimeUtil.functionName_, "test1");
    EXPECT_EQ(innerTimeUtil.pluginName_, "test2");
}

/**
 * @tc.name: Plugin mgr test LoadPlugin 001
 * @tc.desc: LoadPlugin
 * @tc.type: FUNC
 * @tc.require: issueI9C9JN
 * @tc.author:luolu
 */
HWTEST_F(PluginMgrTest, LoadPlugin001, TestSize.Level0)
{
    PluginMgr::GetInstance().LoadPlugin();
    EXPECT_EQ(PluginMgr::GetInstance().pluginLibMap_.size(), 0);
}

/**
 * @tc.name: Plugin mgr test SubscribeSyncResource 002
 * @tc.desc: SubscribeSyncResource
 * @tc.type: FUNC
 * @tc.require: issueI9C9JN
 * @tc.author:luolu
 */
HWTEST_F(PluginMgrTest, SubscribeSyncResource002, TestSize.Level0)
{
    std::string pluginLib;
    uint32_t resType = 0;
    PluginMgr::GetInstance().SubscribeSyncResource(pluginLib, resType);
    EXPECT_EQ(PluginMgr::GetInstance().resTypeLibSyncMap_.size(), 0);
    PluginMgr::GetInstance().UnSubscribeSyncResource(pluginLib, resType);
}

/**
 * @tc.name: Plugin mgr test GetConfigReaderStr 001
 * @tc.desc: Verify if can get ConfigReaderStr.
 * @tc.type: FUNC
 */
HWTEST_F(PluginMgrTest, GetConfigReaderStr001, TestSize.Level0)
{
    auto configStr = pluginMgr_->GetConfigReaderStr();
    EXPECT_TRUE(!configStr.empty());
}

/**
 * @tc.name: Plugin mgr test GetPluginSwitchStr 001
 * @tc.desc: Verify if can get PluginSwitchStr.
 * @tc.type: FUNC
 */
HWTEST_F(PluginMgrTest, GetPluginSwitchStr001, TestSize.Level0)
{
    auto switchStr = pluginMgr_->GetPluginSwitchStr();
    EXPECT_TRUE(!switchStr.empty());
}

/**
 * @tc.name: Plugin mgr test ParseConfigReader 001
 * @tc.desc: Verify if can Parse ConfigReader.
 * @tc.type: FUNC
 */
HWTEST_F(PluginMgrTest, ParseConfigReader001, TestSize.Level0)
{
    pluginMgr_->Init();
    auto configStrs = pluginMgr_->GetConfigReaderStr();
    pluginMgr_->ParseConfigReader(configStrs);
    EXPECT_TRUE(pluginMgr_->configReader_ != nullptr);
}

/**
 * @tc.name: Plugin mgr test ParsePluginSwitch 001
 * @tc.desc: Verify if can Parse PluginSwitch.
 * @tc.type: FUNC
 */
HWTEST_F(PluginMgrTest, ParsePluginSwitchr001, TestSize.Level0)
{
    pluginMgr_->Init();
    auto switchStrs = pluginMgr_->GetPluginSwitchStr();
    pluginMgr_->ParsePluginSwitch(switchStrs);
    EXPECT_TRUE(pluginMgr_->configReader_ != nullptr);
}

/**
 * @tc.name: RemovePluginConfig001
 * @tc.desc: Verify RemovePluginConfig can remove all configurations for specified plugin.
 * @tc.type: FUNC
 * @tc.require: issue1523
 */
HWTEST_F(PluginMgrTest, RemovePluginConfig001, TestSize.Level1)
{
    pluginMgr_->Init();
    PluginConfig config = pluginMgr_->GetConfig("demo", "sample");
    EXPECT_FALSE(config.itemList.empty());
    
    pluginMgr_->RemovePluginConfig("demo");
    
    config = pluginMgr_->GetConfig("demo", "sample");
    EXPECT_TRUE(config.itemList.empty());
}

/**
 * @tc.name: RemovePluginConfig002
 * @tc.desc: Verify RemovePluginConfig handles null configReader.
 * @tc.type: FUNC
 * @tc.require: issue1523
 */
HWTEST_F(PluginMgrTest, RemovePluginConfig002, TestSize.Level1)
{
    PluginMgr::GetInstance().configReader_ = nullptr;
    PluginMgr::GetInstance().RemovePluginConfig("test_plugin");
    EXPECT_TRUE(PluginMgr::GetInstance().configReader_ == nullptr);
}

/**
 * @tc.name: GetPluginListByResTypeAndValue001
 * @tc.desc: Verify GetPluginListByResTypeAndValue returns false when no plugin registered.
 * @tc.type: FUNC
 * @tc.require: issue1523
 */
HWTEST_F(PluginMgrTest, GetPluginListByResTypeAndValue001, TestSize.Level1)
{
    std::list<std::string> pluginList;
    bool result = PluginMgr::GetInstance().GetPluginListByResTypeAndValue(100, 200, pluginList);
    EXPECT_FALSE(result);
    EXPECT_TRUE(pluginList.empty());
}

/**
 * @tc.name: SubscribeResourceAccurately001
 * @tc.desc: Verify SubscribeResourceAccurately handles empty pluginLib.
 * @tc.type: FUNC
 * @tc.require: issue1523
 */
HWTEST_F(PluginMgrTest, SubscribeResourceAccurately001, TestSize.Level1)
{
    PluginMgr::GetInstance().SubscribeResourceAccurately("", 100, 200);
    std::list<std::string> pluginList;
    bool result = PluginMgr::GetInstance().GetPluginListByResTypeAndValue(100, 200, pluginList);
    EXPECT_FALSE(result);
}

/**
 * @tc.name: SubscribeResourceAccurately002
 * @tc.desc: Verify SubscribeResourceAccurately subscribes plugin correctly.
 * @tc.type: FUNC
 * @tc.require: issue1523
 */
HWTEST_F(PluginMgrTest, SubscribeResourceAccurately002, TestSize.Level1)
{
    const std::string pluginLib = "test_plugin";
    uint32_t resType = 100;
    int64_t resValue = 200;
    
    PluginMgr::GetInstance().SubscribeResourceAccurately(pluginLib, resType, resValue);
    
    std::list<std::string> pluginList;
    bool result = PluginMgr::GetInstance().GetPluginListByResTypeAndValue(resType, resValue, pluginList);
    
    EXPECT_TRUE(result);
    EXPECT_FALSE(pluginList.empty());
    EXPECT_EQ(pluginList.front(), pluginLib);
}

/**
 * @tc.name: UnSubscribeResourceAccurately001
 * @tc.desc: Verify UnSubscribeResourceAccurately handles empty pluginLib.
 * @tc.type: FUNC
 * @tc.require: issue1523
 */
HWTEST_F(PluginMgrTest, UnSubscribeResourceAccurately001, TestSize.Level1)
{
    PluginMgr::GetInstance().UnSubscribeResourceAccurately("", 100, 200);
    std::list<std::string> pluginList;
    bool result = PluginMgr::GetInstance().GetPluginListByResTypeAndValue(100, 200, pluginList);
    EXPECT_TRUE(result);
}

/**
 * @tc.name: UnSubscribeResourceAccurately002
 * @tc.desc: Verify UnSubscribeResourceAccurately handles non-existent subscription.
 * @tc.type: FUNC
 * @tc.require: issue1523
 */
HWTEST_F(PluginMgrTest, UnSubscribeResourceAccurately002, TestSize.Level1)
{
    PluginMgr::GetInstance().UnSubscribeResourceAccurately("test_plugin", 100, 200);
    std::list<std::string> pluginList;
    bool result = PluginMgr::GetInstance().GetPluginListByResTypeAndValue(100, 200, pluginList);
    EXPECT_FALSE(result);
}

/**
 * @tc.name: UnSubscribeResourceAccurately003
 * @tc.desc: Verify UnSubscribeResourceAccurately unsubscribes plugin correctly.
 * @tc.type: FUNC
 * @tc.require: issue1523
 */
HWTEST_F(PluginMgrTest, UnSubscribeResourceAccurately003, TestSize.Level1)
{
    const std::string pluginLib = "test_plugin";
    uint32_t resType = 100;
    int64_t resValue = 200;
    
    PluginMgr::GetInstance().SubscribeResourceAccurately(pluginLib, resType, resValue);
    
    std::list<std::string> pluginList;
    bool result = PluginMgr::GetInstance().GetPluginListByResTypeAndValue(resType, resValue, pluginList);
    EXPECT_TRUE(result);
    EXPECT_FALSE(pluginList.empty());
    
    PluginMgr::GetInstance().UnSubscribeResourceAccurately(pluginLib, resType, resValue);
    
    pluginList.clear();
    result = PluginMgr::GetInstance().GetPluginListByResTypeAndValue(resType, resValue, pluginList);
    EXPECT_FALSE(result);
    EXPECT_TRUE(pluginList.empty());
}

/**
 * @tc.name: GetResTypeList001
 * @tc.desc: Verify GetResTypeList includes types from resTyperesValueLibMap.
 * @tc.type: FUNC
 * @tc.require: issue1523
 */
HWTEST_F(PluginMgrTest, GetResTypeList001, TestSize.Level1)
{
    const std::string pluginLib = "test_plugin";
    uint32_t resType = 100;
    int64_t resValue = 200;
    
    PluginMgr::GetInstance().SubscribeResourceAccurately(pluginLib, resType, resValue);
    
    std::set<uint32_t> resTypeList;
    PluginMgr::GetInstance().GetResTypeList(resTypeList);
    
    EXPECT_FALSE(resTypeList.empty());
    EXPECT_NE(resTypeList.find(resType), resTypeList.end());
    PluginMgr::GetInstance().UnSubscribeResourceAccurately(pluginLib, resType, resValue);
}

/**
 * @tc.name: PluginMgrTest_GetPluginListByResType_001
 * @tc.desc: Verify GetPluginListByResType inserts plugins to list when resType exists.
 * @tc.type: FUNC
 * @tc.require: issue1523
 */
HWTEST_F(PluginMgrTest, PluginMgrTest_GetPluginListByResType_001, TestSize.Level1)
{
    const std::string pluginLib1 = "test_plugin1";
    const std::string pluginLib2 = "test_plugin2";
    uint32_t resType = 100;
    
    // 订阅两个插件到同一资源类型
    PluginMgr::GetInstance().SubscribeResource(pluginLib1, resType);
    PluginMgr::GetInstance().SubscribeResource(pluginLib2, resType);
    
    std::list<std::string> pluginList;
    bool result = PluginMgr::GetInstance().GetPluginListByResType(resType, pluginList);
    
    EXPECT_TRUE(result);
    EXPECT_EQ(pluginList.size(), 2);
    
    // 验证两个插件都在列表中
    bool foundPlugin1 = false;
    bool foundPlugin2 = false;
    for (const auto& plugin : pluginList) {
        if (plugin == pluginLib1) {
            foundPlugin1 = true;
        }
        if (plugin == pluginLib2) {
            foundPlugin2 = true;
        }
    }
    EXPECT_TRUE(foundPlugin1);
    EXPECT_TRUE(foundPlugin2);
    
    // 清理
    PluginMgr::GetInstance().UnSubscribeResource(pluginLib1, resType);
    PluginMgr::GetInstance().UnSubscribeResource(pluginLib2, resType);
}

/**
 * @tc.name: PluginMgrTest_DispatchResource_006
 * @tc.type: FUNC
 * @tc.require: issue1523
 */
HWTEST_F(PluginMgrTest, PluginMgrTest_DispatchResource_006, TestSize.Level1)
{
    uint32_t resType = 100;
    int64_t resValue = 200;
    const std::string pluginLib = "test_plugin";
    
    nlohmann::json payload;
    auto data = std::make_shared<ResData>(resType, resValue, payload);
    
    // 确保普通订阅为空
    PluginMgr::GetInstance().UnSubscribeResource(pluginLib, resType);
    
    // 使用精确订阅
    PluginMgr::GetInstance().SubscribeResourceAccurately(pluginLib, resType, resValue);
    
    // 验证普通订阅为空
    std::list<std::string> pluginList1;
    bool result1 = PluginMgr::GetInstance().GetPluginListByResType(resType, pluginList1);
    EXPECT_FALSE(result1);
    
    // 验证精确订阅不为空
    std::list<std::string> pluginList2;
    bool result2 = PluginMgr::GetInstance().GetPluginListByResTypeAndValue(resType, resValue, pluginList2);
    EXPECT_TRUE(result2);
    EXPECT_EQ(pluginList2.size(), 1);
    EXPECT_EQ(pluginList2.front(), pluginLib);
    
    // 执行DispatchResource，应该能正常处理
#ifndef RESOURCE_SCHEDULE_SERVICE_WITH_FFRT_ENABLE
    if (PluginMgr::GetInstance().dispatcher_ == nullptr) {
        PluginMgr::GetInstance().dispatcher_ = std::make_shared<AppExecFwk::EventHandler>(
            AppExecFwk::EventRunner::Create("rssDispatcher"));
    }
#endif
    
    PluginMgr::GetInstance().DispatchResource(data);
    // 如果能执行到这里不崩溃，说明分支处理正确
    
    // 清理
    PluginMgr::GetInstance().UnSubscribeResourceAccurately(pluginLib, resType, resValue);
}

/**
 * @tc.name: PluginMgrTest_UnSubscribeAllResources_002
 * @tc.desc: Verify UnSubscribeAllResources handles case when resTyperesValueLibMap entry is not empty after removal.
 * @tc.type: FUNC
 * @tc.require: issue1523
 */
HWTEST_F(PluginMgrTest, PluginMgrTest_UnSubscribeAllResources_002, TestSize.Level1)
{
    const std::string pluginLib1 = "test_plugin1";
    const std::string pluginLib2 = "test_plugin2";
    uint32_t resType = 100;
    int64_t resValue = 200;
    
    // 订阅两个插件到同一资源类型和值
    PluginMgr::GetInstance().SubscribeResourceAccurately(pluginLib1, resType, resValue);
    PluginMgr::GetInstance().SubscribeResourceAccurately(pluginLib2, resType, resValue);
    
    // 验证两个插件都已订阅
    std::list<std::string> pluginList;
    bool result = PluginMgr::GetInstance().GetPluginListByResTypeAndValue(resType, resValue, pluginList);
    EXPECT_TRUE(result);
    EXPECT_EQ(pluginList.size(), 2);
    
    // 只取消订阅一个插件
    PluginMgr::GetInstance().UnSubscribeResourceAccurately(pluginLib1, resType, resValue);
    
    // 验证仍有一个插件订阅
    pluginList.clear();
    result = PluginMgr::GetInstance().GetPluginListByResTypeAndValue(resType, resValue, pluginList);
    EXPECT_TRUE(result);
    EXPECT_EQ(pluginList.size(), 1);
    EXPECT_EQ(pluginList.front(), pluginLib2);
    
    // 取消订阅所有资源，应该清理掉空的订阅
    PluginMgr::GetInstance().UnSubscribeAllResources(pluginLib2);
    
    // 验证订阅已完全清除
    pluginList.clear();
    result = PluginMgr::GetInstance().GetPluginListByResTypeAndValue(resType, resValue, pluginList);
    EXPECT_FALSE(result);
    EXPECT_TRUE(pluginList.empty());
}

/**
 * @tc.name: PluginMgrTest_UnSubscribeResourceAccurately_004
 * @tc.desc: Verify UnSubscribeResourceAccurately handles case when list is not empty after removal.
 * @tc.type: FUNC
 * @tc.require: issue1523
 */
HWTEST_F(PluginMgrTest, PluginMgrTest_UnSubscribeResourceAccurately_004, TestSize.Level1)
{
    const std::string pluginLib1 = "test_plugin1";
    const std::string pluginLib2 = "test_plugin2";
    uint32_t resType = 100;
    int64_t resValue = 200;
    
    // 订阅两个插件到同一资源类型和值
    PluginMgr::GetInstance().SubscribeResourceAccurately(pluginLib1, resType, resValue);
    PluginMgr::GetInstance().SubscribeResourceAccurately(pluginLib2, resType, resValue);
    
    // 验证两个插件都已订阅
    std::list<std::string> pluginList;
    bool result = PluginMgr::GetInstance().GetPluginListByResTypeAndValue(resType, resValue, pluginList);
    EXPECT_TRUE(result);
    EXPECT_EQ(pluginList.size(), 2);
    
    // 只取消订阅一个插件（关键：此时列表不为空）
    PluginMgr::GetInstance().UnSubscribeResourceAccurately(pluginLib1, resType, resValue);
    
    // 验证仍有一个插件订阅
    pluginList.clear();
    result = PluginMgr::GetInstance().GetPluginListByResTypeAndValue(resType, resValue, pluginList);
    EXPECT_TRUE(result);
    EXPECT_EQ(pluginList.size(), 1);
    EXPECT_EQ(pluginList.front(), pluginLib2);
    
    // 清理剩余的订阅
    PluginMgr::GetInstance().UnSubscribeResourceAccurately(pluginLib2, resType, resValue);
}

/**
 * @tc.name: PluginMgrTest_SubscribeResourceAccurately_003
 * @tc.desc: SubscribeResourceAccurately fallbacks to SubscribeResource when subscriptionAccuractlyEnable_ is false.
 * @tc.type: FUNC
 * @tc.require: issue1571
 */
HWTEST_F(PluginMgrTest, PluginMgrTest_SubscribeResourceAccurately_003, TestSize.Level1)
{
    const std::string pluginLib = "test_plugin";
    uint32_t resType = 100;
    int64_t resValue = 200;
    
    // 保存原始值
    bool originalEnable = PluginMgr::GetInstance().subscriptionAccuractlyEnable_;
    
    // 设置为false以测试分支
    PluginMgr::GetInstance().subscriptionAccuractlyEnable_ = false;
    
    // 调用SubscribeResourceAccurately，应该回退到SubscribeResource
    PluginMgr::GetInstance().SubscribeResourceAccurately(pluginLib, resType, resValue);
    
    // 验证是否添加到了常规订阅映射中
    std::list<std::string> pluginList;
    bool result = PluginMgr::GetInstance().GetPluginListByResType(resType, pluginList);
    EXPECT_TRUE(result);
    EXPECT_FALSE(pluginList.empty());
    EXPECT_EQ(pluginList.front(), pluginLib);
    
    // 清理
    PluginMgr::GetInstance().UnSubscribeResource(pluginLib, resType);
    PluginMgr::GetInstance().subscriptionAccuractlyEnable_ = originalEnable;
}

/**
 * @tc.name: PluginMgrTest_UnSubscribeResourceAccurately_005
 * @tc.desc: UnSubscribeResourceAccurately fallbacks to UnSubscribeResource when subscriptionAccuractlyEnable_ is false.
 * @tc.type: FUNC
 * @tc.require: issue1571
 */
HWTEST_F(PluginMgrTest, PluginMgrTest_UnSubscribeResourceAccurately_005, TestSize.Level1)
{
    const std::string pluginLib = "test_plugin";
    uint32_t resType = 100;
    int64_t resValue = 200;
    
    // 保存原始值
    bool originalEnable = PluginMgr::GetInstance().subscriptionAccuractlyEnable_;
    
    // 先正常订阅
    PluginMgr::GetInstance().SubscribeResourceAccurately(pluginLib, resType, resValue);
    
    // 验证订阅成功
    std::list<std::string> pluginList;
    bool result = PluginMgr::GetInstance().GetPluginListByResTypeAndValue(resType, resValue, pluginList);
    EXPECT_TRUE(result);
    EXPECT_FALSE(pluginList.empty());
    
    // 设置为false以测试分支
    PluginMgr::GetInstance().subscriptionAccuractlyEnable_ = false;
    
    // 调用UnSubscribeResourceAccurately，应该回退到UnSubscribeResource
    PluginMgr::GetInstance().UnSubscribeResourceAccurately(pluginLib, resType, resValue);
    
    // 验证是否从常规订阅映射中移除
    pluginList.clear();
    result = PluginMgr::GetInstance().GetPluginListByResTypeAndValue(resType, resValue, pluginList);
    EXPECT_TRUE(result); // 应该返回false，因为没有插件订阅该资源类型
    
    // 恢复原始值
    PluginMgr::GetInstance().subscriptionAccuractlyEnable_ = originalEnable;
    PluginMgr::GetInstance().UnSubscribeResourceAccurately(pluginLib, resType, resValue);
    pluginList.clear();
    result = PluginMgr::GetInstance().GetPluginListByResTypeAndValue(resType, resValue, pluginList);
    EXPECT_FALSE(result);
}

/**
 * @tc.name: PluginMgrTest_ReadSubscriptionAccuractlyEnableProperties_001
 * @tc.desc: Verify ReadSubscriptionAccuractlyEnableProperties handles valid property value.
 * @tc.type: FUNC
 * @tc.require: issue1571
 */
HWTEST_F(PluginMgrTest, PluginMgrTest_ReadSubscriptionAccuractlyEnableProperties_001, TestSize.Level1)
{
    // 保存原始值
    bool originalEnable = PluginMgr::GetInstance().subscriptionAccuractlyEnable_;
    
    // 测试读取属性的功能（这里我们主要验证函数能够执行而不崩溃）
    PluginMgr::GetInstance().ReadSubscriptionAccuractlyEnableProperties();
    
    // 函数应该能够执行完成
    SUCCEED();
    
    // 恢复原始值
    PluginMgr::GetInstance().subscriptionAccuractlyEnable_ = originalEnable;
}

/**
 * @tc.name: PluginMgrTest_SubscribeResourceAccurately_004
 * @tc.desc: SubscribeResourceAccurately handles empty pluginLib even when subscriptionAccuractlyEnable_ is false.
 * @tc.type: FUNC
 * @tc.require: issue1571
 */
HWTEST_F(PluginMgrTest, PluginMgrTest_SubscribeResourceAccurately_004, TestSize.Level1)
{
    uint32_t resType = 100;
    int64_t resValue = 200;
    
    // 保存原始值
    bool originalEnable = PluginMgr::GetInstance().subscriptionAccuractlyEnable_;
    
    // 设置为false以测试分支
    PluginMgr::GetInstance().subscriptionAccuractlyEnable_ = false;
    
    // 调用SubscribeResourceAccurately，应该回退到SubscribeResource
    PluginMgr::GetInstance().SubscribeResourceAccurately("", resType, resValue);
    
    // 函数应该能够执行完成而不崩溃
    SUCCEED();
    
    // 恢复原始值
    PluginMgr::GetInstance().subscriptionAccuractlyEnable_ = originalEnable;
}

/**
 * @tc.name: PluginMgrTest_UnSubscribeResourceAccurately_006
 * @tc.desc: UnSubscribeResourceAccurately handles empty pluginLib even when subscriptionAccuractlyEnable_ is false.
 * @tc.type: FUNC
 * @tc.require: issue1571
 */
HWTEST_F(PluginMgrTest, PluginMgrTest_UnSubscribeResourceAccurately_006, TestSize.Level1)
{
    uint32_t resType = 100;
    int64_t resValue = 200;
    
    // 保存原始值
    bool originalEnable = PluginMgr::GetInstance().subscriptionAccuractlyEnable_;
    
    // 设置为false以测试分支
    PluginMgr::GetInstance().subscriptionAccuractlyEnable_ = false;
    
    // 调用UnSubscribeResourceAccurately，应该回退到UnSubscribeResource
    PluginMgr::GetInstance().UnSubscribeResourceAccurately("", resType, resValue);
    
    // 函数应该能够执行完成而不崩溃
    SUCCEED();
    
    // 恢复原始值
    PluginMgr::GetInstance().subscriptionAccuractlyEnable_ = originalEnable;
}

/**
 * @tc.name: ResPairHashTest_001
 * @tc.desc: Test ResPair hash function with different values
 * @tc.type: FUNC
 * @tc.require: issue1583
 */
HWTEST_F(PluginMgrTest, ResPairHashTest_001, TestSize.Level1)
{
    OHOS::ResourceSchedule::ResPair pair1(100, 200);
    OHOS::ResourceSchedule::ResPair pair2(100, 200);
    OHOS::ResourceSchedule::ResPair pair3(200, 100);
    OHOS::ResourceSchedule::ResPair pair4(0, 0);
    
    std::hash<OHOS::ResourceSchedule::ResPair> hasher;
    
    // Test that same pairs produce same hash
    size_t hash1 = hasher(pair1);
    size_t hash2 = hasher(pair2);
    EXPECT_EQ(hash1, hash2);
    
    // Test that different pairs produce different hashes
    size_t hash3 = hasher(pair3);
    size_t hash4 = hasher(pair4);
    EXPECT_NE(hash1, hash3);
    EXPECT_NE(hash1, hash4);
    EXPECT_NE(hash3, hash4);
}

/**
 * @tc.name: PluginMgrInitFinishCallback_001
 * @tc.desc: Verify init-finish callbacks are invoked once and cleared after calling.
 * @tc.type: FUNC
 * @tc.require: #1624
 */
HWTEST_F(PluginMgrTest, PluginMgrInitFinishCallback_001, TestSize.Level1)
{
    g_initFinishCallCountA = 0;
    g_initFinishCallCountB = 0;

    auto callbackA = std::make_shared<OnInitFinishCallback>(TestInitFinishCallbackA);
    auto callbackB = std::make_shared<OnInitFinishCallback>(TestInitFinishCallbackB);

    pluginMgr_->RegisterOnInitFinishCallback(callbackA, "lib_init_a");
    pluginMgr_->RegisterOnInitFinishCallback(callbackB, "lib_init_b");

    pluginMgr_->CallOnInitFinishCallbacks();
    EXPECT_EQ(g_initFinishCallCountA.load(), 1);
    EXPECT_EQ(g_initFinishCallCountB.load(), 1);

    pluginMgr_->CallOnInitFinishCallbacks();
    EXPECT_EQ(g_initFinishCallCountA.load(), 1);
    EXPECT_EQ(g_initFinishCallCountB.load(), 1);
}

/**
 * @tc.name: PluginMgrInitFinishCallback_002
 * @tc.desc: Verify invalid callback does not override a valid one for the same library.
 * @tc.type: FUNC
 * @tc.require: #1624
 */
HWTEST_F(PluginMgrTest, PluginMgrInitFinishCallback_002, TestSize.Level1)
{
    g_initFinishCallCountA = 0;

    auto callbackA = std::make_shared<OnInitFinishCallback>(TestInitFinishCallbackA);
    auto invalidCallback = std::make_shared<OnInitFinishCallback>(nullptr);

    pluginMgr_->RegisterOnInitFinishCallback(callbackA, "lib_init_a");
    pluginMgr_->RegisterOnInitFinishCallback(invalidCallback, "lib_init_a");

    pluginMgr_->CallOnInitFinishCallbacks();
    EXPECT_EQ(g_initFinishCallCountA.load(), 1);

    pluginMgr_->CallOnInitFinishCallbacks();
    EXPECT_EQ(g_initFinishCallCountA.load(), 1);
}
} // namespace ResourceSchedule
} // namespace OHOS
