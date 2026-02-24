/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef RESSCHED_COMMON_INCLUDE_RES_SCHED_EVENT_REPORTER_H
#define RESSCHED_COMMON_INCLUDE_RES_SCHED_EVENT_REPORTER_H

#include <string>
#include <unistd.h>
#include <unordered_set>
#include <vector>

#include "ffrt.h"
#include "single_instance.h"

namespace OHOS {
namespace ResourceSchedule {
class ResschedEventReporter {
    DECLARE_SINGLE_INSTANCE(ResschedEventReporter);
public:
    
    /**
    * @brief report file size event,the files must in path '/data'.
    *
    * @param filePaths the file's path will report size
    *
    */
    void ReportFileSizeEvent(const std::vector<std::string>& filePaths);
    
    /**
    * @brief report file size event,the files must in path '/data'.
    *
    * @param filePaths the file's path will report size
    *
    */
    void ReportFileSizeEvent(const std::string& filePath);
private:
    void SubmitDelayTask(int64_t delayTime);
    void ReportFileSizeDaily();
    void ReportFileSizeInner();
    int64_t ValidReportTime(int64_t delayTime);

    std::unordered_set<std::string> allFilePaths_;
    ffrt::mutex mutex_;
    bool isTaskSubmit_ = false;
    std::string fileSizeRecorderName_;
};
} // namespace ResourceSchedule
} // namespace OHOS

#endif // RESSCHED_COMMON_INCLUDE_RES_SCHED_EVENT_REPORTER_H