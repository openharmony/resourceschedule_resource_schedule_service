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
static const std::string RUNNER_NAME = "AppStartUpSceneRecQueue";
class AppStartupSceneRec {
AppStartupSceneRec::AppStartupSceneRec()
{
    ffrtQueue_ = std::make_shared<ffrt::queue>(RUNNER_NAME.c_str(),
        ffrt::queue_attr().qos(ffrt::qos_user_initiated));
}
AppStartupSceneRec::~AppStartupSceneRec()
{
    exitContinuousStartUpTask = nullptr;
    ffrtQueue_ = nullptr;
    startPkgs_.clear();
    startUidSet_.clear();
    startIgnorePkgs_.clear();
}
AppStartupSceneRec& AppStartupSceneRec::GetInstance()
{
    static AppStartupSceneRec instance;
    return instance;
}

void AppStartupSceneRec::RecordIsContinuousStartUp(std::string uid, std::string bundleName)
{
    if (startIgnorePkg_.find(bundleName) != startIgnorePkgs_.end()) {
        CGS_LOGE("recordIsContinuousStartUp bundleName: %{public}s is IgnorePkg", bundleName.c_str());
        return;
    }
    if (exitContinuousStartUpTask != nullptr) {
        ffrtQueue_->cancel(exitContinuousStartUpTask);
    }
    auto tarEndTimePoint = std::chrono:system_clock::now();
    auto tarDuration = std::chrono::duration_cast<std::chrono::microseconds>(tarEndTimePoint.time_since_epoch());
    int64_t curTime = tarDuration.count();
    CGS_LOGI("recordIsContinuousStartUp uid: %{public}s bundleName: %{public}s curTime:%{public}ld",
        uid.c_str(), bundleName.c_str(), curTime);
    if (curTime - lastAppStartTime_ >= CONTINUOUS_START_TIME_OUT) {
        cleanRecordSceneData();
    }
    updateAppStartupNum(uid, curTime, bundleName);
    if (isContinuousStartUp()) {
        if (isReportContinuousStartUp_.load()) {
            return;
        }
        nlohmann::json payload;
        ResSchedUtils::GetInstance().ReportDataInProcess(
            ResType::RES_TYPE_CONTINUOUS_STARTUP, ResType::ContinuousStartUpStatus::START_CONTINUOUS_STARTUP, payload);
        isReportContinuousStartUp_ = true;
    }
    exitContinuousStartUpTask = ffrtQueue_->submit_h([this]) {
        cleanRecordSceneData();
    }, ffrt_task_attr().delay(CONTINUOUS_START_TIME_OUT);
}
void AppStartupSceneRec::CleanRecordSceneData()
{
    CGS_LOGI("cleanRecordSceneData");
    std::unique_lock<ffrt_mutex> lock(mutex_);
    lastStartUid_ = "";
    startPkgs_.clear();
    startUidSet_.clear();
    startIgnorePkgs_.clear();
    if (isReportContinuousStartUp_.load()) {
        nlohmann::json payload;
        ResSchedUtils::GetInstance().ReportDataInProcess(
            ResType::RES_TYPE_CONTINUOUS_STARTUP, ResType::ContinuousStartUpStatus::STOP_CONTINUOUS_STARTUP, payload);
        isReportContinuousStartUp_ = false;
    }
}
void AppStartupSceneRec::UpdateAppStartupNum(std::string uid, int64_t curTime, std::string bundleName)
{
    std::unique_lock<ffrt_mutex> lock(mutex_);
    lastAppStartTime_ = curTime;
    if (lastStartUid_ == uid) {
        CGS_LOGE("same uid: %{public}s, not update app startUp", uid.c_str());
        return;
    }
    lastStartUid_ = uid;
    startPkgs_.emplace_back(bundleName);
    startUidSet_.insert(uid);
}
bool AppStartupSceneRec::IsContinuousStartUp()
{
    std::unique_lock<ffrt_mutex> lock(mutex_);
    if (startPkgs_.size() >= MAX_CONTINUOUS_START_NUM && startUidSet_.size() >= MAX_NO_REPEAT_APP_COUNT) {
        return true;
    }
    return false;
}
}
}
}