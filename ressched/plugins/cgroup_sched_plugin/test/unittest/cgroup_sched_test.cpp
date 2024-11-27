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

#include <dlfcn.h>
#include <gtest/gtest.h>
#include <iostream>
#include <memory>
#include "app_startup_scene_rec.h"
#include "cgroup_sched_common.h"
#include "res_type.h"
#include "cgroup_event_handler.h"
#include "sched_controller.h"
#include "sched_policy.h"
#include "supervisor.h"
#include "window_state_observer.h"
#include "window_manager.h"
#include "wm_common.h"
#include "ability_info.h"
#include "background_mode.h"
#include "appmgr/app_mgr_constants.h"

using namespace testing::ext;
using namespace OHOS::ResourceSchedule::CgroupSetting;
using OHOS::Rosen::WindowModeType;
using OHOS::Rosen::IWindowModeChangedListener;

namespace OHOS {
namespace ResourceSchedule {
namespace {
    static const int32_t APP_START_UP = 0;
    constexpr int32_t INVALID_EXTENSION_TYPE = -1;
}
namespace CgroupSetting {
class CGroupSchedTest : public testing::Test {
public:
    std::shared_ptr<Supervisor> supervisor_;
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void CGroupSchedTest::SetUpTestCase(void)
{
}

void CGroupSchedTest::TearDownTestCase(void)
{
}

void CGroupSchedTest::SetUp(void)
{
    AppStartupSceneRec::GetInstance().Init();
    supervisor_ = std::make_shared<Supervisor>();
}

void CGroupSchedTest::TearDown(void)
{
    AppStartupSceneRec::GetInstance().Deinit();
    supervisor_ = nullptr;
}

/**
 * @tc.name: CGroupSchedTest_WindowStateObserver_001
 * @tc.desc: Window Mode Observer Test
 * @tc.type: FUNC
 * @tc.require: issuesI9BU37
 * @tc.desc:
 */
HWTEST_F(CGroupSchedTest, CGroupSchedTest_WindowStateObserver_001, Function | MediumTest | Level1)
{
    std::shared_ptr<WindowModeObserver> windowModeObserver_ =
        std::make_shared<WindowModeObserver>();
    windowModeObserver_->OnWindowModeUpdate(Rosen::WindowModeType::WINDOW_MODE_SPLIT);
    EXPECT_NE(windowModeObserver_, nullptr);

    windowModeObserver_->OnWindowModeUpdate(Rosen::WindowModeType::WINDOW_MODE_FLOATING);
    EXPECT_NE(windowModeObserver_, nullptr);

    windowModeObserver_->OnWindowModeUpdate(Rosen::WindowModeType::WINDOW_MODE_SPLIT);
    EXPECT_NE(windowModeObserver_, nullptr);

    windowModeObserver_->OnWindowModeUpdate(Rosen::WindowModeType::WINDOW_MODE_SPLIT_FLOATING);
    EXPECT_NE(windowModeObserver_, nullptr);

    windowModeObserver_->OnWindowModeUpdate(Rosen::WindowModeType::WINDOW_MODE_SPLIT);
    EXPECT_NE(windowModeObserver_, nullptr);

    windowModeObserver_->OnWindowModeUpdate(Rosen::WindowModeType::WINDOW_MODE_OTHER);
    EXPECT_NE(windowModeObserver_, nullptr);

    windowModeObserver_->OnWindowModeUpdate(Rosen::WindowModeType::WINDOW_MODE_FLOATING);
    EXPECT_NE(windowModeObserver_, nullptr);

    windowModeObserver_->OnWindowModeUpdate(Rosen::WindowModeType::WINDOW_MODE_SPLIT_FLOATING);
    EXPECT_NE(windowModeObserver_, nullptr);

    windowModeObserver_->OnWindowModeUpdate(Rosen::WindowModeType::WINDOW_MODE_FLOATING);
    EXPECT_NE(windowModeObserver_, nullptr);

    windowModeObserver_->OnWindowModeUpdate(Rosen::WindowModeType::WINDOW_MODE_OTHER);
    EXPECT_NE(windowModeObserver_, nullptr);

    windowModeObserver_->OnWindowModeUpdate(Rosen::WindowModeType::WINDOW_MODE_SPLIT_FLOATING);
    EXPECT_NE(windowModeObserver_, nullptr);

    windowModeObserver_->OnWindowModeUpdate(Rosen::WindowModeType::WINDOW_MODE_OTHER);
    EXPECT_NE(windowModeObserver_, nullptr);

    windowModeObserver_->OnWindowModeUpdate(Rosen::WindowModeType::WINDOW_MODE_SPLIT);
    EXPECT_NE(windowModeObserver_, nullptr);

    windowModeObserver_->OnWindowModeUpdate(Rosen::WindowModeType::WINDOW_MODE_SPLIT);

    windowModeObserver_ = nullptr;
}

/**
 * @tc.name: CGroupSchedTest_WindowStateObserver_002
 * @tc.desc: Window Mode Observer Test
 * @tc.type: FUNC
 * @tc.require: issuesI9BU37
 * @tc.desc:
 */
HWTEST_F(CGroupSchedTest, CGroupSchedTest_WindowStateObserver_002, Function | MediumTest | Level1)
{
    std::shared_ptr<WindowModeObserver> windowModeObserver_ =
        std::make_shared<WindowModeObserver>();
    uint8_t nowWindowMode = windowModeObserver_->MarshallingWindowModeType(
        Rosen::WindowModeType::WINDOW_MODE_SPLIT_FLOATING);
    EXPECT_EQ(nowWindowMode, RSSWindowMode::WINDOW_MODE_SPLIT_FLOATING);

    nowWindowMode = windowModeObserver_->MarshallingWindowModeType(Rosen::WindowModeType::WINDOW_MODE_SPLIT);
    EXPECT_EQ(nowWindowMode, RSSWindowMode::WINDOW_MODE_SPLIT);

    nowWindowMode = windowModeObserver_->MarshallingWindowModeType(Rosen::WindowModeType::WINDOW_MODE_FLOATING);
    EXPECT_EQ(nowWindowMode, RSSWindowMode::WINDOW_MODE_FLOATING);
    nowWindowMode = windowModeObserver_->MarshallingWindowModeType(
        Rosen::WindowModeType::WINDOW_MODE_FULLSCREEN_FLOATING);
    EXPECT_EQ(nowWindowMode, RSSWindowMode::WINDOW_MODE_FLOATING);

    nowWindowMode = windowModeObserver_->MarshallingWindowModeType(Rosen::WindowModeType::WINDOW_MODE_OTHER);
    EXPECT_EQ(nowWindowMode, RSSWindowMode::WINDOW_MODE_OTHER);
    windowModeObserver_ = nullptr;
}

/**
 * @tc.name: CGroupSchedTest_CgroupSchedLog_001
 * @tc.desc: Window Mode Observer Test
 * @tc.type: FUNC
 * @tc.require: issuesI9BU37
 * @tc.desc:
 */
HWTEST_F(CGroupSchedTest, CGroupSchedTest_CgroupSchedLog_001, Function | MediumTest | Level1)
{
    CgroupSchedLog::level_ = LOG_DEBUG;
    bool ret = CgroupSchedLog::JudgeLevel(LOG_INFO);
    EXPECT_TRUE(ret);
    CgroupSchedLog::level_ = LOG_INFO;
    ret = CgroupSchedLog::JudgeLevel(LOG_DEBUG);
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: CGroupSchedTest_WindowStateObserver_002
 * @tc.desc: Window Mode Observer Test
 * @tc.type: FUNC
 * @tc.require: issuesI9BU37
 * @tc.desc:
 */
HWTEST_F(CGroupSchedTest, CGroupSchedTest_SchedController_001, Function | MediumTest | Level1)
{
    auto &schedController = SchedController::GetInstance();
    EXPECT_EQ(nullptr, schedController.windowStateObserver_);
    schedController.SubscribeWindowState();
    schedController.UnsubscribeWindowState();
    EXPECT_EQ(nullptr, schedController.windowStateObserver_);
}

/**
 * @tc.name: CGroupSchedTest_schedController_002
 * @tc.desc: schedController dump test
 * @tc.type: FUNC
 * @tc.require: issuesI9BU37
 * @tc.desc:
 */
HWTEST_F(CGroupSchedTest, CGroupSchedTest_SchedController_002, Function | MediumTest | Level1)
{
    auto &schedController = SchedController::GetInstance();
    schedController.Init();
    auto processRecord = schedController.GetSupervisor()->GetAppRecordNonNull(1000)->GetProcessRecordNonNull(1000);
    processRecord->runningLockState_[0] = ResType::RunninglockState::RUNNINGLOCK_STATE_ENABLE;
    processRecord->GetWindowInfoNonNull(0);
    std::vector<std::string> args = {};
    std::string result;
    schedController.Dump(args, result);

    args = { "-h" };
    result = "";
    schedController.Dump(args, result);
    EXPECT_NE(result.find("show the cgroup_sched_plugin help"), std::string::npos);

    result = "";
    args = { "-getRunningLockInfo" };
    schedController.Dump(args, result);
    EXPECT_NE(result.find("lockType"), std::string::npos);

    result = "";
    args = { "-getProcessEventInfo" };
    schedController.Dump(args, result);
    EXPECT_NE(result.find("processState"), std::string::npos);

    result = "";
    args = { "-getProcessWindowInfo" };
    schedController.Dump(args, result);
    EXPECT_NE(result.find("windowInfo"), std::string::npos);

    result = "";
    args = { "-invalid" };
    schedController.Dump(args, result);
    EXPECT_EQ(result, "");

    result = "";
    args = { "-invalid", "-error" };
    schedController.Dump(args, result);
    EXPECT_EQ(result, "");
}

/**
 * @tc.name: CGroupSchedTest_schedController_003
 * @tc.desc: schedController dlopen function test
 * @tc.type: FUNC
 * @tc.require: issuesI9BU37
 * @tc.desc:
 */
HWTEST_F(CGroupSchedTest, CGroupSchedTest_SchedController_003, Function | MediumTest | Level1)
{
    using OnPluginInitFunc = bool (*)(std::string& libName);
    using OnPluginDisableFunc = void (*)();
    using OnDispatchResourceFunc = void (*)(const std::shared_ptr<ResData>& data);
    using GetProcessGroupFunc = int (*)(const pid_t pid);
    using OnDumpFunc = void (*)(const std::vector<std::string>& args, std::string& result);
    std::string cgroupLibName = "libcgroup_sched.z.so";
    std::string errorLibName = "error.z.so";
    auto handle = dlopen(cgroupLibName.c_str(), RTLD_NOW);
    EXPECT_NE(handle, nullptr);
    OnPluginInitFunc onPluginInitFunc = reinterpret_cast<OnPluginInitFunc>(dlsym(handle, "OnPluginInit"));
    EXPECT_NE(onPluginInitFunc, nullptr);
    EXPECT_TRUE(onPluginInitFunc(cgroupLibName));
    EXPECT_FALSE(onPluginInitFunc(errorLibName));

    OnDispatchResourceFunc onDispatchResourceFunc = reinterpret_cast<OnDispatchResourceFunc>(dlsym(handle,
        "OnDispatchResource"));
    EXPECT_NE(onDispatchResourceFunc, nullptr);
    const std::shared_ptr<ResData>& data = std::make_shared<ResData>(ResType::RES_TYPE_APP_ABILITY_START,
        ResType::AppStartType::APP_COLD_START, nullptr);
    onDispatchResourceFunc(data);
    const std::shared_ptr<ResData>& dataInvalid = std::make_shared<ResData>(-1, -1, nullptr);
    onDispatchResourceFunc(dataInvalid);

    GetProcessGroupFunc getProcessGroupFunc = reinterpret_cast<GetProcessGroupFunc>(dlsym(handle, "GetProcessGroup"));
    EXPECT_NE(getProcessGroupFunc, nullptr);
    getProcessGroupFunc(1000);

    std::vector<std::string> args = {};
    std::string result;
    OnDumpFunc onDumpFunc = reinterpret_cast<OnDumpFunc>(dlsym(handle, "OnDump"));
    EXPECT_NE(onDumpFunc, nullptr);
    onDumpFunc(args, result);

    OnPluginDisableFunc onPluginDisableFunc = reinterpret_cast<OnPluginDisableFunc>(dlsym(handle, "OnPluginDisable"));
    EXPECT_NE(onPluginDisableFunc, nullptr);
    onPluginDisableFunc();
    SUCCEED();
}

/**
 * @tc.name: CGroupSchedTest_WindowStateObserver_002
 * @tc.desc: Window Mode Observer Test
 * @tc.type: FUNC
 * @tc.require: issuesI9BU37
 * @tc.desc:
 */
HWTEST_F(CGroupSchedTest, CGroupSchedTest_SchedController_004, Function | MediumTest | Level1)
{
    const std::shared_ptr<ResData>& data = std::make_shared<ResData>(ResType::RES_TYPE_APP_ABILITY_START,
        ResType::AppStartType::APP_COLD_START, nullptr);
    auto &schedController = SchedController::GetInstance();
    schedController.DispatchResource(data);
    schedController.DispatchOtherResource(ResType::RES_TYPE_REPORT_SCREEN_CAPTURE,
        ResType::AppStartType::APP_COLD_START, nullptr);
    schedController.DispatchOtherResource(ResType::RES_TYPE_AV_CODEC_STATE,
        ResType::AppStartType::APP_COLD_START, nullptr);
    schedController.Init();
    auto processRecord = schedController.GetSupervisor()->GetAppRecordNonNull(1000)->GetProcessRecordNonNull(1000);
    processRecord->runningLockState_[0] = ResType::RunninglockState::RUNNINGLOCK_STATE_ENABLE;
    processRecord->GetWindowInfoNonNull(0);
    EXPECT_EQ(schedController.GetProcessGroup(1000), SP_UPPER_LIMIT);
}

/**
 * @tc.name: CGroupSchedTest_AppStartupSceneRec_001
 * @tc.desc: application startup scene Test
 * @tc.type: FUNC
 * @tc.require: issuesI9IR2I
 * @tc.desc:
 */
HWTEST_F(CGroupSchedTest, CGroupSchedTest_AppStartupSceneRec_001, Function | MediumTest | Level1)
{
    AppStartupSceneRec::GetInstance().CleanRecordSceneData();
    std::string bundleName = "test101";
    std::string uid = "101";
    AppStartupSceneRec::GetInstance().RecordIsContinuousStartup(-1, uid, bundleName);
    AppStartupSceneRec::GetInstance().RecordIsContinuousStartup(APP_START_UP, uid, bundleName);
    bool isReportContinuousStartup = AppStartupSceneRec::GetInstance().isReportContinuousStartup_.load();
    EXPECT_EQ(isReportContinuousStartup, false);
    uid = "102";
    bundleName = "test102";
    AppStartupSceneRec::GetInstance().RecordIsContinuousStartup(APP_START_UP, uid, bundleName);
    isReportContinuousStartup = AppStartupSceneRec::GetInstance().isReportContinuousStartup_.load();
    EXPECT_EQ(isReportContinuousStartup, false);
    uid = "103";
    bundleName = "test103";
    AppStartupSceneRec::GetInstance().RecordIsContinuousStartup(APP_START_UP, uid, bundleName);
    isReportContinuousStartup = AppStartupSceneRec::GetInstance().isReportContinuousStartup_.load();
    EXPECT_EQ(isReportContinuousStartup, false);
    uid = "104";
    bundleName = "test104";
    AppStartupSceneRec::GetInstance().RecordIsContinuousStartup(APP_START_UP, uid, bundleName);
    isReportContinuousStartup = AppStartupSceneRec::GetInstance().isReportContinuousStartup_.load();
    EXPECT_EQ(isReportContinuousStartup, false);
    uid = "105";
    bundleName = "test105";
    AppStartupSceneRec::GetInstance().RecordIsContinuousStartup(APP_START_UP, uid, bundleName);
    isReportContinuousStartup = AppStartupSceneRec::GetInstance().isReportContinuousStartup_.load();
    EXPECT_EQ(isReportContinuousStartup, true);
    AppStartupSceneRec::GetInstance().RecordIsContinuousStartup(APP_START_UP, uid, bundleName);
    isReportContinuousStartup = AppStartupSceneRec::GetInstance().isReportContinuousStartup_.load();
    EXPECT_EQ(isReportContinuousStartup, true);
    uid = "106";
    bundleName = "test106";
    AppStartupSceneRec::GetInstance().RecordIsContinuousStartup(APP_START_UP, uid, bundleName);
    isReportContinuousStartup = AppStartupSceneRec::GetInstance().isReportContinuousStartup_.load();
    EXPECT_EQ(isReportContinuousStartup, true);
}

/**
 * @tc.name: CGroupSchedTest_AppStartupSceneRec_002
 * @tc.desc: application startup scene Test
 * @tc.type: FUNC
 * @tc.require: issuesI9IR2I
 * @tc.desc:
 */
HWTEST_F(CGroupSchedTest, CGroupSchedTest_AppStartupSceneRec_002, Function | MediumTest | Level1)
{
    AppStartupSceneRec::GetInstance().CleanRecordSceneData();
    bool isReportContinuousStartup = AppStartupSceneRec::GetInstance().isReportContinuousStartup_.load();
    EXPECT_EQ(isReportContinuousStartup, false);
    std::string bundleName = "test101";
    std::string uid = "101";
    AppStartupSceneRec::GetInstance().startIgnorePkgs_.emplace("test101");
    AppStartupSceneRec::GetInstance().RecordIsContinuousStartup(APP_START_UP, uid, bundleName);
    uid = "106";
    bundleName = "test106";
    AppStartupSceneRec::GetInstance().RecordIsContinuousStartup(APP_START_UP, uid, bundleName);
    isReportContinuousStartup = AppStartupSceneRec::GetInstance().isReportContinuousStartup_.load();
    EXPECT_EQ(isReportContinuousStartup, false);
    AppStartupSceneRec::GetInstance().RecordIsContinuousStartup(APP_START_UP, uid, bundleName);
    isReportContinuousStartup = AppStartupSceneRec::GetInstance().isReportContinuousStartup_.load();
    EXPECT_EQ(isReportContinuousStartup, false);
}

/**
 * @tc.name: CGroupSchedTest_HandleReportBluetoothConnectState_001
 * @tc.desc: cgroup event handler Test
 * @tc.type: FUNC
 * @tc.require: issuesIB0CYC
 * @tc.desc:
 */
HWTEST_F(CGroupSchedTest, CGroupSchedTest_HandleReportBluetoothConnectState_001, Function | MediumTest | Level1)
{
    auto cgroupEventHandler = std::make_shared<CgroupEventHandler>("CgroupEventHandler_unittest");
    cgroupEventHandler->SetSupervisor(supervisor_);
    EXPECT_NE(cgroupEventHandler->supervisor_, nullptr);
    uint32_t resType = 0;
    int64_t value = 1;
    nlohmann::json payload;
    payload["uid"] = "1000";
    payload["pid"] = "1000";
    auto processRecord = supervisor_->GetAppRecordNonNull(1000)->GetProcessRecordNonNull(1000);
    cgroupEventHandler->HandleReportBluetoothConnectState(resType, value, payload);
    EXPECT_EQ(processRecord->bluetoothState_, 1);
}

/**
 * @tc.name: CGroupSchedTest_HandleMmiInputState_001
 * @tc.desc: cgroup event handler Test
 * @tc.type: FUNC
 * @tc.require: issuesIB0CYC
 * @tc.desc:
 */
HWTEST_F(CGroupSchedTest, CGroupSchedTest_HandleMmiInputState_001, Function | MediumTest | Level1)
{
    auto cgroupEventHandler = std::make_shared<CgroupEventHandler>("CgroupEventHandler_unittest");
    cgroupEventHandler->SetSupervisor(supervisor_);
    EXPECT_NE(cgroupEventHandler->supervisor_, nullptr);
    uint32_t resType = 0;
    int64_t value = 0;
    nlohmann::json payload;
    payload["uid"] = "1000";
    payload["pid"] = "1000";
    payload["bundleName"] = "test";
    payload["syncStatus"] = "0";
    auto processRecord = supervisor_->GetAppRecordNonNull(1000)->GetProcessRecordNonNull(1000);
    cgroupEventHandler->HandleMmiInputState(resType, value, payload);
    EXPECT_EQ(processRecord->mmiStatus_, 0);
}

/**
 * @tc.name: CGroupSchedTest_CgroupEventHandler_001
 * @tc.desc: cgroup event handler Test
 * @tc.type: FUNC
 * @tc.require: issuesIB0CYC
 * @tc.desc:
 */
HWTEST_F(CGroupSchedTest, CGroupSchedTest_CgroupEventHandler_001, Function | MediumTest | Level1)
{
    auto cgroupEventHandler = std::make_shared<CgroupEventHandler>("CgroupEventHandler_unittest");
    cgroupEventHandler->SetSupervisor(supervisor_);
    EXPECT_NE(cgroupEventHandler->supervisor_, nullptr);
    uint32_t resType = 0;
    int64_t value = ResType::ScreenCaptureStatus::START_SCREEN_CAPTURE;
    nlohmann::json payload;
    payload["uid"] = "1000";
    payload["pid"] = "1000";
    auto processRecord = supervisor_->GetAppRecordNonNull(1000)->GetProcessRecordNonNull(1000);
    cgroupEventHandler->HandleReportScreenCaptureEvent(resType, value, payload);
    EXPECT_TRUE(processRecord->screenCaptureState_);
}

/**
 * @tc.name: CGroupSchedTest_CgroupEventHandler_002
 * @tc.desc: cgroup event handler Test
 * @tc.type: FUNC
 * @tc.require: issuesIB0CYC
 * @tc.desc:
 */
HWTEST_F(CGroupSchedTest, CGroupSchedTest_CgroupEventHandler_002, Function | MediumTest | Level1)
{
    auto cgroupEventHandler = std::make_shared<CgroupEventHandler>("CgroupEventHandler_unittest");
    cgroupEventHandler->SetSupervisor(supervisor_);
    EXPECT_NE(cgroupEventHandler->supervisor_, nullptr);
    uint32_t resType = 0;
    int64_t value = ResType::ScreenCaptureStatus::START_SCREEN_CAPTURE;
    nlohmann::json payload;
    payload["saId"] = "1000";
    payload["deviceId"] = "1000";
    cgroupEventHandler->ReportAbilityStatus(resType, value, payload);
    EXPECT_EQ(resType, 0);
}

/**
 * @tc.name: CGroupSchedTest_CgroupEventHandler_003
 * @tc.desc: cgroup event handler Test
 * @tc.type: FUNC
 * @tc.require: issuesIB0CYC
 * @tc.desc:
 */
HWTEST_F(CGroupSchedTest, CGroupSchedTest_CgroupEventHandler_003, Function | MediumTest | Level1)
{
    auto cgroupEventHandler = std::make_shared<CgroupEventHandler>("CgroupEventHandler_unittest");
    supervisor_->InitSuperVisorContent();
    EXPECT_NE(supervisor_->appManager_, nullptr);
    cgroupEventHandler->SetSupervisor(supervisor_);
    EXPECT_NE(cgroupEventHandler->supervisor_, nullptr);
    auto procRecord = cgroupEventHandler->supervisor_->GetAppRecordNonNull(1000)->GetProcessRecordNonNull(1000);
    EXPECT_NE(procRecord, nullptr);
    EXPECT_EQ(procRecord->mmiStatus_, -1);
    uint32_t resType = 0;
    int64_t value = ResType::ScreenCaptureStatus::START_SCREEN_CAPTURE;
    nlohmann::json payload;
    payload["uid"] = 1000;
    payload["pid"] = 1000;
    payload["status"] = 1;
    cgroupEventHandler->UpdateMmiStatus(resType, value, payload);
    EXPECT_EQ(procRecord->mmiStatus_, 1);
}

/**
 * @tc.name: CGroupSchedTest_CgroupEventHandler_004
 * @tc.desc: cgroup event handler Test
 * @tc.type: FUNC
 * @tc.require: issuesIB0CYC
 * @tc.desc:
 */
HWTEST_F(CGroupSchedTest, CGroupSchedTest_CgroupEventHandler_004, Function | MediumTest | Level1)
{
    auto cgroupEventHandler = std::make_shared<CgroupEventHandler>("CgroupEventHandler_unittest");
    cgroupEventHandler->SetSupervisor(supervisor_);
    EXPECT_NE(cgroupEventHandler->supervisor_, nullptr);
    uint32_t resType = 0;
    int64_t value = ResType::WebVideoState::WEB_VIDEO_PLAYING_START;
    nlohmann::json payload;
    payload["uid"] = "1000";
    payload["pid"] = "1000";
    auto processRecord = supervisor_->GetAppRecordNonNull(1000)->GetProcessRecordNonNull(1000);
    cgroupEventHandler->HandleReportWebviewVideoState(resType, value, payload);
    EXPECT_TRUE(processRecord->videoState_);
}

/**
 * @tc.name: CGroupSchedTest_CgroupEventHandler_005
 * @tc.desc: cgroup event handler Test
 * @tc.type: FUNC
 * @tc.require: issuesIB0CYC
 * @tc.desc:
 */
HWTEST_F(CGroupSchedTest, CGroupSchedTest_CgroupEventHandler_005, Function | MediumTest | Level1)
{
    auto cgroupEventHandler = std::make_shared<CgroupEventHandler>("CgroupEventHandler_unittest");
    cgroupEventHandler->SetSupervisor(supervisor_);
    EXPECT_NE(cgroupEventHandler->supervisor_, nullptr);
    uint32_t resType = 0;
    int64_t value = ResType::AvCodecState::CODEC_START_INFO;
    nlohmann::json payload;
    payload["uid"] = "1000";
    payload["pid"] = "1000";
    payload["instanceId"] = "1000";
    auto processRecord = supervisor_->GetAppRecordNonNull(1000)->GetProcessRecordNonNull(1000);
    cgroupEventHandler->HandleReportAvCodecEvent(resType, value, payload);
    EXPECT_TRUE(processRecord->avCodecState_[1000]);
}

/**
 * @tc.name: CGroupSchedTest_CgroupEventHandler_006
 * @tc.desc: cgroup event handler Test
 * @tc.type: FUNC
 * @tc.require: issuesIB0CYC
 * @tc.desc:
 */
HWTEST_F(CGroupSchedTest, CGroupSchedTest_CgroupEventHandler_006, Function | MediumTest | Level1)
{
    auto cgroupEventHandler = std::make_shared<CgroupEventHandler>("CgroupEventHandler_unittest");
    cgroupEventHandler->SetSupervisor(supervisor_);
    EXPECT_NE(cgroupEventHandler->supervisor_, nullptr);
    uint32_t resType = 0;
    int64_t value = ResType::RunninglockState::RUNNINGLOCK_STATE_ENABLE;
    nlohmann::json payload;
    payload["uid"] = "1000";
    payload["pid"] = "1000";
    payload["type"] = "1000";
    auto processRecord = supervisor_->GetAppRecordNonNull(1000)->GetProcessRecordNonNull(1000);
    cgroupEventHandler->HandleReportRunningLockEvent(resType, value, payload);
#ifdef POWER_MANAGER_ENABLE
    EXPECT_TRUE(processRecord->runningLockState_[1000]);
#endif
}

/**
 * @tc.name: CGroupSchedTest_CgroupEventHandler_007
 * @tc.desc: cgroup event handler Test
 * @tc.type: FUNC
 * @tc.require: issuesIB0CYC
 * @tc.desc:
 */
HWTEST_F(CGroupSchedTest, CGroupSchedTest_CgroupEventHandler_007, Function | MediumTest | Level1)
{
    auto cgroupEventHandler = std::make_shared<CgroupEventHandler>("CgroupEventHandler_unittest");
    cgroupEventHandler->SetSupervisor(supervisor_);
    EXPECT_NE(cgroupEventHandler->supervisor_, nullptr);
    uint32_t resType = 0;
    int64_t value = ResType::WebVideoState::WEB_VIDEO_PLAYING_START;
    nlohmann::json payload;
    payload["uid"] = "1000";
    payload["pid"] = "1000";
    cgroupEventHandler->HandleSceneBoardState(resType, value, payload);
    EXPECT_EQ(cgroupEventHandler->supervisor_->sceneBoardPid_, 1000);
}

/**
 * @tc.name: CGroupSchedTest_CgroupEventHandler_008
 * @tc.desc: cgroup event handler Test
 * @tc.type: FUNC
 * @tc.require: issuesIB3UW9
 * @tc.desc:
 */
HWTEST_F(CGroupSchedTest, CGroupSchedTest_CgroupEventHandler_008, Function | MediumTest | Level1)
{
    auto cgroupEventHandler = std::make_shared<CgroupEventHandler>("CgroupEventHandler_unittest");
    cgroupEventHandler->SetSupervisor(supervisor_);
    EXPECT_NE(cgroupEventHandler->supervisor_, nullptr);
    uint32_t resType = ResType::RES_TYPE_COSMIC_CUBE_STATE_CHANGE;
    int64_t valueToHide = ResType::CosmicCubeState::APPLICATION_ABOUT_TO_HIDE;
    int64_t valueToAppear = ResType::CosmicCubeState::APPLICATION_ABOUT_TO_APPEAR;
    int64_t valueToRecover = ResType::CosmicCubeState::APPLICATION_ABOUT_TO_RECOVER;
    // test group with value CosmicCubeState::APPLICATION_ABOUT_TO_HIDE
    nlohmann::json payload;
    payload["uid"] = "1000";
    payload["pid"] = "1000";
    auto app1000 = supervisor_->GetAppRecordNonNull(1000);
    auto processRecord1000 = app1000->GetProcessRecordNonNull(1000);
    cgroupEventHandler->HandleReportCosmicCubeState(resType, valueToHide, payload);
    EXPECT_TRUE(app1000->isCosmicCubeStateHide_);
    // test group with value CosmicCubeState::APPLICATION_ABOUT_TO_APPEAR
    cgroupEventHandler->HandleReportCosmicCubeState(resType, valueToAppear, payload);
    EXPECT_FALSE(app1000->isCosmicCubeStateHide_);

    // mock payload 1 to change ground to background
    nlohmann::json payload1;
    payload1["uid"] = "1001";
    payload1["pid"] = "1001";
    auto app1001 = supervisor_->GetAppRecordNonNull(1001);
    auto processRecord1001 = app1001->GetProcessRecordNonNull(1001);
    cgroupEventHandler->HandleReportCosmicCubeState(resType, valueToHide, payload1);
    EXPECT_TRUE(app1001->isCosmicCubeStateHide_);
    // mock payload 2 to change ground to background
    nlohmann::json payload2;
    payload2["uid"] = "1002";
    payload2["pid"] = "1002";
    auto app1002 = supervisor_->GetAppRecordNonNull(1002);
    auto processRecord1002 = app1002->GetProcessRecordNonNull(1002);
    cgroupEventHandler->HandleReportCosmicCubeState(resType, valueToHide, payload2);
    EXPECT_TRUE(app1002->isCosmicCubeStateHide_);
    // test empty payload
    nlohmann::json emptyPayload;
    // test empty payload with CosmicCubeState::APPLICATION_ABOUT_TO_HIDE
    cgroupEventHandler->HandleReportCosmicCubeState(resType, valueToHide, emptyPayload);
    EXPECT_TRUE(app1001->isCosmicCubeStateHide_);
    EXPECT_TRUE(app1002->isCosmicCubeStateHide_);
    // test empty payload with CosmicCubeState::APPLICATION_ABOUT_TO_APPEAR
    cgroupEventHandler->HandleReportCosmicCubeState(resType, valueToAppear, emptyPayload);
    EXPECT_TRUE(app1001->isCosmicCubeStateHide_);
    EXPECT_TRUE(app1002->isCosmicCubeStateHide_);
    // test empty payload with CosmicCubeState::APPLICATION_ABOUT_TO_RECOVER
    cgroupEventHandler->HandleReportCosmicCubeState(resType, valueToRecover, emptyPayload);
    EXPECT_FALSE(app1001->isCosmicCubeStateHide_);
    EXPECT_FALSE(app1002->isCosmicCubeStateHide_);
}

/**
 * @tc.name: CGroupSchedTest_CgroupEventHandler_009
 * @tc.desc: cgroup event handler Test
 * @tc.type: FUNC
 * @tc.require: issuesIB3UW9
 * @tc.desc:
 */
HWTEST_F(CGroupSchedTest, CGroupSchedTest_CgroupEventHandler_009, Function | MediumTest | Level1)
{
    auto cgroupEventHandler = std::make_shared<CgroupEventHandler>("CgroupEventHandle_unittest");
    cgroupEventHandler->SetSupervisor(supervisor_);
    EXPECT_NE(cgroupEventHandler->supervisor_, nullptr);
    uint32_t resType = 0;
    int64_t value = ResType::WebVideoState::WEB_VIDEO_PLAYING_START;
    nlohmann::json payload;
    payload["uid"] = 1000;
    payload["bundleName"] = "test_009";
    auto app = supervisor_->GetAppRecordNonNull(1000);
    cgroupEventHandler->HandleOnAppStopped(resType, value, payload);
    EXPECT_FALSE(app);
}

/**
 * @tc.name: CGroupSchedTest_CgroupEventHandler_010
 * @tc.desc: cgroup event handler Test
 * @tc.type: FUNC
 * @tc.require: issuesIB3UW9
 * @tc.desc:
 */
HWTEST_F(CGroupSchedTest, CGroupSchedTest_CgroupEventHandler_010, Function | MediumTest | Level1)
{
    auto cgroupEventHandler = std::make_shared<CgroupEventHandler>("CgroupEventHandler_unittest");
    Rosen::WindowType WindowType = Rosen::WindowType::APP_WINDOW_BASE;
    uint32_t windowId = 1;
    EXPECT_TRUE(cgroupEventHandler->supervisor_ != nullptr);
    cgroupEventHandler->HandleWindowVisibilityChanged(windowId, false, WindowType, 12121, 12121);
    cgroupEventHandler->HandleWindowVisibilityChanged(windowId, true, WindowType, 12121, 12121);

    supervisor_->GetAppRecord(12121)->RemoveProcessRecord(12121);
    EXPECT_TRUE(supervisor_->GetAppRecord(12121)->GetProcessRecord(12121) == nullptr);
    cgroupEventHandler->HandleWindowVisibilityChanged(windowId, true, WindowType, 12121, 12121);

    supervisor_->RemoveApplication(12121);
    EXPECT_TRUE(supervisor_->GetAppRecord(12121) == nullptr);
    cgroupEventHandler->HandleWindowVisibilityChanged(windowId, true, WindowType, 12121, 12121);
}

/**
 * @tc.name: CGroupSchedTest_CgroupEventHandler_011
 * @tc.desc: cgroup event handler Test
 * @tc.type: FUNC
 * @tc.require: issuesIB3UW9
 * @tc.desc:
 */
HWTEST_F(CGroupSchedTest, CGroupSchedTest_CgroupEventHandler_011, Function | MediumTest | Level1)
{
    auto cgroupEventHandler = std::make_shared<CgroupEventHandler>("CgroupEventHandler_unittest");
    EXPECT_TRUE(cgroupEventHandler->supervisor_ != nullptr);
    nlohmann::json payload = nlohmann::json::parse("{\"uid\": \"1111\", \"pid\": \"1112\"}");
    cgroupEventHandler->HandleReportMMIProcess(1, 1, payload);
    cgroupEventHandler->HandleReportMMIProcess(1, -1, payload);

    payload = nlohmann::json::parse("{\"uid\": \"-1\", \"pid\": \"1112\"}");
    cgroupEventHandler->HandleReportMMIProcess(1, 1, payload);

    payload = nlohmann::json::parse("{\"uid\": \"1111\", \"pid\": \"-1\"}");
    cgroupEventHandler->HandleReportMMIProcess(1, 1, payload);

    payload = nlohmann::json::parse("{\"uid\": \"1111\"}");
    cgroupEventHandler->HandleReportMMIProcess(1, 1, payload);

    payload = nlohmann::json::parse("{\"pid\": \"1112\"}");
    cgroupEventHandler->HandleReportMMIProcess(1, 1, payload);
}

/**
 * @tc.name: CGroupSchedTest_CgroupEventHandler_012
 * @tc.desc: cgroup event handler Test
 * @tc.type: FUNC
 * @tc.require: issuesIB3UW9
 * @tc.desc:
 */
HWTEST_F(CGroupSchedTest, CGroupSchedTest_CgroupEventHandler_012, Function | MediumTest | Level1)
{
    auto cgroupEventHandler = std::make_shared<CgroupEventHandler>("CgroupEventHandler_unittest");
    EXPECT_TRUE(cgroupEventHandler->supervisor_ != nullptr);
    nlohmann::json payload =
        nlohmann::json::parse("{\"uid\": \"1111\", \"pid\": \"1112\", \"tid\": \"1113\", \"role\": \"1114\"}");
    auto app = cgroupEventHandler->supervisor_->GetAppRecordNonNull(1111);
    EXPECT_TRUE(app != nullptr);
    auto proc = app-> GetProcessRecordNonNull(1112);
    EXPECT_TRUE(proc != nullptr);
    cgroupEventHandler->HandleReportKeyThread(1, 1, payload);

    payload = nlohmann::json::parse("{\"uid\": \"1111\", \"pid\": \"1112\", \"role\": \"1114\"}");
    cgroupEventHandler->HandleReportKeyThread(1, 1, payload);

    payload = nlohmann::json::parse("{\"uid\": \"1111\", \"pid\": \"1112\", \"tid\": \"1113\"}");
    cgroupEventHandler->HandleReportKeyThread(1, 1, payload);
}

/**
 * @tc.name: CGroupSchedTest_CgroupEventHandler_013
 * @tc.desc: cgroup event handler Test
 * @tc.type: FUNC
 * @tc.require: issuesIB3UW9
 * @tc.desc:
 */
HWTEST_F(CGroupSchedTest, CGroupSchedTest_CgroupEventHandler_013, Function | MediumTest | Level1)
{
    auto cgroupEventHandler = std::make_shared<CgroupEventHandler>("CgroupEventHandler_unittest");
    EXPECT_TRUE(cgroupEventHandler->supervisor_ != nullptr);
    nlohmann::json payload = nlohmann::json::parse("{\"uid\": \"1111\", \"pid\": \"1112\"}");
    cgroupEventHandler->HandleReportWindowState(1, 1, payload);
    cgroupEventHandler->HandleReportWindowState(1, 1, payload);
}

/**
 * @tc.name: CGroupSchedTest_CgroupEventHandler_014
 * @tc.desc: cgroup event handler Test
 * @tc.type: FUNC
 * @tc.require: issuesIB3UW9
 * @tc.desc:
 */
HWTEST_F(CGroupSchedTest, CGroupSchedTest_CgroupEventHandler_014, Function | MediumTest | Level1)
{
    auto cgroupEventHandler = std::make_shared<CgroupEventHandler>("CgroupEventHandler_unittest");
    EXPECT_TRUE(cgroupEventHandler->supervisor_ != nullptr);
    const string payloadStr = ("{\"uid\": \"1111\", \"pid\": \"1112\", \"tid\": \"1112\", \"role\": \"2\"}");
    nlohmann::json payload = nlohmann::json::parse(payloadStr);
    cgroupEventHandler->HandleReportKeyThread(ResType::RES_TYPE_REPORT_KEY_THREAD, 0, payload);
    cgroupEventHandler->HandleReportKeyThread(ResType::RES_TYPE_REPORT_KEY_THREAD, 1, payload);

    payload = nlohmann::json::parse("{\"uid\": \"1111\", \"pid\": \"1112\", \"role\": \"2\"}");
    cgroupEventHandler->HandleReportKeyThread(ResType::RES_TYPE_REPORT_KEY_THREAD, 0, payload);

    payload = nlohmann::json::parse("{\"uid\": \"1111\", \"pid\": \"1112\", \"tid\": \"1112\"}");
    cgroupEventHandler->HandleReportKeyThread(ResType::RES_TYPE_REPORT_KEY_THREAD, 0, payload);

    payload = nlohmann::json::parse("{\"uid\": \"1111\", \"pid\": \"1112\"}");
    cgroupEventHandler->HandleReportKeyThread(ResType::RES_TYPE_REPORT_KEY_THREAD, 0, payload);
}

/**
 * @tc.name: CGroupSchedTest_CgroupEventHandler_015
 * @tc.desc: cgroup event handler Test
 * @tc.type: FUNC
 * @tc.require: issuesIB3UW9
 * @tc.desc:
 */
HWTEST_F(CGroupSchedTest, CGroupSchedTest_CgroupEventHandler_015, Function | MediumTest | Level1)
{
    auto cgroupEventHandler = std::make_shared<CgroupEventHandler>("CgroupEventHandler_unittest");
    EXPECT_TRUE(cgroupEventHandler->supervisor_ != nullptr);
    auto app = cgroupEventHandler->supervisor_->GetAppRecordNonNull(1111);
    EXPECT_TRUE(app != nullptr);
    auto proc = app->GetProcessRecordNonNull(1112);
    EXPECT_TRUE(proc != nullptr);
    auto hostProcRecord = app->GetProcessRecordNonNull(1234);
    EXPECT_TRUE(hostProcRecord != nullptr);
    proc->processType_ = ProcRecordType::RENDER;
    app->AddHostProcess(1234);
    proc->hostPid_ = 1234;
    const string payloadStr =
        "{\"uid\": \"1111\", \"pid\": \"1112\", \"windowId\": \"1112\", \"state\": \"0\", \"serialNum\": \"0\"}";
    nlohmann::json payload = nlohmann::json::parse(payloadStr);
    cgroupEventHandler->HandleReportWindowState(ResType::RES_TYPE_REPORT_WINDOW_STATE, 0, payload);

    payload = nlohmann::json::parse("{\"uid\": \"1111\","
        "\"pid\": \"1112\", \"windowId\": \"1112\", \"state\": \"1\", \"serialNum\": \"0\"}");
    cgroupEventHandler->HandleReportWindowState(ResType::RES_TYPE_REPORT_WINDOW_STATE, 0, payload);

    payload = nlohmann::json::parse("{\"uid\": \"1111\", \"pid\": \"1112\", \"state\": \"0\", \"serialNum\": \"0\"}");
    cgroupEventHandler->HandleReportWindowState(ResType::RES_TYPE_REPORT_WINDOW_STATE, 0, payload);

    payload = nlohmann::json::parse("{\"uid\": \"1111\", \"pid\": \"1112\", \"windowId\": \"1112\", \"state\": \"0\"}");
    cgroupEventHandler->HandleReportWindowState(ResType::RES_TYPE_REPORT_WINDOW_STATE, 0, payload);

    payload = nlohmann::json::parse("{\"uid\": \"1111\", \"pid\": \"1112\", \"serialNum\": \"0\"}");
    cgroupEventHandler->HandleReportWindowState(ResType::RES_TYPE_REPORT_WINDOW_STATE, 0, payload);

    payload = nlohmann::json::parse("{\"uid\": \"1111\", \"pid\": \"1112\"}");
    cgroupEventHandler->HandleReportWindowState(ResType::RES_TYPE_REPORT_WINDOW_STATE, 0, payload);
}

/**
 * @tc.name: CGroupSchedTest_CgroupEventHandler_016
 * @tc.desc: cgroup event handler Test
 * @tc.type: FUNC
 * @tc.require: issuesIB3UW9
 * @tc.desc:
 */
HWTEST_F(CGroupSchedTest, CGroupSchedTest_CgroupEventHandler_016, Function | MediumTest | Level1)
{
    auto cgroupEventHandler = std::make_shared<CgroupEventHandler>("CgroupEventHandler_unittest");
    auto temp = cgroupEventHandler->supervisor_;
    cgroupEventHandler->supervisor_ = nullptr;
    nlohmann::json payload = nlohmann::json::parse("{\"pid\": \"1112\"}");
    cgroupEventHandler->HandleReportAudioState(ResType::RES_TYPE_AUDIO_RENDER_STATE_CHANGE, 2, payload);
    cgroupEventHandler->supervisor_ = temp;

    EXPECT_TRUE(cgroupEventHandler->supervisor_ != nullptr);
    payload = nlohmann::json::parse("{\"pid\": 1112}");
    cgroupEventHandler->HandleReportAudioState(ResType::RES_TYPE_AUDIO_RENDER_STATE_CHANGE, 2, payload);

    payload = nlohmann::json::parse("{\"uid\": \"1111\", \"pid\": 1112}");
    cgroupEventHandler->HandleReportAudioState(ResType::RES_TYPE_AUDIO_RENDER_STATE_CHANGE, 2, payload);

    payload = nlohmann::json::parse("{\"pid\": 1112}");
    cgroupEventHandler->HandleReportAudioState(ResType::RES_TYPE_AUDIO_RENDER_STATE_CHANGE, 2, payload);

    payload = nlohmann::json::parse("{\"uid\": 1111, \"pid\": \"1112\"}");
    cgroupEventHandler->HandleReportAudioState(ResType::RES_TYPE_AUDIO_RENDER_STATE_CHANGE, 2, payload);

    payload = nlohmann::json::parse("{\"uid\": 0, \"pid\": 1112}");
    cgroupEventHandler->HandleReportAudioState(ResType::RES_TYPE_AUDIO_RENDER_STATE_CHANGE, 2, payload);

    payload = nlohmann::json::parse("{\"uid\": 1111, \"pid\": 0}");
    cgroupEventHandler->HandleReportAudioState(ResType::RES_TYPE_AUDIO_RENDER_STATE_CHANGE, 2, payload);

    payload = nlohmann::json::parse("{\"uid\": 1111, \"pid\": 1112}");
    cgroupEventHandler->HandleReportAudioState(ResType::RES_TYPE_AUDIO_RENDER_STATE_CHANGE, 2, payload);
    auto app = cgroupEventHandler->supervisor_->GetAppRecordNonNull(1111);
    EXPECT_TRUE(app != nullptr);
    cgroupEventHandler->HandleReportAudioState(ResType::RES_TYPE_AUDIO_RENDER_STATE_CHANGE, 2, payload);
    auto proc = app->GetProcessRecordNonNull(1112);
    EXPECT_TRUE(proc != nullptr);

    payload = nlohmann::json::parse("{\"uid\": \"1111\", \"pid\": \"1112\"}");
    proc->curSchedGroup_ = CgroupSetting::SP_BACKGROUND;
    cgroupEventHandler->HandleReportAudioState(ResType::RES_TYPE_AUDIO_RENDER_STATE_CHANGE, 2, payload);
    EXPECT_TRUE(proc->audioPlayingState_ == 2);
}

/**
 * @tc.name: CGroupSchedTest_CgroupEventHandler_017
 * @tc.desc: cgroup event handler Test
 * @tc.type: FUNC
 * @tc.require: issuesIB3UW9
 * @tc.desc:
 */
HWTEST_F(CGroupSchedTest, CGroupSchedTest_CgroupEventHandler_017, Function | MediumTest | Level1)
{
    auto cgroupEventHandler = std::make_shared<CgroupEventHandler>("CgroupEventHandler_unittest");
    auto temp = cgroupEventHandler->supervisor_;
    cgroupEventHandler->supervisor_ = nullptr;
    nlohmann::json payload = nlohmann::json::parse("{\"clientPid\": \"1112\"}");
    cgroupEventHandler->HandleReportWebviewAudioState(ResType::RES_TYPE_WEBVIEW_AUDIO_STATUS_CHANGE, 0, payload);
    cgroupEventHandler->supervisor_ = temp;

    EXPECT_TRUE(cgroupEventHandler->supervisor_ != nullptr);
    payload = nlohmann::json::parse("{\"clientPid\": \"1112\"}");
    cgroupEventHandler->HandleReportWebviewAudioState(ResType::RES_TYPE_WEBVIEW_AUDIO_STATUS_CHANGE, 0, payload);

    payload = nlohmann::json::parse("{\"uid\": \"1111\"}");
    cgroupEventHandler->HandleReportWebviewAudioState(ResType::RES_TYPE_WEBVIEW_AUDIO_STATUS_CHANGE, 0, payload);

    payload = nlohmann::json::parse("{\"uid\": \"0\", \"clientPid\": \"1112\"}");
    cgroupEventHandler->HandleReportWebviewAudioState(ResType::RES_TYPE_WEBVIEW_AUDIO_STATUS_CHANGE, 0, payload);

    payload = nlohmann::json::parse("{\"uid\": \"1111\", \"clientPid\": \"0\"}");
    cgroupEventHandler->HandleReportWebviewAudioState(ResType::RES_TYPE_WEBVIEW_AUDIO_STATUS_CHANGE, 0, payload);

    payload = nlohmann::json::parse("{\"uid\": \"1111\", \"clientPid\": \"1112\"}");
    auto app = cgroupEventHandler->supervisor_->GetAppRecordNonNull(1111);
    EXPECT_TRUE(app != nullptr);
    cgroupEventHandler->HandleReportWebviewAudioState(ResType::RES_TYPE_WEBVIEW_AUDIO_STATUS_CHANGE, 0, payload);
    auto proc = app->GetProcessRecordNonNull(1112);
    EXPECT_TRUE(proc != nullptr);

    proc->curSchedGroup_ = CgroupSetting::SP_BACKGROUND;
    proc->processType_ = ProcRecordType::RENDER;
    cgroupEventHandler->HandleReportWebviewAudioState(ResType::RES_TYPE_WEBVIEW_AUDIO_STATUS_CHANGE, 0, payload);
    EXPECT_TRUE(proc->audioPlayingState_ == 0);
}

/**
 * @tc.name: CGroupSchedTest_CgroupEventHandler_018
 * @tc.desc: cgroup event handler Test
 * @tc.type: FUNC
 * @tc.require: issuesIB3UW9
 * @tc.desc:
 */
HWTEST_F(CGroupSchedTest, CGroupSchedTest_CgroupEventHandler_018, Function | MediumTest | Level1)
{
    auto cgroupEventHandler = std::make_shared<CgroupEventHandler>("CgroupEventHandler_unittest");
    auto temp = cgroupEventHandler->supervisor_;
    cgroupEventHandler->supervisor_ = nullptr;
    cgroupEventHandler->HandleDrawingContentChangeWindow(2054, WindowType::WINDOW_TYPE_APP_MAIN_WINDOW,
        true, 429, 2024);
    cgroupEventHandler->supervisor_ = temp;

    EXPECT_TRUE(cgroupEventHandler->supervisor_ != nullptr);
    auto app = cgroupEventHandler->supervisor_->GetAppRecordNonNull(2024);
    app->RemoveProcessRecord(429);
    EXPECT_TRUE(app->GetProcessRecord(429) == nullptr);
    cgroupEventHandler->HandleDrawingContentChangeWindow(2054, WindowType::WINDOW_TYPE_APP_MAIN_WINDOW,
        true, 429, 2024);

    cgroupEventHandler->supervisor_->RemoveApplication(2024);
    EXPECT_TRUE(cgroupEventHandler->supervisor_->GetAppRecord(2024) == nullptr);
    cgroupEventHandler->HandleDrawingContentChangeWindow(2054, WindowType::WINDOW_TYPE_APP_MAIN_WINDOW,
        true, 429, 2024);

    app = cgroupEventHandler->supervisor_->GetAppRecordNonNull(2024);
    EXPECT_TRUE(app != nullptr);
    auto proc = app->GetProcessRecordNonNull(429);
    EXPECT_TRUE(proc != nullptr);

    cgroupEventHandler->HandleDrawingContentChangeWindow(2054, WindowType::WINDOW_TYPE_APP_MAIN_WINDOW,
        true, 429, 2024);
    EXPECT_TRUE(proc->processDrawingState_);
    auto windowInfo = proc->GetWindowInfoNonNull(2054);
    EXPECT_TRUE(windowInfo->drawingContentState_);
}

/**
 * @tc.name: CGroupSchedTest_CgroupEventHandler_019
 * @tc.desc: cgroup event handler Test
 * @tc.type: FUNC
 * @tc.require: issuesIB3UW9
 * @tc.desc:
 */
HWTEST_F(CGroupSchedTest, CGroupSchedTest_CgroupEventHandler_019, Function | MediumTest | Level1)
{
    auto cgroupEventHandler = std::make_shared<CgroupEventHandler>("CgroupEventHandler_unittest");
    auto temp = cgroupEventHandler->supervisor_;
    cgroupEventHandler->supervisor_ = nullptr;
    nlohmann::json payload = nlohmann::json::parse("{\"uid\": \"1111\", \"pid\": \"1112\", \"syncStatus\": \"1\"}");
    cgroupEventHandler->HandleReportHisysEvent(ResType::RES_TYPE_REPORT_CAMERA_STATE, 0, payload);
    cgroupEventHandler->supervisor_ = temp;

    EXPECT_TRUE(cgroupEventHandler->supervisor_ != nullptr);
    payload = nlohmann::json::parse("{\"pid\": \"1112\", \"syncStatus\": \"1\"}");
    cgroupEventHandler->HandleReportHisysEvent(ResType::RES_TYPE_REPORT_CAMERA_STATE, 0, payload);

    payload = nlohmann::json::parse("{\"uid\": \"1111\", \"syncStatus\": \"1\"}");
    cgroupEventHandler->HandleReportHisysEvent(ResType::RES_TYPE_REPORT_CAMERA_STATE, 0, payload);

    payload = nlohmann::json::parse("{\"syncStatus\": \"1\"}");
    cgroupEventHandler->HandleReportHisysEvent(ResType::RES_TYPE_REPORT_CAMERA_STATE, 0, payload);

    payload = nlohmann::json::parse("{\"uid\": \"0\", \"pid\": \"1112\", \"syncStatus\": \"1\"}");
    cgroupEventHandler->HandleReportHisysEvent(ResType::RES_TYPE_REPORT_CAMERA_STATE, 0, payload);

    payload = nlohmann::json::parse("{\"uid\": \"1111\", \"pid\": \"0\", \"syncStatus\": \"1\"}");
    cgroupEventHandler->HandleReportHisysEvent(ResType::RES_TYPE_REPORT_CAMERA_STATE, 0, payload);

    payload = nlohmann::json::parse("{\"uid\": \"1111\", \"pid\": \"1112\", \"syncStatus\": \"1\"}");
    cgroupEventHandler->HandleReportHisysEvent(ResType::RES_TYPE_REPORT_CAMERA_STATE, 0, payload);
    auto app = cgroupEventHandler->supervisor_->GetAppRecordNonNull(1111);
    EXPECT_TRUE(app != nullptr);
    cgroupEventHandler->HandleReportHisysEvent(ResType::RES_TYPE_REPORT_CAMERA_STATE, 0, payload);
    auto proc = app->GetProcessRecordNonNull(1112);
    EXPECT_TRUE(proc != nullptr);

    cgroupEventHandler->HandleReportHisysEvent(ResType::RES_TYPE_REPORT_CAMERA_STATE, 0, payload);
    EXPECT_TRUE(proc->cameraState_ == 0);
    cgroupEventHandler->HandleReportHisysEvent(ResType::RES_TYPE_WIFI_CONNECT_STATE_CHANGE, 3, payload);
    EXPECT_TRUE(proc->wifiState_ == 3);
}

/**
 * @tc.name: CGroupSchedTest_CgroupEventHandler_020
 * @tc.desc: cgroup event handler Test
 * @tc.type: FUNC
 * @tc.require: issuesIB3UW9
 * @tc.desc:
 */
HWTEST_F(CGroupSchedTest, CGroupSchedTest_CgroupEventHandler_020, Function | MediumTest | Level1)
{
    auto cgroupEventHandler = std::make_shared<CgroupEventHandler>("CgroupEventHandler_unittest");
    EXPECT_TRUE(cgroupEventHandler->supervisor_ != nullptr);
    nlohmann::json payload = nlohmann::json::parse("{\"uid\": \"2024\", \"pid\": \"429\"}");
    auto app = cgroupEventHandler->supervisor_->GetAppRecordNonNull(2024);
    EXPECT_TRUE(app != nullptr);
    auto proc = app->GetProcessRecordNonNull(429);
    EXPECT_TRUE(proc != nullptr);

    cgroupEventHandler->HandleWebviewScreenCapture(ResType::RES_TYPE_WEBVIEW_SCREEN_CAPTURE, 0, payload);
    EXPECT_TRUE(proc->screenCaptureState_);
    cgroupEventHandler->HandleWebviewScreenCapture(ResType::RES_TYPE_WEBVIEW_SCREEN_CAPTURE, 1, payload);
    EXPECT_FALSE(proc->screenCaptureState_);
}

/**
 * @tc.name: CGroupSchedTest_CgroupEventHandler_021
 * @tc.desc: cgroup event handler Test
 * @tc.type: FUNC
 * @tc.require: issuesIB3UW9
 * @tc.desc:
 */
HWTEST_F(CGroupSchedTest, CGroupSchedTest_CgroupEventHandler_021, Function | MediumTest | Level1)
{
    auto cgroupEventHandler = std::make_shared<CgroupEventHandler>("CgroupEventHandler_unittest");
    auto tmp = cgroupEventHandler->supervisor_;
    cgroupEventHandler->supervisor_ = nullptr;

    uid_t uid = 1000;
    pid_t pid = 1234;
    int32_t hostPid = 2024;
    std::string bundleName = "com.ohos.test";
    std::string abilityName = "MainAbility";
    Rosen::WindowType windowType = Rosen::WindowType::APP_WINDOW_BASE;
    uint64_t displayId = 1;
    uint32_t windowId = 1;
    uintptr_t token = 1111;
    int32_t appState = (int32_t)AppExecFwk::ApplicationState::APP_STATE_CREATE;
    int32_t abilityState = (int32_t)AppExecFwk::AbilityState::ABILITY_STATE_BACKGROUND;
    int32_t abilityType = (int32_t)AppExecFwk::AbilityType::UNKNOWN;
    int32_t extensionState = (int32_t)AppExecFwk::ExtensionState::EXTENSION_STATE_CREATE;
    int32_t typeId = (int32_t)BackgroundTaskMgr::BackgroundMode::DATA_TRANSFER;
    uint32_t resType = ResType::RES_TYPE_REPORT_RENDER_THREAD;
    int64_t value = 1235;
    bool isVisible = true;
    nlohmann::json payload = nlohmann::json::parse("{\"uid\": \"1000\", \"pid\": \"1234\"}");
    ProcessData processData;
    processData.uid = uid;
    processData.pid = pid;
    processData.bundleName = bundleName;
    processData.hostPid = hostPid;
    processData.processType = static_cast<AppExecFwk::ProcessType>((int32_t)AppExecFwk::ProcessType::NORMAL);
    processData.extensionType = static_cast<AppExecFwk::ExtensionAbilityType>(INVALID_EXTENSION_TYPE);

    EXPECT_TRUE(cgroupEventHandler->supervisor_ == nullptr);
    cgroupEventHandler->HandleProcessStateChanged(uid, pid, bundleName, appState);
    cgroupEventHandler->HandleApplicationStateChanged(uid, pid, bundleName, appState);
    cgroupEventHandler->HandleAbilityStateChanged(uid, pid, bundleName, abilityName, token, abilityState, abilityType);
    cgroupEventHandler->HandleExtensionStateChanged(uid, pid,
        bundleName, abilityName, token, abilityState, abilityType);
    cgroupEventHandler->HandleProcessCreated(processData);
    cgroupEventHandler->HandleProcessDied(uid, pid, bundleName);
    cgroupEventHandler->HandleTransientTaskStart(uid, pid, bundleName);
    cgroupEventHandler->HandleTransientTaskEnd(uid, pid, bundleName);
    cgroupEventHandler->HandleContinuousTaskUpdate(uid, pid, {typeId}, value);
    cgroupEventHandler->HandleContinuousTaskCancel(uid, pid, typeId, value);
    cgroupEventHandler->HandleFocusedWindow(windowId, token, windowType, displayId, pid, uid);
    cgroupEventHandler->HandleUnfocusedWindow(windowId, token, windowType, displayId, pid, uid);
    cgroupEventHandler->HandleWindowVisibilityChanged(windowId, isVisible, windowType, pid, uid);
    cgroupEventHandler->HandleReportMMIProcess(resType, value, payload);
    cgroupEventHandler->HandleReportRenderThread(resType, value, payload);

    cgroupEventHandler->supervisor_ = tmp;
    SUCCEED();
}

/**
 * @tc.name: CGroupSchedTest_CgroupEventHandler_022
 * @tc.desc: cgroup event handler Test
 * @tc.type: FUNC
 * @tc.require: issuesIB3UW9
 * @tc.desc:
 */
HWTEST_F(CGroupSchedTest, CGroupSchedTest_CgroupEventHandler_022, Function | MediumTest | Level1)
{
    auto cgroupEventHandler = std::make_shared<CgroupEventHandler>("CgroupEventHandler_unittest");
    cgroupEventHandler->HandleProcessStateChanged(1000, 2000,
        "com.ohos.test", (int32_t)AppExecFwk::ApplicationState::APP_STATE_FOREGROUND);
    EXPECT_TRUE(supervisor_->GetAppRecord(1000) != nullptr);
}

/**
 * @tc.name: CGroupSchedTest_CgroupEventHandler_023
 * @tc.desc: cgroup event handler Test
 * @tc.type: FUNC
 * @tc.require: issuesIB3UW9
 * @tc.desc:
 */
HWTEST_F(CGroupSchedTest, CGroupSchedTest_CgroupEventHandler_023, Function | MediumTest | Level1)
{
    auto cgroupEventHandler = std::make_shared<CgroupEventHandler>("CgroupEventHandler_unittest");
    cgroupEventHandler->HandleApplicationStateChanged(1000, 2000,
        "com.ohos.test", (int32_t)AppExecFwk::ApplicationState::APP_STATE_FOREGROUND);
    EXPECT_TRUE(supervisor_->GetAppRecord(1000) != nullptr);
}

/**
 * @tc.name: CGroupSchedTest_CgroupEventHandler_024
 * @tc.desc: cgroup event handler Test
 * @tc.type: FUNC
 * @tc.require: issuesIB3UW9
 * @tc.desc:
 */
HWTEST_F(CGroupSchedTest, CGroupSchedTest_CgroupEventHandler_024, Function | MediumTest | Level1)
{
    auto cgroupEventHandler = std::make_shared<CgroupEventHandler>("CgroupEventHandler_unittest");
    cgroupEventHandler->HandleApplicationStateChanged(1000, 2000,
        "com.ohos.test", (int32_t)AppExecFwk::ApplicationState::APP_STATE_FOREGROUND);
    EXPECT_TRUE(supervisor_->GetAppRecord(1000) != nullptr);

    cgroupEventHandler->HandleApplicationStateChanged(1000, 2000,
        "com.ohos.test", (int32_t)AppExecFwk::ApplicationState::APP_STATE_FOREGROUND);
    EXPECT_TRUE(supervisor_->GetAppRecord(1000) == nullptr);
}

/**
 * @tc.name: CGroupSchedTest_CgroupEventHandler_025
 * @tc.desc: cgroup event handler Test
 * @tc.type: FUNC
 * @tc.require: issuesIB3UW9
 * @tc.desc:
 */
HWTEST_F(CGroupSchedTest, CGroupSchedTest_CgroupEventHandler_025, Function | MediumTest | Level1)
{
    auto cgroupEventHandler = std::make_shared<CgroupEventHandler>("CgroupEventHandler_unittest");
    cgroupEventHandler->HandleAbilityStateChanged(1000, 1234, "com.ohos.test", "MainAbility",
        1111, (int32_t)AppExecFwk::AbilityState::ABILITY_STATE_FOREGROUND, (int32_t)AppExecFwk::AbilityType::PAGE);
    EXPECT_TRUE(supervisor_->GetAppRecord(1000)->GetProcessRecord(1234)->GetAbilityInfo(1111) != nullptr);

    cgroupEventHandler->HandleAbilityStateChanged(1000, 1234, "com.ohos.test", "MainAbility",
        1111, (int32_t)AppExecFwk::AbilityState::ABILITY_STATE_FOREGROUND, (int32_t)AppExecFwk::AbilityType::PAGE);
    EXPECT_TRUE(supervisor_->GetAppRecord(1000)->GetProcessRecord(1234)->GetAbilityInfo(1111) == nullptr);

    supervisor_->GetAppRecord(1000)->RemoveProcessRecord(1234);
    EXPECT_TRUE(supervisor_->GetAppRecord(1000)->GetProcessRecord(1234) == nullptr);
    cgroupEventHandler->HandleAbilityStateChanged(1000, 1234, "com.ohos.test", "MainAbility",
        1111, (int32_t)AppExecFwk::AbilityState::ABILITY_STATE_FOREGROUND, (int32_t)AppExecFwk::AbilityType::PAGE);

    supervisor_->RemoveApplication(1000);
    EXPECT_TRUE(supervisor_->GetAppRecord(1000) == nullptr);
    cgroupEventHandler->HandleAbilityStateChanged(1000, 1234, "com.ohos.test", "MainAbility",
        1111, (int32_t)AppExecFwk::AbilityState::ABILITY_STATE_FOREGROUND, (int32_t)AppExecFwk::AbilityType::PAGE);
}

/**
 * @tc.name: CGroupSchedTest_CgroupEventHandler_026
 * @tc.desc: cgroup event handler Test
 * @tc.type: FUNC
 * @tc.require: issuesIB3UW9
 * @tc.desc:
 */
HWTEST_F(CGroupSchedTest, CGroupSchedTest_CgroupEventHandler_026, Function | MediumTest | Level1)
{
    auto cgroupEventHandler = std::make_shared<CgroupEventHandler>("CgroupEventHandler_unittest");
    cgroupEventHandler->HandleExtensionStateChanged(1000, 1234, "com.ohos.test", "ExtensionAbility",
        1111, (int32_t)AppExecFwk::ExtensionState::EXTENSION_STATE_READY,
        (int32_t)AppExecFwk::AbilityType::EXTENSION);
    EXPECT_TRUE(supervisor_->GetAppRecord(1000)->GetProcessRecord(1234)->GetAbilityInfo(1111) != nullptr);

    cgroupEventHandler->HandleExtensionStateChanged(1000, 1234, "com.ohos.test", "ExtensionAbility",
        1111, (int32_t)AppExecFwk::ExtensionState::EXTENSION_STATE_TERMINATED,
        (int32_t)AppExecFwk::AbilityType::EXTENSION);
    EXPECT_TRUE(supervisor_->GetAppRecord(1000)->GetProcessRecord(1234)->GetAbilityInfo(1111) == nullptr);

    supervisor_->GetAppRecord(1000)->RemoveProcessRecord(1234);
    EXPECT_TRUE(supervisor_->GetAppRecord(1000)->GetProcessRecord(1234) == nullptr);
    cgroupEventHandler->HandleExtensionStateChanged(1000, 1234, "com.ohos.test", "MainAbility",
        1111, (int32_t)AppExecFwk::ExtensionState::EXTENSION_STATE_TERMINATED,
        (int32_t)AppExecFwk::AbilityType::EXTENSION);

    supervisor_->RemoveApplication(1000);
    EXPECT_TRUE(supervisor_->GetAppRecord(1000) == nullptr);
    cgroupEventHandler->HandleExtensionStateChanged(1000, 1234, "com.ohos.test", "MainAbility",
        1111, (int32_t)AppExecFwk::ExtensionState::EXTENSION_STATE_TERMINATED,
        (int32_t)AppExecFwk::AbilityType::EXTENSION);
}

/**
 * @tc.name: CGroupSchedTest_CgroupEventHandler_027
 * @tc.desc: cgroup event handler Test
 * @tc.type: FUNC
 * @tc.require: issuesIB3UW9
 * @tc.desc:
 */
HWTEST_F(CGroupSchedTest, CGroupSchedTest_CgroupEventHandler_027, Function | MediumTest | Level1)
{
    auto cgroupEventHandler = std::make_shared<CgroupEventHandler>("CgroupEventHandler_unittest");
    cgroupEventHandler->HandleProcessDied(1000, 1234, "com.ohos.test");
    EXPECT_TRUE(supervisor_->GetAppRecord(1000) == nullptr);
    ProcessData processData;
    processData.uid = 1000;
    processData.pid = 1234;
    processData.bundleName = "com.ohos.test";
    processData.hostPid = 2024;
    processData.processType = static_cast<AppExecFwk::ProcessType>((int32_t)AppExecFwk::ProcessType::NORMAL);
    processData.extensionType = static_cast<AppExecFwk::ExtensionAbilityType>(INVALID_EXTENSION_TYPE);
    cgroupEventHandler->HandleProcessCreated(processData);
    EXPECT_TRUE(supervisor_->GetAppRecord(1000)->GetProcessRecord(1234) != nullptr);
    EXPECT_FALSE(supervisor_->GetAppRecord(1000)->GetProcessRecord(1234)->processType_ == ProcRecordType::RENDER);
    processData.pid = 23456;
    cgroupEventHandler->HandleProcessCreated(processData);
    processData.pid = 34567;
    cgroupEventHandler->HandleProcessCreated(processData);
    processData.pid = 45678;
    processData.processType = static_cast<AppExecFwk::ProcessType>((int32_t)AppExecFwk::ProcessType::EXTENSION);
    cgroupEventHandler->HandleProcessCreated(processData);
    processData.pid = 67890;
    processData.processType = static_cast<AppExecFwk::ProcessType>((int32_t)AppExecFwk::ProcessType::GPU);
    cgroupEventHandler->HandleProcessCreated(processData);
    cgroupEventHandler->HandleProcessDied(1000, 1234, "com.ohos.test");
    cgroupEventHandler->HandleProcessDied(1000, 23456, "com.ohos.test");
    cgroupEventHandler->HandleProcessDied(1000, 45678, "com.ohos.test");
    cgroupEventHandler->HandleProcessDied(1000, 67891, "com.ohos.test");
    cgroupEventHandler->HandleProcessDied(1000, 67890, "com.ohos.test");
    EXPECT_TRUE(supervisor_->GetAppRecord(1000) == nullptr);
}

/**
 * @tc.name: CGroupSchedTest_CgroupEventHandler_028
 * @tc.desc: cgroup event handler Test
 * @tc.type: FUNC
 * @tc.require: issuesIB3UW9
 * @tc.desc:
 */
HWTEST_F(CGroupSchedTest, CGroupSchedTest_CgroupEventHandler_028, Function | MediumTest | Level1)
{
    auto cgroupEventHandler = std::make_shared<CgroupEventHandler>("CgroupEventHandler_unittest");
    cgroupEventHandler->HandleTransientTaskStart(1000, 1234, "com.ohos.test");
    EXPECT_TRUE(supervisor_->GetAppRecord(1000) != nullptr);
    EXPECT_TRUE(supervisor_->GetAppRecord(1000)->GetProcessRecord(1234) == nullptr);
    cgroupEventHandler->HandleTransientTaskEnd(1000, 1234, "com.ohos.test");
    EXPECT_TRUE(supervisor_->GetAppRecord(1000) != nullptr);
    EXPECT_TRUE(supervisor_->GetAppRecord(1000)->GetProcessRecord(1234) == nullptr);

    ProcessData processData;
    processData.uid = 1000;
    processData.pid = 1234;
    processData.bundleName = "com.ohos.test";
    processData.hostPid = 2024;
    processData.processType = static_cast<AppExecFwk::ProcessType>((int32_t)AppExecFwk::ProcessType::NORMAL);
    processData.extensionType = static_cast<AppExecFwk::ExtensionAbilityType>(INVALID_EXTENSION_TYPE);
    cgroupEventHandler->HandleProcessCreated(processData);
    cgroupEventHandler->HandleTransientTaskStart(1000, 1234, "com.ohos.test");
    EXPECT_TRUE(supervisor_->GetAppRecord(1000)->GetProcessRecord(1234)->runningTransientTask_);
    cgroupEventHandler->HandleTransientTaskStart(1000, 1234, "com.ohos.test");
    EXPECT_TRUE(!supervisor_->GetAppRecord(1000)->GetProcessRecord(1234)->runningTransientTask_);
}

/**
 * @tc.name: CGroupSchedTest_CgroupEventHandler_029
 * @tc.desc: cgroup event handler Test
 * @tc.type: FUNC
 * @tc.require: issuesIB3UW9
 * @tc.desc:
 */
HWTEST_F(CGroupSchedTest, CGroupSchedTest_CgroupEventHandler_029, Function | MediumTest | Level1)
{
    auto cgroupEventHandler = std::make_shared<CgroupEventHandler>("CgroupEventHandler_unittest");
    int32_t abilityId = 1;
    cgroupEventHandler->HandleContinuousTaskCancel(1000, 1234,
        (int32_t)BackgroundTaskMgr::BackgroundMode::AUDIO_PLAYBACK, abilityId);
    EXPECT_TRUE(supervisor_->GetAppRecord(1000) != nullptr);
    EXPECT_TRUE(supervisor_->GetAppRecord(1000)->GetProcessRecord(1234) == nullptr);

    cgroupEventHandler->HandleContinuousTaskCancel(1000,
        1234, (int32_t)BackgroundTaskMgr::BackgroundMode::AUDIO_PLAYBACK, abilityId);
    auto proc = supervisor_->GetAppRecord(1000)->GetProcessRecord(1234);
    EXPECT_TRUE(proc->continuousTaskFlag_ == (1 << (int32_t)BackgroundTaskMgr::BackgroundMode::AUDIO_PLAYBACK));
    cgroupEventHandler->HandleContinuousTaskCancel(1000, 1234,
        (int32_t)BackgroundTaskMgr::BackgroundMode::AUDIO_PLAYBACK, abilityId);
    EXPECT_TRUE(proc->continuousTaskFlag_ == 0);

    cgroupEventHandler->HandleContinuousTaskCancel(1000, 1234,
        (int32_t)BackgroundTaskMgr::BackgroundMode::AUDIO_PLAYBACK, abilityId);
    EXPECT_TRUE(proc->continuousTaskFlag_ == 4);
}

/**
 * @tc.name: CGroupSchedTest_CgroupEventHandler_030
 * @tc.desc: cgroup event handler Test
 * @tc.type: FUNC
 * @tc.require: issuesIB3UW9
 * @tc.desc:
 */
HWTEST_F(CGroupSchedTest, CGroupSchedTest_CgroupEventHandler_030, Function | MediumTest | Level1)
{
    auto cgroupEventHandler = std::make_shared<CgroupEventHandler>("CgroupEventHandler_unittest");
    auto tmp = cgroupEventHandler->supervisor_;
    cgroupEventHandler->supervisor_ = nullptr;
    nlohmann::json payload = nlohmann::json::parse("{\"uid\": \"1111\", \"pid\": \"1112\"}");
    cgroupEventHandler->HandleSceneBoardState(ResType::RES_TYPE_REPORT_SCENE_BOARD, 1112, payload);
    cgroupEventHandler->supervisor_ = tmp;

    EXPECT_TRUE(cgroupEventHandler->supervisor_ != nullptr);
    payload = nlohmann::json::parse("{\"uid\": \"1111\"}");
    cgroupEventHandler->HandleSceneBoardState(ResType::RES_TYPE_REPORT_SCENE_BOARD, 1112, payload);

    payload = nlohmann::json::parse("{\"uid\": \"1111\","
        "\"pid\": \"1113\", \"tid\": \"1112\", \"bundleNum\": \"0\"}");
    cgroupEventHandler->HandleSceneBoardState(ResType::RES_TYPE_REPORT_SCENE_BOARD, 1112, payload);
    EXPECT_TRUE(cgroupEventHandler->supervisor_->sceneBoardPid_ == 1113);
}

/**
 * @tc.name: CGroupSchedTest_CgroupEventHandler_031
 * @tc.desc: cgroup event handler Test
 * @tc.type: FUNC
 * @tc.require: issuesIB3UW9
 * @tc.desc:
 */
HWTEST_F(CGroupSchedTest, CGroupSchedTest_CgroupEventHandler_031, Function | MediumTest | Level1)
{
    auto cgroupEventHandler = std::make_shared<CgroupEventHandler>("CgroupEventHandler_unittest");
    auto temp = cgroupEventHandler->supervisor_;
    cgroupEventHandler->supervisor_ = nullptr;
    nlohmann::json payload = nlohmann::json::parse("{\"pid\": \"429\", \"type\": \"1234\"}");
    cgroupEventHandler->HandleReportRunningLockEvent(ResType::RES_TYPE_RUNNINGLOCK_STATE, 0, payload);
    cgroupEventHandler->supervisor_ = temp;

    EXPECT_TRUE(cgroupEventHandler->supervisor_ != nullptr);
    payload = nlohmann::json::parse("{\"pid\": \"429\", \"type\": \"1234\"}");
    cgroupEventHandler->HandleReportRunningLockEvent(ResType::RES_TYPE_RUNNINGLOCK_STATE, 0, payload);

    payload = nlohmann::json::parse("{\"uid\": \"2024\", \"type\": \"1234\"}");
    cgroupEventHandler->HandleReportRunningLockEvent(ResType::RES_TYPE_RUNNINGLOCK_STATE, 0, payload);

    payload = nlohmann::json::parse("{\"uid\": \"0\", \"pid\": \"429\", \"type\": \"1234\"}");
    cgroupEventHandler->HandleReportRunningLockEvent(ResType::RES_TYPE_RUNNINGLOCK_STATE, 0, payload);

    payload = nlohmann::json::parse("{\"uid\": \"2024\", \"pid\": \"0\", \"type\": \"1234\"}");
    cgroupEventHandler->HandleReportRunningLockEvent(ResType::RES_TYPE_RUNNINGLOCK_STATE, 0, payload);

    payload = nlohmann::json::parse("{\"uid\": \"2024\", \"pid\": \"429\", \"type\": \"1234\"}");
    cgroupEventHandler->HandleReportRunningLockEvent(ResType::RES_TYPE_RUNNINGLOCK_STATE, 0, payload);
    auto app = cgroupEventHandler->supervisor_->GetAppRecordNonNull(2024);
    EXPECT_TRUE(app != nullptr);
    cgroupEventHandler->HandleReportRunningLockEvent(ResType::RES_TYPE_RUNNINGLOCK_STATE, 0, payload);
    auto proc = app->GetProcessRecordNonNull(429);
    EXPECT_TRUE(proc != nullptr);

    cgroupEventHandler->HandleReportRunningLockEvent(ResType::RES_TYPE_RUNNINGLOCK_STATE, 0, payload);
    EXPECT_FALSE(proc->runningLockState_[1234]);
    cgroupEventHandler->HandleReportRunningLockEvent(ResType::RES_TYPE_RUNNINGLOCK_STATE, 2, payload);
    EXPECT_FALSE(proc->runningLockState_[1234]);
    cgroupEventHandler->HandleReportRunningLockEvent(ResType::RES_TYPE_RUNNINGLOCK_STATE, 3, payload);
    EXPECT_FALSE(proc->runningLockState_[1234]);
}

/**
 * @tc.name: CGroupSchedTest_CgroupEventHandler_032
 * @tc.desc: cgroup event handler Test
 * @tc.type: FUNC
 * @tc.require: issuesIB3UW9
 * @tc.desc:
 */
HWTEST_F(CGroupSchedTest, CGroupSchedTest_CgroupEventHandler_032, Function | MediumTest | Level1)
{
    auto cgroupEventHandler = std::make_shared<CgroupEventHandler>("CgroupEventHandler_unittest");
    auto temp = cgroupEventHandler->supervisor_;
    cgroupEventHandler->supervisor_ = nullptr;
    nlohmann::json payload = nlohmann::json::parse("{\"pid\": \"429\", \"type\": \"1234\"}");
    cgroupEventHandler->HandleReportAvCodecEvent(ResType::RES_TYPE_RUNNINGLOCK_STATE, 0, payload);
    cgroupEventHandler->supervisor_ = temp;

    EXPECT_TRUE(cgroupEventHandler->supervisor_ != nullptr);
    payload = nlohmann::json::parse("{\"pid\": \"2024\", \"instanceId\": \"1234\"}");
    cgroupEventHandler->HandleReportAvCodecEvent(ResType::RES_TYPE_AV_CODEC_STATE, 0, payload);

    payload = nlohmann::json::parse("{\"uid\": \"2024\", \"instanceId\": \"1234\"}");
    cgroupEventHandler->HandleReportAvCodecEvent(ResType::RES_TYPE_AV_CODEC_STATE, 0, payload);

    payload = nlohmann::json::parse("{\"uid\": \"2024\", \"pid\": \"429\"}");
    cgroupEventHandler->HandleReportAvCodecEvent(ResType::RES_TYPE_AV_CODEC_STATE, 0, payload);

    payload = nlohmann::json::parse("{\"uid\": \"0\", \"pid\": \"429\", \"instanceId\": \"1234\"}");
    cgroupEventHandler->HandleReportAvCodecEvent(ResType::RES_TYPE_AV_CODEC_STATE, 0, payload);

    payload = nlohmann::json::parse("{\"uid\": \"2024\", \"pid\": \"0\", \"instanceId\": \"1234\"}");
    cgroupEventHandler->HandleReportAvCodecEvent(ResType::RES_TYPE_AV_CODEC_STATE, 0, payload);

    payload = nlohmann::json::parse("{\"uid\": \"2024\", \"pid\": \"429\", \"instanceId\": \"1234\"}");
    cgroupEventHandler->HandleReportAvCodecEvent(ResType::RES_TYPE_AV_CODEC_STATE, 0, payload);
    auto app = cgroupEventHandler->supervisor_->GetAppRecordNonNull(2024);
    EXPECT_TRUE(app != nullptr);
    cgroupEventHandler->HandleReportAvCodecEvent(ResType::RES_TYPE_AV_CODEC_STATE, 0, payload);
    auto proc = app->GetProcessRecordNonNull(429);
    EXPECT_TRUE(proc != nullptr);

    cgroupEventHandler->HandleReportAvCodecEvent(ResType::RES_TYPE_RUNNINGLOCK_STATE, 0, payload);
    EXPECT_TRUE(proc->avCodecState_[1234]);
    cgroupEventHandler->HandleReportAvCodecEvent(ResType::RES_TYPE_RUNNINGLOCK_STATE, 1, payload);
    EXPECT_FALSE(proc->runningLockState_[1234]);
}

/**
 * @tc.name: CGroupSchedTest_CgroupEventHandler_033
 * @tc.desc: cgroup event handler Test
 * @tc.type: FUNC
 * @tc.require: issuesIB3UW9
 * @tc.desc:
 */
HWTEST_F(CGroupSchedTest, CGroupSchedTest_CgroupEventHandler_033, Function | MediumTest | Level1)
{
    auto cgroupEventHandler = std::make_shared<CgroupEventHandler>("CgroupEventHandler_unittest");
    EXPECT_TRUE(cgroupEventHandler->supervisor_ != nullptr);
    nlohmann::json payload = nlohmann::json::parse("{\"uid\": \"2024\", \"pid\": \"429\"}");
    auto app = cgroupEventHandler->supervisor_->GetAppRecordNonNull(2024);
    EXPECT_TRUE(app != nullptr);
    auto proc = app->GetProcessRecordNonNull(429);
    EXPECT_TRUE(proc != nullptr);

    cgroupEventHandler->HandleReportWebviewAudioState(ResType::RES_TYPE_WEBVIEW_AUDIO_STATUS_CHANGE, 0, payload);
    EXPECT_TRUE(proc->videoState_);
    cgroupEventHandler->HandleReportWebviewAudioState(ResType::RES_TYPE_WEBVIEW_AUDIO_STATUS_CHANGE, 1, payload);
    EXPECT_TRUE(proc->videoState_);
}
} // namespace CgroupSetting
} // namespace ResourceSchedule
} // namespace OHOS
