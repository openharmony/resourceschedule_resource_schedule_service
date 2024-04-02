/*
 * Copyright (c) 2023-2023 Huawei Device Co., Ltd.
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

#include <gtest/gtest.h>

#define private public
#include "hisysevent_observer.h"
#include "mmi_observer.h"
#include "fold_display_mode_observer.h"
#include "device_movement_observer.h"
#include "sched_telephony_observer.h"
#include "audio_observer.h"
#include "connection_subscriber.h"
#include "if_system_ability_manager.h"
#include "system_ability_definition.h"
#include "iservice_registry.h"
#include "observer_manager.h"
#include "download_upload_observer.h"

namespace OHOS {
namespace ResourceSchedule {
namespace {
    static const int32_t TEST_UID = 0;
    static const int32_t TEST_PID = 1000;
    static const int32_t TEST_INSTANCE_ID = 123456;
    static const int32_t JSON_FORMAT = 4;
}
class ObserverEventTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() {}
    void TearDown() {}
    static std::shared_ptr<HiSysEventObserver> hisysEventObserver_;
    static std::shared_ptr<MmiObserver> mmiObserver_;
    static std::shared_ptr<ConnectionSubscriber> connectionSubscriber_;
#ifdef DEVICE_MOVEMENT_PERCEPTION_ENABLE
    static std::shared_ptr<DeviceMovementObserver> deviceMovementObserver_;
#endif
#ifdef RESSCHED_TELEPHONY_STATE_REGISTRY_ENABLE
    static std::shared_ptr<SchedTelephonyObserver> schedTelephonyObserver_;
#endif
#ifdef RESSCHED_AUDIO_FRAMEWORK_ENABLE
    static std::shared_ptr<AudioObserver> audioObserver_;
#endif
    static std::shared_ptr<FoldDisplayModeObserver> foldDisplayModeObserver_;
};

std::shared_ptr<HiSysEventObserver> ObserverEventTest::hisysEventObserver_ = nullptr;
std::shared_ptr<MmiObserver> ObserverEventTest::mmiObserver_ = nullptr;
std::shared_ptr<FoldDisplayModeObserver> ObserverEventTest::foldDisplayModeObserver_ = nullptr;
std::shared_ptr<ConnectionSubscriber> ObserverEventTest::connectionSubscriber_ = nullptr;
#ifdef DEVICE_MOVEMENT_PERCEPTION_ENABLE
    std::shared_ptr<DeviceMovementObserver> ObserverEventTest::deviceMovementObserver_ = nullptr;
#endif
#ifdef RESSCHED_TELEPHONY_STATE_REGISTRY_ENABLE
    std::shared_ptr<SchedTelephonyObserver> ObserverEventTest::schedTelephonyObserver_ = nullptr;
#endif
#ifdef RESSCHED_AUDIO_FRAMEWORK_ENABLE
    std::shared_ptr<AudioObserver> ObserverEventTest::audioObserver_ = nullptr;
#endif

void ObserverEventTest::SetUpTestCase()
{
    hisysEventObserver_ = std::make_shared<HiSysEventObserver>();
    mmiObserver_ = std::make_shared<MmiObserver>();
    connectionSubscriber_ = std::make_shared<ConnectionSubscriber>();
    foldDisplayModeObserver_ = std::make_shared<FoldDisplayModeObserver>();
#ifdef DEVICE_MOVEMENT_PERCEPTION_ENABLE
    deviceMovementObserver_ = std::make_shared<DeviceMovementObserver>();
#endif
#ifdef RESSCHED_TELEPHONY_STATE_REGISTRY_ENABLE
    schedTelephonyObserver_ = std::make_shared<SchedTelephonyObserver>();
#endif
#ifdef RESSCHED_AUDIO_FRAMEWORK_ENABLE
    audioObserver_ = std::make_shared<AudioObserver>();
#endif
}

void ObserverEventTest::TearDownTestCase()
{
    hisysEventObserver_ = nullptr;
    mmiObserver_ = nullptr;
    connectionSubscriber_ = nullptr;
    foldDisplayModeObserver_ = nullptr;
#ifdef DEVICE_MOVEMENT_PERCEPTION_ENABLE
    deviceMovementObserver_ = nullptr;
#endif
#ifdef RESSCHED_TELEPHONY_STATE_REGISTRY_ENABLE
    schedTelephonyObserver_ = nullptr;
#endif
#ifdef RESSCHED_AUDIO_FRAMEWORK_ENABLE
    audioObserver_ = nullptr;
#endif
}

/**
 * @tc.name: hisysEventAvCodecEvent_001
 * @tc.desc: test multimedia encoding and decoding event processing
 * @tc.type: FUNC
 * @tc.require: issueI8IDAO
 */
