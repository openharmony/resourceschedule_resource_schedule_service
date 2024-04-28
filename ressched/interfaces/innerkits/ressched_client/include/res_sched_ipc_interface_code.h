/*
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
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

 * any change to res_sched_ipc_interface_code.h needs to be reviewed by @leonchan5
 */

#ifndef RESSCHED_INTERFACES_INNERKITS_RESSCHED_CLIENT_INCLUDE_RES_SCHED_IPC_INTERFACE_CODE_H
#define RESSCHED_INTERFACES_INNERKITS_RESSCHED_CLIENT_INCLUDE_RES_SCHED_IPC_INTERFACE_CODE_H

/* SAID:1901 */
namespace OHOS {
namespace ResourceSchedule {
    enum class ResourceScheduleInterfaceCode {
        REPORT_DATA = 1,
        KILL_PROCESS = 2,
        REGISTER_SYSTEMLOAD_NOTIFIER = 3,
        UNREGISTER_SYSTEMLOAD_NOTIFIER = 4,
        GET_SYSTEMLOAD_LEVEL = 5,
        TOUCH_DOWN_APP_PRELOAD = 6,
    };

    enum class ResourceSceduleSystemloadNotifierCode {
        SYSTEMLOAD_LEVEL = 1,
    };
} // namespace ResourceSchedule
} // namespace OHOS

#endif // RESSCHED_INTERFACES_INNERKITS_RESSCHED_CLIENT_INCLUDE_RES_SCHED_IPC_INTERFACE_CODE_H
