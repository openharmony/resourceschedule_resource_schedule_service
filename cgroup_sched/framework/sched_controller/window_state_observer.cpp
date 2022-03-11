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

#include "window_state_observer.h"

#include "cgroup_sched_log.h"
#include "cgroup_event_handler.h"
#include "res_type.h"
#include "sched_controller.h"

namespace OHOS {
namespace ResourceSchedule {
void WindowStateObserver::OnFocused(const sptr<FocusChangeInfo>& focusChangeInfo)
{
    auto cgHander = SchedController::GetInstance().GetCgroupEventHandler();
    if (cgHander && focusChangeInfo) {
        cgHander->PostTask([cgHander, focusChangeInfo] {
            cgHander->HandleFocusedWindow(focusChangeInfo->windowId_, focusChangeInfo->abilityToken_,
                focusChangeInfo->windowType_, focusChangeInfo->displayId_,
                focusChangeInfo->pid_, focusChangeInfo->uid_);
        });
    }
}

void WindowStateObserver::OnUnfocused(const sptr<FocusChangeInfo>& focusChangeInfo)
{
    auto cgHander = SchedController::GetInstance().GetCgroupEventHandler();
    if (cgHander && focusChangeInfo) {
        cgHander->PostTask([cgHander, focusChangeInfo] {
            cgHander->HandleUnfocusedWindow(focusChangeInfo->windowId_, focusChangeInfo->abilityToken_,
                focusChangeInfo->windowType_, focusChangeInfo->displayId_,
                focusChangeInfo->pid_, focusChangeInfo->uid_);
        });
    }
}

void WindowVisibilityObserver::OnWindowVisibilityChanged(
    const std::vector<sptr<WindowVisibilityInfo>>& windowVisibilityInfo)
{
    auto cgHander = SchedController::GetInstance().GetCgroupEventHandler();
    if (!cgHander) {
        return;
    }
    for (auto& info : windowVisibilityInfo) {
        if (!info) {
            continue;
        }
        cgHander->PostTask([cgHander, info] {
            cgHander->HandleWindowVisibilityChanged(info->windowId_, info->isVisible_, info->pid_, info->uid_);
        });
    }
}
} // namespace OHOS
} // namespace ResourceSchedule