HWTEST_F(ObserverEventTest, hisysEventAvCodecEvent_001, testing::ext::TestSize.Level1)
{
    nlohmann::json sysEvent;
    std::string eventName = "INVAILD";
    sysEvent["name_"] = eventName;

    // incorrect uid keyword
    sysEvent["UID"] = TEST_UID;
    hisysEventObserver_->ProcessAvCodecEvent(sysEvent, eventName);
    EXPECT_NE(hisysEventObserver_, nullptr);

    // incorrect pid keyword
    sysEvent["CLIENT_UID"] = TEST_UID;
    sysEvent["PID"] = TEST_PID;
    hisysEventObserver_->ProcessAvCodecEvent(sysEvent, eventName);
    EXPECT_NE(hisysEventObserver_, nullptr);

    // incorrect instance id keyword
    sysEvent["CLIENT_UID"] = TEST_UID;
    sysEvent["CLIENT_PID"] = TEST_PID;
    sysEvent["INSTANCE_ID"] = TEST_INSTANCE_ID;
    hisysEventObserver_->ProcessAvCodecEvent(sysEvent, eventName);
    EXPECT_NE(hisysEventObserver_, nullptr);

    // codec start info state scene
    sysEvent["CODEC_INSTANCE_ID"] = TEST_INSTANCE_ID;
    eventName = "CODEC_START_INFO";
    sysEvent["name_"] = eventName;
    hisysEventObserver_->ProcessAvCodecEvent(sysEvent, eventName);
    EXPECT_NE(hisysEventObserver_, nullptr);

    // codec stop info state scene
    eventName = "CODEC_STOP_INFO";
    sysEvent["name_"] = eventName;
    hisysEventObserver_->ProcessAvCodecEvent(sysEvent, eventName);
    EXPECT_NE(hisysEventObserver_, nullptr);

    // codec fault state scene
    eventName = "FAULT";
    sysEvent["name_"] = eventName;
    hisysEventObserver_->ProcessAvCodecEvent(sysEvent, eventName);
    EXPECT_NE(hisysEventObserver_, nullptr);
}

/**
 * @tc.name: hisysEventRunningLockEvent_001
 * @tc.desc: test running lock event processing
 * @tc.type: FUNC
 * @tc.require: issueI8IDAO
 */
