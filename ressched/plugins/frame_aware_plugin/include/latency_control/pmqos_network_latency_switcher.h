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

#ifndef PMQOS_NETWORK_LATENCY_CONTROLLER_H
#define PMQOS_NETWORK_LATENCY_CONTROLLER_H

#include <cerrno>
#include <cstddef>
#include <optional>
#include <string>
#include <string_view>
#include <utility>

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <unique_fd.h>

#include "inetwork_latency_switcher.h"
#include "res_sched_log.h"

namespace OHOS::ResourceSchedule {
class PmqosNetworkLatencySwitcher : public INetworkLatencySwitcher {
public:
    static inline std::string_view PMQOS_PATH = "/dev/network_latency";

    virtual void LowLatencyOn() override
    {
        if (pmqosFd) {
            RESSCHED_LOGE("%{public}s: fd is already opened", __func__);
            return;
        }

        UniqueFd fd(open(PMQOS_PATH.data(), O_WRONLY));
        if (fd == -1) {
            RESSCHED_LOGE("%{public}s: cannot open device file: %{public}d", __func__, errno);
            return;
        }

        ssize_t ret = write(fd, &PMQOS_LOW_LATENCY_VALUE, sizeof(PMQOS_LOW_LATENCY_VALUE));
        if (ret == -1) {
            RESSCHED_LOGE("%{public}s: cannot write to device: %{public}d", __func__, errno);
            return;
        }

        pmqosFd = std::move(fd);
        RESSCHED_LOGD("%{public}s: pmqos activated", __func__);
    }

    virtual void LowLatencyOff() override
    {
        if (!pmqosFd) {
            RESSCHED_LOGE("%{public}s: fd is already closed", __func__);
            return;
        }

        pmqosFd.reset();
        RESSCHED_LOGD("%{public}s: pmqos deactivated", __func__);
    }
private:
    static inline int32_t PMQOS_LOW_LATENCY_VALUE = 0;
    std::optional<UniqueFd> pmqosFd;
};
} // namespace OHOS::ResourceSchedule

#endif // PMQOS_NETWORK_LATENCY_CONTROLLER_H
