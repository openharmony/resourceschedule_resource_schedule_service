/*
 * Copyright (c) 2025-2025 Huawei Device Co., Ltd.
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

#include "res_sa_init.h"

#include "ffrt.h"
#include "res_sched_client.h"
#include "res_sched_log.h"

namespace OHOS {
namespace ResourceSchedule {

ResSchedIpcThread& ResSchedIpcThread::GetInstance()
{
    static ResSchedIpcThread instance;
    return instance;
}

void ResSchedIpcThread::SetQos(int32_t clientPid)
{
    if (!isInit_.load()) {
        return;
    }

    if (clientPid == selfPid_) {
        return;
    }

    int32_t tid = gettid();
    int32_t pid = selfPid_;
    std::unique_lock<ffrt::mutex> lock(ipcThreadSetQosMutex_);
    if (ipcThreadTids_.find(tid) != ipcThreadTids_.end()) {
        lock.unlock();
        return;
    }
    if (ipcThreadTids_.size() > MAX_IPC_THREAD_NUM) {
        RESSCHED_LOGI("ResSchedIpcThread SetQos Thread size EXCEEDS LIMIT.");
        ipcThreadTids_.clear();
    }
    ipcThreadTids_.insert(tid);
    lock.unlock();

    // Throw out an ffrt task to prevent deadlocks
    ffrt::submit([this, tid, pid]() {
        std::string strTid = std::to_string(tid);
        std::unordered_map<std::string, std::string> mapPayload;
        mapPayload["pid"] = std::to_string(pid);
        mapPayload[strTid] = std::to_string(RSS_IPC_QOS_LEVEL);
        mapPayload["bundleName"] = RSS_BUNDLE_NAME;
        RESSCHED_LOGI("ResSchedIpcThread SetQos Thread tid=%{public}u.", tid);
        OHOS::ResourceSchedule::ResSchedClient::GetInstance().ReportData(
            OHOS::ResourceSchedule::ResType::RES_TYPE_THREAD_QOS_CHANGE, 0, mapPayload);
    });
}

void ResSchedIpcThread::SetInitFlag(bool flag)
{
    selfPid_ = getpid();
    isInit_ = flag;
}

} // namespace ResourceSchedule
} // namespace OHOS
