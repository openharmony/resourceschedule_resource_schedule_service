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

#ifndef CGROUP_SCHED_FRAMEWORK_SCHED_CONTROLLER_INCLUDE_WINDOW_STATE_OBSERVER_H_
#define CGROUP_SCHED_FRAMEWORK_SCHED_CONTROLLER_INCLUDE_WINDOW_STATE_OBSERVER_H_

#include <sys/types.h>
#include "iremote_object.h"
#include "window_manager.h"
#include "wm_common.h"

namespace OHOS {
namespace ResourceSchedule {
using OHOS::Rosen::IFocusChangedListener;
using OHOS::Rosen::IVisibilityChangedListener;
using OHOS::Rosen::WindowType;
using OHOS::Rosen::FocusChangeInfo;
using OHOS::Rosen::WindowVisibilityInfo;

class WindowStateObserver : public IFocusChangedListener {
public:
    void OnFocused(const sptr<FocusChangeInfo>& focusChangeInfo) override;
    void OnUnfocused(const sptr<FocusChangeInfo>& focusChangeInfo) override;
};

class WindowVisibilityObserver : public IVisibilityChangedListener {
public:
    void OnWindowVisibilityChanged(const std::vector<sptr<WindowVisibilityInfo>>& windowVisibilityInfo) override;
};
} // namespace ResourceSchedule
} // namespace OHOS
#endif // CGROUP_SCHED_FRAMEWORK_SCHED_CONTROLLER_INCLUDE_WINDOW_STATE_OBSERVER_H_
