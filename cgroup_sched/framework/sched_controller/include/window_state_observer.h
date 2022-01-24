/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef WINDOW_STATE_OBSERVER_H
#define WINDOW_STATE_OBSERVER_H

#include <sys/types.h>
#include "iremote_object.h"
#include "window_manager.h"
#include "wm_common.h"

namespace OHOS {
namespace ResourceSchedule {

using OHOS::Rosen::IFocusChangedListener;
using OHOS::Rosen::WindowType;

class WindowStateObserver : public IFocusChangedListener {
public:
    void OnFocused(uint32_t windowId, sptr<IRemoteObject> abilityToken,
        WindowType windowType, int32_t displayId) override;
    void OnUnfocused(uint32_t windowId, sptr<IRemoteObject> abilityToken,
        WindowType windowType, int32_t displayId) override;
};

} // namespace ResourceSchedule
} // namespace OHOS
#endif // WINDOW_STATE_OBSERVER_H
