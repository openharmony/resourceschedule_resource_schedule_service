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
#include "res_sched_log.h"

namespace OHOS {
namespace ResourceSchedule {
using OnSystemloadLevelCb = std::function<void(napi_env, napi_value, int32_t)>;
struct CallBackContext {
    napi_env env = nullptr;
    std::shared_ptr<NativeReference> callbackRef = nullptr;
    OnSystemloadLevelCb onSystemloadLevelCb = nullptr;
    int32_t level = 0;
}

SystemloadListener::SystemloadListener(napi_env env, napi_value callbackObj, OnSystemloadLevelCb callback)
    : napiEnv_(env), systemloadLevelCb_(callback)
{
    if (napiEnv_ == nullptr || callbackObj == nullptr) {
        return;
    }
    napi_ref tmpRef = nullptr;
    napi_create_reference(napiEnv_, callbackObj, 1, &tmpRef);
    callbackRef_.reset(reinterpret_cast<NativeReference*>(tmpRef));
    napi_value callbackWorkName  = nullptr;
    napi_create_string_utf8(env, "ThreadSafeFunction in SystemloadListener", NAPI_AUTO_LENGTH, &callbackWorkName);
    napi_create_threadsafe_function(env, nullptr, nullptr, callbackWorkName, 0, 1, nullptr, nullptr, nullptr,
        ThreadSafeCallBack, &threadSafeFunction_);
}

void SystemloadListener::ThreadSafeCallBack(napi_env ThreadSafeEnv, napi_value js_cb, void* context, void* data)
{
    RESSCHED_LOGI("SystemloadListener ThreadSafeCallBack start");
    CallBackContext* callBackContext = reinterpret_cast<CallBackContext>(data);
    callBackContext->env;
    callBackContext->onSystemloadLevelCb(callBackContext->env,
        callBackContext->callbackRef->GetNapiValue(), callBackContext->level);
    delete callBackContext;
}

void SystemloadListener::OnSystemloadLevel(int32_t level)
{
    if (napiEnv_ == nullptr || callbackRef_ == nullptr || systemloadLevelCb_ == nullptr) {
        return;
    }
    CallBackContext* callBackContext = new CallBackContext();
    callBackContext->env = napiEnv_;
    callBackContext->callbackRef = callbackRef_;
    callBackContext->level = level;
    callBackContext->onSystemloadLevelCb = systemloadLevelCb_;
    napi_acquire_threadsafe_function(threadSafeFunction_);
    napi_call_threadsafe_function(threadSafeFunction_, callBackContext, napi_tsfn_blocking);
}
} // ResourceSchedule
} // OHOS