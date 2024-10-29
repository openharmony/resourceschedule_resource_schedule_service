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

using namespace testing::ext;
using namespace OHOS::ResourceSchedule::CgroupSetting;
using OHOS::Rosen::WindowModeType;
using OHOS::Rosen::IWindowModeChangedListener;

namespace OHOS {
namespace ResourceSchedule {
namespace {
    static const int32_t APP_START_UP = 0;
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
    EXPECT_TRUE(cgroupEventHandler->supervisor_->sceneBoardPid_, 1000);
}
} // namespace CgroupSetting
} // namespace ResourceSchedule
} // namespace OHOS
