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

#include "sched_telephony_observer.h"

#include "ressched_utils.h"
#include "res_sched_log.h"
#include "res_type.h"
namespace OHOS {
namespace ResourceSchedule {
void SchedTelephonyObserver::OnCallStateUpdated(int32_t slotId, int32_t callState, const std::u16string &phoneNumber)
{
    RESSCHED_LOGD("SchedTelephonyObserver::OnCallStateUpdated callState is %{public}d", callState);
    const nlohmann::json payload = nlohmann::json::object();
    ResSchedUtils::GetInstance().ReportDataInProcess(ResType::RES_TYPE_CALL_STATE_UPDATE, callState, payload);
}
} // namespace ResourceSchedule
} // namespace OHOS