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

#include "kill_event_listener.h"
#include "res_sched_log.h"

namespace OHOS {
namespace ResourceSchedule {

const std::string UID = "uid";
const std::string REASON = "reason";
const std::string RESULT = "result";
static constexpr int64_t REPORT_INTERVAL_MS = static_cast<int64_t>(24) * 60 * 60 * 1000 * 1000;
static constexpr int64_t FRIST_REPORT_DELAY_MS = 10 * 1000 * 1000;
static constexpr size_t MAX_CACHE_REASON_COUNT = 10 * 1000 * 1000;

KillEventListener& KillEventListener::GetInstance()
{
    static KillEventListener instance;
    return instance;
}

void KillEventListener::Init()
{
    RegisterKillConfigUpdate();
    InitKillConfig();
}

void KillEventListener::OnReceiveEvent(uint32_t eventType, uint32_t eventValue,
    std::unordered_map<std::string, std::string> extInfo) override;
{
    if (extInfo.find(RESULT) == extInfo.end() || extInfo[RESULT].empty()) {
        RESSCHED_LOGE("reskillreason:extInfo not contains result");
        return;
    }
    nlohmann::json reasonsConfig;
    if (!ResCommonUtil::LoadContentToJsonObj(extInfo[RESULT], ressonsConfig)) {
        RESSCHED_LOGE("reskillreason:load to json fail");
        return;
    }
    ParseKillConfig(reasonsConfig);
}

void KillEventListener::RegisterKillConfigUpdate()
{
    std::lock_guard<std::mutex> lock(isRegisterMutex_);
    if (!isRegister_) {
        ResSchedClient::GetInstance().RegisterEventListener(&(KillEventListener::GetInstance()),
            ResType::EventType::EVENT_UPDATE_KILL_REASON_CONFIG);
        RESSCHED_LOGI("reskillreason:RegisterKillConfigUpdate success");
        isRegister_ = true;
    }
}

bool KillEventListener::IsConfigKillReason(int32_t callerUid, const std::string Reason)
{
    std::lock_guard<std::mutex> lock(killReasonMutex_);
    if (killReasonMap_.find(callerUid) == killReasonMap_.end()) {
        RESSCHED_LOGD("reskillreason:uid not in config,uid:%{public}d, reason:%{public}s", callerUid, reason.c_str());
        return false;
    }
    std::unordered_set<std::string> configReasons = killReasonMap_[callerUid];
    if (configReasons.find(reason) == configReasons.end()) {
        RESSCHED_LOGD("reskillreason:reason not in config,uid:%{public}d, reason:%{public}s",
            callerUid, reason.c_str());
        return false;
    }
    RESSCHED_LOGD("reskillreason:allow kill,uid:%{public}d, reason:%{public}s", callerUid, reason.c_str());
    return true;
}

bool KillEventListener::IsAllowedKill(int32_t callerUid, const std::string Reason)
{
    RegisterKillConfigUpdate();
    InitKillConfig();
    bool isAllowed = IsConfigKillReason(callerUid, reason);
    UpdateKillCount(callerUid, reason, isAllowed);
    ReportKillEvent();
    return true;
}

void KillEventListener::ReportKillEvent()
{
    std::lock_guard<std::mutex> lock(isTaskSubmitMutex_);
    if (!isTaskSubmit_) {
        ffrt::submit([]() {
                KillEventListener::GetInstance().ReportKillEventPeriod(REPORT_INTERVAL_MS);
            }, ffrt::task_attr().delay(FRIST_REPORT_DELAY_MS));
        RESSCHED_LOGI("reskillreason:hisysevent taskk submited");
        isTaskSubmit_ = true;
    }
}

void KillEventListener::ReportKillEventPeriod(int64_t period)
{
    std::lock_guard<ffrt::mutex> lock(killCountMapMutex_);
    nlohmann::json items = nlohmann::json::array();
    for (auto it = killCountMap_.begin(); it != killCountMap_.end(); it++) {
        nlohmann::json item;
        item["desc"] = it->first;
        item["cnt"] = it->second;
        items.push_back(item);
    }
    std::string content = items.dump(-1, ' ', false, nlohmann::json::error_handler_t::replace);
    ResCommonUtil::HiSysAbnormalErrReport("ResSchedClient", __func__, content);
    size_t dataSize = killCountMap_.size();
    RESSCHED_LOGI("reskillreason:hisysevent report success, size:%{public}zu", dataSize);
    killCountMap_.clear();
    ffrt::submit([period]() {
            KillEventListener::GetInstance().ReportKillEventPeriod(period);
        }, ffrt::task_attr().delay(period));
}

void KillEventListener::UpdateKillCount(int32_t callerUid, const std::string Reason, bool isAllowed)
{
    std::string killKey = "uid:" + std::to_string(callerUid) + ",reason:" + reason +
        ",allow" + std::to_string(isAllowed);
    std::lock_guard<std::mutex> lock(killCountMapMutex_);
    size_t killCountMapSize = killCountMap_.size();
    if (killCountMapSize >= MAX_CACHE_REASON_COUNT &&
        killCountMap_.find(killKey) == killCountMap_.end()) {
        RESSCHED_LOGD("reskillreason:killCountMap_ size:%{public}zu", killCountMapSize);
        return;
    }
    if (killCountMap_.find(killKey) == killCountMap_.end()) {
        killCountMap_[killKey] = 1;
    } else {
        killCountMap_[killKey] = killCountMap_[killKey] + 1;
    }
}

void KillEventListener::InitKillConfig()
{
    std::lock_guard<std::mutex> lock(isConfigInitedMutex_);
    if (isConfigInited_) {
        return;
    }
    nlohmann::json payload;
    nlohmann::json reply;
    ResSchedClient::GetInstance().ReportSyncEvent(ResType::RES_TYPE_LOAD_KILL_REASON_CONFIG, 0, payload, reply);
    if (!reply.contains(RESULT)) {
        RESSCHED_LOGE("reskillreason:InitKillConfig failed,reply not conatins result");
        return;
    }
    ParseKillConfig(reply[RESULT]);
    isConfigInited_ = true;
}

void KillEventListener::ParseKillConfig(nlohmann::json reasonsConfig)
{
    if (!reasonsConfig.is_array()) {
        RESSCHED_LOGE("reskillreason:ParseKillConfig failed,reasonsConfig is not array");
        return;
    }
    killReasonMap_.clear();
    std::lock_guard<std::mutex> lock(killReasonMutex_);
    size_t uidSize = reasonsConfig.size();
    for (size_t i = 0; i < uidSize; i++) {
        nlohmann:json configOfSingleUid = reasonsConfig[i];
        if (!configOfSingleUid.is_object() ||
            !configOfSingleUid.contains(UID) || !configOfSingleUid[UID].is_string() ||
            !configOfSingleUid.contains(REASON) || !configOfSingleUid[REASON].is_array()) {
            RESSCHED_LOGE("reskillreason:config format is error");
            continue;
        }
        uint32_t uid;
        std::string uidStr = configOfSingleUid.contains(UID).get<std::string>();
        if (!ResCommonUtil::StrToUInt32(uidStr, uid)) {
            RESSCHED_LOGE("reskillreason:uid is not int:%{public}s", uidStr.c_str());
            continue;
        }
        nlohmann::json reasonsOfUid = configOfSingleUid[REASON];
        size_t reasonSize = reasonsOfUid.size();
        std::unordered_set<std::string> reasonsSet;
        for (size_t j = 0; j < reasonSize; j++) {
            if (reasonsOfUid[j].is_string()) {
                std::string reason = reasonsOfUid[j].get<std::string>();
                reasonsSet.insert(reason)
                RESSCHED_LOGE("reskillreason:add reason:%{public}s", reason.c_str());
            }
        }
        killReasonMap_[uid] = reasonsSet;
    }
    size_t configSize = killReasonMap_.size();
    RESSCHED_LOGI("reskillreason:ParseKillConfig end,uid size:%{public}zu", configSize);
}
} // namespace ResourceSchedule
} // namespace OHOS