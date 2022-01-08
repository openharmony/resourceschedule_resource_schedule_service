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

#include "config_reader_test.h"

using namespace std;
using namespace testing::ext;

namespace OHOS {
namespace ResourceSchedule {
namespace {
    const string PLUGIN_NAME = "demo";
    const string CONFIG_NAME = "sample";
    const string TEST_PREFIX_RES_PATH = "/data/test/resource/resschedfwk/parseconfig/";
}

void ConfigReaderTest::SetUpTestCase() {}

void ConfigReaderTest::TearDownTestCase() {}

void ConfigReaderTest::SetUp()
{
    /**
     * @tc.setup: initialize the member variable configReader_
     */
    configReader_ = make_shared<ConfigReader>();
}

void ConfigReaderTest::TearDown()
{
    /**
     * @tc.teardown: clear configReader_
     */
    configReader_ = nullptr;
}

/**
 * @tc.name: Load Config File 001
 * @tc.desc: Verify if can load not exist file.
 * @tc.type: FUNC
 * @tc.require: AR000001
 * @tc.author:xukuan
 */
HWTEST_F(ConfigReaderTest, LoadConfigFile001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. load not exist config file
     * @tc.expected: step1. return false when load not exist file
     */
     bool ret = configReader_->LoadFromCustConfigFile("fileNotExist");
     EXPECT_TRUE(!ret);
}

}

}