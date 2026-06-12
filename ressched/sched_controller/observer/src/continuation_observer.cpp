/*
 * Copyright (c) 2026-2026 Huawei Device Co., Ltd.
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

#include "continuation_observer.h"

#include "nlohmann/json.hpp"
#include "res_common_util.h"
#include "res_sched_log.h"
#include "res_sched_mgr.h"
#include "res_type.h"

namespace OHOS {
namespace ResourceSchedule {
namespace {
    static constexpr int32_t CONTINUATION_STATE = 0;
}
void ContinuationObserver::OnCallback(const AAFwk::OnCallbackInfo &info)
{
    RESSCHED_LOGI("ContinuationObserver OnCallback, bundleName:%{public}s", info.bundleName.c_str());
    nlohmann::json payload;
    payload["continueState"] = std::to_string(info.continueState);
    payload["bundleName"] = info.bundleName;
    payload["continueType"] = info.continueType;
    ResSchedMgr::GetInstance().ReportData(
        ResType::RES_TYPE_DISTRIBUTED_CONTINUATION_EVENT, CONTINUATION_STATE, payload);
}
} // namespace ResourceSchedule
} // namespace OHOS
