/*
 * Copyright (c) 2024-2025 Huawei Device Co., Ltd.
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

#include "app_startup_scene_rec.h"

#include "res_sched_log.h"
#include "res_sched_mgr.h"
#include "res_type.h"

namespace OHOS {
namespace ResourceSchedule {
static const int32_t CONTINUOUS_START_TIME_OUT = 15 * 1000 * 1000;
static const int32_t MAX_NO_REPEAT_APP_COUNT = 4;
static const int32_t MAX_CONTINUOUS_START_NUM = 5;
static const int32_t APP_START_UP = 0;
static const std::string RUNNER_NAME = "AppStartupSceneRecQueue";
AppStartupSceneRec::AppStartupSceneRec()
{
}
AppStartupSceneRec::~AppStartupSceneRec()
{
}
void AppStartupSceneRec::Init()
{
    ffrtQueue_ = std::make_shared<ffrt::queue>(RUNNER_NAME.c_str(),
        ffrt::queue_attr().qos(ffrt::qos_user_initiated));
}
void AppStartupSceneRec::Deinit()
{
    std::lock_guard<ffrt::mutex> lock(mutex_);
    if (exitContinuousStartupTask != nullptr) {
        ffrtQueue_->cancel(exitContinuousStartupTask);
        exitContinuousStartupTask = nullptr;
    }
    ffrtQueue_.reset();
    startPkgs_.clear();
    startUidSet_.clear();
}
AppStartupSceneRec& AppStartupSceneRec::GetInstance()
{
    static AppStartupSceneRec instance;
    return instance;
}

bool AppStartupSceneRec::IsAppStartUp(int32_t abilityState)
{
    return abilityState == APP_START_UP;
}

void AppStartupSceneRec::RecordIsContinuousStartup(const std::string& uid, const std::string& bundleName)
{
    auto tarEndTimePoint = std::chrono::steady_clock::now();
    auto tarDuration = std::chrono::duration_cast<std::chrono::microseconds>(tarEndTimePoint.time_since_epoch());
    int64_t curTime = tarDuration.count();
    std::lock_guard<ffrt::mutex> lock(mutex_);
    if (exitContinuousStartupTask != nullptr) {
        ffrtQueue_->cancel(exitContinuousStartupTask);
        exitContinuousStartupTask = nullptr;
    }
    if (curTime - lastAppStartTime_ >= CONTINUOUS_START_TIME_OUT) {
        CleanRecordSceneData();
    }
    UpdateAppStartupNum(uid, curTime, bundleName);
    if (IsContinuousStartup() && !isReportContinuousStartup_.load()) {
        nlohmann::json payload;
        ResSchedMgr::GetInstance().ReportData(
            ResType::RES_TYPE_CONTINUOUS_STARTUP, ResType::ContinuousStartupStatus::START_CONTINUOUS_STARTUP, payload);
        isReportContinuousStartup_ = true;
    }
    exitContinuousStartupTask = ffrtQueue_->submit_h([this] {
        std::lock_guard<ffrt::mutex> lock(mutex_);
        CleanRecordSceneData();
    }, ffrt::task_attr().delay(CONTINUOUS_START_TIME_OUT));
}
void AppStartupSceneRec::CleanRecordSceneData()
{
    RESSCHED_LOGD("CleanRecordSceneData");
    appStartCount_ = 0;
    startPkgs_.clear();
    startUidSet_.clear();
    exitContinuousStartupTask = nullptr;
    if (isReportContinuousStartup_.load()) {
        nlohmann::json payload;
        ResSchedMgr::GetInstance().ReportData(
            ResType::RES_TYPE_CONTINUOUS_STARTUP, ResType::ContinuousStartupStatus::STOP_CONTINUOUS_STARTUP, payload);
        isReportContinuousStartup_ = false;
    }
}
void AppStartupSceneRec::UpdateAppStartupNum(const std::string uid, const int64_t curTime,
    const std::string bundleName)
{
    lastAppStartTime_ = curTime;
    appStartCount_++;
    if (isReportContinuousStartup_.load()) {
        RESSCHED_LOGD("UpdateAppStartupNum appStartCount_:%{public}d", appStartCount_);
        return;
    }
    startPkgs_.emplace_back(bundleName);
    startUidSet_.insert(uid);
}
bool AppStartupSceneRec::IsContinuousStartup()
{
    if (startPkgs_.size() >= MAX_CONTINUOUS_START_NUM && startUidSet_.size() >= MAX_NO_REPEAT_APP_COUNT) {
        return true;
    }
    return false;
}
}
}