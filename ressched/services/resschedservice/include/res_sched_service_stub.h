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

#ifndef RESSCHED_SERVICES_RESSCHEDSERVICE_INCLUDE_RES_SCHED_SERVICE_STUB_H
#define RESSCHED_SERVICES_RESSCHEDSERVICE_INCLUDE_RES_SCHED_SERVICE_STUB_H

#include <map>
#include "iremote_stub.h"
#include "ires_sched_service.h"

namespace OHOS {
namespace ResourceSchedule {
class ResSchedServiceStub : public IRemoteStub<IResSchedService> {
public:
    ResSchedServiceStub();
    ~ResSchedServiceStub();
    DISALLOW_COPY_AND_MOVE(ResSchedServiceStub);
    int32_t OnRemoteRequest(
        uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;

private:
    int32_t ReportDataInner(MessageParcel& data, MessageParcel& reply);
    nlohmann::json StringToJsonObj(const std::string& str);

    void Init();

    using RequestFuncType = std::function<int32_t (MessageParcel& data, MessageParcel& reply)>;
    std::map<uint32_t, RequestFuncType> funcMap_;
};
} // namespace ResourceSchedule
} // namespace OHOS

#endif // RESSCHED_SERVICES_RESSCHEDSERVICE_INCLUDE_RES_SCHED_SERVICE_STUB_H
