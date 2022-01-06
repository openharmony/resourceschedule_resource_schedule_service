/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: Resource schedule client.
 */

#include "res_sched_client.h"
#include "if_system_ability_manager.h"
#include "res_sched_log.h"
#include "res_sched_mgr.h"
#include "system_ability_definition.h"

namespace OHOS {
namespace ResourceSchedule {

ResSchedClient& ResSchedClient::GetInstance()
{
    static ResSchedClient instance;
    return instance;
}

void ResSchedClient::ReportDataInProcess(uint32_t resType, int64_t value, const std::string& payload)
{
    ResSchedMgr::GetInstance().ReportData(resType, value, payload);
}

} // namespace ResourceSchedule
} // namespace OHOS