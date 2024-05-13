/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef RESSCHED_EXECUTOR_INTERFACES_INNERKITS_RESSCHED_EXECUTOR_CLIENT_INCLUDE_RES_EXE_TYPE_H
#define RESSCHED_EXECUTOR_INTERFACES_INNERKITS_RESSCHED_EXECUTOR_CLIENT_INCLUDE_RES_EXE_TYPE_H

#include <cstdint>

namespace OHOS {
namespace ResourceSchedule {
namespace ResExeType {
enum : uint32_t {
    // first resType, please add resType after
    RES_TYPE_FIRST = 0,
    // common resType for plugin
    RES_TYPE_COMMON_SYNC = RES_TYPE_FIRST,
    // thermal aware event sync
    RES_TYPE_THERMAL_AWARE_SYNC_EVENT,
    // last sync resType, please add sync resType before
    RES_TYPE_SYNC_END,

    // first async resType, please add async resType after
    RES_TYPE_ASYNC_FIRST = RES_TYPE_SYNC_END,
    // common resType for plugin
    RES_TYPE_COMMON_ASYNC = RES_TYPE_ASYNC_FIRST,
    // thermal aware event async
    RES_TYPE_THERMAL_AWARE_ASYNC_EVENT,
    // last resType, please add resType before
    RES_TYPE_LAST = 1000,
    // resType for debug ipc
    RES_TYPE_DEBUG = RES_TYPE_LAST,
};
} // namespace ResExeType
} // namespace ResourceSchedule
} // namespace OHOS

#endif // RESSCHED_EXECUTOR_INTERFACES_INNERKITS_RESSCHED_EXECUTOR_CLIENT_INCLUDE_RES_EXE_TYPE_H
