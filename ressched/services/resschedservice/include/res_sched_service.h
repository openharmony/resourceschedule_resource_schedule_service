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

    ErrCode ReportData(uint32_t resType, int64_t value, const std::string& payload) override;

    ErrCode ReportSyncEvent(uint32_t resType, int64_t value, const std::string& payload,
        std::string& reply, int32_t& resultValue) override;

    ErrCode KillProcess(const std::string& payload, int32_t& resultValue) override;

    ErrCode RegisterSystemloadNotifier(const sptr<IRemoteObject>& notifier) override;

    ErrCode UnRegisterSystemloadNotifier() override;

    ErrCode RegisterEventListener(const sptr<IRemoteObject>& eventListener, uint32_t eventType,
        uint32_t listenerGroup) override;

    ErrCode UnRegisterEventListener(uint32_t eventType, uint32_t listenerGroup) override;

    ErrCode GetSystemloadLevel(int32_t& resultValue) override;

    void OnDeviceLevelChanged(int32_t type, int32_t level);

    ErrCode IsAllowedAppPreload(const std::string& bundleName, int32_t preloadMode, bool& resultValue) override;

    void LoadAppPreloadPlugin();

    ErrCode IsAllowedLinkJump(bool isAllowedLinkJump, int32_t& resultValue) override;

    int32_t Dump(int32_t fd, const std::vector<std::u16string>& args) override;

    void DumpSystemLoadInfo(std::string &result);

    void DumpAllPluginConfig(std::string &result);

private:
    DISALLOW_COPY_AND_MOVE(ResSchedService);

    void DumpAllInfo(std::string &result);
    void DumpUsage(std::string &result);
    void DumpExt(const std::vector<std::string>& argsInStr, std::string &result);
    void DumpExecutorDebugCommand(const std::vector<std::string>& args, std::string& result);
    bool AllowDump();

    nlohmann::json StringToJsonObj(const std::string& str);
    int32_t CheckReportDataParcel(const uint32_t& type, const int64_t& value, const std::string& payload);
    bool IsLimitRequest(int32_t uid);
    void CheckAndUpdateLimitData(int64_t nowTime);
    void PrintLimitLog(int32_t uid);
    void ReportBigData();
    void InreaseBigDataCount();
    int32_t RemoteRequestCheck();

    OnIsAllowedAppPreloadFunc appPreloadFunc_ = nullptr;
    bool isLoadAppPreloadPlugin_ = false;
    using RequestFuncType = std::function<int32_t (MessageParcel& data, MessageParcel& reply)>;
    std::map<uint32_t, RequestFuncType> funcMap_;
    std::map<int32_t, int32_t> appRequestCountMap_;
    std::atomic<int32_t> allRequestCount_ {0};
    std::atomic<int32_t> bigDataReportCount_ {0};
    std::atomic<int64_t> nextCheckTime_ = {0};
    std::atomic<int64_t> nextReportBigDataTime_ = {0};
    std::atomic<bool> isReportBigData_ = {false};
    std::atomic<bool> isPrintLimitLog_ = {true};
    std::mutex mutex_;
};
} // namespace ResourceSchedule
} // namespace OHOS

#endif // RESSCHED_SERVICES_RESSCHEDSERVICE_INCLUDE_RES_SCHED_SERVICE_H
