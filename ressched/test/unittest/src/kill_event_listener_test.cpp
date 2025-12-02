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
 * @tc.name: ShouldDoNothingWhenReceiveMapNotContainsPushKey
 * @tc.desc: OnReceiveEvent test
 * @tc.type: FUNC
 * @tc.require: I6EEJI
 * @tc.author: zxf
 */
HWTEST_F(KillEventListenerTest, ShouldDoNothingWhenReceiveMapNotContainsPushKey, Function | MediumTest | Level0)
{
    KillEventListener::GetInstance().killReasonMap_.clear();
    std::unordered_map<std::string, std::string> extInfo;
    KillEventListener::GetInstance().OnReceiveEvent(1, 1, extInfo);
    EXPECT_TRUE(KillEventListener::GetInstance().killReasonMap_.empty());
    std::unordered_set<std::string> reasons;
    KillEventListener::GetInstance().killReasonMap_[1] = reasons;
    KillEventListener::GetInstance().OnReceiveEvent(1, 1, extInfo);
    EXPECT_TRUE(KillEventListener::GetInstance().killReasonMap_.size() == 1);
    EXPECT_TRUE(KillEventListener::GetInstance().killReasonMap_.find(1) !=
        KillEventListener::GetInstance().killReasonMap_.end() &&
        KillEventListener::GetInstance().killReasonMap_[1].size() == 0);
}

/**
 * @tc.name: ShouldDoNothingWhenReceiveMapPushValueIsEmpty
 * @tc.desc: OnReceiveEvent test
 * @tc.type: FUNC
 * @tc.require: I6EEJI
 * @tc.author: zxf
 */
HWTEST_F(KillEventListenerTest, ShouldDoNothingWhenReceiveMapPushValueIsEmpty, Function | MediumTest | Level0)
{
    KillEventListener::GetInstance().killReasonMap_.clear();
    std::unordered_map<std::string, std::string> extInfo;
    extInfo["CLOUD_PUSH_CONFIG"] = "";
    KillEventListener::GetInstance().OnReceiveEvent(1, 1, extInfo);
    EXPECT_TRUE(KillEventListener::GetInstance().killReasonMap_.empty());
    std::unordered_set<std::string> reasons;
    KillEventListener::GetInstance().killReasonMap_[1] = reasons;
    KillEventListener::GetInstance().OnReceiveEvent(1, 1, extInfo);
    EXPECT_TRUE(KillEventListener::GetInstance().killReasonMap_.size() == 1);
    EXPECT_TRUE(KillEventListener::GetInstance().killReasonMap_.find(1) !=
        KillEventListener::GetInstance().killReasonMap_.end() &&
        KillEventListener::GetInstance().killReasonMap_[1].size() == 0);
}

/**
 * @tc.name: ShouldDoNothingWhenReceiveMapPushValueIsNotObj
 * @tc.desc: OnReceiveEvent test
 * @tc.type: FUNC
 * @tc.require: I6EEJI
 * @tc.author: zxf
 */
HWTEST_F(KillEventListenerTest, ShouldDoNothingWhenReceiveMapPushValueIsNotObj, Function | MediumTest | Level0)
{
    KillEventListener::GetInstance().killReasonMap_.clear();
    std::unordered_map<std::string, std::string> extInfo;
    extInfo["CLOUD_PUSH_CONFIG"] = "123";
    KillEventListener::GetInstance().OnReceiveEvent(1, 1, extInfo);
    EXPECT_TRUE(KillEventListener::GetInstance().killReasonMap_.empty());
    std::unordered_set<std::string> reasons;
    KillEventListener::GetInstance().killReasonMap_[1] = reasons;
    KillEventListener::GetInstance().OnReceiveEvent(1, 1, extInfo);
    EXPECT_TRUE(KillEventListener::GetInstance().killReasonMap_.size() == 1);
    EXPECT_TRUE(KillEventListener::GetInstance().killReasonMap_.find(1) !=
        KillEventListener::GetInstance().killReasonMap_.end() &&
        KillEventListener::GetInstance().killReasonMap_[1].size() == 0);
}