HWTEST_F(ObserverEventTest, hisysEventRunningLockEvent_001, testing::ext::TestSize.Level1)
{
    nlohmann::json sysEvent;
    std::string eventName = "INVAILD";
    // incorrect uid keyword
    sysEvent["uid"] = TEST_UID;
    hisysEventObserver_->ProcessRunningLockEvent(sysEvent, eventName);
    EXPECT_NE(hisysEventObserver_, nullptr);

    // incorrect pid keyword
    sysEvent["UID"] = TEST_UID;
    sysEvent["pid"] = TEST_PID;
    hisysEventObserver_->ProcessRunningLockEvent(sysEvent, eventName);
    EXPECT_NE(hisysEventObserver_, nullptr);

    // incorrect type keyword
    sysEvent["PID"] = TEST_PID;
    sysEvent["type"] = 0;
    hisysEventObserver_->ProcessRunningLockEvent(sysEvent, eventName);
    EXPECT_NE(hisysEventObserver_, nullptr);

    // incorrect state keyword
    sysEvent["TYPE"] = 0;
    sysEvent["state"] = 0;
    hisysEventObserver_->ProcessRunningLockEvent(sysEvent, eventName);
    EXPECT_NE(hisysEventObserver_, nullptr);

    // running lock state disable
    sysEvent["STATE"] = RunningLockState::RUNNINGLOCK_STATE_DISABLE;
    hisysEventObserver_->ProcessRunningLockEvent(sysEvent, eventName);
    EXPECT_NE(hisysEventObserver_, nullptr);

    // running lock state enable
    sysEvent["STATE"] = RunningLockState::RUNNINGLOCK_STATE_ENABLE;
    hisysEventObserver_->ProcessRunningLockEvent(sysEvent, eventName);
    EXPECT_NE(hisysEventObserver_, nullptr);

    // running lock state proxied
    sysEvent["STATE"] = RunningLockState::RUNNINGLOCK_STATE_PROXIED;
    hisysEventObserver_->ProcessRunningLockEvent(sysEvent, eventName);
    EXPECT_NE(hisysEventObserver_, nullptr);
}

/**
 * @tc.name: hisysEventAudioEvent_001
 * @tc.desc: test hisysevent audio event processing
 * @tc.type: FUNC
 * @tc.require: issueI8IDAO
 */
HWTEST_F(ObserverEventTest, hisysEventAudioEvent_001, testing::ext::TestSize.Level1)
{
    nlohmann::json sysEvent;
    std::string eventName = "INVAILD";
    // incorrect uid keyword
    sysEvent["uid"] = TEST_UID;
    hisysEventObserver_->ProcessAudioEvent(sysEvent, eventName);
    EXPECT_NE(hisysEventObserver_, nullptr);

    // incorrect pid keyword
    sysEvent["UID"] = TEST_UID;
    sysEvent["pid"] = TEST_PID;
    hisysEventObserver_->ProcessAudioEvent(sysEvent, eventName);
    EXPECT_NE(hisysEventObserver_, nullptr);

    // incorrect state keyword
    sysEvent["PID"] = TEST_PID;
    sysEvent["state"] = 0;
    hisysEventObserver_->ProcessAudioEvent(sysEvent, eventName);
    EXPECT_NE(hisysEventObserver_, nullptr);

    // audio state running
    sysEvent["STATE"] = AudioState::AUDIO_STATE_RUNNING;
    hisysEventObserver_->ProcessAudioEvent(sysEvent, eventName);
    EXPECT_NE(hisysEventObserver_, nullptr);

    // audio state stopped
    sysEvent["STATE"] = AudioState::AUDIO_STATE_STOPPED;
    hisysEventObserver_->ProcessAudioEvent(sysEvent, eventName);
    EXPECT_NE(hisysEventObserver_, nullptr);
}

/**
 * @tc.name: hisysEventCameraEvent_001
 * @tc.desc: test hisysevent camera event processing
 * @tc.type: FUNC
 * @tc.require: issueI8IDAO
 */
HWTEST_F(ObserverEventTest, hisysEventCameraEvent_001, testing::ext::TestSize.Level1)
{
    nlohmann::json sysEvent;
    std::string eventName = "INVAILD";
    // incorrect uid keyword
    sysEvent["uid"] = TEST_UID;
    hisysEventObserver_->ProcessCameraEvent(sysEvent, eventName);
    EXPECT_NE(hisysEventObserver_, nullptr);

    // incorrect pid keyword
    sysEvent["UID"] = TEST_UID;
    sysEvent["pid"] = TEST_PID;
    hisysEventObserver_->ProcessCameraEvent(sysEvent, eventName);
    EXPECT_NE(hisysEventObserver_, nullptr);

    // eventName test
    sysEvent["PID"] = TEST_PID;
    eventName = "CAMERA_CONNECT";
    hisysEventObserver_->ProcessCameraEvent(sysEvent, eventName);
    EXPECT_NE(hisysEventObserver_, nullptr);

    eventName = "CAMERA_DISCONNECT";
    hisysEventObserver_->ProcessCameraEvent(sysEvent, eventName);
    EXPECT_NE(hisysEventObserver_, nullptr);
}

