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
#include "account_observer.h"
#ifdef RESOURCE_SCHEDULE_SERVICE_WITH_APP_NAP_ENABLE
#include "hisysevent_observer.h"
#ifdef MMI_ENABLE
#include "mmi_observer.h"
#endif
#endif
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
#include "app_state_observer.h"
#include "window_state_observer.h"
#include "background_task_observer.h"
#include "app_startup_scene_rec.h"

namespace OHOS {
namespace ResourceSchedule {
namespace {
#ifdef RESOURCE_SCHEDULE_SERVICE_WITH_APP_NAP_ENABLE
    static const int32_t TEST_UID = 0;
    static const int32_t TEST_PID = 1000;
    static const int32_t TEST_INSTANCE_ID = 123456;
    static const int32_t JSON_FORMAT = 4;
#ifdef MMI_ENABLE
    static const int32_t MMI_TEST_UID = 0;
    static const int32_t MMI_TEST_PID = 1000;
#endif
#endif
}
class ObserverEventTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() {}
    void TearDown() {}
#ifdef RESOURCE_SCHEDULE_SERVICE_WITH_APP_NAP_ENABLE
    static std::shared_ptr<HiSysEventObserver> hisysEventObserver_;
#ifdef MMI_ENABLE
    static std::shared_ptr<MmiObserver> mmiObserver_;
#endif
#endif
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
    static std::shared_ptr<FoldDisplayOrientationObserver> foldDisplayOrientationObserver_;
};

#ifdef RESOURCE_SCHEDULE_SERVICE_WITH_APP_NAP_ENABLE
std::shared_ptr<HiSysEventObserver> ObserverEventTest::hisysEventObserver_ = nullptr;
#ifdef MMI_ENABLE
std::shared_ptr<MmiObserver> ObserverEventTest::mmiObserver_ = nullptr;
#endif
#endif
std::shared_ptr<FoldDisplayModeObserver> ObserverEventTest::foldDisplayModeObserver_ = nullptr;
std::shared_ptr<FoldDisplayOrientationObserver> ObserverEventTest::foldDisplayOrientationObserver_ = nullptr;
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
#ifdef RESOURCE_SCHEDULE_SERVICE_WITH_APP_NAP_ENABLE
    hisysEventObserver_ = std::make_shared<HiSysEventObserver>();
#ifdef MMI_ENABLE
    mmiObserver_ = std::make_shared<MmiObserver>();
#endif
#endif
    connectionSubscriber_ = std::make_shared<ConnectionSubscriber>();
    foldDisplayModeObserver_ = std::make_shared<FoldDisplayModeObserver>();
    foldDisplayOrientationObserver_ = std::make_shared<FoldDisplayOrientationObserver>();
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
#ifdef RESOURCE_SCHEDULE_SERVICE_WITH_APP_NAP_ENABLE
    hisysEventObserver_ = nullptr;
#ifdef MMI_ENABLE
    mmiObserver_ = nullptr;
#endif
#endif
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

#ifdef RESOURCE_SCHEDULE_SERVICE_WITH_APP_NAP_ENABLE
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
    sysEvent["STATE"] = -1;
    hisysEventObserver_->ProcessRunningLockEvent(sysEvent, eventName);
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
    sysEvent["TYPE"] = -1;
    hisysEventObserver_->ProcessWifiEvent(sysEvent, eventName);
    eventName = "test";
    hisysEventObserver_->ProcessWifiEvent(sysEvent, eventName);
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
    hisysEventObserver_->OnServiceDied();
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
 * @tc.name: processHiSysEvent_003
 * @tc.desc: the process hisysevent test
 * @tc.type: FUNC
 * @tc.require: issuesIAJZVI
 */
HWTEST_F(ObserverEventTest, processHiSysEvent_003, testing::ext::TestSize.Level1)
{
    nlohmann::json sysEvent;
    sysEvent["domain_"] = "INVAILD";
    std::string eventName = "CAMERA_CONNECT";
    hisysEventObserver_->ProcessHiSysEvent(eventName, sysEvent);
    eventName = "CAMERA_DISCONNECT";
    hisysEventObserver_->ProcessHiSysEvent(eventName, sysEvent);
    eventName = "WIFI_CONNECTION";
    hisysEventObserver_->ProcessHiSysEvent(eventName, sysEvent);
    eventName = "WIFI_SCAN";
    hisysEventObserver_->ProcessHiSysEvent(eventName, sysEvent);
    eventName = "PLAYER_STATE";
    hisysEventObserver_->ProcessHiSysEvent(eventName, sysEvent);
    EXPECT_NE(hisysEventObserver_, nullptr);
}

#ifdef MMI_ENABLE
/**
 * @tc.name: mmiObserverEvent_001
 * @tc.desc: test multimodal input sync bundleName interface
 * @tc.type: FUNC
 * @tc.require: issueI8IDAO
 */
HWTEST_F(ObserverEventTest, mmiObserverEvent_001, testing::ext::TestSize.Level1)
{
    int32_t pid = MMI_TEST_PID;
    int32_t uid = MMI_TEST_UID;
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
#endif
#endif

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
    EXPECT_EQ(t2, -1);
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
    schedTelephonyObserver_->OnCallStateUpdated(slotId, callState, phoneNumber);
    EXPECT_TRUE(schedTelephonyObserver_ != nullptr);
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
    connectionSubscriber_->OnExtensionSuspended(data);
    connectionSubscriber_->OnExtensionResumed(data);
    connectionSubscriber_->MarshallingDlpStateData(data1, payload);
    SUCCEED();
    connectionSubscriber_->OnDlpAbilityOpened(data1);
    SUCCEED();
    connectionSubscriber_->OnDlpAbilityClosed(data1);
    SUCCEED();
    connectionSubscriber_->OnServiceDied();
    EXPECT_TRUE(connectionSubscriber_ != nullptr);
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
    std::shared_ptr<AudioStandard::AudioRendererChangeInfo> audioRendererChangeInfo =
        std::make_shared<AudioStandard::AudioRendererChangeInfo>();
    nlohmann::json payload;
    audioObserver_->MarshallingAudioRendererChangeInfo(audioRendererChangeInfo, payload);
    SUCCEED();
    std::vector<std::shared_ptr<AudioStandard::AudioRendererChangeInfo>> audioRenderVector;
    audioRenderVector.emplace_back(std::move(audioRendererChangeInfo));
    audioObserver_->OnRendererStateChange(audioRenderVector);
    SUCCEED();

    //ringerMode equals mode_
    AudioStandard::AudioRingerMode ringerMode = AudioStandard::AudioRingerMode::RINGER_MODE_NORMAL;
    audioObserver_->OnRingerModeUpdated(ringerMode);
    SUCCEED();
    //ringerMode is not mode_
    audioObserver_->OnRingerModeUpdated(ringerMode);
    SUCCEED();

    //test the interface of OnVolumeKeyEvent
    AudioStandard::VolumeEvent volumeEvent;
    audioObserver_->OnVolumeKeyEvent(volumeEvent);
    SUCCEED();

    //test the interface of OnAudioSceneChange
    AudioStandard::AudioScene audioScene = AudioStandard::AudioScene::AUDIO_SCENE_DEFAULT;
    audioObserver_->OnAudioSceneChange(audioScene);
    SUCCEED();

    // test the interface of OnPreferredOutputDeviceUpdated
    std::vector<std::shared_ptr<AudioStandard::AudioDeviceDescriptor>> descs;
    audioObserver_->OnPreferredOutputDeviceUpdated(descs);
    EXPECT_TRUE(audioObserver_ != nullptr);
    audioObserver_->processRenderStateMap_.clear();
#endif
}

/**
 * @tc.name: audioObserverObserverEvent_002
 * @tc.desc: test HandleInnerAudioStateChange
 * @tc.type: FUNC
 * @tc.require: issueI8VZVN
 */
HWTEST_F(ObserverEventTest, audioObserverEvent_002, testing::ext::TestSize.Level1)
{
#ifdef RESSCHED_AUDIO_FRAMEWORK_ENABLE
    std::shared_ptr<AudioStandard::AudioRendererChangeInfo> audioRendererChangeInfo = nullptr;
    nlohmann::json payload;
    std::unordered_set<int32_t> newRunningPid;
    std::unordered_set<int32_t> newStopSessionPid;
    audioObserver_->HandleInnerAudioStateChange(audioRendererChangeInfo, newRunningPid, newStopSessionPid);
    audioRendererChangeInfo = std::make_shared<AudioStandard::AudioRendererChangeInfo>();
    audioRendererChangeInfo->clientPid = 1;
    audioRendererChangeInfo->rendererState = AudioStandard::RendererState::RENDERER_INVALID;
    audioObserver_->HandleInnerAudioStateChange(audioRendererChangeInfo, newRunningPid, newStopSessionPid);
    EXPECT_EQ(newRunningPid.size(), 0);
    newRunningPid.clear();
    newStopSessionPid.clear();
    audioRendererChangeInfo->rendererState = AudioStandard::RendererState::RENDERER_RUNNING;
    audioObserver_->HandleInnerAudioStateChange(audioRendererChangeInfo, newRunningPid, newStopSessionPid);
    EXPECT_EQ(newRunningPid.size(), 1);
    audioObserver_->HandleInnerAudioStateChange(audioRendererChangeInfo, newRunningPid, newStopSessionPid);
    newRunningPid.clear();
    newStopSessionPid.clear();
    audioRendererChangeInfo->rendererState = AudioStandard::RendererState::RENDERER_STOPPED;
    audioObserver_->HandleInnerAudioStateChange(audioRendererChangeInfo, newRunningPid, newStopSessionPid);
    EXPECT_EQ(newStopSessionPid.size(), 1);
    audioObserver_->HandleInnerAudioStateChange(audioRendererChangeInfo, newRunningPid, newStopSessionPid);
    newRunningPid.clear();
    newStopSessionPid.clear();
    audioRendererChangeInfo->rendererState = AudioStandard::RendererState::RENDERER_RELEASED;
    audioObserver_->HandleInnerAudioStateChange(audioRendererChangeInfo, newRunningPid, newStopSessionPid);
    EXPECT_EQ(newStopSessionPid.size(), 1);
    newRunningPid.clear();
    newStopSessionPid.clear();
    audioRendererChangeInfo->rendererState = AudioStandard::RendererState::RENDERER_PAUSED;
    audioObserver_->HandleInnerAudioStateChange(audioRendererChangeInfo, newRunningPid, newStopSessionPid);
    EXPECT_EQ(newStopSessionPid.size(), 1);
    newRunningPid.clear();
    newStopSessionPid.clear();
    audioObserver_->processRenderStateMap_.clear();
#endif
}

/**
 * @tc.name: audioObserverObserverEvent_003
 * @tc.desc: test ProcessRunningSessionState
 * @tc.type: FUNC
 * @tc.require: issueI8VZVN
 */
HWTEST_F(ObserverEventTest, audioObserverEvent_003, testing::ext::TestSize.Level1)
{
#ifdef RESSCHED_AUDIO_FRAMEWORK_ENABLE
    std::shared_ptr<AudioStandard::AudioRendererChangeInfo> audioRendererChangeInfo = nullptr;
    std::unordered_set<int32_t> newRunningPid;
    audioObserver_->ProcessRunningSessionState(audioRendererChangeInfo, newRunningPid);
    audioRendererChangeInfo = std::make_shared<AudioStandard::AudioRendererChangeInfo>();
    audioRendererChangeInfo->clientPid = 1;
    audioRendererChangeInfo->rendererState = AudioStandard::RendererState::RENDERER_RUNNING;
    audioObserver_->ProcessRunningSessionState(audioRendererChangeInfo, newRunningPid);
    audioObserver_->ProcessRunningSessionState(audioRendererChangeInfo, newRunningPid);
    EXPECT_EQ(newRunningPid.size(), 1);
    audioObserver_->processRenderStateMap_.clear();
#endif
}

/**
 * @tc.name: audioObserverObserverEvent_004
 * @tc.desc: test ProcessStopSessionState
 * @tc.type: FUNC
 * @tc.require: issueI8VZVN
 */
HWTEST_F(ObserverEventTest, audioObserverEvent_004, testing::ext::TestSize.Level1)
{
#ifdef RESSCHED_AUDIO_FRAMEWORK_ENABLE
    std::shared_ptr<AudioStandard::AudioRendererChangeInfo> audioRendererChangeInfo = nullptr;
    std::unordered_set<int32_t> newRunningPid;
    std::unordered_set<int32_t> newStopSessionPid;
    audioObserver_->ProcessStopSessionState(audioRendererChangeInfo, newStopSessionPid);
    audioRendererChangeInfo = std::make_shared<AudioStandard::AudioRendererChangeInfo>();
    audioRendererChangeInfo->clientPid = 1;
    audioObserver_->ProcessStopSessionState(audioRendererChangeInfo, newStopSessionPid);
    EXPECT_EQ(newStopSessionPid.size(), 1);
    newStopSessionPid.clear();
    audioObserver_->ProcessRunningSessionState(audioRendererChangeInfo, newRunningPid);
    audioObserver_->ProcessStopSessionState(audioRendererChangeInfo, newStopSessionPid);
    EXPECT_EQ(newStopSessionPid.size(), 1);
    audioObserver_->processRenderStateMap_.clear();
#endif
}

/**
 * @tc.name: audioObserverObserverEvent_005
 * @tc.desc: test HandleStartAudioStateEvent
 * @tc.type: FUNC
 * @tc.require: issueI8VZVN
 */
HWTEST_F(ObserverEventTest, audioObserverEvent_005, testing::ext::TestSize.Level1)
{
#ifdef RESSCHED_AUDIO_FRAMEWORK_ENABLE
    std::unordered_set<int32_t> newRunningPid;
    int32_t pid = 0;
    newRunningPid.insert(pid);
    audioObserver_->HandleStartAudioStateEvent(newRunningPid);
    ProcessRenderState processRenderState;
    audioObserver_->processRenderStateMap_[pid] = processRenderState;
    audioObserver_->HandleStartAudioStateEvent(newRunningPid);
    int32_t SessionId = 0;
    processRenderState.renderRunningSessionSet.insert(SessionId);
    audioObserver_->processRenderStateMap_[pid] = processRenderState;
    audioObserver_->HandleStartAudioStateEvent(newRunningPid);
    EXPECT_TRUE(audioObserver_->processRenderStateMap_.find(pid) != audioObserver_->processRenderStateMap_.end());
    audioObserver_->processRenderStateMap_.clear();
#endif
}

/**
 * @tc.name: audioObserverObserverEvent_006
 * @tc.desc: test HandleStopAudioStateEvent
 * @tc.type: FUNC
 * @tc.require: issueI8VZVN
 */
HWTEST_F(ObserverEventTest, audioObserverEvent_006, testing::ext::TestSize.Level1)
{
#ifdef RESSCHED_AUDIO_FRAMEWORK_ENABLE
    int32_t pid = 0;
    std::unordered_set<int32_t> newStopSessionPid;
    newStopSessionPid.insert(pid);
    audioObserver_->HandleStopAudioStateEvent(newStopSessionPid);
    ProcessRenderState processRenderState;
    int32_t SessionId = 0;
    processRenderState.renderRunningSessionSet.insert(SessionId);
    audioObserver_->processRenderStateMap_[pid] = processRenderState;
    audioObserver_->HandleStopAudioStateEvent(newStopSessionPid);
    processRenderState.renderRunningSessionSet.clear();
    audioObserver_->processRenderStateMap_[pid] = processRenderState;
    audioObserver_->HandleStopAudioStateEvent(newStopSessionPid);
    EXPECT_TRUE(audioObserver_->processRenderStateMap_.find(pid) == audioObserver_->processRenderStateMap_.end());
    audioObserver_->processRenderStateMap_.clear();
#endif
}

#ifdef RESOURCE_SCHEDULE_SERVICE_WITH_APP_NAP_ENABLE
#ifdef MMI_ENABLE
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
    auto instance = ObserverManager::GetInstance();
    if (instance) {
        instance->GetAllMmiStatusData();
    }
    SUCCEED();
}
#endif
#endif

