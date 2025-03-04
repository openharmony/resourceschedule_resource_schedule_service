/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "gtest/gtest.h"

#include <unordered_map>
#include <vector>

#define private public
#include "socperf_executor_plugin.h"
#include "socperf_executor_wirte_node.h"
#undef private

namespace OHOS {
namespace ResourceSchedule {
using namespace std;
using namespace testing::ext;

namespace {
}

class SocPerfExecutorPluginTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};


void SocPerfExecutorPluginTest::SetUpTestCase(void) {}

void SocPerfExecutorPluginTest::TearDownTestCase() {}

void SocPerfExecutorPluginTest::SetUp() {}

void SocPerfExecutorPluginTest::TearDown() {}

/**
* @tc.name: SocPerfExecutorPluginTest_API_001
* @tc.desc: test socperf_executor_config api
* @tc.type: FUNC
*/
HWTEST_F(SocPerfExecutorPluginTest, SocPerfExecutorPluginTest_API_001, Function | MediumTest | Level0)
{
    SocPerfExecutorPlugin::GetInstance().Init();
    EXPECT_EQ(SocPerfExecutorPlugin::GetInstance().resType_.size(), 1);
    EXPECT_EQ(SocPerfExecutorPlugin::GetInstance().functionMap_.size(), 1);
}

/**
* @tc.name: SocPerfExecutorPluginTest_API_002
* @tc.desc: test socperf_executor_config api
* @tc.type: FUNC
*/
HWTEST_F(SocPerfExecutorPluginTest, SocPerfExecutorPluginTest_API_002, Function | MediumTest | Level0)
{
    SocPerfExecutorPlugin::GetInstance().resType_.insert(1);
    SocPerfExecutorPlugin::GetInstance().functionMap_[1] = [](const std::shared_ptr<ResData>& data){};
    SocPerfExecutorPlugin::GetInstance().Disable();
    EXPECT_TRUE(SocPerfExecutorPlugin::GetInstance().resType_.empty());
    EXPECT_TRUE(SocPerfExecutorPlugin::GetInstance().functionMap_.empty());
}

/**
* @tc.name: SocPerfExecutorPluginTest_API_003
* @tc.desc: test socperf_executor_config api
* @tc.type: FUNC
*/
HWTEST_F(SocPerfExecutorPluginTest, SocPerfExecutorPluginTest_API_003, Function | MediumTest | Level0)
{
    std::shared_ptr<ResData> data = nullptr;
    SocPerfExecutorPlugin::GetInstance().DispatchResource(data);

    data = std::make_shared<ResData>();
    data->value = -1;
    SocPerfExecutorPlugin::GetInstance().DispatchResource(data);

    data->value = 1;
    data->resType = 1;
    auto func = [](const std::shared_ptr<ResData>& data){};
    SocPerfExecutorPlugin::GetInstance().functionMap_[data->resType] = func;
    SocPerfExecutorPlugin::GetInstance().DispatchResource(data);
    EXPECT_NE(data, nullptr);
}

/**
* @tc.name: SocPerfExecutorPluginTest_API_004
* @tc.desc: test socperf_executor_config api
* @tc.type: FUNC
*/
HWTEST_F(SocPerfExecutorPluginTest, SocPerfExecutorPluginTest_API_004, Function | MediumTest | Level0)
{
    std::shared_ptr<ResData> data = nullptr;
    SocPerfExecutorPlugin::GetInstance().HandleSocperfWirteNode(data);

    data = std::make_shared<ResData>();
    data->value = 0;
    SocPerfExecutorPlugin::GetInstance().HandleSocperfWirteNode(data);

    data->value = SOCPERF_EVENT_WIRTE_NODE;
    SocPerfExecutorPlugin::GetInstance().HandleSocperfWirteNode(data);
    EXPECT_NE(data, nullptr);
}

/**
* @tc.name: SocPerfExecutorPluginTest_API_005
* @tc.desc: test socperf_executor_config api
* @tc.type: FUNC
*/
HWTEST_F(SocPerfExecutorPluginTest, SocPerfExecutorPluginTest_API_005, Function | MediumTest | Level0)
{
    SocPerfExecutorPlugin::GetInstance().SocPerfInitNode();
    std::shared_ptr<ResData> data = std::make_shared<ResData>();
    EXPECT_NE(data, nullptr);
}

/**
* @tc.name: SocPerfExecutorPluginTest_API_006
* @tc.desc: test socperf_executor_config api
* @tc.type: FUNC
*/
HWTEST_F(SocPerfExecutorPluginTest, SocPerfExecutorPluginTest_API_006, Function | MediumTest | Level0)
{
    std::shared_ptr<ResData> data = nullptr;
    SocPerfExecutorPlugin::GetInstance().SocPerfWirteNode(data);

    data = std::make_shared<ResData>();
    data->value = -1;
    SocPerfExecutorPlugin::GetInstance().SocPerfWirteNode(data);

    data->value = 1;
    data->payload = {{"QOSID_STRING", {1, 2, 3}}, {"VALUE_STRING", {10, 20, 30}}};
    SocPerfExecutorPlugin::GetInstance().SocPerfWirteNode(data);

    data->payload = {{"QOSID_STRING", {1, 2, 3}}, {"VALUE_STRING", {10}}};
    SocPerfExecutorPlugin::GetInstance().SocPerfWirteNode(data);
    EXPECT_NE(data, nullptr);
}
} // namespace ResourceSchedule
} // namespace OHOS
