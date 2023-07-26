/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "resschedobserver_fuzzer.h"
#ifdef RESSCHED_TELEPHONY_STATE_REGISTRY_ENABLE
#include "sched_telephony_observer.h"
#endif
#ifdef DEVICE_MOVEMENT_PERCEPTION_ENABLE
#include "device_movement_observer.h"
#endif
#include "res_sched_service.h"

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <securec.h>
#include <string>
#include <vector>

#ifndef errno_t
typedef int errno_t;
#endif

#ifndef EOK
#define EOK 0
#endif

namespace OHOS {
namespace ResourceSchedule {
    const uint8_t* g_data = nullptr;
    size_t g_size = 0;
    size_t g_pos;

/*
* describe: get data from outside untrusted data(g_data) which size is according to sizeof(T)
* tips: only support basic type
*/
    template<class T>
    T GetData()
    {
        T object {};
        size_t objectSize = sizeof(object);
        if (g_data == nullptr || objectSize > g_size - g_pos) {
            return object;
        }
        errno_t ret = memcpy_s(&object, objectSize, g_data + g_pos, objectSize);
        if (ret != EOK) {
            return {};
        }
        g_pos += objectSize;
        return object;
    }

/*
* get a string from g_data
*/
    std::u16string GetU16StringFromData(int strlen)
    {
        if (strlen <= 0) {
            return u"";
        }
        char16_t cstr[strlen];
        cstr[strlen - 1] = u'\0';
        for (int i = 0; i < strlen - 1; i++) {
            char16_t tmp = GetData<char16_t>();
            if (tmp == u'\0') {
                tmp = u'1';
            }
            cstr[i] = tmp;
        }
        std::u16string str(cstr);
        return str;
    }

#ifdef RESSCHED_TELEPHONY_STATE_REGISTRY_ENABLE
    bool OnCallStateUpdatedFuzzTest(const uint8_t* data, size_t size)
    {
        if (data == nullptr) {
            return false;
        }

        if (size <= sizeof(int32_t) + sizeof(int32_t)) {
            return false;
        }

        // initialize
        g_data = data;
        g_size = size;
        g_pos = 0;

        // getdata
        int32_t slotId = GetData<int32_t>();
        int32_t callState = GetData<int32_t>();
        std::u16string phoneNumber = GetU16StringFromData(int(size) - sizeof(int32_t) - sizeof(int32_t));
        auto telephonyObserver = std::make_unique<SchedTelephonyObserver>();
        telephonyObserver->OnCallStateUpdated(slotId, callState, phoneNumber);

        return true;
    }
#endif // RESSCHED_TELEPHONY_STATE_REGISTRY_ENABLE

#ifdef DEVICE_MOVEMENT_PERCEPTION_ENABLE
    bool OnRemoteRequestFuzzTest(const uint8_t* data, size_t size)
    {
        if (data == nullptr) {
            return false;
        }

        if (size <= sizeof(int32_t) + sizeof(int32_t)) {
            return false;
        }

        // initialize
        g_data = data;
        g_size = size;
        g_pos = 0;

        // getdata
        uint32_t code = GetData<uint32_t>();
        MessageParcel fuzzData;

        fuzzData.WriteInterfaceToken(ResSchedServiceStub::GetDescriptor());
        fuzzData.WriteBuffer(g_data + g_pos, g_size - g_pos);
        fuzzData.RewindRead(0);
        MessageParcel reply;
        MessageOption option;
        auto deviceMovementObserver = std::make_unique<DeviceMovementObserver>();
        deviceMovementObserver->OnRemoteRequest(code, fuzzData, reply, option);

        return true;
    }
#endif // DEVICE_MOVEMENT_PERCEPTION_ENABLE
} // namespace ResourceSchedule
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
#ifdef RESSCHED_TELEPHONY_STATE_REGISTRY_ENABLE
    OHOS::ResourceSchedule::OnCallStateUpdatedFuzzTest(data, size);
#endif
#ifdef DEVICE_MOVEMENT_PERCEPTION_ENABLE
    OHOS::ResourceSchedule::OnRemoteRequestFuzzTest(data, size);
#endif
    return 0;
}