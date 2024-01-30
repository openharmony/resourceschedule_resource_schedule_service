/*
 * Copyright (c) 2024-2024 Huawei Device Co., Ltd.
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

#ifdef COMPONENT_SCHED_ENABLE
#include <gtest/gtest.h>

#include "component_sched_plugin.h"

#include"component_sched_client.h"

#include "config_info.h"
#include "plugin_mgr.h"
#include "res_sched_log.h"
#include "res_type.h"

namespace OHOS {
namespace ResourceSchedule {

using namespace testing;
using namespace testing::ext;
using namespace ResType;
using namespace OHOS::ResourceSchedule;

const std::string LIB_NAME = "libcomponent_sched_plugin.z.so";

extern "C" bool OnPluginInit(const std::string& libName);
extern "C" void OnPluginDisable();
extern "C" void OnDispatchResource(const std::shared_ptr<ResData>& data);
class CompSchedPluginTest : public testing::Test {
public:
    static void SetUpTestCase()
    {
        ComponentSchedPlugin::GetInstance().Init();
    }
    static void TearDownTestCase() {}
    void SetUp() {}
    void TearDown() {}
};

/**
 * @tc.name: CompSchedPluginMockTest_001
 * @tc.desc: Test component_sched_plugin method.
 * @tc.type: FUNC
 * @tc.require: SR000H0H5E
 */
HWTEST_F(CompSchedPluginTest, CompSchedPluginMockTest_001, TestSize.Level1)
{
    nlohmann::json payload;
    payload["level"] = "1";
    std::shared_ptr<ResData> resData1 = std::make_shared<ResData>(ResType::RES_TYPE_THERMAL_STATE, 0, payload);
    ComponentSchedPlugin::GetInstance().DispatchResource(resData1);
}

/**
 * @tc.name: CompSchedPluginMockTest_002
 * @tc.desc: Test component_sched_plugin method.
 * @tc.type: FUNC
 * @tc.require: SR000H0H5E
 */
HWTEST_F(CompSchedPluginTest, CompSchedPluginMockTest_002, TestSize.Level1)
{
    EXPECT_FALSE(OnPluginInit("xxx"));
    EXPECT_TRUE(OnPluginInit("libcomponent_sched_plugin.z.so"));
    OnPluginDisable();
    OnDispatchResource(nullptr);
    nlohmann::json payload;
    payload["level"] = "0";
    std::shared_ptr<ResData> resData1 = std::make_shared<ResData>(ResType::RES_TYPE_THERMAL_STATE, 1, payload);
    OnDispatchResource(resData1);
    SUCCEED();
}
} // namespace ComponentScheduler
} // namespace OHOS
#endif
