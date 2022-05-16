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

#include "camera_observer.h"

#include "res_sched_log.h"

namespace OHOS {
namespace ResourceSchedule {
void CameraObserver::OnHandle(const std::string& domain, const std::string& eventName, const int eventType,
    const std::string& eventDetail)
{
    RESSCHED_LOGI("EventDetail: %{public}s", eventDetail.c_str());
}

void CameraObserver::OnServiceDied()
{
    RESSCHED_LOGE("CameraObserver service disconnected");
}
}  // namespace ResourceSchedule
}  // namespace OHOS