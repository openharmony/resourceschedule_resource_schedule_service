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

#ifndef RESSCHED_SCHED_CONTROLLER_OBSERVER_INCLUDE_AUDIO_OBSERVER_H
#define RESSCHED_SCHED_CONTROLLER_OBSERVER_INCLUDE_AUDIO_OBSERVER_H
#ifdef RESSCHED_AUDIO_FRAMEWORK_ENABLE
#include <string>
#include <unordered_map>

#include "nlohmann/json.hpp"
#include "audio_stream_manager.h"
#include "audio_system_manager.h"
#include "audio_routing_manager.h"

namespace OHOS {
namespace ResourceSchedule {
class AudioObserver :
    public AudioStandard::AudioRendererStateChangeCallback,
    public AudioStandard::AudioRingerModeCallback,
    public AudioStandard::VolumeKeyEventCallback,
    public AudioStandard::AudioManagerAudioSceneChangedCallback,
    public AudioStandard::AudioPreferredOutputDeviceChangeCallback {
public:
    void OnRendererStateChange(
        const std::vector<std::shared_ptr<AudioStandard::AudioRendererChangeInfo>> &audioRendererChangeInfos) override;
    void OnRingerModeUpdated(const AudioStandard::AudioRingerMode &ringerMode) override;
    void OnVolumeKeyEvent(AudioStandard::VolumeEvent volumeEvent) override;
    void OnAudioSceneChange(const AudioStandard::AudioScene audioScene) override;
    void OnPreferredOutputDeviceUpdated(
        const std::vector<std::shared_ptr<AudioStandard::AudioDeviceDescriptor>> &descs)  override;
private:
    int32_t mode_ = -1;
    void MarshallingAudioRendererChangeInfo(
        const std::shared_ptr<AudioStandard::AudioRendererChangeInfo> &audioRendererChangeInfo,
        nlohmann::json &payload);
    bool IsRenderStateChange(const std::shared_ptr<AudioStandard::AudioRendererChangeInfo>& info);
    std::unordered_map<int32_t, AudioStandard::RendererState> renderState_;
    std::unordered_map<int32_t, int32_t> volumeState_;
};
} // namespace ResourceSchedule
} // namespace OHOS
#endif
#endif // RESSCHED_SCHED_CONTROLLER_OBSERVER_INCLUDE_AUDIO_OBSERVER_H
