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

#ifndef RESSCHED_SCENE_RECOGNIZE_SCNEE_RECOGNIZER_BASE_H
#define RESSCHED_SCENE_RECOGNIZE_SCNEE_RECOGNIZER_BASE_H

#include <string>
 
#include "nlohmann/json.hpp"


namespace OHOS {
namespace ResourceSchedule {
class SceneRecognizerBase {
public:
    virtual ~SceneRecognizerBase() = default;
    virtual void OnDispatchResource(uint32_t, int64_t value, const nlohmann::json& payload) = 0;
};
} // namespace ResourceSchedule
} // namespace OHOS
#endif // RESSCHED_SCENE_RECOGNIZE_SCNEE_RECOGNIZER_BASE_H