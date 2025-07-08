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

#include "ohos.resourceschedule.systemload.proj.hpp"
#include "ohos.resourceschedule.systemload.impl.hpp"
#include "taihe/runtime.hpp"
#include "stdexcept"
#include "res_sched_client.h"
#include "res_sched_log.h"

using namespace taihe;
using namespace ohos::resourceschedule::systemload;
using namespace OHOS::ResourceSchedule;
using namespace OHOS;
namespace {
class SystemloadCallback : public ResSchedSystemloadNotifierClient {
private:
    std::shared_ptr<taihe::callback<void(SystemLoadLevel)>> innerCallback_ = nullptr;
public:
    explicit SystemloadCallback(std::shared_ptr<taihe::callback<void(SystemLoadLevel)>> callback)
        : innerCallback_(callback) {}
    virtual ~SystemloadCallback() = default;
    void OnSystemloadLevel(int32_t level)
    {
        (*innerCallback_)(SystemLoadLevel(static_cast<SystemLoadLevel::key_t>(level)));
    }
    using CallbackPair = std::pair<ani_ref, sptr<SystemloadCallback>>;
    static std::list<CallbackPair> callbackList_;
    static std::mutex callbackMutex_;
};
std::list<SystemloadCallback::CallbackPair> SystemloadCallback::callbackList_ = {};
std::mutex SystemloadCallback::callbackMutex_;

void onSystemLoadChange(callback_view<void(SystemLoadLevel)> callback, uintptr_t opq)
{
    RESSCHED_LOGD("Register Systemload Callback");
    std::lock_guard<std::mutex> autoLock(SystemloadCallback::callbackMutex_);
    auto iter = SystemloadCallback::callbackList_.begin();
    ani_ref ref = (ani_ref)(opq);
    ani_env* env = taihe::get_env();
    for (; iter != SystemloadCallback::callbackList_.end(); iter++) {
        ani_boolean isEquals = false;
        if (env->Reference_StrictEquals(iter->first, ref, &isEquals) == ANI_OK && isEquals) {
            RESSCHED_LOGW("Register a exist callback");
            return;
        }
    }
    std::shared_ptr<taihe::callback<void(SystemLoadLevel)>> taiheCallback =
        std::make_shared<taihe::callback<void(SystemLoadLevel)>>(callback);
    sptr<SystemloadCallback> systemloadListener =
        new (std::nothrow)SystemloadCallback(taiheCallback);
    ResSchedClient::GetInstance().RegisterSystemloadNotifier(systemloadListener);
    SystemloadCallback::callbackList_.emplace_back(std::move(ref), systemloadListener);
    RESSCHED_LOGD("Register Systemload Callback end");
}

void offSystemLoadChange(callback_view<void(SystemLoadLevel)> callback, uintptr_t opq)
{
    RESSCHED_LOGD("UnRegister Systemload Callback");
    std::lock_guard<std::mutex> autoLock(SystemloadCallback::callbackMutex_);
    auto iter = SystemloadCallback::callbackList_.begin();
    ani_ref ref = (ani_ref)(opq);
    ani_env* env = taihe::get_env();
    for (; iter != SystemloadCallback::callbackList_.end(); iter++) {
        ani_boolean isEquals = false;
        if (env->Reference_StrictEquals(iter->first, ref, &isEquals) == ANI_OK && isEquals) {
            ResSchedClient::GetInstance().UnRegisterSystemloadNotifier(iter->second);
            SystemloadCallback::callbackList_.erase(iter);
            RESSCHED_LOGD("UnRegister Systemload Callback end");
            break;
        }
    }
}

SystemLoadLevel getLevelSync()
{
    return SystemLoadLevel(static_cast<SystemLoadLevel::key_t>(ResSchedClient::GetInstance().GetSystemloadLevel()));
}
} // namespace
TH_EXPORT_CPP_API_onSystemLoadChange(onSystemLoadChange);
TH_EXPORT_CPP_API_offSystemLoadChange(offSystemLoadChange);
TH_EXPORT_CPP_API_getLevelSync(getLevelSync);