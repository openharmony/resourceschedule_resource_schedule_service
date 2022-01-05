/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: save reported resource data.
 */

#ifndef FOUNDATION_RESOURCESCHEDULE_SERVICES_RESSCHEDMGR_PLUGINBASE_INCLUDE_RES_DATA_H
#define FOUNDATION_RESOURCESCHEDULE_SERVICES_RESSCHEDMGR_PLUGINBASE_INCLUDE_RES_DATA_H

#include <cstdint>
#include <string>

namespace OHOS {
namespace ResourceSchedule {
struct ResData {
    ResData() = default;
    ~ResData() = default;

    ResData(uint32_t type, int64_t value, const std::string& payload) : resType(type), value(value), payload(payload) {}

    uint32_t resType = 0;
    int64_t value = 0;
    std::string payload;
};
} // namespace ResourceSchedule
} // namespace OHOS

#endif // FOUNDATION_RESOURCESCHEDULE_SERVICES_RESSCHEDMGR_PLUGINBASE_INCLUDE_RES_DATA_H