#ifdef RESOURCE_REQUEST_REQUEST
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
    downLoadUploadObserver_->OnRunningTaskCountUpdate(1);
    EXPECT_TRUE(downLoadUploadObserver_->isReportScene);
    downLoadUploadObserver_->OnRunningTaskCountUpdate(0);
    EXPECT_FALSE(downLoadUploadObserver_->isReportScene);
    downLoadUploadObserver_->OnRunningTaskCountUpdate(-1);
    EXPECT_FALSE(downLoadUploadObserver_->isReportScene);
    downLoadUploadObserver_ = nullptr;
}
#endif
/**
 * @tc.name: accountObserver_001
 * @tc.desc: test account observer
 * @tc.type: FUNC
 * @tc.require: issuesI9SSQY
 */
HWTEST_F(ObserverEventTest, accountObserver_001, testing::ext::TestSize.Level1)
{
    AccountSA::OsAccountSubscribeInfo osAccountSubscribeInfo;
    osAccountSubscribeInfo.SetOsAccountSubscribeType(AccountSA::OS_ACCOUNT_SUBSCRIBE_TYPE::ACTIVATING);
    osAccountSubscribeInfo.SetName("ResschdeAccountActivatingSubscriberTest");
    auto accountObserver = std::make_shared<AccountObserver>(osAccountSubscribeInfo);
    accountObserver->OnAccountsChanged(200);
    SUCCEED();
    accountObserver->OnAccountsChanged(201);
    SUCCEED();
    accountObserver->OnAccountsChanged(-1);
    EXPECT_TRUE(accountObserver != nullptr);
    accountObserver = nullptr;
}

