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

#define private public
#define protected public

#include "errors.h"
#include "datashare_predicates.h"
#include "datashare_result_set.h"
#include "data_share_utils.h"
#include "datashare_values_bucket.h"
#include "ioobe_task.h"
#include "oobe_manager.h"
#include "res_data_ability_observer.h"
#include "res_data_ability_provider.h"
#include "system_ability_definition.h"
#include "uri.h"

using namespace std;
using namespace testing::ext;

namespace OHOS {
namespace ResourceSchedule {
namespace {
const std::string KEYWORD = "basic_statement_agreed";
const std::string SETTING_COLUMN_KEYWORD = "KEYWORD";
const std::string SETTING_COLUMN_VALUE = "VALUE";
const std::string SETTING_URI_PROXY = "datashare:///com.ohos.settingsdata/entry/settingsdata/SETTINGSDATA?Proxy=true";
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
 * @tc.name: oobe manager TestDataShareUtils_001
 * @tc.desc: test the interface GetValue of dataShareUtils
 * @tc.type: FUNC
 * @tc.require: issueI97493
 * @tc.author:zhumingjie
 */
HWTEST_F(OOBEMgrTest, TestDataShareUtils_001, Function | MediumTest | Level0)
{
    int32_t result = 0;
    ResourceSchedule::DataShareUtils.GetInstance().GetValue(KEYWORD, result);
    EXPECT_NE(result, 0);
}

/**
 * @tc.name: oobe manager TestDataShareUtils_002
 * @tc.desc: test the interface GetValue of dataShareUtils
 * @tc.type: FUNC
 * @tc.require: issueI97493
 * @tc.author:zhumingjie
 */
HWTEST_F(OOBEMgrTest, TestDataShareUtils_002, Function | MediumTest | Level0)
{
    int64_t result = 0;
    ResourceSchedule::DataShareUtils.GetInstance().GetValue(KEYWORD, result);
    EXPECT_NE(result, 0);
}

/**
 * @tc.name: oobe manager TestDataShareUtils_003
 * @tc.desc: test the interface GetValue of dataShareUtils
 * @tc.type: FUNC
 * @tc.require: issueI97493
 * @tc.author:zhumingjie
 */
HWTEST_F(OOBEMgrTest, TestDataShareUtils_003, Function | MediumTest | Level0)
{
    std::string result;
    ResourceSchedule::DataShareUtils.GetInstance().GetValue(KEYWORD, result);
    EXPECT_NE(result, "");
}

/**
 * @tc.name: oobe manager TestDataShareUtils_004
 * @tc.desc: test the interface GetValue of dataShareUtils
 * @tc.type: FUNC
 * @tc.require: issueI97493
 * @tc.author:zhumingjie
 */
HWTEST_F(OOBEMgrTest, TestDataShareUtils_004, Function | MediumTest | Level0)
{
    bool result;
    int32_t ret = ResourceSchedule::DataShareUtils.GetInstance().GetValue(KEYWORD, result);
    EXPECT_EQ(ret, ERR_INVALID_OPERATION);
}

/**
 * @tc.name: oobe manager TestDataShareUtils_005
 * @tc.desc: test the interface GetStringValue of dataShareUtils
 * @tc.type: FUNC
 * @tc.require: issueI97493
 * @tc.author:zhumingjie
 */
HWTEST_F(OOBEMgrTest, TestDataShareUtils_005, Function | MediumTest | Level0)
{
    std::string result;
    int32_t ret1 = ResourceSchedule::DataShareUtils.GetInstance().GetStringValue(KEYWORD, result);
    EXPECT_EQ(ret1, ERR_OK);

    int32_t ret2 = ResourceSchedule::DataShareUtils.GetInstance().GetStringValue("", result);
    EXPECT_NE(ret2, ERR_OK);

    int32_t ret3 = ResourceSchedule::DataShareUtils.GetInstance().GetStringValue("test", result);
    EXPECT_NE(ret3, ERR_OK);

    int32_t ret4 = ResourceSchedule::DataShareUtils.GetInstance().GetStringValue("123", result);
    EXPECT_NE(ret4, ERR_OK);
}

/**
 * @tc.name: oobe manager TestDataShareUtils_006
 * @tc.desc: test the interface CreateDataShareHelper of dataShareUtils
 * @tc.type: FUNC
 * @tc.require: issueI97493
 * @tc.author:zhumingjie
 */
HWTEST_F(OOBEMgrTest, TestDataShareUtils_006, Function | MediumTest | Level0)
{
    DataShareUtils& dataShareUtils = DataShareUtils::GetInstance();
    dataShareUtils.remoteObj_ = nullptr;
    std::shared_ptr<DataShare::DataShareHelper> helper = dataShareUtils.CreateDataShareHelper();
    EXPECT_EQ(helper, nullptr);
}

/**
 * @tc.name: oobe manager TestDataShareUtils_007
 * @tc.desc: test the interface CreateDataShareHelper of dataShareUtils
 * @tc.type: FUNC
 * @tc.require: issueI97493
 * @tc.author:zhumingjie
 */
HWTEST_F(OOBEMgrTest, TestDataShareUtils_007, Function | MediumTest | Level0)
{
    DataShareUtils& dataShareUtils = DataShareUtils::GetInstance();
    dataShareUtils.InitSystemAbilityManager();
    std::shared_ptr<DataShare::DataShareHelper> helper = dataShareUtils.CreateDataShareHelper();
    EXPECT_NE(helper, nullptr);
}

/**
 * @tc.name: oobe manager TestDataShareUtils_008
 * @tc.desc: test the interface ReleaseDataShareHelper of dataShareUtils
 * @tc.type: FUNC
 * @tc.require: issueI97493
 * @tc.author:zhumingjie
 */
HWTEST_F(OOBEMgrTest, TestDataShareUtils_008, Function | MediumTest | Level0)
{
    DataShareUtils& dataShareUtils = DataShareUtils::GetInstance();
    dataShareUtils.InitSystemAbilityManager();
    std::shared_ptr<DataShare::DataShareHelper> helper = dataShareUtils.CreateDataShareHelper();
    bool flag = dataShareUtils.ReleaseDataShareHelper(helper);
    EXPECT_TRUE(flag);
}

/**
 * @tc.name: oobe manager TestDataShareUtils_009
 * @tc.desc: test the interface InitSystemAbilityManager of dataShareUtils
 * @tc.type: FUNC
 * @tc.require: issueI97493
 * @tc.author:zhumingjie
 */
HWTEST_F(OOBEMgrTest, TestDataShareUtils_009, Function | MediumTest | Level0)
{
    DataShareUtils& dataShareUtils = DataShareUtils::GetInstance();
    dataShareUtils.InitSystemAbilityManager();
    SUCCEED();
}

/**
 * @tc.name: oobe manager TestDataShareUtils_010
 * @tc.desc: test the interface AssembleUri of dataShareUtils
 * @tc.type: FUNC
 * @tc.require: issueI97493
 * @tc.author:zhumingjie
 */
HWTEST_F(OOBEMgrTest, TestDataShareUtils_011, Function | MediumTest | Level0)
{
    DataShareUtils& dataShareUtils = DataShareUtils::GetInstance();
    dataShareUtils.AssembleUri(KEYWORD);
    SUCCEED();
}

/**
 * @tc.name: oobe manager TestOOBEManager_001
 * @tc.desc: test the interface InitSystemAbilityListener of OOBEManager
 * @tc.type: FUNC
 * @tc.require: issueI97493
 * @tc.author:zhumingjie
 */
HWTEST_F(OOBEMgrTest, TestOOBEManager_001, Function | MediumTest | Level0)
{
    OOBEManager& oobeMgr = OOBEManager::GetInstance();
    oobeMgr.InitSystemAbilityListener();

    oobeMgr.InitSystemAbilityListener();
    SUCCEED();
}

/**
 * @tc.name: oobe manager TestOOBEManager_002
 * @tc.desc: test the interface Initialize of OOBEManager
 * @tc.type: FUNC
 * @tc.require: issueI97493
 * @tc.author:zhumingjie
 */
HWTEST_F(OOBEMgrTest, TestOOBEManager_002, Function | MediumTest | Level0)
{
    OOBEManager& oobeMgr = OOBEManager::GetInstance();
    bool flag = oobeMgr.Initialize();
    EXPECT_EQ(flag, true);
}

/**
 * @tc.name: oobe manager TestOOBEManager_003
 * @tc.desc: test the interface SubmitTask of OOBEManager
 * @tc.type: FUNC
 * @tc.require: issueI97493
 * @tc.author:zhumingjie
 */
HWTEST_F(OOBEMgrTest, TestOOBEManager_003, Function | MediumTest | Level0)
{
    OOBEManager& oobeMgr = OOBEManager::GetInstance();
    bool flag = oobeMgr.SubmitTask(nullptr);
    EXPECT_EQ(flag, false);
}

/**
 * @tc.name: oobe manager TestOOBEManager_004
 * @tc.desc: test the interface SubmitTask of OOBEManager
 * @tc.type: FUNC
 * @tc.require: issueI97493
 * @tc.author:zhumingjie
 */
HWTEST_F(OOBEMgrTest, TestOOBEManager_004, Function | MediumTest | Level0)
{
    std::shared_ptr<IOOBETask> oobeTask = std::make_shared<OOBETaskImpl>();
    OOBEManager& oobeMgr = OOBEManager::GetInstance();
    bool flag = oobeMgr.SubmitTask(oobeTask);
    EXPECT_EQ(flag, true);
}

/**
 * @tc.name: oobe manager TestOOBEManager_005
 * @tc.desc: test the interface StartListen of OOBEManager
 * @tc.type: FUNC
 * @tc.require: issueI97493
 * @tc.author:zhumingjie
 */
HWTEST_F(OOBEMgrTest, TestOOBEManager_005, Function | MediumTest | Level0)
{
    OOBEManager& oobeMgr = OOBEManager::GetInstance();
    oobeMgr.StartListen();
    SUCCEED();
}

/**
 * @tc.name: oobe manager TestResDataAbilityProvider_001
 * @tc.desc: test the interface RegisterObserver of ResDataAbilityProvider
 * @tc.type: FUNC
 * @tc.require: issueI97493
 * @tc.author:zhumingjie
 */
HWTEST_F(OOBEMgrTest, TestResDataAbilityProvider_001, Function | MediumTest | Level0)
{
    ResDataAbilityProvider& resDataProvider = ResDataAbilityProvider::GetInstance();
    ResDataAbilityObserver::UpdateFunc updateFunc = [&]() {};
    int32_t ret = resDataProvider.RegisterObserver(KEYWORD, updateFunc);
    EXPECT_EQ(ret, ERR_OK);
}

/**
 * @tc.name: oobe manager TestResDataAbilityProvider_002
 * @tc.desc: test the interface RegisterObserver of ResDataAbilityProvider
 * @tc.type: FUNC
 * @tc.require: issueI97493
 * @tc.author:zhumingjie
 */
HWTEST_F(OOBEMgrTest, TestResDataAbilityProvider_002, Function | MediumTest | Level0)
{
    ResDataAbilityProvider& resDataProvider = ResDataAbilityProvider::GetInstance();
    ResDataAbilityObserver::UpdateFunc updateFunc = [&]() {};
    int32_t ret = resDataProvider.UnregisterObserver(KEYWORD, updateFunc);
    EXPECT_EQ(ret, ERR_OK);
}
#undef private
#undef protected
} // namespace ResourceSchedule
} // namespace OHOS
