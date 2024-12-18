/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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

namespace OHOS {
namespace ResourceSchedule {
class SocPerfPlugin : public Plugin {
    DECLARE_SINGLE_INSTANCE(SocPerfPlugin)

public:
    void Init() override;

    void Disable() override;

    void DispatchResource(const std::shared_ptr<ResData>& resData) override;

private:
    using ReqAppTypeFunc = int32_t (*)(const std::string &bundleName);
    std::set<uint32_t> resTypes;
    std::unordered_map<uint32_t, std::function<void(const std::shared_ptr<ResData>& data)>> functionMap;
    std::map<int32_t, int32_t> uidToAppTypeMap_;
    std::string perfReqAppTypeSoPath_;
    std::string perfReqAppTypeSoFunc_;
    int32_t focusAppType_;
    int64_t screenStatus_;
    std::string deviceMode_;
    ffrt::mutex screenMutex_;
    void* handle_ = nullptr;
    ReqAppTypeFunc reqAppTypeFunc_ = nullptr;
    int32_t RES_TYPE_SCENE_BOARD_ID = 0;
    int32_t RES_TYPE_RGM_BOOTING_STATUS = 0;
    bool socperfGameBoostSwitch_ = false;
    void InitEventId();
    void InitFunctionMap();
    void AddEventToFunctionMap();
    void InitResTypes();
    void InitPerfCrucialSo();
    void InitPerfCrucialFunc(const char* perfSoPath, const char* perfSoFunc);
    void HandleWindowFocus(const std::shared_ptr<ResData>& data);
    void HandleEventClick(const std::shared_ptr<ResData>& data);
    void HandleEventKey(const std::shared_ptr<ResData>& data);
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
    void HandleWebDragResize(const std::shared_ptr<ResData>& data);
    bool HandleSocperfSceneBoard(const std::shared_ptr<ResData>& data);
    bool HandleSocperfAccountActivating(const std::shared_ptr<ResData> &data);
    bool HandleCustEvent(const std::shared_ptr<ResData>& data);
    bool HandleCustEventBegin(const std::shared_ptr<ResData>& data);
    bool HandleCustEventEnd(const std::shared_ptr<ResData>& data);
    bool HandleRgmBootingStatus(const std::shared_ptr<ResData>& data);
    bool HandleAppColdStartEx(const std::shared_ptr<ResData>& data);
    bool HandleSceneRotation(const std::shared_ptr<ResData>& data);
    bool HandleBmmMoniterStatus(const std::shared_ptr<ResData>& data);
    bool HandlePowerModeChanged(const std::shared_ptr<ResData>& data);
    bool HandleScreenStatusAnalysis(const std::shared_ptr<ResData>& data);
    bool HandleGameBoost(const std::shared_ptr<ResData>& data);
    bool UpdateFocusAppType(const std::shared_ptr<ResData>& data);
    bool HandleUninstallEvent(const std::shared_ptr<ResData>& data);
    bool UpdateUidToAppTypeMap(const std::shared_ptr<ResData>& data);
    bool UpdateUidToAppTypeMap(const std::shared_ptr<ResData>& data, const int32_t appType);
    int32_t GetUidByData(const std::shared_ptr<ResData>& data);
    void HandleScreenOn();
    void HandleScreenOff();
    std::string GetBundleNameByUid(const int32_t uid);
};
} // namespace ResourceSchedule
} // namespace OHOS

#endif //RESSCHED_RESOURCESCHEDULE_SOC_PERF_ENABLE
#endif // RESSCHED_PLUGINS_SOCPERF_PLUGIN_INCLUDE_SOCPERF_PLUGIN_H