#ifdef RESOURCE_SCHEDULE_SERVICE_WITH_APP_NAP_ENABLE
/**
 * @tc.name: OnEvent_001
 * @tc.desc: test account observer
 * @tc.type: FUNC
 * @tc.require: issuesI9SSQY
 */
HWTEST_F(ObserverEventTest, OnEvent_001, testing::ext::TestSize.Level1)
{
    nlohmann::json sysEvent;
    sysEvent["domain_"] = "RUNNINGLOCKTESTRUNNINGLOCKTESTRUNNINGLOCKTESTRUNNINGLOCKTESTRUNNINGLOCKTESTRUNNINGLOCKTEST";
    hisysEventObserver_->OnEvent(std::make_shared<HiviewDFX::HiSysEventRecord>(
        sysEvent.dump(JSON_FORMAT)));
    sysEvent["name_"] = "RUNNINGLOCKTESTRUNNINGLOCKTESTRUNNINGLOCKTESTRUNNINGLOCKTESTRUNNINGLOCKTESTRUNNINGLOCKTEST1";
    sysEvent["test1"] = "RUNNINGLOCKTESTRUNNINGLOCKTESTRUNNINGLOCKTESTRUNNINGLOCKTESTRUNNINGLOCKTESTRUNNINGLOCKTEST1";
    sysEvent["test2"] = "RUNNINGLOCKTESTRUNNINGLOCKTESTRUNNINGLOCKTESTRUNNINGLOCKTESTRUNNINGLOCKTESTRUNNINGLOCKTEST1";
    sysEvent["test3"] = "RUNNINGLOCKTESTRUNNINGLOCKTESTRUNNINGLOCKTESTRUNNINGLOCKTESTRUNNINGLOCKTESTRUNNINGLOCKTEST1";
    sysEvent["test4"] = "RUNNINGLOCKTESTRUNNINGLOCKTESTRUNNINGLOCKTESTRUNNINGLOCKTESTRUNNINGLOCKTESTRUNNINGLOCKTEST1";
    sysEvent["test5"] = "RUNNINGLOCKTESTRUNNINGLOCKTESTRUNNINGLOCKTESTRUNNINGLOCKTESTRUNNINGLOCKTESTRUNNINGLOCKTEST1";
    sysEvent["test6"] = "RUNNINGLOCKTESTRUNNINGLOCKTESTRUNNINGLOCKTESTRUNNINGLOCKTESTRUNNINGLOCKTESTRUNNINGLOCKTEST1";
    sysEvent["test7"] = "RUNNINGLOCKTESTRUNNINGLOCKTESTRUNNINGLOCKTESTRUNNINGLOCKTESTRUNNINGLOCKTESTRUNNINGLOCKTEST1";
    sysEvent["test8"] = "RUNNINGLOCKTESTRUNNINGLOCKTESTRUNNINGLOCKTESTRUNNINGLOCKTESTRUNNINGLOCKTESTRUNNINGLOCKTEST1";
    sysEvent["test9"] = "RUNNINGLOCKTESTRUNNINGLOCKTESTRUNNINGLOCKTESTRUNNINGLOCKTESTRUNNINGLOCKTESTRUNNINGLOCKTEST1";
    sysEvent["test10"] = "RUNNINGLOCKTESTRUNNINGLOCKTESTRUNNINGLOCKTESTRUNNINGLOCKTESTRUNNINGLOCKTESTRUNNINGLOCKTEST1";
    sysEvent["test11"] = "RUNNINGLOCKTESTRUNNINGLOCKTESTRUNNINGLOCKTESTRUNNINGLOCKTESTRUNNINGLOCKTESTRUNNINGLOCKTEST1";
    sysEvent["test12"] = "RUNNINGLOCKTESTRUNNINGLOCKTESTRUNNINGLOCKTESTRUNNINGLOCKTESTRUNNINGLOCKTESTRUNNINGLOCKTEST1";
    sysEvent["test13"] = "RUNNINGLOCKTESTRUNNINGLOCKTESTRUNNINGLOCKTESTRUNNINGLOCKTESTRUNNINGLOCKTESTRUNNINGLOCKTEST1";
    sysEvent["test14"] = "RUNNINGLOCKTESTRUNNINGLOCKTESTRUNNINGLOCKTESTRUNNINGLOCKTESTRUNNINGLOCKTESTRUNNINGLOCKTEST1";
    sysEvent["test15"] = "RUNNINGLOCKTESTRUNNINGLOCKTESTRUNNINGLOCKTESTRUNNINGLOCKTESTRUNNINGLOCKTESTRUNNINGLOCKTEST1";
    sysEvent["test16"] = "RUNNINGLOCKTESTRUNNINGLOCKTESTRUNNINGLOCKTESTRUNNINGLOCKTESTRUNNINGLOCKTESTRUNNINGLOCKTEST1";
    sysEvent["test17"] = "RUNNINGLOCKTESTRUNNINGLOCKTESTRUNNINGLOCKTESTRUNNINGLOCKTESTRUNNINGLOCKTESTRUNNINGLOCKTEST1";
    sysEvent["test18"] = "RUNNINGLOCKTESTRUNNINGLOCKTESTRUNNINGLOCKTESTRUNNINGLOCKTESTRUNNINGLOCKTESTRUNNINGLOCKTEST1";
    sysEvent["UID"] = TEST_UID;
    sysEvent["PID"] = TEST_PID;
    hisysEventObserver_->OnEvent(std::make_shared<HiviewDFX::HiSysEventRecord>(
        sysEvent.dump(JSON_FORMAT)));
    sysEvent["domain_"] = 1;
    hisysEventObserver_->OnEvent(std::make_shared<HiviewDFX::HiSysEventRecord>(
        sysEvent.dump(JSON_FORMAT)));
    EXPECT_NE(hisysEventObserver_, nullptr);
    EXPECT_NE(hisysEventObserver_, nullptr);
}
#endif

