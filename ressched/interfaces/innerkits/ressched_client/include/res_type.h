/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: some definition for resource info.
 */

#ifndef FOUNDATION_RESOURCESCHEDULE_INTERFACES_INNERKITS_RESSCHED_CLIENT_INCLUDE_RES_TYPE_H
#define FOUNDATION_RESOURCESCHEDULE_INTERFACES_INNERKITS_RESSCHED_CLIENT_INCLUDE_RES_TYPE_H

#include <cstdint>

namespace OHOS {
namespace ResourceSchedule {
namespace ResType {
enum : uint32_t {
    // screen status, value 0 means screen off, value 1 means screen on, else are invalid.
    RES_TYPE_SCREEN_STATUS,
};
}
} // namespace ResourceSchedule
} // namespace OHOS

#endif // FOUNDATION_RESOURCESCHEDULE_INTERFACES_INNERKITS_RESSCHED_CLIENT_INCLUDE_RES_TYPE_H