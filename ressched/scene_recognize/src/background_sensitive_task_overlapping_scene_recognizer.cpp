/*
 * Copyright (c) 2024-2025 Huawei Device Co., Ltd.
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

#include <file_ex.h>
#include <set>
#include <sstream>

#include "background_sensitive_task_overlapping_scene_recognizer.h"
#include "background_mode.h"
#include "res_sched_log.h"
#include "res_sched_mgr.h"
#include "res_type.h"
#include "string_ex.h"

namespace OHOS {
namespace ResourceSchedule {
namespace {
    static const pid_t PID_MIN = -1;
    static const pid_t PID_MAX = INT32_MAX;
    static const char* PID_KEY = "pid";
    static const char* TYPE_IDS_KEY = "typeIds";
    static const std::set<uint32_t> PERCEIVABLE_MODES = {
        BackgroundTaskMgr::BackgroundMode::Type::LOCATION,
        BackgroundTaskMgr::BackgroundMode::Type::VOIP,
        BackgroundTaskMgr::BackgroundMode::Type::AUDIO_PLAYBACK,
        BackgroundTaskMgr::BackgroundMode::Type::MULTI_DEVICE_CONNECTION,
    };
}

BackgroundSensitiveTaskOverlappingSceneRecognizer::~BackgroundSensitiveTaskOverlappingSceneRecognizer()
{
    RESSCHED_LOGI("~BackgroundPerceivableSceneRecoginzer");
}

BackgroundSensitiveTaskOverlappingSceneRecognizer::BackgroundSensitiveTaskOverlappingSceneRecognizer()
{
    perceivableTasks_ = {};
    AddAcceptResTypes({
        ResType::RES_TYPE_APP_STATE_CHANGE,
        ResType::RES_TYPE_CONTINUOUS_TASK,
        ResType::RES_TYPE_REPORT_SCENE_BOARD,
    });
}
 
void BackgroundSensitiveTaskOverlappingSceneRecognizer::OnDispatchResource(uint32_t resType, int64_t value,
    const nlohmann::json& payload)
{
    if (!payload.contains(PID_KEY) || !payload[PID_KEY].is_string()) {
        return;
    }
    switch (resType) {
        case ResType::RES_TYPE_APP_STATE_CHANGE:
            HandleForeground(resType, value, payload);
            break;
        case ResType::RES_TYPE_CONTINUOUS_TASK:
            HandleContinuousTask(resType, value, payload);
            break;
        case ResType::RES_TYPE_REPORT_SCENE_BOARD:
            StrToInt(payload[PID_KEY].get<std::string>(), sceneboardPid_);
            break;
        default:
            break;
    }
}

void BackgroundSensitiveTaskOverlappingSceneRecognizer::HandleContinuousTask(uint32_t resType, int64_t value,
    const nlohmann::json& payload)
{
    pid_t pid = -1;
    StrToInt(payload[PID_KEY].get<std::string>(), pid);
    std::vector<uint32_t> typeIds;
    if (payload.contains(TYPE_IDS_KEY) && payload[TYPE_IDS_KEY].is_array()) {
        typeIds = payload.value(TYPE_IDS_KEY, std::vector<uint32_t>());
    }
    std::stringstream typeIdsStr;
    for (auto it = typeIds.begin(); it != typeIds.end();) {
        if (!typeIdsStr.str().empty()) {
            typeIdsStr << ", ";
        }
        typeIdsStr << *it;
        if (PERCEIVABLE_MODES.find(*it) == PERCEIVABLE_MODES.end()) {
            it = typeIds.erase(it);
        } else {
            it ++;
        }
    }
    RESSCHED_LOGI("%{public}s, resType:%{public}d, value:%{public}lld, typeIds:{%{public}s}",
        __func__, resType, (long long)value, typeIdsStr.str().c_str());
    if (value == ResType::ContinuousTaskStatus::CONTINUOUS_TASK_START && !typeIds.empty()) {
        HandleTaskStart(pid, typeIds);
    } else if (value == ResType::ContinuousTaskStatus::CONTINUOUS_TASK_UPDATE) {
        HandleTaskUpdate(pid, typeIds);
    } else if (value == ResType::ContinuousTaskStatus::CONTINUOUS_TASK_END) {
        HandleTaskStop(pid);
    } else {
        RESSCHED_LOGW("%{public}s, unknow ContinuousTaskStatus value", __func__);
    }
}

/**
 * @brief judge is enter scene.
 *  Rules for entering the BackgroundSensitiveTaskOverlapping scene:
 *  1.Only one background sensitive continuous task,
 *    and the foreground app is NOT the app that start the continous task or scene board.
 *  2.Two or more background sensitive continous tasks, and the foregound app is NOT scene board.
 * @return True if enter scene, else false.
 */
