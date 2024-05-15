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

#ifndef RESOURCE_SCHEDULE_SERVICE_RESSCHED_COMMON_OOBE_MANAGER_H
#define RESOURCE_SCHEDULE_SERVICE_RESSCHED_COMMON_OOBE_MANAGER_H

#include "data_ability_observer_stub.h"
#include "errors.h"
#include "mutex"
#include "ioobe_task.h"
#include <vector>

namespace OHOS {
namespace ResourceSchedule {

class OOBEManager {
public:
    static OOBEManager& GetInstance();
    bool SubmitTask(const std::shared_ptr<IOOBETask>& task);
    void StartListen();
    ErrCode UnregisterObserver();
    bool GetOOBValue();

private:
    class ResDataAbilityObserver : public AAFwk::DataAbilityObserverStub {
    public:
        ResDataAbilityObserver() = default;
        ~ResDataAbilityObserver()  = default;
        void OnChange() override;

        using UpdateFunc = std::function<void()>;
        void SetUpdateFunc(UpdateFunc& func);
    private:
        UpdateFunc update_ = nullptr;
    };

    bool g_oobeValue = false;
    static std::mutex mutex_;
    static std::vector<std::shared_ptr<IOOBETask>> oobeTasks_;
    static sptr<OOBEManager::ResDataAbilityObserver> observer_;
    OOBEManager();
    ~OOBEManager();
    void Initialize();
    ErrCode RegisterObserver(const std::string& key, ResDataAbilityObserver::UpdateFunc& func);
};
} // ResourceSchedule
} // OHOS
#endif //RESOURCE_SCHEDULE_SERVICE_RESSCHED_COMMON_OOBE_MANAGER_H