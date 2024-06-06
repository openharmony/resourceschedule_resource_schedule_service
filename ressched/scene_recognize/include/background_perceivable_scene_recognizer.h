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

#ifndef RESSCHED_SCENE_RECOGNIZE_BACKGROUND_PERCEIVABLE_SCENE_RECOGNIZER_H
#define RESSCHED_SCENE_RECOGNIZE_BACKGROUND_PERCEIVABLE_SCENE_RECOGNIZER_H

#include "scene_recognizer_base.h"

namespace OHOS {
namespace ResourceSchedule {
class BackgroundPerceivableSceneRecoginzer : public SceneRecognizerBase {
public:
    BackgroundPerceivableSceneRecoginzer();
    ~BackgroundPerceivableSceneRecoginzer();
    void OnDispatchResource(uint32_t, int64_t value, const nlohmann::json& payload) override;
private:
    pid_t foregroundPid_ = -1;
    pid_t sceneboardPid_ = -1;
    std::unordered_map<pid_t, std::vector<uint32_t>> perceivableTasks_;
    bool isInBackgroundPerceivableScene_ = false;

    void HandleContinuousTask(uint32_t resType, int64_t value, const nlohmann::json& payload);
    void HandleTaskStart(pid_t pid, const std::vector<uint32_t> &typeIds);
    void HandleTaskUpdate(pid_t pid, const std::vector<uint32_t> &typeIds);
    void HandleTaskStop(pid_t pid);
    void HandleForeground(uint32_t resType, int64_t value, const nlohmann::json& payload);
    bool checkEnterScene();
};
} // namespace ResourceSchedule
} // namespace OHOS
#endif // RESSCHED_SCENE_RECOGNIZE_BACKGROUND_PERCEIVABLE_SCENE_RECOGNIZER_H