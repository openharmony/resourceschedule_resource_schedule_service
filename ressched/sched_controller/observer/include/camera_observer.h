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

#ifndef RESSCHED_SCHED_CONTROLLER_OBSERVER_INCLUDE_CAMERA_OBSERVER_H
#define RESSCHED_SCHED_CONTROLLER_OBSERVER_INCLUDE_CAMERA_OBSERVER_H

#include <string>

#include "hisysevent_subscribe_callback.h"

namespace OHOS {
namespace ResourceSchedule {
class CameraObserver : public HiviewDFX::HiSysEventSubscribeCallBack {
public:
    CameraObserver() : HiviewDFX::HiSysEventSubscribeCallBack() {}
    virtual ~CameraObserver() {}
    void OnHandle(const std::string& domain, const std::string& eventName, const int eventType,
        const std::string& eventDetail) override;
    void OnServiceDied() override;
};
}  // namespace ResourceSchedule
}  // namespace OHOS
#endif  // RESSCHED_SCHED_CONTROLLER_OBSERVER_INCLUDE_CAMERA_OBSERVER_H