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

#include "res_sched_client.h"
#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "ires_sched_service.h"
#include "res_sched_log.h"
#include "res_sched_errors.h"
#include "res_sched_mgr.h"
#include "system_ability_definition.h"

namespace OHOS {
namespace ResourceSchedule {

#define CHECK_REMOTE_OBJECT_AND_RETURN(object, value)     \
    if (!object) {                                        \
        if (Connect() != ERR_OK) {                        \
            return value;                                 \
        }                                                 \
    }                                                     \

ResSchedClient& ResSchedClient::GetInstance()
{
    static ResSchedClient instance;
    return instance;
}

void ResSchedClient::ReportDataInProcess(uint32_t resType, int64_t value, const std::string& payload)
{
    ResSchedMgr::GetInstance().ReportData(resType, value, payload);
}

void ResSchedClient::ReportData(uint32_t resType, int64_t value, const std::string& payload)
{
    CHECK_REMOTE_OBJECT_AND_RETURN(remoteObject_, );
    sptr<IResSchedService> rss = iface_cast<IResSchedService>(remoteObject_);
    rss->ReportData(resType, value, payload);
}

ErrCode ResSchedClient::Connect()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (remoteObject_ != nullptr) {
        return ERR_OK;
    }

    sptr<ISystemAbilityManager> systemManager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (systemManager == nullptr) {
        RESSCHED_LOGE("ResSchedClient::Fail to get registry.");
        return GET_RES_SCHED_SERVICE_FAILED;
    }

    remoteObject_ = systemManager->GetSystemAbility(RES_SCHED_SYS_ABILITY_ID);
    if (remoteObject_ == nullptr) {
        RESSCHED_LOGE("ResSchedClient::Fail to connect resource schedule service.");
        return GET_RES_SCHED_SERVICE_FAILED;
    }
    RESSCHED_LOGD("ResSchedClient::Connect resource schedule service success.");
    return ERR_OK;
}
} // namespace ResourceSchedule
} // namespace OHOS