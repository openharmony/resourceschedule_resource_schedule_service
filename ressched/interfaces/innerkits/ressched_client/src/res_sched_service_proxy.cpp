/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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
#include "ipc_types.h"                       // for NO_ERROR
#include "ipc_util.h"                        // for WRITE_PARCEL
#include "iremote_object.h"                  // for IRemoteObject
#include "message_option.h"                  // for MessageOption, MessageOption::TF_ASYNC
#include "message_parcel.h"                  // for MessageParcel
#include "res_sched_errors.h"                // for GET_RES_SCHED_SERVICE_FAILED
#include "res_sched_log.h"                   // for RESSCHED_LOGD, RESSCHED_LOGE
#include "res_sched_ipc_interface_code.h"    // for ResourceScheduleInterfaceCode

namespace OHOS {
namespace ResourceSchedule {
void ResSchedServiceProxy::ReportData(uint32_t resType, int64_t value, const nlohmann::json& payload)
{
    int32_t error;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option = { MessageOption::TF_ASYNC };
    WRITE_PARCEL(data, InterfaceToken, ResSchedServiceProxy::GetDescriptor(), void(), ResSchedServiceProxy);
    WRITE_PARCEL(data, Uint32, resType, void(), ResSchedServiceProxy);
    WRITE_PARCEL(data, Int64, value, void(), ResSchedServiceProxy);
    WRITE_PARCEL(data, String, payload.dump(-1, ' ', false, nlohmann::detail::error_handler_t::replace), void(),
        ResSchedServiceProxy);
    error = Remote()->SendRequest(static_cast<uint32_t>(ResourceScheduleInterfaceCode::REPORT_DATA),
        data, reply, option);
    if (error != NO_ERROR) {
        RESSCHED_LOGE("Send request error: %{public}d.", error);
        return;
    }
    RESSCHED_LOGD("%{public}s, success.", __func__);
}

int32_t ResSchedServiceProxy::KillProcess(const nlohmann::json& payload)
{
    int32_t error;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option = { MessageOption::TF_SYNC };
    WRITE_PARCEL(data, InterfaceToken, ResSchedServiceProxy::GetDescriptor(), RES_SCHED_DATA_ERROR,
        ResSchedServiceProxy);
    WRITE_PARCEL(data, String, payload.dump(-1, ' ', false, nlohmann::detail::error_handler_t::replace),
        RES_SCHED_DATA_ERROR, ResSchedServiceProxy);
    error = Remote()->SendRequest(static_cast<uint32_t>(ResourceScheduleInterfaceCode::KILL_PROCESS),
        data, reply, option);
    if (error != NO_ERROR) {
        RESSCHED_LOGE("Send request error: %{public}d.", error);
        return RES_SCHED_REQUEST_FAIL;
    }
    RESSCHED_LOGD("%{public}s, success.", __func__);
    return reply.ReadInt32();
}

void ResSchedServiceProxy::RegisterSystemloadNotifier(const sptr<IRemoteObject>& notifier)
{
    int32_t error;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option = { MessageOption::TF_SYNC };
    WRITE_PARCEL(data, InterfaceToken, ResSchedServiceProxy::GetDescriptor(), void(), ResSchedServiceProxy);
    WRITE_PARCEL(data, RemoteObject, notifier, void(), ResSchedServiceProxy);
    error = Remote()->SendRequest(static_cast<uint32_t>(ResourceScheduleInterfaceCode::REGISTER_SYSTEMLOAD_NOTIFIER),
        data, reply, option);
    if (error != NO_ERROR) {
        RESSCHED_LOGE("Send request error: %{public}d.", error);
        return;
    }
    RESSCHED_LOGD("%{public}s, success.", __func__);
}

void ResSchedServiceProxy::UnRegisterSystemloadNotifier()
{
    int32_t error;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option = { MessageOption::TF_SYNC };
    WRITE_PARCEL(data, InterfaceToken, ResSchedServiceProxy::GetDescriptor(), void(), ResSchedServiceProxy);
    error = Remote()->SendRequest(
        static_cast<uint32_t>(ResourceScheduleInterfaceCode::UNREGISTER_SYSTEMLOAD_NOTIFIER), data, reply, option);
    if (error != NO_ERROR) {
        RESSCHED_LOGE("Send request error: %{public}d.", error);
        return;
    }
    RESSCHED_LOGD("%{public}s, success.", __func__);
}

int32_t ResSchedServiceProxy::GetSystemloadLevel()
{
    int32_t error;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option = { MessageOption::TF_SYNC };
    WRITE_PARCEL(data, InterfaceToken, ResSchedServiceProxy::GetDescriptor(), RES_SCHED_DATA_ERROR,
        ResSchedServiceProxy);
    error = Remote()->SendRequest(static_cast<uint32_t>(ResourceScheduleInterfaceCode::GET_SYSTEMLOAD_LEVEL),
        data, reply, option);
    if (error != NO_ERROR) {
        RESSCHED_LOGE("Send request error: %{public}d.", error);
        return RES_SCHED_REQUEST_FAIL;
    }
    RESSCHED_LOGD("%{public}s, success.", __func__);
    return reply.ReadInt32();
}

bool ResSchedServiceProxy::IsAllowedAppPreload(const std::string& bundleName, int32_t preloadMode)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option = { MessageOption::TF_SYNC };
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        return false;
    }

    WRITE_PARCEL(data, String, bundleName, false, ResSchedServiceProxy);
    WRITE_PARCEL(data, Int32, preloadMode, false, ResSchedServiceProxy);

    auto remote = Remote();
    if (!remote) {
        RESSCHED_LOGE("Get remote failed");
        return false;
    }

    int32_t error = remote->SendRequest(static_cast<uint32_t>(ResourceScheduleInterfaceCode::TOUCH_DOWN_APP_PRELOAD),
        data, reply, option);
    if (error != NO_ERROR) {
        RESSCHED_LOGE("Send request error: %{public}d.", error);
        return false;
    }

    bool isAllowedPreload = false;
    if (!reply.ReadBool(isAllowedPreload)) {
        RESSCHED_LOGE("Read result failed");
        return false;
    }
    RESSCHED_LOGD("%{public}s, success.", __func__);
    return isAllowedPreload;
}
} // namespace ResourceSchedule
} // namespace OHOS
