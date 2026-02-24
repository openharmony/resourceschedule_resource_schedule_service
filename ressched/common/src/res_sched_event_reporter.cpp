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

#include "res_sched_event_reporter.h"

#include "file_ex.h"
#include "hisysevent.h"
#include "res_sched_log.h"
#include "res_common_util.h"


namespace OHOS {
namespace ResourceSchedule {
namespace {
static const int64_t FIRST_REPORT_TIME = 10 * 1000 * 1000;
static const char* FILE_SIZE_REPORTER_RECORDER = "/data/service/el1/public/ressched/file_size_report_time";
static const int64_t ONE_DAY_MICRO_SECOND = static_cast<int64_t>(24) * 60 * 60 * 1000 * 1000;
static const char* DATA_FILE_PATH = "/data";
}

IMPLEMENT_SINGLE_INSTANCE(ResschedEventReporter);

void ResschedEventReporter::ReportFileSizeEvent(const std::vector<std::string>& filePaths)
{
    std::lock_guard<ffrt::mutex> autoLock(mutex_);
    if (!filePaths.empty()) {
        allFilePaths_.insert(filePaths.begin(), filePaths.end());
    }
    if (!isTaskSubmit_) {
        allFilePaths_.insert(FILE_SIZE_REPORTER_RECORDER);
        SubmitDelayTask(FIRST_REPORT_TIME);
        isTaskSubmit_ = true;
    }
}

void ResschedEventReporter::ReportFileSizeEvent(const std::string& filePath)
{
    std::vector<std::string> tmpVector;
    tmpVector.emplace_back(filePath);
    ReportFileSizeEvent(tmpVector);
}

void ResschedEventReporter::SubmitDelayTask(int64_t delayTime)
{
    ffrt::submit([]() {
        ResschedEventReporter::GetInstance().ReportFileSizeDaily();
        }, ffrt::task_attr().delay(delayTime));
}

void ResschedEventReporter::ReportFileSizeDaily()
{
    std::string lastReportTime;
    LoadStringFromFile(FILE_SIZE_REPORTER_RECORDER, lastReportTime);
    if (lastReportTime.empty()) {
        ReportFileSizeInner();
    } else {
        int64_t lastReportTimeValue = -1;
        if (!ResCommonUtil::StrToInt64(lastReportTime, lastReportTimeValue) ||
            lastReportTimeValue < 0) {
            ReportFileSizeInner();
        } else {
            int64_t nowTime = ResCommonUtil::GetNowMicroTime();
            if (nowTime - lastReportTimeValue < ONE_DAY_MICRO_SECOND) {
                int64_t nextReportTime = ONE_DAY_MICRO_SECOND - (nowTime- lastReportTimeValue);
                nextReportTime = ValidReportTime(nextReportTime);
                SubmitDelayTask(nextReportTime);
            } else {
                ReportFileSizeInner();
            }
        }
    }
}

int64_t ResschedEventReporter::ValidReportTime(int64_t delayTime)
{
    // Avoid the impact of users actively modifying system time
    if (delayTime < 0) {
        return 0;
    } else if (delayTime > ONE_DAY_MICRO_SECOND) {
        return ONE_DAY_MICRO_SECOND;
    } else {
        return delayTime;
    }
}

void ResschedEventReporter::ReportFileSizeInner()
{
    RESSCHED_LOGI("ReportFileSizeInner call");
    std::lock_guard<ffrt::mutex> autoLock(mutex_);
    std::vector<std::string> files;
    std::vector<uint64_t> filesSize;
    for (const auto& item : allFilePaths_) {
        files.emplace_back(item);
        filesSize.emplace_back(ResCommonUtil::GetFileSize(item));
    }
    double remainSize = ResCommonUtil::GetDeviceVailidSize(DATA_FILE_PATH);
    HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::FILEMANAGEMENT, "USER_DATA_SIZE",
        HiviewDFX::HiSysEvent::EventType::STATISTIC,
        "COMPONENT_NAME", "resource_schedule_service",
        "PARTITION_NAME", DATA_FILE_PATH,
        "REMAIN_PARTITION_SIZE", remainSize,
        "FILE_OR_FOLDER_PATH", files,
        "FILE_OR_FOLDER_SIZE", filesSize);
    int64_t nowMicroTime = ResCommonUtil::GetNowMicroTime();
    SaveStringToFile(FILE_SIZE_REPORTER_RECORDER, std::to_string(nowMicroTime));
    SubmitDelayTask(ONE_DAY_MICRO_SECOND);
}
} // namespace ResourceSchedule
} // namespace OHOS