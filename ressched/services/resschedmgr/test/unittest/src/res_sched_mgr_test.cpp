/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: ressched manager unnittest
 */

#include "res_sched_mgr_test.h"

using namespace std;
using namespace testing::ext;

namespace OHOS {
namespace ResourceSchedule {
namespace {
    const string LIB_NAME = "libunittestplugin_cplusplus.z.so";
}

void ResSchedMgrTest::SetUpTestCase() {}

void ResSchedMgrTest::TearDownTestCase() {}

void ResSchedMgrTest::SetUp()
{
    /**
     * @tc.setup: initialize the member variable pluginMgr_
     */
}

void ResSchedMgrTest::TearDown()
{
    /**
     * @tc.teardown: clear pluginMgr_
     */
}

/**
 * @tc.name: Init plugin mgr 001
 * @tc.desc: Verify if can Init the plugin correctly
 * @tc.type: FUNC
 * @tc.require: AR000001
 * @tc.author:xukuan
 */
HWTEST_F(ResSchedMgrTest, Init001, TestSize.Level1)
{

     ResSchedMgr::GetInstance().Init();
     ResSchedMgr::GetInstance().ReportData(ResType::RES_TYPE_SCREEN_STATUS, 0, "11")
     EXPECT_TRUE(ResSchedMgr::GetInstance().mainHandler_ != nullptr);
}
}
}