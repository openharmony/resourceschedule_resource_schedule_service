/*
 * Copyright (c) 2023-2023 Huawei Device Co., Ltd.
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
#include "connection_subscriber.h"

#include "res_sched_log.h"
#include "res_sched_mgr.h"
#include "ressched_utils.h"
#include "res_type.h"

namespace OHOS {
namespace ResourceSchedule {
void ConnectionSubscriber::MarshallingConnectionData(
    const AbilityRuntime::ConnectionData& data, nlohmann::json &payload)
{
    payload["extensionPid"] = data.extensionPid;
    payload["extensionUid"] = data.extensionUid;
    payload["extensionBundleName"] = data.extensionBundleName;
    payload["extensionModuleName"] = data.extensionModuleName;
    payload["extensionName"] = data.extensionName;
    payload["extensionType"] = static_cast<uint32_t>(data.extensionType);
    payload["callerUid"] = data.callerUid;
    payload["callerPid"] = data.callerPid;
    payload["callerName"] = data.callerName;
}

void ConnectionSubscriber::OnExtensionConnected(const AbilityRuntime::ConnectionData& data)
{
    RESSCHED_LOGD("Ressched ConnectionSubscriber OnExtensionConnected");
    nlohmann::json payload;
    MarshallingConnectionData(data, payload);
    ResSchedUtils::GetInstance().ReportDataInProcess(
        ResType::RES_TYPE_CONNECTION_OBSERVER, ResType::ConnectionObserverStatus::EXTENSION_CONNECTED, payload);
}

void ConnectionSubscriber::OnExtensionDisconnected(const AbilityRuntime::ConnectionData& data)
{
    RESSCHED_LOGD("Ressched ConnectionSubscriber OnExtensionDisconnected");
    nlohmann::json payload;
    MarshallingConnectionData(data, payload);
    ResSchedUtils::GetInstance().ReportDataInProcess(
        ResType::RES_TYPE_CONNECTION_OBSERVER, ResType::ConnectionObserverStatus::EXTENSION_DISCONNECTED, payload);
}

void ConnectionSubscriber::MarshallingDlpStateData(const AbilityRuntime::DlpStateData& data, nlohmann::json &payload)
{
    payload["targetPid"] = data.targetPid;
    payload["targetUid"] = data.targetUid;
    payload["targetBundleName"] = data.targetBundleName;
    payload["targetModuleName"] = data.targetModuleName;
    payload["targetAbilityName"] = data.targetAbilityName;
    payload["callerUid"] = data.callerUid;
    payload["callerPid"] = data.callerPid;
    payload["callerName"] = data.callerName;
}

void ConnectionSubscriber::OnDlpAbilityOpened(const AbilityRuntime::DlpStateData& data)
{
    RESSCHED_LOGD("Ressched ConnectionSubscriber OnDlpAbilityOpened");
    nlohmann::json payload;
    MarshallingDlpStateData(data, payload);
    ResSchedUtils::GetInstance().ReportDataInProcess(
        ResType::RES_TYPE_CONNECTION_OBSERVER, ResType::ConnectionObserverStatus::DLP_ABILITY_OPENED, payload);
}

void ConnectionSubscriber::OnDlpAbilityClosed(const AbilityRuntime::DlpStateData& data)
{
    RESSCHED_LOGD("Ressched ConnectionSubscriber OnDlpAbilityClosed");
    nlohmann::json payload;
    MarshallingDlpStateData(data, payload);
    ResSchedUtils::GetInstance().ReportDataInProcess(
        ResType::RES_TYPE_CONNECTION_OBSERVER, ResType::ConnectionObserverStatus::DLP_ABILITY_CLOSED, payload);
}

void ConnectionSubscriber::OnServiceDied()
{}
} // namespace ResourceSchedule
} // namespace OHOS
