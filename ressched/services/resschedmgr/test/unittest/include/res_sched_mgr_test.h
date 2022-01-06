/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: ressched manager unnittest
 */


#ifndef FOUNDATION_RESOURCESCHEDULE_SERVICES_RESSCHEDMGR_TEST_UNITTEST_INCLUDE_RES_SCHED_MGR_TEST_H
#define FOUNDATION_RESOURCESCHEDULE_SERVICES_RESSCHEDMGR_TEST_UNITTEST_INCLUDE_RES_SCHED_MGR_TEST_H

#include <memory>
#include "gtest/gtest.h"
#define private public
#include "plugin_mgr.h"
#include "res_type.h"
#include "res_sched_mgr"
#undef private

namespace OHOS {
    namespace ResourceSchedule {
        class ResSchedMgrTest : public testing::Test {
        public:
            static void SetUpTestCase();
            static void TearDownTestCase();
            void SetUp();
            void TearDown();
        protected:
            std::shared_ptr<ResData> ReportData(uinit32_t resType);
        };
    }// namespace ResourceSchedule
}// namespace OHOS

#endif //FOUNDATION_RESOURCESCHEDULE_SERVICES_RESSCHEDMGR_TEST_UNITTEST_INCLUDE_RES_SCHED_MGR_TEST_H