/**
 * @tc.name: ShouldDoNothingWhenReceiveMapPushValueNotContainsResult
 * @tc.desc: OnReceiveEvent test
 * @tc.type: FUNC
 * @tc.require: I6EEJI
 * @tc.author: zxf
 */
HWTEST_F(KillEventListenerTest, ShouldDoNothingWhenReceiveMapPushValueNotContainsResult, Function | MediumTest | Level0)
{
    KillEventListener::GetInstance().killReasonMap_.clear();
    std::unordered_map<std::string, std::string> extInfo;
    extInfo["CLOUD_PUSH_CONFIG"] = "{\"xx\":1}";
    KillEventListener::GetInstance().OnReceiveEvent(1, 1, extInfo);
    EXPECT_TRUE(KillEventListener::GetInstance().killReasonMap_.empty());
    std::unordered_set<std::string> reasons;
    KillEventListener::GetInstance().killReasonMap_[1] = reasons;
    KillEventListener::GetInstance().OnReceiveEvent(1, 1, extInfo);
    EXPECT_TRUE(KillEventListener::GetInstance().killReasonMap_.size() == 1);
    EXPECT_TRUE(KillEventListener::GetInstance().killReasonMap_.find(1) !=
        KillEventListener::GetInstance().killReasonMap_.end() &&
        KillEventListener::GetInstance().killReasonMap_[1].size() == 0);
}

/**
 * @tc.name: ShouldDoNothingWhenReceiveResultIsNotArray
 * @tc.desc: OnReceiveEvent test
 * @tc.type: FUNC
 * @tc.require: I6EEJI
 * @tc.author: zxf
 */
HWTEST_F(KillEventListenerTest, ShouldDoNothingWhenReceiveResultIsNotArray, Function | MediumTest | Level0)
{
    KillEventListener::GetInstance().killReasonMap_.clear();
    std::unordered_map<std::string, std::string> extInfo;
    extInfo["CLOUD_PUSH_CONFIG"] = "{\"result\":1}";
    KillEventListener::GetInstance().OnReceiveEvent(1, 1, extInfo);
    EXPECT_TRUE(KillEventListener::GetInstance().killReasonMap_.empty());
    std::unordered_set<std::string> reasons;
    KillEventListener::GetInstance().killReasonMap_[1] = reasons;
    KillEventListener::GetInstance().OnReceiveEvent(1, 1, extInfo);
    EXPECT_TRUE(KillEventListener::GetInstance().killReasonMap_.size() == 1);
    EXPECT_TRUE(KillEventListener::GetInstance().killReasonMap_.find(1) !=
        KillEventListener::GetInstance().killReasonMap_.end() &&
        KillEventListener::GetInstance().killReasonMap_[1].size() == 0);
}

/**
 * @tc.name: ShouldClearConfWhenReceiveResultArrayIsEmpty
 * @tc.desc: OnReceiveEvent test
 * @tc.type: FUNC
 * @tc.require: I6EEJI
 * @tc.author: zxf
 */
HWTEST_F(KillEventListenerTest, ShouldClearConfWhenReceiveResultArrayIsEmpty, Function | MediumTest | Level0)
{
    KillEventListener::GetInstance().killReasonMap_.clear();
    std::unordered_map<std::string, std::string> extInfo;
    std::unordered_set<std::string> reasons;
    KillEventListener::GetInstance().killReasonMap_[1] = reasons;
    extInfo["CLOUD_PUSH_CONFIG"] = "{\"result\":[]}";
    KillEventListener::GetInstance().OnReceiveEvent(1, 1, extInfo);
    EXPECT_TRUE(KillEventListener::GetInstance().killReasonMap_.empty());
}

/**
 * @tc.name: ShouldSetRegisterFlagTrueAfterRegister
 * @tc.desc: RegisterKillConfigUpdate test
 * @tc.type: FUNC
 * @tc.require: I6EEJI
 * @tc.author: zxf
 */
