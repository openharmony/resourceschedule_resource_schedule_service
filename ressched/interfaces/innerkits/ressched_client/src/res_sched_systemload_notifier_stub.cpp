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

#include "res_sched_systemload_notifier_stub.h"

#include "errors.h"
#include "ipc_object_stub.h"
#include "ipc_types.h"
#include "ipc_util.h"
#include "res_sched_errors.h"
#include "res_sched_ipc_interface_code.h"
#include "res_sched_log.h"
#include "res_type.h"

namespace OHOS {
namespace ResourceSchedule {

int32_t ResSchedSystemloadNotifierStub::OnRemoteRequest(uint32_t code, MessageParcel& data,
                                                        MessageParcel& reply, MessageOption& option)
{
    RESSCHED_LOGI("systemload notifier ipc code = %{public}u", code);
    switch (code) {
        case static_cast<uint32_t>(ResourceSceduleSystemloadNotifierCode::SYSTEMLOAD_LEVEL): {
            return OnSystemloadLevelInner(data, reply);
        }
        default: {
            RESSCHED_LOGE("unknown request code, please check");
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
        }
    }
}

int32_t ResSchedSystemloadNotifierStub::OnSystemloadLevelInner(MessageParcel& data, MessageParcel& reply)
{
    std::u16string descriptor = IResSchedSystemloadNotifier::GetDescriptor();
    std::u16string remoteDescriptor = data.ReadInterfaceToken();
    if (descriptor != remoteDescriptor) {
        RESSCHED_LOGE("descriptor check failed");
        return ERR_INVALID_VALUE;
    }
    int32_t level = static_cast<int32_t>(ResType::SystemloadLevel::LOW);
    READ_PARCEL(data, Int32, level, ERR_RES_SCHED_PARCEL_ERROR, ResSchedSystemloadNotifierStub);
    if ((level > static_cast<int32_t>(ResType::SystemloadLevel::ESCAPE))
        || (level < static_cast<int32_t>(ResType::SystemloadLevel::LOW))) {
        RESSCHED_LOGE("wrong level type.");
        return ERR_INVALID_VALUE;
    }
    OnSystemloadLevel(level);
    return ERR_OK;
}
} // namespace ResourceSchedule
} // namespace OHOS
