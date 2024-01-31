/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "hisysevent_observer.h"

#ifdef RESSCHED_COMMUNICATION_BLUETOOTH_ENABLE
#include "bluetooth_def.h"
#endif
#include "res_sched_log.h"
#include "res_sched_mgr.h"
#include "res_type.h"

namespace OHOS {
namespace ResourceSchedule {
namespace {
    static const std::string WIFI_CONNECTION = "WIFI_CONNECTION";
    static const std::string WIFI_SCAN = "WIFI_SCAN";
    static const std::string CAMERA_CONNECT = "CAMERA_CONNECT";
    constexpr int32_t INDENT                    = -1;
    constexpr int32_t WIFISCAN                  = 2;
    constexpr int32_t WIFICONNECTED             = 3;
    constexpr int32_t WIFIDISCONNECTED          = 5;
    constexpr int32_t CAMERACONNECT             = 0;
    constexpr int32_t CAMERADISCONNECT          = 1;
    constexpr int32_t RUNNINGLOCK_DISABLE       = 0;
    constexpr int32_t RUNNINGLOCK_ENABLE        = 1;
    constexpr int32_t RUNNINGLOCK_PROXIED       = 2;
    constexpr int32_t MAX_LENGTH                = 1024;
}

HiSysEventObserver::HiSysEventObserver() : HiviewDFX::HiSysEventListener()
{
    handleObserverMap_ = {
        {"RUNNINGLOCK", std::bind(&HiSysEventObserver::ProcessRunningLockEvent,
            std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)},
        {"STREAM_CHANGE", std::bind(&HiSysEventObserver::ProcessAudioEvent,
            std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)},
        {"CAMERA_CONNECT", std::bind(&HiSysEventObserver::ProcessCameraEvent,
            std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)},
        {"CAMERA_DISCONNECT", std::bind(&HiSysEventObserver::ProcessCameraEvent,
            std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)},
        {"BR_SWITCH_STATE", std::bind(&HiSysEventObserver::ProcessBluetoothEvent,
            std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)},
        {"BLE_SWITCH_STATE", std::bind(&HiSysEventObserver::ProcessBluetoothEvent,
            std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)},
        {"WIFI_CONNECTION", std::bind(&HiSysEventObserver::ProcessWifiEvent,
            std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)},
        {"WIFI_SCAN", std::bind(&HiSysEventObserver::ProcessWifiEvent,
            std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)},
        {"PLAYER_STATE", std::bind(&HiSysEventObserver::ProcessScreenCaptureEvent,
            std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)},
    };
}

HiSysEventObserver::~HiSysEventObserver()
{
    handleObserverMap_.clear();
}

bool HiSysEventObserver::CheckJsonValue(const nlohmann::json& value, std::initializer_list<std::string> params)
{
    for (const auto& param : params) {
        if (value.find(param) == value.end()) {
            return false;
        }
    }
    return true;
}

void HiSysEventObserver::OnEvent(std::shared_ptr<HiviewDFX::HiSysEventRecord> sysEvent)
{
    if (sysEvent == nullptr) {
        RESSCHED_LOGE("OnEvent hisysevent info is null");
        return;
    }
    std::string eventDetail = sysEvent->AsJson();
    if (eventDetail.length() > MAX_LENGTH) {
        RESSCHED_LOGE("eventDetail length is invalid");
        return;
    }
    RESSCHED_LOGD("Process hisysevent event, detail:%{public}s", eventDetail.c_str());
    nlohmann::json root = nlohmann::json::parse(eventDetail, nullptr, false);
    if (root.is_discarded()) {
        RESSCHED_LOGE("Parse hisysevent data failed");
        return;
    }
    if (!CheckJsonValue(root, { "domain_", "name_" })
        || !root.at("domain_").is_string() || !root.at("name_").is_string()) {
        RESSCHED_LOGE("hisysevent data domain info lost");
        return;
    }
    std::string domainName = root.at("domain_").get<std::string>();
    std::string eventName = root.at("name_").get<std::string>();
    RESSCHED_LOGD("hisysevent info, domain: %{public}s, name:%{public}s", domainName.c_str(), eventName.c_str());
    ProcessHiSysEvent(eventName, root);
}

void HiSysEventObserver::ProcessHiSysEvent(const std::string& eventName, const nlohmann::json& root)
{
    if (root.at("domain_").get<std::string>() == "AV_CODEC") {
        ProcessAvCodecEvent(root, eventName);
        return;
    }

    auto funcIter = handleObserverMap_.find(eventName.c_str());
    if (funcIter != handleObserverMap_.end()) {
        auto function = funcIter->second;
        if (function) {
            function(this, root, eventName);
        }
    }
}

void HiSysEventObserver::ProcessAvCodecEvent(const nlohmann::json& root, const std::string& eventName)
{
    std::string str = root.dump(INDENT, ' ', false, nlohmann::json::error_handler_t::replace);
    RESSCHED_LOGD("Process av_codec event, event root:%{public}s", str.c_str());
    nlohmann::json payload;
    if (root.contains("CLIENT_UID") && root.at("CLIENT_UID").is_number_integer()) {
        payload["uid"] = std::to_string(root.at("CLIENT_UID").get<std::int32_t>());
    } else {
        RESSCHED_LOGE("av_codec event uid format error!");
        return;
    }
    if (root.contains("CLIENT_PID") && root.at("CLIENT_PID").is_number_integer()) {
        payload["pid"] = std::to_string(root.at("CLIENT_PID").get<std::int32_t>());
    } else {
        RESSCHED_LOGE("av_codec event pid format error!");
        return;
    }
    if (root.contains("CODEC_INSTANCE_ID") && root.at("CODEC_INSTANCE_ID").is_number_integer()) {
        payload["instanceId"] = std::to_string(root.at("CODEC_INSTANCE_ID").get<std::int32_t>());
    } else {
        RESSCHED_LOGE("av_codec event instanceId format error!");
        return;
    }

    if (eventName == "CODEC_START_INFO") {
        ResSchedMgr::GetInstance().ReportData(ResType::RES_TYPE_AV_CODEC_STATE,
            ResType::AvCodecState::CODEC_START_INFO, payload);
    } else if (eventName == "CODEC_STOP_INFO") {
        ResSchedMgr::GetInstance().ReportData(ResType::RES_TYPE_AV_CODEC_STATE,
            ResType::AvCodecState::CODEC_STOP_INFO, payload);
    }
}

void HiSysEventObserver::ProcessRunningLockEvent(const nlohmann::json& root, const std::string& eventName)
{
    std::string str = root.dump(INDENT, ' ', false, nlohmann::json::error_handler_t::replace);
    RESSCHED_LOGD("Process runninglock event, event root:%{public}s", str.c_str());
    nlohmann::json payload;
    if (root.contains("UID") && root.at("UID").is_number_integer()) {
        payload["uid"] = std::to_string(root.at("UID").get<std::int32_t>());
    } else {
        RESSCHED_LOGE("running lock event uid format error!");
        return;
    }
    if (root.contains("PID") && root.at("PID").is_number_integer()) {
        payload["pid"] = std::to_string(root.at("PID").get<std::int32_t>());
    } else {
        RESSCHED_LOGE("running lock event pid format error!");
        return;
    }
    if (root.contains("TYPE") && root.at("TYPE").is_number_integer()) {
        payload["type"] = std::to_string(root.at("TYPE").get<std::uint32_t>());
    } else {
        RESSCHED_LOGE("running lock event lock type format error!");
        return;
    }

    if (root.contains("STATE") && root.at("STATE").is_number_integer()) {
        RunningLockState lockState = RunningLockState(root.at("STATE").get<std::int32_t>());
        RESSCHED_LOGD("Process runninglock event, event type is:%{public}d", lockState);
        switch (lockState) {
            case RunningLockState::RUNNINGLOCK_STATE_DISABLE: {
                ResSchedMgr::GetInstance().ReportData(ResType::RES_TYPE_RUNNINGLOCK_STATE,
                    RUNNINGLOCK_DISABLE, payload);
                break;
            }
            case RunningLockState::RUNNINGLOCK_STATE_ENABLE: {
                ResSchedMgr::GetInstance().ReportData(ResType::RES_TYPE_RUNNINGLOCK_STATE,
                    RUNNINGLOCK_ENABLE, payload);
                break;
            }
            case RunningLockState::RUNNINGLOCK_STATE_PROXIED:
            case RunningLockState::RUNNINGLOCK_STATE_UNPROXIED_RESTORE: {
                ResSchedMgr::GetInstance().ReportData(ResType::RES_TYPE_RUNNINGLOCK_STATE,
                    RUNNINGLOCK_PROXIED, payload);
                break;
            }
            default:
                break;
        }
    } else {
        RESSCHED_LOGE("running lock event state format error!");
        return;
    }
}

void HiSysEventObserver::ProcessAudioEvent(const nlohmann::json& root, const std::string& eventName)
{
    std::string str = root.dump(INDENT, ' ', false, nlohmann::json::error_handler_t::replace);
    RESSCHED_LOGD("Process audio event, event root :%{public}s", str.c_str());
    nlohmann::json payload;
    if (root.contains("UID") && root.at("UID").is_number_integer()) {
        payload["uid"] = root.at("UID").get<std::int32_t>();
    } else {
        RESSCHED_LOGE("audio event uid format error!");
        return;
    }
    if (root.contains("PID") && root.at("PID").is_number_integer()) {
        payload["pid"] = root.at("PID").get<std::int32_t>();
    } else {
        RESSCHED_LOGE("audio event pid format error!");
        return;
    }

    if (root.contains("STATE") && root.at("STATE").is_number_integer()) {
        AudioState audioState = AudioState(root.at("STATE").get<std::int32_t>());
        RESSCHED_LOGD("Process audio event, event type is:%{public}d", audioState);
        switch (audioState) {
            case AudioState::AUDIO_STATE_RUNNING:
                ResSchedMgr::GetInstance().ReportData(ResType::RES_TYPE_AUDIO_RENDER_STATE_CHANGE,
                    ResType::AudioStatus::RENDERER_RUNNING, payload);
                break;
            case AudioState::AUDIO_STATE_STOPPED:
                ResSchedMgr::GetInstance().ReportData(ResType::RES_TYPE_AUDIO_RENDER_STATE_CHANGE,
                    ResType::AudioStatus::RENDERER_STOPPED, payload);
                break;
            case AudioState::AUDIO_STATE_RELEASED:
                ResSchedMgr::GetInstance().ReportData(ResType::RES_TYPE_AUDIO_RENDER_STATE_CHANGE,
                    ResType::AudioStatus::RENDERER_RELEASED, payload);
                break;
            case AudioState::AUDIO_STATE_PAUSED:
                ResSchedMgr::GetInstance().ReportData(ResType::RES_TYPE_AUDIO_RENDER_STATE_CHANGE,
                    ResType::AudioStatus::RENDERER_PAUSED, payload);
                break;
            default:
                break;
        }
    } else {
        RESSCHED_LOGE("audio event state format error!");
        return;
    }
}

void HiSysEventObserver::ProcessCameraEvent(const nlohmann::json& root, const std::string& eventName)
{
    std::string str = root.dump(INDENT, ' ', false, nlohmann::json::error_handler_t::replace);
    RESSCHED_LOGD("Process camera event, event root:%{public}s, eventName:%{public}s", str.c_str(), eventName.c_str());
    nlohmann::json payload;
    if (root.contains("UID") && root.at("UID").is_number_integer()) {
        payload["uid"] = std::to_string(root.at("UID").get<std::int32_t>());
    } else {
        RESSCHED_LOGE("camera event uid format error!");
        return;
    }
    if (root.contains("PID") && root.at("PID").is_number_integer()) {
        payload["pid"] = std::to_string(root.at("PID").get<std::int32_t>());
    } else {
        RESSCHED_LOGE("camera event pid format error!");
        return;
    }

    if (eventName == CAMERA_CONNECT) {
        ResSchedMgr::GetInstance().ReportData(ResType::RES_TYPE_REPORT_CAMERA_STATE, CAMERACONNECT, payload);
    } else {
        ResSchedMgr::GetInstance().ReportData(ResType::RES_TYPE_REPORT_CAMERA_STATE, CAMERADISCONNECT, payload);
    }
}

void HiSysEventObserver::ProcessBluetoothEvent(const nlohmann::json& root, const std::string& eventName)
{
    std::string str = root.dump(INDENT, ' ', false, nlohmann::json::error_handler_t::replace);
    RESSCHED_LOGD("Process bluetooth event, event root :%{public}s", str.c_str());
    nlohmann::json payload;
    if (root.contains("UID") && root.at("UID").is_number_integer()) {
        payload["uid"] = std::to_string(root.at("UID").get<std::int32_t>());
    } else {
        RESSCHED_LOGE("bluetooth event uid format error!");
        return;
    }
    if (root.contains("PID") && root.at("PID").is_number_integer()) {
        payload["pid"] = std::to_string(root.at("PID").get<std::int32_t>());
    } else {
        RESSCHED_LOGE("bluetooth event pid format error!");
        return;
    }

#ifdef RESSCHED_COMMUNICATION_BLUETOOTH_ENABLE
    if (root.contains("STATE") && root.at("STATE").is_number_integer()) {
        RESSCHED_LOGD("Process bluetooth event, event type is:%{public}d", root.at("STATE").get<std::int32_t>());
        if (root.at("STATE").get<std::int32_t>() == Bluetooth::BTStateID::STATE_TURN_ON) {
            ResSchedMgr::GetInstance().ReportData(ResType::RES_TYPE_BLUETOOTH_A2DP_CONNECT_STATE_CHANGE,
                Bluetooth::BTStateID::STATE_TURN_ON, payload);
        } else if (root.at("STATE").get<std::int32_t>() == Bluetooth::BTStateID::STATE_TURN_OFF) {
            ResSchedMgr::GetInstance().ReportData(ResType::RES_TYPE_BLUETOOTH_A2DP_CONNECT_STATE_CHANGE,
                Bluetooth::BTStateID::STATE_TURN_OFF, payload);
        }
    } else {
        RESSCHED_LOGE("Bluetooth event type not support!");
        return;
    }
#endif
}

void HiSysEventObserver::ProcessWifiEvent(const nlohmann::json& root, const std::string& eventName)
{
    std::string str = root.dump(INDENT, ' ', false, nlohmann::json::error_handler_t::replace);
    RESSCHED_LOGD("Process wifi event, event root :%{public}s, eventName:%{public}s", str.c_str(), eventName.c_str());
    nlohmann::json payload;
    if (root.contains("uid_") && root.at("uid_").is_number_integer()) {
        payload["uid"] = std::to_string(root.at("uid_").get<std::int32_t>());
    } else {
        RESSCHED_LOGE("Wifi event uid format error!");
        return;
    }
    if (root.contains("pid_") && root.at("pid_").is_number_integer()) {
        payload["pid"] = std::to_string(root.at("pid_").get<std::int32_t>());
    } else {
        RESSCHED_LOGE("Wifi event pid format error!");
        return;
    }

    WifiState connectionType;
    if (root.contains("TYPE") && root.at("TYPE").is_number_integer()) {
        connectionType = WifiState(root.at("TYPE").get<std::int32_t>());
    } else {
        RESSCHED_LOGE("Wifi event type format error!");
        return;
    }
    if (eventName == WIFI_CONNECTION) {
        switch (connectionType) {
            case WifiState::CONNECTED:
                ResSchedMgr::GetInstance().ReportData(ResType::RES_TYPE_WIFI_CONNECT_STATE_CHANGE,
                    WIFICONNECTED, payload);
                break;
            case WifiState::DISCONNECTED:
                ResSchedMgr::GetInstance().ReportData(ResType::RES_TYPE_WIFI_CONNECT_STATE_CHANGE,
                    WIFIDISCONNECTED, payload);
                break;
            default:
                break;
        }
    } else if (eventName == WIFI_SCAN) {
        ResSchedMgr::GetInstance().ReportData(ResType::RES_TYPE_WIFI_CONNECT_STATE_CHANGE, WIFISCAN, payload);
    } else {
        RESSCHED_LOGE("Wifi event name not support!");
        return;
    }
}

void HiSysEventObserver::ProcessScreenCaptureEvent(const nlohmann::json& root, const std::string& eventName)
{
    std::string str = root.dump(INDENT, ' ', false, nlohmann::json::error_handler_t::replace);
    RESSCHED_LOGD("Process screen capture event, event root:%{public}s, eventName:%{public}s", str.c_str(), eventName.c_str());
    nlohmann::json payload;
    if (root.contains("APP_UID") && root.at("APP_UID").is_number_integer()) {
        payload["uid"] = std::to_string(root.at("APP_UID").get<std::int32_t>());
    } else {
        RESSCHED_LOGE("screen capture uid format error!");
        return;
    }
    if (root.contains("APP_PID") && root.at("APP_PID").is_number_integer()) {
        payload["pid"] = std::to_string(root.at("APP_PID").get<std::int32_t>());
    } else {
        RESSCHED_LOGE("screen capture pid format error!");
        return;
    }

    if (!root.contains("STATUS") || !root.at("STATUS").is_string()) {
        return;
    }

    if (root.at("STATUS") == "start") {
        ResSchedMgr::GetInstance().ReportData(ResType::RES_TYPE_REPORT_SCREEN_CAPTURE,
            ResType::ScreenCaptureStatus::START_SCREEN_CAPTURE, payload);
    } else if (root.at("STATUS") == "stop") {
        ResSchedMgr::GetInstance().ReportData(ResType::RES_TYPE_REPORT_SCREEN_CAPTURE,
            ResType::ScreenCaptureStatus::STOP_SCREEN_CAPTURE, payload);
    } else {
        RESSCHED_LOGE("screen capture status not support!");
    }
}

void HiSysEventObserver::OnServiceDied()
{
    RESSCHED_LOGE("HiSysEventObserver service disconnected");
}
} // namespace ResourceSchedule
} // namespace OHOS
