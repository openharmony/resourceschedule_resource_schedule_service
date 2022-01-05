/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: resource schedule manager class
 */

#ifndef FOUNDATION_RESOURCESCHEDULE_SERVICES_RESSCHEDMGR_RESSCHEDFWK_INCLUDE_RES_SCHED_MGR_H
#define FOUNDATION_RESOURCESCHEDULE_SERVICES_RESSCHEDMGR_RESSCHEDFWK_INCLUDE_RES_SCHED_MGR_H

#include <sys/types.h>
#include <string>
#include "event_handler.h"
#include "single_instance.h"

namespace OHOS {
namespace ResourceSchedule {
class ResSchedMgr {
    DECLARE_SINGLE_INSTANCE(ResSchedMgr);

public:
    /**
     * Init resource schedule manager.
     */
    void Init();

    /**
     * Stop resource schedule manager.
     */
    void Stop();

    /**
     * Report data from other modules, will report resource data async.
     *
     * @param resType Resource type
     * @param value bit64 content.
     * @param payload Extra content.
     */
    void ReportData(uint32_t resType, int64_t value = 0, const std::string& payload = "");

private:
    // main handler, use for report data
    std::shared_ptr<AppExecFwk::EventHandler> mainHandler_;
};
} // namespace ResourceSchedule
} // namespace OHOS

#endif // FOUNDATION_RESOURCESCHEDULE_SERVICES_RESSCHEDMGR_RESSCHEDFWK_INCLUDE_RES_SCHED_MGR_H