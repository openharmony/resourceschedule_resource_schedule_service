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

#ifndef RESSCHED_INTERFACES_KITS_NAPI_JS_NAPI_UTILS_H
#define RESSCHED_INTERFACES_KITS_NAPI_JS_NAPI_UTILS_H

#include <memory>

#include "napi/native_api.h"
#include "napi/native_node_api.h"

namespace OHOS {
namespace ResourceSchedule {
constexpr size_t ARGC_MAX_COUNT = 10;

#define NAPI_CALL_NO_THROW(theCall, retVal)      \
    do {                                         \
        if ((theCall) != napi_ok) {              \
            return retVal;                       \
        }                                        \
    } while (0)


inline napi_value CreateJsUndefined(napi_env env)
{
    napi_value result = nullptr;
    napi_get_undefined(env, &result);
    return result;
}

inline napi_value CreateJsNull(napi_env env)
{
    napi_value result = nullptr;
    napi_get_null(env, &result);
    return result;
}

inline napi_value CreateJsNumber(napi_env env, int32_t value)
{
    napi_value result = nullptr;
    napi_create_int32(env, value, &result);
    return result;
}

inline napi_value CreateJsNumber(napi_env env, uint32_t value)
{
    napi_value result = nullptr;
    napi_create_uint32(env, value, &result);
    return result;
}

inline napi_value CreateJsNumber(napi_env env, int64_t value)
{
    napi_value result = nullptr;
    napi_create_int64(env, value, &result);
    return result;
}

inline napi_value CreateJsNumber(napi_env env, double value)
{
    napi_value result = nullptr;
    napi_create_double(env, value, &result);
    return result;
}

template<class T>
napi_value CreateJsValue(napi_env env, const T& value)
{
    using ValueType = std::remove_cv_t<std::remove_reference_t<T>>;
    napi_value result = nullptr;
    if constexpr (std::is_same_v<ValueType, bool>) {
        napi_get_boolean(env, value, &result);
        return result;
    } else if constexpr (std::is_arithmetic_v<ValueType>) {
        return CreateJsNumber(env, value);
    } else if constexpr (std::is_same_v<ValueType, std::string>) {
        napi_create_string_utf8(env, value.c_str(), value.length(), &result);
        return result;
    } else if constexpr (std::is_enum_v<ValueType>) {
        return CreateJsNumber(env, static_cast<std::make_signed_t<ValueType>>(value));
    } else if constexpr (std::is_same_v<ValueType, const char*>) {
        (value != nullptr) ? napi_create_string_utf8(env, value, strlen(value), &result) :
            napi_get_undefined(env, &result);
        return result;
    }
}

inline bool ConvertFromJsNumber(napi_env env, napi_value jsValue, int32_t& value)
{
    NAPI_CALL_NO_THROW(napi_get_value_int32(env, jsValue, &value), false);
    return true;
}

inline bool ConvertFromJsNumber(napi_env env, napi_value jsValue, uint32_t& value)
{
    NAPI_CALL_NO_THROW(napi_get_value_uint32(env, jsValue, &value), false);
    return true;
}

inline bool ConvertFromJsNumber(napi_env env, napi_value jsValue, int64_t& value)
{
    NAPI_CALL_NO_THROW(napi_get_value_int64(env, jsValue, &value), false);
    return true;
}

inline bool ConvertFromJsNumber(napi_env env, napi_value jsValue, double& value)
{
    NAPI_CALL_NO_THROW(napi_get_value_double(env, jsValue, &value), false);
    return true;
}

template<class T>
bool ConvertFromJsValue(napi_env env, napi_value jsValue, T& value)
{
    if (jsValue == nullptr) {
        return false;
    }

    using ValueType = std::remove_cv_t<std::remove_reference_t<T>>;
    if constexpr (std::is_same_v<ValueType, bool>) {
        NAPI_CALL_NO_THROW(napi_get_value_bool(env, jsValue, &value), false);
        return true;
    } else if constexpr (std::is_arithmetic_v<ValueType>) {
        return ConvertFromJsNumber(env, jsValue, value);
    } else if constexpr (std::is_same_v<ValueType, std::string>) {
        size_t len = 0;
        NAPI_CALL_NO_THROW(napi_get_value_string_utf8(env, jsValue, nullptr, 0, &len), false);
        auto buffer = std::make_unique<char[]>(len + 1);
        size_t strLength = 0;
        NAPI_CALL_NO_THROW(napi_get_value_string_utf8(env, jsValue, buffer.get(), len + 1, &strLength), false);
        value = buffer.get();
        return true;
    } else if constexpr (std::is_enum_v<ValueType>) {
        std::make_signed_t<ValueType> numberValue = 0;
        if (!ConvertFromJsNumber(env, jsValue, numberValue)) {
            return false;
        }
        value = static_cast<ValueType>(numberValue);
        return true;
    }
    return false;
}
} // ResourceSchedule
} // OHOS

#endif // RESSCHED_INTERFACES_KITS_NAPI_JS_NAPI_UTILS_H