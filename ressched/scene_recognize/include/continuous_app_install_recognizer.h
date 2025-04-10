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

#ifndef RESSCHED_SCENE_RECOGNIZE_CONTINUOUS_APP_INSTALL_RECOGNIZER_H
#define RESSCHED_SCENE_RECOGNIZE_CONTINUOUS_APP_INSTALL_RECOGNIZER_H

#include "scene_recognizer_base.h"
#include "ffrt.h"

namespace OHOS {
namespace ResourceSchedule {
class ContinuousAppInstallRecognizer : public SceneRecognizerBase,
    public std::enable_shared_from_this<ContinuousAppInstallRecognizer> {
public:
    ContinuousAppInstallRecognizer();
    ~ContinuousAppInstallRecognizer();
    void OnDispatchResource(uint32_t resType, int64_t value, const nlohmann::json& payload) override;
private:
    ffrt::task_handle exitAppInstall_ = nullptr;
    std::atomic<bool> isInContinuousInstall_ = {false};
};
} // namespace ResourceSchedule
} // namespace OHOS
#endif // RESSCHED_SCENE_RECOGNIZE_CONTINUOUS_APP_INSTALL_RECOGNIZER_H