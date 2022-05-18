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

#include "socperf_fuzzer.h"

#define private public
#include "socperf_server.h"
#include "socperf_stub.h"

namespace OHOS {
namespace SOCPERF {
    constexpr int32_t MIN_LEN = 4;
    static bool isInited = false;

    int32_t onRemoteRequest(uint32_t code, MessageParcel& data)
    {
        int32_t ret = -1;
        MessageParcel reply;
        MessageOption option;
        if (!isInited) {
            if (!DelayedSingleton<SocPerfServer>::GetInstance()->socPerf.Init()) {
                return ret;
            }
            isInited = true;
        }
        ret = DelayedSingleton<SocPerfServer>::GetInstance()->OnRemoteRequest(code, data, reply, option);
        return ret;
    }

    void DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
    {
        if (size <= MIN_LEN) {
            return;
        }

        MessageParcel dataMessageParcel;
        if (!dataMessageParcel.WriteInterfaceToken(SocPerfStub::GetDescriptor())) {
            return;
        }

        uint32_t code = *(reinterpret_cast<const uint32_t*>(data));
        size -= sizeof(uint32_t);

        dataMessageParcel.WriteBuffer(data + sizeof(uint32_t), size);
        dataMessageParcel.RewindRead(0);

        onRemoteRequest(code, dataMessageParcel);
    }
} // namespace SOCPERF
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::SOCPERF::DoSomethingInterestingWithMyAPI(data, size);
    return 0;
}

