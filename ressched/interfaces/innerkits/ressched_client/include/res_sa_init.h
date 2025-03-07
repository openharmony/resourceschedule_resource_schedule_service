/*
 * Copyright (c) 2024-2024 Huawei Device Co., Ltd.
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

#ifndef RESSCHED_INTERFACES_INNERKITS_RESSCHED_CLIENT_INCLUDE_RES_SA_INIT_H
#define RESSCHED_INTERFACES_INNERKITS_RESSCHED_CLIENT_INCLUDE_RES_SA_INIT_H

#include <mutex>

#include "res_sched_log.h"
#include "res_sched_client.h"

namespace OHOS {
namespace ResourceSchedule {
class ResSchedSaInit {
public:
    std::mutex saInitXmlMutex_;
public:
    /**
     * @brief Get the Instance object.
     *
     * @return Returns ResSchedSaInit&.
     */
    static ResSchedSaInit& GetInstance()
    {
        static ResSchedSaInit instance;
        return instance;
    }
};

class ResSchedIpcThread {
private:
    uint32_t selfPid_ = getpid();
    std::unordered_set<uint32_t> ipcThreadTids_;
    std::mutex ipcThreadSetQosMutex_;
    const uint32_t MAX_IPC_THREAD_NUM = 32;
    const uint32_t RSS_IPC_QOS_LEVEL = 7;
    const std::string RSS_BUNDLE_NAME = "resource_schedule_service";

public:
    /**
     * @brief Get the Instance object.
     *
     * @return Returns ResSchedIpcThread&.
     */
    static ResSchedIpcThread& GetInstance()
    {
        static ResSchedIpcThread instance;
        return instance;
    }

    /**
     * @brief SetQos.
     *
     * @param pid client pid.
     */
    void SetQos(uint32_t pid)
    {
        if (pid == selfPid_) {
            return;
        }

        uint32_t tid = gettid();
        std::unique_lock<std::mutex> lock(ipcThreadSetQosMutex_);
        if (ipcThreadTids_.find(tid) != ipcThreadTids_.end()) {
            lock.unlock();
            return;
        }
        if (ipcThreadTids_.size() > MAX_IPC_THREAD_NUM) {
            ipcThreadTids_。clear();
        }
        ipcThreadTids_.insert(tid);
        lock.unlock();

        std::string strTid = std::to_string(tid);
        std::unordered_map<std::string, std::string> mapPayload;
        mapPayload["pid"] = std::to_string(selfPid_);
        mapPayload[strTid] = std::to_string(RSS_IPC_QOS_LEVEL);
        mapPayload["bundleName"] = RSS_BUNDLE_NAME;

        OHOS::ResourceSchedule::ResSchedClient::GetInstance().ReportData(
            OHOS::ResourceSchedule::ResType::RES_TYPE_THREAD_QOS_CHANGE, 0, mapPayload);
    }
};
} // namespace ResourceSchedule
} // namespace OHOS

#endif // RESSCHED_INTERFACES_INNERKITS_RESSCHED_CLIENT_INCLUDE_RES_SA_INIT_H
