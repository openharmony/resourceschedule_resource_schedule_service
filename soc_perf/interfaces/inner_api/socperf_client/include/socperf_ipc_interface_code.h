/*
 *  Copyright (c) 2023 Huawei Device Co., Ltd.
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

 * any change to socperf_ipc_interface_code.h needs to be reviewed by @leonchan5
 */

/* SAID:1906 */
namespace OHOS {
namespace SOCPERF {
    enum class SocPerfInterfaceCode {
        TRANS_IPC_ID_PERF_REQUEST             = 0x0001,
        TRANS_IPC_ID_PERF_REQUEST_EX          = 0x0002,
        TRANS_IPC_ID_POWER_LIMIT_BOOST_FREQ   = 0x0005,
        TRANS_IPC_ID_THERMAL_LIMIT_BOOST_FREQ = 0x0008,
        TRANS_IPC_ID_LIMIT_REQUEST            = 0x0009,
    };
} // namespace SOCPERF
} // namespace OHOS