/**
 * @tc.name: hisysEventBluetoothEvent_001
 * @tc.desc: test hisysevent bluetooth event processing
 * @tc.type: FUNC
 * @tc.require: issueI8IDAO
 */
HWTEST_F(ObserverEventTest, hisysEventBluetoothEvent_001, testing::ext::TestSize.Level1)
{
    nlohmann::json sysEvent;
    std::string eventName = "INVAILD";
    // incorrect uid keyword
    sysEvent["uid"] = TEST_UID;
    hisysEventObserver_->ProcessBluetoothEvent(sysEvent, eventName);
    EXPECT_NE(hisysEventObserver_, nullptr);

    // incorrect pid keyword
    sysEvent["UID"] = TEST_UID;
    sysEvent["pid"] = TEST_PID;
    hisysEventObserver_->ProcessBluetoothEvent(sysEvent, eventName);
    EXPECT_NE(hisysEventObserver_, nullptr);

    // incorrect state keyword
    sysEvent["PID"] = TEST_PID;
    sysEvent["state"] = 0;
    hisysEventObserver_->ProcessBluetoothEvent(sysEvent, eventName);
    EXPECT_NE(hisysEventObserver_, nullptr);

    // bluetooth state turn on
    sysEvent["STATE"] = 1;
    hisysEventObserver_->ProcessBluetoothEvent(sysEvent, eventName);
    EXPECT_NE(hisysEventObserver_, nullptr);

    // bluetooth state turn off
    sysEvent["STATE"] = 3;
    hisysEventObserver_->ProcessBluetoothEvent(sysEvent, eventName);
    EXPECT_NE(hisysEventObserver_, nullptr);
}

/**
 * @tc.name: hisysEventWifiEvent_001
 * @tc.desc: test hisysevent wifi event processing
 * @tc.type: FUNC
 * @tc.require: issueI8IDAO
 */
HWTEST_F(ObserverEventTest, hisysEventWifiEvent_001, testing::ext::TestSize.Level1)
{
    nlohmann::json sysEvent;
    std::string eventName = "INVAILD";
    // incorrect uid keyword
    sysEvent["uid"] = TEST_UID;
    hisysEventObserver_->ProcessWifiEvent(sysEvent, eventName);
    EXPECT_NE(hisysEventObserver_, nullptr);

    // incorrect pid keyword
    sysEvent["uid_"] = TEST_UID;
    sysEvent["pid"] = TEST_PID;
    hisysEventObserver_->ProcessWifiEvent(sysEvent, eventName);
    EXPECT_NE(hisysEventObserver_, nullptr);

    // incorrect eventname
    sysEvent["pid_"] = TEST_PID;
    hisysEventObserver_->ProcessWifiEvent(sysEvent, eventName);
    EXPECT_NE(hisysEventObserver_, nullptr);

    // wifi connection state: connected
    eventName = "WIFI_CONNECTION";
    sysEvent["TYPE"] = WifiState::CONNECTED;
    hisysEventObserver_->ProcessWifiEvent(sysEvent, eventName);
    EXPECT_NE(hisysEventObserver_, nullptr);

    // wifi connection state: disconnected
    sysEvent["TYPE"] = WifiState::DISCONNECTED;
    hisysEventObserver_->ProcessWifiEvent(sysEvent, eventName);
    EXPECT_NE(hisysEventObserver_, nullptr);

    // wifi scan state
    eventName = "WIFI_SCAN";
    hisysEventObserver_->ProcessWifiEvent(sysEvent, eventName);
    EXPECT_NE(hisysEventObserver_, nullptr);
}

/**
 * @tc.name: hisysEventScreenCaptureEvent_001
 * @tc.desc: test hisysevent screen capture event processing
 * @tc.type: FUNC
 */
