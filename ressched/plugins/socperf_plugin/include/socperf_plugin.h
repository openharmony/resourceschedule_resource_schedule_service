/*
 * Copyright (c) 2022-2025 Huawei Device Co., Ltd.
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

#ifndef RESSCHED_PLUGINS_SOCPERF_PLUGIN_INCLUDE_SOCPERF_PLUGIN_H
#define RESSCHED_PLUGINS_SOCPERF_PLUGIN_INCLUDE_SOCPERF_PLUGIN_H
#ifdef RESSCHED_RESOURCESCHEDULE_SOC_PERF_ENABLE
#include <set>
#include <unordered_map>
#include <mutex>
#include "ffrt.h"
#include "plugin.h"
#include "single_instance.h"
#include "socperf_client.h"
#include <vector>
#include <string>
#include <sstream>

namespace OHOS {
namespace ResourceSchedule {
class AppKeyMessage {
private:
    int32_t appType;
    std::string bundleName;

public:
    AppKeyMessage(int32_t appType = 0,
        const std::string& bundleName = "") : appType(appType), bundleName(bundleName) {}

    int32_t GetAppType() const
    {
        return appType;
    }

    void setAppType(int32_t newAppType)
    {
        appType = newAppType;
    }

    std::string GetBundleName() const
    {
        return bundleName;
    }

    void setBundleName(const std::string& newBundleName)
    {
        bundleName = newBundleName;
    }
};

struct Frequencies {
    std::vector<int32_t> tags;
    std::vector<int64_t> configs;
};

/**
 * Type for acquire BatteryChargeState.
 */
enum class BatteryChargeState : uint32_t {
    // Battery is discharge
    CHARGE_STATE_NONE,
    // Battery is charging
    CHARGE_STATE_ENABLE,
    // Battery is not charging
    CHARGE_STATE_DISABLE,
    // Battery charge full
    CHARGE_STATE_FULL,
    // The bottom of the enum
    CHARGE_STATE_BUTT
};

class SocPerfPlugin : public Plugin {
    DECLARE_SINGLE_INSTANCE(SocPerfPlugin)

public:
    void Init() override;

    void Disable() override;