/**
 * @tc.name: DisableDataShareObserver_001
 * @tc.desc: test account observer DisableDataShareObserver
 * @tc.type: FUNC
 * @tc.require: issuesI9SSQY
 */
HWTEST_F(ObserverEventTest, DisableDataShareObserver_001, testing::ext::TestSize.Level1)
{
    auto instance = ObserverManager::GetInstance();
    if (instance) {
        instance->DisableDataShareObserver();
    }
    EXPECT_TRUE(instance);
}

/**
 * @tc.name: DisableAudioObserver_001
 * @tc.desc: test account observer DisableAudioObserver
 * @tc.type: FUNC
 * @tc.require: issuesI9SSQY
 */
HWTEST_F(ObserverEventTest, DisableAudioObserver_001, testing::ext::TestSize.Level1)
{
    auto instance = ObserverManager::GetInstance();
    if (instance) {
        instance->DisableAudioObserver();
        instance->InitAudioObserver();
        instance->DisableAudioObserver();
    }
#ifdef RESSCHED_AUDIO_FRAMEWORK_ENABLE
    EXPECT_EQ(instance->audioObserver_, nullptr);
#else
    SUCCEED();
#endif
}

/**
 * @tc.name: DisableDeviceMovementObserver_001
 * @tc.desc: test account observer DisableDeviceMovementObserver
 * @tc.type: FUNC
 * @tc.require: issuesI9SSQY
 */
HWTEST_F(ObserverEventTest, DisableDeviceMovementObserver_001, testing::ext::TestSize.Level1)
{
    auto instance = ObserverManager::GetInstance();
    if (instance) {
        instance->DisableDeviceMovementObserver();
        instance->InitDeviceMovementObserver();
        instance->DisableDeviceMovementObserver();
    }
#ifdef DEVICE_MOVEMENT_PERCEPTION_ENABLE
    EXPECT_EQ(instance->deviceMovementObserver_, nullptr);
#else
    SUCCEED();
#endif
}

#ifdef RESOURCE_SCHEDULE_SERVICE_WITH_APP_NAP_ENABLE
#ifdef MMI_ENABLE
/**
 * @tc.name: DisableMMiEventObserver_001
 * @tc.desc: test account observer DisableMMiEventObserver
 * @tc.type: FUNC
 * @tc.require: issuesI9SSQY
 */
HWTEST_F(ObserverEventTest, DisableMMiEventObserver_001, testing::ext::TestSize.Level1)
{
    auto instance = ObserverManager::GetInstance();
    if (instance) {
        instance->DisableMMiEventObserver();
    }
    EXPECT_EQ(instance->mmiEventObserver_, nullptr);
}

/**
 * @tc.name: DisableMMiEventObserver_002
 * @tc.desc: test account observer DisableMMiEventObserver
 * @tc.type: FUNC
 * @tc.require: issuesI9SSQY
 */
HWTEST_F(ObserverEventTest, DisableMMiEventObserver_002, testing::ext::TestSize.Level1)
{
    auto instance = ObserverManager::GetInstance();
    if (instance) {
        instance->isNeedReport_ = true;
        instance->InitMMiEventObserver();
        instance->DisableMMiEventObserver();
    }
    EXPECT_EQ(instance->mmiEventObserver_, nullptr);
}
#endif
#endif

/**
 * @tc.name: DisableConnectionSubscriber_001
 * @tc.desc: test account observer DisableConnectionSubscriber
 * @tc.type: FUNC
 * @tc.require: issuesI9SSQY
 */
HWTEST_F(ObserverEventTest, DisableConnectionSubscriber_001, testing::ext::TestSize.Level1)
{
    auto instance = ObserverManager::GetInstance();
    if (instance) {
        instance->DisableConnectionSubscriber();
        instance->InitConnectionSubscriber();
        instance->DisableConnectionSubscriber();
    }
    EXPECT_EQ(instance->connectionSubscriber_, nullptr);
}

/**
 * @tc.name: DisableAVSessionStateChangeListener_001
 * @tc.desc: test account observer DisableAVSessionStateChangeListener
 * @tc.type: FUNC
 * @tc.require: issuesI9SSQY
 */
#ifdef RESSCHED_MULTIMEDIA_AV_SESSION_ENABLE
HWTEST_F(ObserverEventTest, DisableAVSessionStateChangeListener_001, testing::ext::TestSize.Level1)
{
    auto instance = ObserverManager::GetInstance();
    if (instance) {
        instance->DisableAVSessionStateChangeListener();
    }
    EXPECT_EQ(instance->avSessionStateListener_, nullptr);
}
#endif

#ifdef RESOURCE_REQUEST_REQUEST
/**
 * @tc.name: DisableDownloadUploadObserver_001
 * @tc.desc: test account observer DisableDownloadUploadObserver
 * @tc.type: FUNC
 * @tc.require: issuesI9SSQY
 */
HWTEST_F(ObserverEventTest, DisableDownloadUploadObserver_001, testing::ext::TestSize.Level1)
{
    auto instance = ObserverManager::GetInstance();
    if (instance) {
        instance->DisableDownloadUploadObserver();
    }
    EXPECT_EQ(instance->downLoadUploadObserver_, nullptr);
}

/**
 * @tc.name: InitDownloadUploadObserver_001
 * @tc.desc: test account observer InitDownloadUploadObserver
 * @tc.type: FUNC
 * @tc.require: issuesI9SSQY
 */
HWTEST_F(ObserverEventTest, InitDownloadUploadObserver_001, testing::ext::TestSize.Level1)
{
    auto instance = ObserverManager::GetInstance();
    if (instance) {
        instance->InitDownloadUploadObserver();
    }
    EXPECT_NE(instance->downLoadUploadObserver_, nullptr);
}
#endif

/**
 * @tc.name: DisableTelephonyObserver_001
 * @tc.desc: test account observer DisableTelephonyObserver
 * @tc.type: FUNC
 * @tc.require: issuesI9SSQY
 */
HWTEST_F(ObserverEventTest, DisableTelephonyObserver_001, testing::ext::TestSize.Level1)
{
    auto instance = ObserverManager::GetInstance();
    if (instance) {
        instance->DisableTelephonyObserver();
    }
#ifdef RESSCHED_TELEPHONY_STATE_REGISTRY_ENABLE
    EXPECT_EQ(instance->telephonyObserver_, nullptr);
#else
    SUCCEED();
#endif
}

#ifdef RESOURCE_SCHEDULE_SERVICE_WITH_APP_NAP_ENABLE
/**
 * @tc.name: DisableHiSysEventObserver_001
 * @tc.desc: test account observer DisableHiSysEventObserver
 * @tc.type: FUNC
 * @tc.require: issuesI9SSQY
 */
HWTEST_F(ObserverEventTest, DisableHiSysEventObserver_001, testing::ext::TestSize.Level1)
{
    auto instance = ObserverManager::GetInstance();
    if (instance) {
        instance->DisableHiSysEventObserver();
    }
    EXPECT_EQ(instance->hiSysEventObserver_, nullptr);
}
#endif

/**
 * @tc.name: DisableDisplayModeObserver_001
 * @tc.desc: test account observer DisableDisplayModeObserver
 * @tc.type: FUNC
 * @tc.require: issuesI9SSQY
 */