HWTEST_F(KillEventListenerTest, ShouldSetRegisterFlagTrueAfterRegister, Function | MediumTest | Level0)
{
    KillEventListener::GetInstance().isRegister_ = false;
    KillEventListener::GetInstance().RegisterKillConfigUpdate();
    EXPECT_TRUE(KillEventListener::GetInstance().isRegister_);
}

/**
 * @tc.name: ShouldReturnTrueWhenUidAndReasonInConf
 * @tc.desc: IsConfigKillReason test
 * @tc.type: FUNC
 * @tc.require: I6EEJI
 * @tc.author: zxf
 */
HWTEST_F(KillEventListenerTest, ShouldReturnTrueWhenUidAndReasonInConf, Function | MediumTest | Level0)
{
    KillEventListener::GetInstance().killReasonMap_.clear();
    KillEventListener::GetInstance().IsConfigKillReason(1, "test");
    std::unordered_set<std::string> ressonSet;
    ressonSet.insert("test");
    KillEventListener::GetInstance().killReasonMap_[1] = ressonSet;
    EXPECT_TRUE(KillEventListener::GetInstance().IsConfigKillReason(1, "test"));
}

/**
 * @tc.name: ShouldReturnFalseWhenReasonNotInConf
 * @tc.desc: IsConfigKillReason test
 * @tc.type: FUNC
 * @tc.require: I6EEJI
 * @tc.author: zxf
 */
HWTEST_F(KillEventListenerTest, ShouldReturnFalseWhenReasonNotInConf, Function | MediumTest | Level0)
{
    KillEventListener::GetInstance().killReasonMap_.clear();
    KillEventListener::GetInstance().IsConfigKillReason(1, "test");
    std::unordered_set<std::string> ressonSet;
    ressonSet.insert("test");
    KillEventListener::GetInstance().killReasonMap_[1] = ressonSet;
    EXPECT_TRUE(!KillEventListener::GetInstance().IsConfigKillReason(1, "test2"));
}

/**
 * @tc.name: ShouldReturnFalseWhenUidNotInConf
 * @tc.desc: IsConfigKillReason test
 * @tc.type: FUNC
 * @tc.require: I6EEJI
 * @tc.author: zxf
 */
HWTEST_F(KillEventListenerTest, ShouldReturnFalseWhenUidNotInConf, Function | MediumTest | Level0)
{
    KillEventListener::GetInstance().killReasonMap_.clear();
    KillEventListener::GetInstance().IsConfigKillReason(1, "test");
    std::unordered_set<std::string> ressonSet;
    ressonSet.insert("test");
    KillEventListener::GetInstance().killReasonMap_[1] = ressonSet;
    EXPECT_TRUE(!KillEventListener::GetInstance().IsConfigKillReason(2, "test"));
}

/**
 * @tc.name: ShouldSetCountTo1WhenKeyFirstReprt
 * @tc.desc: UpdateKillCount test
 * @tc.type: FUNC
 * @tc.require: I6EEJI
 * @tc.author: zxf
 */
HWTEST_F(KillEventListenerTest, ShouldSetCountTo1WhenKeyFirstReprt, Function | MediumTest | Level0)
{
    KillEventListener::GetInstance().killCountMap_.clear();
    KillEventListener::GetInstance().UpdateKillCount(1, "test", false);
    EXPECT_TRUE(KillEventListener::GetInstance().killCountMap_["uid:1,reason:test,allow0"] == 1);
}

/**
 * @tc.name: ShouldCountIncrement1WhenReasonNotFirstReort
 * @tc.desc: UpdateKillCount test
 * @tc.type: FUNC
 * @tc.require: I6EEJI
 * @tc.author: zxf
 */
HWTEST_F(KillEventListenerTest, ShouldCountIncrement1WhenReasonNotFirstReort, Function | MediumTest | Level0)
{
    KillEventListener::GetInstance().killCountMap_.clear();
    KillEventListener::GetInstance().killCountMap_["uid:1,reason:test,allow0"] = 3;
    KillEventListener::GetInstance().UpdateKillCount(1, "test", false);
    EXPECT_TRUE(KillEventListener::GetInstance().killCountMap_["uid:1,reason:test,allow0"] == 4);
}

