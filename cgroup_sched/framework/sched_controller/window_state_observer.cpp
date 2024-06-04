/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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
#include "ressched_utils.h"

namespace OHOS {
namespace ResourceSchedule {
    const uint8_t WINDOW_MODE_SPLIT_BIT = 1;
    const uint8_t WINDOW_MODE_BIT_EXIT = 1;
void WindowStateObserver::OnFocused(const sptr<FocusChangeInfo>& focusChangeInfo)
{
    if (!focusChangeInfo) {
        return;
    }

    auto cgHandler = SchedController::GetInstance().GetCgroupEventHandler();
    if (cgHandler) {
        auto windowId = focusChangeInfo->windowId_;
        auto token = reinterpret_cast<uintptr_t>(focusChangeInfo->abilityToken_.GetRefPtr());
        auto windowType = focusChangeInfo->windowType_;
        auto displayId = focusChangeInfo->displayId_;
        auto pid = focusChangeInfo->pid_;
        auto uid = focusChangeInfo->uid_;

        cgHandler->PostTask([cgHandler, windowId, token, windowType, displayId, pid, uid] {
            cgHandler->HandleFocusedWindow(windowId, token, windowType, displayId, pid, uid);
        });
    }

    nlohmann::json payload;
    payload["pid"] = std::to_string(focusChangeInfo->pid_);
    payload["uid"] = std::to_string(focusChangeInfo->uid_);
    payload["windowId"] = std::to_string(focusChangeInfo->windowId_);
    payload["windowType"] = std::to_string((int32_t)(focusChangeInfo->windowType_));
    payload["displayId"] = std::to_string(focusChangeInfo->displayId_);
    ResSchedUtils::GetInstance().ReportDataInProcess(
        ResType::RES_TYPE_WINDOW_FOCUS, ResType::WindowFocusStatus::WINDOW_FOCUS, payload);
}

void WindowStateObserver::OnUnfocused(const sptr<FocusChangeInfo>& focusChangeInfo)
{
    if (!focusChangeInfo) {
        return;
    }

    auto cgHandler = SchedController::GetInstance().GetCgroupEventHandler();
    if (cgHandler) {
        auto windowId = focusChangeInfo->windowId_;
        auto token = reinterpret_cast<uintptr_t>(focusChangeInfo->abilityToken_.GetRefPtr());
        auto windowType = focusChangeInfo->windowType_;
        auto displayId = focusChangeInfo->displayId_;
        auto pid = focusChangeInfo->pid_;
        auto uid = focusChangeInfo->uid_;

        cgHandler->PostTask([cgHandler, windowId, token, windowType, displayId, pid, uid] {
            cgHandler->HandleUnfocusedWindow(windowId, token, windowType, displayId, pid, uid);
        });
    }

    nlohmann::json payload;
    payload["pid"] = std::to_string(focusChangeInfo->pid_);
    payload["uid"] = std::to_string(focusChangeInfo->uid_);
    payload["windowId"] = std::to_string(focusChangeInfo->windowId_);
    payload["windowType"] = std::to_string((int32_t)(focusChangeInfo->windowType_));
    payload["displayId"] = std::to_string(focusChangeInfo->displayId_);
    ResSchedUtils::GetInstance().ReportDataInProcess(
        ResType::RES_TYPE_WINDOW_FOCUS, ResType::WindowFocusStatus::WINDOW_UNFOCUS, payload);
}

void WindowVisibilityObserver::MarshallingWindowVisibilityInfo(const sptr<WindowVisibilityInfo>& info,
    nlohmann::json& payload)
{
    bool isVisible = info->visibilityState_ < Rosen::WindowVisibilityState::WINDOW_VISIBILITY_STATE_TOTALLY_OCCUSION;
    payload["pid"] = std::to_string(info->pid_);
    payload["uid"] = std::to_string(info->uid_);
    payload["windowId"] = std::to_string(info->windowId_);
    payload["windowType"] = std::to_string((int32_t)info->windowType_);
    payload["isVisible"] = isVisible;
    payload["visibilityState"] = std::to_string(info->visibilityState_);
}

void WindowVisibilityObserver::OnWindowVisibilityChanged(
    const std::vector<sptr<WindowVisibilityInfo>>& windowVisibilityInfo)
{
    auto cgHandler = SchedController::GetInstance().GetCgroupEventHandler();
    if (!cgHandler) {
        return;
    }
    for (auto& info : windowVisibilityInfo) {
        if (!info) {
            continue;
        }
        auto windowId = info->windowId_;
        auto visibilityState = info->visibilityState_;
        auto windowType = info->windowType_;
        auto pid = info->pid_;
        auto uid = info->uid_;
        cgHandler->PostTask([cgHandler, windowId, visibilityState, windowType, pid, uid] {
            cgHandler->HandleWindowVisibilityChanged(windowId, visibilityState, windowType, pid, uid);
        });
        nlohmann::json payload;
        MarshallingWindowVisibilityInfo(info, payload);
        bool isVisible = visibilityState < Rosen::WindowVisibilityState::WINDOW_VISIBILITY_STATE_TOTALLY_OCCUSION;
        ResSchedUtils::GetInstance().ReportDataInProcess(ResType::RES_TYPE_WINDOW_VISIBILITY_CHANGE,
            isVisible ? ResType::WindowVisibilityStatus::VISIBLE : ResType::WindowVisibilityStatus::INVISIBLE, payload);
    }
}

void WindowDrawingContentObserver::MarshallingWindowDrawingContentInfo(const sptr<WindowDrawingContentInfo>& info,
    nlohmann::json& payload)
{
    payload["pid"] = std::to_string(info->pid_);
    payload["uid"] = std::to_string(info->uid_);
    payload["windowId"] = std::to_string(info->windowId_);
    payload["windowType"] = std::to_string((int32_t)info->windowType_);
    payload["drawingContentState"] = info->drawingContentState_;
}

void WindowDrawingContentObserver::OnWindowDrawingContentChanged(
    const std::vector<sptr<WindowDrawingContentInfo>>& changeInfo)
{
    auto cgHandler = SchedController::GetInstance().GetCgroupEventHandler();
    if (!cgHandler) {
        return;
    }
    for (const auto& info : changeInfo) {
        if (!info) {
            continue;
        }
        auto windowId = info->windowId_;
        auto windowType = info->windowType_;
        auto drawingContentState = info->drawingContentState_;
        auto pid = info->pid_;
        auto uid = info->uid_;
        cgHandler->PostTask([cgHandler, windowId, windowType, drawingContentState, pid, uid] {
            cgHandler->HandleDrawingContentChangeWindow(windowId, windowType, drawingContentState, pid, uid);
        });
        nlohmann::json payload;
        MarshallingWindowDrawingContentInfo(info, payload);
        ResSchedUtils::GetInstance().ReportDataInProcess(ResType::RES_TYPE_WINDOW_DRAWING_CONTENT_CHANGE,
            drawingContentState ? ResType::WindowDrawingStatus::Drawing : ResType::WindowDrawingStatus::NotDrawing,
            payload);
    }
}
void WindowModeObserver::OnWindowModeUpdate(const WindowModeType mode)
{
    CGS_LOGI("WindowModeObserver OnWindowModeUpdate mode: %{public}hhu ", mode);
    uint8_t nowWindowMode = MarshallingWindowModeType(mode);
    uint8_t windowModeChangeBit = nowWindowMode ^ lastWindowMode_;
    nlohmann::json payload;
    uint8_t windowModeSplitValue = (nowWindowMode >> WINDOW_MODE_SPLIT_BIT) & WINDOW_MODE_BIT_EXIT;
    uint8_t windowModeFloatingValue = nowWindowMode & WINDOW_MODE_BIT_EXIT;
    switch (windowModeChangeBit) {
        case RSSWindowMode::WINDOW_MODE_FLOATING_CHANGED:
            ResSchedUtils::GetInstance().ReportDataInProcess(ResType::RES_TYPE_FLOATING_WINDOW,
                windowModeFloatingValue, payload);
            break;
        case RSSWindowMode::WINDOW_MODE_SPLIT_CHANGED:
            ResSchedUtils::GetInstance().ReportDataInProcess(ResType::RES_TYPE_SPLIT_SCREEN,
                windowModeSplitValue, payload);
            break;
        case RSSWindowMode::WINDOW_MODE_SPLIT_FLOATING_CHANGED:
            ResSchedUtils::GetInstance().ReportDataInProcess(ResType::RES_TYPE_SPLIT_SCREEN,
                windowModeSplitValue, payload);
            ResSchedUtils::GetInstance().ReportDataInProcess(ResType::RES_TYPE_FLOATING_WINDOW,
                windowModeFloatingValue, payload);
            break;
        default:
            break;
    }
    lastWindowMode_ = nowWindowMode;
}

uint8_t WindowModeObserver::MarshallingWindowModeType(const WindowModeType mode)
{
    uint8_t nowWindowMode = RSSWindowMode::WINDOW_MODE_OTHER;
    switch (mode) {
        case Rosen::WindowModeType::WINDOW_MODE_SPLIT:
            nowWindowMode = RSSWindowMode::WINDOW_MODE_SPLIT;
            break;
        case Rosen::WindowModeType::WINDOW_MODE_FLOATING:
        case Rosen::WindowModeType::WINDOW_MODE_FULLSCREEN_FLOATING:
            nowWindowMode = RSSWindowMode::WINDOW_MODE_FLOATING;
            break;
        case Rosen::WindowModeType::WINDOW_MODE_SPLIT_FLOATING:
            nowWindowMode = RSSWindowMode::WINDOW_MODE_SPLIT_FLOATING;
            break;
        default:
            nowWindowMode = RSSWindowMode::WINDOW_MODE_OTHER;
            break;
    }
    return nowWindowMode;
}
} // namespace ResourceSchedule
} // namespace OHOS
