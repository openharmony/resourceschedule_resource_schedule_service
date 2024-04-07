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
#ifndef RESOURCE_SCHEDULE_SERVICE_RESSCHED_RES_DATA_ABILITY_PROVIDER_H
#define RESOURCE_SCHEDULE_SERVICE_RESSCHED_RES_DATA_ABILITY_PROVIDER_H

#include "datashare_helper.h"
#include "errors.h"
#include <map>
#include "mutex"
#include "res_data_ability_observer.h"

namespace OHOS {
namespace ResourceSchedule {
class ResDataAbilityProvider : public NoCopyable {
public:
    static ResDataAbilityProvider& GetInstance();
    ErrCode RegisterObserver(const std::string& key, ResDataAbilityObserver::UpdateFunc& func);
    ErrCode UnregisterObserver(const std::string& key, ResDataAbilityObserver::UpdateFunc& func);

protected:
    ~ResDataAbilityProvider() override;

private:
    static ResDataAbilityProvider* instance_;
    static std::mutex mutex_;
    static std::map<std::string, sptr<ResDataAbilityObserver>> observers_;
};
} // namespace ResourceSchedule
} // namespace OHOS
#endif // RESOURCE_SCHEDULE_SERVICE_RESSCHED_RES_DATA_ABILITY_PROVIDER_H