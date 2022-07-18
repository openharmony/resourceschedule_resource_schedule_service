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

#include "audio_ring_mode_observer.h"

#include "ressched_utils.h"
#include "res_sched_log.h"
#include "res_type.h"

namespace OHOS {
namespace ResourceSchedule {
void AudioRingModeObserver::OnRingerModeUpdated(const AudioStandard::AudioRingerMode &ringerMode)
{
    RESSCHED_LOGD("enter AudioRingModeObserver::OnRingerModeUpdated");
    if (ringerMode != mode_) {
        const Json::Value payload = "";
        ResSchedUtils::GetInstance().ReportDataInProcess(ResType::RES_TYPE_AUDIO_RING_MODE_CHANGE, ringerMode, payload);
    }
}
} // namespace ResourceSchedule
} // namespace OHOS
