/*
 * Copyright (c) 2025-2025 Huawei Device Co., Ltd.
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
#include <gtest/gtest.h>

#define private public
#include "suspend_manager_base_observer.h"
#include "suspend_manager_base_client.h"
#include "suspend_state_observer_base_stub.h"
#include "suspend_state_observer_base_proxy.h"
#include "accesstoken_kit.h"
#include "token_setproc.h"
#include "singleton.h"
#include "res_sched_service.h"
#include "suspend_manager_base_log.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace ResourceSchedule {
namespace {
    static const std::string FOUNDATION_NAME = "foundation";
}

class SuspendStateObserverBaseStubTestObj : public SuspendStateObserverBaseStub {
public:
    std::vector<int32_t> pidList_;
    int32_t uid_;
    States state_ = States::ACTIVE_STATE;
public:
    ErrCode OnActive(const std::vector<int32_t> &pidList, int32_t uid)
    {
        SUSPEND_MANAGER_LOGI("SuspendStateObserverBaseStubTestObj register OnActive");
        pidList_ = pidList;
        uid_ = uid;
        state_ = States::ACTIVE_STATE;
        return ERR_OK;
    }
    ErrCode OnDoze(const std::vector<int32_t> &pidList, int32_t uid)
    {
        SUSPEND_MANAGER_LOGI("SuspendStateObserverBaseStubTestObj register OnDoze");
        pidList_ = pidList;
        uid_ = uid;
        state_ = States::DOZE_STATE;
        return ERR_OK;
    }
    ErrCode OnFrozen(const std::vector<int32_t> &pidList, int32_t uid)
    {
        SUSPEND_MANAGER_LOGI("SuspendStateObserverBaseStubTestObj register OnFrozen");
        pidList_ = pidList;
        uid_ = uid;
        state_ = States::FROZEN_STATE;
        return ERR_OK;
    }
};

class SuspendStateObserverTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp();
    void TearDown();
};

void SuspendStateObserverTest::SetUp()
{}

void SuspendStateObserverTest::TearDown()
{}

void GetNativeToken(const std::string &name)
{
    auto tokenId = Security::AccessToken::AccessTokenKit::GetNativeTokenId(name);
    EXPECT_EQ(0, SetSelfTokenID(tokenId));
}

/**
 * @tc.name: RegisterSuspendObserverTest_001
 * @tc.desc: client侧注册去注册全流程实现.
 * @tc.type: FUNC
 * @tc.require: ICQKUB
 */
HWTEST_F(SuspendStateObserverTest, RegisterSuspendObserverTest_001, TestSize.Level1)
{
    auto selfTokenId = GetSelfTokenID();
    GetNativeToken(FOUNDATION_NAME);
    /* 构造一个观察者对象 */
    sptr<SuspendStateObserverBaseStub> suspendObservers_;
    /* 注册空对象，失败 */
    SuspendManagerBaseClient::GetInstance().RegisterSuspendObserver(suspendObservers_);
    suspendObservers_ = sptr<SuspendStateObserverBaseStub>(new SuspendStateObserverBaseStubTestObj());
    EXPECT_NE(suspendObservers_, nullptr);
    /* 注册该对象，成功 */
    SuspendManagerBaseClient::GetInstance().RegisterSuspendObserver(suspendObservers_);
    /* 再次注册该对象，失败 */
    SuspendManagerBaseClient::GetInstance().RegisterSuspendObserver(suspendObservers_);
    /* 去注册空对象，失败 */
    SuspendManagerBaseClient::GetInstance().UnregisterSuspendObserver(nullptr);
    /* 去注册该对象，成功 */
    SuspendManagerBaseClient::GetInstance().UnregisterSuspendObserver(suspendObservers_);
    /* 再次去注册该对象，失败 */
    SuspendManagerBaseClient::GetInstance().UnregisterSuspendObserver(suspendObservers_);

    EXPECT_EQ(0, SetSelfTokenID(selfTokenId));
}

/**
 * @tc.name: RegisterSuspendObserverTest_002
 * @tc.desc: service中序列化反序列化全流程实现.
 * @tc.type: FUNC
 * @tc.require: ICQKUB
 */
