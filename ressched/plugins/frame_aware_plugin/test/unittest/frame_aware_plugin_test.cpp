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

#include "gtest/gtest.h"
#include "res_type.h"
#include "frame_aware_plugin.h"

namespace OHOS {
namespace ResourceSchedule {
using namespace testing;
using namespace testing::ext;
using namespace ResType;

class FrameAwarePluginTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void FrameAwarePluginTest::SetUpTestCase()
{
}

void FrameAwarePluginTest::TearDownTestCase()
{
}

void FrameAwarePluginTest::SetUp()
{
    FrameAwarePlugin::GetInstance().Init();
}

void FrameAwarePluginTest::TearDown()
{
    FrameAwarePlugin::GetInstance().Disable();
}


/**
 * @tc.name: HandleAppStateChangeTest
 * @tc.desc: Test whether HandleAppStateChange interface is normal
 * @tc.type: FUNC
 */
HWTEST_F(FrameAwarePluginTest, HandleAppStateChangeTest, TestSize.Level1)
{
    uint32_t type = 0;
    int64_t value = 0;
    nlohmann::json payload;
    nlohmann::json payloadarray;
    payload["test"] = false;
    payloadarray[0] = false;
    std::shared_ptr<ResData> data = std::make_shared<ResData>(type, value, payload);
    std::shared_ptr<ResData> data1 = std::make_shared<ResData>(type, value, payloadarray);
    data->resType = RES_TYPE_APP_STATE_CHANGE;
    data->value = 0;
    data1->resType = RES_TYPE_APP_STATE_CHANGE;
    data1->value = 0;

    FrameAwarePlugin::GetInstance().DispatchResource(data1); // not object
    FrameAwarePlugin::GetInstance().DispatchResource(data); // not contain pid
    data->payload.clear();
    data->payload["pid"] = true;
    FrameAwarePlugin::GetInstance().DispatchResource(data); // not contain uid
    data->payload.clear();
    data->payload["pid"] = true;
    data->payload["uid"] = true;
    FrameAwarePlugin::GetInstance().DispatchResource(data); // not contain bundleName
    data->payload["bundleName"] = true;
    FrameAwarePlugin::GetInstance().DispatchResource(data); // pid is not string
    data->payload["pid"] = "12345";
    FrameAwarePlugin::GetInstance().DispatchResource(data); // uid is not string
    data->payload["uid"] = "12345";
    FrameAwarePlugin::GetInstance().DispatchResource(data); // bundleName is not string
    data->payload["bundleName"] = "test";
    FrameAwarePlugin::GetInstance().DispatchResource(data); // all right
}

/**
 * @tc.name: HandleProcessStateChangeTest
 * @tc.desc: Test whether HandleProcessStateChange interface is normal
 * @tc.type: FUNC
 */
HWTEST_F(FrameAwarePluginTest, HandleProcessStateChangeTest, TestSize.Level1)
{
    uint32_t type = 0;
    int64_t value = 0;
    nlohmann::json payload;
    nlohmann::json payloadarray;
    payload["test"] = false;
    payloadarray[0] = false;
    std::shared_ptr<ResData> data = std::make_shared<ResData>(type, value, payload);
    std::shared_ptr<ResData> data1 = std::make_shared<ResData>(type, value, payloadarray);
    data->resType = RES_TYPE_PROCESS_STATE_CHANGE;
    data->value = 0;
    data1->resType = RES_TYPE_PROCESS_STATE_CHANGE;
    data1->value = 0;

    FrameAwarePlugin::GetInstance().DispatchResource(data1); // not object
    FrameAwarePlugin::GetInstance().DispatchResource(data); // not contain pid
    data->payload.clear();
    data->payload["pid"] = true;
    FrameAwarePlugin::GetInstance().DispatchResource(data); // not contain uid
    data->payload.clear();
    data->payload["pid"] = true;
    data->payload["uid"] = true;
    FrameAwarePlugin::GetInstance().DispatchResource(data); // not contain bundleName
    data->payload["bundleName"] = true;
    FrameAwarePlugin::GetInstance().DispatchResource(data); // pid is not string
    data->payload["pid"] = "12345";
    FrameAwarePlugin::GetInstance().DispatchResource(data); // uid is not string
    data->payload["uid"] = "12345";
    FrameAwarePlugin::GetInstance().DispatchResource(data); // bundleName is not string
    data->payload["bundleName"] = "test";
    FrameAwarePlugin::GetInstance().DispatchResource(data); // all right
}

/**
 * @tc.name: HandleCgroupAdjusterTest
 * @tc.desc: Test whether HandleCgroupAdjuster interface is normal
 * @tc.type: FUNC
 */
HWTEST_F(FrameAwarePluginTest, HandleCgroupAdjusterTest, TestSize.Level1)
{
    uint32_t type = 0;
    int64_t value = 0;
    nlohmann::json payload;
    nlohmann::json payloadarray;
    payload["test"] = false;
    payloadarray[0] = false;
    std::shared_ptr<ResData> data = std::make_shared<ResData>(type, value, payload);
    std::shared_ptr<ResData> data1 = std::make_shared<ResData>(type, value, payloadarray);
    data->resType = RES_TYPE_CGROUP_ADJUSTER;
    data->value = 0;
    data1->resType = RES_TYPE_CGROUP_ADJUSTER;
    data1->value = 0;

    FrameAwarePlugin::GetInstance().DispatchResource(data1); // not object
    FrameAwarePlugin::GetInstance().DispatchResource(data); // not contain pid
    data->payload.clear();
    data->payload["pid"] = true;
    FrameAwarePlugin::GetInstance().DispatchResource(data); // not contain uid
    data->payload.clear();
    data->payload["pid"] = true;
    data->payload["uid"] = true;
    FrameAwarePlugin::GetInstance().DispatchResource(data); // pid is not string
    data->payload["pid"] = "12345";
    FrameAwarePlugin::GetInstance().DispatchResource(data); // uid is not string
    data->payload["uid"] = "12345";
    FrameAwarePlugin::GetInstance().DispatchResource(data); // not contain oldGroup
    data->payload.clear();
    data->payload["pid"] = "12345";
    data->payload["uid"] = "12345";
    data->payload["oldGroup"] = true;
    FrameAwarePlugin::GetInstance().DispatchResource(data); // not contain newGroup
    data->payload["newGroup"] = true;
    FrameAwarePlugin::GetInstance().DispatchResource(data); // oldGroup is not string
    data->payload["oldGroup"] = "12345";
    FrameAwarePlugin::GetInstance().DispatchResource(data); // newGroup is not string
    data->payload["newGroup"] = "12345";
    FrameAwarePlugin::GetInstance().DispatchResource(data); // data->value = 0
    data->value = 1;
    data->payload["newGroup"] = "12345";
    FrameAwarePlugin::GetInstance().DispatchResource(data); // data->value = 1
}

/**
 * @tc.name: HandleWindowsFocusTest
 * @tc.desc: Test whether HandleWindowsFocus interface is normal
 * @tc.type: FUNC
 */
HWTEST_F(FrameAwarePluginTest, HandleWindowsFocusTest, TestSize.Level1)
{
    uint32_t type = 0;
    int64_t value = 0;
    nlohmann::json payload;
    nlohmann::json payloadarray;
    payload["test"] = false;
    payloadarray[0] = false;
    std::shared_ptr<ResData> data = std::make_shared<ResData>(type, value, payload);
    std::shared_ptr<ResData> data1 = std::make_shared<ResData>(type, value, payloadarray);
    data->resType = RES_TYPE_WINDOW_FOCUS;
    data->value = 0;
    data1->resType = RES_TYPE_WINDOW_FOCUS;
    data1->value = 0;

    FrameAwarePlugin::GetInstance().DispatchResource(data1); // not object
    FrameAwarePlugin::GetInstance().DispatchResource(data); // not contain pid
    data->payload.clear();
    data->payload["pid"] = true;
    FrameAwarePlugin::GetInstance().DispatchResource(data); // not contain uid
    data->payload["uid"] = true;
    FrameAwarePlugin::GetInstance().DispatchResource(data); // pid is not string
    data->payload["pid"] = "12345";
    FrameAwarePlugin::GetInstance().DispatchResource(data); // uid is not string
    data->payload["uid"] = "12345";
    FrameAwarePlugin::GetInstance().DispatchResource(data); // all right
}

/**
 * @tc.name: HandleContinuousTest
 * @tc.desc: Test whether HandleContinuous interface is normal
 * @tc.type: FUNC
 */
HWTEST_F(FrameAwarePluginTest, HandleContinuousTest, TestSize.Level1)
{
    uint32_t type = 0;
    int64_t value = 0;
    nlohmann::json payload;
    nlohmann::json payloadarray;
    payload["test"] = false;
    payloadarray[0] = false;
    std::shared_ptr<ResData> data = std::make_shared<ResData>(type, value, payload);
    std::shared_ptr<ResData> data1 = std::make_shared<ResData>(type, value, payloadarray);
    data->resType = RES_TYPE_CONTINUOUS_TASK;
    data->value = 0;
    data1->resType = RES_TYPE_CONTINUOUS_TASK;
    data1->value = 0;

    FrameAwarePlugin::GetInstance().DispatchResource(data1); // not object
    FrameAwarePlugin::GetInstance().DispatchResource(data); // not contain pid
    data->payload.clear();
    data->value = 0;
    data->payload["pid"] = true;
    FrameAwarePlugin::GetInstance().DispatchResource(data); // not contain uid
    data->payload["uid"] = true;
    FrameAwarePlugin::GetInstance().DispatchResource(data); // pid is not string
    data->payload["pid"] = "12345";
    FrameAwarePlugin::GetInstance().DispatchResource(data); // uid is not string
    data->payload["uid"] = "12345";
    FrameAwarePlugin::GetInstance().DispatchResource(data); // all right
}

/**
 * @tc.name: HandleReportRenderTest
 * @tc.desc: Test whether HandleReportRender interface is normal
 * @tc.type: FUNC
 */
HWTEST_F(FrameAwarePluginTest, HandleReportRenderTest, TestSize.Level1)
{
    uint32_t type = 0;
    int64_t value = 0;
    nlohmann::json payload;
    nlohmann::json payloadarray;
    payload["test"] = false;
    payloadarray[0] = false;
    std::shared_ptr<ResData> data = std::make_shared<ResData>(type, value, payload);
    std::shared_ptr<ResData> data1 = std::make_shared<ResData>(type, value, payloadarray);
    data->resType = RES_TYPE_REPORT_RENDER_THREAD;
    data->value = 0;
    data1->resType = RES_TYPE_REPORT_RENDER_THREAD;
    data1->value = 0;

    FrameAwarePlugin::GetInstance().DispatchResource(data1); // not object
    FrameAwarePlugin::GetInstance().DispatchResource(data); // not contain pid
    data->payload.clear();
    data->payload["pid"] = true;
    FrameAwarePlugin::GetInstance().DispatchResource(data); // not contain uid
    data->payload["uid"] = true;
    FrameAwarePlugin::GetInstance().DispatchResource(data); // pid is not string
    data->payload["pid"] = "12345";
    FrameAwarePlugin::GetInstance().DispatchResource(data); // uid is not string
    data->payload["uid"] = "12345";
    FrameAwarePlugin::GetInstance().DispatchResource(data); // all right
}

/**
 * @tc.name: HandleNetworkLatencyRequestTest
 * @tc.desc: Test whether HandleNetworkLatencyRequest interface is normal
 * @tc.type: FUNC
 */
HWTEST_F(FrameAwarePluginTest, HandleNetworkLatencyRequestTest, TestSize.Level1)
{
    uint32_t type = 0;
    int64_t value = 0;
    nlohmann::json payload;
    nlohmann::json payloadarray;
    payload["test"] = false;
    payloadarray[0] = false;
    std::shared_ptr<ResData> data = std::make_shared<ResData>(type, value, payload);
    std::shared_ptr<ResData> data1 = std::make_shared<ResData>(type, value, payloadarray);
    data->resType = RES_TYPE_NETWORK_LATENCY_REQUEST;
    data->value = 0;
    data1->resType = RES_TYPE_NETWORK_LATENCY_REQUEST;
    data1->value = 0;

    FrameAwarePlugin::GetInstance().DispatchResource(data1); // is not object
    FrameAwarePlugin::GetInstance().DispatchResource(data); // not contain clientPid
    data->payload.clear();
    data->payload["clientPid"] = true;
    FrameAwarePlugin::GetInstance().DispatchResource(data); // clientPid is not string
    data->payload["clientPid"] = "12345";
    FrameAwarePlugin::GetInstance().DispatchResource(data); // not contain identity
    data->payload["identity"] = true;
    FrameAwarePlugin::GetInstance().DispatchResource(data); // identity is not string
    data->payload["identity"] = "12345";
    FrameAwarePlugin::GetInstance().DispatchResource(data); // all right
}
} // namespace ResourceSchedule
} // namespace OHOS
