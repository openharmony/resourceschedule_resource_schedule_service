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

#ifndef RESSCHED_SCENE_RECOGNIZE_SCNEE_RECOGNIZER_MGR_H
#define RESSCHED_SCENE_RECOGNIZE_SCNEE_RECOGNIZER_MGR_H

#include <string>
#include <unordered_map>

#include "nlohmann/json.hpp"
#include "scene_recognizer_base.h"
#include "ffrt.h"
#include "ffrt_inner.h"
#include "single_instance.h"

namespace OHOS {
namespace ResourceSchedule {
namespace RecognizeType {
enum : uint32_t {
    CONTINUOUS_APP_INSTALL_RECOGNIZER,
    SYSTEM_UPGRADE_SCENE_RECOGNIZER,
    BACKGROUND_SENSITIVE_TASK_OVERLAPPING_SCENE_RECOGNIZER,
    SLIDE_RECOGNIZER,
};
}
class SceneRecognizerMgr {
    DECLARE_SINGLE_INSTANCE_BASE(SceneRecognizerMgr);
public:
    SceneRecognizerMgr();
    ~SceneRecognizerMgr();
    void DispatchResource(uint32_t, int64_t value, const nlohmann::json& payload);
    void SetListFlingTimeoutTime(int64_t value);
    void SetListFlingEndTime(int64_t value);
    void SetListFlingSpeedLimit(float value);
    void SetSlideNormalDetectingTime(int64_t value);
    void SubmitTask(const std::function<void>& task);
private:
    std::unordered_map<uint32_t, std::shared_ptr<SceneRecognizerBase>> sceneRecognizers_;
    std::shared_ptr<ffrt::queue> ffrtQueue_;
};
} // namespace ResourceSchedule
} // namespace OHOS
#endif // RESSCHED_SCENE_RECOGNIZE_SCNEE_RECOGNIZER_MGR_H