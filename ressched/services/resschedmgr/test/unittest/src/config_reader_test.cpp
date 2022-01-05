/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: config reader unnittest
 */

#include "config_reader_test.h"

using namespace std;
using namespace testing::ext;

namespace OHOS {
namespace ResourceSchedule {
namespace {
const string PLUGIN_NAME = "demo";
const string CONIFG_NAME = "sample";
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