    void DispatchResource(const std::shared_ptr<ResData>& resData) override;

private:
    using ReqAppTypeFunc = int32_t (*)(const std::string &bundleName);
    std::set<uint32_t> resTypes;
    std::set<int32_t> focusAppUids_;
    bool weakActionStatus_ = true;
    std::set<std::string> keyAppName_;
    std::set<int32_t> focusCustGameUids_;
    std::set<int32_t> keyAppType_;
    // app names set which use camera
    std::set<std::string> appNameUseCamera_;
    // app names set which add move boost
    std::set<std::string> appNameMoveEvent_;
    std::unordered_map<uint32_t, std::function<void(const std::shared_ptr<ResData>& data)>> functionMap;
    // app's uid match app
    std::map<int32_t, AppKeyMessage> uidToAppMsgMap_;
    // app's pid match app type
    std::map<int32_t, int32_t> pidToAppTypeMap_;
    // socperf battery capacity config
    std::map<int32_t, Frequencies> socperfBatteryConfig_;
    std::string perfReqAppTypeSoPath_;
    std::string perfReqAppTypeSoFunc_;
    bool isFocusAppsGameType_ = false;
    int64_t screenStatus_;
    std::string deviceMode_;
    std::string deviceOrientation_;
    ffrt::mutex screenMutex_;
    void* handle_ = nullptr;
    ReqAppTypeFunc reqAppTypeFunc_ = nullptr;
    int32_t RES_TYPE_SCENE_BOARD_ID = 0;
    int32_t RES_TYPE_RGM_BOOTING_STATUS = 0;
    int32_t maxBatteryLimitCapacity_ = -1;
    int32_t lastBatteryLimitCap_ = -1;
    bool socperfGameBoostSwitch_ = false;
    bool custGameState_ = false;
    void InitEventId();
    void InitFunctionMap();
    void AddEventToFunctionMap();
    void AddOtherEventToFunctionMap();
    void InitResTypes();
    void InitOtherResTypes();
    void InitPerfCrucialSo();
    void InitWeakInterAction();
    void AddKeyAppName(const std::string& subValue);
    void AddKeyAppType(const std::string& subValue);
    void UpdateWeakActionStatus();
    void InitPerfCrucialFunc(const char* perfSoPath, const char* perfSoFunc);
    void HandleWindowFocus(const std::shared_ptr<ResData>& data);
    void HandleEventClick(const std::shared_ptr<ResData>& data);
    void HandleEventKey(const std::shared_ptr<ResData>& data);
#ifdef RESSCHED_RESOURCESCHEDULE_CROWN_POWER_KEY_ENABLE
    void HandlePowerEventKey(const std::shared_ptr<ResData>& data);
    void HandleCrownRotation(const std::shared_ptr<ResData>& data);
#endif // RESSCHED_RESOURCESCHEDULE_CROWN_POWER_KEY_ENABLE
    void HandleLoadPage(const std::shared_ptr<ResData>& data);
    void HandlePopPage(const std::shared_ptr<ResData>& data);
    void HandleEventSlide(const std::shared_ptr<ResData>& data);
    void HandleEventWebGesture(const std::shared_ptr<ResData>& data);
    void HandleAppAbilityStart(const std::shared_ptr<ResData>& data);
    void HandleResizeWindow(const std::shared_ptr<ResData>& data);
    void HandleMoveWindow(const std::shared_ptr<ResData>& data);
    void HandleRemoteAnimation(const std::shared_ptr<ResData>& data);
    bool InitFeatureSwitch(std::string featureName);
    void HandleDragStatusBar(const std::shared_ptr<ResData>& data);
    void HandleWebGestureMove(const std::shared_ptr<ResData>& data);
    void HandleWebSlideNormal(const std::shared_ptr<ResData>& data);
    void HandleLoadUrl(const std::shared_ptr<ResData>& data);
    void HandleMousewheel(const std::shared_ptr<ResData>& data);
    bool HandleAppStateChange(const std::shared_ptr<ResData>& data);
    void HandleDeviceModeStatusChange(const std::shared_ptr<ResData>& data);
    void HandleDeviceOrientationStatusChange(const std::shared_ptr<ResData>& data);
    void HandleWebDragResize(const std::shared_ptr<ResData>& data);
    bool HandleSocperfSceneBoard(const std::shared_ptr<ResData>& data);
    bool HandleSocperfAccountActivating(const std::shared_ptr<ResData> &data);
#ifdef RESSCHED_RESOURCESCHEDULE_CUST_SOC_PERF_ENABLE
    bool HandleCustEvent(const std::shared_ptr<ResData>& data);
    bool HandleCustEventBegin(const std::shared_ptr<ResData>& data);
    bool HandleCustEventEnd(const std::shared_ptr<ResData>& data);
#endif // RESSCHED_RESOURCESCHEDULE_CUST_SOC_PERF_ENABLE
    bool HandleRgmBootingStatus(const std::shared_ptr<ResData>& data);
    bool HandleAppColdStartEx(const std::shared_ptr<ResData>& data);
    bool HandleSceneRotation(const std::shared_ptr<ResData>& data);
    bool HandleBmmMoniterStatus(const std::shared_ptr<ResData>& data);
    bool HandlePowerModeChanged(const std::shared_ptr<ResData>& data);
    bool HandleScreenStatusAnalysis(const std::shared_ptr<ResData>& data);
    bool HandleGameBoost(const std::shared_ptr<ResData>& data);
    bool UpdateFocusAppType(const std::shared_ptr<ResData>& data, bool focusStatus);
    bool HandleUninstallEvent(const std::shared_ptr<ResData>& data);
    bool UpdateUidToAppMsgMap(const std::shared_ptr<ResData>& data);
    bool UpdateUidToAppMsgMap(const std::shared_ptr<ResData>& data, const int32_t appType,
        const std::string& bundleName);
    bool IsFocusAppsAllGame();
    bool UpdatesFocusAppsType(int32_t appType);
    bool HandleDeadProcess(const std::shared_ptr<ResData>& data);
    bool IsGameEvent(const std::shared_ptr<ResData>& data);
    bool HandleProcessStateChange(const std::shared_ptr<ResData> &data);
    bool HandleCameraStateChange(const std::shared_ptr<ResData> &data);
    bool InitBundleNameBoostList();
    bool HandleSubValue(const std::string& subValue, std::set<std::string>& nameSet);
    bool IsAllowBoostScene();
    bool HandleMoveEventBoost(const std::shared_ptr<ResData>& data, bool isSet);
    bool HandleSceenModeBoost(const std::string& deviceModeType);
    bool HandleSceenOrientationBoost(const std::string& deviceModeType);
    bool HandleGameStateChange(const std::shared_ptr<ResData>& data);
    bool UpdateCustGameState(const std::shared_ptr<ResData>& data);
    bool HandleCustAction(const std::shared_ptr<ResData> &data);
    bool HandleRecentBuild(const std::shared_ptr<ResData>& data);
    bool InitBatteryCapacityLimitFreq();
    bool HandleBatteryStatusChange(const std::shared_ptr<ResData>& data);
    bool HandleBatterySubValue(const int32_t capacity, const int32_t tag, const int64_t config);
    bool HandleFreqLimit(const std::shared_ptr<ResData>& data, bool isChargeState);
    bool HandleChargingStatusChange(const std::shared_ptr<ResData>& data);
    bool HandleRecoverBatteryLimit();
    bool HandleBatteryLimit(int32_t capacity);
    int32_t GetLimitCapacity(int32_t capacity);
    int32_t GetPidByData(const std::shared_ptr<ResData>& data, const std::string& key);
    int32_t GetUidByData(const std::shared_ptr<ResData>& data);
    void HandleScreenOn();
    void HandleScreenOff();
    std::vector<int64_t> GetConfigs(int32_t size);
    std::string GetBundleNameByUid(const int32_t uid);
#ifdef RESSCHED_RESOURCESCHEDULE_FILE_COPY_SOC_PERF_ENABLE
    bool HandleFileCopyStatus(const std::shared_ptr<ResData>& data);
#endif
    void HandleWebSildeScroll(const std::shared_ptr<ResData>& data);
};
} // namespace ResourceSchedule
} // namespace OHOS

#endif //RESSCHED_RESOURCESCHEDULE_SOC_PERF_ENABLE
#endif // RESSCHED_PLUGINS_SOCPERF_PLUGIN_INCLUDE_SOCPERF_PLUGIN_H
