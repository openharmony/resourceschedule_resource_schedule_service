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

#ifndef RESOURCE_SCHEDULE_SERVICE_OOBE_MANAGER_H
#define RESOURCE_SCHEDULE_SERVICE_OOBE_MANAGER_H

#include "errors.h"
#include "mutex"
#include <vector>
#include "ioobe_task.h"
#include "if_system_ability_manager.h"
#include "system_ability_status_change_stub.h"

namespace OHOS {
namespace ResourceSchedule {

class OOBEManager {
public:
    static OOBEManager& GetInstance();
    bool SubmitTask(const std::shared_ptr<IOOBETask>& task);

private:
    bool g_oobeValue = false;
    static OOBEManager* oobeInstance_;
    static std::mutex mutex_;
    static std::vector<std::shared_ptr<IOOBETask>> oobeTasks_;
    OOBEManager();
    ~OOBEManager();
    void Initialize();
    void InitSystemAbilityListener();
    void StartListen();
    class SystemAbilityStatusChangeListener : public OHOS::SystemAbilityStatusChangeStub {
    public:
        SystemAbilityStatusChangeListener() {};
        virtual void OnAddSystemAbility(int32_t systemAbilityId, const std::string& deviceId) override;
        virtual void OnRemoveSystemAbility(int32_t systemAbilityId, const std::string& deviceId) override;
    };
    sptr<SystemAbilityStatusChangeListener> sysAbilityListener_ = nullptr;
};
} // ResourceSchedule
} // OHOS
#endif //RESOURCE_SCHEDULE_SERVICE_OOBE_MANAGER_H