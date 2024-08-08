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

#ifndef CGROUP_SCHED_FRAMEWORK_SCHED_CONTROLLER_INCLUDE_APP_STARTUP_SCENE_REC_H
#define CGROUP_SCHED_FRAMEWORK_SCHED_CONTROLLER_INCLUDE_APP_STARTUP_SCENE_REC_H

#include <string>
#include <unordered_set>
#include <mutex>
#include <memory>
#include <atomic>
#include "ffrt.h"
#include "nlohmann/json.hpp"
namespace OHOS {
namespace ResourceSchedule {
class AppStartupSceneRec {
public:
    static AppStartupSceneRec &GetInstance();
    void Init();
    void Deinit();
    void RecordIsContinuousStartup(int32_t abilityState, std::string uid, std::string bundleName);
private:
    AppStartupSceneRec();
    ~AppStartupSceneRec();
    bool IsAppStartUp(int32_t abilityState);
    void CleanRecordSceneData();
    void UpdateAppStartupNum(std::string uid, int64_t curTime, std::string bundleName);
    bool IsContinuousStartup();
    int32_t appStartCount_ = 0;
    int64_t lastAppStartTime_ = 0;
    std::atomic_bool isReportContinuousStartup_ = false;
    std::vector<std::string> startPkgs_;
    std::unordered_set<std::string> startUidSet_;
    std::unordered_set<std::string> startIgnorePkgs_;
    std::shared_ptr<ffrt::queue> ffrtQueue_ = nullptr;
    ffrt::task_handle exitContinuousStartupTask;
    ffrt::mutex mutex_;
};
}
}
#endif // CGROUP_SCHED_FRAMEWORK_SCHED_CONTROLLER_INCLUDE_APP_STARTUP_SCENE_REC_H