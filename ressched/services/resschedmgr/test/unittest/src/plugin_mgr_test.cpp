/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: plugin manager unnittest
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

/**
 * @tc.name: Init plugin mgr 001
 * @tc.desc: Verify if can Init the plugin correctly
 * @tc.type: FUNC
 * @tc.require: AR000001
 * @tc.author:xukuan
 */
HWTEST_F(PluginMgrTest, Init001, TestSize.Level1)
{

     pluginMrt_->Init();
     EXPECT_TRUE(!pluginMrt_->initStatus == pluginMgr_->INIT_SUCCESS);
     EXPECT_TRUE(!pluginMrt_->pluginLibMap_.size() == 1);
}

}

}