HWTEST_F(ObserverEventTest, DisableDisplayModeObserver_001, testing::ext::TestSize.Level1)
{
    auto instance = ObserverManager::GetInstance();
    if (instance) {
        instance->DisableDisplayModeObserver();
    }
    EXPECT_EQ(instance->foldDisplayModeObserver_, nullptr);
}

/**
 * @tc.name: InitDisplayModeObserver_001
 * @tc.desc: test account observer InitDisplayModeObserver
 * @tc.type: FUNC
 * @tc.require: issuesI9SSQY
 */
HWTEST_F(ObserverEventTest, InitDisplayModeObserver_001, testing::ext::TestSize.Level1)
{
    auto instance = ObserverManager::GetInstance();
    if (instance) {
        instance->InitDisplayModeObserver();
        instance->InitDisplayModeObserver();
        instance->DisableDisplayModeObserver();
    }
    EXPECT_EQ(instance->foldDisplayModeObserver_, nullptr);
}

/**
 * @tc.name: InitDisplayOrientationObserver_001
 * @tc.desc: test account observer InitDisplayOrientationObserver
 * @tc.type: FUNC
 * @tc.require: issuesI9SSQY
 */
HWTEST_F(ObserverEventTest, InitDisplayOrientationObserver_001, testing::ext::TestSize.Level1)
{
    auto instance = ObserverManager::GetInstance();
    bool isFoldable = OHOS::Rosen::DisplayManager::GetInstance().IsFoldable();
    if (!isFoldable) {
        return;
    }
    if (instance) {
        instance->foldDisplayOrientationObserver_ = nullptr;
        instance->InitDisplayOrientationObserver();
        EXPECT_NE(instance->foldDisplayOrientationObserver_, nullptr);
        instance->DisableDisplayOrientationObserver();
        EXPECT_EQ(instance->foldDisplayOrientationObserver_, nullptr);
    }
    EXPECT_EQ(instance->foldDisplayOrientationObserver_, nullptr);
}
#ifdef RESOURCE_SCHEDULE_SERVICE_WITH_APP_NAP_ENABLE
#ifdef MMI_ENABLE
/**
 * @tc.name: GetAllMmiStatusData_001
 * @tc.desc: test account observer GetAllMmiStatusData
 * @tc.type: FUNC
 * @tc.require: issuesI9SSQY
 */
HWTEST_F(ObserverEventTest, GetAllMmiStatusData_001, testing::ext::TestSize.Level1)
{
    auto instance = ObserverManager::GetInstance();
    EXPECT_NE(instance, nullptr);
    if (instance) {
        instance->GetAllMmiStatusData();
    }
    SUCCEED();
}
#endif
#endif

/**
 * @tc.name: AddItemToSysAbilityListener_001
 * @tc.desc: test AddItemToSysAbilityListener
 * @tc.type: FUNC
 * @tc.require: issuesIAJZVI
 */
HWTEST_F(ObserverEventTest, AddItemToSysAbilityListener_001, testing::ext::TestSize.Level1)
{
    sptr<ISystemAbilityManager> systemAbilityManager
        = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    auto instance = ObserverManager::GetInstance();
    if (instance) {
        instance->AddItemToSysAbilityListener(DFX_SYS_EVENT_SERVICE_ABILITY_ID, systemAbilityManager);
        instance->AddItemToSysAbilityListener(-1, systemAbilityManager);
    }
    EXPECT_FALSE(instance->sysAbilityListener_);
}

/**
 * @tc.name: ObserverManagerTest_001
 * @tc.desc: test ObserverManagerTest
 * @tc.type: FUNC
 * @tc.require: issuesIC5T7D
 */
HWTEST_F(ObserverEventTest, ObserverManagerTest_001, testing::ext::TestSize.Level1)
{
    auto instance = ObserverManager::GetInstance();
    EXPECT_NE(instance, nullptr);
    if (instance) {
        instance->sysAbilityListener_->OnAddSystemAbility(APP_MGR_SERVICE_ID, 0);
        instance->sysAbilityListener_->OnAddSystemAbility(SUBSYS_ACCOUNT_SYS_ABILITY_ID_BEGIN, 0);
        instance->sysAbilityListener_->OnAddSystemAbility(WINDOW_MANAGER_SERVICE_ID, 0);
        instance->sysAbilityListener_->OnRemoveSystemAbility(APP_MGR_SERVICE_ID, 0);
        instance->sysAbilityListener_->OnRemoveSystemAbility(SUBSYS_ACCOUNT_SYS_ABILITY_ID_BEGIN, 0);
        instance->sysAbilityListener_->OnRemoveSystemAbility(WINDOW_MANAGER_SERVICE_ID, 0);
    }
    SUCCEED();
}

/**
 * @tc.name: RmsApplicationStateObserver_001
 * @tc.desc: test for OnForegroundApplicationChanged, OnApplicationStateChanged
 * @tc.type: FUNC
 * @tc.require:
 * @tc.desc:
 */
HWTEST_F(ObserverEventTest, RmsApplicationStateObserver_001, testing::ext::TestSize.Level1)
{
    auto observer = std::make_shared<RmsApplicationStateObserver>();
    EXPECT_TRUE(observer != nullptr);
    AppExecFwk::AppStateData data;
    data.bundleName = "com.ohos.test";
    data.pid = 999;
    data.uid = 1000;
    data.state = 0;
    data.accessTokenId = 0;
    data.isFocused = false;
 
    observer->OnForegroundApplicationChanged(data);
    observer->OnApplicationStateChanged(data);
 
    data.uid = -1;
    observer->OnForegroundApplicationChanged(data);
    observer->OnApplicationStateChanged(data);
 
    data.uid = 1000;
    data.pid = -1;
    observer->OnForegroundApplicationChanged(data);
    observer->OnApplicationStateChanged(data);
 
    data.pid = 999;
    data.bundleName = "";
    observer->OnForegroundApplicationChanged(data);
    observer->OnApplicationStateChanged(data);
    sleep(1);
    SUCCEED();
}
 
/**
 * @tc.name: RmsApplicationStateObserver_002
 * @tc.desc: test for OnAbilityStateChanged, OnExtensionStateChanged
 * @tc.type: FUNC
 * @tc.require:
 * @tc.desc:
 */
HWTEST_F(ObserverEventTest, RmsApplicationStateObserver_002, testing::ext::TestSize.Level1)
{
    auto observer = std::make_shared<RmsApplicationStateObserver>();
    EXPECT_TRUE(observer != nullptr);
    AppStartupSceneRec::GetInstance().Init();
 
    AppExecFwk::AbilityStateData data;
    data.moduleName = "entry";
    data.bundleName = "com.ohos.test";
    data.abilityName = "MainAbility";
    data.abilityState = 0;
    data.pid = 999;
    data.uid = 1000;
    data.abilityRecordId = 111;
    data.abilityType = 0;
    data.isFocused = false;
    data.token = new RmsApplicationStateObserver();
    observer->OnAbilityStateChanged(data);
    observer->OnExtensionStateChanged(data);
    data.bundleName = "";
    data.uid = -1;
    observer->OnAbilityStateChanged(data);
    observer->OnExtensionStateChanged(data);
    sleep(1);
    SUCCEED();
}
 
/**
 * @tc.name: RmsApplicationStateObserver_003
 * @tc.desc: test for OnProcessCreated, OnProcessDied
 * @tc.type: FUNC
 * @tc.require:
 * @tc.desc:
 */