/**
 * @tc.name: ShouldDiscardDataWhenCacheCountReach1000AndKeyInCache
 * @tc.desc: UpdateKillCount test
 * @tc.type: FUNC
 * @tc.require: I6EEJI
 * @tc.author: zxf
 */
HWTEST_F(KillEventListenerTest, ShouldDiscardDataWhenCacheCountReach1000AndKeyInCache, Function | MediumTest | Level0)
{
    KillEventListener::GetInstance().killCountMap_.clear();
    for (int i = 0; i < 1001; ++i) {
        KillEventListener::GetInstance().UpdateKillCount(i, "test", false);
    }
    KillEventListener::GetInstance().UpdateKillCount(1, "test", false);
    EXPECT_TRUE(KillEventListener::GetInstance().killCountMap_["uid:1,reason:test,allow0"] == 2);
}

/**
 * @tc.name: ShouldCountIncrement1WhenCacheCountReach1000AndKeyNotInCache
 * @tc.desc: UpdateKillCount test
 * @tc.type: FUNC
 * @tc.require: I6EEJI
 * @tc.author: zxf
 */
HWTEST_F(KillEventListenerTest, ShouldCountIncrement1WhenCacheCountReach1000AndKeyNotInCache,
    Function | MediumTest | Level0)
{
    KillEventListener::GetInstance().killCountMap_.clear();
    for (int i = 0; i < 1001; ++i) {
        KillEventListener::GetInstance().UpdateKillCount(i, "test", false);
    }
    KillEventListener::GetInstance().UpdateKillCount(1001, "test", false);
    EXPECT_TRUE(KillEventListener::GetInstance().killCountMap_.find("uid:1001,reason:test,allow0") ==
        KillEventListener::GetInstance().killCountMap_.end());
}

/**
 * @tc.name: ShouldUpdateConfToEmptyWhenJsonIsEmpty
 * @tc.desc: ParseKillConfig test
 * @tc.type: FUNC
 * @tc.require: I6EEJI
 * @tc.author: zxf
 */
HWTEST_F(KillEventListenerTest, ShouldDoNothingWhenJsonIsEmpty, Function | MediumTest | Level0)
{
    nlohmann::json json;
    KillEventListener::GetInstance().killReasonMap_.clear();
    KillEventListener::GetInstance().ParseKillConfig(json);
    EXPECT_TRUE(KillEventListener::GetInstance().killReasonMap_.size() == 0);
    std::unordered_set<std::string> reasons;
    KillEventListener::GetInstance().killReasonMap_[1] = reasons;
    KillEventListener::GetInstance().ParseKillConfig(json);
    EXPECT_TRUE(KillEventListener::GetInstance().killReasonMap_.size() == 1);
    EXPECT_TRUE(KillEventListener::GetInstance().killReasonMap_.find(1) !=
        KillEventListener::GetInstance().killReasonMap_.end() &&
        KillEventListener::GetInstance().killReasonMap_[1].size() == 0);
}

/**
 * @tc.name: ShouldUpdateConfToEmptyWhenJsonResultIsNotArray
 * @tc.desc: ParseKillConfig test
 * @tc.type: FUNC
 * @tc.require: I6EEJI
 * @tc.author: zxf
 */
HWTEST_F(KillEventListenerTest,  ShouldDoNothingWhenJsonResultIsNotArray, Function | MediumTest | Level0)
{
    nlohmann::json json;
    json["result"] = "xxx";
    KillEventListener::GetInstance().killReasonMap_.clear();
    KillEventListener::GetInstance().ParseKillConfig(json);
    EXPECT_TRUE(KillEventListener::GetInstance().killReasonMap_.size() == 0);
    std::unordered_set<std::string> reasons;
    KillEventListener::GetInstance().killReasonMap_[1] = reasons;
    KillEventListener::GetInstance().ParseKillConfig(json);
    EXPECT_TRUE(KillEventListener::GetInstance().killReasonMap_.size() == 1);
    EXPECT_TRUE(KillEventListener::GetInstance().killReasonMap_.find(1) !=
        KillEventListener::GetInstance().killReasonMap_.end() &&
        KillEventListener::GetInstance().killReasonMap_[1].size() == 0);
}

