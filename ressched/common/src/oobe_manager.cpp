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
sptr<OOBEManager::ResDataAbilityObserver> OOBEManager::observer_;
namespace {
const std::string KEYWORD = "basic_statement_agreed";
} // namespace

OOBEManager::OOBEManager()
{
    Initialize();
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

ErrCode OOBEManager::RegisterObserver(const std::string& key, ResDataAbilityObserver::UpdateFunc& func)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto uri = DataShareUtils::GetInstance().AssembleUri(key);
    auto helper = DataShareUtils::GetInstance().CreateDataShareHelper();
    if (helper == nullptr) {
        RESSCHED_LOGE("RegisterObserver: helper does not created!");
        return ERR_NO_INIT;
    }
    if (!observer_) {
        helper->UnregisterObserver(uri, observer_);
        observer_ = nullptr;
    }
    observer_ = new ResDataAbilityObserver();
    observer_->SetUpdateFunc(func);
    helper->RegisterObserver(uri, observer_);
    DataShareUtils::GetInstance().ReleaseDataShareHelper(helper);
    RESSCHED_LOGI("succeed to register observer of uri=%{public}s", uri.ToString().c_str());
    return ERR_OK;
}
    
ErrCode OOBEManager::UnregisterObserver(const std::string& key)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto uri = DataShareUtils::GetInstance().AssembleUri(key);
    auto helper = DataShareUtils::GetInstance().CreateDataShareHelper();
    if (helper == nullptr) {
        RESSCHED_LOGE("UnregisterObserver: helper does not created!");
        return ERR_NO_INIT;
    }
    helper->UnregisterObserver(uri, observer_);
    DataShareUtils::GetInstance().ReleaseDataShareHelper(helper);
    observer_ = nullptr;
    RESSCHED_LOGI("succeed to register observer of uri=%{public}s", uri.ToString().c_str());
    return ERR_OK;
}

void OOBEManager::ResDataAbilityObserver::OnChange()
{
    if (update_) {
        update_();
    }
}

void OOBEManager::ResDataAbilityObserver::SetUpdateFunc(UpdateFunc& func)
{
    update_ = func;
}

void OOBEManager::InitSysAbilityListener()
{
    if (sysAbilityListener_ != nullptr) {
        return;
    }
    sysAbilityListener_ = new (std::nothrow) SystemAbilityStatusChangeListener();
    if (sysAbilityListener_ == nullptr) {
        RESSCHED_LOGE("Failed to create statusChangeListener due to no memory");
        return;
    }
    sptr<ISystemAbilityManager> systemAbilityManager
        = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (systemAbilityManager == nullptr) {
        sysAbilityListener_ = nullptr;
        RESSCHED_LOGE("systemAbilityManager is null");
        return;
    }
    int32_t ret = systemAbilityManager->
        SubscribeSystemAbility(DISTRIBUTED_KV_DATA_SERVICE_ABILITY_ID, sysAbilityListener_);
    if (ret != ERR_OK) {
        RESSCHED_LOGE("subscribe system ability id: %{public}d faild", DISTRIBUTED_KV_DATA_SERVICE_ABILITY_ID);
        sysAbilityListener_ = nullptr;
    }
}

void OOBEManager::SystemAbilityStatusChangeListener::OnAddSystemAbility(
    int32_t systemAbilityId, const std::string& deviceId)
{
    switch (systemAbilityId) {
        case DISTRIBUTED_KV_DATA_SERVICE_ABILITY_ID: {
            RESSCHED_LOGI("DataShare SystemAbility start successfully! begin to listen Database.");
            OOBEManager::GetInstance().StartListen();
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
    switch (systemAbilityId) {
        case DISTRIBUTED_KV_DATA_SERVICE_ABILITY_ID: {
            RESSCHED_LOGI("Begin to UnregisterObserver for Database.");
            OOBEManager::GetInstance().UnregisterObserver(KEYWORD);
            break;
        }
        default: {
            RESSCHED_LOGI("Unhandled systemAbilityId:%{public}d", systemAbilityId);
            break;
        }
    }
}

void OOBEManager::Initialize()
{
    InitSysAbilityListener();
    int resultValue = 0;
    auto dataShareUtils = ResourceSchedule::DataShareUtils::GetInstance();
    dataShareUtils.GetValue(KEYWORD, resultValue);
    if (resultValue != 0) {
        g_oobeValue = true;
    }
}

bool OOBEManager::SubmitTask(const std::shared_ptr<IOOBETask>& task)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (task == nullptr) {
        RESSCHED_LOGE("Bad task passed!");
        return false;
    }
    if (g_oobeValue == true) {
        task->ExcutingTask();
        RESSCHED_LOGD("OOBE agreed, task has excuted!");
        return true;
    }
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
            g_oobeValue = true;
            for (auto task : oobeTasks_) {
                task->ExcutingTask();
            }
        }else {
            g_oobeValue = false;
        }
    };
    RegisterObserver(KEYWORD, updateFunc);
}
} // namespace ResourceSchedule
} // namespace OHOS
