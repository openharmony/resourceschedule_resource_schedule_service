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

#include "ressched_fuzzer.h"

#include "iservice_registry.h"
#include "system_ability_definition.h"

#include "res_sched_mgr.h"
#include "res_sched_service.h"
#include "res_sched_service_stub.h"

namespace OHOS {
namespace ResourceSchedule {
    constexpr int32_t MIN_LEN = 4;
    std::mutex mutexLock;
    sptr<IRemoteObject> remoteObj;

    bool DoInit()
    {
        std::lock_guard<std::mutex> lock(mutexLock);
        if (remoteObj) {
            return true;
        }
        auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        if (!samgr) {
            return false;
        }
        remoteObj = samgr->GetSystemAbility(RES_SCHED_SYS_ABILITY_ID);
        if (!remoteObj) {
            return false;
        }
        return true;
    }

    int32_t onRemoteRequest(uint32_t code, MessageParcel& data)
    {
        if (!DoInit()) {
            return -1;
        }
        MessageParcel reply;
        MessageOption option;
        return remoteObj->SendRequest(code, data, reply, option);
    }

    bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
    {
        if (size <= MIN_LEN) {
            return false;
        }

        MessageParcel dataMessageParcel;
        if (!dataMessageParcel.WriteInterfaceToken(ResSchedServiceStub::GetDescriptor())) {
            return false;
        }

        uint32_t code = *(reinterpret_cast<const uint32_t*>(data));
        size -= sizeof(uint32_t);

        dataMessageParcel.WriteBuffer(data + sizeof(uint32_t), size);
        dataMessageParcel.RewindRead(0);

        onRemoteRequest(code, dataMessageParcel);
        return true;
    }
} // namespace ResourceSchedule
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::ResourceSchedule::DoSomethingInterestingWithMyAPI(data, size);
    return 0;
}

