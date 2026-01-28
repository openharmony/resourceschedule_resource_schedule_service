/*
 * Copyright (c) 2022-2025 Huawei Device Co., Ltd.
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

#include "display_power_event_observer.h"
#include "res_sched_log.h"
#include "res_sched_mgr.h"
#include "res_type.h"
#include "res_value.h"

namespace OHOS {
namespace ResourceSchedule {

void DisplayPowerEventObserver::OnDisplayPowerEvent(Rosen::DisplayPowerEvent event, Rosen::EventStatus status)
{
    switch (event) {
        case Rosen::DisplayPowerEvent::WAKE_UP:
            if (status == Rosen::EventStatus::BEGIN) {
                RESSCHED_LOGI("OnDisplayPowerEvent WAKE_UP BEGIN");
                nlohmann::json payload;
                ResSchedMgr::GetInstance().ReportData(ResType::RES_TYPE_DISPLAY_POWER_WAKE_UP,
                    ResType::WakeUpStatus::WAKE_UP_START, payload);
            }
            break;
        default:
            break;
    }
}
} // namespace ResourceSchedule
} // namespace OHOS