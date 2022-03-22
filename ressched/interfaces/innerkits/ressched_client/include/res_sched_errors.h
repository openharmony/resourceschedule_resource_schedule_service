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

#ifndef RESSCHED_INTERFACES_INNERKITS_RESSCHED_CLIENT_INCLUDE_RES_SCHED_ERRORS_H
#define RESSCHED_INTERFACES_INNERKITS_RESSCHED_CLIENT_INCLUDE_RES_SCHED_ERRORS_H

#include "errors.h"

namespace OHOS {
namespace ResourceSchedule {
enum {
    RES_SCHED_MODULE_COMMON = 0x00,
    RES_SCHED_MODULE_SERVICE = 0x01,
};

constexpr ErrCode RES_SCHED_COMMON_ERR_OFFSET = ErrCodeOffset(SUBSYS_IAWARE, RES_SCHED_MODULE_COMMON);
enum {
    ERR_RES_SCHED_INVALID_PARAM = RES_SCHED_COMMON_ERR_OFFSET + 1,
    GET_RES_SCHED_SERVICE_FAILED,
};

constexpr ErrCode RES_SCHED_SERVICE_ERR_OFFSET = ErrCodeOffset(SUBSYS_IAWARE, RES_SCHED_MODULE_SERVICE);
enum {
    ERR_RES_SCHED_PARCEL_ERROR = RES_SCHED_SERVICE_ERR_OFFSET + 1,
    ERR_RES_SCHED_PERMISSION_DENIED,
    ERR_RES_SCHED_WRITE_FILE_FAILED,
};
} // namespace ResourceSchedule
} // namespace OHOS

#endif // RESSCHED_INTERFACES_INNERKITS_RESSCHED_CLIENT_INCLUDE_RES_SCHED_ERRORS_H