HWTEST_F(ObserverEventTest, hisysEventScreenCaptureEvent_001, testing::ext::TestSize.Level1)
{
    nlohmann::json sysEvent;
    std::string eventName = "INVAILD";
    // incorrect uid keyword
    sysEvent["uid"] = TEST_UID;
    hisysEventObserver_->ProcessScreenCaptureEvent(sysEvent, eventName);
    EXPECT_NE(hisysEventObserver_, nullptr);

    // incorrect pid keyword
    sysEvent["APP_UID"] = TEST_UID;
    sysEvent["pid"] = TEST_PID;
    hisysEventObserver_->ProcessScreenCaptureEvent(sysEvent, eventName);
    EXPECT_NE(hisysEventObserver_, nullptr);

    // incorrect eventname
    sysEvent["APP_PID"] = TEST_PID;
    hisysEventObserver_->ProcessScreenCaptureEvent(sysEvent, eventName);
    EXPECT_NE(hisysEventObserver_, nullptr);

    // START_SCREEN_CAPTURE
    eventName = "PLAYER_STATE";
    sysEvent["STATUS"] = "start";
    hisysEventObserver_->ProcessScreenCaptureEvent(sysEvent, eventName);
    EXPECT_NE(hisysEventObserver_, nullptr);

    // STOP_SCREEN_CAPTURE
    sysEvent["STATUS"] = "stop";
    hisysEventObserver_->ProcessScreenCaptureEvent(sysEvent, eventName);
    EXPECT_NE(hisysEventObserver_, nullptr);
}

/**
 * @tc.name: processHiSysEvent_001
 * @tc.desc: the hisysevent onevent test
 * @tc.type: FUNC
 * @tc.require: issueI8IDAO
 */
HWTEST_F(ObserverEventTest, processHiSysEvent_001, testing::ext::TestSize.Level1)
{
    hisysEventObserver_->OnEvent(nullptr);

    nlohmann::json sysEvent;
    sysEvent["domain_"] = "RUNNINGLOCK";
    sysEvent["name_"] = "RUNNINGLOCK";
    sysEvent["UID"] = TEST_UID;
    sysEvent["PID"] = TEST_PID;
    hisysEventObserver_->OnEvent(std::make_shared<HiviewDFX::HiSysEventRecord>(
        sysEvent.dump(JSON_FORMAT)));
    EXPECT_NE(hisysEventObserver_, nullptr);
}

/**
 * @tc.name: processHiSysEvent_002
 * @tc.desc: the process hisysevent test
 * @tc.type: FUNC
 * @tc.require: issueI8IDAO
 */
HWTEST_F(ObserverEventTest, processHiSysEvent_002, testing::ext::TestSize.Level1)
{
    nlohmann::json sysEvent;
    std::string eventName = "INVAILD";
    sysEvent["domain_"] = "AV_CODEC";
    hisysEventObserver_->ProcessHiSysEvent(eventName, sysEvent);
    EXPECT_NE(hisysEventObserver_, nullptr);

    sysEvent["domain_"] = "INVAILD";
    eventName = "RUNNINGLOCK";
    hisysEventObserver_->ProcessHiSysEvent(eventName, sysEvent);
    EXPECT_NE(hisysEventObserver_, nullptr);
}

/**
 * @tc.name: mmiObserverEvent_001
 * @tc.desc: test multimodal input sync bundleName interface
 * @tc.type: FUNC
 * @tc.require: issueI8IDAO
 */
HWTEST_F(ObserverEventTest, mmiObserverEvent_001, testing::ext::TestSize.Level1)
{
    int32_t pid = TEST_PID;
    int32_t uid = TEST_UID;
    std::string bundleName;
    int32_t status = 0;
    // the scene of bundleName is null
    mmiObserver_->SyncBundleName(pid, uid, bundleName, status);
    EXPECT_NE(mmiObserver_, nullptr);

    // the scene of bundleName is not null
    bundleName = "inputmethod";
    mmiObserver_->SyncBundleName(pid, uid, bundleName, status);
    EXPECT_NE(mmiObserver_, nullptr);
}

