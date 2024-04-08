/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "datashare_errno.h"
#include "datashare_predicates.h"
#include "datashare_result_set.h"
#include "data_share_utils.h"
#include "datashare_values_bucket.h"
#include "ipc_skeleton.h"
#include "iservice_registry.h"
#include "res_data_ability_provider.h"
#include "res_sched_log.h"
#include "system_ability_definition.h"

namespace OHOS {
namespace ResourceSchedule {
ResDataAbilityProvider* ResDataAbilityProvider::instance_;
std::mutex ResDataAbilityProvider::mutex_;
std::map<std::string, sptr<ResDataAbilityObserver>> ResDataAbilityProvider::observers_;

ResDataAbilityProvider::~ResDataAbilityProvider()
{
    RESSCHED_LOGI("ResDataAbilityProvider has been deconstructed");
}

ResDataAbilityProvider& ResDataAbilityProvider::GetInstance()
{
    if (instance_ == nullptr) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (instance_ == nullptr) {
            instance_ = new ResDataAbilityProvider();
        }
    }
    return *instance_;
}

ErrCode ResDataAbilityProvider::RegisterObserver(const std::string& key, ResDataAbilityObserver::UpdateFunc& func)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto iter = observers_.find(key);
    if (iter != observers_.end()) {
        return ERR_INVALID_OPERATION;
    }
    std::string callingIdentity = IPCSkeleton::ResetCallingIdentity();
    auto uri = DataShareUtils::GetInstance().AssembleUri(key);
    auto helper = DataShareUtils::GetInstance().CreateDataShareHelper();
    if (helper == nullptr) {
        IPCSkeleton::SetCallingIdentity(callingIdentity);
        return ERR_NO_INIT;
    }
    sptr<ResDataAbilityObserver> observer = new ResDataAbilityObserver();
    observer->SetUpdateFunc(func);
    observers_.insert(std::map<std::string, sptr<ResDataAbilityObserver>>::value_type (key, observer));
    helper->RegisterObserver(uri, observer);
    DataShareUtils::GetInstance().ReleaseDataShareHelper(helper);
    IPCSkeleton::SetCallingIdentity(callingIdentity);
    RESSCHED_LOGI("succeed to register observer of uri=%{public}s", uri.ToString().c_str());
    return ERR_OK;
}

ErrCode ResDataAbilityProvider::UnregisterObserver(const std::string& key, ResDataAbilityObserver::UpdateFunc& func)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto iter = observers_.find(key);
    if (iter == observers_.end()) {
        return ERR_INVALID_OPERATION;
    }
    std::string callingIdentity = IPCSkeleton::ResetCallingIdentity();
    auto uri = DataShareUtils::GetInstance().AssembleUri(key);
    auto helper = DataShareUtils::GetInstance().CreateDataShareHelper();
    if (helper == nullptr) {
        IPCSkeleton::SetCallingIdentity(callingIdentity);
        return ERR_NO_INIT;
    }
    helper->UnregisterObserver(uri, iter->second);
    DataShareUtils::GetInstance().ReleaseDataShareHelper(helper);
    IPCSkeleton::SetCallingIdentity(callingIdentity);
    RESSCHED_LOGI("succeed to register observer of uri=%{public}s", uri.ToString().c_str());
    observers_.earse(key);
    return ERR_OK;
}
} // namespace ResourceSchedule
} // namespace OHOS