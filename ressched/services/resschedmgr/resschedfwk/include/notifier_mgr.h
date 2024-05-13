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

#ifndef RESSCHED_SERVICES_RESSCHEDMGR_RESSCHEDFWK_INCLUDE_NOTIFIER_MGR_H
#define RESSCHED_SERVICES_RESSCHEDMGR_RESSCHEDFWK_INCLUDE_NOTIFIER_MGR_H

#include <map>
#include <mutex>
#include <string>

#include "ffrt.h"
#include "iremote_object.h"
#include "refbase.h"
#include "res_type.h"
#include "single_instance.h"

namespace OHOS {
namespace ResourceSchedule {
class NotifierMgr {
    DECLARE_SINGLE_INSTANCE_BASE(NotifierMgr);
public:
    struct NotifierInfo {
        sptr<IRemoteObject> notifier = nullptr;
        bool foreground = true;
        bool hapApp = false;
        int32_t level = ResType::SystemloadLevel::LOW;
    };
    using NotifierMap = std::map<int32_t, NotifierInfo>;
    void Init();
    void RegisterNotifier(int32_t pid, const sptr<IRemoteObject>& notifier);
    void UnRegisterNotifier(int32_t pid);
    void OnRemoteNotifierDied(const sptr<IRemoteObject>& notifier);
    void OnDeviceLevelChanged(int32_t type, int32_t level);
    void OnApplicationStateChange(int32_t state, int32_t pid);
    int32_t GetSystemloadLevel();
    std::vector<std::pair<int32_t, bool>> DumpRegisterInfo();
private:
    NotifierMgr() = default;
    ~NotifierMgr();
    void RemoveNotifierLock(const sptr<IRemoteObject>& notifier);
    void OnDeviceLevelChangedLock(int32_t level);
    void HandleDeviceLevelChange(std::vector<sptr<IRemoteObject>>& notifierVec, int32_t level);
    bool IsHapApp();

    bool initialized_ = false;
    std::mutex notifierMutex_;
    NotifierMap notifierMap_;
    ResType::SystemloadLevel systemloadLevel_ = ResType::SystemloadLevel::LOW;
    sptr<IRemoteObject::DeathRecipient> notifierDeathRecipient_ = nullptr;
    std::shared_ptr<ffrt::queue> notifierHandler_ = nullptr;
};
} // ResourceSchedule
} // OHOS
#endif // RESSCHED_SERVICES_RESSCHEDMGR_RESSCHEDFWK_INCLUDE_NOTIFIER_MGR_H