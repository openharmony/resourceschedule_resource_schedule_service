/*
 * Copyright (c) 2025 Huawei Technologies Co., Ltd. 2025-2025. All right reserved.
 */

#include "display_power_event_observer.h"
#include "res_sched_log.h"
#include "res_sched_mgr.h"
#include "res_type.h"

namespace OHOS{
namespace ResourceSchedule{

void DisplayPowerEventObserver::OnDisplayPowerEvent(Rosen::DisplayPowerEvent event, Rosen::EventStatus status)
{
    switch (event) {
        case Rosen::DisplayPowerEvent::WAKE_UP:
            if (status == Rosen::EventStatus::BEGIN){
                RESSCHED_LOGI("OnDisplayPowerEvent WAKE_UP BEGIN");
                nlohmann::json payload;
                ResScheMgr::GetInstance().ReportData(ResType::RES_TYPE_DISPLAY_POWER_WAKE_UP,
                                                        0, payload);
            }
            break;
        default:
            break;
    }
}
} // namespace ResourceSchedule
} // namespace OHOS