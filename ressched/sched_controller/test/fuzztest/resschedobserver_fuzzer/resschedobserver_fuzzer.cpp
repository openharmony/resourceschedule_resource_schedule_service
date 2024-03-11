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

#define private public

#include "resschedobserver_fuzzer.h"
#ifdef RESSCHED_TELEPHONY_STATE_REGISTRY_ENABLE
#include "sched_telephony_observer.h"
#endif
#ifdef DEVICE_MOVEMENT_PERCEPTION_ENABLE
#include "device_movement_observer.h"
#endif
#ifdef RESSCHED_AUDIO_FRAMEWORK_ENABLE
#include "audio_observer.h"
#endif
#ifdef RESSCHED_COMMUNICATION_BLUETOOTH_ENABLE
#include "bluetooth_def.h"
#endif

#include "hisysevent_observer.h"
#include "observer_manager.h"
#include "system_ability_definition.h"
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

namespace {
    static const int32_t TWO_PARAMETERS = 2;
    static const int32_t THREE_PARAMETERS = 3;
    static const int32_t FOUR_PARAMETERS = 4;
    static const int32_t FIVE_PARAMETERS = 5;
    static const int32_t SIX_PARAMETERS = 6;
}
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

/*
* get a string from g_data
*/
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


    bool HisysEventAvCodecEventFuzzTest(const uint8_t* data, size_t size)
    {
        if (data == nullptr) {
            return false;
        }

        if (size <= SIX_PARAMETERS * sizeof(int32_t) + sizeof(std::string)) {
            return false;
        }

        // initialize
        g_data = data;
        g_size = size;
        g_pos = 0;

        nlohmann::json sysEvent;
        sysEvent["UID"] = GetData<int32_t>();
        sysEvent["PID"] = GetData<int32_t>();
        sysEvent["CLIENT_UID"] = GetData<int32_t>();
        sysEvent["CLIENT_PID"] = GetData<int32_t>();
        sysEvent["INSTANCE_ID"] = GetData<int32_t>();
        sysEvent["CODEC_INSTANCE_ID"] = GetData<int32_t>();
        std::string eventName = GetStringFromData(int(size) - SIX_PARAMETERS * sizeof(int32_t));
        sysEvent["name_"] = eventName;
        std::shared_ptr<HiSysEventObserver> hisysEventObserver_ = std::make_shared<HiSysEventObserver>();
        hisysEventObserver_->ProcessAvCodecEvent(sysEvent, eventName);
        return true;
    }

    bool HisysEventRunningLockEventFuzzTest(const uint8_t* data, size_t size)
    {
        if (data == nullptr) {
            return false;
        }

        if (size <= FOUR_PARAMETERS * sizeof(int32_t) + sizeof(std::string)) {
            return false;
        }

        // initialize
        g_data = data;
        g_size = size;
        g_pos = 0;

        nlohmann::json sysEvent;
        sysEvent["UID"] = GetData<int32_t>();
        sysEvent["PID"] = GetData<int32_t>();
        sysEvent["TYPE"] = GetData<int32_t>();
        sysEvent["STATE"] = GetData<int32_t>();
        std::string eventName = GetStringFromData(int(size) - FOUR_PARAMETERS * sizeof(int32_t));
        sysEvent["name_"] = eventName;
        std::shared_ptr<HiSysEventObserver> hisysEventObserver_ = std::make_shared<HiSysEventObserver>();
        hisysEventObserver_->ProcessRunningLockEvent(sysEvent, eventName);
        return true;
    }

    bool HisysEventAudioEventFuzzTest(const uint8_t* data, size_t size)
    {
        if (data == nullptr) {
            return false;
        }

        if (size <= THREE_PARAMETERS * sizeof(int32_t) + sizeof(std::string)) {
            return false;
        }

        // initialize
        g_data = data;
        g_size = size;
        g_pos = 0;

        nlohmann::json sysEvent;
        sysEvent["UID"] = GetData<int32_t>();
        sysEvent["PID"] = GetData<int32_t>();
        sysEvent["STATE"] = GetData<int32_t>();
        std::string eventName = GetStringFromData(int(size) - THREE_PARAMETERS * sizeof(int32_t));
        sysEvent["name_"] = eventName;
        std::shared_ptr<HiSysEventObserver> hisysEventObserver_ = std::make_shared<HiSysEventObserver>();
        hisysEventObserver_->ProcessAudioEvent(sysEvent, eventName);
        return true;
    }

    bool HisysEventBluetoothFuzzTest(const uint8_t* data, size_t size)
    {
        if (data == nullptr) {
            return false;
        }

        if (size <= THREE_PARAMETERS * sizeof(int32_t) + sizeof(std::string)) {
            return false;
        }

        // initialize
        g_data = data;
        g_size = size;
        g_pos = 0;

        nlohmann::json sysEvent;
        sysEvent["UID"] = GetData<int32_t>();
        sysEvent["PID"] = GetData<int32_t>();
        sysEvent["STATE"] = GetData<int32_t>();
        std::string eventName = GetStringFromData(int(size) - THREE_PARAMETERS * sizeof(int32_t));
        sysEvent["name_"] = eventName;
        std::shared_ptr<HiSysEventObserver> hisysEventObserver_ = std::make_shared<HiSysEventObserver>();
        hisysEventObserver_->ProcessBluetoothEvent(sysEvent, eventName);
        return true;
    }

    bool HisysEventCameraEventFuzzTest(const uint8_t* data, size_t size)
    {
        if (data == nullptr) {
            return false;
        }

        if (size <= TWO_PARAMETERS * sizeof(int32_t) + sizeof(std::string)) {
            return false;
        }

        // initialize
        g_data = data;
        g_size = size;
        g_pos = 0;

        nlohmann::json sysEvent;
        sysEvent["UID"] = GetData<int32_t>();
        sysEvent["PID"] = GetData<int32_t>();
        std::string eventName = GetStringFromData(int(size) - TWO_PARAMETERS * sizeof(int32_t));
        sysEvent["name_"] = eventName;
        std::shared_ptr<HiSysEventObserver> hisysEventObserver_ = std::make_shared<HiSysEventObserver>();
        hisysEventObserver_->ProcessCameraEvent(sysEvent, eventName);
        return true;
    }

    bool HisysEventWifiEventFuzzTest(const uint8_t* data, size_t size)
    {
        if (data == nullptr) {
            return false;
        }

        if (size <= THREE_PARAMETERS * sizeof(int32_t) + sizeof(std::string)) {
            return false;
        }

        // initialize
        g_data = data;
        g_size = size;
        g_pos = 0;

        nlohmann::json sysEvent;
        sysEvent["UID"] = GetData<int32_t>();
        sysEvent["PID"] = GetData<int32_t>();
        sysEvent["TYPE"] = GetData<int32_t>();
        std::string eventName = GetStringFromData(int(size) - THREE_PARAMETERS * sizeof(int32_t));
        sysEvent["name_"] = eventName;
        std::shared_ptr<HiSysEventObserver> hisysEventObserver_ = std::make_shared<HiSysEventObserver>();
        hisysEventObserver_->ProcessWifiEvent(sysEvent, eventName);
        return true;
    }

    bool HisysEventScreenCaptureEventFuzzTest(const uint8_t* data, size_t size)
    {
        if (data == nullptr) {
            return false;
        }

        if (size <= FIVE_PARAMETERS * sizeof(int32_t) + sizeof(std::string)) {
            return false;
        }

        // initialize
        g_data = data;
        g_size = size;
        g_pos = 0;

        nlohmann::json sysEvent;
        sysEvent["UID"] = GetData<int32_t>();
        sysEvent["PID"] = GetData<int32_t>();
        sysEvent["STATUS"] = GetData<int32_t>();
        sysEvent["APP_UID"] = GetData<int32_t>();
        sysEvent["APP_PID"] = GetData<int32_t>();
        std::string eventName = GetStringFromData(int(size) - FIVE_PARAMETERS * sizeof(int32_t));
        sysEvent["name_"] = eventName;
        std::shared_ptr<HiSysEventObserver> hisysEventObserver_ = std::make_shared<HiSysEventObserver>();
        hisysEventObserver_->ProcessScreenCaptureEvent(sysEvent, eventName);
        return true;
    }

    bool ProcessHiSysEventFuzzTest(const uint8_t* data, size_t size)
    {
        if (data == nullptr) {
            return false;
        }

        if (size <=  TWO_PARAMETERS * sizeof(std::string)) {
            return false;
        }

        // initialize
        g_data = data;
        g_size = size;
        g_pos = 0;

        nlohmann::json sysEvent;
        sysEvent["domain_"] = GetStringFromData(int(size));
        std::string eventName = GetStringFromData(int(size) - sizeof(std::string));
        sysEvent["name_"] = eventName;
        std::shared_ptr<HiSysEventObserver> hisysEventObserver_ = std::make_shared<HiSysEventObserver>();
        hisysEventObserver_->ProcessHiSysEvent(eventName, sysEvent);
        return true;
    }

    bool ObserverManagerFuzzTest(const uint8_t* data, size_t size)
    {
        if (data == nullptr) {
            return false;
        }

        if (size <=  TWO_PARAMETERS * sizeof(std::string)) {
            return false;
        }

        // initialize
        g_data = data;
        g_size = size;
        g_pos = 0;
        auto instance = ObserverManager::GetInstance();
        if (instance) {
            instance->GetAllMmiStatusData();
        }
        return true;
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

    bool OnReceiveDeviceMovementEventFuzzTest(const uint8_t* data, size_t size)
    {
        if (data == nullptr) {
            return false;
        }

        // initialize
        g_data = data;
        g_size = size;
        g_pos = 0;

        // getdata
        MessageParcel fuzzData;

        fuzzData.WriteInterfaceToken(ResSchedServiceStub::GetDescriptor());
        fuzzData.WriteBuffer(g_data + g_pos, g_size - g_pos);
        fuzzData.RewindRead(0);
        auto deviceMovementObserver = std::make_unique<DeviceMovementObserver>();
        deviceMovementObserver->OnReceiveDeviceMovementEvent(fuzzData);

        return true;
    }
#endif // DEVICE_MOVEMENT_PERCEPTION_ENABLE
} // namespace ResourceSchedule
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    OHOS::ResourceSchedule::HisysEventAvCodecEventFuzzTest(data, size);
    OHOS::ResourceSchedule::HisysEventRunningLockEventFuzzTest(data, size);
    OHOS::ResourceSchedule::HisysEventAudioEventFuzzTest(data, size);
    OHOS::ResourceSchedule::HisysEventCameraEventFuzzTest(data, size);
    OHOS::ResourceSchedule::HisysEventBluetoothFuzzTest(data, size);
    OHOS::ResourceSchedule::HisysEventWifiEventFuzzTest(data, size);
    OHOS::ResourceSchedule::HisysEventScreenCaptureEventFuzzTest(data, size);
    OHOS::ResourceSchedule::ProcessHiSysEventFuzzTest(data, size);
    OHOS::ResourceSchedule::ObserverManagerFuzzTest(data, size);
    /* Run your code on data */
#ifdef RESSCHED_TELEPHONY_STATE_REGISTRY_ENABLE
    OHOS::ResourceSchedule::OnCallStateUpdatedFuzzTest(data, size);
#endif
#ifdef DEVICE_MOVEMENT_PERCEPTION_ENABLE
    OHOS::ResourceSchedule::OnRemoteRequestFuzzTest(data, size);
    OHOS::ResourceSchedule::OnReceiveDeviceMovementEventFuzzTest(data, size);
#endif
    return 0;
}