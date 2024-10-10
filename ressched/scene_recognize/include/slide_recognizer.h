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

#ifndef RESSCHED_SCENE_RECOGNIZE_SLIDE_RECOGNIZER_H
#define RESSCHED_SCENE_RECOGNIZE_SLIDE_RECOGNIZER_H

#include "scene_recognizer_base.h"
#include "ffrt.h"

namespace OHOS {
namespace ResourceSchedule {
namespace SlideRecognizeStat {
enum : uint32_t {
    IDLE,
    SLIDE_NORMAL_DETECTING,
    SLIDE_NORMAL,
    LIST_FLING,
};
}
static constexpr int64_t DEFAULT_LIST_FLINT_TIME_OUT_TIME = 3 * 1000 * 1000;
static constexpr int64_t DEFAULT_LIST_FLINT_END_TIME = 150 * 1000;
static constexpr float DEFAULT_LIST_FLING_SPEED_LIMIT = 3.0;
static constexpr int64_t DEFAULT_SLIDE_NORMAL_DETECTING_TIME = 50;
class SlideRecognizer : public SceneRecognizerBase {
public:
    SlideRecognizer() = default;
    ~SlideRecognizer();
    void OnDispatchResource(uint32_t, int64_t value, const nlohmann::json& payload) override;
    void SetListFlingTimeoutTime(int64_t value);
    void SetListFlingEndTime(int64_t value);
    void SetListFlingSpeedLimit(float value);
    void SetSlideNormalDetectingTime(int64_t value);
private:
    void HandleSlideEvent(int64_t value, const nlohmann::json& payload);
    void HandleSendFrameEvent(const nlohmann::json& payload);
    void HandleClickEvent(int64_t value, const nlohmann::json& payload);
    void HandleSlideDetecting(const nlohmann::json& payload);
    void HandleListFlingStart(const nlohmann::json& payload);
    void HandleSlideOFFEvent();
    nlohmann::json FillRealPid(const nlohmann::json& payload);
    ffrt::task_handle listFlingEndTask_ = nullptr;
    ffrt::task_handle listFlingTimeOutTask_ = nullptr;
    int64_t slideDetectingTime_ = -1;
    uint32_t state_ = SlideRecognizeStat::IDLE;
    int64_t listFlingTimeOutTime_ = DEFAULT_LIST_FLINT_TIME_OUT_TIME;
    int64_t listFlingEndTime_ = DEFAULT_LIST_FLINT_END_TIME;
    float listFlingSpeedLimit_ = DEFAULT_LIST_FLING_SPEED_LIMIT;
    int64_t slideNormalDecectingTime_ = DEFAULT_SLIDE_NORMAL_DETECTING_TIME;
    std::string slidePid_ = "";
};
} // namespace ResourceSchedule
} // namespace OHOS
#endif // RESSCHED_SCENE_RECOGNIZE_SLIDE_RECOGNIZER_H