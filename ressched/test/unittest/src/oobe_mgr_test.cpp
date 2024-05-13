/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "oobe_mgr_test.h"

#include "errors.h"
#include "oobe_datashare_utils.h"
#include "ioobe_task.h"
#include "oobe_manager.h"

using namespace std;
using namespace testing::ext;

namespace OHOS {
namespace ResourceSchedule {
namespace {
const std::string KEYWORD = "basic_statement_agreed";
} // namespace

class OOBETaskImpl : public IOOBETask {
public:
    OOBETaskImpl() {}
    void ExcutingTask() override {}
};

void OOBEMgrTest::SetUpTestCase() {}

void OOBEMgrTest::TearDownTestCase() {}

void OOBEMgrTest::SetUp() {}

void OOBEMgrTest::TearDown() {}

/**
 * @tc.name: oobe manager TestOOBEManager_001
 * @tc.desc: test the interface RegisterObserver of OOBEManager
 * @tc.type: FUNC
 * @tc.require: issueI97493
 * @tc.author:zhumingjie
 */
HWTEST_F(OOBEMgrTest, TestOOBEManager_001, Function | MediumTest | Level0)
{
    OOBEManager& oobeMgr = OOBEManager::GetInstance();
    OOBEManager::ResDataAbilityObserver::UpdateFunc updateFunc = [&]() {};
    int32_t ret = oobeMgr.RegisterObserver(KEYWORD, updateFunc);
    EXPECT_EQ(ret, ERR_OK);

    int32_t ret1 = oobeMgr.UnregisterObserver();
    EXPECT_EQ(ret1, ERR_OK);
}

/**
 * @tc.name: oobe manager TestOOBEManager_002
 * @tc.desc: test the interface RegisterObserver of OOBEManager
 * @tc.type: FUNC
 * @tc.require: issueI97493
 * @tc.author:zhumingjie
 */
HWTEST_F(OOBEMgrTest, TestOOBEManager_002, Function | MediumTest | Level0)
{
    OOBEManager& oobeMgr = OOBEManager::GetInstance();
    OOBEManager::ResDataAbilityObserver::UpdateFunc updateFunc = [&]() {};
    oobeMgr.observer_ = new OOBEManager::ResDataAbilityObserver();
    int32_t ret = oobeMgr.RegisterObserver(KEYWORD, updateFunc);
    EXPECT_EQ(ret, ERR_OK);

    int32_t ret1 = oobeMgr.UnregisterObserver();
    EXPECT_EQ(ret1, ERR_OK);
}

/**
 * @tc.name: oobe manager TestOOBEManager_003
 * @tc.desc: test the interface OnChange and SetUpdateFunc of ResDataShareAbilityObserver
 * @tc.type: FUNC
 * @tc.require: issueI97493
 * @tc.author:zhumingjie
 */
HWTEST_F(OOBEMgrTest, TestOOBEManager_003, Function | MediumTest | Level0)
{
    sptr<OOBEManager::ResDataAbilityObserver> oobeObserver = new OOBEManager::ResDataAbilityObserver();
    oobeObserver->OnChange();
    SUCCEED();

    OOBEManager::ResDataAbilityObserver::UpdateFunc updateFunc = [&]() {};
    oobeObserver->SetUpdateFunc(updateFunc);
    EXPECT_NE(oobeObserver->update_, nullptr);

    oobeObserver->update_();
    SUCCEED();
}

/**
 * @tc.name: oobe manager TestOOBEManager_004
 * @tc.desc: test the interface Initialize of OOBEManager
 * @tc.type: FUNC
 * @tc.require: issueI97493
 * @tc.author:zhumingjie
 */
HWTEST_F(OOBEMgrTest, TestOOBEManager_004, Function | MediumTest | Level0)
{
    OOBEManager& oobeMgr = OOBEManager::GetInstance();
    oobeMgr.Initialize();
    SUCCEED();
}

/**
 * @tc.name: oobe manager TestOOBEManager_005
 * @tc.desc: test the interface SubmitTask of OOBEManager
 * @tc.type: FUNC
 * @tc.require: issueI97493
 * @tc.author:zhumingjie
 */
HWTEST_F(OOBEMgrTest, TestOOBEManager_005, Function | MediumTest | Level0)
{
    std::shared_ptr<IOOBETask> oobeTask = std::make_shared<OOBETaskImpl>();
    OOBEManager& oobeMgr = OOBEManager::GetInstance();
    oobeMgr.g_oobeValue = true;
    oobeMgr.oobeTasks_.clear();
    bool flag = oobeMgr.SubmitTask(oobeTask);
    EXPECT_EQ(flag, true);

    oobeMgr.g_oobeValue = false;
    bool flag1 = oobeMgr.SubmitTask(oobeTask);
    EXPECT_EQ(oobeMgr.oobeTasks_.size(), 1);

    bool flag2 = oobeMgr.SubmitTask(nullptr);
    EXPECT_EQ(flag2, false);
}

/**
 * @tc.name: oobe manager TestOOBEManager_006
 * @tc.desc: test the interface SubmitTask of OOBEManager
 * @tc.type: FUNC
 * @tc.require: issueI97493
 * @tc.author:zhumingjie
 */
HWTEST_F(OOBEMgrTest, TestOOBEManager_006, Function | MediumTest | Level0)
{
    OOBEManager& oobeMgr = OOBEManager::GetInstance();
    oobeMgr.StartListen();
    SUCCEED();
}

/**
 * @tc.name: oobe manager TestOOBEManager_007
 * @tc.desc: test the interface SubmitTask of OOBEManager
 * @tc.type: FUNC
 * @tc.require: issueI97493
 * @tc.author:zhumingjie
 */
HWTEST_F(OOBEMgrTest, TestOOBEManager_007, Function | MediumTest | Level0)
{
    OOBEManager& oobeMgr = OOBEManager::GetInstance();
    bool flag = oobeMgr.GetOOBValue();
    SUCCEED();
}
#undef private
#undef protected
} // namespace ResourceSchedule
} // namespace OHOS
