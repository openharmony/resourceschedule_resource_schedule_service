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

#include "fold_display_mode_observer.h"

#include "nlohmann/json.hpp"
#include "res_common_util.h"
#include "res_sched_log.h"
#include "res_sched_mgr.h"
#include "res_type.h"

namespace OHOS {
namespace ResourceSchedule {
using OHOS::Rosen::FoldDisplayMode;

namespace {
    const std::string DISPLAY_MODE_FULL = "displayFull";
    const std::string DISPLAY_MODE_SUB = "displaySub";
    const std::string DISPLAY_MODE_MAIN = "displayMain";
    const std::string DEVICE_MODE_STR = "deviceMode";
}

void FoldDisplayModeObserver::OnDisplayModeChanged(FoldDisplayMode diplayMode)
{
    // FULL SUB MAIN report enter the device mode, other display mode quit the current device mode
    RESSCHED_LOGD("Fold display mode %{public}d last %{public}s", diplayMode, currentDisplayMode.c_str());
    if (diplayMode == FoldDisplayMode::FULL) {
        ReportDisplayModeStatus(ResType::DeviceModeStatus::MODE_ENTER, DISPLAY_MODE_FULL);
        currentDisplayMode = DISPLAY_MODE_FULL;
    } else if (diplayMode == FoldDisplayMode::SUB) {
        ReportDisplayModeStatus(ResType::DeviceModeStatus::MODE_ENTER, DISPLAY_MODE_SUB);
        currentDisplayMode = DISPLAY_MODE_SUB;
    } else if (diplayMode == FoldDisplayMode::MAIN) {
        ReportDisplayModeStatus(ResType::DeviceModeStatus::MODE_ENTER, DISPLAY_MODE_MAIN);
        currentDisplayMode = DISPLAY_MODE_MAIN;
    } else {
        ReportDisplayModeStatus(ResType::DeviceModeStatus::MODE_QUIT, currentDisplayMode);
        currentDisplayMode = DISPLAY_MODE_UNKOWN;
    }
}

void FoldDisplayModeObserver::ReportDisplayModeStatus(int64_t status, const std::string& mode)
{
    nlohmann::json payload;
    payload[DEVICE_MODE_STR] = mode;
    ResSchedMgr::GetInstance().ReportData(ResType::RES_TYPE_DEVICE_MODE_STATUS, status, payload);
}
} // namespace ResourceSchedule
} // namespace OHOS