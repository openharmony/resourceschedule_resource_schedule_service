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

#ifndef RESSCHED_INTERFACES_KITS_NAPI_JS_SYSTEMLOAD_INIT_H
#define RESSCHED_INTERFACES_KITS_NAPI_JS_SYSTEMLOAD_INIT_H

#include "napi/native_api.h"
#include "napi/native_node_api.h"

namespace OHOS {
namespace ResourceSchedule {
#ifdef __cplusplus
extern "C" {
#endif

__attribute__((constructor)) void RegisterModule(void);
napi_value InitSystemloadApi(napi_env env, napi_value exports);

#ifdef __cplusplus
}
#endif

static napi_module systemloadModule = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = InitSystemloadApi,
    .nm_modname = "resourceschedule.systemload",
    .nm_priv = ((void*)0),
    .reserved = { 0 },
};
} // namespace ResourceSchedule
} // namespace OHOS

#endif // RESSCHED_INTERFACES_KITS_NAPI_JS_SYSTEMLOAD_INIT_H
