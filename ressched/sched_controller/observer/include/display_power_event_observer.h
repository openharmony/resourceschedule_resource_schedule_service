/*
+ * Copyright (c) 2025 Huawei Technologies Co., Ltd. 2025-2025. All right reserved.
+ */
 
#ifdef RESSCHED_SCHED_CONTROLLER_OBSERVER_INCLUDE_INCLUDE_DISPLAY_POWER_EVENT_OBSERVER_H
#define RESSCHED_SCHED_CONTROLLER_OBSERVER_INCLUDE_INCLUDE_DISPLAY_POWER_EVENT_OBSERVER_H
#include "display_manager.h"

namespace OHOS {
namespace ResourceSchedule {

class DisplayPowerEventObserver : public Rosen::IDisplayPowerEventListenser {
public:
    void OnDisplayPowerEvent(Rosen::DisplayPowerEvent event, Rosen::EventStatus status) override;
};

}
}

#endif