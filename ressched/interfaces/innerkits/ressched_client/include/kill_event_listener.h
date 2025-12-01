/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef RESSCHED_INTERFACES_INNERKITS_RESSCHED_CLIENT_INCLUDE_KILL_EVENT_LISTENER_H
#define RESSCHED_INTERFACES_INNERKITS_RESSCHED_CLIENT_INCLUDE_KILL_EVENT_LISTENER_H

#include <cstdint>
#include <unordered_map>
#include <unordered_set>
#include <mutex>
#include "nlohmann/json.hpp"
#include "res_common_util.h"
#include "res_sched_event_listener.h"
#include "ffrt.h"
#include "res_sched_client.h"

namespace OHOS {
namespace ResourceSchedule {
class KillEventListener : public ResourceSchedule::ResSchedEventListener {
public:
    static KillEventListener& GetInstance();
    void OnReceiveEvent(uint32_t eventType, uint32_t eventValue,
        std::unordered_map<std::string, std::string> extInfo) override;
    bool IsAllowedKill(uint32_t callerUid, const std::string reason);
    void Init();
private:
    void InitKillConfig();
    void ParseKillConfig(nlohmann::json reasonsConfig);
    void ReportKillEventPeriod(int64_t period);
    void ReportKillEvent();
    bool IsConfigKillReason(uint32_t callerUid, const std::string reason);
    void RegisterKillConfigUpdate();
    void UpdateKillCount(uint32_t callerUid, const std::string reason, bool isAllowed);
    std::mutex killReasonMutex_;
    std::unordered_map<uint32_t, std::unordered_set<std::string>> killReasonMap_;
    std::mutex isTaskSubmitMutex_;
    bool isTaskSubmit_ = false;
    std::mutex isRegisterMutex_;
    bool isRegister_ = false;
    std::mutex isConfigInitedMutex_;
    bool isConfigInited_ = false;
    ffrt::mutex killCountMapMutex_;
    std::unordered_map<std::string, uint32_t> killCountMap_;
};
} // namespace ResourceSchedule
} // namespace OHOS

#endif // RESSCHED_INTERFACES_INNERKITS_RESSCHED_CLIENT_INCLUDE_KILL_EVENT_LISTENER_H
