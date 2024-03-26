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

#include "js_systemload_listener.h"

namespace OHOS {
namespace ResourceSchedule {
SystemloadListener::SystemloadListener(napi_env env, OnSystemloadLevelCb callback)
    : napiEnv_(env), systemloadLevelCb_(callback)
{
}

void SystemloadListener::OnSystemloadLevel(int32_t level)
{
    if (napiEnv_ == nullptr || systemloadLevelCb_ == nullptr) {
        return;
    }
    systemloadLevelCb_(napiEnv_, level);
}
} // ResourceSchedule
} // OHOS