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

#ifndef SOC_PERF_PLUGIN_HANDLER_H
#define SOC_PERF_PLUGIN_HANDLER_H

#include "event_handler.h"
#include "event_runner.h"
#include "res_data.h"
#include "res_sched_log.h"
#include "socperf_client.h"
#include "res_type.h"

namespace OHOS {
namespace ResourceSchedule {
class SocPerfPluginHandler : public AppExecFwk::EventHandler {
public:
    explicit SocPerfPluginHandler(const std::shared_ptr<AppExecFwk::EventRunner>& runner);
    virtual ~SocPerfPluginHandler() override;
    virtual void ProcessEvent(const AppExecFwk::InnerEvent::Pointer& event) override;

private:
    void DispatchResource(const std::shared_ptr<ResData>& data);
};

namespace {
    const int INNER_EVENT_ID_SOC_PERF_PLUGIN_DISPATCH = 0;
    const int PERF_REQUEST_CMD_ID_SCREEN_STATUS = 10000;
}
} // namespace ResourceSchedule
} // namespace OHOS

#endif // SOC_PERF_PLUGIN_HANDLER_H