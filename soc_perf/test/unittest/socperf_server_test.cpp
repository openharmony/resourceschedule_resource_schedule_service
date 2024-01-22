/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#define private public
#define protected public

#include <gtest/gtest.h>
#include <gtest/hwext/gtest-multithread.h>
#include "socperf_ipc_interface_code.h"
#include "socperf_server.h"
#include "socperf_stub.h"
#include "socperf.h"

using namespace testing::ext;
using namespace testing::mt;

namespace OHOS {
namespace SOCPERF {
class SocPerfServerTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
private:
    std::shared_ptr<SocPerfServer> socPerfServer_;
    SocPerf socPerf_;
};

void SocPerfServerTest::SetUpTestCase(void)
{
}

void SocPerfServerTest::TearDownTestCase(void)
{
}

void SocPerfServerTest::SetUp(void)
{
    socPerfServer_ = DelayedSingleton<SocPerfServer>::GetInstance();
    socPerfServer_->OnStart();

    socPerf_.Init();
}

void SocPerfServerTest::TearDown(void)
{
    socPerfServer_ = nullptr;
}

/*
 * @tc.name: SocPerfServerTest_SocPerfAPI_001
 * @tc.desc: test socperf api
 * @tc.type FUNC
 * @tc.require: issueI78T3V
 */
HWTEST_F(SocPerfServerTest, SocPerfServerTest_SocPerfAPI_001, Function | MediumTest | Level0)
{
    std::string msg = "testBoost";
    socPerf_.PerfRequest(10000, msg);
    socPerf_.PerfRequestEx(10000, true, msg);
    socPerf_.PerfRequestEx(10000, false, msg);
    socPerf_.PerfRequestEx(10028, true, msg);
    socPerf_.PerfRequestEx(10028, false, msg);
    socPerf_.LimitRequest(ActionType::ACTION_TYPE_POWER, {1001}, {999000}, msg);
    socPerf_.LimitRequest(ActionType::ACTION_TYPE_THERMAL, {1001}, {999000}, msg);
    socPerf_.LimitRequest(ActionType::ACTION_TYPE_POWER, {1001}, {1325000}, msg);
    socPerf_.LimitRequest(ActionType::ACTION_TYPE_THERMAL, {1001}, {1325000}, msg);
    socPerf_.PowerLimitBoost(true, msg);
    socPerf_.ThermalLimitBoost(true, msg);
    EXPECT_EQ(msg, "testBoost");
    std::string id = "1000";
    std::string name = "lit_cpu_freq";
    std::string pair = "1001";
    std::string mode = "1";
    std::string persisMode = "1";
    std::string configFile = "";
    bool ret = socPerf_.CheckResourceTag(id.c_str(), name.c_str(), pair.c_str(), mode.c_str(), persisMode.c_str(),
        configFile.c_str());
    EXPECT_TRUE(ret);
    ret = socPerf_.CheckResourceTag(nullptr, name.c_str(), pair.c_str(), mode.c_str(), persisMode.c_str(),
        configFile.c_str());
    EXPECT_FALSE(ret);
}

/*
 * @tc.name: SocPerfServerTest_SocPerfServerAPI_000
 * @tc.desc: test socperf server api
 * @tc.type FUNC
 * @tc.require: issueI78T3V
 */
HWTEST_F(SocPerfServerTest, SocPerfServerTest_SocPerfServerAPI_000, Function | MediumTest | Level0)
{
    std::string msg = "testBoost";
    socPerfServer_->PerfRequest(10000, msg);
    socPerfServer_->PerfRequestEx(10000, true, msg);
    socPerfServer_->PerfRequestEx(10000, false, msg);
    socPerfServer_->LimitRequest(ActionType::ACTION_TYPE_POWER, {1001}, {1364000}, msg);
    socPerfServer_->LimitRequest(ActionType::ACTION_TYPE_POWER, {11001}, {2}, msg);
    socPerfServer_->LimitRequest(ActionType::ACTION_TYPE_MAX, {11001}, {2}, msg);
    socPerfServer_->PowerLimitBoost(true, msg);
    socPerfServer_->LimitRequest(ActionType::ACTION_TYPE_THERMAL, {1001}, {1364000}, msg);
    socPerfServer_->ThermalLimitBoost(true, msg);
    socPerfServer_->PowerLimitBoost(false, msg);
    socPerfServer_->ThermalLimitBoost(false, msg);
    bool allowDump = socPerfServer_->AllowDump();
    EXPECT_TRUE(allowDump);
    int32_t fd = -1;
    std::vector<std::u16string> args = {to_utf16("1"), to_utf16("2"), to_utf16("3"), to_utf16("-a"), to_utf16("-h")};
    socPerfServer_->Dump(fd, args);
    socPerfServer_->OnStop();
    EXPECT_EQ(msg, "testBoost");
}

/*
 * @tc.name: SocPerfServerTest_SocperfThreadWrapp_001
 * @tc.desc: test log switch func
 * @tc.type FUNC
 * @tc.require: issueI78T3V
 */
HWTEST_F(SocPerfServerTest, SocPerfServerTest_SocperfThreadWrapp_001, Function | MediumTest | Level0)
{
    std::string msg = "";
#ifdef SOCPERF_ADAPTOR_FFRT
    auto socPerfThreadWrap = std::make_shared<SocPerfThreadWrap>();
#else
    auto runner = AppExecFwk::EventRunner::Create("socperf#");
    auto socPerfThreadWrap = std::make_shared<SocPerfThreadWrap>(runner);
#endif
    socPerfThreadWrap->PostDelayTask(1000, nullptr);
    socPerfThreadWrap->InitResNodeInfo(nullptr);
    socPerfThreadWrap->InitPerfFunc(nullptr, nullptr);
    socPerfThreadWrap->InitPerfFunc(nullptr, msg.c_str());
    socPerfThreadWrap->InitPerfFunc(msg.c_str(), nullptr);
    socPerfThreadWrap->InitPerfFunc(msg.c_str(), msg.c_str());
    socPerfThreadWrap->InitGovResNodeInfo(nullptr);
    socPerfThreadWrap->DoFreqActionPack(nullptr);
    socPerfThreadWrap->UpdateLimitStatus(0, nullptr, 0);
    socPerfThreadWrap->DoFreqAction(0, nullptr);
    socPerfThreadWrap->DoFreqAction(1000, nullptr);
    EXPECT_NE(msg.c_str(), "-1");
    bool ret = false;
    int inValidResId = 9999;
    ret = socPerfThreadWrap->IsResId(inValidResId);
    EXPECT_FALSE(ret);
    ret = socPerfThreadWrap->IsValidResId(inValidResId);
    EXPECT_FALSE(ret);
    ret = socPerfThreadWrap->IsGovResId(inValidResId);
    EXPECT_FALSE(ret);
    int32_t fd = socPerfThreadWrap->GetFdForFilePath("");
    EXPECT_TRUE(fd < 0);
    int32_t level = 10;
    int64_t value = 0;
    ret = socPerfThreadWrap->GetResValueByLevel(inValidResId, level, value);
    EXPECT_FALSE(ret);
}

class SocperfStubTest : public SocPerfStub {
public:
    SocperfStubTest() {}
    void PerfRequest(int32_t cmdId, const std::string& msg) override {}
    void PerfRequestEx(int32_t cmdId, bool onOffTag, const std::string& msg) override {}
    void PowerLimitBoost(bool onOffTag, const std::string& msg) override {}
    void ThermalLimitBoost(bool onOffTag, const std::string& msg) override {}
    void LimitRequest(int32_t clientId,
        const std::vector<int32_t>& tags, const std::vector<int64_t>& configs, const std::string& msg) override {}
};

/*
 * @tc.name: SocPerfStubTest_SocPerfServerAPI_001
 * @tc.desc: test socperf stub api
 * @tc.type FUNC
 * @tc.require: issueI78T3V
 */
HWTEST_F(SocPerfServerTest, SocPerfStubTest_SocPerfServerAPI_001, Function | MediumTest | Level0)
{
    SocperfStubTest socPerfStub;
    MessageParcel data;
    data.WriteInterfaceToken(SocPerfStub::GetDescriptor());
    data.WriteInt32(10000);
    data.WriteString("");
    MessageParcel reply;
    MessageOption option;
    uint32_t requestIpcId = static_cast<uint32_t>(SocPerfInterfaceCode::TRANS_IPC_ID_PERF_REQUEST);
    socPerfStub.OnRemoteRequest(requestIpcId, data, reply, option);
}

/*
 * @tc.name: SocPerfStubTest_SocPerfServerAPI_002
 * @tc.desc: test socperf stub api
 * @tc.type FUNC
 * @tc.require: issueI78T3V
 */
HWTEST_F(SocPerfServerTest, SocPerfStubTest_SocPerfServerAPI_002, Function | MediumTest | Level0)
{
    SocperfStubTest socPerfStub;
    MessageParcel data;
    data.WriteInterfaceToken(SocPerfStub::GetDescriptor());
    data.WriteInt32(10000);
    data.WriteString("");
    MessageParcel reply;
    MessageOption option;
    uint32_t requestExIpcId = static_cast<uint32_t>(SocPerfInterfaceCode::TRANS_IPC_ID_PERF_REQUEST_EX);
    socPerfStub.OnRemoteRequest(requestExIpcId, data, reply, option);
}

/*
 * @tc.name: SocPerfStubTest_SocPerfServerAPI_003
 * @tc.desc: test socperf stub api
 * @tc.type FUNC
 * @tc.require: issueI78T3V
 */
HWTEST_F(SocPerfServerTest, SocPerfStubTest_SocPerfServerAPI_003, Function | MediumTest | Level0)
{
    SocperfStubTest socPerfStub;
    MessageParcel data;
    data.WriteInterfaceToken(SocPerfStub::GetDescriptor());
    data.WriteInt32(1);
    std::vector<int32_t> tags = {1001};
    data.WriteInt32Vector(tags);
    std::vector<int64_t> configs = {1416000};
    data.WriteInt64Vector(configs);
    data.WriteString("");
    MessageParcel reply;
    MessageOption option;
    uint32_t powerLimitId = static_cast<uint32_t>(SocPerfInterfaceCode::TRANS_IPC_ID_LIMIT_REQUEST);
    socPerfStub.OnRemoteRequest(powerLimitId, data, reply, option);
}

/*
 * @tc.name: SocPerfStubTest_SocPerfServerAPI_004
 * @tc.desc: test socperf stub api
 * @tc.type FUNC
 * @tc.require: issueI78T3V
 */
HWTEST_F(SocPerfServerTest, SocPerfStubTest_SocPerfServerAPI_004, Function | MediumTest | Level0)
{
    SocperfStubTest socPerfStub;
    MessageParcel data;
    data.WriteInterfaceToken(SocPerfStub::GetDescriptor());
    data.WriteBool(true);
    MessageParcel reply;
    MessageOption option;
    uint32_t powerLimitIpcId = static_cast<uint32_t>(SocPerfInterfaceCode::TRANS_IPC_ID_POWER_LIMIT_BOOST_FREQ);
    socPerfStub.OnRemoteRequest(powerLimitIpcId, data, reply, option);
}

/*
 * @tc.name: SocPerfStubTest_SocPerfServerAPI_005
 * @tc.desc: test socperf stub api
 * @tc.type FUNC
 * @tc.require: issueI78T3V
 */
HWTEST_F(SocPerfServerTest, SocPerfStubTest_SocPerfServerAPI_005, Function | MediumTest | Level0)
{
    SocperfStubTest socPerfStub;
    MessageParcel data;
    data.WriteInterfaceToken(SocPerfStub::GetDescriptor());
    data.WriteBool(true);
    MessageParcel reply;
    MessageOption option;
    uint32_t thermalLimitIpcId = static_cast<uint32_t>(SocPerfInterfaceCode::TRANS_IPC_ID_THERMAL_LIMIT_BOOST_FREQ);
    socPerfStub.OnRemoteRequest(thermalLimitIpcId, data, reply, option);
}

/*
 * @tc.name: SocPerfStubTest_SocPerfServerAPI_006
 * @tc.desc: test socperf stub api
 * @tc.type FUNC
 * @tc.require: issueI78T3V
 */
HWTEST_F(SocPerfServerTest, SocPerfStubTest_SocPerfServerAPI_006, Function | MediumTest | Level0)
{
    SocperfStubTest socPerfStub;
    MessageParcel data;
    data.WriteInterfaceToken(SocPerfStub::GetDescriptor());
    data.WriteBool(true);
    MessageParcel reply;
    MessageOption option;
    uint32_t ipcId = 0x0004;
    socPerfStub.OnRemoteRequest(ipcId, data, reply, option);
}

} // namespace SOCPERF
} // namespace OHOS