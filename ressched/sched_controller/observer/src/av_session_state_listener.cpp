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

#include "av_session_state_listener.h"

#include "res_sched_log.h"
#include "res_sched_mgr.h"
#include "ressched_utils.h"
#include "res_type.h"

namespace OHOS {
namespace ResourceSchedule {
void AvSessionStateListener::MarshallingAVSessionDescriptor(const AVSession::AVSessionDescriptor& descriptor,
    nlohmann::json &payload)
{
    payload["sessionId_"] = descriptor.sessionId_;
    payload["pid_"] = descriptor.pid_;
    payload["uid_"] = descriptor.uid_;
}

void AvSessionStateListener::OnSessionCreate(const AVSession::AVSessionDescriptor& descriptor)
{
    RESSCHED_LOGD("Ressched AvSessionStateListener OnSessionCreate");
    nlohmann::json payload;
    MarshallingAVSessionDescriptor(descriptor, payload);
    ResSchedUtils::GetInstance().ReportDataInProcess(
        ResType::RES_TYPE_AV_SESSION_ON_SESSION_CREATE, 0, payload);
}

void AvSessionStateListener::OnSessionRelease(const AVSession::AVSessionDescriptor& descriptor)
{
    RESSCHED_LOGD("Ressched AvSessionStateListener OnSessionRelease");
    nlohmann::json payload;
    MarshallingAVSessionDescriptor(descriptor, payload);
    ResSchedUtils::GetInstance().ReportDataInProcess(
        ResType::RES_TYPE_AV_SESSION_ON_SESSION_RELEASE, 0, payload);
}

void AvSessionStateListener::OnTopSessionChange(const AVSession::AVSessionDescriptor& descriptor)
{
    RESSCHED_LOGD("Ressched AvSessionStateListener OnTopSessionChange");
    nlohmann::json payload;
    MarshallingAVSessionDescriptor(descriptor, payload);
    ResSchedUtils::GetInstance().ReportDataInProcess(
        ResType::RES_TYPE_AV_SESSION_ON_TOP_SESSION_CHANGE, 0, payload);
}
} // OHOS
} // namespace ResourceSchedule