HWTEST_F(SuspendStateObserverTest, RegisterSuspendObserverTest_002, TestSize.Level1)
{
    sptr<SuspendStateObserverBaseStubTestObj> suspendObservers_ =
        sptr<SuspendStateObserverBaseStubTestObj>(new SuspendStateObserverBaseStubTestObj());
    EXPECT_NE(suspendObservers_, nullptr);

    auto suspendManagerService_ = DelayedSingleton<ResSchedService>::GetInstance();
    EXPECT_NE(suspendManagerService_, nullptr);

    MessageParcel data;
    ASSERT_TRUE(data.WriteInterfaceToken(ResSchedService::GetDescriptor()));
    ASSERT_TRUE(data.WriteRemoteObject(suspendObservers_->AsObject()));
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(IResSchedServiceIpcCode::COMMAND_REGISTER_SUSPEND_OBSERVER);
    ErrCode ret = suspendManagerService_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(ret, ERR_OK);

    ASSERT_TRUE(data.WriteInterfaceToken(ResSchedService::GetDescriptor()));
    ASSERT_TRUE(data.WriteRemoteObject(suspendObservers_->AsObject()));
    code = static_cast<uint32_t>(IResSchedServiceIpcCode::COMMAND_UNREGISTER_SUSPEND_OBSERVER);
    ret = suspendManagerService_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(ret, ERR_OK);
}

/**
 * @tc.name: RegisterSuspendObserverTest_003
 * @tc.desc: implement中注册去注册边界实现.
 * @tc.type: FUNC
 * @tc.require: ICQKUB
 */
HWTEST_F(SuspendStateObserverTest, RegisterSuspendObserverTest_003, TestSize.Level1)
{
    sptr<SuspendStateObserverBaseStubTestObj> suspendObservers_ =
        sptr<SuspendStateObserverBaseStubTestObj>(new SuspendStateObserverBaseStubTestObj());
    EXPECT_NE(suspendObservers_, nullptr);

    int32_t ret = SuspendManagerBaseObserver::GetInstance().RegisterSuspendObserverInner(nullptr);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);

    ret = SuspendManagerBaseObserver::GetInstance().UnregisterSuspendObserverInner(nullptr);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);

    ret = SuspendManagerBaseObserver::GetInstance().UnregisterSuspendObserverInner(suspendObservers_);
    EXPECT_EQ(ret, ERR_OK);
}

/**
 * @tc.name: RegisterSuspendObserverTest_004
 * @tc.desc: implement中注册去注册边界实现.
 * @tc.type: FUNC
 * @tc.require: ICQKUB
 */
HWTEST_F(SuspendStateObserverTest, RegisterSuspendObserverTest_004, TestSize.Level1)
{
    sptr<SuspendStateObserverBaseStubTestObj> suspendObservers_ =
        sptr<SuspendStateObserverBaseStubTestObj>(new SuspendStateObserverBaseStubTestObj());
    EXPECT_NE(suspendObservers_, nullptr);

    int32_t ret = SuspendManagerBaseObserver::GetInstance().RegisterSuspendObserverInner(nullptr);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);

    ret = SuspendManagerBaseObserver::GetInstance().UnregisterSuspendObserverInner(nullptr);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name: UpdateSuspendObserverTest_001
 * @tc.desc: 正常注册，观察者状态变化.
 * @tc.type: FUNC
 * @tc.require: ICQKUB
 */
