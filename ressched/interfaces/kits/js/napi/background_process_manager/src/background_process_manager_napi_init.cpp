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

#include "background_process_manager_napi_init.h"
#include "background_process_manager.h"
#include "res_sched_log.h"

namespace {
    constexpr int SET_PROCESS_PRIORITY_PARAM_NUM = 2;
    constexpr int RESET_PROCESS_PRIORITY_PARAM_NUM = 1;
    constexpr int PID_INDEX = 0;
    constexpr int PRIORITY_INDEX = 1;
}

namespace OHOS {
namespace ResourceSchedule {
#ifdef __cplusplus
extern "C" {
#endif

napi_value SetProcessPriority(napi_env env, napi_callback_info info)
{
    napi_value ret;
    size_t argc = SET_PROCESS_PRIORITY_PARAM_NUM;
    napi_value argv[SET_PROCESS_PRIORITY_PARAM_NUM] = { nullptr };
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc != SET_PROCESS_PRIORITY_PARAM_NUM) {
        RESSCHED_LOGE("param num error");
        napi_create_int32(env, ERR_BACKGROUND_PROCESS_MANAGER_INVALID_PARAM, &ret);
        return ret;
    }

    napi_valuetype pidType;
    napi_typeof(env, argv[PID_INDEX], &pidType);

    napi_valuetype priorityType;
    napi_typeof(env, argv[PRIORITY_INDEX], &priorityType);

    if (pidType != napi_number || priorityType != napi_number) {
        RESSCHED_LOGE("param type error");
        napi_create_int32(env, ERR_BACKGROUND_PROCESS_MANAGER_INVALID_PARAM, &ret);
        return ret;
    }

    int32_t pid = -1;
    int32_t priority = -1;

    napi_get_value_int32(env, argv[PID_INDEX], &pid);
    napi_get_value_int32(env, argv[PRIORITY_INDEX], &priority);

    BackgroundProcessManager_ProcessPriority processPriority =
        static_cast<BackgroundProcessManager_ProcessPriority>(priority);

    OH_BackgroundProcessManager_SetProcessPriority(pid, processPriority);
    napi_create_int32(env, ERR_BACKGROUND_PROCESS_MANAGER_SUCCESS, &ret);
    return ret;
}

napi_value ResetProcessPriority(napi_env env, napi_callback_info info)
{
    napi_value ret;
    size_t argc = RESET_PROCESS_PRIORITY_PARAM_NUM;
    napi_value argv[RESET_PROCESS_PRIORITY_PARAM_NUM] = { nullptr };
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc != RESET_PROCESS_PRIORITY_PARAM_NUM) {
        RESSCHED_LOGE("param num error");
        napi_create_int32(env, ERR_BACKGROUND_PROCESS_MANAGER_INVALID_PARAM, &ret);
        return ret;
    }

    napi_valuetype pidType;
    napi_typeof(env, argv[PID_INDEX], &pidType);

    if (pidType != napi_number) {
        RESSCHED_LOGE("param type error");
        napi_create_int32(env, ERR_BACKGROUND_PROCESS_MANAGER_INVALID_PARAM, &ret);
        return ret;
    }

    int32_t pid = -1;

    napi_get_value_int32(env, argv[PID_INDEX], &pid);

    OH_BackgroundProcessManager_ResetProcessPriority(pid);
    napi_create_int32(env, ERR_BACKGROUND_PROCESS_MANAGER_SUCCESS, &ret);
    return ret;
}

static void SetPropertyName(napi_env env, napi_value dstObj, int32_t objName, const char * propName)
{
    napi_value prop = nullptr;
    if (napi_create_int32(env, objName, &prop) == napi_ok) {
        napi_set_named_property(env, dstObj, propName, prop);
    }
}

static napi_value ProcessPriorityInit(napi_env env, napi_value exports)
{
    napi_value obj = nullptr;
    napi_create_object(env, &obj);
    SetPropertyName(env, obj, static_cast<uint32_t>(BackgroundProcessManager_ProcessPriority::PROCESS_BACKGROUND),
        "PROCESS_BACKGROUND");
    SetPropertyName(env, obj, static_cast<uint32_t>(BackgroundProcessManager_ProcessPriority::PROCESS_INACTIVE),
        "PROCESS_INACTIVE");

    napi_property_descriptor desc[] = {
        DECLARE_NAPI_PROPERTY("ProcessPriority", obj),
    };

    napi_define_properties(env, exports, sizeof(desc) / sizeof(*desc), desc);
    return exports;
}

static napi_value InitBackgroundProcessManagerAPi(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_FUNCTION("setProcessPriority", SetProcessPriority),
        DECLARE_NAPI_FUNCTION("resetProcessPriority", ResetProcessPriority),
    };

    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(*desc), desc));
    return exports;
}

napi_value Init(napi_env env, napi_value exports)
{
    InitBackgroundProcessManagerAPi(env, exports);
    ProcessPriorityInit(env, exports);
    return exports;
}

__attribute__((constructor)) void RegisterModule(void)
{
    napi_module_register(&backgroundProcessManagerModule);
}

#ifdef __cplusplus
}
#endif
}
}