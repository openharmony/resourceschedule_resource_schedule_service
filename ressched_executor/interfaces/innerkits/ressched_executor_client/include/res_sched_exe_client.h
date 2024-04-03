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

#ifndef RESSCHED_EXECUTOR_INTERFACES_INNERKITS_RESSCHED_EXECUTOR_CLIENT_INCLUDE_RES_SCHED_EXE_CLIENT_H
#define RESSCHED_EXECUTOR_INTERFACES_INNERKITS_RESSCHED_EXECUTOR_CLIENT_INCLUDE_RES_SCHED_EXE_CLIENT_H

#include <cstdint>
#include <cstring>
#include <mutex>
#include <unordered_map>

#include "iremote_object.h"
#include "nocopyable.h"
#include "refbase.h"

#include "ires_sched_exe_service.h"

namespace OHOS {
namespace ResourceSchedule {
class ResSchedExeClient {
public:
    /**
     * @brief Get the Instance object.
     *
     * @return Returns ResSchedExeClient&.
     */
    static ResSchedExeClient& GetInstance();

    /**
     * @brief Send request data to the ressched_executor through inter-process communication.
     *
     * @param resType Indicates the resource type, all of the type have listed in res_type.h.
     * @param value Indicates the value of the resource type, defined by the developers.
     * @param context Indicates the context info of the resource type event.
     * @param reply Indicates the context info of the ipc reply.
     * @return function result
     */
    int32_t SendResRequest(uint32_t resType, int64_t value,
        const std::unordered_map<std::string, std::string>& context, std::string reply);

    /**
     * @brief Report resource data to the ressched_executor through inter-process communication.
     *
     * @param resType Indicates the resource type, all of the type have listed in res_type.h.
     * @param value Indicates the value of the resource type, defined by the developers.
     * @param context Indicates the context info of the resource type event.
     */
    void ReportData(uint32_t resType, int64_t value, const std::unordered_map<std::string, std::string>& context);

    /**
     * @brief Stop remote Object, reset ResSchedExeClient.
     */
    void StopRemoteObject();

    /**
     * @brief Send debug command for debug ipc.
     *
     * @param isSync is request sync
     */
    void SendDebugCommand(bool isSync);

protected:
    ResSchedExeClient() = default;
    virtual ~ResSchedExeClient();

private:
    class ResSchedExeDeathRecipient : public IRemoteObject::DeathRecipient {
    public:
        explicit ResSchedExeDeathRecipient(ResSchedExeClient &resSchedExeClient);

        ~ResSchedExeDeathRecipient();

        void OnRemoteDied(const wptr<IRemoteObject> &object) override;

    private:
        ResSchedExeClient &resSchedExeClient_;
    };

    int32_t TryConnect();
    int32_t SendRequestInner(bool isSync, uint32_t resType, int64_t value,
        const std::unordered_map<std::string, std::string>& context, std::string reply);
    nlohmann::json ConvertMapTojson(const std::unordered_map<std::string, std::string>& context);

    std::mutex mutex_;
    sptr<IRemoteObject> remoteObject_;
    sptr<IResSchedExeService> resSchedExe_;
    sptr<ResSchedExeDeathRecipient> recipient_;
    DISALLOW_COPY_AND_MOVE(ResSchedExeClient);
};
} // namespace ResourceSchedule
} // namespace OHOS

#endif // RESSCHED_EXECUTOR_INTERFACES_INNERKITS_RESSCHED_EXECUTOR_CLIENT_INCLUDE_RES_SCHED_EXE_CLIENT_H