/**
 * @tc.name: deviceMovementObserverEvent_001
 * @tc.desc: test multimodal input sync bundleName interface
 * @tc.type: FUNC
 * @tc.require: issueI8VZVN
 */
HWTEST_F(ObserverEventTest, deviceMovementObserverEvent_001, testing::ext::TestSize.Level1)
{
#ifdef DEVICE_MOVEMENT_PERCEPTION_ENABLE
    // data is null
    int32_t code = -1;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    int32_t testRequest = deviceMovementObserver_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(testRequest, -1);

    //code is MOVEMENT_CHANGE
    data.WriteInterfaceToken(DeviceMovementObserver::GetDescriptor());
    code = static_cast<int32_t>(Msdp::ImovementCallback::MOVEMENT_CHANGE);
    int32_t t1 = deviceMovementObserver_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(t1, 0);

    //code is not MOVEMENT_CHANGE
    code = -1;
    int32_t t2 = deviceMovementObserver_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(t2, IPCObjectStub::OnRemoteRequest(code, data, reply, option));
#endif
}

/**
 * @tc.name: schedTelephonyObserverEvent_001
 * @tc.desc: test multimodal input sync bundleName interface
 * @tc.type: FUNC
 * @tc.require: issueI8VZVN
 */
HWTEST_F(ObserverEventTest, schedTelephonyObserverEvent_001, testing::ext::TestSize.Level1)
{
#ifdef RESSCHED_TELEPHONY_STATE_REGISTRY_ENABLE
    int32_t slotId = 0;
    int32_t callState = 0;
    std::u16string phoneNumber;
    schedTelephonyObserver_->onCallStateUpdated(slotId, callState, phoneNumber);
    SUCCEED();
#endif
}

/**
 * @tc.name: connectionSubscriberEvent_001
 * @tc.desc: test multimodal input sync bundleName interface
 * @tc.type: FUNC
 * @tc.require: issueI8VZVN
 */
HWTEST_F(ObserverEventTest, connectionSubscriberEvent_001, testing::ext::TestSize.Level1)
{
    //test the interface
    AbilityRuntime::ConnectionData data;
    AbilityRuntime::DlpStateData data1;
    nlohmann::json payload;
    connectionSubscriber_->MarshallingConnectionData(data, payload);
    SUCCEED();
    connectionSubscriber_->OnExtensionConnected(data);
    SUCCEED();
    connectionSubscriber_->OnExtensionDisconnected(data);
    SUCCEED();
    connectionSubscriber_->MarshallingDlpStateData(data1, payload);
    SUCCEED();
    connectionSubscriber_->OnDlpAbilityOpened(data1);
    SUCCEED();
    connectionSubscriber_->OnDlpAbilityClosed(data1);
    SUCCEED();
    connectionSubscriber_->OnServiceDied();
    SUCCEED();
}

/**
 * @tc.name: audioObserverObserverEvent_001
 * @tc.desc: test multimodal input sync bundleName interface
 * @tc.type: FUNC
 * @tc.require: issueI8VZVN
 */
HWTEST_F(ObserverEventTest, audioObserverEvent_001, testing::ext::TestSize.Level1)
{
#ifdef RESSCHED_AUDIO_FRAMEWORK_ENABLE
    std::unique_ptr<AudioStandard::AudioRendererChangeInfo> audioRendererChangeInfo;
    nlohmann::json payload;
    audioObserver_->MarshallingAudioRendererChangeInfo(audioRendererChangeInfo, payload);
    SUCCEED();
    audioObserver_->OnRendererStateChange(audioRendererChangeInfo);
    SUCCEED();

    //ringerMode equals mode_
    AudioStandard::AudioRingerMode ringerMode = -1;.
    audioObserver_->OnRingerModeUpdated(ringerMode);
    SUCCEED();
    //ringerMode is not mode_
    AudioStandard::AudioRingerMode ringerMode1 = 0;
    audioObserver_->OnRingerModeUpdated(ringerMode);
    SUCCEED();

    //test the interface of OnVolumeKeyEvent
    AudioStandard::VolumeEvent volumeEvent;
    audioObserver_->OnVolumeKeyEvent(volumeEvent);
    SUCCEED();
#endif
}

