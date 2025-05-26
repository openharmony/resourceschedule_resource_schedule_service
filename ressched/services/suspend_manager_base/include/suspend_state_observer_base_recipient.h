/*
 * Copyright (c) 2025-2025 Huawei Device Co., Ltd.
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

#ifndef EFFICIENCY_MANAGER_SUSPEND_STATE_OBSERVER_BASE_RECIPIENT_H
#define EFFICIENCY_MANAGER_SUSPEND_STATE_OBSERVER_BASE_RECIPIENT_H

#include <functional>
#include "iremote_object.h"

#include "suspend_manager_base_log.h"

namespace OHOS {
namespace ResourceSchedule {
class SuspendStateObserverBaseRecipient : public IRemoteObject::DeathRecipient {
public:
    using RemoteDiedHandler = std::function<void(const wptr<IRemoteObject> &)>;
    explicit SuspendStateObserverBaseRecipient(RemoteDiedHandler handler) : handler_(handler)
    {}

    ~SuspendStateObserverBaseRecipient()
    {}

    void OnRemoteDied(const wptr<IRemoteObject> &remote)
    {
        SUSPEND_MSG_LOGI("SuspendStateObserverBaseRecipient on remote died.");
        if (handler_) {
            handler_(remote);
        }
    }

private:
    RemoteDiedHandler handler_;
};
} // namespace ResourceSchedule
} // namespace OHOS
#endif // EFFICIENCY_MANAGER_SUSPEND_STATE_OBSERVER_BASE_RECIPIENT_H