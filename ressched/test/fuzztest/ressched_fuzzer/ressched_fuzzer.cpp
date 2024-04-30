/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <securec.h>
#include <vector>

#include "iremote_stub.h"
#include "ires_sched_service.h"
#include "iservice_registry.h"
#include "plugin_mgr.h"
#include "res_sched_service.h"
#include "singleton.h"
#include "system_ability_definition.h"
#include "res_sched_client.h"
#include "res_sched_service_stub.h"
#include "res_sched_systemload_notifier_client.h"

#define private public
#define protected public

#ifndef errno_t
typedef int errno_t;
#endif

#ifndef EOK
#define EOK 0
#endif

namespace OHOS {
namespace ResourceSchedule {
namespace {
    static const int32_t TWO_PARAMETERS = 2;
}
    constexpr int32_t MAX_CODE = 5;
    constexpr int32_t MIN_LEN = 4;
    std::mutex mutexLock;
    sptr<IRemoteObject> remoteObj;
    const uint8_t* g_data = nullptr;
    size_t g_size = 0;
    size_t g_pos;

    /**
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

    std::string GetStringFromData(int strlen)
    {
        if (strlen <= 0) {
            return "";
        }
        char cstr[strlen];
        cstr[strlen - 1] = '\0';
        for (int i = 0; i < strlen - 1; i++) {
            char tmp = GetData<char>();
            if (tmp == '\0') {
                tmp = '1';
            }
            cstr[i] = tmp;
        }
        std::string str(cstr);
        return str;
    }

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
        if (!dataMessageParcel.WriteInterfaceToken(IRemoteStub<IResSchedService>::GetDescriptor())) {
            return false;
        }

        uint32_t code = *(reinterpret_cast<const uint32_t*>(data));
        size -= sizeof(uint32_t);

        dataMessageParcel.WriteBuffer(data + sizeof(uint32_t), size);
        dataMessageParcel.RewindRead(0);

        onRemoteRequest(code, dataMessageParcel);
        return true;
    }

    bool OnRemoteRequest(const uint8_t* data, size_t size)
    {
        if (data == nullptr) {
            return false;
        }

        if (size <= MIN_LEN) {
            return false;
        }

        // initialize
        g_data = data;
        g_size = size;
        g_pos = 0;

        // getdata
        uint32_t fuzzCode = GetData<uint32_t>();
        MessageParcel fuzzData;
        fuzzData.WriteInterfaceToken(ResSchedServiceStub::GetDescriptor());
        fuzzData.WriteBuffer(g_data + g_pos, g_size - g_pos);
        fuzzData.RewindRead(0);
        MessageParcel fuzzReply;
        MessageOption fuzzOption;
        DelayedSingleton<ResSchedService>::GetInstance()->OnRemoteRequest(fuzzCode % MAX_CODE,
            fuzzData, fuzzReply, fuzzOption);
        return true;
    }

    bool ResSchedClientFuzzTest(const uint8_t* data, size_t size)
    {
        if (data == nullptr) {
            return false;
        }

        if (size <= sizeof(uint32_t) + sizeof(int64_t) + TWO_PARAMETERS * sizeof(std::string)) {
            return false;
        }

        // initialize
        g_data = data;
        g_size = size;
        g_pos = 0;

        uint32_t resType = GetData<uint32_t>();
        int64_t value = GetData<int64_t>();
        int32_t preloadMode = GetData<int32_t>();
        std::unordered_map<std::string, std::string> mapPayload;
        mapPayload["pid"] = GetStringFromData(int(size) - sizeof(uint32_t) - sizeof(int64_t));
        mapPayload["processName"] = GetStringFromData(int(size) - sizeof(std::string) -
        sizeof(uint32_t) - sizeof(int64_t));
        std::string bundleName = GetStringFromData(int(size) - TWO_PARAMETERS * sizeof(std::string) -
        sizeof(uint32_t) - sizeof(int64_t));

        ResSchedClient::GetInstance().ReportData(resType, value, mapPayload);
        ResSchedClient::GetInstance().KillProcess(mapPayload);
        ResSchedClient::GetInstance().StopRemoteObject();

        sptr<ResSchedSystemloadNotifierClient> callbackObj;
        ResSchedClient::GetInstance().RegisterSystemloadNotifier(callbackObj);
        ResSchedClient::GetInstance().UnRegisterSystemloadNotifier(callbackObj);
        ResSchedClient::GetInstance().GetSystemloadLevel();
        ResSchedClient::GetInstance().IsAllowedAppPreload(bundleName, preloadMode);
        return true;
    }
} // namespace ResourceSchedule
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::ResourceSchedule::DoSomethingInterestingWithMyAPI(data, size);
    OHOS::ResourceSchedule::OnRemoteRequest(data, size);
    OHOS::ResourceSchedule::ResSchedClientFuzzTest(data, size);
    return 0;
}
