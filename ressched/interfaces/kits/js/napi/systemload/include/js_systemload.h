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

#ifndef RESSCHED_INTERFACES_KITS_JS_SYSTEMLOAD_H
#define RESSCHED_INTERFACES_KITS_JS_SYSTEMLOAD_H

#include <list>
#include <memory>
#include <mutex>
#include <string>
#include <map>

#include "js_systemload_listener.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "native_engine/native_engine.h"
#include "refbase.h"
#include "single_instance.h"

namespace OHOS {
namespace ResourceSchedule {
class Systemload {
    DECLARE_SINGLE_INSTANCE_BASE(Systemload);
public:
    struct SystemloadLevelCbInfo {
        explicit SystemloadLevelCbInfo(napi_env env)
            : nativeEnv(env) {}
        ~SystemloadLevelCbInfo()
        {
            if (nativeEnv) {
                if (callback) {
                    napi_delete_reference(nativeEnv, callback);
                    callback = nullptr;
                }
                if (asyncWork) {
                    napi_delete_async_work(nativeEnv, asyncWork);
                    asyncWork = nullptr;
                }
            }
        }
        napi_ref callback = nullptr;
        napi_async_work asyncWork = nullptr;
        napi_deferred deferred = nullptr;
        napi_env nativeEnv = nullptr;
        int32_t result = 0;
    };

    using CallBackPair = std::pair<std::unique_ptr<NativeReference>, sptr<SystemloadListener>>;
    // static function
    static napi_value SystemloadOn(napi_env env, napi_callback_info info);
    static napi_value SystemloadOff(napi_env env, napi_callback_info info);
    static napi_value GetLevel(napi_env env, napi_callback_info info);

    void OnSystemloadLevel(napi_env env, napi_value callbackObj, int32_t level);
private:
    Systemload() = default;
    ~Systemload();
    napi_value RegisterSystemloadCallback(napi_env env, napi_callback_info info);
    napi_value UnRegisterSystemloadCallback(napi_env env, napi_callback_info info);
    napi_value GetSystemloadLevel(napi_env env, napi_callback_info info);
    bool CheckCallbackParam(napi_env env, napi_callback_info info, std::string &cbType, napi_value *jsCallback);

    static void Execute(napi_env env, void *data);
    static void Complete(napi_env env, napi_status status, void *data);
    static void CompleteCb(napi_env env, napi_status status, void* data);

    std::mutex jsCallbackMapLock_;
    std::map<std::string, std::list<CallBackPair> > jsCallBackMap_;
};
} // ResourceSchedule
} // OHOS

#endif // RESSCHED_INTERFACES_KITS_JS_SYSTEMLOAD_H