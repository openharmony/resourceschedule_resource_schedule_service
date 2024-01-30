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

#ifndef DEVICE_STANDBY_PLUGIN_H
#define DEVICE_STANDBY_PLUGIN_H
#ifdef RSS_DEVICE_STANDBY_ENABLE

#include <set>

#include "event_handler.h"
#include "nlohmann/json.hpp"

#include "plugin.h"
#include "single_instance.h"

namespace OHOS {
namespace ResourceSchedule {
class DeviceStandbyPlugin : public Plugin {
    DECLARE_SINGLE_INSTANCE(DeviceStandbyPlugin)

public:
    void Init() override;
    void Disable() override;
    void DispatchResource(const std::shared_ptr<ResData>& resData) override;

private:
    std::set<uint32_t> resTypes_;
};
} // namespace ResourceSchedule
} // namespace OHOS
#endif // RSS_DEVICE_STANDBY_ENABLE
#endif // DEVICE_STANDBY_PLUGIN_H

