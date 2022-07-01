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

#ifndef NOOP_NETWORK_LATENCY_SWITCHER_H
#define NOOP_NETWORK_LATENCY_SWITCHER_H

#include "inetwork_latency_switcher.h"

namespace OHOS::ResourceSchedule {
// Noop switcher is used as a default object and does nothing
class NoopNetworkLatencySwitcher : public INetworkLatencySwitcher {
public:
    virtual void LowLatencyOn() override {}
    virtual void LowLatencyOff() override {}
};
} // namespace OHOS::ResourceSchedule

#endif // NOOP_NETWORK_LATENCY_SWITCHER_H
