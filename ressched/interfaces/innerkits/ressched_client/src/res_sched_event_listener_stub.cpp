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

#include "res_sched_event_listener_stub.h"

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
namespace {
    std::unordered_map<std::string, std::string> StringToStringJsonObj(const std::string& extInfo)
    {
        std::unordered_map<std::string, std::string> ret;
        nlohmann::json jsonObj = nlohmann::json::object();
        if (extInfo.empty()) {
            return ret;
        }
        nlohmann::json jsonTmp = nlohmann::json::parse(extInfo, nullptr, false);
        if (jsonTmp.is_discarded()) {
            RESSCHED_LOGE("%{public}s parse extInfo to json failed: %{public}s.", __func__, extInfo.c_str());
            return ret;
        }
        if (!jsonTmp.is_object()) {
            RESSCHED_LOGD("%{public}s extInfo converted result is not a jsonObj: %{public}s.",
                __func__, extInfo.c_str());
            return ret;
        }
        return jsonTmp;
    }
}

int32_t ResSchedEventListenerStub::OnRemoteRequest(uint32_t code, MessageParcel& data,
    MessageParcel& reply, MessageOption& option)
{
    RESSCHED_LOGI("event listener ipc code = %{public}u", code);
    switch (code) {
        case static_cast<uint32_t>(ResourceScedulEventListenerCode::RECEIVE_EVENT): {
            return OnReceiveEventInner(data, reply);
        }
        default: {
            RESSCHED_LOGE("event listener unknown request code, please check");
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
        }
    }
}

int32_t ResSchedEventListenerStub::OnReceiveEventInner(MessageParcel& data, MessageParcel& reply)
{
    uint32_t eventType = 0;
    uint32_t eventValue = 0;
    std::string extInfo;
    int32_t ret = ParseAndCheckEventParcel(data, eventType, eventValue, extInfo);
    if (ret != ERR_OK) {
        RESSCHED_LOGE("%{public}s: parse fail=%{public}d", __func__, ret);
        return ret;
    }
    OnReceiveEvent(eventType, eventValue, StringToStringJsonObj(extInfo));
    return ERR_OK;
}

int32_t ResSchedEventListenerStub::ParseAndCheckEventParcel(MessageParcel& data, uint32_t& eventType,
    uint32_t& eventValue, std::string& extInfo)
{
    std::u16string descriptor = IResSchedEventListener::GetDescriptor();
    std::u16string remoteDescriptor = data.ReadInterfaceToken();
    if (descriptor != remoteDescriptor) {
        return ERR_RES_SCHED_PARCEL_ERROR;
    }
    READ_PARCEL(data, Uint32, eventType, ERR_RES_SCHED_PARCEL_ERROR, ResSchedEventListenerStub);
    READ_PARCEL(data, Uint32, eventValue, ERR_RES_SCHED_PARCEL_ERROR, ResSchedEventListenerStub);
    READ_PARCEL(data, String, extInfo, ERR_RES_SCHED_PARCEL_ERROR, ResSchedEventListenerStub);
    return ERR_OK;
}
} // namespace ResourceSchedule
} // namespace OHOS
