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

#include "res_sched_systemload_notifier_proxy.h"

#include "ipc_types.h"
#include "ipc_util.h"
#include "iremote_object.h"
#include "message_option.h"
#include "message_parcel.h"
#include "res_sched_errors.h"
#include "res_sched_log.h"
#include "res_sched_ipc_interface_code.h"

namespace OHOS {
namespace ResourceSchedule {
void ResSchedSystemloadNotifierProxy::OnSystemloadLevel(int32_t level)
{
    RESSCHED_LOGD("ResSchedSystemloadNotifierProxy OnSystemloadLevel");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    WRITE_PARCEL(
        data, InterfaceToken, ResSchedSystemloadNotifierProxy::GetDescriptor(), void(), ResSchedServiceProxy);
    WRITE_PARCEL(data, Int32, level, void(), ResSchedServiceProxy);
    int32_t errCode = Remote()->SendRequest(static_cast<uint32_t>
        (ResourceSceduleSystemloadNotifierCode::SYSTEMLOAD_LEVEL), data, reply, option);
    RESSCHED_LOGD("result = %{public}d", errCode);
}
} // namespace ResourceSchedule
} // namespace OHOS
