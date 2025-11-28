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
#include "gtest/hwext/gtest-multithread.h"

#include "kill_event_listener.h"

namespace OHOS {
namespace ResourceSchedule {
using namespace std;
using namespace testing::ext;
using namespace testing::mt;

class KillEventListenerTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void KillEventListenerTest::SetUpTestCase(void) {}

void KillEventListenerTest::TearDownTestCase() {}

void KillEventListenerTest::SetUp() {}

void KillEventListenerTest::TearDown() {}

/**
 * @tc.name: OnReceiveEvent001
 * @tc.desc: OnReceiveEvent test
 * @tc.type: FUNC
 * @tc.require: I6EEJI
 * @tc.author: zxf
 */
HWTEST_F(KillEventListenerTest, OnReceiveEvent001, Function | MediumTest | Level0)
{
    KillEventListener::GetInstance().killReasonMap_.clear();
    std::unordered_map<std::string, std::string> extInfo;
    KillEventListener::GetInstance().OnReceiveEvent(1, 1, extInfo);
    extInfo["result"] = "";
    KillEventListener::GetInstance().OnReceiveEvent(1, 1, extInfo);
    extInfo["result"] = "123";
    KillEventListener::GetInstance().OnReceiveEvent(1, 1, extInfo);
    extInfo["result"] = "{\"xx\":1}";
    KillEventListener::GetInstance().OnReceiveEvent(1, 1, extInfo);
    EXPECT_TRUE(KillEventListener::GetInstance().killReasonMap_.empty());
}

/**
 * @tc.name: RegisterKilConfigUpdate001
 * @tc.desc: RegisterKilConfigUpdate test
 * @tc.type: FUNC
 * @tc.require: I6EEJI
 * @tc.author: zxf
 */
HWTEST_F(KillEventListenerTest, RegisterKilConfigUpdate001, Function | MediumTest | Level0)
{
    KillEventListener::GetInstance().isRegister_ = true;
    KillEventListener::GetInstance().RegisterKilConfigUpdate();
    KillEventListener::GetInstance().isRegister_ = false;
    KillEventListener::GetInstance().RegisterKilConfigUpdate();
    EXPECT_TRUE(KillEventListener::GetInstance().isRegister_);
}

/**
 * @tc.name: IsConfigKillReason001
 * @tc.desc: IsConfigKillReason test
 * @tc.type: FUNC
 * @tc.require: I6EEJI
 * @tc.author: zxf
 */
HWTEST_F(KillEventListenerTest, IsConfigKillReason001, Function | MediumTest | Level0)
{
    KillEventListener::GetInstance().killReasonMap_.clear();
    KillEventListener::GetInstance().IsConfigKillReason(1, "test");
    std::unordered_set<std::string> ressonSet;
    ressonSet.insert("test");
    KillEventListener::GetInstance().killReasonMap_[1] = ressonSet;
    EXPECT_TRUE(KillEventListener::GetInstance().IsConfigKillReason(1, "test"));
    EXPECT_TRUE(!KillEventListener::GetInstance().IsConfigKillReason(1, "test2"));
    EXPECT_TRUE(!KillEventListener::GetInstance().IsConfigKillReason(2, "test"));
}

/**
 * @tc.name: UpdateKillCount001
 * @tc.desc: UpdateKillCount test
 * @tc.type: FUNC
 * @tc.require: I6EEJI
 * @tc.author: zxf
 */
HWTEST_F(KillEventListenerTest, UpdateKillCount001, Function | MediumTest | Level0)
{
    KillEventListener::GetInstance().killCountMap_.clear();
    KillEventListener::GetInstance().UpdateKillCount(1, "test", false);
    EXPECT_TRUE(KillEventListener::GetInstance().killCountMap_["udi:1,reason:test,allow:0"] == 1);
    KillEventListener::GetInstance().UpdateKillCount(1, "test", false);
    EXPECT_TRUE(KillEventListener::GetInstance().killCountMap_["udi:1,reason:test,allow:0"] == 2);
    for (int i = 0; i < 1001; ++i) {
        KillEventListener::GetInstance().UpdateKillCount(i, "test", false);
    }
    KillEventListener::GetInstance().UpdateKillCount(1001, "test", false);
    KillEventListener::GetInstance().UpdateKillCount(1, "test", false);
    EXPECT_TRUE(KillEventListener::GetInstance().killCountMap_.size() == 1000);
}

/**
 * @tc.name: ParseKillConfig001
 * @tc.desc: ParseKillConfig test
 * @tc.type: FUNC
 * @tc.require: I6EEJI
 * @tc.author: zxf
 */
HWTEST_F(KillEventListenerTest, ParseKillConfig001, Function | MediumTest | Level0)
{
    nlohmann::json json;
    KillEventListener::GetInstance().ParseKillConfig(json);
    nlohmann::json items = nlohmann::json::array();
    nlohmann::json item1 = "sssfd";
    nlohmann::json item2;
    item2["testKey"] = "testValue";
    nlohmann::json item3;
    item3["uid"] = 1;
    nlohmann::json item4;
    item4["uid"] = "1";
    nlohmann::json item5;
    item5["uid"] = "1";
    item5["reason"] = "testValue";
    nlohmann::json reasons = nlohmann::json::array();
    reasons.insert("xxx");
    reasons.insert(2);
    nlohmann::json item6;
    item6["uid"] = "1";
    item6["reason"] = reasons;
    nlohmann::json item7;
    item7["uid"] = "xx";
    item6["reason"] = reasons;
    items.push_back(item1);
    items.push_back(item2);
    items.push_back(item3);
    items.push_back(item4);
    items.push_back(item5);
    items.push_back(item6);
    items.push_back(item7);
    KillEventListener::GetInstance().killReasonMap_.clear();
    KillEventListener::GetInstance().ParseKillConfig(items);
    EXPECT_TRUE(KillEventListener::GetInstance().killReasonMap_.size() == 1);
    EXPECT_TRUE(KillEventListener::GetInstance().killReasonMap_[1].size() == 1);
}

/**
 * @tc.name: IsAllowedKill001
 * @tc.desc: IsAllowedKill test
 * @tc.type: FUNC
 * @tc.require: I6EEJI
 * @tc.author: zxf
 */
HWTEST_F(KillEventListenerTest, IsAllowedKill001, Function | MediumTest | Level0)
{
    KillEventListener::GetInstance().killReasonMap_.clear();
    KillEventListener::GetInstance().isTaskSubmit_ = true;
    KillEventListener::GetInstance().isRegister_ = true;
    KillEventListener::GetInstance().IsAllowedKill(1, "test");
    KillEventListener::GetInstance().isTaskSubmit_ = false;
    KillEventListener::GetInstance().Init();
    KillEventListener::GetInstance().IsAllowedKill(1, "test");
    EXPECT_TRUE(KillEventListener::GetInstance().isTaskSubmit_);
}
} // namespace ResourceSchedule
} // namespace OHOS
