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
#undef private

namespace OHOS {
namespace ResourceSchedule {
using namespace std;
using namespace testing::ext;

namespace {
}

class SocperfExecutorPluginTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};


void SocperfExecutorPluginTest::SetUpTestCase(void) {}

void SocperfExecutorPluginTest::TearDownTestCase() {}

void SocperfExecutorPluginTest::SetUp() {}

void SocperfExecutorPluginTest::TearDown() {}

/**
* @tc.name: SocperfExecutorPluginTest_API_001
* @tc.desc: test socperf_executor_config api
* @tc.type: FUNC
*/
HWTEST_F(SocperfExecutorPluginTest, SocperfExecutorPluginTest_API_001, Function | MediumTest | Level0)
{
    SocperfExecutorPlugin::GetInstance().Init();
    EXPECT_EQ(SocperfExecutorPlugin::GetInstance().resType_.size(), 1);
    EXPECT_EQ(SocperfExecutorPlugin::GetInstance().functionMap_.size(), 1);
}

/**
* @tc.name: SocperfExecutorPluginTest_API_002
* @tc.desc: test socperf_executor_config api
* @tc.type: FUNC
*/
HWTEST_F(SocperfExecutorPluginTest, SocperfExecutorPluginTest_API_002, Function | MediumTest | Level0)
{
    SocperfExecutorPlugin::GetInstance().resType_.insert(1);
    SocperfExecutorPlugin::GetInstance().functionMap_[1] = [](const std::shared_ptr<ResData>& data){}
    SocperfExecutorPlugin::GetInstance().Disable();
    EXPECT_TRUE(SocperfExecutorPlugin::GetInstance().resType_.empty());
    EXPECT_TRUE(SocperfExecutorPlugin::GetInstance().functionMap_.empty());
}

/**
* @tc.name: SocperfExecutorPluginTest_API_003
* @tc.desc: test socperf_executor_config api
* @tc.type: FUNC
*/
HWTEST_F(SocperfExecutorPluginTest, SocperfExecutorPluginTest_API_003, Function | MediumTest | Level0)
{
    std::shared_ptr<ResData> data = nullptr;
    SocperfExecutorPlugin::GetInstance().DispatchResource(data);

    data = std::make_shared<ResData>();
    data->value = -1;
    SocperfExecutorPlugin::GetInstance().DispatchResource(data);

    data->value = 1;
    data->resType = 1;
    auto func = [](const std::shared_ptr<ResData>& data){};
    SocperfExecutorPlugin::GetInstance().functionMap_[data->resType] = func;
    SocperfExecutorPlugin::GetInstance().DispatchResource(data);
    EXPECT_NE(data, nullptr);
}

/**
* @tc.name: SocperfExecutorPluginTest_API_004
* @tc.desc: test socperf_executor_config api
* @tc.type: FUNC
*/
HWTEST_F(SocperfExecutorPluginTest, SocperfExecutorPluginTest_API_004, Function | MediumTest | Level0)
{
    std::shared_ptr<ResData> data = nullptr;
    SocperfExecutorPlugin::GetInstance().HandleSocperfWirteNode(data);

    data = std::make_shared<ResData>();
    data->value = 0;
    SocperfExecutorPlugin::GetInstance().HandleSocperfWirteNode(data);

    data->value = SOCPERF_EVENT_WIRTE_NODE;
    SocperfExecutorPlugin::GetInstance().HandleSocperfWirteNode(data);
    EXPECT_NE(data, nullptr);
}

/**
* @tc.name: SocperfExecutorPluginTest_API_005
* @tc.desc: test socperf_executor_config api
* @tc.type: FUNC
*/
HWTEST_F(SocperfExecutorPluginTest, SocperfExecutorPluginTest_API_005, Function | MediumTest | Level0)
{
    SocperfExecutorPlugin::GetInstance().SocPerfInitNode();
    std::shared_ptr<ResData> data = std::make_shared<ResData>();
    EXPECT_NE(data, nullptr);
}

/**
* @tc.name: SocperfExecutorPluginTest_API_006
* @tc.desc: test socperf_executor_config api
* @tc.type: FUNC
*/
HWTEST_F(SocperfExecutorPluginTest, SocperfExecutorPluginTest_API_006, Function | MediumTest | Level0)
{
    std::shared_ptr<ResData> data = nullptr;
    SocperfExecutorPlugin::GetInstance().SocPerfWirteNode(data);

    data = std::make_shared<ResData>();
    data->value = -1;
    SocperfExecutorPlugin::GetInstance().SocPerfWirteNode(data);

    data->value = 1;
    data->payload = {{"QOSID_STRING", {1, 2, 3}}, {"VALUE_STRING", {10, 20, 30}}};
    SocperfExecutorPlugin::GetInstance().SocPerfWirteNode(data);

    data->payload = {{"QOSID_STRING", {1, 2, 3}}, {"VALUE_STRING", {10}}};
    SocperfExecutorPlugin::GetInstance().SocPerfWirteNode(data);
    EXPECT_NE(data, nullptr);
}
} // namespace ResourceSchedule
} // namespace OHOS
