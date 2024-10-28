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

#include "batch_log_printer.h"
#include "res_common_util.h"
#include "res_sched_log.h"

namespace OHOS {
namespace ResourceSchedule {
namespace {
    static constexpr char QUEUE_NAME[] = "batch_log_printer_queue";
    static constexpr int32_t PRINT_SIZE = 64;
    static constexpr int32_t BATCH_SIZE = 3000;
}

IMPLEMENT_SINGLE_INSTANCE(BatchLogPrinter);

BatchLogPrinter::BatchLogPrinter()
{
    logQueue_ = std::make_shared<ffrt::queue>(QUEUE_NAME, ffrt::queue_attr().qos(ffrt::qos_default));
}

void BatchLogPrinter::SubmitLog(const std::string& log)
{
    if (logQueue_ == nullptr) {
        return;
    }
    int64_t timestamp = ResCommonUtil::GetCurrentTimestamp();
    logQueue_->submit([log, timestamp]() {
        BatchLogPrinter::GetInstance().RecordLog(log, timestamp);
    });
}

void BatchLogPrinter::RecordLog(const std::string& log, const int64_t& timestamp)
{
    std::string recordLog = ResCommonUtil::ConvertTimestampToStr(timestamp) + ":" + log;
    allLogs_.push_back(recordLog);
    if (allLogs_.size() >= PRINT_SIZE) {
        std::vector<std::string> batchLogs = std::vector<std::string>();
        GetBatch(batchLogs);
        for (auto& item : batchLogs) {
            RESSCHED_LOGI("BatchLong:%{public}s", item.c_str());
        }
    }
}

void BatchLogPrinter::GetBatch(std::vector<std::string>& batchLogs)
{
    std::string batch;
    for (auto& item : allLogs_) {
        if (batch.length() + item.length() > BATCH_SIZE) {
            batchLogs.push_back(batch);
            batch = "";
        }
        if (batch.length() != 0) {
            batch.append("|");
        }
        batch.append(item);
    }
    if (!batch.empty()) {
        batchLogs.push_back(batch);
    }
    allLogs_.clear();
}
} // namespace ResourceSchedule
} // namespace OHOS