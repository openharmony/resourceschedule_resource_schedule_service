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

#include "gtest/gtest.h"
#include "res_data.h"
#include "res_type.h"
#define private public
#define protected public
#include "socperf_plugin.h"
#undef private
#undef protected

namespace OHOS {
namespace ResourceSchedule {
using namespace testing;
using namespace testing::ext;
using namespace ResType;
using namespace std;

class SocPerfPluginTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void SocPerfPluginTest::SetUpTestCase()
{
}

void SocPerfPluginTest::TearDownTestCase()
{
}

void SocPerfPluginTest::SetUp()
{
    SocPerfPlugin::GetInstance().Init();
}

void SocPerfPluginTest::TearDown()
{
    SocPerfPlugin::GetInstance().Disable();
}


/**
 * @tc.name: HandleAppStateChangeTest
 * @tc.desc: Test whether HandleAppStateChange interface is normal
 * @tc.type: FUNC
 */
HWTEST_F(SocPerfPluginTest, HandleLoadPageTest, Function | MediumTest | Level0)
{
    nlohmann::json payload;
    shared_ptr<ResData> resData = share_ptr<ResData>(RES_TYPE_LOAD_PAGE, LoadPageType::LOAD_PAGE_START, payload);
    SocPerfPlugin::GetInstance().HandleLoadPage(resData);

    resData->value = LoadPageType::LOAD_PAGE_COMPLETE;
    SocPerfPlugin::GetInstance().HandleLoadPage(resData);
}

/**
 * @tc.name: HandleAppStateChangeTest
 * @tc.desc: Test whether HandleAppStateChange interface is normal
 * @tc.type: FUNC
 */
HWTEST_F(SocPerfPluginTest, HandleRemoteAnimationTest, Function | MediumTest | Level0)
{
    nlohmann::json payload;
    shared_ptr<ResData> resData =
        share_ptr<ResData>(RES_TYPE_SHOW_REMOTE_ANIMATION, ShowRemoteAnimationStatus::ANIMATION_BEGIN, payload);
    SocPerfPlugin::GetInstance().HandleRemoteAnimation(resData);

    resData->value = ShowRemoteAnimationStatus::ANIMATION_END;
    SocPerfPlugin::GetInstance().HandleRemoteAnimation(resData);
}
} // namespace ResourceSchedule
} // namespace OHOS
