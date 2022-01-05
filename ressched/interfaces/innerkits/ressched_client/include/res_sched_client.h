/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: Definiton of resource schedule client.
 */

#ifndef FOUNDATION_RESOURCESCHEDULE_INTERFACES_INNERKITS_RESSCHED_CLIENT_INCLUDE_RES_SCHED_CLIENT_H
#define FOUNDATION_RESOURCESCHEDULE_INTERFACES_INNERKITS_RESSCHED_CLIENT_INCLUDE_RES_SCHED_CLIENT_H

#include "iremote_object.h"

namespace OHOS {
namespace ResourceSchedule {
/*
 * this class wraped the functions of IResSchedService,effect is the same.
 * but through ResSchedClient, you don't need to get IResSchedService from samgr,
 * just use the functions is ok.
 */
class ResSchedClient {
public:
    /**
     * Only need one client connect to ResSchedService, singleton pattern.
     *
     * @return Returns the only one implement of ResSchedClient.
     */
    static ResSchedClient& GetInstance();

    /**
     * Report source data to resource schedule service in the same process.
     *
     * @param resType Resource type, all of the type have listed in res_type.h.
     * @param value Value of resource type, defined by the developers.
     * @param payload is empty is valid. The interface only used for In-Process call.
     * ATTENTION: payload is empty is valid. The interface only used for In-Process call.
     */
    void ReportDataInProcess(uint32_t resType, int64_t value, const std::string& payload);

protected:
    ResSchedClient() = default;
    virtual ~ResSchedClient() = default;

private:
    DISALLOW_COPY_AND_MOVE(ResSchedClient);
};

} // namespace ResourceSchedule
} // namespace OHOS

#endif // FOUNDATION_RESOURCESCHEDULE_INTERFACES_INNERKITS_RESSCHED_CLIENT_INCLUDE_RES_SCHED_CLIENT_H