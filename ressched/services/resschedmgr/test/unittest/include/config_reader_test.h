/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: config reader unnittest
 */


#ifndef FOUNDATION_RESOURCESCHEDULE_SERVICES_RESSCHEDMGR_TEST_UNITTEST_INCLUDE_CONFIG_READER_TEST_H
#define FOUNDATION_RESOURCESCHEDULE_SERVICES_RESSCHEDMGR_TEST_UNITTEST_INCLUDE_CONFIG_READER_TEST_H

#include <map>
#include <memory>
#include "gtest/gtest.h"
#include "config_reader.h"

namespace OHOS {
namespace ResourceSchedule {
class ConfigReaderTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
protected:
    std::shared_ptr<ConfigReader> configReader_;

};
}// namespace ResourceSchedule
}// namespace OHOS

#endif //FOUNDATION_RESOURCESCHEDULE_SERVICES_RESSCHEDMGR_TEST_UNITTEST_INCLUDE_CONFIG_READER_TEST_H