/**
 * @tc.name: ShouldNotInertToConfWhenConfNotObj
 * @tc.desc: ParseKillConfig test
 * @tc.type: FUNC
 * @tc.require: I6EEJI
 * @tc.author: zxf
 */
HWTEST_F(KillEventListenerTest, ShouldNotInertToConfWhenConfNotObj, Function | MediumTest | Level0)
{
    nlohmann::json json;
    nlohmann::json items = nlohmann::json::array();
    nlohmann::json item = "sssfd";
    items.push_back(item);
    json["result"] = items;
    KillEventListener::GetInstance().killReasonMap_.clear();
    KillEventListener::GetInstance().ParseKillConfig(json);
    EXPECT_TRUE(KillEventListener::GetInstance().killReasonMap_.size() == 0);
}

/**
 * @tc.name: ShouldNotInertToConfWhenConfNotContainsUid
 * @tc.desc: ParseKillConfig test
 * @tc.type: FUNC
 * @tc.require: I6EEJI
 * @tc.author: zxf
 */
HWTEST_F(KillEventListenerTest, ShouldNotInertToConfWhenConfNotContainsUid, Function | MediumTest | Level0)
{
    nlohmann::json json;
    nlohmann::json items = nlohmann::json::array();
    nlohmann::json item;
    item["testKey"] = "testValue";
    items.push_back(item);
    json["result"] = items;
    KillEventListener::GetInstance().killReasonMap_.clear();
    KillEventListener::GetInstance().ParseKillConfig(json);
    EXPECT_TRUE(KillEventListener::GetInstance().killReasonMap_.size() == 0);
}

/**
 * @tc.name: ShouldNotInertToConfWhenUidIsNotString
 * @tc.desc: ParseKillConfig test
 * @tc.type: FUNC
 * @tc.require: I6EEJI
 * @tc.author: zxf
 */
HWTEST_F(KillEventListenerTest, ShouldNotInertToConfWhenUidIsNotString, Function | MediumTest | Level0)
{
    nlohmann::json json;
    nlohmann::json items = nlohmann::json::array();
    nlohmann::json item;
    item["uid"] = 1;
    items.push_back(item);
    json["result"] = items;
    KillEventListener::GetInstance().killReasonMap_.clear();
    KillEventListener::GetInstance().ParseKillConfig(json);
    EXPECT_TRUE(KillEventListener::GetInstance().killReasonMap_.size() == 0);
}

/**
 * @tc.name: ShouldOnlyInertUidToConfWhenReasonsIsEmpty
 * @tc.desc: ParseKillConfig test
 * @tc.type: FUNC
 * @tc.require: I6EEJI
 * @tc.author: zxf
 */
HWTEST_F(KillEventListenerTest, ShouldOnlyInertUidToConfWhenReasonsIsEmpty, Function | MediumTest | Level0)
{
    nlohmann::json json;
    nlohmann::json items = nlohmann::json::array();
    nlohmann::json item;
    nlohmann::json reasons = nlohmann::json::array();
    item["reason"] = reasons;
    item["uid"] = "1";
    items.push_back(item);
    json["result"] = items;
    KillEventListener::GetInstance().killReasonMap_.clear();
    KillEventListener::GetInstance().ParseKillConfig(json);
    EXPECT_TRUE(KillEventListener::GetInstance().killReasonMap_.size() == 1);
    EXPECT_TRUE(KillEventListener::GetInstance().killReasonMap_.find(1) !=
        KillEventListener::GetInstance().killReasonMap_.end() &&
        KillEventListener::GetInstance().killReasonMap_[1].size() == 0);
}

/**
 * @tc.name: ShouldOnlyInertUidToConfWhenReasonsIsNotArray
 * @tc.desc: ParseKillConfig test
 * @tc.type: FUNC
 * @tc.require: I6EEJI
 * @tc.author: zxf
 */
