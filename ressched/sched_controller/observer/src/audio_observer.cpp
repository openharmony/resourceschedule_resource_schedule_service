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
#ifdef RESSCHED_AUDIO_FRAMEWORK_ENABLE
#include "audio_observer.h"

#include "res_sched_log.h"
#include "res_sched_mgr.h"
#include "res_type.h"

namespace OHOS {
namespace ResourceSchedule {
void AudioObserver::MarshallingAudioRendererChangeInfo(
    const std::shared_ptr<AudioStandard::AudioRendererChangeInfo> &audioRendererChangeInfo, nlohmann::json &payload)
{
    if (audioRendererChangeInfo == nullptr) {
        return;
    }
    payload["uid"] = std::to_string(audioRendererChangeInfo->clientUID);
    payload["sessionId"] = std::to_string(audioRendererChangeInfo->sessionId);
    payload["rendererState"] = static_cast<int32_t>(audioRendererChangeInfo->rendererState);
    payload["pid"] = std::to_string(audioRendererChangeInfo->clientPid);
    RESSCHED_LOGD("MarshallingAudioRendererChangeInfo, uid: %{public}d, pid: %{public}d",
        audioRendererChangeInfo->clientUID, audioRendererChangeInfo->clientPid);
    /* struct AudioRendererInfo */
    payload["rendererInfo.contentType"] = static_cast<int32_t>(audioRendererChangeInfo->rendererInfo.contentType);
    payload["rendererInfo.streamUsage"] = static_cast<int32_t>(audioRendererChangeInfo->rendererInfo.streamUsage);
}

bool AudioObserver::IsRenderStateChange(const std::shared_ptr<AudioStandard::AudioRendererChangeInfo>& info)
{
    auto item = renderState_.find(info->sessionId);
    if (item != renderState_.end()) {
        return item->second != info->rendererState;
    }
    return true;
}

void AudioObserver::OnRendererStateChange(
    const std::vector<std::shared_ptr<AudioStandard::AudioRendererChangeInfo>> &audioRendererChangeInfos)
{
    for (const auto &audioRendererChangeInfo : audioRendererChangeInfos) {
        RESSCHED_LOGD("enter AudioRenderStateObserver::OnRendererStateChange, state: %{public}d",
            audioRendererChangeInfo->rendererState);
        nlohmann::json payload;
        MarshallingAudioRendererChangeInfo(audioRendererChangeInfo, payload);
        if (IsRenderStateChange(audioRendererChangeInfo)) {
            ResSchedMgr::GetInstance().ReportData(ResType::RES_TYPE_AUDIO_RENDER_STATE_CHANGE,
                audioRendererChangeInfo->rendererState, payload);
        }
    }
    renderState_.clear();
    for (const auto &audioRendererChangeInfo : audioRendererChangeInfos) {
        renderState_[audioRendererChangeInfo->sessionId] = audioRendererChangeInfo->rendererState;
    }
}

void AudioObserver::OnRingerModeUpdated(const AudioStandard::AudioRingerMode &ringerMode)
{
    if (ringerMode != mode_) {
        RESSCHED_LOGI("enter AudioRingModeObserver::OnRingerModeUpdated, ringerMode: %{public}d", ringerMode);
        mode_ = ringerMode;
        const nlohmann::json payload = nlohmann::json::object();
        ResSchedMgr::GetInstance().ReportData(ResType::RES_TYPE_AUDIO_RING_MODE_CHANGE, ringerMode, payload);
    }
}

void AudioObserver::OnVolumeKeyEvent(AudioStandard::VolumeEvent volumeEvent)
{
    RESSCHED_LOGI("enter AudioVolumeKeyObserver::OnVolumeKeyEvent, streamType: %{public}d, volumeLevel: %{public}d",
        volumeEvent.volumeType, volumeEvent.volume);
    auto item = volumeState_.find(volumeEvent.volumeType);
    if (item != volumeState_.end() && item->second == volumeEvent.volume) {
        RESSCHED_LOGI("volume not change");
        return;
    }
    nlohmann::json payload;
    payload["volumeType"] = std::to_string(volumeEvent.volumeType);
    payload["volumeLevel"] = std::to_string(volumeEvent.volume);
    volumeState_[volumeEvent.volumeType] = volumeEvent.volume;
    ResSchedMgr::GetInstance().ReportData(ResType::RES_TYPE_AUDIO_VOLUME_KEY_CHANGE,
        volumeEvent.volume, payload);
}

void AudioObserver::OnAudioSceneChange(const AudioStandard::AudioScene audioScene)
{
    RESSCHED_LOGD("enter audioSceneKeyObserver::OnAudioSceneChange, audioScene: %{public}d", audioScene);
    nlohmann::json payload;
    payload["audioScene"] = std::to_string(audioScene);
    ResSchedMgr::GetInstance().ReportData(ResType::RES_TYPE_AUDIO_SCENE_CHANGE, audioScene, payload);
}

void AudioObserver::OnPreferredOutputDeviceUpdated(
    const std::vector<std::shared_ptr<AudioStandard::AudioDeviceDescriptor>> &descs)
{
    for (const auto &desc : descs) {
        RESSCHED_LOGI("device change, type: %{public}d", desc->deviceType_);
        nlohmann::json payload;
        payload["deviceType"] = std::to_string(desc->deviceType_);
        payload["deviceId"] = std::to_string(desc->deviceId_);
        ResSchedMgr::GetInstance().ReportData(ResType::RES_TYPE_OUTPUT_DEVICE_CHANGE,
            desc->deviceId_, payload);
    }
}
} // namespace ResourceSchedule
} // namespace OHOS
#endif
