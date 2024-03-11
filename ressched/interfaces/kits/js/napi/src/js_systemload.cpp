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


#include "js_systemload.h"

#include <functional>

#include "js_napi_utils.h"
#include "js_systemload_listener.h"
#include "res_sched_client.h"
#include "res_sched_log.h"

namespace OHOS {
namespace ResourceSchedule {
static constexpr size_t ARG_COUNT_ONE = 1;
static constexpr size_t ARG_COUNT_TWO = 2;
static const std::string SYSTEMLOAD_LEVEL = "systemLoadChange";


Systemload& Systemload::GetInstance()
{
    static Systemload systemload;
    return systemload;
}

napi_value Systemload::SystemloadOn(napi_env env, napi_callback_info info)
{
    return GetInstance().RegisterSystemloadCallback(env, info);
}

napi_value Systemload::SystemloadOff(napi_env env, napi_callback_info info)
{
    return GetInstance().UnRegisterSystemloadCallback(env, info);
}

napi_value Systemload::GetLevel(napi_env env, napi_callback_info info)
{
    return GetInstance().GetSystemloadLevel(env, info);
}

void Systemload::OnSystemloadLevel(napi_env env, int32_t level)
{
    RESSCHED_LOGI("OnSystemloadLevel asyncCallback.");
    std::lock_guard<std::mutex> autoLock(jsCallbackMapLock_);
    if (jsCallBackMap_.find(SYSTEMLOAD_LEVEL) == jsCallBackMap_.end()) {
        RESSCHED_LOGE("OnSystemloadLevel cb type has not register yet.");
        return;
    }
    auto* cbInfo = new (std::nothrow) SystemloadLevelCbInfo;
    if (cbInfo == nullptr) {
        RESSCHED_LOGE("OnSystemloadLevel cbInfo null.");
        return;
    }
    cbInfo->result = level;
    napi_value resourceName = nullptr;
    NAPI_CALL_RETURN_VOID(env,
        napi_create_string_latin1(env, "OnSystemloadLevel", NAPI_AUTO_LENGTH, &resourceName));

    std::unique_ptr<NativeReference> callbackRef;
    NAPI_CALL_RETURN_VOID(env,
        napi_create_reference(env, jsCallBackMap_[SYSTEMLOAD_LEVEL]->GetNapiValue(), 1, &cbInfo->callback));
    callbackRef.reset(reinterpret_cast<NativeReference *>(cbInfo->callback));

    NAPI_CALL_RETURN_VOID(env, napi_create_async_work(env, nullptr, resourceName,
        [] (napi_env env, void* data) {},
        CompleteCb,
        static_cast<void *>(cbInfo),
        &cbInfo->asyncWork));
    NAPI_CALL_RETURN_VOID(env, napi_queue_async_work(env, cbInfo->asyncWork));
    napi_value result = nullptr;
    NAPI_CALL_RETURN_VOID(env, napi_get_null(env, &result));
    RESSCHED_LOGI("OnSystemloadLevel asyncCallback end");
}

napi_value Systemload::RegisterSystemloadCallback(napi_env env, napi_callback_info info)
{
    RESSCHED_LOGD("Regster Systemload Callback");
    std::string cbType;
    napi_value jsCallback = nullptr;

    if (!CheckCallbackParam(env, info, cbType, &jsCallback)) {
        RESSCHED_LOGE("Register Systemload Callback parameter error.");
        return CreateJsUndefined(env);
    }

    napi_ref tempRef = nullptr;
    napi_create_reference(env, jsCallback, 1, &tempRef);
    std::unique_ptr<NativeReference> callbackRef;
    callbackRef.reset(reinterpret_cast<NativeReference*>(tempRef));
    auto systemloadLevelCb = std::bind(
        &Systemload::OnSystemloadLevel, this, std::placeholders::_1, std::placeholders::_2);
    sptr<SystemloadListener> systemloadListener = new (std::nothrow) SystemloadListener(env, systemloadLevelCb);
    if (systemloadListener == nullptr) {
        RESSCHED_LOGE("Register Systemload listener nullptr.");
        return CreateJsUndefined(env);
    }
    ResSchedClient::GetInstance().RegisterSystemloadNotifier(cbType, systemloadListener);
    std::lock_guard<std::mutex> autoLock(jsCallbackMapLock_);
    if (jsCallBackMap_.find(cbType) != jsCallBackMap_.end()) {
        RESSCHED_LOGW("Register a exist callback type.");
        return CreateJsUndefined(env);
    }
    jsCallBackMap_[cbType] = std::move(callbackRef);
    return CreateJsUndefined(env);
}

napi_value Systemload::UnRegisterSystemloadCallback(napi_env env, napi_callback_info info)
{
    RESSCHED_LOGD("UnRegster Systemload Callback");
    std::string cbType;
    napi_value jsCallback = nullptr;

    if (!CheckCallbackParam(env, info, cbType, &jsCallback)) {
        RESSCHED_LOGE("Register Systemload Callback parameter error.");
        return CreateJsUndefined(env);
    }

    ResSchedClient::GetInstance().UnRegisterSystemloadNotifier(cbType);
    std::lock_guard<std::mutex> autoLock(jsCallbackMapLock_);
    if (jsCallBackMap_.find(cbType) != jsCallBackMap_.end()) {
        jsCallBackMap_.erase(cbType);
    }
    return CreateJsUndefined(env);
}

napi_value Systemload::GetSystemloadLevel(napi_env env, napi_callback_info info)
{
    RESSCHED_LOGI("GetSystemloadLevel, promise.");
    auto* cbInfo = new (std::nothrow) SystemloadLevelCbInfo;
    if (cbInfo == nullptr) {
        return CreateJsUndefined(env);
    }
    napi_value resourceName;
    NAPI_CALL(env, napi_create_string_latin1(env, "GetSystemloadLevel", NAPI_AUTO_LENGTH, &resourceName));
    napi_deferred deferred;
    napi_value promise = nullptr;
    NAPI_CALL(env, napi_create_promise(env, &deferred, &promise));
    cbInfo->deferred = deferred;

    NAPI_CALL(env, napi_create_async_work(env, nullptr, resourceName,
        Execute,
        Complete,
        static_cast<void *>(cbInfo),
        &cbInfo->asyncWork));
    NAPI_CALL(env, napi_queue_async_work(env, cbInfo->asyncWork));
    RESSCHED_LOGI("GetSystemloadLevel, promise end");
    return promise;
}

void Systemload::Execute(napi_env env, void* data)
{
    RESSCHED_LOGI("GetSystemloadLevel, worker pool thread execute.");
    auto* cbInfo = static_cast<SystemloadLevelCbInfo*>(data);
    if (cbInfo == nullptr) {
        RESSCHED_LOGW("GetSystemloadLevel execute cb info is nullptr.");
        return;
    }
    cbInfo->result = ResSchedClient::GetInstance().GetSystemloadLevel();
    RESSCHED_LOGI("GetSystemloadLevel, worker pool thread execute end.");
}

void Systemload::Complete(napi_env env, napi_status status, void* data)
{
    RESSCHED_LOGI("GetSystemloadLevel,  main event thread complete.");
    auto* cbInfo = static_cast<SystemloadLevelCbInfo*>(data);
    if (cbInfo == nullptr) {
        RESSCHED_LOGW("GetSystemloadLevel Complete cb info is nullptr.");
        return;
    }
    napi_value result = nullptr;
    napi_create_uint32(env, static_cast<uint32_t>(cbInfo->result), &result);
    NAPI_CALL_RETURN_VOID(env, napi_resolve_deferred(env, cbInfo->deferred, result));
    NAPI_CALL_RETURN_VOID(env, napi_delete_async_work(env, cbInfo->asyncWork));
    delete cbInfo;
    cbInfo = nullptr;
    RESSCHED_LOGI("GetSystemloadLevel,  main event thread complete end.");
}

void Systemload::CompleteCb(napi_env env, napi_status status, void* data)
{
    RESSCHED_LOGI("CompleteCb, main event thread complete callback.");
    auto* cbInfo = static_cast<SystemloadLevelCbInfo*>(data);
    if (cbInfo == nullptr) {
        RESSCHED_LOGW("GetSystemloadLevel Complete cb info is nullptr.");
        return;
    }
    napi_value callback = nullptr;
    napi_value undefined = nullptr;
    napi_value result = nullptr;
    napi_value callResult = nullptr;
    NAPI_CALL_RETURN_VOID(env, napi_get_undefined(env, &undefined));
    NAPI_CALL_RETURN_VOID(env, napi_get_reference_value(env, cbInfo->callback, &callback));

    napi_create_uint32(env, static_cast<uint32_t>(cbInfo->result), &result);
    NAPI_CALL_RETURN_VOID(env, napi_call_function(env, undefined, callback, 1, &result, &callResult));
    NAPI_CALL_RETURN_VOID(env, napi_delete_async_work(env, cbInfo->asyncWork));
    delete cbInfo;
    cbInfo = nullptr;
    RESSCHED_LOGI("CompleteCb, main event thread complete end.");
}

bool Systemload::CheckCallbackParam(napi_env env, napi_callback_info info,
                                    std::string &cbType, napi_value *jsCallback)
{
    size_t argc = ARG_COUNT_TWO;
    napi_value argv[ARG_COUNT_TWO] = { 0 };
    NAPI_CALL_BASE(env, napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr), false);
    if (argc != ARG_COUNT_TWO) {
        RESSCHED_LOGE("Parameter error. The type of \"number of parameters\" must be 2");
        return false;
    }
    if (!ConvertFromJsValue(env, argv[0], cbType)) {
        RESSCHED_LOGE("Parameter error. The type of \"type\" must be string");
        return false;
    }

    *jsCallback = argv[ARG_COUNT_ONE];
    if (jsCallback == nullptr) {
        RESSCHED_LOGE("listenerObj is nullptr");
        return false;
    }
    bool isCallable = false;
    napi_is_callable(env, *jsCallback, &isCallable);
    if (!isCallable) {
        RESSCHED_LOGE("Parameter error. The type of \"callback\" must be Callback<Array<ContinuationResult>>");
        return false;
    }
    return true;
}
} // ResourceSchedule
} // OHOS