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

#ifndef RESSCHED_SERVICES_RESSCHEDMGR_TEST_UNITTEST_INCLUDE_CONFIG_READER_TEST_H
#define RESSCHED_SERVICES_RESSCHEDMGR_TEST_UNITTEST_INCLUDE_CONFIG_READER_TEST_H

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
    std::shared_ptr<ConfigReader> configReader_ = nullptr;
    bool ParseConfigFile(const std::string& fileName);
};
} // namespace ResourceSchedule
} // namespace OHOS

#endif // RESSCHED_SERVICES_RESSCHEDMGR_TEST_UNITTEST_INCLUDE_CONFIG_READER_TEST_H
