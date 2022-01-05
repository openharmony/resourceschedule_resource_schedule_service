/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: config reader unnittest
 */


#ifndef FOUNDATION_RESOURCESCHEDULE_SERVICES_RESSCHEDMGR_TEST_UNITTEST_INCLUDE_PLUGIN_MGR_TEST_H
#define FOUNDATION_RESOURCESCHEDULE_SERVICES_RESSCHEDMGR_TEST_UNITTEST_INCLUDE_PLUGIN_MGR_TEST_H

#include "gtest/gtest.h"
#include "plugin_mgr.h"
#include "mock.h"

namespace OHOS {
namespace ResourceSchedule {
class PluginMgrTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
protected:
    std::shared_ptr<ConfigReader> pluginMgr_;

};
}// namespace ResourceSchedule
}// namespace OHOS

#endif //FOUNDATION_RESOURCESCHEDULE_SERVICES_RESSCHEDMGR_TEST_UNITTEST_INCLUDE_PLUGIN_MGR_TEST_H
