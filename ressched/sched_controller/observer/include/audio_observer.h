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

#ifndef RESSCHED_SCHED_CONTROLLER_OBSERVER_INCLUDE_AUDIO_OBSERVER_H
#define RESSCHED_SCHED_CONTROLLER_OBSERVER_INCLUDE_AUDIO_OBSERVER_H

#include <string>

#include "audio_stream_manager.h"
#include "audio_system_manager.h"

namespace OHOS {
namespace ResourceSchedule {
class AudioObserver : public AudioStandard::AudioRendererStateChangeCallback,
    public AudioStandard::AudioRingerModeCallback, public AudioStandard::VolumeKeyEventCallback {
public:
    void OnRendererStateChange(
        const std::vector<std::unique_ptr<AudioStandard::AudioRendererChangeInfo>> &audioRendererChangeInfos) override;
    void OnRingerModeUpdated(const AudioStandard::AudioRingerMode &ringerMode) override;
    void OnVolumeKeyEvent(AudioStandard::VolumeEvent volumeEvent) override;
private:
    int32_t mode_ = -1;
};
} // namespace ResourceSchedule
} // namespace OHOS
#endif // RESSCHED_SCHED_CONTROLLER_OBSERVER_INCLUDE_AUDIO_OBSERVER_H