HWTEST_F(SuspendStateObserverTest, UpdateSuspendObserverTest_001, TestSize.Level1)
{
    /* 构造一个观察者对象 */
    sptr<SuspendStateObserverBaseStubTestObj> suspendObservers_ =
        sptr<SuspendStateObserverBaseStubTestObj>(new SuspendStateObserverBaseStubTestObj());
    EXPECT_NE(suspendObservers_, nullptr);

    /* 注册观察者对象 */
    int32_t ret = SuspendManagerBaseObserver::GetInstance().RegisterSuspendObserverInner(suspendObservers_);
    EXPECT_EQ(ret, ERR_OK);

    /* 校验观察者初始值 */
    EXPECT_EQ(suspendObservers_->state_, States::ACTIVE_STATE);
    EXPECT_EQ(suspendObservers_->uid_, 0);
    EXPECT_TRUE(suspendObservers_->pidList_.empty());

    /* 通知观察者frozen信息 */
    States state = States::FROZEN_STATE;
    int32_t uid = 100;
    std::vector<int32_t> pidList = {10, 20, 30};
    ret = SuspendManagerBaseObserver::GetInstance().UpdateSuspendObserver(state, uid, pidList);
    EXPECT_EQ(ret, ERR_OK);
    EXPECT_EQ(suspendObservers_->state_, state);
    EXPECT_EQ(suspendObservers_->uid_, uid);
    EXPECT_EQ(suspendObservers_->pidList_, pidList);

    /* 通知观察者active信息 */
    state = States::ACTIVE_STATE;
    uid = 200;
    pidList = {40, 50, 60};
    ret = SuspendManagerBaseObserver::GetInstance().UpdateSuspendObserver(state, uid, pidList);
    EXPECT_EQ(ret, ERR_OK);
    EXPECT_EQ(suspendObservers_->state_, state);
    EXPECT_EQ(suspendObservers_->uid_, uid);
    EXPECT_EQ(suspendObservers_->pidList_, pidList);

    /* 通知观察者doze信息 */
    state = States::DOZE_STATE;
    uid = 300;
    pidList = {70, 80, 90};
    ret = SuspendManagerBaseObserver::GetInstance().UpdateSuspendObserver(state, uid, pidList);
    EXPECT_EQ(ret, ERR_OK);
    EXPECT_EQ(suspendObservers_->state_, state);
    EXPECT_EQ(suspendObservers_->uid_, uid);
    EXPECT_EQ(suspendObservers_->pidList_, pidList);

    /* 去注册观察者对象 */
    ret = SuspendManagerBaseObserver::GetInstance().UnregisterSuspendObserverInner(suspendObservers_);
    EXPECT_EQ(ret, ERR_OK);

    /* 通知观察者frozen信息 */
    state = States::FROZEN_STATE;
    uid = 100;
    pidList = {10, 20, 30};
    ret = SuspendManagerBaseObserver::GetInstance().UpdateSuspendObserver(state, uid, pidList);
    EXPECT_EQ(ret, ERR_OK);

    /* 观察者信息不再变化 */
    EXPECT_NE(suspendObservers_->state_, state);
    EXPECT_NE(suspendObservers_->uid_, uid);
    EXPECT_NE(suspendObservers_->pidList_, pidList);
}

/**
 * @tc.name: UpdateSuspendObserverTest_002
 * @tc.desc: 未注册场景观察者状态变化.
 * @tc.type: FUNC
 * @tc.require: ICQKUB
 */
HWTEST_F(SuspendStateObserverTest, UpdateSuspendObserverTest_002, TestSize.Level1)
{
    /* 构造一个观察者对象 */
    sptr<SuspendStateObserverBaseStubTestObj> suspendObservers_ =
        sptr<SuspendStateObserverBaseStubTestObj>(new SuspendStateObserverBaseStubTestObj());
    EXPECT_NE(suspendObservers_, nullptr);

    /* 注册空的观察者对象 */
    int32_t ret = SuspendManagerBaseObserver::GetInstance().RegisterSuspendObserverInner(nullptr);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);

    /* 校验观察者初始值 */
    EXPECT_EQ(suspendObservers_->state_, States::ACTIVE_STATE);
    EXPECT_EQ(suspendObservers_->uid_, 0);
    EXPECT_TRUE(suspendObservers_->pidList_.empty());

    /* 通知观察者frozen信息 */
    States state = States::FROZEN_STATE;
    int32_t uid = 100;
    std::vector<int32_t> pidList = {10, 20, 30};
    ret = SuspendManagerBaseObserver::GetInstance().UpdateSuspendObserver(state, uid, pidList);
    EXPECT_EQ(ret, ERR_OK);
    /* 观察者信息无变化 */
    EXPECT_EQ(suspendObservers_->state_, States::ACTIVE_STATE);
    EXPECT_EQ(suspendObservers_->uid_, 0);
    EXPECT_TRUE(suspendObservers_->pidList_.empty());
}

