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
    DECLARE_SINGLE_INSTANCE(NotifierMgr);
public:
    using NotifierMap = std::map<int32_t, std::map<std::string, sptr<IRemoteObject>>>;
    void Init();
    void RegisterNotifier(int32_t pid, const std::string& cbType, const sptr<IRemoteObject>& notifier);
    void UnRegisterNotifier(int32_t pid, const std::string& cbType);
    void RemoteNotifierDied(const sptr<IRemoteObject>& notifier);
    void OnDeviceLevelChanged(int32_t type, int32_t level, std::string& action);
    int32_t GetSystemloadLevel();
private:
    void RemoveNotifierLock(const sptr<IRemoteObject>& notifier);
    void OnDeviceLevelChangedLock(const std::string& cbType, int32_t level, std::string& action);

    std::mutex notifierMutex_;
    NotifierMap notifierMap_;
    ResType::SystemloadLevel systemloadLevel_ = ResType::SystemloadLevel::LOW;
    sptr<IRemoteObject::DeathRecipient> notifierDeathRecipient_ = nullptr;
    std::shared_ptr<ffrt::queue> notifierHandler_ = nullptr;
};
} // OHOS
} // ResourceSchedule
#endif // RESSCHED_SERVICES_RESSCHEDMGR_RESSCHEDFWK_INCLUDE_NOTIFIER_MGR_H