HWTEST_F(ObserverEventTest, RmsApplicationStateObserver_003, testing::ext::TestSize.Level1)
{
    auto observer = std::make_shared<RmsApplicationStateObserver>();
    EXPECT_TRUE(observer != nullptr);
    AppExecFwk::ProcessData data;
    data.bundleName = "com.ohos.test";
    data.pid = 999;
    data.uid = 1000;
    data.state = AppExecFwk::AppProcessState::APP_STATE_CREATE;
    data.isContinuousTask = false;
    data.isKeepAlive = false;
    data.isFocused = false;
 
    observer->OnProcessCreated(data);
    observer->OnProcessDied(data);
    data.bundleName = "";
    data.uid = -1;
    observer->OnProcessCreated(data);
    observer->OnProcessDied(data);
    sleep(1);
    SUCCEED();
}
 
/**
 * @tc.name: RmsApplicationStateObserver_004
 * @tc.desc: test for OnAppStateChanged, OnAppCacheStateChanged
 * @tc.type: FUNC
 * @tc.require:
 * @tc.desc:
 */
HWTEST_F(ObserverEventTest, RmsApplicationStateObserver_004, testing::ext::TestSize.Level1)
{
    auto observer = std::make_shared<RmsApplicationStateObserver>();
    EXPECT_TRUE(observer != nullptr);
    AppStateData appStateData;
    appStateData.pid = 100;
    appStateData.uid = 1000;
    appStateData.bundleName = "test";
    appStateData.extensionType = AppExecFwk::ExtensionAbilityType::UNSPECIFIED;
    observer->OnAppStateChanged(appStateData);
    observer->OnAppCacheStateChanged(appStateData);
    appStateData.uid = -1;
    observer->OnAppStateChanged(appStateData);
    observer->OnAppCacheStateChanged(appStateData);
 
    ProcessData processData;
    processData.pid = 100;
    processData.uid = 1000;
    processData.bundleName = "test";
    observer->OnProcessStateChanged(processData);
    processData.pid = -1;
    observer->OnProcessStateChanged(processData);
    SUCCEED();
}

/**
 * @tc.name: RmsApplicationStateObserver_005
 * @tc.desc: test for OnProcessPreForegroundChanged
 * @tc.type: FUNC
 */
HWTEST_F(ObserverEventTest, RmsApplicationStateObserver_005, testing::ext::TestSize.Level1)
{
    auto observer = std::make_shared<RmsApplicationStateObserver>();
    EXPECT_TRUE(observer != nullptr);
    PreloadProcessData processData;
    observer->OnProcessPreForegroundChanged(processData);
    processData.isPreForeground = true;
    processData.pid = 100;
    processData.uid = 1000;
    processData.bundleName = "test";
    observer->OnProcessPreForegroundChanged(processData);
}

/**
 * @tc.name: WindowStateObserver_001
 * @tc.desc: test for OnFocused, OnUnfocused
 * @tc.type: FUNC
 * @tc.require:
 * @tc.desc:
 */
HWTEST_F(ObserverEventTest, WindowStateObserver_001, testing::ext::TestSize.Level1)
{
    auto observer = std::make_shared<WindowStateObserver>();
    EXPECT_TRUE(observer != nullptr);
    sptr<Rosen::FocusChangeInfo> info = new Rosen::FocusChangeInfo();
    info->uid_ = 1000;
    info->pid_ = 999;
    observer->OnFocused(info);
    observer->OnUnfocused(info);
    observer->OnFocused(nullptr);
    observer->OnUnfocused(nullptr);
    auto windowModeObserver = std::make_shared<WindowModeObserver>();
    windowModeObserver->OnWindowModeUpdate(Rosen::WindowModeType::WINDOW_MODE_SPLIT);
    windowModeObserver->OnWindowModeUpdate(Rosen::WindowModeType::WINDOW_MODE_FLOATING);
    windowModeObserver->OnWindowModeUpdate(Rosen::WindowModeType::WINDOW_MODE_FULLSCREEN_FLOATING);
    windowModeObserver->OnWindowModeUpdate(Rosen::WindowModeType::WINDOW_MODE_FULLSCREEN);
    windowModeObserver->OnWindowModeUpdate(Rosen::WindowModeType::WINDOW_MODE_SPLIT_FLOATING);
    windowModeObserver->OnWindowModeUpdate(Rosen::WindowModeType::WINDOW_MODE_OTHER);
    auto pipStateObserver = std::make_shared<PiPStateObserver>();
    pipStateObserver->OnPiPStateChanged("test", false);
    sleep(1);
    SUCCEED();
}
 
/**
 * @tc.name: WindowStateObserver_002
 * @tc.desc: test for OnFocused, OnUnfocused
 * @tc.type: FUNC
 * @tc.require:
 * @tc.desc:
 */
HWTEST_F(ObserverEventTest, WindowStateObserver_002, testing::ext::TestSize.Level1)
{
    auto observer = std::make_shared<WindowVisibilityObserver>();
    EXPECT_TRUE(observer != nullptr);
    std::vector<sptr<Rosen::WindowVisibilityInfo> > windowVisibilityInfo;
    for (int32_t i = 0; i < 2; i++) {
        sptr<Rosen::WindowVisibilityInfo> info = new Rosen::WindowVisibilityInfo();
        windowVisibilityInfo.push_back(info);
    }
    windowVisibilityInfo.push_back(nullptr);
    observer->OnWindowVisibilityChanged(windowVisibilityInfo);
    SUCCEED();
}
 
/**
 * @tc.name: WindowStateObserver_003
 * @tc.desc: test for OnWindowModeUpdate
 * @tc.type: FUNC
 * @tc.require:
 * @tc.desc:
 */
HWTEST_F(ObserverEventTest, WindowStateObserver_003, testing::ext::TestSize.Level1)
{
    auto observer = std::make_shared<WindowModeObserver>();
    EXPECT_TRUE(observer != nullptr);
    observer->OnWindowModeUpdate(WindowModeType::WINDOW_MODE_SPLIT);
    sleep(1);
    SUCCEED();
}
/**
 * @tc.name: WindowStateObserver_004
 * @tc.desc: test for OnPiPStateChanged
 * @tc.type: FUNC
 * @tc.require:
 * @tc.desc:
 */
HWTEST_F(ObserverEventTest, WindowStateObserver_004, testing::ext::TestSize.Level1)
{
    auto observer = std::make_shared<PiPStateObserver>();
    EXPECT_TRUE(observer != nullptr);
    observer->OnPiPStateChanged("bundleName", false);
    sleep(1);
    SUCCEED();
}

/**
 * @tc.name: WindowDrawingContentObserver_001
 * @tc.desc: test for OnWindowDrawingContentChanged
 * @tc.type: FUNC
 * @tc.require:
 * @tc.desc:
 */
HWTEST_F(ObserverEventTest, WindowDrawingContentObserver_001, testing::ext::TestSize.Level1)
{
    auto observer = std::make_shared<WindowDrawingContentObserver>();
    EXPECT_TRUE(observer != nullptr);
 
    std::vector<sptr<WindowDrawingContentInfo>> changeInfo;
    OHOS::sptr<OHOS::Rosen::WindowDrawingContentInfo> info = new OHOS::Rosen::WindowDrawingContentInfo();
    changeInfo.push_back(info);
    observer->OnWindowDrawingContentChanged(changeInfo);
}

/**
 * @tc.name: AppStartupSceneRec_001
 * @tc.desc: test for RecordIsContinuousStartup
 * @tc.type: FUNC
 * @tc.require:
 * @tc.desc:
 */
HWTEST_F(ObserverEventTest, AppStartupSceneRec_001, testing::ext::TestSize.Level1)
{
    auto observer = std::make_shared<AppStartupSceneRec>();
    EXPECT_TRUE(observer != nullptr);
    observer->Init();
    observer->RecordIsContinuousStartup("01", "TEST");
    sleep(1);
    SUCCEED();
}