/**
 * @tc.name: UpdateSuspendObserverTest_003
 * @tc.desc: 动态注册场景观察者状态变化.
 * @tc.type: FUNC
 * @tc.require: ICQKUB
 */
HWTEST_F(SuspendStateObserverTest, UpdateSuspendObserverTest_003, TestSize.Level1)
{
    /* 构造两个观察者对象 */
    sptr<SuspendStateObserverBaseStubTestObj> suspendObservers1_ =
        sptr<SuspendStateObserverBaseStubTestObj>(new SuspendStateObserverBaseStubTestObj());
    EXPECT_NE(suspendObservers1_, nullptr);
    sptr<SuspendStateObserverBaseStubTestObj> suspendObservers2_ =
        sptr<SuspendStateObserverBaseStubTestObj>(new SuspendStateObserverBaseStubTestObj());
    EXPECT_NE(suspendObservers2_, nullptr);

    /* 注册一个观察者对象 */
    int32_t ret = SuspendManagerBaseObserver::GetInstance().RegisterSuspendObserverInner(suspendObservers1_);
    EXPECT_EQ(ret, ERR_OK);

    /* 校验观察者初始值 */
    EXPECT_EQ(suspendObservers1_->state_, States::ACTIVE_STATE);
    EXPECT_EQ(suspendObservers1_->uid_, 0);
    EXPECT_TRUE(suspendObservers1_->pidList_.empty());
    EXPECT_EQ(suspendObservers2_->state_, States::ACTIVE_STATE);
    EXPECT_EQ(suspendObservers2_->uid_, 0);
    EXPECT_TRUE(suspendObservers2_->pidList_.empty());

    /* 通知观察者frozen1信息 */
    States stateFrozen1 = States::FROZEN_STATE;
    int32_t uidFrozen1 = 100;
    std::vector<int32_t> pidListFrozen1 = {10, 20, 30};
    ret = SuspendManagerBaseObserver::GetInstance().UpdateSuspendObserver(stateFrozen1, uidFrozen1, pidListFrozen1);
    EXPECT_EQ(ret, ERR_OK);
    /* 注册观察者信息变化 */
    EXPECT_EQ(suspendObservers1_->state_, stateFrozen1);
    EXPECT_EQ(suspendObservers1_->uid_, uidFrozen1);
    EXPECT_EQ(suspendObservers1_->pidList_, pidListFrozen1);
    /* 未注册观察者信息无变化 */
    EXPECT_EQ(suspendObservers2_->state_, States::ACTIVE_STATE);
    EXPECT_EQ(suspendObservers2_->uid_, 0);
    EXPECT_TRUE(suspendObservers2_->pidList_.empty());

    /* 注册第二个观察者对象 */
    ret = SuspendManagerBaseObserver::GetInstance().RegisterSuspendObserverInner(suspendObservers2_);
    EXPECT_EQ(ret, ERR_OK);

    /* 通知观察者frozen2信息 */
    States stateFrozen2 = States::FROZEN_STATE;
    int32_t uidFrozen2 = 200;
    std::vector<int32_t> pidListFrozen2 = {10, 20, 30};
    ret = SuspendManagerBaseObserver::GetInstance().UpdateSuspendObserver(stateFrozen2, uidFrozen2, pidListFrozen2);
    EXPECT_EQ(ret, ERR_OK);
    /* 注册观察者信息变化 */
    EXPECT_EQ(suspendObservers1_->state_, stateFrozen2);
    EXPECT_EQ(suspendObservers1_->uid_, uidFrozen2);
    EXPECT_EQ(suspendObservers1_->pidList_, pidListFrozen2);
    EXPECT_EQ(suspendObservers2_->state_, stateFrozen2);
    EXPECT_EQ(suspendObservers2_->uid_, uidFrozen2);
    EXPECT_EQ(suspendObservers2_->pidList_, pidListFrozen2);

    /* 去注册两个观察者对象 */
    ret = SuspendManagerBaseObserver::GetInstance().UnregisterSuspendObserverInner(suspendObservers1_);
    EXPECT_EQ(ret, ERR_OK);
    ret = SuspendManagerBaseObserver::GetInstance().UnregisterSuspendObserverInner(suspendObservers2_);
    EXPECT_EQ(ret, ERR_OK);
}

