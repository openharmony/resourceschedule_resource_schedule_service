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

#include "res_sched_event_listener_proxy.h"

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
void ResSchedEventListenerProxy::OnReceiveEvent(uint32_t eventType, uint32_t eventValue,
    const nlohmann::json& extInfo)
{
    int32_t error;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option = { MessageOption::TF_ASYNC };
    error = WriteParcelForReceiveEvent(eventType, eventValue, extInfo, data);
    if (error != NO_ERROR) {
        RESSCHED_LOGE("%{public}s:write parcel error: %{public}d.", __func__, error);
        return;
    }
    error = Remote()->SendRequest(static_cast<uint32_t>(ResourceScedulEventListenerCode::RECEIVE_EVENT),
        data, reply, option);
    if (error != NO_ERROR) {
        RESSCHED_LOGE("%{public}s:Send request error: %{public}d.", __func__, error);
        return;
    }
    RESSCHED_LOGD("%{public}s, success.", __func__);
}

int32_t ResSchedEventListenerProxy::WriteParcelForReceiveEvent(const uint32_t eventType,
    const int64_t eventValue, const nlohmann::json& extInfo, MessageParcel& data)
{
    WRITE_PARCEL(data, InterfaceToken, ResSchedEventListenerProxy::GetDescriptor(), RES_SCHED_DATA_ERROR,
        ResSchedEventListenerProxy);
    WRITE_PARCEL(data, Uint32, eventType, RES_SCHED_DATA_ERROR, ResSchedEventListenerProxy);
    WRITE_PARCEL(data, Uint32, eventValue, RES_SCHED_DATA_ERROR, ResSchedEventListenerProxy);
    WRITE_PARCEL(data, String, extInfo.dump(-1, ' ', false, nlohmann::detail::error_handler_t::replace),
        RES_SCHED_DATA_ERROR, ResSchedEventListenerProxy);
    return NO_ERROR;
}
} // namespace ResourceSchedule
} // namespace OHOS
