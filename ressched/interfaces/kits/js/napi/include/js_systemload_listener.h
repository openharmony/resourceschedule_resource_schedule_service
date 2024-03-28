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

#ifndef RESSCHED_INTERFACES_KITS_JS_SYSTEMLOAD_LISTENER_H
#define RESSCHED_INTERFACES_KITS_JS_SYSTEMLOAD_LISTENER_H

#include <functional>

#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "res_sched_systemload_notifier_stub.h"

namespace OHOS {
namespace ResourceSchedule {
class SystemloadListener : public ResSchedSystemloadNotifierStub {
public:
    using OnSystemloadLevelCb = std::function<void(napi_env env, int32_t)>;
    SystemloadListener(napi_env env, OnSystemloadLevelCb callback);
    virtual ~SystemloadListener() = default;
    void OnSystemloadLevel(int32_t level) override;
private:
    napi_env napiEnv_ = nullptr;
    OnSystemloadLevelCb systemloadLevelCb_ = nullptr;
};
} // ResourceSchedule
} // OHOS

#endif // RESSCHED_INTERFACES_KITS_JS_SYSTEMLOAD_LISTENER_H