/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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

#ifndef RESSCHED_SERVICES_RESSCHEDMGR_RESSCHEDFWK_INCLUDE_RES_SCHED_MGR_H
#define RESSCHED_SERVICES_RESSCHEDMGR_RESSCHEDFWK_INCLUDE_RES_SCHED_MGR_H

#include <sys/types.h>
#include <string>
#include <unordered_set>
#include "event_handler.h"
#include "kill_process.h"
#include "single_instance.h"
#include "nlohmann/json.hpp"

namespace OHOS {
namespace ResourceSchedule {
class ResSchedMgr {
    DECLARE_SINGLE_INSTANCE(ResSchedMgr);

public:
    /**
     * Init resource schedule manager.
     */
    void Init();

    /**
     * Stop resource schedule manager.
     */
    void Stop();

    /**
     * Report data from other modules, will report resource data async.
     *
     * @param resType Resource type
     * @param value bit64 content.
     * @param payload Extra content.
     */
    void ReportData(uint32_t resType, int64_t value = 0, const nlohmann::json& payload = nullptr);

    /**
     * Report data inner, will report resource data async.
     *
     * @param resType Resource type.
     * @param value bit64 content.
     * @param payload Extra content.
     */
    void ReportDataInner(uint32_t resType, int64_t value = 0, const nlohmann::json& payload = nullptr);

    /**
     * Kill process by pid.
     *
     * @param payload process message
     */
    int32_t KillProcessByClient(const nlohmann::json& payload = nullptr);

    /**
     * Init resource_schedule_executor plugin manager.
     *
     * @param isProcessInit is process init
     */
    void InitExecutorPlugin(bool isProcessInit = false);

    /**
     * Init foreground app.
     *
     */
    void InitForegroundAppInfo();
    
    /**
     * process app state change.
     *
     * @param state the app's state
     * @param pid the app's pid
     */
    void OnApplicationStateChange(int32_t state, int32_t pid);

    /**
     * judge app is foreground.
     *
     * @param pid the app's pid
     * @return true if the app is foreground
     */
    bool IsForegroundApp(int32_t pid);
private:
    std::shared_ptr<KillProcess> killProcess_ = nullptr;
    std::mutex foregroundPidsMutex_;
    std::unordered_set<int32_t> foregroundPids_;
};
} // namespace ResourceSchedule
} // namespace OHOS

#endif // RESSCHED_SERVICES_RESSCHEDMGR_RESSCHEDFWK_INCLUDE_RES_SCHED_MGR_H
