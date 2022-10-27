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

#ifndef RESSCHED_SERVICES_RESSCHEDMGR_TEST_UNITTEST_INCLUDE_MOCK_H
#define RESSCHED_SERVICES_RESSCHEDMGR_TEST_UNITTEST_INCLUDE_MOCK_H

#include <algorithm>
#include <dlfcn.h>
#include <iostream>
#include <memory>
#include <thread>
#include "datetime_ex.h"
#include "event_runner.h"
#include "plugin_mgr.h"

namespace OHOS {
namespace ResourceSchedule {
class MockPluginMgr : public PluginMgr {
public:
    MockPluginMgr() = default;
    const std::string TEST_PREFIX_RES_SWITCH_PATH =
        "/data/test/resource/resschedfwk/parseswitch/res_sched_plugin_switch.xml";
    const std::string TEST_PREFIX_RES_PATH = "/data/test/resource/resschedfwk/parseconfig/res_sched_config.xml";
    const std::string MOCK_RUNNER_NAME = "mockRssDispatcher";
    const int32_t DISPATCH_WARNING_TIME = 1; // ms
    const int32_t DISPATCH_TIME_OUT = 10; // ms
    enum : int32_t {
        SWITCH_NULL,
        LOAD_CONFIG_FAIL,
        LOAD_CUST_CONFIG_FAIL,
        INIT_SUCCESS
    };
    int32_t initStatus;

    void Init()
    {
        if (pluginSwitch_) {
            initStatus = SWITCH_NULL;
            return;
        }

        if (!pluginSwitch_) {
            pluginSwitch_ = std::make_unique<PluginSwitch>();
            bool loadRet = pluginSwitch_->LoadFromConfigFile(TEST_PREFIX_RES_SWITCH_PATH);
            if (!loadRet) {
                initStatus = LOAD_CONFIG_FAIL;
            }
        }

        if (!configReader_) {
            configReader_ = std::make_unique<ConfigReader>();
            bool loadRet = configReader_->LoadFromCustConfigFile(TEST_PREFIX_RES_PATH);
            if (!loadRet) {
                initStatus = LOAD_CUST_CONFIG_FAIL;
            }
        }

        initStatus = INIT_SUCCESS;
    }
};
} // namespace ResourceSchedule
} // namespace OHOS

#endif // RESSCHED_SERVICES_RESSCHEDMGR_TEST_UNITTEST_INCLUDEMOCK_H
