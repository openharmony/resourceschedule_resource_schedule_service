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
#include "res_sched_log.h"
#include "res_sched_errors.h"
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
    RESSCHED_LOGI("ResSchedClient::ReportDataInProcess resType = %{public}d, value = %{public}lld,"
                  " payload = %{public}s", resType, value, payload.c_str());
    ResSchedMgr::GetInstance().ReportData(resType, value, payload);
}

void ResSchedClient::ReportData(uint32_t resType, int64_t value, const std::string& payload)
{
    if (TryConnect() != ERR_OK) {
        return;
    }
    rss_->ReportData(resType, value, payload);
}

ErrCode ResSchedClient::TryConnect()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (rss_ != nullptr) {
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

    rss_ = iface_cast<IResSchedService>(remoteObject_);
    if (rss_ == nullptr) {
        return GET_RES_SCHED_SERVICE_FAILED;
    }
    recipient_ = new ResSchedDeathRecipient(*this);
    if (recipient_ == nullptr) {
        return GET_RES_SCHED_SERVICE_FAILED;
    }
    rss_->AsObject()->AddDeathRecipient(recipient_);
    RESSCHED_LOGD("ResSchedClient::Connect resource schedule service success.");
    return ERR_OK;
}

void ResSchedClient::StopRemoteObject()
{
    if ((rss_ != nullptr) && (rss_->AsObject() != nullptr)) {
        rss_->AsObject()->RemoveDeathRecipient(recipient_);
    }
    rss_ = nullptr;
}

ResSchedClient::ResSchedDeathRecipient::ResSchedDeathRecipient(ResSchedClient &resSchedClient)
    : resSchedClient_(resSchedClient) {};

ResSchedClient::ResSchedDeathRecipient::~ResSchedDeathRecipient() {};

void ResSchedClient::ResSchedDeathRecipient::OnRemoteDied(const wptr<IRemoteObject> &remote)
{
    resSchedClient_.StopRemoteObject();
}
} // namespace ResourceSchedule
} // namespace OHOS+