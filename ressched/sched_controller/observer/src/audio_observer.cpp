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
#include "audio_observer.h"

#include "ressched_utils.h"
#include "res_sched_log.h"
#include "res_type.h"

namespace OHOS {
namespace ResourceSchedule {
void AudioObserver::OnRendererStateChange(
    const std::vector<std::unique_ptr<AudioStandard::AudioRendererChangeInfo>> &audioRendererChangeInfos)
{
    for (const auto &audioRendererChangeInfo : audioRendererChangeInfos) {
        RESSCHED_LOGD("enter AudioRenderStateObserver::OnRendererStateChange, state: %{public}d",
            audioRendererChangeInfo->rendererState);
        nlohmann::json payload;
        payload["uid"] = std::to_string(audioRendererChangeInfo->clientUID);
        payload["sessionId"] = std::to_string(audioRendererChangeInfo->sessionId);
        ResSchedUtils::GetInstance().ReportDataInProcess(ResType::RES_TYPE_AUDIO_RENDER_STATE_CHANGE,
            audioRendererChangeInfo->rendererState, payload);
    }
}

void AudioObserver::OnRingerModeUpdated(const AudioStandard::AudioRingerMode &ringerMode)
{
    if (ringerMode != mode_) {
        RESSCHED_LOGD("enter AudioRingModeObserver::OnRingerModeUpdated, ringerMode: %{public}d", ringerMode);
        mode_ = ringerMode;
        const nlohmann::json payload = nlohmann::json::object();
        ResSchedUtils::GetInstance().ReportDataInProcess(ResType::RES_TYPE_AUDIO_RING_MODE_CHANGE, ringerMode, payload);
    }
}

void AudioObserver::OnVolumeKeyEvent(AudioStandard::VolumeEvent volumeEvent)
{
    RESSCHED_LOGD("enter AudioVolumeKeyObserver::OnVolumeKeyEvent, streamType: %{public}d, volumeLevel: %{public}d",
        volumeEvent.volumeType, volumeEvent.volume);
    nlohmann::json payload;
    payload["volumeType"] = std::to_string(volumeEvent.volumeType);
    ResSchedUtils::GetInstance().ReportDataInProcess(ResType::RES_TYPE_AUDIO_VOLUME_KEY_CHANGE,
        volumeEvent.volume, payload);
}
} // namespace ResourceSchedule
} // namespace OHOS
