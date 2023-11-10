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

#ifndef NETWORK_LATENCY_CONTROLLER_H
#define NETWORK_LATENCY_CONTROLLER_H

#include <chrono>
#include <memory>
#include <mutex>
#include <string>
#include <utility>
#include <unordered_set>

#include "inner_event.h"
#include "ffrt.h"

#include "inetwork_latency_switcher.h"

namespace OHOS::ResourceSchedule {
class NetworkLatencyController {
public:
    void Init();
    void Init(std::unique_ptr<INetworkLatencySwitcher> switcher);
    void HandleRequest(long long value, const std::string &identity);

    static const long long NETWORK_LATENCY_REQUEST_LOW = 0;
    static const long long NETWORK_LATENCY_REQUEST_NORMAL = 1;

private:
    void HandleAddRequest(const std::string &identity);
    void HandleDelRequest(const std::string &identity);
    void AddRequest(const std::string &identity);
    void DelRequest(const std::string &identity);
    void AutoDisableTask(const std::string &identity);

    std::mutex mtx;
    std::shared_ptr<ffrt::queue> ffrtQueue_{nullptr};
    ffrt::mutex latencyFfrtMutex_;
    int32_t switchToFfrt_ = 1000;
    std::unique_ptr<INetworkLatencySwitcher> switcher;
    std::unordered_set<std::string> requests;
    std::unordered_map<std::string, ffrt::task_handle> taskHandlerMap_;
};
} // namespace OHOS::ResourceSchedule

#endif // NETWORK_LATENCY_CONTROLLER_H
