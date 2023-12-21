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

#ifndef RESSCHED_SCHED_CONTROLLER_OBSERVER_INCLUDE_HISYSEVENT_OBSERVER_H
#define RESSCHED_SCHED_CONTROLLER_OBSERVER_INCLUDE_HISYSEVENT_OBSERVER_H

#include <memory>
#include <string>

#include "hisysevent_listener.h"
#include "nlohmann/json.hpp"

namespace OHOS {
namespace ResourceSchedule {
class HiSysEventObserver : public HiviewDFX::HiSysEventListener {
public:
    void OnEvent(std::shared_ptr<HiviewDFX::HiSysEventRecord> sysEvent) override;
    void OnServiceDied() override;
    void ProcessHiSysEvent(const std::string& eventName, const nlohmann::json& root);

    HiSysEventObserver();
    ~HiSysEventObserver();

private:
    void ProcessAvCodecEvent(const nlohmann::json& root, const std::string& eventName);
    void ProcessRunningLockEvent(const nlohmann::json& root, const std::string& eventName);
    void ProcessAudioEvent(const nlohmann::json& root, const std::string& eventName);
    void ProcessCameraEvent(const nlohmann::json& root, const std::string& eventName);
    void ProcessBluetoothEvent(const nlohmann::json& root, const std::string& eventName);
    void ProcessWifiEvent(const nlohmann::json& root, const std::string& eventName);
    void ProcessScreenCaptureEvent(const nlohmann::json& root, const std::string& eventName);
    bool CheckJsonValue(const nlohmann::json& value, std::initializer_list<std::string> params);

    std::map<std::string,
        std::function<void(HiSysEventObserver *, const nlohmann::json&, const std::string&)>> handleObserverMap_;
};

/**
* Audio State
*/
enum class AudioState : int32_t {
    AUDIO_STATE_INVALID = -1,
    AUDIO_STATE_NEW,
    AUDIO_STATE_PREPARED,
    AUDIO_STATE_RUNNING,
    AUDIO_STATE_STOPPED,
    AUDIO_STATE_RELEASED,
    AUDIO_STATE_PAUSED
};

/**
* Runninglock State
*/
enum class RunningLockState : int32_t {
    RUNNINGLOCK_STATE_DISABLE = 0,
    RUNNINGLOCK_STATE_ENABLE,
    RUNNINGLOCK_STATE_PROXIED,
    RUNNINGLOCK_STATE_UNPROXIED_RESTORE,
};

/**
* Wifi State
*/
enum class WifiState : int32_t {
    CONNECTED = 0,
    DISCONNECTED
};
} // namespace ResourceSchedule
} // namespace OHOS
#endif // RESSCHED_SCHED_CONTROLLER_OBSERVER_INCLUDE_HISYSEVENT_OBSERVER_H
