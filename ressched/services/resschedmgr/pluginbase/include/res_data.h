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

#ifndef RESSCHED_SERVICES_RESSCHEDMGR_PLUGINBASE_INCLUDE_RES_DATA_H
#define RESSCHED_SERVICES_RESSCHEDMGR_PLUGINBASE_INCLUDE_RES_DATA_H

#include <string>
#include "nlohmann/json.hpp"

namespace OHOS {
namespace ResourceSchedule {
struct ResData {
    ResData() = default;
    ~ResData() = default;

    ResData(uint32_t type, int64_t value, const nlohmann::json& payload) : resType(type),
        value(value), payload(payload) {}

    uint32_t resType = 0;
    int64_t value = 0;
    nlohmann::json payload;
};
} // namespace ResourceSchedule
} // namespace OHOS

#endif // RESSCHED_SERVICES_RESSCHEDMGR_PLUGINBASE_INCLUDE_RES_DATA_H
