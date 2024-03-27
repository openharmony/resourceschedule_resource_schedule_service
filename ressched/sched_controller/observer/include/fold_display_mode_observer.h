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

#ifndef RESSCHED_SCHED_CONTROLLER_OBSERVER_INCLUDE_FOLD_DISPLAY_MODE_OBSERVER_H
#define RESSCHED_SCHED_CONTROLLER_OBSERVER_INCLUDE_FOLD_DISPLAY_MODE_OBSERVER_H

#include <string>
#include "dm_common.h"
#include "display_manager.h"

namespace OHOS {
namespace ResourceSchedule {
using OHOS::Rosen::FoldDisplayMode;

namespace {
    const std::string DISPLAY_MODE_UNKOWN = "displayUnknown";
}
class FoldDisplayModeObserver : public OHOS::Rosen::DisplayManager::IDisplayModeListener {
public:
    /**
     * @brief Called back when display mode changed.
     *
     * @param diplayMode diplay mode UNKONWN = 0, FULL = 1, MAIN = 2, SUB = 3 ...
    */
    void OnDisplayModeChanged(FoldDisplayMode diplayMode) override;

private:
    std::string currentDisplayMode = DISPLAY_MODE_UNKOWN;
    void ReportDisplayModeStatus(int64_t status, const std::string& mode);
};
} // namespace ResourceSchedule
} // namespace OHOS
#endif // RESSCHED_SCHED_CONTROLLER_OBSERVER_INCLUDE_FOLD_DISPLAY_MODE_OBSERVER_H