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
    std::shared_ptr<SocPerfServer> socPerfServer_ = DelayedSingleton<SocPerfServer>::GetInstance();
};

void SocPerfServerTest::SetUpTestCase(void)
{
}

void SocPerfServerTest::TearDownTestCase(void)
{
}

void SocPerfServerTest::SetUp(void)
{
}

void SocPerfServerTest::TearDown(void)
{
}

/*
 * @tc.name: SocPerfServerTest_Init_Config_001
 * @tc.desc: test init config
 * @tc.type FUNC
 * @tc.require: issueI78T3V
 */
HWTEST_F(SocPerfServerTest, SocPerfServerTest_Init_Config_001, Function | MediumTest | Level0)
{
    socPerfServer_->OnStart();
    bool ret = socPerfServer_->socPerf.Init();
    sleep(1);
    EXPECT_TRUE(ret);
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
    socPerfServer_->socPerf.PerfRequest(10000, msg);
    socPerfServer_->socPerf.PerfRequestEx(10000, true, msg);
    socPerfServer_->socPerf.PerfRequestEx(10000, false, msg);
    socPerfServer_->socPerf.PerfRequestEx(10028, true, msg);
    socPerfServer_->socPerf.PerfRequestEx(10028, false, msg);
    socPerfServer_->socPerf.LimitRequest(ActionType::ACTION_TYPE_POWER, {1001}, {999000}, msg);
    socPerfServer_->socPerf.LimitRequest(ActionType::ACTION_TYPE_THERMAL, {1001}, {999000}, msg);
    socPerfServer_->socPerf.LimitRequest(ActionType::ACTION_TYPE_POWER, {1001}, {1325000}, msg);
    socPerfServer_->socPerf.LimitRequest(ActionType::ACTION_TYPE_THERMAL, {1001}, {1325000}, msg);
    socPerfServer_->socPerf.PowerLimitBoost(true, msg);
    socPerfServer_->socPerf.ThermalLimitBoost(true, msg);
    EXPECT_EQ(msg, "testBoost");
    std::string id = "1000";
    std::string name = "lit_cpu_freq";
    std::string pair = "1001";
    std::string mode = "1";
    std::string persisMode = "1";
    std::string configFile = "";
    bool ret = socPerfServer_->socPerf.CheckResourceTag(id.c_str(), name.c_str(), pair.c_str(), mode.c_str(),
        persisMode.c_str(), configFile.c_str());
    EXPECT_TRUE(ret);
    ret = socPerfServer_->socPerf.CheckResourceTag(nullptr, name.c_str(), pair.c_str(), mode.c_str(),
        persisMode.c_str(), configFile.c_str());
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
 * @tc.name: SocPerfServerTest_SocPerfServerAPI_001
 * @tc.desc: test socperf server api
 * @tc.type FUNC
 * @tc.require: issueI78T3V
 */
HWTEST_F(SocPerfServerTest, SocPerfServerTest_SocPerfServerAPI_001, Function | MediumTest | Level0)
{
    std::string msg = "";
    socPerfServer_->SetRequestStatus(false, msg);
    socPerfServer_->socPerf.ClearAllAliveRequest();
    EXPECT_FALSE(socPerfServer_->socPerf.perfRequestEnable_);
#ifdef SOCPERF_ADAPTOR_FFRT
    auto socPerfThreadWrap = std::make_shared<SocPerfThreadWrap>();
#else
    auto runner = AppExecFwk::EventRunner::Create("socperf#");
    auto socPerfThreadWrap = std::make_shared<SocPerfThreadWrap>(runner);
#endif
    socPerfThreadWrap->ClearAllAliveRequest();
    for (const std::pair<int32_t, std::shared_ptr<ResStatus>>& item : socPerfThreadWrap->resStatusInfo) {
        if (item.second == nullptr) {
            continue;
        }
        std::list<std::shared_ptr<ResAction>>& resActionList = item.second->resActionList[ACTION_TYPE_PERF];
        EXPECT_TRUE(resActionList.empty());
    }
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
    void SetRequestStatus(bool status, const std::string& msg) override {};
    void SetThermalLevel(int32_t level) override {};
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
    int32_t ret = socPerfStub.OnRemoteRequest(requestIpcId, data, reply, option);
    EXPECT_EQ(ret, ERR_OK);
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
    int32_t ret = socPerfStub.OnRemoteRequest(requestExIpcId, data, reply, option);
    EXPECT_EQ(ret, ERR_OK);
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
    int32_t ret = socPerfStub.OnRemoteRequest(powerLimitId, data, reply, option);
    EXPECT_EQ(ret, ERR_OK);
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
    int32_t ret = socPerfStub.OnRemoteRequest(powerLimitIpcId, data, reply, option);
    EXPECT_EQ(ret, ERR_OK);
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
    int32_t ret = socPerfStub.OnRemoteRequest(thermalLimitIpcId, data, reply, option);
    EXPECT_EQ(ret, ERR_OK);
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
    uint32_t ipcId = 0x0006;
    int32_t ret = socPerfStub.OnRemoteRequest(ipcId, data, reply, option);
    EXPECT_NE(ret, ERR_OK);
}

/*
 * @tc.name: SocPerfStubTest_SocPerfServerAPI_007
 * @tc.desc: test socperf stub api
 * @tc.type FUNC
 * @tc.require: issueI78T3V
 */
HWTEST_F(SocPerfServerTest, SocPerfStubTest_SocPerfServerAPI_007, Function | MediumTest | Level0)
{
    SocperfStubTest socPerfStub;
    MessageParcel data;
    data.WriteInterfaceToken(SocPerfStub::GetDescriptor());
    data.WriteBool(false);
    data.WriteString("");
    MessageParcel reply;
    MessageOption option;
    uint32_t ipcId = static_cast<uint32_t>(SocPerfInterfaceCode::TRANS_IPC_ID_SET_STATUS);
    int32_t ret = socPerfStub.OnRemoteRequest(ipcId, data, reply, option);
    EXPECT_EQ(ret, ERR_OK);

    MessageParcel dataPerf;
    dataPerf.WriteInterfaceToken(SocPerfStub::GetDescriptor());
    dataPerf.WriteInt32(10000);
    dataPerf.WriteString("");
    MessageParcel replyPerf;
    MessageOption optionPerf;
    uint32_t requestPerfIpcId = static_cast<uint32_t>(SocPerfInterfaceCode::TRANS_IPC_ID_PERF_REQUEST);
    ret = socPerfStub.OnRemoteRequest(requestPerfIpcId, dataPerf, replyPerf, optionPerf);
    EXPECT_EQ(ret, ERR_OK);

    MessageParcel dataLimit;
    dataLimit.WriteInterfaceToken(SocPerfStub::GetDescriptor());
    dataLimit.WriteInt32(1);
    std::vector<int32_t> tags = {1001};
    dataLimit.WriteInt32Vector(tags);
    std::vector<int64_t> configs = {1416000};
    dataLimit.WriteInt64Vector(configs);
    dataLimit.WriteString("");
    MessageParcel replyLimit;
    MessageOption optionLimit;
    uint32_t powerLimitId = static_cast<uint32_t>(SocPerfInterfaceCode::TRANS_IPC_ID_LIMIT_REQUEST);
    ret = socPerfStub.OnRemoteRequest(powerLimitId, dataLimit, reply, option);
    EXPECT_EQ(ret, ERR_OK);
}

/*
 * @tc.name: SocPerfServerTest_SetThermalLevel_001
 * @tc.desc: perf request lvl server API
 * @tc.type FUNC
 * @tc.require: issue#I95U8S
 */
HWTEST_F(SocPerfServerTest, SocPerfServerTest_SetThermalLevel_Server_001, Function | MediumTest | Level0)
{
    SocperfStubTest socPerfStub;
    MessageParcel data;
    data.WriteInterfaceToken(SocPerfStub::GetDescriptor());
    data.WriteInt32(3);
    MessageParcel reply;
    MessageOption option;
    uint32_t ipcId = static_cast<uint32_t>(SocPerfInterfaceCode::TRANS_IPC_ID_SET_THERMAL_LEVEL);
    int32_t ret = socPerfStub.OnRemoteRequest(ipcId, data, reply, option);
    EXPECT_EQ(ret, ERR_OK);
}

/*
 * @tc.name: SocPerfServerTest_SetThermalLevel_Server_002
 * @tc.desc: perf request lvl server API
 * @tc.type FUNC
 * @tc.require: issue#I95U8S
 */
HWTEST_F(SocPerfServerTest, SocPerfServerTest_SetThermalLevel_Server_002, Function | MediumTest | Level0)
{
    socPerfServer_->SetThermalLevel(3);
    EXPECT_EQ(socPerfServer_->socPerf.thermalLvl_, 3);
}

/*
 * @tc.name: SocPerfServerTest_SetThermalLevel_Server_003
 * @tc.desc: perf request lvl server API
 * @tc.type FUNC
 * @tc.require: issue#I95U8S
 */
HWTEST_F(SocPerfServerTest, SocPerfServerTest_SetThermalLevel_Server_003, Function | MediumTest | Level0)
{
    const int32_t appColdStartCmdId = 10000;
    const int32_t appWarmStartCmdId = 10001;
    if (socPerfServer_->socPerf.perfActionsInfo[appColdStartCmdId] == nullptr ||
        socPerfServer_->socPerf.perfActionsInfo[appWarmStartCmdId] == nullptr) {
        SUCCEED();
        return;
    }
    std::shared_ptr<Actions> appColdStartActions = socPerfServer_->socPerf.perfActionsInfo[appColdStartCmdId];
    std::list<std::shared_ptr<Action>>  appColdStartActionList = appColdStartActions->actionList;
    for (auto item : appColdStartActionList) {
        item->thermalCmdId_ = 88888;
        bool ret = socPerfServer_->socPerf.DoPerfRequestThremalLvl(appColdStartCmdId, item, EVENT_INVALID);
        EXPECT_FALSE(ret);

        item->thermalCmdId_ = appWarmStartCmdId;
        ret = socPerfServer_->socPerf.DoPerfRequestThremalLvl(appColdStartCmdId, item, EVENT_INVALID);
        EXPECT_TRUE(ret);
    }
    SUCCEED();
}

/*
 * @tc.name: SocPerfServerTest_SetThermalLevel_Server_004
 * @tc.desc: perf request lvl server API
 * @tc.type FUNC
 * @tc.require: issue#I95U8S
 */
HWTEST_F(SocPerfServerTest, SocPerfServerTest_SetThermalLevel_Server_004, Function | MediumTest | Level0)
{
    const int32_t appColdStartCmdId = 10000;
    const int32_t appWarmStartCmdId = 10001;
    if (socPerfServer_->socPerf.perfActionsInfo[appColdStartCmdId] == nullptr ||
        socPerfServer_->socPerf.perfActionsInfo[appWarmStartCmdId] == nullptr) {
        SUCCEED();
        return;
    }
    std::shared_ptr<Actions> appWarmStartActions = socPerfServer_->socPerf.perfActionsInfo[appWarmStartCmdId];
    std::shared_ptr<Actions> appColdStartActions = socPerfServer_->socPerf.perfActionsInfo[appColdStartCmdId];
    std::list<std::shared_ptr<Action>> appWarmStartActionList = appWarmStartActions->actionList;
    int32_t minThermalLvl = 3;
    for (auto item : appWarmStartActionList) {
        (*item).thermalLvl_ = minThermalLvl;
        minThermalLvl++;
    }
    std::list<std::shared_ptr<Action>>  appColdStartActionList = appColdStartActions->actionList;
    for (auto item : appColdStartActionList) {
        (*item).thermalCmdId_ = appWarmStartCmdId;
        socPerfServer_->SetThermalLevel(1);
        bool ret = socPerfServer_->socPerf.DoPerfRequestThremalLvl(appColdStartCmdId, item, EVENT_INVALID);
        EXPECT_FALSE(ret);

        socPerfServer_->SetThermalLevel(3);
        ret = socPerfServer_->socPerf.DoPerfRequestThremalLvl(appColdStartCmdId, item, EVENT_INVALID);
        EXPECT_TRUE(ret);

        socPerfServer_->SetThermalLevel(99);
        ret = socPerfServer_->socPerf.DoPerfRequestThremalLvl(appColdStartCmdId, item, EVENT_INVALID);
        EXPECT_TRUE(ret);
    }
}

/*
 * @tc.name: SocPerfServerTest_SetThermalLevel_Server_005
 * @tc.desc: perf request lvl server API
 * @tc.type FUNC
 * @tc.require: issue#I95U8S
 */
HWTEST_F(SocPerfServerTest, SocPerfServerTest_SetThermalLevel_Server_005, Function | MediumTest | Level0)
{
    std::shared_ptr<SocPerfThreadWrap> socPerfThreadWrap = socPerfServer_->socPerf.socperfThreadWraps[0];
    socPerfThreadWrap->resStatusInfo[1000]->candidatesValue[ACTION_TYPE_PERFLVL] = 1000;
    bool ret = socPerfThreadWrap->ArbitratePairResInPerfLvl(1000);
    EXPECT_TRUE(ret);

    socPerfThreadWrap->resStatusInfo[1000]->candidatesValue[ACTION_TYPE_PERFLVL] = INVALID_VALUE;
    socPerfThreadWrap->resStatusInfo[1001]->candidatesValue[ACTION_TYPE_PERFLVL] = 1000;
    ret = socPerfThreadWrap->ArbitratePairResInPerfLvl(1000);
    EXPECT_TRUE(ret);

    socPerfThreadWrap->resStatusInfo[1000]->candidatesValue[ACTION_TYPE_PERFLVL] = INVALID_VALUE;
    socPerfThreadWrap->resStatusInfo[1001]->candidatesValue[ACTION_TYPE_PERFLVL] = INVALID_VALUE;
    ret = socPerfThreadWrap->ArbitratePairResInPerfLvl(1000);
    EXPECT_FALSE(ret);
}
} // namespace SOCPERF
} // namespace OHOS