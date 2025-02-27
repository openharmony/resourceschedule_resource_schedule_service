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

#include "nativetoken_kit.h"
#include "token_setproc.h"
#define private public
#include "socperf_executor_plugin.h"
#undef private

namespace OHOS {
namespace ResourceSchedule {
using namespace std;
using namespace testing::ext;

namespace {
    constexpr int32_t SYNC_THREAD_NUM = 100;
    constexpr int32_t SYNC_INTERNAL_TIME = 200;
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
    SocperfExecutorPlugin plugin;
    plugin.resType_.insert(1);
    Plugin.functionMap_[1] = [](const std::shared_ptr<ResData>& data){}
    plugin.Disable();
    EXPECT_TRUE(plugin.resType_.empty());
    EXPECT_TRUE(plugin.functionMap_.empty());
}

/**
* @tc.name: SocperfExecutorPluginTest_API_003
* @tc.desc: test socperf_executor_config api
* @tc.type: FUNC
*/
HWTEST_F(SocperfExecutorPluginTest, SocperfExecutorPluginTest_API_003, Function | MediumTest | Level0)
{
    SocperfExecutorPlugin plugin;
    std::shared_ptr<ResData> data = nullptr;
    plugin.DispatchResource(data);

    data = std::shared<ResData>();
    data->value = -1;
    plugin.DispatchResource(data);

    data->value = 1;
    data->resType = 1;
    auto func = [](const std::shared_ptr<ResData>& data){};
    plugin.functionMap_[data->resType] = func;
    plugin.DispatchResource(data);
    EXPECT_NE(data, nullptr);
}

/**
* @tc.name: SocperfExecutorPluginTest_API_004
* @tc.desc: test socperf_executor_config api
* @tc.type: FUNC
*/
HWTEST_F(SocperfExecutorPluginTest, SocperfExecutorPluginTest_API_004, Function | MediumTest | Level0)
{
    SocperfExecutorPlugin plugin;
    std::shared_ptr<ResData> data = nullptr;
    plugin.HandleSocperfWirteNode(data);

    data = std::shared<ResData>();
    data->value = 0;
    plugin.HandleSocperfWirteNode(data);

    data->value = SOCPERF_EVENT_WIRTE_NODE;
    ExPECT_CALL(plugin, SocPerfWirteNode(data)).Times(1);
    plugin.HandleSocperfWirteNode(data);
    EXPECT_NE(data, nullptr);
}

/**
* @tc.name: SocperfExecutorPluginTest_API_005
* @tc.desc: test socperf_executor_config api
* @tc.type: FUNC
*/
HWTEST_F(SocperfExecutorPluginTest, SocperfExecutorPluginTest_API_005, Function | MediumTest | Level0)
{
    SocperfExecutorPlugin plugin;
    plugin.SocPerfInitNode();
    EXPECT_NE(plugin, nullptr);
}

/**
* @tc.name: SocperfExecutorPluginTest_API_006
* @tc.desc: test socperf_executor_config api
* @tc.type: FUNC
*/
HWTEST_F(SocperfExecutorPluginTest, SocperfExecutorPluginTest_API_006, Function | MediumTest | Level0)
{
    SocperfExecutorPlugin plugin;
    std::shared_ptr<ResData> data = nullptr;
    plugin.SocPerfWirteNode(data);

    data = std::shared<ResData>();
    data->value = -1;
    plugin.SocPerfWirteNode(data);

    data->value = 1;
    data->payload = {{"QOSID_STRING", {1, 2, 3}}, {"VALUE_STRING", {10, 20, 30}}};
    plugin.SocPerfWirteNode(data);

    data->payload = {{"QOSID_STRING", {1, 2, 3}}, {"VALUE_STRING", {10}}};
    plugin.SocPerfWirteNode(data);
    EXPECT_NE(data, nullptr);
}
} // namespace ResourceSchedule
} // namespace OHOS
