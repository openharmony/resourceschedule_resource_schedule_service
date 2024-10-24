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

#include "background_sensitive_task_overlapping_scene_recognizer.h"
#include "continuous_app_install_recognizer.h"
#include "slide_recognizer.h"
#include "res_sched_log.h"
#include "scene_recognizer_mgr.h"
#include "system_upgrade_scene_recognizer.h"

namespace OHOS {
namespace ResourceSchedule {
IMPLEMENT_SINGLE_INSTANCE(SceneRecognizerMgr);

SceneRecognizerMgr::SceneRecognizerMgr()
{
    sceneRecognizers_ = {
        { RecognizeType::CONTINUOUS_APP_INSTALL_RECOGNIZER, std::make_shared<ContinuousAppInstallRecognizer>() },
        { RecognizeType::SYSTEM_UPGRADE_SCENE_RECOGNIZER, std::make_shared<SystemUpgradeSceneRecognizer>() },
        { RecognizeType::BACKGROUND_SENSITIVE_TASK_OVERLAPPING_SCENE_RECOGNIZER,
            std::make_shared<BackgroundSensitiveTaskOverlappingSceneRecognizer>() },
        { RecognizeType::SLIDE_RECOGNIZER, std::make_shared<SlideRecognizer>()},
        };
    ffrtQueue_ = std::make_shared<ffrt::queue>("scene_recognizers_queue",
        ffrt::queue_attr().qos(ffrt::qos_user_interactive));
}

SceneRecognizerMgr::~SceneRecognizerMgr()
{
    sceneRecognizers_.clear();
    if (ffrtQueue_ != nullptr) {
        ffrtQueue_.reset();
    }
    RESSCHED_LOGI("~SceneRecognizerMgr");
}

void SceneRecognizerMgr::DispatchResource(uint32_t resType, int64_t value, const nlohmann::json& payload)
{
    if (ffrtQueue_ == nullptr) {
        return;
    }
    for (auto recognizerItem : sceneRecognizers_) {
        auto recognizer = recognizerItem.second;
        ffrtQueue_->submit([resType, value, payload, recognizer]() {
            recognizer->OnDispatchResource(resType, value, payload);
        });
    }
}

void SceneRecognizerMgr::SubmitTask(const std::function<void>& task)
{
    if (ffrtQueue_ == nullptr) {
        return;
    }
    ffrtQueue_->submit(task);
}

void SceneRecognizerMgr::SetListFlingTimeoutTime(int64_t value)
{
    std::static_pointer_cast<SlideRecognizer>(sceneRecognizers_[RecognizeType::SLIDE_RECOGNIZER])->
        SetListFlingTimeoutTime(value);
}

void SceneRecognizerMgr::SetListFlingEndTime(int64_t value)
{
    std::static_pointer_cast<SlideRecognizer>(sceneRecognizers_[RecognizeType::SLIDE_RECOGNIZER])->
        SetListFlingEndTime(value);
}
void SceneRecognizerMgr::SetListFlingSpeedLimit(float value)
{
    std::static_pointer_cast<SlideRecognizer>(sceneRecognizers_[RecognizeType::SLIDE_RECOGNIZER])->
        SetListFlingSpeedLimit(value);
}
void SceneRecognizerMgr::SetSlideNormalDetectingTime(int64_t value)
{
    std::static_pointer_cast<SlideRecognizer>(sceneRecognizers_[RecognizeType::SLIDE_RECOGNIZER])->
        SetSlideNormalDetectingTime(value);
}

extern "C" {
    void SetListFlingTimeoutTime(int64_t value)
    {
        SceneRecognizerMgr::GetInstance().SetListFlingTimeoutTime(value);
    }
    void SetListFlingEndTime(int64_t value)
    {
        SceneRecognizerMgr::GetInstance().SetListFlingEndTime(value);
    }
    void SetListFlingSpeedLimit(float value)
    {
        SceneRecognizerMgr::GetInstance().SetListFlingSpeedLimit(value);
    }
    void SetSlideNormalDetectingTime(int64_t value)
    {
        SceneRecognizerMgr::GetInstance().SetSlideNormalDetectingTime(value);
    }
}
} // namespace ResourceSchedule
} // namespace OHOS