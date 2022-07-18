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

#include "audio_volume_key_observer.h"

#include "ressched_utils.h"
#include "res_sched_log.h"
#include "res_type.h"

namespace OHOS {
namespace ResourceSchedule {
void AudioVolumeKeyObserver::OnVolumeKeyEvent(AudioStandard::VolumeEvent volumeEvent)
{
    RESSCHED_LOGD("enter AudioVolumeKeyObserver::OnVolumeKeyEvent, streamType: %{public}d, volumeLevel: %{public}d",
        volumeEvent.volumeType, volumeEvent.volume);
    Json::Value payload;
    payload["volumeType"] = std::to_string(volumeEvent.volumeType);
    ResSchedUtils::GetInstance().ReportDataInProcess(ResType::RES_TYPE_AUDIO_VOLUME_KEY_CHANGE,
        volumeEvent.volume, payload);
}
} // namespace ResourceSchedule
} // namespace OHOS
