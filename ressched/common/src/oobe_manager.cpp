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

#include "oobe_datashare_utils.h"
#include "res_sched_log.h"
#include "oobe_manager.h"
#include <functional>
#include <vector>
#include "ipc_skeleton.h"

namespace OHOS {
namespace ResourceSchedule {
std::mutex OOBEManager::mutex_;
std::vector<std::shared_ptr<IOOBETask>> OOBEManager::oobeTasks_;
std::vector<std::function<void()>> OOBEManager::dataShareFunctions_;
sptr<OOBEManager::ResDataAbilityObserver> OOBEManager::observer_ = nullptr;
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
    static OOBEManager instance;
    return instance;
}

bool OOBEManager::GetOOBValue()
{
    std::lock_guard<std::mutex> lock(mutex_);
    return g_oobeValue;
}

ErrCode OOBEManager::RegisterObserver(const std::string& key, const ResDataAbilityObserver::UpdateFunc& func)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!DataShareUtils::GetInstance().GetDataShareReadyFlag()) {
        RESSCHED_LOGE("RegisterObserver: dataShare is not ready!");
        std::function dataShareFunction = [key, func, this]() {
            ReRegisterObserver(key, func);
        };
        dataShareFunctions_.push_back(dataShareFunction);
        return ERR_NO_INIT;
    }
    
    std::string callingIdentity = IPCSkeleton::ResetCallingIdentity();
    auto uri = DataShareUtils::GetInstance().AssembleUri(key);
    auto helper = DataShareUtils::GetInstance().CreateDataShareHelper();
    if (helper == nullptr) {
        IPCSkeleton::SetCallingIdentity(callingIdentity);
        RESSCHED_LOGE("RegisterObserver: helper does not created!");
        return ERR_NO_INIT;
    }
    if (observer_ != nullptr) {
        RESSCHED_LOGE("Secondary RegisterObserver!");
        UnregisterObserver();
    }
    observer_ = new (std::nothrow)ResDataAbilityObserver();
    if (observer_ == nullptr) {
        IPCSkeleton::SetCallingIdentity(callingIdentity);
        return ERR_INVALID_OPERATION;
    }
    observer_->SetUpdateFunc(func);
    helper->RegisterObserver(uri, observer_);
    DataShareUtils::GetInstance().ReleaseDataShareHelper(helper);
    IPCSkeleton::SetCallingIdentity(callingIdentity);
    RESSCHED_LOGI("succeed to register observer of uri=%{public}s", uri.ToString().c_str());
    return ERR_OK;
}

void OOBEManager::ReRegisterObserver(const std::string& key, const ResDataAbilityObserver::UpdateFunc& func)
{
    int resultValue = 0;
    ResourceSchedule::DataShareUtils::GetInstance().GetValue(key, resultValue);
    if (resultValue != 0) {
        func();
        return;
    }
    RegisterObserver(key, func);
}
    
ErrCode OOBEManager::UnregisterObserver()
{
    std::string callingIdentity = IPCSkeleton::ResetCallingIdentity();
    auto uri = DataShareUtils::GetInstance().AssembleUri(KEYWORD);
    auto helper = DataShareUtils::GetInstance().CreateDataShareHelper();
    if (helper == nullptr) {
        IPCSkeleton::SetCallingIdentity(callingIdentity);
        RESSCHED_LOGE("UnregisterObserver: helper does not created!");
        return ERR_NO_INIT;
    }
    helper->UnregisterObserver(uri, observer_);
    DataShareUtils::GetInstance().ReleaseDataShareHelper(helper);
    observer_ = nullptr;
    IPCSkeleton::SetCallingIdentity(callingIdentity);
    RESSCHED_LOGI("succeed to register observer of uri=%{public}s", uri.ToString().c_str());
    return ERR_OK;
}

void OOBEManager::ResDataAbilityObserver::OnChange()
{
    if (update_) {
        update_();
    }
}

void OOBEManager::ResDataAbilityObserver::SetUpdateFunc(const UpdateFunc& func)
{
    update_ = func;
}

void OOBEManager::Initialize()
{
    int resultValue = 0;
    ResourceSchedule::DataShareUtils::GetInstance().GetValue(KEYWORD, resultValue);
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
    if (g_oobeValue) {
        task->ExcutingTask();
        return true;
    }
    oobeTasks_.push_back(task);
    return true;
}

void OOBEManager::StartListen()
{
    int resultValue = 0;
    ResourceSchedule::DataShareUtils::GetInstance().GetValue(KEYWORD, resultValue);
    if (resultValue != 0) {
        std::lock_guard<std::mutex> lock(mutex_);
        g_oobeValue = true;
        for (auto task : oobeTasks_) {
            task->ExcutingTask();
        }
        return;
    }
    OOBEManager::ResDataAbilityObserver::UpdateFunc updateFunc = [&]() {
        int result = 0;
        ResourceSchedule::DataShareUtils::GetInstance().GetValue(KEYWORD, result);
        if (result != 0) {
            RESSCHED_LOGI("User consent authorization!");
            std::lock_guard<std::mutex> lock(mutex_);
            g_oobeValue = true;
            for (auto task : oobeTasks_) {
                task->ExcutingTask();
            }
        } else {
            RESSCHED_LOGI("User does not consent authorization!");
            g_oobeValue = false;
        }
    };
    RegisterObserver(KEYWORD, updateFunc);
}

void OOBEManager::OnReceiveDataShareReadyCallBack()
{
    for (auto function : dataShareFunctions_) {
        function();
    }
    dataShareFunctions_.clear();
}
} // namespace ResourceSchedule
} // namespace OHOS