bool BackgroundSensitiveTaskOverlappingSceneRecognizer::CheckEnterScene()
{
    if (isInBackgroundPerceivableScene_ || foregroundPid_ == sceneboardPid_) {
        RESSCHED_LOGD("already in background sensitive scene or foreground is sceneboard");
        return false;
    }

    // more than one app has benn applied for sensitive task, there must be a task in background
    if (perceivableTasks_.size() > 1) {
        return true;
    }
    if (perceivableTasks_.size() > 0 && perceivableTasks_.find(foregroundPid_) == perceivableTasks_.end()) {
        return true;
    }
    return false;
}

void BackgroundSensitiveTaskOverlappingSceneRecognizer::EnterScene()
{
    nlohmann::json payload;
    ResSchedMgr::GetInstance().ReportData(ResType::RES_TYPE_BACKGROUND_PERCEIVABLE_SCENE,
        ResType::BackgroundPerceivableStatus::PERCEIVABLE_START, payload);
    isInBackgroundPerceivableScene_ = true;
}

void BackgroundSensitiveTaskOverlappingSceneRecognizer::ExitScene()
{
    nlohmann::json payload;
    ResSchedMgr::GetInstance().ReportData(ResType::RES_TYPE_BACKGROUND_PERCEIVABLE_SCENE,
        ResType::BackgroundPerceivableStatus::PERCEIVABLE_STOP, payload);
    isInBackgroundPerceivableScene_ = false;
}

void BackgroundSensitiveTaskOverlappingSceneRecognizer::HandleTaskStart(pid_t pid,
    const std::vector<uint32_t> &filteredTypeIds)
{
    if (!IsValidPid(pid) || !IsValidFilteredTypeIds(filteredTypeIds)) {
        return;
    }
    perceivableTasks_[pid] = filteredTypeIds;
    if (CheckEnterScene()) {
        RESSCHED_LOGI("perceivable task start enter scene");
        EnterScene();
    }
}

void BackgroundSensitiveTaskOverlappingSceneRecognizer::HandleTaskUpdate(pid_t pid,
    const std::vector<uint32_t> &filteredTypeIds)
{
    if (filteredTypeIds.empty()) {
        if (perceivableTasks_.find(pid) != perceivableTasks_.end()) {
            perceivableTasks_.erase(pid);
        }
        if (!CheckEnterScene() && isInBackgroundPerceivableScene_) {
            RESSCHED_LOGI("after task update all perceivable task stop exit scene");
            ExitScene();
        }
    } else {
        perceivableTasks_[pid] = filteredTypeIds;
        if (CheckEnterScene()) {
            nlohmann::json payload;
            RESSCHED_LOGI("after task update perceivable task update enter scene");
            EnterScene();
        }
    }
}

void BackgroundSensitiveTaskOverlappingSceneRecognizer::HandleTaskStop(pid_t pid)
{
    if (perceivableTasks_.find(pid) != perceivableTasks_.end()) {
        perceivableTasks_.erase(pid);
        if (!CheckEnterScene() && isInBackgroundPerceivableScene_) {
            RESSCHED_LOGI("after task stop all perceivable task stop exit scene");
            ExitScene();
        }
    }
}

void BackgroundSensitiveTaskOverlappingSceneRecognizer::HandleForeground(uint32_t resType, int64_t value,
    const nlohmann::json& payload)
{
    if (value != ResType::ProcessStatus::PROCESS_FOREGROUND) {
        return;
    }
    StrToInt(payload[PID_KEY].get<std::string>(), foregroundPid_);
    if (foregroundPid_ == sceneboardPid_ && isInBackgroundPerceivableScene_) {
        RESSCHED_LOGI("sceneboard foreground exit scene");
        ExitScene();
    } else if (CheckEnterScene()) {
        RESSCHED_LOGI("sceneboard background and has perceivable task enter scene");
        EnterScene();
    }
}

bool BackgroundSensitiveTaskOverlappingSceneRecognizer::IsValidPid(pid_t pid)
{
    return (pid >= PID_MIN && pid <= PID_MAX);
}

bool BackgroundSensitiveTaskOverlappingSceneRecognizer::IsValidFilteredTypeIds(
    const std::vector<uint32_t> &filteredTypeIds)
{
    for (auto fileteredTypeId : filteredTypeIds) {
        if (fileteredTypeId < BackgroundTaskMgr::BackgroundMode::Type::DATA_TRANSFER ||
            fileteredTypeId > BackgroundTaskMgr::BackgroundMode::Type::END) {
            return false;
        }
    }
    if (filteredTypeIds.size() > BackgroundTaskMgr::BackgroundMode::Type::END) {
        return false;
    }
    return true;
}
} // namespace ResourceSchedule
} // namespace OHOS