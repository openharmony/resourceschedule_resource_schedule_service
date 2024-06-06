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

#include <file_ex.h>
#include <set>

#include "background_perceivable_scene_recognizer.h"
#include "background_mode.h"
#include "res_sched_log.h"
#include "res_sched_mgr.h"
#include "res_type.h"
#include "string_ex.h"
#include "updating_scene_recognizer.h"


namespace OHOS {
namespace ResourceSchedule {
namespace {
    static const int32_t INVALID_VALUE = -1;
    static const std::set<uint32_t> PERCEIVABLE_MODES = {
        BackgroundTaskMgr::BackgroundMode::Type::LOCATION,
        BackgroundTaskMgr::BackgroundMode::Type::VOIP,
        BackgroundTaskMgr::BackgroundMode::Type::AUDIO_PLAYBACK,
        BackgroundTaskMgr::BackgroundMode::Type::MULTI_DEVICE_CONNECTION,
    };
}

BackgroundPerceivableSceneRecoginzer::~BackgroundPerceivableSceneRecoginzer()
{
    RESSCHED_LOGI("~BackgroundPerceivableSceneRecoginzer");
}

BackgroundPerceivableSceneRecoginzer::BackgroundPerceivableSceneRecoginzer()
{
    perceivableTasks_ = {};
}
 
void BackgroundPerceivableSceneRecoginzer::OnDispatchResource(uint32_t resType, int64_t value,
    const nlohmann::json& payload)
{
    if (!payload.contains("pid") || !payload["pid"].is_string()) {
        return;
    }
    int32_t invalidValue = INVALID_VALUE;
    switch (resType) {
        case ResType::RES_TYPE_APP_STATE_CHANGE:
            HandleForeground(resType, value, payload);
            break;
        case ResType::RES_TYPE_CONTINUOUS_TASK:
            HandleContinuousTask(resType, value, payload);
            break;
        case ResType::RES_TYPE_REPORT_SCENE_BOARD:
            StrToInt(payload["pid"].get<std::string>(), sceneboardPid_);
            break;
        default:
            return;
    }
}

void BackgroundPerceivableSceneRecoginzer::HandleContinuousTask(uint32_t resType, int64_t value,
    const nlohmann::json& payload)
{
    pid_t pid = -1;
    StrToInt(payload["pid"].get<std::string>(), pid);
    std::vector<uint32_t> typeIds;
    if (payload.contains("typeIds") && payload["typeIds"].is_array()) {
        typeIds = payload["typeIds"].get<std::vector<uint32_t>>();
    }
    for (auto it = typeIds.begin(); it != typeIds.end();) {
        if (PERCEIVABLE_MODES.find(*it) == PERCEIVABLE_MODES.end()) {
            it = typeIds.erase(it);
        } else {
            it ++;
        }
    }
    if (value == ResType::ContinuousTaskStatus::CONTINUOUS_TASK_START && !typeIds.empty()) {
        HandleTaskStart(pid, typeIds);
    } else if (value == ResType::ContinuousTaskStatus::CONTINUOUS_TASK_UPDATE) {
        HandleTaskUpdate(pid, typeIds);
    } else if (value == ResType::ContinuousTaskStatus::CONTINUOUS_TASK_END) {
        HandleTaskStop(pid);
    } else {
        RESSCHED_LOGW("unknow ContinuousTaskStatus value");
    }
}

bool BackgroundPerceivableSceneRecoginzer::checkEnterScene()
{
    if (isInBackgroundPerceivableScene_ || foregroundPid_ == sceneboardPid_) {
        return false;
    }
    if (perceivableTasks_.size() > 1) {
        return true;
    }
    if (perceivableTasks_.size() > 0 && perceivableTasks_.find(foregroundPid_) == perceivableTasks_.end()) {
        return true;
    }
    return false;
}

void BackgroundPerceivableSceneRecoginzer::HandleTaskStart(pid_t pid, const std::vector<uint32_t> &typeIds)
{
    perceivableTasks_[pid] = typeIds;
    if (checkEnterScene()) {
        RESSCHED_LOGI("perceivable task start enter scene");
        nlohmann::json payload;
        ResSchedMgr::GetInstance().ReportData(ResType::RES_TYPE_BACKGROUND_PERCEIVABLE_SCENE,
            ResType::BackgroundPerceivableStatus::PERCEIVABLE_START, payload);
        isInBackgroundPerceivableScene_ = true;
    }
}

void BackgroundPerceivableSceneRecoginzer::HandleTaskUpdate(pid_t pid, const std::vector<uint32_t> &typeIds)
{
    if (typeIds.empty()) {
        perceivableTasks_.erase(pid);
        if (perceivableTasks_.empty() && isInBackgroundPerceivableScene_) {
            RESSCHED_LOGI("after task update all perceivable task stop exit scene");
            nlohmann::json payload;
            ResSchedMgr::GetInstance().ReportData(ResType::RES_TYPE_BACKGROUND_PERCEIVABLE_SCENE,
                ResType::BackgroundPerceivableStatus::PERCEIVABLE_STOP, payload);
            isInBackgroundPerceivableScene_ = false;
        }
    } else {
        perceivableTasks_[pid] = typeIds;
        if (checkEnterScene()) {
            nlohmann::json payload;
            ResSchedMgr::GetInstance().ReportData(ResType::RES_TYPE_BACKGROUND_PERCEIVABLE_SCENE,
                ResType::BackgroundPerceivableStatus::PERCEIVABLE_START, payload);
            isInBackgroundPerceivableScene_ = true;
        }
    }
}

void BackgroundPerceivableSceneRecoginzer::HandleTaskStop(pid_t pid)
{
    if (perceivableTasks_.find(pid) != perceivableTasks_.end()) {
        perceivableTasks_.erase(pid);
        if (perceivableTasks_.empty() && isInBackgroundPerceivableScene_) {
            RESSCHED_LOGI("after task stop all perceivable task stop exit scene");
            nlohmann::json payload;
            ResSchedMgr::GetInstance().ReportData(ResType::RES_TYPE_BACKGROUND_PERCEIVABLE_SCENE,
                ResType::BackgroundPerceivableStatus::PERCEIVABLE_STOP, payload);
            isInBackgroundPerceivableScene_ = false;
        }
    }
}

void BackgroundPerceivableSceneRecoginzer::HandleForeground(uint32_t resType, int64_t value,
    const nlohmann::json& payload)
{
    if (value != ResType::ProcessStatus::PROCESS_FOREGROUND) {
        return;
    }
    StrToInt(payload["pid"].get<std::string>(), foregroundPid_);
    nlohmann::json curPayload;
    if (foregroundPid_ == sceneboardPid_ && isInBackgroundPerceivableScene_) {
        RESSCHED_LOGI("sceneboard foreground exit scene");
        ResSchedMgr::GetInstance().ReportData(ResType::RES_TYPE_BACKGROUND_PERCEIVABLE_SCENE,
            ResType::BackgroundPerceivableStatus::PERCEIVABLE_STOP, curPayload);
        isInBackgroundPerceivableScene_ = false;
    } else if (checkEnterScene()) {
        RESSCHED_LOGI("sceneboard background and has perceivable task enter scene");
        ResSchedMgr::GetInstance().ReportData(ResType::RES_TYPE_BACKGROUND_PERCEIVABLE_SCENE,
            ResType::BackgroundPerceivableStatus::PERCEIVABLE_START, curPayload);
        isInBackgroundPerceivableScene_ = true;
    }
}
} // namespace ResourceSchedule
} // namespace OHOS