HWTEST_F(KillEventListenerTest, ShouldNotInertUidToConfWhenReasonsIsNotArray, Function | MediumTest | Level0)
{
    nlohmann::json json;
    nlohmann::json items = nlohmann::json::array();
    nlohmann::json item;
    item["uid"] = "1";
    item["reason"] = "testValue";
    items.push_back(item);
    json["result"] = items;
    KillEventListener::GetInstance().killReasonMap_.clear();
    KillEventListener::GetInstance().ParseKillConfig(json);
    EXPECT_TRUE(KillEventListener::GetInstance().killReasonMap_.size() == 0);
}

/**
 * @tc.name: ShouldOnlyInertUidToConfWhenAllReasonIsNotString
 * @tc.desc: ParseKillConfig test
 * @tc.type: FUNC
 * @tc.require: I6EEJI
 * @tc.author: zxf
 */
HWTEST_F(KillEventListenerTest, ShouldOnlyInertUidToConfWhenAllReasonIsNotString, Function | MediumTest | Level0)
{
    nlohmann::json json;
    nlohmann::json items = nlohmann::json::array();
    nlohmann::json reasons = nlohmann::json::array();
    reasons.push_back(2);
    reasons.push_back(3);
    nlohmann::json item;
    item["uid"] = "1";
    item["reason"] = reasons;
    items.push_back(item);
    json["result"] = items;
    KillEventListener::GetInstance().killReasonMap_.clear();
    KillEventListener::GetInstance().ParseKillConfig(json);
    EXPECT_TRUE(KillEventListener::GetInstance().killReasonMap_.size() == 1);
    EXPECT_TRUE(KillEventListener::GetInstance().killReasonMap_.find(1) !=
        KillEventListener::GetInstance().killReasonMap_.end() &&
        KillEventListener::GetInstance().killReasonMap_[1].size() == 0);
}

/**
 * @tc.name: ShouldInertReasonsWhenDataIsLegal
 * @tc.desc: ParseKillConfig test
 * @tc.type: FUNC
 * @tc.require: I6EEJI
 * @tc.author: zxf
 */
HWTEST_F(KillEventListenerTest, ShouldInertReasonsWhenDataIsLegal, Function | MediumTest | Level0)
{
    nlohmann::json json;
    nlohmann::json items = nlohmann::json::array();
    nlohmann::json reasons = nlohmann::json::array();
    reasons.push_back("xxx");
    nlohmann::json item;
    item["uid"] = "1";
    item["reason"] = reasons;
    items.push_back(item);
    json["result"] = items;
    KillEventListener::GetInstance().killReasonMap_.clear();
    KillEventListener::GetInstance().ParseKillConfig(json);
    EXPECT_TRUE(KillEventListener::GetInstance().killReasonMap_.size() == 1);
    EXPECT_TRUE(KillEventListener::GetInstance().killReasonMap_.find(1) !=
        KillEventListener::GetInstance().killReasonMap_.end() &&
        KillEventListener::GetInstance().killReasonMap_[1].size() == 1 &&
        KillEventListener::GetInstance().killReasonMap_[1].find("xxx") !=
        KillEventListener::GetInstance().killReasonMap_[1].end());
}

/**
 * @tc.name: ShouldNotInertToConfWhenUidCanNotTransToInt
 * @tc.desc: ParseKillConfig test
 * @tc.type: FUNC
 * @tc.require: I6EEJI
 * @tc.author: zxf
 */
HWTEST_F(KillEventListenerTest, ShouldNotInertToConfWhenUidCanNotTransToInt, Function | MediumTest | Level0)
{
    nlohmann::json json;
    nlohmann::json items = nlohmann::json::array();
    nlohmann::json reasons = nlohmann::json::array();
    reasons.push_back("xxx");
    reasons.push_back(2);
    nlohmann::json item;
    item["uid"] = "xx";
    item["reason"] = reasons;
    items.push_back(item);
    json["result"] = items;
    KillEventListener::GetInstance().killReasonMap_.clear();
    KillEventListener::GetInstance().ParseKillConfig(json);
    EXPECT_TRUE(KillEventListener::GetInstance().killReasonMap_.size() == 0);
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
