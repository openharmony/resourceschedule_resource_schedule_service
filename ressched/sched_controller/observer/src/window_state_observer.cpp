/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#include "nlohmann/json.hpp"
#include "res_sched_log.h"
#include "res_sched_mgr.h"
#include "res_type.h"

namespace OHOS {
namespace ResourceSchedule {

const uint8_t WINDOW_MODE_SPLIT_BIT = 1;
const uint8_t WINDOW_MODE_BIT_EXIT = 1;
const uint32_t FREE_MULTI_WINDOW_STATE_CHANGE = 10024;

void WindowStateObserver::OnFocused(const sptr<FocusChangeInfo>& focusChangeInfo)
{
    if (!focusChangeInfo) {
        return;
    }

    nlohmann::json payload;
    payload["pid"] = std::to_string(focusChangeInfo->pid_);
    payload["uid"] = std::to_string(focusChangeInfo->uid_);
    payload["windowId"] = std::to_string(focusChangeInfo->windowId_);
    payload["windowType"] = std::to_string((int32_t)(focusChangeInfo->windowType_));
    payload["displayId"] = std::to_string(focusChangeInfo->displayId_);
    ResSchedMgr::GetInstance().ReportData(
        ResType::RES_TYPE_WINDOW_FOCUS, ResType::WindowFocusStatus::WINDOW_FOCUS, payload);
}

void WindowStateObserver::OnUnfocused(const sptr<FocusChangeInfo>& focusChangeInfo)
{
    if (!focusChangeInfo) {
        return;
    }

    nlohmann::json payload;
    payload["pid"] = std::to_string(focusChangeInfo->pid_);
    payload["uid"] = std::to_string(focusChangeInfo->uid_);
    payload["windowId"] = std::to_string(focusChangeInfo->windowId_);
    payload["windowType"] = std::to_string((int32_t)(focusChangeInfo->windowType_));
    payload["displayId"] = std::to_string(focusChangeInfo->displayId_);
    ResSchedMgr::GetInstance().ReportData(
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
    payload["callingPid"] = std::to_string(info->callingPid_);
}

void WindowVisibilityObserver::OnWindowVisibilityChanged(
    const std::vector<sptr<WindowVisibilityInfo>>& windowVisibilityInfo)
{
    for (auto& info : windowVisibilityInfo) {
        if (!info) {
            continue;
        }
        auto windowId = info->windowId_;
        auto visibilityState = info->visibilityState_;
        auto windowType = info->windowType_;
        auto pid = info->pid_;
        auto uid = info->uid_;

        nlohmann::json payload;
        MarshallingWindowVisibilityInfo(info, payload);
        bool isVisible = visibilityState < Rosen::WindowVisibilityState::WINDOW_VISIBILITY_STATE_TOTALLY_OCCUSION;
        ResSchedMgr::GetInstance().ReportData(ResType::RES_TYPE_WINDOW_VISIBILITY_CHANGE,
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
    for (const auto& info : changeInfo) {
        if (!info) {
            continue;
        }
        auto windowId = info->windowId_;
        auto windowType = info->windowType_;
        auto drawingContentState = info->drawingContentState_;
        auto pid = info->pid_;
        auto uid = info->uid_;

        nlohmann::json payload;
        MarshallingWindowDrawingContentInfo(info, payload);
        ResSchedMgr::GetInstance().ReportData(ResType::RES_TYPE_WINDOW_DRAWING_CONTENT_CHANGE,
            drawingContentState ? ResType::WindowDrawingStatus::Drawing : ResType::WindowDrawingStatus::NotDrawing,
            payload);
    }
}
void WindowModeObserver::OnWindowModeUpdate(const WindowModeType mode)
{
    RESSCHED_LOGD("WindowModeObserver OnWindowModeUpdate mode: %{public}hhu ", mode);
    uint8_t nowWindowMode = MarshallingWindowModeType(mode);
    uint8_t windowModeChangeBit = nowWindowMode ^ lastWindowMode_;
    nlohmann::json payload;
    uint8_t windowModeSplitValue = (nowWindowMode >> WINDOW_MODE_SPLIT_BIT) & WINDOW_MODE_BIT_EXIT;
    uint8_t windowModeFloatingValue = nowWindowMode & WINDOW_MODE_BIT_EXIT;
    switch (windowModeChangeBit) {
        case RSSWindowMode::WINDOW_MODE_FLOATING_CHANGED:
            ResSchedMgr::GetInstance().ReportData(ResType::RES_TYPE_FLOATING_WINDOW,
                windowModeFloatingValue, payload);
            break;
        case RSSWindowMode::WINDOW_MODE_SPLIT_CHANGED:
            ResSchedMgr::GetInstance().ReportData(ResType::RES_TYPE_SPLIT_SCREEN,
                windowModeSplitValue, payload);
            break;
        case RSSWindowMode::WINDOW_MODE_SPLIT_FLOATING_CHANGED:
            ResSchedMgr::GetInstance().ReportData(ResType::RES_TYPE_SPLIT_SCREEN,
                windowModeSplitValue, payload);
            ResSchedMgr::GetInstance().ReportData(ResType::RES_TYPE_FLOATING_WINDOW,
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

void PiPStateObserver::OnPiPStateChanged(const std::string& bundleName, const bool isForeground)
{
    RESSCHED_LOGI("Receive OnPiPStateChange %{public}s %{public}d", bundleName.c_str(), isForeground);
    nlohmann::json payload;
    ResSchedMgr::GetInstance().ReportData(ResType::RES_TRPE_PIP_STATUS,
        static_cast<int64_t>(isForeground), payload);
}

void WindowStyleObserver::OnWindowStyleUpdate(WindowStyleType styleType)
{
    bool isFreeStyleType = (styleType == WindowStyleType::WINDOW_STYLE_FREE_MULTI_WINDOW);
    RESSCHED_LOGI("Update window style : %{public}d", isFreeStyleType);
    nlohmann::json payload;
    payload["extType"] = std::to_string(FREE_MULTI_WINDOW_STATE_CHANGE);
    int64_t value = isFreeStyleType ? 1 : 0;
    ResSchedMgr::GetInstance().ReportData(ResType::RES_TYPE_KEY_PERF_SCENE, value, payload);
    //CgroupSchedEnhance::GetInstance().SetFreeMultiWindowStyle(isFreeStyleType);
}
} // namespace ResourceSchedule
} // namespace OHOS