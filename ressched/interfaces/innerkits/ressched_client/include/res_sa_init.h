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

#ifndef RESSCHED_INTERFACES_INNERKITS_RESSCHED_CLIENT_INCLUDE_RES_SA_INIT_H
#define RESSCHED_INTERFACES_INNERKITS_RESSCHED_CLIENT_INCLUDE_RES_SA_INIT_H

#include <mutex>
#include <unordered_set>

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
    static ResSchedSaInit& GetInstance();
};

class ResSchedIpcThread {
private:
    uint32_t selfPid_;
    std::unordered_set<uint32_t> ipcThreadTids_;
    std::mutex ipcThreadSetQosMutex_;
    std::atomic<bool> isInit_ = {false};
    const uint32_t MAX_IPC_THREAD_NUM = 32;
    const uint32_t RSS_IPC_QOS_LEVEL = 7;
    const std::string RSS_BUNDLE_NAME = "resource_schedule_service";

public:
    /**
     * @brief Get the Instance object.
     *
     * @return Returns ResSchedIpcThread&.
     */
    static ResSchedIpcThread& GetInstance();

    /**
     * @brief SetQos.
     *
     * @param pid client pid.
     */
    void SetQos(uint32_t pid);

    /**
     * @brief SetInitFlag.
     *
     * @param flag init finish flag.
     */
     void SetInitFlag(bool flag);
};
} // namespace ResourceSchedule
} // namespace OHOS

#endif // RESSCHED_INTERFACES_INNERKITS_RESSCHED_CLIENT_INCLUDE_RES_SA_INIT_H
