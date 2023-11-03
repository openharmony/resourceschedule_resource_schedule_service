/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef RESSCHED_SCHED_CONTROLLER_OBSERVER_INCLUDE_MMIEVENT_OBSERVER_H
#define RESSCHED_SCHED_CONTROLLER_OBSERVER_INCLUDE_MMIEVENT_OBSERVER_H

#include <string>

#include "mmi_event_observer.h"

namespace OHOS {
namespace ResourceSchedule {
class MmiObserver : public MMI::MMIEventObserver {
public:
    /**
     * @brief Called back when mmi state changed, sync bundlename to nap.
     *
     * @param pid the pid.
     * @param uid the uid.
     * @param bundleName the bundleName.
     * @param syncStatus the sync status.
    */
    void SyncBundleName(int32_t pid, int32_t uid, std::string bundleName, int32_t syncStatus) override;
};
} // namespace ResourceSchedule
} // namespace OHOS
#endif // RESSCHED_SCHED_CONTROLLER_OBSERVER_INCLUDE_MMIEVENT_OBSERVER_H
