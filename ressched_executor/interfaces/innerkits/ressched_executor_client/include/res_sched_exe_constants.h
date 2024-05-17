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

#ifndef RESSCHED_EXECUTOR_INTERFACES_INNERKITS_RESSCHED_EXECUTOR_CLIENT_INCLUDE_RES_SCHED_EXE_CONSTANTS_H
#define RESSCHED_EXECUTOR_INTERFACES_INNERKITS_RESSCHED_EXECUTOR_CLIENT_INCLUDE_RES_SCHED_EXE_CONSTANTS_H

#include <cstdint>

namespace OHOS {
namespace ResourceSchedule {
enum ResIpcType : uint32_t {
    REQUEST_SEND_SYNC = 0,
    REQUEST_SEND_ASYNC,
    REQUEST_KILL_PROCESS,
    REQUEST_SEND_DEBUG,
};

/**
 * common error code
 */
enum ResErrCode : int32_t {
    // no error
    RSSEXE_NO_ERR = 0,
    // permission denied
    RSSEXE_PERMISSION_DENIED,
    // last error code, unesd, please add error code before
    RSSEXE_LAST_ERR,
};

/**
 * ipc error code
 */
enum ResIpcErrCode : int32_t {
    // plugin deliever res error
    RSSEXE_PLUGIN_ERROR = -1,
    // ipc send data read/write error
    RSSEXE_DATA_ERROR = ResErrCode::RSSEXE_LAST_ERR,
    // client call proxy error
    RSSEXE_REQUEST_FAIL,
    // send request by ipc error
    RSSEXE_SEND_REQUEST_FAIL,
    // client get ressched_executor serveice error
    RSSEXE_GET_SERVICE_FAIL,
    // client connect to ressched_executor serveice error
    RSSEXE_CONNECT_FAIL,
};
} // namespace ResourceSchedule
} // namespace OHOS

#endif // RESSCHED_EXECUTOR_INTERFACES_INNERKITS_RESSCHED_EXECUTOR_CLIENT_INCLUDE_RES_SCHED_EXE_CONSTANTS_H