/**
 * @tc.name: AppStartupSceneRec_002
 * @tc.desc: test for IsAppStartUp
 * @tc.type: FUNC
 * @tc.require:
 * @tc.desc:
 */
HWTEST_F(ObserverEventTest, AppStartupSceneRec_002, testing::ext::TestSize.Level1)
{
    auto observer = std::make_shared<AppStartupSceneRec>();
    EXPECT_TRUE(observer != nullptr);
    observer->Init();
    EXPECT_TRUE(observer->IsAppStartUp(0));
    EXPECT_FALSE(observer->IsAppStartUp(1));
    SUCCEED();
}

/**
 * @tc.name: BackgroundTaskObserver_001
 * @tc.desc: test for BackgroundTaskObserver
 * @tc.type: FUNC
 * @tc.require:
 * @tc.desc:
 */
HWTEST_F(ObserverEventTest, BackgroundTaskObserver_001, testing::ext::TestSize.Level1)
{
    auto observer = std::make_shared<BackgroundTaskObserver>();
    EXPECT_TRUE(observer != nullptr);
    std::shared_ptr<TransientTaskAppInfo> info = std::make_shared<TransientTaskAppInfo>("test", 1000, 1000);
    observer->OnTransientTaskStart(info);
    observer->OnTransientTaskEnd(info);
    observer->OnTransientTaskErr(info);
    observer->OnAppTransientTaskStart(info);
    observer->OnAppTransientTaskEnd(info);
    std::vector<uint32_t> typeIds = { 1 };
    std::shared_ptr<ContinuousTaskCallbackInfo> info2 = std::make_shared<ContinuousTaskCallbackInfo>(1,
        1000, 1000, "test", false, false, typeIds, -1, 0);
    observer->OnContinuousTaskStart(info2);
    observer->OnContinuousTaskStop(info2);
    observer->OnContinuousTaskUpdate(info2);
    std::shared_ptr<BackgroundTaskMgr::ResourceCallbackInfo> callbackInfo =
        std::make_shared<BackgroundTaskMgr::ResourceCallbackInfo>(1000, 1000, 1, "test");
    observer->OnAppEfficiencyResourcesApply(callbackInfo);
    observer->OnAppEfficiencyResourcesReset(callbackInfo);
    observer->OnProcEfficiencyResourcesApply(callbackInfo);
    observer->OnProcEfficiencyResourcesReset(callbackInfo);
}

/**
 * @tc.name: BackgroundTaskObserver_002
 * @tc.desc: test for OnAppTransientTaskStart,OnAppTransientTaskEnd
 * @tc.type: FUNC
 * @tc.require:
 * @tc.desc:
 */
HWTEST_F(ObserverEventTest, BackgroundTaskObserver_002, testing::ext::TestSize.Level1)
{
    auto observer = std::make_shared<BackgroundTaskObserver>();
    auto info = std::make_shared<TransientTaskAppInfo>();
    EXPECT_TRUE(observer != nullptr);
    EXPECT_TRUE(info != nullptr);
    observer->OnAppTransientTaskStart(info);
    observer->OnAppTransientTaskEnd(info);
    SUCCEED();
}

/**
 * @tc.name: BackgroundTaskObserver_003
 * @tc.desc: test for ,OnContinuousTaskStart,OnContinuousTaskStop,OnContinuousTaskUpdate
 * @tc.type: FUNC
 * @tc.require:
 * @tc.desc:
 */
HWTEST_F(ObserverEventTest, BackgroundTaskObserver_003, testing::ext::TestSize.Level1)
{
    auto observer = std::make_shared<BackgroundTaskObserver>();
    auto info = std::make_shared<ContinuousTaskCallbackInfo>();
    EXPECT_TRUE(observer != nullptr);
    EXPECT_TRUE(info != nullptr);
    observer->OnContinuousTaskStart(info);
    observer->OnContinuousTaskStop(info);
    observer->OnContinuousTaskUpdate(info);
    SUCCEED();
}

/**
 * @tc.name: BackgroundTaskObserver_004
 * @tc.desc: test for ,OnContinuousTaskStart,OnContinuousTaskStop,OnContinuousTaskUpdate
 * @tc.type: FUNC
 * @tc.require:
 * @tc.desc:
 */
HWTEST_F(ObserverEventTest, BackgroundTaskObserver_004, testing::ext::TestSize.Level1)
{
    auto observer = std::make_shared<BackgroundTaskObserver>();
    EXPECT_TRUE(observer != nullptr);
    const sptr<IRemoteObject> sptrDeath = nullptr;
    wptr<IRemoteObject> wptrDeath = sptrDeath;
    observer->OnRemoteDied(wptrDeath);
    SUCCEED();
}

/**
 * @tc.name: BackgroundTaskObserver_005
 * @tc.desc: test for OnAppEfficiencyResourcesApply,OnAppEfficiencyResourcesReset
 * @tc.type: FUNC
 * @tc.require:
 * @tc.desc:
 */
HWTEST_F(ObserverEventTest, BackgroundTaskObserver_005, testing::ext::TestSize.Level1)
{
    auto observer = std::make_shared<BackgroundTaskObserver>();
    auto info = std::make_shared<ResourceCallbackInfo>();
    EXPECT_TRUE(observer != nullptr);
    EXPECT_TRUE(info != nullptr);
    observer->OnAppEfficiencyResourcesApply(info);
    observer->OnAppEfficiencyResourcesReset(info);
    SUCCEED();
}

/**
 * @tc.name: BackgroundTaskObserver_006
 * @tc.desc: test for OnAppEfficiencyResourcesApply,OnAppEfficiencyResourcesReset
 * @tc.type: FUNC
 * @tc.require:
 * @tc.desc:
 */
HWTEST_F(ObserverEventTest, BackgroundTaskObserver_006, testing::ext::TestSize.Level1)
{
    auto observer = std::make_shared<BackgroundTaskObserver>();
    auto info = std::make_shared<ResourceCallbackInfo>();
    EXPECT_TRUE(observer != nullptr);
    EXPECT_TRUE(info != nullptr);
    observer->OnProcEfficiencyResourcesApply(info);
    observer->OnProcEfficiencyResourcesReset(info);
    SUCCEED();
}

/**
 * @tc.name: BackgroundTaskObserver_007
 * @tc.desc: test for OnTransientTaskStart,OnTransientTaskEnd,OnTransientTaskErr
 * @tc.type: FUNC
 * @tc.require:
 * @tc.desc:
 */
HWTEST_F(ObserverEventTest, BackgroundTaskObserver_007, testing::ext::TestSize.Level1)
{
    auto observer = std::make_shared<BackgroundTaskObserver>();
    auto info = std::make_shared<TransientTaskAppInfo>();
    EXPECT_TRUE(observer != nullptr);
    EXPECT_TRUE(info != nullptr);
    observer->OnTransientTaskStart(info);
    observer->OnTransientTaskEnd(info);
    observer->OnTransientTaskErr(info);
    SUCCEED();
}

/**
 * @tc.name: AppStateObserver_001
 * @tc.desc: test for OnForegroundApplicationChanged
 * @tc.type: FUNC
 * @tc.require:
 * @tc.desc:
 */
HWTEST_F(ObserverEventTest, AppStateObserver_001, testing::ext::TestSize.Level1)
{
    auto observer = std::make_shared<RmsApplicationStateObserver>();
    EXPECT_TRUE(observer != nullptr);
    AppExecFwk::AppStateData data;
    data.bundleName = "com.ohos.test";
    data.pid = 999;
    data.uid = 1000;
    data.state = 0;
    data.accessTokenId = 0;
    data.isFocused = false;
    observer->OnForegroundApplicationChanged(data);
    SUCCEED();
}

 /**
 * @tc.name: AppStateObserver_002
 * @tc.desc: test for OnAbilityStateChanged
 * @tc.type: FUNC
 * @tc.require:
 * @tc.desc:
 */
