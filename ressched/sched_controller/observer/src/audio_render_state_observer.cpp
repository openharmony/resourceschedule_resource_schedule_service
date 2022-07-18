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

#include "audio_render_state_observer.h"

#include "ressched_utils.h"
#include "res_sched_log.h"
#include "res_type.h"

namespace OHOS {
namespace ResourceSchedule {
void AudioRenderStateObserver::OnRendererStateChange(
    const std::vector<std::unique_ptr<AudioStandard::AudioRendererChangeInfo>> &audioRendererChangeInfos)
{
    RESSCHED_LOGD("enter AudioRenderStateObserver::OnRendererStateChange, state: %{public}d",
        audioRendererChangeInfo->rendererState);
    for (const auto &audioRendererChangeInfo : audioRendererChangeInfos) {
        Json::Value payload;
        payload["uid"] = std::to_string(audioRendererChangeInfo->clientUID);
        ResSchedUtils::GetInstance().ReportDataInProcess(ResType::RES_TYPE_AUDIO_RENDER_STATE_CHANGE,
            audioRendererChangeInfo->rendererState, payload);
    }
}
} // namespace ResourceSchedule
} // namespace OHOS
