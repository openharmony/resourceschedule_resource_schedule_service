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

#ifndef FOUNDATION_RESOURCESCHEDULE_SERVICES_RESSCHEDMGR_TEST_UNITTEST_INCLUDE_MOCK_H
#define FOUNDATION_RESOURCESCHEDULE_SERVICES_RESSCHEDMGR_TEST_UNITTEST_INCLUDE_MOCK_H

#include <memory>
#include <thread>
#include "plugin_mgr.h"
#include "event_runner.h"
#include <iostream>
#include <dlfcn.h>
#include "datetime_ex.h"
#include <algorithm>

namespace OHOS {
namespace ResourceSchedule {
class MockPluginMgr : public PluginMgr {
public:
    MockPluginMgr() = default;
    const std::string TEST_PREFIX_RES_SWITCH_PATH = "/data/test/resource/resschedfwk/parseswitch/res_sched_plugin_switch.xml";
    const std::string TEST_PREFIX_RES_PATH = "/data/test/resource/resschedfwk/parseconfig/res_sched_config.xml";
    const std::string MOCK_RUNNER_NAME = "mockRssDispatcher";
    const int DISPATCH_WARNING_TIME = 1; // ms
    const int DISPATCH_TIME_OUT = 10; // ms
    enum : int {
        SWITCH_NULL,
        LOAD_CONFIG_FAIL,
        LOAD_CUST_CONFIG_FAIL,
        INIT_SUCCESS
    };
    int initStatus;

    void Init()
    {
        if (pluginSwitch_ != nullptr) {
            initStatus = SWITCH_NULL;
            return;
        }

        if (pluginSwitch_ == nullptr) {
            pluginSwitch_ = std::make_unique<PluginSwitch>();
            bool loadRet = pluginSwitch_->LoadFromConfigFile(TEST_PREFIX_RES_SWITCH_PATH);
            if (!loadRet) {
                initStatus = LOAD_CONFIG_FAIL;
            }
        }

        if (configReader_ == nullptr) {
            configReader_ = std::make_unique<ConfigReader>();
            bool loadRet = configReader_->LoadFromCustConfigFile(TEST_PREFIX_RES_PATH);
            if (!loadRet) {
                initStatus = LOAD_CUST_CONFIG_FAIL;
            }
        }

        if (dispatcherHandler_ == nullptr) {
            dispatcherHandler_ = std::make_shared<AppExecFwk::EventHandler>(AppExecFwk::EventRunner::Create(MOCK_RUNNER_NAME));
        }
        initStatus = INIT_SUCCESS;
    }
};

} // namespace ResourceSchedule
} // namespace OHOS
#endif //FOUNDATION_RESOURCESCHEDULE_SERVICES_RESSCHEDMGR_TEST_UNITTEST_INCLUDEMOCK_H
