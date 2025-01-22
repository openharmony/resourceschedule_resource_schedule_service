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

#include "js_systemload_napi_init.h"

#include "res_type.h"
#include "js_systemload.h"

namespace OHOS {
namespace ResourceSchedule {
#ifdef __cplusplus
extern "C" {
#endif

static napi_ref g_systemloadLevelTypeConstructor = nullptr;

static napi_value EnumSystemloadLevelConstructor(napi_env env, napi_callback_info info)
{
    size_t argc = 0;
    napi_value args[1] = { nullptr };
    napi_value res = nullptr;
    void* data = nullptr;

    napi_status status = napi_get_cb_info(env, info, &argc, args, &res, &data);
    if (status != napi_ok) {
        return nullptr;
    }
    return res;
}

static napi_value InitSystemloadLevelType(napi_env env, napi_value exports)
{
    napi_value systemloadLevelLow;
    napi_value systemloadLevelNormal;
    napi_value systemloadLevelMedium;
    napi_value systemloadLevelHigh;
    napi_value systemloadLevelOverheated;
    napi_value systemloadLevelWarning;
    napi_value systemloadLevelEmergency;
    napi_value systemloadLevelEscape;

    napi_create_uint32(env, static_cast<uint32_t>(ResType::SystemloadLevel::LOW),
        &systemloadLevelLow);
    napi_create_uint32(env, static_cast<uint32_t>(ResType::SystemloadLevel::NORMAL),
        &systemloadLevelNormal);
    napi_create_uint32(env, static_cast<uint32_t>(ResType::SystemloadLevel::MEDIUM),
        &systemloadLevelMedium);
    napi_create_uint32(env, static_cast<uint32_t>(ResType::SystemloadLevel::HIGH),
        &systemloadLevelHigh);
    napi_create_uint32(env, static_cast<uint32_t>(ResType::SystemloadLevel::OVERHEATED),
        &systemloadLevelOverheated);
    napi_create_uint32(env, static_cast<uint32_t>(ResType::SystemloadLevel::WARNING),
        &systemloadLevelWarning);
    napi_create_uint32(env, static_cast<uint32_t>(ResType::SystemloadLevel::EMERGENCY),
        &systemloadLevelEmergency);
    napi_create_uint32(env, static_cast<uint32_t>(ResType::SystemloadLevel::ESCAPE),
        &systemloadLevelEscape);

    napi_property_descriptor desc[] = {
        DECLARE_NAPI_STATIC_PROPERTY("LOW", systemloadLevelLow),
        DECLARE_NAPI_STATIC_PROPERTY("NORMAL", systemloadLevelNormal),
        DECLARE_NAPI_STATIC_PROPERTY("MEDIUM", systemloadLevelMedium),
        DECLARE_NAPI_STATIC_PROPERTY("HIGH", systemloadLevelHigh),
        DECLARE_NAPI_STATIC_PROPERTY("OVERHEATED", systemloadLevelOverheated),
        DECLARE_NAPI_STATIC_PROPERTY("WARNING", systemloadLevelWarning),
        DECLARE_NAPI_STATIC_PROPERTY("EMERGENCY", systemloadLevelEmergency),
        DECLARE_NAPI_STATIC_PROPERTY("ESCAPE", systemloadLevelEscape),
    };

    napi_value result = nullptr;
    int32_t refCount = 1;
    napi_define_class(env, "SystemLoadLevel", NAPI_AUTO_LENGTH, EnumSystemloadLevelConstructor,
        nullptr, sizeof(desc) / sizeof(*desc), desc, &result);
    napi_create_reference(env, result, refCount, &g_systemloadLevelTypeConstructor);
    napi_set_named_property(env, exports, "SystemLoadLevel", result);
    return exports;
}

napi_value InitSystemloadApi(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_FUNCTION("on", Systemload::SystemloadOn),
        DECLARE_NAPI_FUNCTION("off", Systemload::SystemloadOff),
        DECLARE_NAPI_FUNCTION("getLevel", Systemload::GetLevel),
    };

    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(*desc), desc));

    InitSystemloadLevelType(env, exports);

    return exports;
}

__attribute__((constructor)) void RegisterModule(void)
{
    napi_module_register(&systemloadModule);
}

#ifdef __cplusplus
}
#endif
} // ResourceSchedule
} // OHOS