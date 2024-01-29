/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include <cstdint>

#include <chrono>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <utility>

#include <unistd.h>

#include "plugin_mgr.h"
#include "rme_log_domain.h"
#include "event_runner.h"
#include "ffrt_inner.h"

#include "latency_control/inetwork_latency_switcher.h"
#include "latency_control/network_latency_controller.h"
#include "latency_control/noop_network_latency_switcher.h"
#include "latency_control/pmqos_network_latency_switcher.h"

#undef LOG_TAG
#define LOG_TAG "ueaServer-NetworkLatencyController"

namespace OHOS::ResourceSchedule {
namespace {
    const std::string NET_LATENCY_TIMER_NAME = "netLatTimer";
    const std::chrono::duration TIMEOUT = std::chrono::seconds(60); // 1 minute timeout
}

void NetworkLatencyController::Init()
{
    // use PMQoS switch if available
    int err = access(PmqosNetworkLatencySwitcher::PMQOS_PATH.data(), W_OK);
    if (!err) {
        RME_LOGI("%{public}s: using pmqos latency switcher", __func__);
        Init(std::make_unique<PmqosNetworkLatencySwitcher>());
        return;
    }

    // Another latency switchers can be implemented if required.
    // If nothing matched, use default object, which is noop switcher.
    RME_LOGI("%{public}s: using default latency switcher", __func__);
    Init(std::make_unique<NoopNetworkLatencySwitcher>());
}

void NetworkLatencyController::Init(std::unique_ptr<INetworkLatencySwitcher> sw)
{
    ffrtQueue_ = std::make_shared<ffrt::queue>("network_manager_ffrtQueue",
        ffrt::queue_attr().qos(ffrt::qos_user_interactive));
    if (ffrtQueue_ == nullptr) {
        RME_LOGE("%{public}s: failed: cannot allocate event handler", __func__);
        return;
    }

    switcher = std::move(sw);
}

void NetworkLatencyController::HandleRequest(long long value, const std::string &identity)
{
    if (!switcher || ffrtQueue_ == nullptr) {
        RME_LOGE("%{public}s: controller is not initialized", __func__);
        return;
    }

    switch (value) {
        case NETWORK_LATENCY_REQUEST_LOW:
            HandleAddRequest(identity);
            break;
        case NETWORK_LATENCY_REQUEST_NORMAL:
            HandleDelRequest(identity);
            break;
        default:
            RME_LOGW("%{public}s: invalid value: %{public}lld", __func__, value);
            return;
    }
}

void NetworkLatencyController::HandleAddRequest(const std::string &identity)
{
    // cancel auto disable task first
    std::unique_lock<ffrt::mutex> autoLock(latencyFfrtMutex_);
    for (auto iter = taskHandlerMap_.begin(); iter != taskHandlerMap_.end(); iter++) {
        if (iter->first == identity) {
            ffrtQueue_->cancel(iter->second);
            taskHandlerMap_.erase(iter->first);
        }
    }
    std::unique_lock<std::mutex> lk(mtx);

    RME_LOGD("%{public}s: add new request from %{public}s", __func__, identity.c_str());
    AddRequest(identity);

    // set up the auto disable timer
    taskHandlerMap_[identity] = ffrtQueue_->submit_h(
        [this, identity] { AutoDisableTask(identity); },
        ffrt::task_attr().delay(
            std::chrono::duration_cast<std::chrono::milliseconds>(TIMEOUT).count() * switchToFfrt_
            )
    );
}

void NetworkLatencyController::HandleDelRequest(const std::string &identity)
{
    // cancel auto disable task first
    std::unique_lock<ffrt::mutex> autoLock(latencyFfrtMutex_);
    for (auto iter = taskHandlerMap_.begin(); iter != taskHandlerMap_.end(); iter++) {
        if (iter->first == identity) {
            ffrtQueue_->cancel(iter->second);
            taskHandlerMap_.erase(iter->first);
        }
    }
    std::unique_lock<std::mutex> lk(mtx);

    RME_LOGD("%{public}s: delete request from %{public}s", __func__, identity.c_str());
    DelRequest(identity);
}

void NetworkLatencyController::AddRequest(const std::string &identity)
{
    bool wasEmpty = requests.empty();
    requests.insert(identity);

    // check whether it is the first request
    if (wasEmpty) {
        RME_LOGD("%{public}s: activating low latency", __func__);
        switcher->LowLatencyOn();
    }
}

void NetworkLatencyController::DelRequest(const std::string &identity)
{
    bool wasEmpty = requests.empty();
    requests.erase(identity);

    // check whether is was the last request
    if (!wasEmpty && requests.empty()) {
        RME_LOGD("%{public}s: no callers left, restore normal latency", __func__);
        switcher->LowLatencyOff();
    }
}

void NetworkLatencyController::AutoDisableTask(const std::string &identity)
{
    std::unique_lock<std::mutex> lk(mtx);

    RME_LOGD("%{public}s: identity %{public}s timed out", __func__, identity.c_str());
    DelRequest(identity);
}
} // namespace OHOS::ResourceSchedule
