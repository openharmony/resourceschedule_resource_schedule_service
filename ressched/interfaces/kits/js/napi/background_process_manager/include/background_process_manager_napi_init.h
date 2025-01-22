/*
* Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef RESOURCESCHEDULE_INTERFACES_KITS_NAPI_BACKGROUND_PROCESS_MANAGER_NAPI_H
#define RESOURCESCHEDULE_INTERFACES_KITS_NAPI_BACKGROUND_PROCESS_MANAGER_NAPI_H

#include "napi/native_api.h"
#include "napi/native_node_api.h"

namespace OHOS {
namespace ResourceSchedule {

#ifdef __cplusplus
extern "C" {
#endif

__attribute__((constructor)) void RegisterModule(void);
napi_value Init(napi_env env, napi_value exports);

#ifdef __cplusplus
}
#endif

static napi_module backgroundProcessManagerModule = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = Init,
    .nm_modname = "resourceschedule.backgroundProcessManager",
    .nm_priv = ((void*)0),
    .reserved = { 0 },
};
}
}
#endif