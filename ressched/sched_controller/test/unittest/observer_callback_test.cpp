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

#include <gtest/gtest.h>

#include "application_info.h"
#include "res_sched_log.h"
#include "net_supplier_info.h"

#include "audio_observer.h"
#include "observer_manager.h"
#include "ressched_utils.h"
#include "res_type.h"

namespace OHOS {
namespace ResourceSchedule {
class ObserverCallbackTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() {}
    void TearDown() {}
};

/**
 * @tc.name: registeraudiocallback_00100
 * @tc.desc: test registeraudiocallback
 * @tc.type: FUNC
 * @tc.require: SR000H0H5E AR000H0ROH
 */
HWTEST_F(ObserverCallbackTest, registeraudiocallback_00100, testing::ext::TestSize.Level1)
{
    ObserverManager::GetInstance().InitAudioObserver();
    
}

/**
 * @tc.name: registeraudiocallback_00100
 * @tc.desc: test registeraudiocallback
 * @tc.type: FUNC
 * @tc.require: SR000H0H5E AR000H0ROH
 */
HWTEST_F(ObserverCallbackTest, unregisteraudiocallback_00101, testing::ext::TestSize.Level1)
{
    ObserverManager::GetInstance().DisableAudioObserver();
    
}

/**
 * @tc.name: registerphonycallback_00101
 * @tc.desc: test dispatching ResType::RES_TYPE_WIFI_CONNECT_STATE_CHANGE
 *           when receive COMMON_EVENT_CONNECTIVITY_CHANGE and code is NET_CONN_STATE_CONNECTED.
 * @tc.type: FUNC
 * @tc.require: SR000H0H5E AR000H0ROH
 */
HWTEST_F(EventControllerTest, registerphonycallback_00102, testing::ext::TestSize.Level1)
{
    
}

}
}