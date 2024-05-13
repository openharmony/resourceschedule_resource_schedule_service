/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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

#ifndef RESSCHED_SERVICES_RESSCHEDSERVICE_INCLUDE_RES_SCHED_SERVICE_H
#define RESSCHED_SERVICES_RESSCHEDSERVICE_INCLUDE_RES_SCHED_SERVICE_H

#include "plugin_mgr.h"
#include "res_sched_service_stub.h"

namespace OHOS {
namespace ResourceSchedule {
class ResSchedService : public ResSchedServiceStub {
public:
    ResSchedService() = default;
    ~ResSchedService() override = default;

    void ReportData(uint32_t resType, int64_t value, const nlohmann::json& payload) override;

    int32_t KillProcess(const nlohmann::json& payload) override;

    void RegisterSystemloadNotifier(const sptr<IRemoteObject>& notifier) override;

    void UnRegisterSystemloadNotifier() override;

    int32_t GetSystemloadLevel() override;

    void OnDeviceLevelChanged(int32_t type, int32_t level);

    bool IsAllowedAppPreload(const std::string& bundleName, int32_t preloadMode) override;

    void LoadAppPreloadPlugin();

    int32_t Dump(int32_t fd, const std::vector<std::u16string>& args) override;

    void DumpProcessRunningLock(std::string& result);

    void DumpProcessWindowInfo(std::string &result);

    void DumpProcessEventState(std::string &result);

    void DumpSystemLoadInfo(std::string &result);

private:
    DISALLOW_COPY_AND_MOVE(ResSchedService);

    void DumpAllInfo(std::string &result);
    void DumpUsage(std::string &result);
    bool AllowDump();

    OnIsAllowedAppPreloadFunc appPreloadFunc_ = nullptr;
    bool isLoadAppPreloadPlugin_ = false;
};
} // namespace ResourceSchedule
} // namespace OHOS

#endif // RESSCHED_SERVICES_RESSCHEDSERVICE_INCLUDE_RES_SCHED_SERVICE_H
