/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "mmi_observer.h"

#include "nlohmann/json.hpp"
#include "res_sched_log.h"
#include "res_sched_mgr.h"
#include "res_type.h"

namespace OHOS {
namespace ResourceSchedule {
namespace {
    constexpr int32_t MMI_STATE = 0;
}
void MmiObserver::SyncBundleName(int32_t pid, int32_t uid, std::string bundleName)
{
    if (bundleName.size() == 0) {
        RESSCHED_LOGE("mmi sync bundleName error!");
        return;
    }
    RESSCHED_LOGI("mmi sync bundleName, pid:%{public}d, uid:%{public}d, bundleName:%{public}s", pid, uid,
        bundleName.c_str());
    nlohmann::json payload;
    payload["pid"] = pid;
    payload["uid"] = uid;
    payload["bundleName"] = bundleName;
    ResSchedMgr::GetInstance().ReportData(ResType::RES_TYPE_MMI_INPUT_STATE, MMI_STATE, payload);
}
} // namespace ResourceSchedule
} // namespace OHOS