/**
 * @tc.name: mmiObserverEvent_002
 * @tc.desc: test multimodal input get mmi status status interface
 * @tc.type: FUNC
 * @tc.require: issueI8ZIVH
 */
HWTEST_F(ObserverEventTest, mmiObserverEvent_002, testing::ext::TestSize.Level1)
{
    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    EXPECT_NE(samgr, nullptr);
    auto remoteObj = samgr->GetSystemAbility(MULTIMODAL_INPUT_SERVICE_ID);
    if (!remoteObj) {
        return;
    }
    auto instance = ObserverManager::GetInstance();
    if (instance) {
        instance->GetAllMmiStatusData();
    }
    SUCCEED();
}
#ifndef RESOURCE_REQUEST_REQUEST
/**
 * @tc.name: foldDisplayModeObserver_001
 * @tc.desc: test fold display mode status interface
 * @tc.type: FUNC
 * @tc.require: issueI8ZIVH
 */
HWTEST_F(ObserverEventTest, foldDisplayModeObserver_001, testing::ext::TestSize.Level1)
{
    const std::string DISPLAY_MODE_MAIN = "displayMain";
    foldDisplayModeObserver_->OnDisplayModeChanged(FoldDisplayMode::MAIN);
    EXPECT_EQ(foldDisplayModeObserver_->currentDisplayMode, DISPLAY_MODE_MAIN);

    const std::string DISPLAY_MODE_FULL = "displayFull";
    foldDisplayModeObserver_->OnDisplayModeChanged(FoldDisplayMode::FULL);
    EXPECT_EQ(foldDisplayModeObserver_->currentDisplayMode, DISPLAY_MODE_FULL);

    const std::string DISPLAY_MODE_SUB = "displaySub";
    foldDisplayModeObserver_->OnDisplayModeChanged(FoldDisplayMode::SUB);
    EXPECT_EQ(foldDisplayModeObserver_->currentDisplayMode, DISPLAY_MODE_SUB);

    const std::string DISPLAY_MODE_UNKOWN = "displayUnknown";
    foldDisplayModeObserver_->OnDisplayModeChanged(FoldDisplayMode::UNKNOWN);
    EXPECT_EQ(foldDisplayModeObserver_->currentDisplayMode, DISPLAY_MODE_UNKOWN);
}

/**
 * @tc.name: downLoadUploadObserver_001
 * @tc.desc: Test downLoad Upload Observer
 * @tc.type: FUNC
 * @tc.require: issuesI9BU37
 */
HWTEST_F(ObserverEventTest, downLoadUploadObserver_001, testing::ext::TestSize.Level1)
{
    std::shared_ptr<DownLoadUploadObserver> downLoadUploadObserver_ =
        std::make_shared<DownLoadUploadObserver>();
    downLoadUploadObserver_->OnRunningTaskCountUpdate(0);
    SUCCEED();
    downLoadUploadObserver_->OnRunningTaskCountUpdate(1);
    SUCCEED();
    downLoadUploadObserver_ = nullptr;
}

/**
 * @tc.name: observerManager_001
 * @tc.desc: test observer Manager
 * @tc.type: FUNC
 * @tc.require: issuesI9BU37
 */
HWTEST_F(ObserverEventTest, observerManager_001, testing::ext::TestSize.Level1)
{
    auto instance = ObserverManager::GetInstance();
    if (instance) {
        auto downLoadUploadObserver_ = std::make_shared<DownLoadUploadObserver>();
        instance->downLoadUploadObserver_ = downLoadUploadObserver_;
        instance->InitDownloadUploadObserver();
        instance->DisableDownloadUploadObserver();
    }
    SUCCEED();
}
#endif
}
}