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

#include "data_share_utils.h"
#include "res_data_ability_provider.h"
#include "res_sched_log.h"
#include "oobe_manager.h"
#include <vector>
#include "iservice_registry.h"
#include "system_ability_definition.h"

namespace OHOS {
namespace ResourceSchedule {
OOBEManager* OOBEManager::oobeInstance_;
std::mutex OOBEManager::mutex_;
std::vector<std::shared_ptr<IOOBETask>> OOBEManager::oobeTasks_;
namespace {
const std::string KEYWORD = "basic_statement_agreed";
} // namespace

OOBEManager::OOBEManager()
{
}

OOBEManager::~OOBEManager()
{
    RESSCHED_LOGI("OOBEManager has been deconstructed");
}

OOBEManager& OOBEManager::GetInstance()
{
    if (oobeInstance_ == nullptr) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (oobeInstance_ == nullptr) {
            oobeInstance_ = new OOBEManager();
        }
    }
    return *oobeInstance_;
}

void OOBEManager::InitSystemAbilityListener()
{
    if (sysAbilityListener_ != nullptr) {
        return;
    }
    sysAbilityListener_ = new (std::nothrow) SystemAbilityStatusChangeListener();
    if (sysAbilityListener_ == nullptr) {
        RESSCHED_LOGI("Failed to create statusChangeListener due to no memory");
        return;
    }
    sptr<ISystemAbilityManager> systemAbilityManager
        = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (systemAbilityManager == nullptr) {
        RESSCHED_LOGI("systemAbilityManager is null");
        return;
    }
    int32_t ret = systemAbilityManager
        ->SubscribeSystemAbility(DISTRIBUTED_KV_DATA_SERVICE_ABILITY_ID, sysAbilityListener_);
    if (ret != ERR_OK) {
        RESSCHED_LOGI("subscribe system ability id: %{public}d faild", DISTRIBUTED_KV_DATA_SERVICE_ABILITY_ID);
        sysAbilityListener_ = nullptr;
    }
}

void OOBEManager::SystemAbilityStatusChangeListener::OnAddSystemAbility(
    int32_t systemAbilityId, const std::string& deviceId)
{
    RESSCHED_LOGI("OOBEManager add system ability systemAbilityId:%{public}d", systemAbilityId);
    switch (systemAbilityId) {
        case DISTRIBUTED_KV_DATA_SERVICE_ABILITY_ID: {
            if (!OOBEManager::GetInstance().Initialize()) {
                RESSCHED_LOGI("the user does not agreed to the authorization.");
                OOBEManager::GetInstance().StartListen();
            }
            break;
        }
        default: {
            RESSCHED_LOGI("Unhandled systemAbilityId:%{public}d", systemAbilityId);
            break;
        }
    }
}

void OOBEManager::SystemAbilityStatusChangeListener::OnRemoveSystemAbility(
    int32_t systemAbilityId, const std::string& deviceId)
{
    ResDataAbilityObserver::UpdateFunc updateFunc = [&]() {};
    ResDataAbilityProvider::GetInstance().UnregisterObserver(KEYWORD, updateFunc);
    RESSCHED_LOGI("OOBEManager remove system ability systemAbilityId:%{public}d", systemAbilityId);
}

bool OOBEManager::Initialize()
{
    int resultValue = 0;
    auto dataShareUtils = ResourceSchedule::DataShareUtils::GetInstance();
    dataShareUtils.GetValue(KEYWORD, resultValue);
    if (resultValue != 0) {
        return true;
    }
    return false;
}

bool OOBEManager::SubmitTask(const std::shared_ptr<IOOBETask>& task)
{
    if (task == nullptr) {
        return false;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    oobeTasks_.push_back(task);
    return true;
}

void OOBEManager::StartListen()
{
    ResourceSchedule::ResDataAbilityObserver::UpdateFunc updateFunc = [&]() {
        int result = 0;
        ResourceSchedule::DataShareUtils::GetInstance().GetValue(KEYWORD, result);
        if (result != 0) {
            RESSCHED_LOGI("User consent authorization!");
            std::lock_guard<std::mutex> lock(mutex_);
            for (auto task : oobeTasks_) {
                task->ExcutingTask();
            }
        }
    };
    ResDataAbilityProvider::GetInstance().RegisterObserver(KEYWORD, updateFunc);
}
} // namespace ResourceSchedule
} // namespace OHOS
