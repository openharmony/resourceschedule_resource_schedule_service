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

#include "res_sched_service_stub.h"
#include "res_sched_log.h"
#include "res_sched_errors.h"
#include "ipc_skeleton.h"
#include "ipc_util.h"

namespace OHOS {
namespace ResourceSchedule {
namespace {
    constexpr int SYSTEM_UID = 1000;
    constexpr int ROOT_UID = 0;

    bool CheckPermission(int uid)
    {
        return uid == SYSTEM_UID || uid == ROOT_UID;
    }

    bool IsValidToken(MessageParcel& data)
    {
        std::u16string descriptor = ResSchedServiceStub::GetDescriptor();
        std::u16string remoteDescriptor = data.ReadInterfaceToken();
        return descriptor == remoteDescriptor;
    }
}

ResSchedServiceStub::ResSchedServiceStub()
{
    Init();
}

ResSchedServiceStub::~ResSchedServiceStub()
{
    funcMap_.clear();
}

int32_t ResSchedServiceStub::ReportDataInner(MessageParcel& data, [[maybe_unused]] MessageParcel& reply)
{
    if (!IsValidToken(data)) {
        return ERR_RES_SCHED_PARCEL_ERROR;
    }
    uint32_t type = 0;
    READ_PARCEL(data, Uint32, type, ERR_RES_SCHED_PARCEL_ERROR, ResSchedServiceStub);

    int64_t value = 0;
    READ_PARCEL(data, Int64, value, ERR_RES_SCHED_PARCEL_ERROR, ResSchedServiceStub);

    std::string payload;
    READ_PARCEL(data, String, payload, ERR_RES_SCHED_PARCEL_ERROR, ResSchedServiceStub);

    ReportData(type, value, payload);
    return ERR_OK;
}

int ResSchedServiceStub::OnRemoteRequest(uint32_t code, MessageParcel &data,
    MessageParcel &reply, MessageOption &option)
{
    auto uid = IPCSkeleton::GetCallingUid();
    RESSCHED_LOGD("ResSchedServiceStub::OnRemoteRequest, code = %{public}d, flags = %{public}d,"
        " uid = %{public}d", code, option.GetFlags(), uid);
    if (!CheckPermission(uid)) {
        RESSCHED_LOGE("ResSchedServiceStub::OnRemoteRequest failed, illegal calling uid %{public}d。", uid);
        return ERR_RES_SCHED_PERMISSION_DENIED;
    }

    auto itFunc = funcMap_.find(code);
    if (itFunc != funcMap_.end()) {
        auto requestFunc = itFunc->second;
        if (requestFunc != nullptr) {
            return requestFunc(data, reply);
        }
    }
    return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
}

void ResSchedServiceStub::Init()
{
    funcMap_ = {
            { REPORT_DATA,
                [this](auto& data, auto& reply) {return ReportDataInner(data, reply); } },
    };
}
} // namespace ResourceSchedule
} // namespace OHOS