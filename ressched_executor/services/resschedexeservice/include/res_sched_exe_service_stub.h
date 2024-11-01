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

#ifndef RESSCHED_EXECUTOR_SERVICES_RESSCHEDEXESERVICE_INCLUDE_RES_SCHED_EXE_SERVICE_STUB_H
#define RESSCHED_EXECUTOR_SERVICES_RESSCHEDEXESERVICE_INCLUDE_RES_SCHED_EXE_SERVICE_STUB_H

#include <cstdint>

#include "iremote_stub.h"

#include "ires_sched_exe_service.h"

namespace OHOS {
namespace ResourceSchedule {
class ResSchedExeServiceStub : public IRemoteStub<IResSchedExeService> {
public:
    /**
     * @brief Construct a new ResSchedExeServiceStub object.
     *
     * @param isSaSerial Is SA serialize
     */
    ResSchedExeServiceStub(bool isSaSerial = true);

    /**
     * @brief Destroy the ResSchedExeServiceStub object.
     */
    ~ResSchedExeServiceStub();

    /**
     * @brief Recieve remote ipc request.
     *
     * @param code Indicates the request code.
     * @param data Indicates the object client send.
     * @param reply Indicates the object server reply.
     * @param option Indicates the ipc type.
     * @return function result.
     */
    int32_t OnRemoteRequest(
        uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;

private:
    int32_t ReportRequestInner(MessageParcel& data, MessageParcel& reply);
    int32_t KillProcessInner(MessageParcel& data, MessageParcel& reply);
    int32_t ReportDebugInner(MessageParcel& data);
    bool ParseParcel(MessageParcel& data, uint32_t& resType, int64_t& value, nlohmann::json& context);
    void Init();
    DISALLOW_COPY_AND_MOVE(ResSchedExeServiceStub);
};
} // namespace ResourceSchedule
} // namespace OHOS

#endif // RESSCHED_EXECUTOR_SERVICES_RESSCHEDEXESERVICE_INCLUDE_RES_SCHED_EXE_SERVICE_STUB_H
