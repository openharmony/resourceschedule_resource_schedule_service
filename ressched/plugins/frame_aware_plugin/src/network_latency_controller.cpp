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

#include <memory>
#include <string>
#include <utility>

#include <unistd.h>

#include "inetwork_latency_switcher.h"
#include "noop_network_latency_switcher.h"
#include "pmqos_network_latency_switcher.h"
#include "res_sched_log.h"

#include "network_latency_controller.h"

namespace OHOS::ResourceSchedule {
void NetworkLatencyController::Init()
{
    // use PMQoS switch if available
    int err = access(PmqosNetworkLatencySwitcher::PMQOS_PATH.data(), W_OK);
    if (!err) {
        RESSCHED_LOGI("%{public}s: using pmqos latency switcher", __func__);
        Init(std::make_unique<PmqosNetworkLatencySwitcher>());
        return;
    }

    // Another latency switchers can be implemented if required.
    // If nothing matched, use default object, which is noop switcher.
    RESSCHED_LOGI("%{public}s: using default latency switcher", __func__);
    Init(std::make_unique<NoopNetworkLatencySwitcher>());
}

void NetworkLatencyController::Init(std::unique_ptr<INetworkLatencySwitcher> sw)
{
    switcher = std::move(sw);
}

void NetworkLatencyController::HandleRequest(long long value, const std::string &identity)
{
    if (identity.empty()) {
        RESSCHED_LOGW("%{public}s: empty identity provided", __func__);
        return;
    }

    switch (value) {
        case NETWORK_LATENCY_REQUEST_LOW:
            AddRequest(identity);
            break;
        case NETWORK_LATENCY_REQUEST_NORMAL:
            DelRequest(identity);
            break;
        default:
            RESSCHED_LOGW("%{public}s: invalid value: %{public}lld", __func__, value);
            return;
    }
}

void NetworkLatencyController::AddRequest(const std::string &identity)
{
    bool wasEmpty = requests.empty();

    if (requests.find(identity) != requests.end()) {
        RESSCHED_LOGW("%{public}s: identity %{public}s already registered", __func__, identity.c_str());
        return;
    }

    requests.insert(identity);
    if (wasEmpty) {
        RESSCHED_LOGD("%{public}s: activating low latency", __func__);
        switcher->LowLatencyOn();
    }
}

void NetworkLatencyController::DelRequest(const std::string &identity)
{
    if (requests.erase(identity) == 0) {
        RESSCHED_LOGW("%{public}s: identity %{public}s not registered", __func__, identity.c_str());
        return;
    }

    if (requests.empty()) {
        RESSCHED_LOGD("%{public}s: restore normal latency", __func__);
        switcher->LowLatencyOff();
    }
}
} // namespace OHOS::ResourceSchedule