HWTEST_F(ObserverEventTest, AppStateObserver_002, testing::ext::TestSize.Level1)
{
    auto observer = std::make_shared<RmsApplicationStateObserver>();
    EXPECT_TRUE(observer != nullptr);
    AppExecFwk::AbilityStateData data;
    data.moduleName = "entry";
    data.bundleName = "com.ohos.test";
    data.abilityName = "MainAbility";
    data.abilityState = 0;
    data.pid = 999;
    data.uid = 1000;
    data.abilityRecordId = 111;
    data.abilityType = 0;
    data.isFocused = false;
    observer->OnAbilityStateChanged(data);
    SUCCEED();
}

 /**
 * @tc.name: AppStateObserver_003
 * @tc.desc: test for OnExtensionStateChanged
 * @tc.type: FUNC
 * @tc.require:
 * @tc.desc:
 */
HWTEST_F(ObserverEventTest, AppStateObserver_003, testing::ext::TestSize.Level1)
{
    auto observer = std::make_shared<RmsApplicationStateObserver>();
    EXPECT_TRUE(observer != nullptr);
    AppExecFwk::AbilityStateData data;
    data.moduleName = "entry";
    data.bundleName = "com.ohos.test";
    data.abilityName = "MainAbility";
    data.abilityState = 0;
    data.pid = 999;
    data.uid = 1000;
    data.abilityRecordId = 111;
    data.abilityType = 0;
    data.isFocused = false;
    observer->OnExtensionStateChanged(data);
    SUCCEED();
}

 /**
 * @tc.name: AppStateObserver_004
 * @tc.desc: test for OnProcessCreated
 * @tc.type: FUNC
 * @tc.require:
 * @tc.desc:
 */
HWTEST_F(ObserverEventTest, AppStateObserver_004, testing::ext::TestSize.Level1)
{
    auto observer = std::make_shared<RmsApplicationStateObserver>();
    EXPECT_TRUE(observer != nullptr);
    AppExecFwk::ProcessData data;
    data.bundleName = "com.ohos.test";
    data.pid = 999;
    data.uid = 1000;
    data.state = AppExecFwk::AppProcessState::APP_STATE_CREATE;
    data.isContinuousTask = false;
    data.isKeepAlive = false;
    data.isFocused = false;
    observer->OnProcessCreated(data);
    SUCCEED();
}

 /**
 * @tc.name: AppStateObserver_005
 * @tc.desc: test for OnProcessDied
 * @tc.type: FUNC
 * @tc.require:
 * @tc.desc:
 */
HWTEST_F(ObserverEventTest, AppStateObserver_005, testing::ext::TestSize.Level1)
{
    auto observer = std::make_shared<RmsApplicationStateObserver>();
    EXPECT_TRUE(observer != nullptr);
    AppExecFwk::ProcessData data;
    data.bundleName = "com.ohos.test";
    data.pid = 999;
    data.uid = 1000;
    data.state = AppExecFwk::AppProcessState::APP_STATE_CREATE;
    data.isContinuousTask = false;
    data.isKeepAlive = false;
    data.isFocused = false;
    observer->OnProcessDied(data);
    SUCCEED();
}

/**
 * @tc.name: AppStateObserver_006
 * @tc.desc: test for OnApplicationStateChanged
 * @tc.type: FUNC
 * @tc.require:
 * @tc.desc:
 */
HWTEST_F(ObserverEventTest, AppStateObserver_006, testing::ext::TestSize.Level1)
{
    auto observer = std::make_shared<RmsApplicationStateObserver>();
    EXPECT_TRUE(observer != nullptr);
    AppStateData appStateData;
    appStateData.pid = 100;
    appStateData.uid = 1000;
    appStateData.bundleName = "test";
    appStateData.extensionType = AppExecFwk::ExtensionAbilityType::UNSPECIFIED;
    observer->OnApplicationStateChanged(appStateData);
    SUCCEED();
}

/**
 * @tc.name: AppStateObserver_007
 * @tc.desc: test for OnProcessStateChanged
 * @tc.type: FUNC
 * @tc.require:
 * @tc.desc:
 */
HWTEST_F(ObserverEventTest, AppStateObserver_007, testing::ext::TestSize.Level1)
{
    auto observer = std::make_shared<RmsApplicationStateObserver>();
    EXPECT_TRUE(observer != nullptr);
    AppExecFwk::ProcessData data;
    data.bundleName = "com.ohos.test";
    data.pid = 999;
    data.uid = 1000;
    data.state = AppExecFwk::AppProcessState::APP_STATE_CREATE;
    data.isContinuousTask = false;
    data.isKeepAlive = false;
    data.isFocused = false;
    observer->OnProcessStateChanged(data);
    SUCCEED();
}

/**
 * @tc.name: AppStateObserver_008
 * @tc.desc: test for OnAppStateChanged
 * @tc.type: FUNC
 * @tc.require:
 * @tc.desc:
 */
HWTEST_F(ObserverEventTest, AppStateObserver_008, testing::ext::TestSize.Level1)
{
    auto observer = std::make_shared<RmsApplicationStateObserver>();
    EXPECT_TRUE(observer != nullptr);
    AppStateData appStateData;
    appStateData.pid = 100;
    appStateData.uid = 1000;
    appStateData.bundleName = "test";
    appStateData.extensionType = AppExecFwk::ExtensionAbilityType::UNSPECIFIED;
    observer->OnAppStateChanged(appStateData);
    SUCCEED();
}

/**
 * @tc.name: AppStateObserver_009
 * @tc.desc: test for OnAppCacheStateChanged
 * @tc.type: FUNC
 * @tc.require:
 * @tc.desc:
 */
HWTEST_F(ObserverEventTest, AppStateObserver_009, testing::ext::TestSize.Level1)
{
    auto observer = std::make_shared<RmsApplicationStateObserver>();
    EXPECT_TRUE(observer != nullptr);
    AppStateData appStateData;
    appStateData.pid = 100;
    appStateData.uid = 1000;
    appStateData.bundleName = "test";
    appStateData.extensionType = AppExecFwk::ExtensionAbilityType::UNSPECIFIED;
    observer->OnAppCacheStateChanged(appStateData);
    SUCCEED();
}

/**
 * @tc.name: AppStateObserver_010
 * @tc.desc: test for OnProcessPreForegroundChanged
 * @tc.type: FUNC
 * @tc.require:
 * @tc.desc:
 */
HWTEST_F(ObserverEventTest, AppStateObserver_010, testing::ext::TestSize.Level1)
{
    auto observer = std::make_shared<RmsApplicationStateObserver>();
    EXPECT_TRUE(observer != nullptr);
    PreloadProcessData processData;
    observer->OnProcessPreForegroundChanged(processData);
    processData.isPreForeground = true;
    processData.pid = 100;
    processData.uid = 1000;
    processData.bundleName = "test";
    observer->OnProcessPreForegroundChanged(processData);
    SUCCEED();
}

/**
 * @tc.name: AppStateObserver_011
 * @tc.desc: test for OnAppStopped
 * @tc.type: FUNC
 * @tc.require:
 * @tc.desc:
 */
HWTEST_F(ObserverEventTest, AppStateObserver_011, testing::ext::TestSize.Level1)
{
    auto observer = std::make_shared<RmsApplicationStateObserver>();
    EXPECT_TRUE(observer != nullptr);
    AppStateData appStateData;
    appStateData.pid = 100;
    appStateData.uid = 1000;
    appStateData.bundleName = "test";
    appStateData.extensionType = AppExecFwk::ExtensionAbilityType::UNSPECIFIED;
    observer->OnAppStopped(appStateData);
    SUCCEED();
}
}
}