/**
 * @tc.name: UpdateSuspendObserverTest_004
 * @tc.desc: 动态去注册场景观察者状态变化.
 * @tc.type: FUNC
 * @tc.require: ICQKUB
 */
HWTEST_F(SuspendStateObserverTest, UpdateSuspendObserverTest_004, TestSize.Level1)
{
    /* 构造两个观察者对象 */
    sptr<SuspendStateObserverBaseStubTestObj> suspendObservers1_ =
        sptr<SuspendStateObserverBaseStubTestObj>(new SuspendStateObserverBaseStubTestObj());
    EXPECT_NE(suspendObservers1_, nullptr);
    sptr<SuspendStateObserverBaseStubTestObj> suspendObservers2_ =
        sptr<SuspendStateObserverBaseStubTestObj>(new SuspendStateObserverBaseStubTestObj());
    EXPECT_NE(suspendObservers2_, nullptr);

    /* 注册两个观察者对象 */
    int32_t ret = SuspendManagerBaseObserver::GetInstance().RegisterSuspendObserverInner(suspendObservers1_);
    EXPECT_EQ(ret, ERR_OK);
    ret = SuspendManagerBaseObserver::GetInstance().RegisterSuspendObserverInner(suspendObservers2_);
    EXPECT_EQ(ret, ERR_OK);

    /* 通知观察者frozen1信息 */
    States stateFrozen1 = States::FROZEN_STATE;
    int32_t uidFrozen1 = 100;
    std::vector<int32_t> pidListFrozen1 = {10, 20, 30};
    ret = SuspendManagerBaseObserver::GetInstance().UpdateSuspendObserver(stateFrozen1, uidFrozen1, pidListFrozen1);

    /* 去注册第一个观察者对象 */
    ret = SuspendManagerBaseObserver::GetInstance().UnregisterSuspendObserverInner(suspendObservers1_);
    EXPECT_EQ(ret, ERR_OK);

    /* 通知观察者active信息 */
    States stateActive = States::ACTIVE_STATE;
    int32_t uidActive = 300;
    std::vector<int32_t> pidListActive = {40, 50, 60};
    ret = SuspendManagerBaseObserver::GetInstance().UpdateSuspendObserver(stateActive, uidActive, pidListActive);
    EXPECT_EQ(ret, ERR_OK);

    /* 未注册观察者信息无变化 */
    EXPECT_EQ(suspendObservers1_->state_, stateFrozen1);
    EXPECT_EQ(suspendObservers1_->uid_, uidFrozen1);
    EXPECT_EQ(suspendObservers1_->pidList_, pidListFrozen1);
    /* 注册观察者信息变化 */
    EXPECT_EQ(suspendObservers2_->state_, stateActive);
    EXPECT_EQ(suspendObservers2_->uid_, uidActive);
    EXPECT_EQ(suspendObservers2_->pidList_, pidListActive);

    /* 去注册第二个观察者对象 */
    ret = SuspendManagerBaseObserver::GetInstance().UnregisterSuspendObserverInner(suspendObservers2_);
    EXPECT_EQ(ret, ERR_OK);

    /* 全去注册后再通知观察者doze信息 */
    States stateDoze = States::DOZE_STATE;
    int32_t uidDoze = 400;
    std::vector<int32_t> pidListDoze = {70, 80, 90};
    ret = SuspendManagerBaseObserver::GetInstance().UpdateSuspendObserver(stateDoze, uidDoze, pidListDoze);
    EXPECT_EQ(ret, ERR_OK);
    /* 未注册观察者信息无变化 */
    EXPECT_EQ(suspendObservers1_->state_, stateFrozen1);
    EXPECT_EQ(suspendObservers1_->uid_, uidFrozen1);
    EXPECT_EQ(suspendObservers1_->pidList_, pidListFrozen1);
    EXPECT_EQ(suspendObservers2_->state_, stateActive);
    EXPECT_EQ(suspendObservers2_->uid_, uidActive);
    EXPECT_EQ(suspendObservers2_->pidList_, pidListActive);

    /* 已完全去注册，无法继续去注册 */
    ret = SuspendManagerBaseObserver::GetInstance().UnregisterSuspendObserverInner(suspendObservers1_);
    EXPECT_EQ(ret, ERR_OK);
    ret = SuspendManagerBaseObserver::GetInstance().UnregisterSuspendObserverInner(suspendObservers2_);
    EXPECT_EQ(ret, ERR_OK);
}

