/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: plugin switch unnittest
 */


#ifndef FOUNDATION_RESOURCESCHEDULE_SERVICES_RESSCHEDMGR_TEST_UNITTEST_INCLUDE_PLUGIN_SWITCH_TEST_H
#define FOUNDATION_RESOURCESCHEDULE_SERVICES_RESSCHEDMGR_TEST_UNITTEST_INCLUDE_PLUGIN_SWITCH_TEST_H

#include <map>
#include <memory>
#include "gtest/gtest.h"
#define private public
#include "plugin_switch.h"
#undef private

namespace OHOS {
    namespace ResourceSchedule {
        class PluginSwitchTest : public testing::Test {
        public:
            static void SetUpTestCase();
            static void TearDownTestCase();
            void SetUp();
            void TearDown();
        protected:
            std::shared_ptr<PluginSwitch> pluginSwitch_;
        };
    }// namespace ResourceSchedule
}// namespace OHOS

#endif //FOUNDATION_RESOURCESCHEDULE_SERVICES_RESSCHEDMGR_TEST_UNITTEST_INCLUDE_PLUGIN_SWITCH_TEST_H
