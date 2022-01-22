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

#include "socperf_plugin_handler.h"

namespace OHOS {
namespace ResourceSchedule {
using namespace ResType;

SocPerfPluginHandler::SocPerfPluginHandler(
    const std::shared_ptr<AppExecFwk::EventRunner>& runner) : AppExecFwk::EventHandler(runner)
{
}

SocPerfPluginHandler::~SocPerfPluginHandler()
{
}

void SocPerfPluginHandler::ProcessEvent(const AppExecFwk::InnerEvent::Pointer &event)
{
    switch (event->GetInnerEventId()) {
        case INNER_EVENT_ID_SOC_PERF_PLUGIN_DISPATCH: {
            DispatchResource(event->GetSharedObject<ResData>());
            break;
        }
        default: {
            break;
        }
    }
}

void SocPerfPluginHandler::DispatchResource(const std::shared_ptr<ResData>& data)
{
    RESSCHED_LOGI("SocPerfPluginHandler::DispatchResource status type %{public}u", data->resType);
    if (data->resType == RES_TYPE_SCREEN_STATUS) {
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequest(PERF_REQUEST_CMD_ID_SCREEN_STATUS, "");
        return;
    }
}
} // namespace ResourceSchedule
} // namespace OHOS