/**
 * @tc.name: UpdateSuspendObserverTest_005
 * @tc.desc: 正常注册，观察者状态变化.
 * @tc.type: FUNC
 * @tc.require: ICQKUB
 */
HWTEST_F(SuspendStateObserverTest, UpdateSuspendObserverTest_005, TestSize.Level1)
{
    /* 构造一个观察者对象 */
    sptr<SuspendStateObserverBaseStubTestObj> suspendObservers_ =
        sptr<SuspendStateObserverBaseStubTestObj>(new SuspendStateObserverBaseStubTestObj());
    EXPECT_NE(suspendObservers_, nullptr);

    /* 注册观察者对象 */
    int32_t ret = SuspendManagerBaseObserver::GetInstance().RegisterSuspendObserverInner(suspendObservers_);
    EXPECT_EQ(ret, ERR_OK);

    /* 校验观察者初始值 */
    EXPECT_EQ(suspendObservers_->state_, States::ACTIVE_STATE);
    EXPECT_EQ(suspendObservers_->uid_, 0);
    EXPECT_TRUE(suspendObservers_->pidList_.empty());

    /* 通知观察者frozen信息 */
    States state = static_cast<States>(5);
    int32_t uid = 100;
    std::vector<int32_t> pidList = {10, 20, 30};
    ret = SuspendManagerBaseObserver::GetInstance().UpdateSuspendObserver(state, uid, pidList);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
    EXPECT_EQ(suspendObservers_->state_, States::ACTIVE_STATE);
    EXPECT_EQ(suspendObservers_->uid_, 0);
    EXPECT_TRUE(suspendObservers_->pidList_.empty());
}

/**
 * @tc.name: SuspendStateObserverBaseStubTest_001
 * @tc.desc: SuspendStateObserverBaseStub状态变化，测试active消息解析.
 * @tc.type: FUNC
 * @tc.require: ICQKUB
 */
HWTEST_F(SuspendStateObserverTest, SuspendStateObserverBaseStubTest_001, TestSize.Level1)
{
    sptr<SuspendStateObserverBaseStubTestObj> suspendObservers_ =
        sptr<SuspendStateObserverBaseStubTestObj>(new SuspendStateObserverBaseStubTestObj());
    EXPECT_NE(suspendObservers_, nullptr);
    sptr<SuspendStateObserverBaseProxy> proxy = new SuspendStateObserverBaseProxy(suspendObservers_);
    EXPECT_NE(proxy, nullptr);
    int32_t uid = 100;
    std::vector<int32_t> pidList = {10, 20, 30};
    MessageParcel data;
    const uint32_t message = static_cast<uint32_t>(ISuspendStateObserverBaseIpcCode::COMMAND_ON_ACTIVE);
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    /* 非法data解析失败 */
    ErrCode ret = suspendObservers_->OnRemoteRequest(message, data, reply, option);
    EXPECT_NE(ret, ERR_OK);
    /* 合法data解析成功 */
    proxy->OnActive(pidList, uid);
    EXPECT_EQ(suspendObservers_->state_, States::ACTIVE_STATE);
    EXPECT_EQ(suspendObservers_->uid_, uid);
    EXPECT_EQ(suspendObservers_->pidList_, pidList);
}

/**
 * @tc.name: SuspendStateObserverBaseStubTest_002
 * @tc.desc: SuspendStateObserverBaseStub状态变化，测试Frozen消息解析.
 * @tc.type: FUNC
 * @tc.require: ICQKUB
 */
