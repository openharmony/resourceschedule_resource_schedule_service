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

#ifndef CGROUP_SCHED_FRAMEWORK_SCHED_CONTROLLER_INCLUDE_WINDOW_STATE_OBSERVER_H_
#define CGROUP_SCHED_FRAMEWORK_SCHED_CONTROLLER_INCLUDE_WINDOW_STATE_OBSERVER_H_

#include <sys/types.h>
#include "iremote_object.h"
#include "window_manager_lite.h"
#include "wm_common.h"
#include "nlohmann/json.hpp"
#include <cstdint>

namespace OHOS {
namespace ResourceSchedule {
namespace RSSWindowMode {
    enum:uint8_t {
        // The frist bit represents the floating bit,0 to enter, 1 to exit
        // The second bit represents the split bit,0 to enter, 1 to exit
        WINDOW_MODE_SPLIT_FLOATING = 0b00,
        WINDOW_MODE_SPLIT = 0b01,
        WINDOW_MODE_FLOATING = 0b10,
        WINDOW_MODE_OTHER = 0b11,

        // The frist bit represents the floating bit,0 to changed, 1 to unchanged
        // The second bit represents the split bit,0 to changed, 1 to unchanged
        WINDOW_MODE_SPLIT_FLOATING_UNCHANGED = 0b00,
        WINDOW_MODE_FLOATING_CHANGED = 0b01,
        WINDOW_MODE_SPLIT_CHANGED = 0b10,
        WINDOW_MODE_SPLIT_FLOATING_CHANGED = 0b11,
    };
}
using OHOS::Rosen::IFocusChangedListener;
using OHOS::Rosen::IVisibilityChangedListener;
using OHOS::Rosen::IDrawingContentChangedListener;
using OHOS::Rosen::IWindowModeChangedListener;
using OHOS::Rosen::WindowType;
using OHOS::Rosen::FocusChangeInfo;
using OHOS::Rosen::WindowVisibilityInfo;
using OHOS::Rosen::WindowDrawingContentInfo;
using OHOS::Rosen::WindowModeType;

class WindowStateObserver : public IFocusChangedListener {
public:
    void OnFocused(const sptr<FocusChangeInfo>& focusChangeInfo) override;
    void OnUnfocused(const sptr<FocusChangeInfo>& focusChangeInfo) override;
};

class WindowVisibilityObserver : public IVisibilityChangedListener {
public:
    void OnWindowVisibilityChanged(const std::vector<sptr<WindowVisibilityInfo>>& windowVisibilityInfo) override;

private:
    void MarshallingWindowVisibilityInfo(const sptr<WindowVisibilityInfo>& info, nlohmann::json& payload);
};

class WindowDrawingContentObserver : public IDrawingContentChangedListener {
public:
    void OnWindowDrawingContentChanged(const std::vector<sptr<WindowDrawingContentInfo>>& changeInfo) override;

private:
    void MarshallingWindowDrawingContentInfo(const sptr<WindowDrawingContentInfo>& info, nlohmann::json& payload);
};
class WindowModeObserver : public IWindowModeChangedListener {
public:
    void OnWindowModeUpdate(const WindowModeType mode) override;
private:
    uint8_t MarshallingWindowModeType(const WindowModeType mode);
    uint8_t lastWindowMode_ = RSSWindowMode::WINDOW_MODE_OTHER;
};
} // namespace ResourceSchedule
} // namespace OHOS
#endif // CGROUP_SCHED_FRAMEWORK_SCHED_CONTROLLER_INCLUDE_WINDOW_STATE_OBSERVER_H_
