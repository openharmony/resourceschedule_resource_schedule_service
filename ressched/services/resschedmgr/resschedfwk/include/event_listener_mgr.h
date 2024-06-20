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

#ifndef RESSCHED_SERVICES_RESSCHEDMGR_RESSCHEDFWK_INCLUDE_EVENT_LISTENER_MGR_H
#define RESSCHED_SERVICES_RESSCHEDMGR_RESSCHEDFWK_INCLUDE_EVENT_LISTENER_MGR_H

#include <map>
#include <mutex>
#include <string>
#include <list>
#include <vector>

#include "ffrt.h"
#include "iremote_object.h"
#include "refbase.h"
#include "res_type.h"
#include "nlohmann/json.hpp"
#include "single_instance.h"

namespace OHOS {
namespace ResourceSchedule {
class EventListenerMgr {
    DECLARE_SINGLE_INSTANCE_BASE(EventListenerMgr);
public:
    struct EventListenerInfo {
        sptr<IRemoteObject> listener = nullptr;
        pid_t pid;
    };
    using EventListenerMap = std::map<uint32_t, std::map<pid_t, EventListenerInfo>>;
    void Init();
    void Deinit();
    void RegisterEventListener(int32_t callingPid, const sptr<IRemoteObject>& listener, uint32_t eventType);
    void UnRegisterEventListener(int32_t callingPid, uint32_t eventType);
    void OnRemoteNotifierDied(const sptr<IRemoteObject>& notifier);
    void SendEvent(uint32_t eventType, uint32_t eventValue, const nlohmann::json &extInfo);
    std::unordered_map<int32_t, std::vector<pid_t>> DumpRegisterInfo();
private:
    EventListenerMgr() = default;
    ~EventListenerMgr();
    void RemoveListenerLock(const sptr<IRemoteObject>& listener);
    void SendEventLock(uint32_t eventType, uint32_t eventValue, const nlohmann::json &extInfo);
    void HandleSendEvent(std::vector<sptr<IRemoteObject>>& listenerVec, 
        uint32_t eventType, uint32_t eventValue, const nlohmann::json &extInfo);
    void OnRemoteListenerDied(const sptr<IRemoteObject>& listener);

    bool initialized_ = false;
    std::mutex mutex_;
    EventListenerMap eventListenerMap_;
    sptr<IRemoteObject::DeathRecipient> eventListenerDeathRecipient_ = nullptr;
    std::shared_ptr<ffrt::queue> eventSenderQueue_ = nullptr;
};
} // ResourceSchedule
} // OHOS
#endif // RESSCHED_SERVICES_RESSCHEDMGR_RESSCHEDFWK_INCLUDE_NOTIFIER_MGR_H