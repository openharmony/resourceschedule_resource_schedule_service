/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef RESSCHED_INTERFACES_INNERKITS_RESSCHED_CLIENT_INCLUDE_RES_SCHED_CLIENT_H
#define RESSCHED_INTERFACES_INNERKITS_RESSCHED_CLIENT_INCLUDE_RES_SCHED_CLIENT_H

#include <cstdint>               // for int64_t, uint32_t
#include <unordered_map>         // for unordered_map
#include <mutex>                 // for mutex
#include <iosfwd>                // for string
#include "errors.h"              // for ErrCode
#include "iremote_object.h"      // for IRemoteObject, IRemoteObject::DeathR...
#include "ires_sched_service.h"  // for IResSchedService
#include "nlohmann/json.hpp"          // for Value
#include "nocopyable.h"          // for DISALLOW_COPY_AND_MOVE
#include "refbase.h"             // for sptr, wptr

namespace OHOS {
namespace ResourceSchedule {
/*
 * this class wraped the functions of IResSchedService,effect is the same.
 * but through ResSchedClient, you don't need to get IResSchedService from samgr,
 * just use the functions is ok.
 */
class ResSchedClient {
public:
    /**
     * Only need one client connect to ResSchedService, singleton pattern.
     *
     * @return Returns the only one implement of ResSchedClient.
     */
    static ResSchedClient& GetInstance();

    void ReportData(uint32_t resType, int64_t value, const std::unordered_map<std::string, std::string>& mapPayload);

    void StopRemoteObject();

    /**
     * Report source data to resource schedule service in the same process.
     *
     * @param resType Resource type, all of the type have listed in res_type.h.
     * @param value Value of resource type, defined by the developers.
     * @param payload is empty is valid. The interface only used for In-Process call.
     * ATTENTION: payload is empty is valid. The interface only used for In-Process call.
     */
    void ReportDataInProcess(uint32_t resType, int64_t value, const nlohmann::json& payload);

protected:
    ResSchedClient() = default;
    virtual ~ResSchedClient() = default;

private:
    class ResSchedDeathRecipient : public IRemoteObject::DeathRecipient {
    public:
        explicit ResSchedDeathRecipient(ResSchedClient &resSchedClient);

        ~ResSchedDeathRecipient();

        void OnRemoteDied(const wptr<IRemoteObject> &object) override;

    private:
        ResSchedClient &resSchedClient_;
    };
    ErrCode TryConnect();
    std::mutex mutex_;
    sptr<ResSchedDeathRecipient> recipient_;
    sptr<IRemoteObject> remoteObject_;
    sptr<IResSchedService> rss_;
    DISALLOW_COPY_AND_MOVE(ResSchedClient);
};
} // namespace ResourceSchedule
} // namespace OHOS

#endif // RESSCHED_INTERFACES_INNERKITS_RESSCHED_CLIENT_INCLUDE_RES_SCHED_CLIENT_H
