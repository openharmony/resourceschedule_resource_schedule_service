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
        auto windowId = focusChangeInfo->windowId_;
        auto token = reinterpret_cast<uint64_t>(focusChangeInfo->abilityToken_.GetRefPtr());
        auto windowType = focusChangeInfo->windowType_;
        auto displayId = focusChangeInfo->displayId_;
        auto pid = focusChangeInfo->pid_;
        auto uid = focusChangeInfo->uid_;

        cgHander->PostTask([cgHander, windowId, token, windowType, displayId, pid, uid] {
            cgHander->HandleFocusedWindow(windowId, token, windowType, displayId, pid, uid);
        });
    }
}

void WindowStateObserver::OnUnfocused(const sptr<FocusChangeInfo>& focusChangeInfo)
{
    auto cgHander = SchedController::GetInstance().GetCgroupEventHandler();
    if (cgHander && focusChangeInfo) {
        auto windowId = focusChangeInfo->windowId_;
        auto token = reinterpret_cast<uint64_t>(focusChangeInfo->abilityToken_.GetRefPtr());
        auto windowType = focusChangeInfo->windowType_;
        auto displayId = focusChangeInfo->displayId_;
        auto pid = focusChangeInfo->pid_;
        auto uid = focusChangeInfo->uid_;

        cgHander->PostTask([cgHander, windowId, token, windowType, displayId, pid, uid] {
            cgHander->HandleUnfocusedWindow(windowId, token, windowType, displayId, pid, uid);
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
        auto windowId = info->windowId_;
        auto isVisible = info->isVisible_;
        auto pid = info->pid_;
        auto uid = info->uid_;
        cgHander->PostTask([cgHander, windowId, isVisible, pid, uid] {
            cgHander->HandleWindowVisibilityChanged(windowId, isVisible, pid, uid);
        });
    }
}
} // namespace ResourceSchedule
} // namespace OHOS
