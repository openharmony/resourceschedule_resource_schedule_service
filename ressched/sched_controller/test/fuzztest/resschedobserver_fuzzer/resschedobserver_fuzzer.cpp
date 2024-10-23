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
#ifndef RESOURCE_REQUEST_REQUEST
#include "download_upload_observer.h"
#endif

#include "hisysevent_observer.h"
#include "observer_manager.h"
#include "system_ability_definition.h"
#include "res_sched_service.h"
#include "mmi_observer.h"
#include "connection_subscriber.h"
#include "fold_display_mode_observer.h"
#include "av_session_state_listener.h"

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
    static const int32_t ENUM_MAX = 4;
    static const int32_t ENUM_MIN = 0;
    static const int32_t DATA_LENGTH = 10;
    const int32_t INDENT = -1;
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

        if (size <= SIX_PARAMETERS * sizeof(int32_t) + DATA_LENGTH) {
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
        std::string eventName = GetStringFromData(DATA_LENGTH);
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

        if (size <= FOUR_PARAMETERS * sizeof(int32_t) + DATA_LENGTH) {
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
        std::string eventName = GetStringFromData(DATA_LENGTH);
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

        if (size <= THREE_PARAMETERS * sizeof(int32_t) + DATA_LENGTH) {
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
        std::string eventName = GetStringFromData(DATA_LENGTH);
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

        if (size <= THREE_PARAMETERS * sizeof(int32_t) + DATA_LENGTH) {
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
        std::string eventName = GetStringFromData(DATA_LENGTH);
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

        if (size <= TWO_PARAMETERS * sizeof(int32_t) + DATA_LENGTH) {
            return false;
        }

        // initialize
        g_data = data;
        g_size = size;
        g_pos = 0;

        nlohmann::json sysEvent;
        sysEvent["UID"] = GetData<int32_t>();
        sysEvent["PID"] = GetData<int32_t>();
        std::string eventName = GetStringFromData(DATA_LENGTH);
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

        if (size <= THREE_PARAMETERS * sizeof(int32_t) + DATA_LENGTH) {
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
        std::string eventName = GetStringFromData(DATA_LENGTH);
        sysEvent["name_"] = eventName;
        std::shared_ptr<HiSysEventObserver> hisysEventObserver_ = std::make_shared<HiSysEventObserver>();
        hisysEventObserver_->ProcessWifiEvent(sysEvent, eventName);
        return true;
    }

    bool ProcessHiSysEventFuzzTest(const uint8_t* data, size_t size)
    {
        if (data == nullptr) {
            return false;
        }

        if (size <=  TWO_PARAMETERS * DATA_LENGTH) {
            return false;
        }

        // initialize
        g_data = data;
        g_size = size;
        g_pos = 0;

        nlohmann::json sysEvent;
        sysEvent["domain_"] = GetStringFromData(DATA_LENGTH);
        std::string eventName = GetStringFromData(DATA_LENGTH);
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

        if (size <= DATA_LENGTH) {
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

    bool HisysEventOnEventFuzzTest(const uint8_t* data, size_t size)
    {
        if (data == nullptr) {
            return false;
        }

        if (size <=  TWO_PARAMETERS * (sizeof(int32_t) + DATA_LENGTH)) {
            return false;
        }

        // initialize
        g_data = data;
        g_size = size;
        g_pos = 0;

        nlohmann::json sysEvent;
        sysEvent["UID"] = GetData<int32_t>();
        sysEvent["PID"] = GetData<int32_t>();
        sysEvent["domain_"] = GetStringFromData(DATA_LENGTH);
        std::string eventName = GetStringFromData(DATA_LENGTH);
        sysEvent["name_"] = eventName;
        std::string tmp = sysEvent.dump(INDENT, ' ', false, nlohmann::json::error_handler_t::replace);

        std::shared_ptr<HiSysEventObserver> hisysEventObserver_ = std::make_shared<HiSysEventObserver>();
        hisysEventObserver_->OnEvent(std::make_shared<HiviewDFX::HiSysEventRecord>(tmp));
        return true;
    }

    bool HiSysEventOnServiceDiedFuzzTest(const uint8_t* data, size_t size)
    {
        if (data == nullptr) {
            return false;
        }
        // initialize
        g_data = data;
        g_size = size;
        g_pos = 0;
        std::shared_ptr<HiSysEventObserver> hisysEventObserver_ = std::make_shared<HiSysEventObserver>();
        hisysEventObserver_->OnServiceDied();
        return true;
    }

    bool MmiObserverSyncBundleNameFuzzTest(const uint8_t* data, size_t size)
    {
        if (data == nullptr) {
            return false;
        }

        if (size <=  THREE_PARAMETERS * sizeof(int32_t) + DATA_LENGTH) {
            return false;
        }

        // initialize
        g_data = data;
        g_size = size;
        g_pos = 0;

        int32_t pid = GetData<int32_t>();
        int32_t uid = GetData<int32_t>();
        int32_t syncStatus = GetData<int32_t>();

        std::string bundleName = GetStringFromData(DATA_LENGTH);
        auto mmiObserver = std::make_shared<MmiObserver>();
        mmiObserver->SyncBundleName(pid, uid, bundleName, syncStatus);
        return true;
    }

    bool ConnectionSubscriberExtensionFuzzTest(const uint8_t* data, size_t size)
    {
        if (data == nullptr) {
            return false;
        }

        if (size <= FIVE_PARAMETERS * sizeof(int32_t) + FOUR_PARAMETERS * DATA_LENGTH) {
            return false;
        }

        // initialize
        g_data = data;
        g_size = size;
        g_pos = 0;

        AbilityRuntime::ConnectionData connectionData;
        connectionData.extensionPid = GetData<int32_t>();
        connectionData.extensionUid = GetData<int32_t>();
        connectionData.callerPid = GetData<int32_t>();
        connectionData.callerUid = GetData<int32_t>();
        connectionData.extensionType = AppExecFwk::ExtensionAbilityType(GetData<int32_t>());
        connectionData.extensionBundleName = GetStringFromData(DATA_LENGTH);
        connectionData.extensionModuleName = GetStringFromData(DATA_LENGTH);
        connectionData.extensionName = GetStringFromData(DATA_LENGTH);
        connectionData.callerName = GetStringFromData(DATA_LENGTH);

        auto connectionSubscriber = std::make_unique<ConnectionSubscriber>();
        connectionSubscriber->OnExtensionConnected(connectionData);
        connectionSubscriber->OnExtensionDisconnected(connectionData);
        return true;
    }

    bool ConnectionSubscriberDlpAbilityFuzzTest(const uint8_t* data, size_t size)
    {
        if (data == nullptr) {
            return false;
        }

        if (size <= THREE_PARAMETERS * sizeof(int32_t) + FOUR_PARAMETERS * DATA_LENGTH) {
            return false;
        }

        // initialize
        g_data = data;
        g_size = size;
        g_pos = 0;

        AbilityRuntime::DlpStateData targetConnectionData;
        targetConnectionData.targetPid = GetData<int32_t>();
        targetConnectionData.callerPid = GetData<int32_t>();
        targetConnectionData.callerUid = GetData<int32_t>();
        targetConnectionData.callerName = GetStringFromData(DATA_LENGTH);
        targetConnectionData.targetBundleName = GetStringFromData(DATA_LENGTH);
        targetConnectionData.targetModuleName = GetStringFromData(DATA_LENGTH);
        targetConnectionData.targetAbilityName = GetStringFromData(DATA_LENGTH);

        auto connectionSubscriber = std::make_unique<ConnectionSubscriber>();
        connectionSubscriber->OnDlpAbilityOpened(targetConnectionData);
        connectionSubscriber->OnDlpAbilityClosed(targetConnectionData);
        connectionSubscriber->OnServiceDied();
        return true;
    }

    bool FoldDisplayModeObserverFuzzTest(const uint8_t* data, size_t size)
    {
        if (data == nullptr) {
            return false;
        }

        if (size <= sizeof(uint32_t)) {
            return false;
        }

        // initialize
        g_data = data;
        g_size = size;
        g_pos = 0;

        FoldDisplayMode diplayMode = static_cast<FoldDisplayMode>(GetData<uint32_t>() % (ENUM_MAX - ENUM_MIN + 1));
        auto foldDisplayModeObserver = std::make_unique<FoldDisplayModeObserver>();
        foldDisplayModeObserver->OnDisplayModeChanged(diplayMode);
        return true;
    }

    bool AvSessionStateListenerFuzzTest(const uint8_t* data, size_t size)
    {
        if (data == nullptr) {
            return false;
        }

        if (size <= TWO_PARAMETERS * sizeof(pid_t) + DATA_LENGTH) {
            return false;
        }

        // initialize
        g_data = data;
        g_size = size;
        g_pos = 0;

        AVSession::AVSessionDescriptor descriptor;
        descriptor.sessionId_ = GetStringFromData(DATA_LENGTH);
        descriptor.pid_ = GetData<pid_t>();
        descriptor.uid_ = GetData<pid_t>();
        
        auto avSessionStateListener = std::make_unique<AvSessionStateListener>();
        avSessionStateListener->OnSessionCreate(descriptor);
        avSessionStateListener->OnSessionRelease(descriptor);
        avSessionStateListener->OnTopSessionChange(descriptor);
        return true;
    }

#ifdef RESSCHED_AUDIO_FRAMEWORK_ENABLE
    bool AudioObserverStateChangeFuzzTest(const uint8_t* data, size_t size)
    {
        if (data == nullptr) {
            return false;
        }

        if (size <= FIVE_PARAMETERS * sizeof(int32_t)) {
            return false;
        }

        // initialize
        g_data = data;
        g_size = size;
        g_pos = 0;

        nlohmann::json payload;
        std::shared_ptr<AudioStandard::AudioRendererChangeInfo> audioRendererChangeInfo =
            std::make_shared<AudioStandard::AudioRendererChangeInfo>();
        std::vector<std::shared_ptr<AudioStandard::AudioRendererChangeInfo>> audioRendererChangeInfos;

        audioRendererChangeInfo->clientUID = GetData<int32_t>();
        audioRendererChangeInfo->sessionId = GetData<int32_t>();
        audioRendererChangeInfo->rendererState = AudioStandard::RendererState(GetData<int32_t>());
        audioRendererChangeInfo->rendererInfo.contentType = AudioStandard::ContentType(GetData<int32_t>());
        audioRendererChangeInfo->rendererInfo.streamUsage = AudioStandard::StreamUsage(GetData<int32_t>());
        auto audioObserver = std::make_unique<AudioObserver>();
        audioObserver->MarshallingAudioRendererChangeInfo(audioRendererChangeInfo, payload);
        audioRendererChangeInfos.push_back(move(audioRendererChangeInfo));
        audioObserver->OnRendererStateChange(audioRendererChangeInfos);
        return true;
    }
    
    bool AudioObserverModeUpdatedFuzzTest(const uint8_t* data, size_t size)
    {
        if (data == nullptr) {
            return false;
        }

        if (size <= sizeof(int32_t)) {
            return false;
        }

        // initialize
        g_data = data;
        g_size = size;
        g_pos = 0;

        AudioStandard::AudioRingerMode ringerMode = AudioStandard::AudioRingerMode(GetData<int32_t>());
        auto audioObserver = std::make_unique<AudioObserver>();
        audioObserver->OnRingerModeUpdated(ringerMode);
        return true;
    }

    bool AudioObserverEventFuzzTest(const uint8_t* data, size_t size)
    {
        if (data == nullptr) {
            return false;
        }

        if (size <= THREE_PARAMETERS * sizeof(int32_t) + sizeof(bool)) {
            return false;
        }

        // initialize
        g_data = data;
        g_size = size;
        g_pos = 0;

        AudioStandard::VolumeEvent volumeEvent;
        volumeEvent.volumeType = AudioStandard::AudioVolumeType(GetData<int32_t>());
        volumeEvent.volume = GetData<int32_t>();
        volumeEvent.updateUi = GetData<bool>();
        volumeEvent.volumeGroupId = GetData<int32_t>();

        auto audioObserver = std::make_unique<AudioObserver>();
        audioObserver->OnVolumeKeyEvent(volumeEvent);
        return true;
    }
#endif

#ifdef RESSCHED_TELEPHONY_STATE_REGISTRY_ENABLE
    bool OnCallStateUpdatedFuzzTest(const uint8_t* data, size_t size)
    {
        if (data == nullptr) {
            return false;
        }

        if (size <= TWO_PARAMETERS * sizeof(int32_t) + DATA_LENGTH) {
            return false;
        }

        // initialize
        g_data = data;
        g_size = size;
        g_pos = 0;

        // getdata
        int32_t slotId = GetData<int32_t>();
        int32_t callState = GetData<int32_t>();
        std::u16string phoneNumber = GetU16StringFromData(DATA_LENGTH);
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

        if (size <= sizeof(uint32_t)) {
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

#ifndef RESOURCE_REQUEST_REQUEST
    bool DownLoadUploadObserverFuzzTest(const uint8_t* data, size_t size)
    {
        if (data == nullptr) {
            return false;
        }

        if (size <= sizeof(int)) {
            return false;
        }
        // initialize
        g_data = data;
        g_size = size;
        g_pos = 0;

        int count = GetData<int>();
        auto downLoadUploadObserver = std::make_shared<DownLoadUploadObserver>();
        downLoadUploadObserver->OnRunningTaskCountUpdate(count);
        return true;
    }
#endif
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
    OHOS::ResourceSchedule::ProcessHiSysEventFuzzTest(data, size);
    OHOS::ResourceSchedule::ObserverManagerFuzzTest(data, size);
    OHOS::ResourceSchedule::MmiObserverSyncBundleNameFuzzTest(data, size);
    OHOS::ResourceSchedule::ConnectionSubscriberExtensionFuzzTest(data, size);
    OHOS::ResourceSchedule::ConnectionSubscriberDlpAbilityFuzzTest(data, size);
    OHOS::ResourceSchedule::HisysEventOnEventFuzzTest(data, size);
    OHOS::ResourceSchedule::HiSysEventOnServiceDiedFuzzTest(data, size);
    OHOS::ResourceSchedule::FoldDisplayModeObserverFuzzTest(data, size);
    OHOS::ResourceSchedule::AvSessionStateListenerFuzzTest(data, size);

#ifdef RESSCHED_AUDIO_FRAMEWORK_ENABLE
    OHOS::ResourceSchedule::AudioObserverStateChangeFuzzTest(data, size);
    OHOS::ResourceSchedule::AudioObserverEventFuzzTest(data, size);
    OHOS::ResourceSchedule::AudioObserverModeUpdatedFuzzTest(data, size);
#endif
    /* Run your code on data */
#ifdef RESSCHED_TELEPHONY_STATE_REGISTRY_ENABLE
    OHOS::ResourceSchedule::OnCallStateUpdatedFuzzTest(data, size);
#endif
#ifdef DEVICE_MOVEMENT_PERCEPTION_ENABLE
    OHOS::ResourceSchedule::OnRemoteRequestFuzzTest(data, size);
    OHOS::ResourceSchedule::OnReceiveDeviceMovementEventFuzzTest(data, size);
#endif
#ifndef RESOURCE_REQUEST_REQUEST
    OHOS::ResourceSchedule::DownLoadUploadObserverFuzzTest(data, size);
#endif
    return 0;
}