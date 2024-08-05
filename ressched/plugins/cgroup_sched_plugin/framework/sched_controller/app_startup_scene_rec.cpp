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

#include "res_type.h"
#include "ressched_utils.h"
#include "app_startup_scene_rec.h"
#include "cgroup_sched_log.h"
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
    if (exitContinuousStartupTask != nullptr) {
        ffrtQueue_->cancel(exitContinuousStartupTask);
        exitContinuousStartupTask = nullptr;
    }
    ffrtQueue_.reset();
    startPkgs_.clear();
    startUidSet_.clear();
    startIgnorePkgs_.clear();
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

void AppStartupSceneRec::RecordIsContinuousStartup(int32_t abilityState, std::string uid, std::string bundleName)
{
    if (!IsAppStartUp(abilityState)) {
        CGS_LOGE("abilityState is not app startUp");
        return;
    }
    if (startIgnorePkgs_.find(bundleName) != startIgnorePkgs_.end()) {
        CGS_LOGE("recordIsContinuousStartup bundleName: %{public}s is IgnorePkg", bundleName.c_str());
        return;
    }
    if (exitContinuousStartupTask != nullptr) {
        ffrtQueue_->cancel(exitContinuousStartupTask);
        exitContinuousStartupTask = nullptr;
    }
    auto tarEndTimePoint = std::chrono::system_clock::now();
    auto tarDuration = std::chrono::duration_cast<std::chrono::microseconds>(tarEndTimePoint.time_since_epoch());
    int64_t curTime = tarDuration.count();
    if (curTime - lastAppStartTime_ >= CONTINUOUS_START_TIME_OUT) {
        CleanRecordSceneData();
    }
    UpdateAppStartupNum(uid, curTime, bundleName);
    if (IsContinuousStartup() && !isReportContinuousStartup_.load()) {
        nlohmann::json payload;
        ResSchedUtils::GetInstance().ReportDataInProcess(ResType::RES_TYPE_CONTINUOUS_STARTUP,
            ResType::ContinuousStartupStatus::START_CONTINUOUS_STARTUP, payload);
        isReportContinuousStartup_ = true;
    }
    exitContinuousStartupTask = ffrtQueue_->submit_h([this] {
        CleanRecordSceneData();
    }, ffrt::task_attr().delay(CONTINUOUS_START_TIME_OUT));
}
void AppStartupSceneRec::CleanRecordSceneData()
{
    CGS_LOGI("CleanRecordSceneData");
    std::unique_lock<ffrt::mutex> lock(mutex_);
    appStartCount_ = 0;
    startPkgs_.clear();
    startUidSet_.clear();
    exitContinuousStartupTask = nullptr;
    if (isReportContinuousStartup_.load()) {
        nlohmann::json payload;
        ResSchedUtils::GetInstance().ReportDataInProcess(
            ResType::RES_TYPE_CONTINUOUS_STARTUP, ResType::ContinuousStartupStatus::STOP_CONTINUOUS_STARTUP, payload);
        isReportContinuousStartup_ = false;
    }
}
void AppStartupSceneRec::UpdateAppStartupNum(std::string uid, int64_t curTime, std::string bundleName)
{
    std::unique_lock<ffrt::mutex> lock(mutex_);
    lastAppStartTime_ = curTime;
    appStartCount_++;
    if (isReportContinuousStartup_.load()) {
        CGS_LOGI("UpdateAppStartupNum appStartCount_:%{public}d", appStartCount_);
        return;
    }
    startPkgs_.emplace_back(bundleName);
    startUidSet_.insert(uid);
}
bool AppStartupSceneRec::IsContinuousStartup()
{
    std::unique_lock<ffrt::mutex> lock(mutex_);
    if (startPkgs_.size() >= MAX_CONTINUOUS_START_NUM && startUidSet_.size() >= MAX_NO_REPEAT_APP_COUNT) {
        return true;
    }
    return false;
}
}
}