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

#include "res_sched_service_proxy.h"
#include "ipc_types.h"                  // for NO_ERROR
#include "ipc_util.h"                   // for WRITE_PARCEL
#include "iremote_object.h"             // for IRemoteObject
#include "message_option.h"             // for MessageOption, MessageOption::TF_ASYNC
#include "message_parcel.h"             // for MessageParcel
#include "res_sched_log.h"              // for RESSCHED_LOGD, RESSCHED_LOGE

namespace OHOS {
namespace ResourceSchedule {
void ResSchedServiceProxy::ReportData(uint32_t resType, int64_t value, const nlohmann::json& payload)
{
    int32_t error;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option = { MessageOption::TF_ASYNC };
    WRITE_PARCEL(data, InterfaceToken, ResSchedServiceProxy::GetDescriptor(), , ResSchedServiceProxy);
    WRITE_PARCEL(data, Uint32, resType, , ResSchedServiceProxy);
    WRITE_PARCEL(data, Int64, value, , ResSchedServiceProxy);
    WRITE_PARCEL(data, String, payload.dump(), , ResSchedServiceProxy);
    error = Remote()->SendRequest(IResSchedService::REPORT_DATA, data, reply, option);
    if (error != NO_ERROR) {
        RESSCHED_LOGE("Send request error: %{public}d", error);
        return;
    }
    RESSCHED_LOGD("%{public}s, success.", __func__);
}
} // namespace ResourceSchedule
} // namespace OHOS
