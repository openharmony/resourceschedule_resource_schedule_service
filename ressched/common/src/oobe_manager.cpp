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
ffrt::recursive_mutex OOBEManager::mutex_;
std::vector<std::shared_ptr<IOOBETask>> OOBEManager::oobeTasks_;
std::vector<std::function<void()>> OOBEManager::dataShareFunctions_;
sptr<OOBEManager::ResDataAbilityObserver> OOBEManager::observer_ = nullptr;
namespace {
const std::string KEYWORD = "basic_statement_agreed";
const std::int32_t MAX_RETRY_COUNT = 30; // 2s尝试一次，尝试30此，1min后停止轮询
const std::int32_t DELAYTIME_US = 2 * 1000 * 1000;
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
    std::lock_guard<ffrt::recursive_mutex> lock(mutex_);
    RESSCHED_LOGE("get oobe value, oobe value = %{public}d", oobeValue_);
    return (oobeValue_ == OOBEVALUE::IS_TRUE);
}

ErrCode OOBEManager::RegisterObserver(const std::string& key, const ResDataAbilityObserver::UpdateFunc& func)
{
    std::lock_guard<ffrt::recursive_mutex> lock(mutex_);
    if (!DataShareUtils::GetInstance().IsConnectDataShareSucc()) {
        if (DataShareUtils::GetInstance().GetDataShareReadyFlag()) {
            RESSCHED_LOGE("dataShare is ready but conntect fail");
            return ERR_NO_INIT;
        }
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
    ffrt::task_attr taskAttr;
    taskAttr.delay(DELAYTIME_US);
    ffrt::submit([]() {
        std::uint32_t retry_count = 0;
        OOBEManager::GetInstance().CheckOobeValue(retry_count);
    },
        taskAttr);
    return ERR_OK;
}

void OOBEManager::CheckOobeValue (int32_t count) 
{
    int32_t ret = -1;
    bool needCheck = false;
    {
        std::lock_guard<ffrt::recursive_mutex> lock(mutex_);
        needCheck = (oobeValue_ == OOBEVALUE::INVALID && count < MAX_RETRY_COUNT);
    }
    if (needCheck) {
        RESSCHED_LOGW("oobeValue is invalid, retry to get oobe value");
        ret = FlushOobeValue();
    } else {
        RESSCHED_LOGI("oobeValue is %{public}d, retry count is %{public}d", oobeValue_, count);
        return;
    }
    if (ret == ERR_INVALID_OPERATION) {
        ffrt::task_attr taskAttr;
        taskAttr.delay(DELAYTIME_US);
        ffrt::submit([cnt = count + 1]() {OOBEManager::GetInstance().CheckOobeValue(cnt);}, taskAttr);
    }
}

void OOBEManager::ReRegisterObserver(const std::string& key, const ResDataAbilityObserver::UpdateFunc& func)
{
    FlushOobeValue();
    if (oobeValue_ == OOBEVALUE::IS_TRUE) {
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
    RESSCHED_LOGI("succeed to unregister observer of uri=%{public}s", uri.ToString().c_str());
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
    ErrCode res = ResourceSchedule::DataShareUtils::GetInstance().GetValue(KEYWORD, resultValue);
    if (res != ERR_OK) {
        return;
    }
    if (resultValue != 0) {
        oobeValue_ = OOBEVALUE::IS_TRUE;
    } else {
        oobeValue_ = OOBEVALUE::IS_FALSE;
    }
}

bool OOBEManager::SubmitTask(const std::shared_ptr<IOOBETask>& task)
{
    std::lock_guard<ffrt::recursive_mutex> lock(mutex_);
    if (task == nullptr) {
        RESSCHED_LOGE("Bad task passed!");
        return false;
    }
    if (oobeValue_ == OOBEVALUE::IS_TRUE) {
        task->ExcutingTask();
        return true;
    }
    oobeTasks_.push_back(task);
    return true;
}

void OOBEManager::StartListen()
{
    FlushOobeValue();
    if (oobeValue_ == OOBEVALUE::IS_TRUE) {
        return;
    }
    OOBEManager::ResDataAbilityObserver::UpdateFunc updateFunc = [&]() {
        FlushOobeValue();
    };
    RegisterObserver(KEYWORD, updateFunc);
}

ErrCode OOBEManager::FlushOobeValue()
{
    int result = 0;
    ErrCode res = ResourceSchedule::DataShareUtils::GetInstance().GetValue(KEYWORD, result);
    if (res != ERR_OK) {
        return res;
    }
    if (result != 0) {
        std::lock_guard<ffrt::recursive_mutex> lock(mutex_);
        oobeValue_ = OOBEVALUE::IS_TRUE;
        for (auto task : oobeTasks_) {
            task->ExcutingTask();
        }
        std::vector <std::shared_ptr<IOOBETask>>().swap(oobeTasks_);
    } else {
        oobeValue_ = OOBEVALUE::IS_FALSE;
    }
    return res;
}

void OOBEManager::OnReceiveDataShareReadyCallBack()
{
    ffrt::submit([]() {
        DataShareUtils::GetInstance().SetDataShareReadyFlag(true);
        OOBEManager::GetInstance().ExecuteDataShareFunction();
        });
}

void OOBEManager::ExecuteDataShareFunction()
{
    std::vector<std::function<void()>> dataShareFunctions;
    {
        std::lock_guard<ffrt::recursive_mutex> lock(mutex_);
        dataShareFunctions = std::move(dataShareFunctions_);
    }
    for (auto function : dataShareFunctions) {
        function();
    }
    RESSCHED_LOGI("execute data share function success");
}
} // namespace ResourceSchedule
} // namespace OHOS