HWTEST_F(SuspendStateObserverTest, SuspendStateObserverBaseStubTest_002, TestSize.Level1)
{
    sptr<SuspendStateObserverBaseStubTestObj> suspendObservers_ =
        sptr<SuspendStateObserverBaseStubTestObj>(new SuspendStateObserverBaseStubTestObj());
    EXPECT_NE(suspendObservers_, nullptr);
    sptr<SuspendStateObserverBaseProxy> proxy = new SuspendStateObserverBaseProxy(suspendObservers_);
    EXPECT_NE(proxy, nullptr);
    int32_t uid = 100;
    std::vector<int32_t> pidList = {10, 20, 30};
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    const uint32_t message = static_cast<uint32_t>(ISuspendStateObserverBaseIpcCode::COMMAND_ON_FROZEN);
    /* 非法data解析失败 */
    ErrCode ret = suspendObservers_->OnRemoteRequest(message, data, reply, option);
    EXPECT_NE(ret, ERR_OK);
    proxy->OnFrozen(pidList, uid);
    /* 合法data解析成功 */
    EXPECT_EQ(suspendObservers_->state_, States::FROZEN_STATE);
    EXPECT_EQ(suspendObservers_->uid_, uid);
    EXPECT_EQ(suspendObservers_->pidList_, pidList);
}

/**
 * @tc.name: SuspendStateObserverBaseStubTest_003
 * @tc.desc: SuspendStateObserverBaseStub状态变化，测试Doze消息解析.
 * @tc.type: FUNC
 * @tc.require: ICQKUB
 */
HWTEST_F(SuspendStateObserverTest, SuspendStateObserverBaseStubTest_003, TestSize.Level1)
{
    sptr<SuspendStateObserverBaseStubTestObj> suspendObservers_ =
        sptr<SuspendStateObserverBaseStubTestObj>(new SuspendStateObserverBaseStubTestObj());
    EXPECT_NE(suspendObservers_, nullptr);
    sptr<SuspendStateObserverBaseProxy> proxy = new SuspendStateObserverBaseProxy(suspendObservers_);
    EXPECT_NE(proxy, nullptr);
    int32_t uid = 100;
    std::vector<int32_t> pidList = {10, 20, 30};
    MessageParcel data;
    const uint32_t message = static_cast<uint32_t>(ISuspendStateObserverBaseIpcCode::COMMAND_ON_DOZE);
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    /* 非法data解析失败 */
    ErrCode ret = suspendObservers_->OnRemoteRequest(message, data, reply, option);
    EXPECT_NE(ret, ERR_OK);
    proxy->OnDoze(pidList, uid);
    /* 合法data解析成功 */
    EXPECT_EQ(suspendObservers_->state_, States::DOZE_STATE);
    EXPECT_EQ(suspendObservers_->uid_, uid);
    EXPECT_EQ(suspendObservers_->pidList_, pidList);
}

/**
 * @tc.name: SuspendStateObserverBaseStubTest_004
 * @tc.desc: test SuspendStateObserverBaseStub.
 * @tc.type: FUNC
 * @tc.require: ICQKUB
 */
HWTEST_F(SuspendStateObserverTest, SuspendStateObserverBaseStubTest_004, TestSize.Level1)
{
    sptr<SuspendStateObserverBaseStubTestObj> suspendObservers_ =
        sptr<SuspendStateObserverBaseStubTestObj>(new SuspendStateObserverBaseStubTestObj());
    EXPECT_NE(suspendObservers_, nullptr);
    int32_t uid = 100;
    MessageParcel data;
    const uint32_t message = static_cast<uint32_t>(ISuspendStateObserverBaseIpcCode::COMMAND_ON_FROZEN);
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    /* 非法data解析失败 */
    ErrCode ret = suspendObservers_->OnRemoteRequest(message, data, reply, option);
    EXPECT_NE(ret, ERR_OK);

    /* 空方法接口覆盖 */
    std::vector<int32_t> pidList = {10, 20, 30};
    suspendObservers_->OnActive(pidList, uid);
    suspendObservers_->OnDoze(pidList, uid);
    suspendObservers_->OnFrozen(pidList, uid);
}
} // namespace ResourceSchedule
} // namespace OHOS