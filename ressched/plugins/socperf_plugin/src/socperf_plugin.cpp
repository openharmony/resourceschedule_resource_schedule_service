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

#ifdef RESSCHED_RESOURCESCHEDULE_SOC_PERF_ENABLE
#include "socperf_plugin.h"
#include "app_mgr_constants.h"
#include "bundle_mgr_interface.h"
#include "config_info.h"
#include "dlfcn.h"
#include "fcntl.h"
#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "plugin_mgr.h"
#include "res_type.h"
#include "socperf_log.h"
#include "system_ability_definition.h"

namespace OHOS {
namespace ResourceSchedule {
using namespace ResType;
namespace {
    const std::string LIB_NAME = "libsocperf_plugin.z.so";
    const std::string PLUGIN_NAME = "SOCPERF";
    const std::string CONFIG_NAME_SOCPERF_FEATURE_SWITCH = "socperfFeatureSwitch";
    const std::string CONFIG_NAME_SOCPERF_EVENT_ID = "socperfEventId";
    const std::string SUB_ITEM_KEY_NAME_SOCPERF_ON_DEMAND = "socperf_on_demand";
    const std::string SUB_ITEM_KEY_NAME_SOCPERF_GAME_BOOST = "socperf_game_boost";
    const std::string CONFIG_NAME_SOCPERF_BUNDLE_NAME_BOOST_LIST = "socperfBundleNameBoostList";
    const std::string SUB_ITEM_KEY_NAME_APP_USE_CAMERA_BOOST = "app_use_camera_boost";
    const std::string SUB_ITEM_KEY_NAME_APP_MOVE_EVENT_BOOST = "app_move_event_boost";
    const std::string CNOFIG_NAME_SOCPERF_CRUCIAL_FUNC = "socperfCrucialFunc";
    const std::string SUB_ITEM_KEY_NAME_SOCPERF_RERQ_APPTYPE_PATH = "socperf_req_apptype_path";
    const std::string SUB_ITEM_KEY_NAME_SOCPERF_RERQ_APPTYPE_FUNC = "socperf_req_apptype_func";
    const std::string BUNDLE_NAME = "bundleName";
    const std::string PID_NAME = "pid";
    const std::string CLIENT_PID_NAME = "clientPid";
    const std::string UID_NAME = "uid";
    const std::string CALLING_UID_NAME = "callingUid";
    const std::string SOCPERF_TYPE_ID = "socperf_type_id";
    const std::string SOCPERF_TYPE_RGM = "socperf_type_rgm";
    const std::string EXTENSION_TYPE_KEY = "extensionType";
    const std::string DEVICE_MODE_TYPE_KEY = "deviceModeType";
    const std::string DEVICE_MODE_PAYMODE_NAME = "deviceMode";
    const std::string DISPLAY_MODE_KEY = "display";
    const std::string DEVICE_ORIENTATION_TYPE_KEY = "deviceOrientation";
    const std::string DISPLAY_MODE_FULL = "displayFull";
    const std::string DISPLAY_MODE_MAIN = "displayMain";
    const std::string DISPLAY_MODE_SUB = "displaySub";
    const std::string SCREEN_MODE_KEY = "screenStatus";
    const std::string SCREEN_MODE_ON = "screen_on";
    const std::string SCREEN_MODE_OFF = "screen_off";
    const std::string POWER_MODE_KEY = "power";
    const std::string POWER_MODE = "powerMode";
    const std::string PRELOAD_MODE = "isPreload";
    const std::string WEAK_ACTION_STRING = "weakInterAction";
    const std::string WEAK_ACTION_MODE = "actionmode:weakaction";
    const std::string KEY_APP_TYPE = "key_app_type";
    const std::string GAME_ENV = "env";
    const int32_t INVALID_VALUE                             = -1;
    const int32_t APP_TYPE_GAME                             = 2;
    const int32_t INVALID_APP_TYPE                          = 0;
    const int32_t POWERMODE_ON                              = 601;
    const int64_t TIME_INTERVAL                             = 5000;
    const int64_t SCREEN_OFF_TIME_DELAY                     = 5000000L;
    const int32_t PERF_REQUEST_CMD_ID_RGM_BOOTING_START     = 1000;
    const int32_t PERF_REQUEST_CMD_ID_POWERMODE_CHANGED     = 9000;
    const int32_t PERF_REQUEST_CMD_ID_APP_START             = 10000;
    const int32_t PERF_REQUEST_CMD_ID_WARM_START            = 10001;
    const int32_t PERF_REQUEST_CMD_ID_WINDOW_SWITCH         = 10002;
    const int32_t PERF_REQUEST_CMD_ID_EVENT_CLICK           = 10006;
    const int32_t PERF_REQUEST_CMD_ID_LOAD_PAGE_START       = 10007;
    const int32_t PERF_REQUEST_CMD_ID_EVENT_FLING           = 10008;
    const int32_t PERF_REQUEST_CMD_ID_EVENT_SLIDE_OVER      = 10009;
    const int32_t PERF_REQUEST_CMD_ID_EVENT_TOUCH_DOWN      = 10010;
    const int32_t PERF_REQUEST_CMD_ID_LOAD_PAGE_COMPLETE    = 10011;
    const int32_t PERF_REQUEST_CMD_ID_EVENT_WEB_GESTURE     = 10012;
    const int32_t PERF_REQUEST_CMD_ID_POP_PAGE              = 10016;
    const int32_t PERF_REQUEST_CMD_ID_RESIZE_WINDOW         = 10018;
    const int32_t PERF_REQUEST_CMD_ID_MOVE_WINDOW           = 10019;
    const int32_t PERF_REQUEST_CMD_ID_WEB_GESTURE_MOVE      = 10020;
    const int32_t PERF_REQUEST_CMD_ID_WEB_SLIDE_NORMAL      = 10025;
    const int32_t PERF_REQUEST_CMD_ID_ROTATION              = 10027;
    const int32_t PERF_REQUEST_CMD_ID_SCREEN_ON             = 10028;
    const int32_t PERF_REQUEST_CMD_ID_SCREEN_OFF            = 10029;
    const int32_t PERF_REQUEST_CMD_ID_REMOTE_ANIMATION      = 10030;
    const int32_t PERF_REQUEST_CMD_ID_DRAG_STATUS_BAR       = 10034;
    const int32_t PERF_REQUEST_CMD_ID_GAME_START            = 10036;
    const int32_t PERF_REQUEST_CMD_ID_EVENT_TOUCH_UP        = 10040;
    const int32_t PERF_REQUEST_CMD_ID_REMOTE_UNLOCK         = 10041;
    const int32_t PERF_REQUEST_CMD_ID_ACCOUNT_ACTIVATING    = 10043;
    const int32_t PERF_REQUEST_CMD_ID_EVENT_KEY_DOWN        = 10044;
#ifdef RESSCHED_RESOURCESCHEDULE_CROWN_POWER_KEY_ENABLE
    const int32_t PERF_REQUEST_CMD_ID_POWER_KEY             = 10045;
    const int32_t PERF_REQUEST_CMD_ID_CROWN_ROTATION        = 10046;
#endif // RESSCHED_RESOURCESCHEDULE_CROWN_POWER_KEY_ENABLE
    const int32_t PERF_REQUEST_CMD_ID_LOAD_URL              = 10070;
    const int32_t PERF_REQUEST_CMD_ID_MOUSEWHEEL            = 10071;
    const int32_t PERF_REQUEST_CMD_ID_WEB_DRAG_RESIZE       = 10073;
    const int32_t PERF_REQUEST_CMD_ID_BMM_MONITER_START     = 10081;
    const int32_t PERF_REQUEST_CMD_ID_DISPLAY_MODE_FULL     = 10082;
    const int32_t PERF_REQUEST_CMD_ID_DISPLAY_MODE_MAIN     = 10083;
    const int32_t PERF_REQUEST_CMD_ID_GAME_BOOST            = 10085;
    const int32_t PERF_REQUEST_CMD_ID_APP_USE_CAMERA        = 10086;
#ifdef RESSCHED_RESOURCESCHEDULE_FILE_COPY_SOC_PERF_ENABLE
    const int32_t PERF_REQUEST_CMD_ID_FILE_COPY             = 10087;
#endif
    const int32_t PERF_REQUEST_CMD_ID_SCREEN_SWITCHED       = 10090;
    const int32_t PERF_REQUEST_CMD_ID_MOVE_EVENT_BOOST      = 10091;
    const int32_t PERF_REQUEST_CMD_ID_EVENT_DRAG            = 10092;
    const int32_t PERF_REQUEST_CMD_ID_GAME_BOOST_LEVEL1     = 10093;
    const int32_t PERF_REQUEST_CMD_ID_GAME_BOOST_LEVEL2     = 10094;
    const int32_t PERF_REQUEST_CMD_ID_GAME_BOOST_LEVEL3     = 10095;
    const int32_t PERF_REQUEST_CMD_ID_WEB_SLIDE_SCROLL      = 10097;
    const int32_t PERF_REQUEST_CMD_ID_DISPLAY_GLOBAL_L      = 10098;
    const int32_t PERF_REQUEST_CMD_ID_DISPLAY_GLOBAL_P      = 10203;
    const int32_t PERF_REQUEST_CMD_ID_RECENT_BUILD          = 10200;
}
IMPLEMENT_SINGLE_INSTANCE(SocPerfPlugin)

void SocPerfPlugin::Init()
{
    InitEventId();
    InitResTypes();
    InitFunctionMap();
    for (auto resType : resTypes) {
        PluginMgr::GetInstance().SubscribeResource(LIB_NAME, resType);
    }
    InitPerfCrucialSo();
    InitBundleNameBoostList();
    InitWeakInterAction();
    SOC_PERF_LOGI("SocPerfPlugin::Init success");
}

void SocPerfPlugin::InitPerfCrucialSo()
{
    PluginConfig itemLists = PluginMgr::GetInstance().GetConfig(PLUGIN_NAME, CNOFIG_NAME_SOCPERF_CRUCIAL_FUNC);
    for (const Item& item : itemLists.itemList) {
        for (SubItem sub : item.subItemList) {
            if (sub.name == SUB_ITEM_KEY_NAME_SOCPERF_RERQ_APPTYPE_PATH) {
                perfReqAppTypeSoPath_ = sub.value;
            }
            if (sub.name == SUB_ITEM_KEY_NAME_SOCPERF_RERQ_APPTYPE_FUNC) {
                perfReqAppTypeSoFunc_ = sub.value;
            }
        }
    }

    if (!perfReqAppTypeSoPath_.empty() && !perfReqAppTypeSoFunc_.empty()) {
        InitPerfCrucialFunc(perfReqAppTypeSoPath_.c_str(), perfReqAppTypeSoFunc_.c_str());
    }
    PluginMgr::GetInstance().RemoveConfig(PLUGIN_NAME, CNOFIG_NAME_SOCPERF_CRUCIAL_FUNC);
}

void SocPerfPlugin::InitWeakInterAction()
{
    PluginConfig itemLists = PluginMgr::GetInstance().GetConfig(PLUGIN_NAME, WEAK_ACTION_STRING);
    for (const Item& item : itemLists.itemList) {
        for (SubItem sub : item.subItemList) {
            if (sub.name == BUNDLE_NAME) {
                AddKeyAppName(sub.value);
            } else if (sub.name == KEY_APP_TYPE) {
                AddKeyAppType(sub.value);
            }
        }
    }
    PluginMgr::GetInstance().RemoveConfig(PLUGIN_NAME, WEAK_ACTION_STRING);
}

void SocPerfPlugin::AddKeyAppName(const std::string& subValue)
{
    if (subValue.empty()) {
        return;
    }
    keyAppName_.insert(subValue);
}

void SocPerfPlugin::AddKeyAppType(const std::string& subValue)
{
    if (subValue.empty()) {
        return;
    }
    keyAppType_.insert(atoi(subValue.c_str()));
}

void SocPerfPlugin::InitPerfCrucialFunc(const char* perfSoPath, const char* perfSoFunc)
{
    if (!perfSoPath || !perfSoFunc) {
        return;
    }
    handle_ = dlopen(perfSoPath, RTLD_NOW);
    if (!handle_) {
        SOC_PERF_LOGE("perf so doesn't exist");
        return;
    }

    reqAppTypeFunc_ = reinterpret_cast<ReqAppTypeFunc>(dlsym(handle_, perfSoFunc));
    if (!reqAppTypeFunc_) {
        SOC_PERF_LOGE("perf func req app type doesn't exist");
        dlclose(handle_);
        handle_ = nullptr;
    }
}

void SocPerfPlugin::InitEventId()
{
    PluginConfig itemLists = PluginMgr::GetInstance().GetConfig(PLUGIN_NAME, CONFIG_NAME_SOCPERF_EVENT_ID);
    for (const Item& item : itemLists.itemList) {
        for (SubItem sub : item.subItemList) {
            if (sub.name == SOCPERF_TYPE_ID) {
                RES_TYPE_SCENE_BOARD_ID = atoi(sub.value.c_str());
            } else if (sub.name == SOCPERF_TYPE_RGM) {
                RES_TYPE_RGM_BOOTING_STATUS = atoi(sub.value.c_str());
            }
        }
    }
    PluginMgr::GetInstance().RemoveConfig(PLUGIN_NAME, CONFIG_NAME_SOCPERF_EVENT_ID);
}

bool SocPerfPlugin::InitBundleNameBoostList()
{
    PluginConfig itemLists = PluginMgr::GetInstance().GetConfig(PLUGIN_NAME,
        CONFIG_NAME_SOCPERF_BUNDLE_NAME_BOOST_LIST);
    bool ret = false;
    for (const Item& item : itemLists.itemList) {
        for (SubItem sub : item.subItemList) {
            if (sub.name == SUB_ITEM_KEY_NAME_APP_USE_CAMERA_BOOST) {
                ret = HandleSubValue(sub.value.c_str(), appNameUseCamera_);
            } else if (sub.name == SUB_ITEM_KEY_NAME_APP_MOVE_EVENT_BOOST) {
                ret = HandleSubValue(sub.value.c_str(), appNameMoveEvent_);
            }
        }
    }
    PluginMgr::GetInstance().RemoveConfig(PLUGIN_NAME, CONFIG_NAME_SOCPERF_BUNDLE_NAME_BOOST_LIST);
    return ret;
}

bool SocPerfPlugin::HandleSubValue(const std::string& subValue, std::set<std::string>& nameSet)
{
    if (subValue.empty()) {
        return false;
    }
    std::stringstream sstream(subValue);
    std::string bundleName;
    while (std::getline(sstream, bundleName, ',')) {
        nameSet.insert(bundleName);
    }
    return true;
}

void SocPerfPlugin::InitFunctionMap()
{
    functionMap = {
        { RES_TYPE_WINDOW_FOCUS,
            [this](const std::shared_ptr<ResData>& data) { HandleWindowFocus(data); } },
        { RES_TYPE_CLICK_RECOGNIZE,
            [this](const std::shared_ptr<ResData>& data) { HandleEventClick(data); } },
        { RES_TYPE_LOAD_PAGE,
            [this](const std::shared_ptr<ResData>& data) { HandleLoadPage(data); } },
        { RES_TYPE_SLIDE_RECOGNIZE,
            [this](const std::shared_ptr<ResData>& data) { HandleEventSlide(data); } },
        { RES_TYPE_WEB_GESTURE,
            [this](const std::shared_ptr<ResData>& data) { HandleEventWebGesture(data); } },
        { RES_TYPE_POP_PAGE,
            [this](const std::shared_ptr<ResData>& data) { HandlePopPage(data); } },
        { RES_TYPE_APP_ABILITY_START,
            [this](const std::shared_ptr<ResData>& data) { HandleAppAbilityStart(data); } },
        { RES_TYPE_RESIZE_WINDOW,
            [this](const std::shared_ptr<ResData>& data) { HandleResizeWindow(data); } },
        { RES_TYPE_MOVE_WINDOW,
            [this](const std::shared_ptr<ResData>& data) { HandleMoveWindow(data); } },
        { RES_TYPE_SHOW_REMOTE_ANIMATION,
            [this](const std::shared_ptr<ResData>& data) { HandleRemoteAnimation(data); } },
        { RES_TYPE_DRAG_STATUS_BAR,
            [this](const std::shared_ptr<ResData>& data) { HandleDragStatusBar(data); } },
        { RES_TYPE_WEB_GESTURE_MOVE,
            [this](const std::shared_ptr<ResData>& data) { HandleWebGestureMove(data); } },
        { RES_TYPE_WEB_SLIDE_NORMAL,
            [this](const std::shared_ptr<ResData>& data) { HandleWebSlideNormal(data); } },
        { RES_TYPE_LOAD_URL,
            [this](const std::shared_ptr<ResData>& data) { HandleLoadUrl(data); } },
        { RES_TYPE_MOUSEWHEEL,
            [this](const std::shared_ptr<ResData>& data) { HandleMousewheel(data); } },
        { RES_TYPE_APP_STATE_CHANGE,
            [this](const std::shared_ptr<ResData>& data) { HandleAppStateChange(data); } },
        { RES_TYPE_DEVICE_MODE_STATUS,
            [this](const std::shared_ptr<ResData>& data) { HandleDeviceModeStatusChange(data); } },
        { RES_TYPE_WEB_DRAG_RESIZE,
            [this](const std::shared_ptr<ResData>& data) { HandleWebDragResize(data); } },
        { RES_TYPE_ACCOUNT_ACTIVATING,
            [this](const std::shared_ptr<ResData>& data) { HandleSocperfAccountActivating(data); } },
#ifdef RESSCHED_RESOURCESCHEDULE_FILE_COPY_SOC_PERF_ENABLE
        { RES_TYPE_FILE_COPY_STATUS,
            [this](const std::shared_ptr<ResData>& data) { HandleFileCopyStatus(data); } },
#endif
        { RES_TYPE_WEB_SLIDE_SCROLL,
            [this](const std::shared_ptr<ResData>& data) { HandleWebSildeScroll(data); } },
    };
    AddEventToFunctionMap();
}

void SocPerfPlugin::AddEventToFunctionMap()
{
#ifdef RESSCHED_RESOURCESCHEDULE_CUST_SOC_PERF_ENABLE
    functionMap.insert(std::make_pair(RES_TYPE_ANCO_CUST,
        [this](const std::shared_ptr<ResData>& data) { HandleCustEvent(data); }));
    functionMap.insert(std::make_pair(RES_TYPE_SOCPERF_CUST_EVENT_BEGIN,
        [this](const std::shared_ptr<ResData>& data) { HandleCustEventBegin(data); }));
    functionMap.insert(std::make_pair(RES_TYPE_SOCPERF_CUST_EVENT_END,
        [this](const std::shared_ptr<ResData>& data) { HandleCustEventEnd(data); }));
    functionMap.insert(std::make_pair(RES_TYPE_SOCPERF_CUST_ACTION,
        [this](const std::shared_ptr<ResData>& data) { HandleCustAction(data); }));
#endif // RESSCHED_RESOURCESCHEDULE_CUST_SOC_PERF_ENABLE
    functionMap.insert(std::make_pair(RES_TYPE_ONLY_PERF_APP_COLD_START,
        [this](const std::shared_ptr<ResData>& data) { HandleAppColdStartEx(data); }));
    functionMap.insert(std::make_pair(RES_TYPE_SCENE_ROTATION,
        [this](const std::shared_ptr<ResData>& data) { HandleSceneRotation(data); }));
    functionMap.insert(std::make_pair(RES_TYPE_BMM_MONITER_CHANGE_EVENT,
        [this](const std::shared_ptr<ResData>& data) { HandleBmmMoniterStatus(data); }));
    functionMap.insert(std::make_pair(RES_TYPE_POWER_MODE_CHANGED,
        [this](const std::shared_ptr<ResData>& data) { HandlePowerModeChanged(data); }));
    functionMap.insert(std::make_pair(RES_TYPE_SCREEN_STATUS,
        [this](const std::shared_ptr<ResData>& data) { HandleScreenStatusAnalysis(data); }));
    functionMap.insert(std::make_pair(RES_TYPE_APP_GAME_BOOST_EVENT,
        [this](const std::shared_ptr<ResData>& data) { HandleGameBoost(data); }));
    functionMap.insert(std::make_pair(RES_TYPE_KEY_EVENT,
        [this](const std::shared_ptr<ResData>& data) { HandleEventKey(data); }));
#ifdef RESSCHED_RESOURCESCHEDULE_CROWN_POWER_KEY_ENABLE
    functionMap.insert(std::make_pair(RES_TYPE_MMI_INPUT_POWER_KEY,
        [this](const std::shared_ptr<ResData>& data) { HandlePowerEventKey(data); }));
    functionMap.insert(std::make_pair(RES_TYPE_CROWN_ROTATION_STATUS,
        [this](const std::shared_ptr<ResData>& data) { HandleCrownRotation(data); }));
#endif // RESSCHED_RESOURCESCHEDULE_CROWN_POWER_KEY_ENABLE
    functionMap.insert(std::make_pair(RES_TYPE_APP_INSTALL_UNINSTALL,
        [this](const std::shared_ptr<ResData>& data) { HandleUninstallEvent(data); }));
    functionMap.insert(std::make_pair(RES_TYPE_PROCESS_STATE_CHANGE,
        [this](const std::shared_ptr<ResData>& data) { HandleProcessStateChange(data); }));
    functionMap.insert(std::make_pair(RES_TYPE_REPORT_CAMERA_STATE,
        [this](const std::shared_ptr<ResData>& data) { HandleCameraStateChange(data); }));
    functionMap.insert(std::make_pair(RES_TYPE_REPORT_GAME_STATE_CHANGE,
        [this](const std::shared_ptr<ResData>& data) { HandleGameStateChange(data); }));
    if (RES_TYPE_SCENE_BOARD_ID != 0) {
        functionMap.insert(std::make_pair(RES_TYPE_SCENE_BOARD_ID,
            [this](const std::shared_ptr<ResData>& data) { HandleSocperfSceneBoard(data); }));
    }
    if (RES_TYPE_RGM_BOOTING_STATUS != 0) {
        functionMap.insert(std::make_pair(RES_TYPE_RGM_BOOTING_STATUS,
            [this](const std::shared_ptr<ResData>& data) { HandleRgmBootingStatus(data); }));
    }
    AddOtherEventToFunctionMap();
}

void SocPerfPlugin::AddOtherEventToFunctionMap()
{
    functionMap.insert(std::make_pair(RES_TYPE_RECENT_BUILD,
        [this](const std::shared_ptr<ResData>& data) { HandleRecentBuild(data); }));
    functionMap.insert(std::make_pair(RES_TYPE_DEVICE_ORIENTATION_STATUS,
        [this](const std::shared_ptr<ResData>& data) { HandleDeviceOrientationStatusChange(data); }));
    socperfGameBoostSwitch_ = InitFeatureSwitch(SUB_ITEM_KEY_NAME_SOCPERF_GAME_BOOST);
}

void SocPerfPlugin::InitResTypes()
{
    resTypes = {
        RES_TYPE_WINDOW_FOCUS,
        RES_TYPE_CLICK_RECOGNIZE,
        RES_TYPE_KEY_EVENT,
        RES_TYPE_LOAD_PAGE,
        RES_TYPE_SLIDE_RECOGNIZE,
        RES_TYPE_WEB_GESTURE,
        RES_TYPE_POP_PAGE,
        RES_TYPE_APP_ABILITY_START,
        RES_TYPE_RESIZE_WINDOW,
        RES_TYPE_MOVE_WINDOW,
        RES_TYPE_SHOW_REMOTE_ANIMATION,
        RES_TYPE_DRAG_STATUS_BAR,
        RES_TYPE_WEB_GESTURE_MOVE,
        RES_TYPE_WEB_SLIDE_NORMAL,
        RES_TYPE_LOAD_URL,
        RES_TYPE_MOUSEWHEEL,
        RES_TYPE_APP_STATE_CHANGE,
        RES_TYPE_DEVICE_MODE_STATUS,
        RES_TYPE_WEB_DRAG_RESIZE,
        RES_TYPE_ACCOUNT_ACTIVATING,
        RES_TYPE_DEVICE_ORIENTATION_STATUS,
#ifdef RESSCHED_RESOURCESCHEDULE_CUST_SOC_PERF_ENABLE
        RES_TYPE_ANCO_CUST,
        RES_TYPE_SOCPERF_CUST_EVENT_BEGIN,
        RES_TYPE_SOCPERF_CUST_EVENT_END,
        RES_TYPE_SOCPERF_CUST_ACTION,
#endif // RESSCHED_RESOURCESCHEDULE_CUST_SOC_PERF_ENABLE
        RES_TYPE_ONLY_PERF_APP_COLD_START,
        RES_TYPE_SCENE_ROTATION,
        RES_TYPE_BMM_MONITER_CHANGE_EVENT,
        RES_TYPE_POWER_MODE_CHANGED,
        RES_TYPE_SCREEN_STATUS,
        RES_TYPE_APP_GAME_BOOST_EVENT,
        RES_TYPE_APP_INSTALL_UNINSTALL,
        RES_TYPE_MMI_INPUT_POWER_KEY,
        RES_TYPE_CROWN_ROTATION_STATUS,
        RES_TYPE_PROCESS_STATE_CHANGE,
        RES_TYPE_REPORT_CAMERA_STATE,
        RES_TYPE_REPORT_GAME_STATE_CHANGE,
#ifdef RESSCHED_RESOURCESCHEDULE_FILE_COPY_SOC_PERF_ENABLE
        RES_TYPE_FILE_COPY_STATUS,
#endif
        RES_TYPE_WEB_SLIDE_SCROLL,
    };
    InitOtherResTypes();
}

void SocPerfPlugin::InitOtherResTypes()
{
    resTypes.insert(RES_TYPE_RECENT_BUILD);
    if (RES_TYPE_SCENE_BOARD_ID != 0) {
        resTypes.insert(RES_TYPE_SCENE_BOARD_ID);
    }
    if (RES_TYPE_RGM_BOOTING_STATUS != 0) {
        resTypes.insert(RES_TYPE_RGM_BOOTING_STATUS);
    }
}

void SocPerfPlugin::Disable()
{
    functionMap.clear();
    for (auto resType : resTypes) {
        PluginMgr::GetInstance().UnSubscribeResource(LIB_NAME, resType);
    }
    resTypes.clear();
    if (handle_ != nullptr) {
        dlclose(handle_);
        handle_ = nullptr;
    }
    SOC_PERF_LOGI("SocPerfPlugin::Disable success");
}

void SocPerfPlugin::DispatchResource(const std::shared_ptr<ResData>& data)
{
    auto funcIter = functionMap.find(data->resType);
    if (funcIter != functionMap.end()) {
        auto function = funcIter->second;
        if (function) {
            function(data);
        }
    }
}

bool SocPerfPlugin::InitFeatureSwitch(std::string featureName)
{
    PluginConfig itemLists = PluginMgr::GetInstance().GetConfig(PLUGIN_NAME, CONFIG_NAME_SOCPERF_FEATURE_SWITCH);
    for (const Item& item : itemLists.itemList) {
        for (SubItem sub : item.subItemList) {
            if (sub.name == featureName) {
                return sub.value == "1";
            }
        }
    }
    return false;
}

static int32_t ParsePayload(const std::shared_ptr<ResData>& data, const std::string& key)
{
    if (!data->payload.contains(key)) {
        return INVALID_VALUE;
    }
    if (data->payload.at(key).is_string()) {
        return atoi(data->payload[key].get<std::string>().c_str());
    }
    if (data->payload.at(key).is_number_integer()) {
        return data->payload[key].get<int32_t>();
    }
    return INVALID_VALUE;
}

void SocPerfPlugin::HandleAppAbilityStart(const std::shared_ptr<ResData>& data)
{
    if (data->value == AppStartType::APP_COLD_START) {
        if (data->payload != nullptr && data->payload.contains(PRELOAD_MODE) &&
            atoi(data->payload[PRELOAD_MODE].get<std::string>().c_str()) == 1) {
            SOC_PERF_LOGI("SocPerfPlugin: socperf->APP_COLD_START is invalid as preload");
            return;
        }
        SOC_PERF_LOGI("SocPerfPlugin: socperf->APP_COLD_START");
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequest(PERF_REQUEST_CMD_ID_APP_START, "");
        int32_t appType = INVALID_VALUE;
        if (reqAppTypeFunc_ != nullptr && data->payload != nullptr && data->payload.contains(BUNDLE_NAME)) {
            std::string bundleName = data->payload[BUNDLE_NAME].get<std::string>().c_str();
            appType = reqAppTypeFunc_(bundleName);
            UpdateUidToAppMsgMap(data, appType, bundleName);
        }
        if (appType == APP_TYPE_GAME) {
            SOC_PERF_LOGI("SocPerfPlugin: socperf->Game cold start");
            OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequest(PERF_REQUEST_CMD_ID_GAME_START, "");
        }
    } else if (data->value == AppStartType::APP_WARM_START) {
        SOC_PERF_LOGI("SocPerfPlugin: socperf->APP_WARM_START");
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequest(PERF_REQUEST_CMD_ID_WARM_START, "");
    }
}

bool SocPerfPlugin::UpdateUidToAppMsgMap(const std::shared_ptr<ResData>& data, int32_t appType,
    const std::string& bundleName)
{
    int32_t uid = GetUidByData(data);
    if (appType != INVALID_VALUE && appType != INVALID_APP_TYPE && uid != INVALID_VALUE) {
        AppKeyMessage appMsg(appType, bundleName);
        uidToAppMsgMap_[uid] = appMsg;
        return true;
    }
    return false;
}

void SocPerfPlugin::HandleWindowFocus(const std::shared_ptr<ResData>& data)
{
    if (data->value == WindowFocusStatus::WINDOW_FOCUS) {
        SOC_PERF_LOGI("SocPerfPlugin: socperf->WINDOW_SWITCH focus");
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequest(PERF_REQUEST_CMD_ID_WINDOW_SWITCH, "");
        UpdateFocusAppType(data, true);
    } else if (data->value == WindowFocusStatus::WINDOW_UNFOCUS) {
        UpdateFocusAppType(data, false);
    }
}

void SocPerfPlugin::UpdateWeakActionStatus()
{
    bool status = true;
    for (const int32_t& appUid : focusAppUids_) {
        if (uidToAppMsgMap_.find(appUid) != uidToAppMsgMap_.end()) {
            if (keyAppName_.find(uidToAppMsgMap_[appUid].GetBundleName()) != keyAppName_.end()) {
                status = false;
                break;
            } else if (keyAppType_.find(uidToAppMsgMap_[appUid].GetAppType()) != keyAppType_.end()) {
                status = false;
                break;
            }
        }
    }
    if (status != weakActionStatus_) {
        weakActionStatus_ = status;
        OHOS::SOCPERF::SocPerfClient::GetInstance().RequestDeviceMode(WEAK_ACTION_MODE, weakActionStatus_);
    }
}

bool SocPerfPlugin::UpdateFocusAppType(const std::shared_ptr<ResData>& data, bool focusStatus)
{
    int32_t uid = GetUidByData(data);
    if (uid == INVALID_VALUE) {
        return false;
    }
    SOC_PERF_LOGI("SocPerfPlugin: socperf->UpdateFocusAppType, %{public}d", uid);
    if (!focusStatus) {
        focusAppUids_.erase(uid);
        UpdateWeakActionStatus();
        isFocusAppsGameType_ = IsFocusAppsAllGame();
        return true;
    }
    focusAppUids_.insert(uid);
    UpdateWeakActionStatus();
    int32_t pid = GetPidByData(data, PID_NAME);
    if (uidToAppMsgMap_.find(uid) != uidToAppMsgMap_.end()) {
        if (pid != INVALID_VALUE) {
            pidToAppTypeMap_[pid] = uidToAppMsgMap_[uid].GetAppType();
        }
        isFocusAppsGameType_ = UpdatesFocusAppsType(uidToAppMsgMap_[uid].GetAppType());
        return true;
    }
    if (reqAppTypeFunc_ == nullptr) {
        SOC_PERF_LOGD("SocPerfPlugin: socperf->WINDOW_SWITCH reqAppTypeFunc_ is null");
        return false;
    }
    std::string bundleName = GetBundleNameByUid(uid);
    int32_t focusAppType = reqAppTypeFunc_(bundleName);
    if (focusAppType != INVALID_VALUE && focusAppType != INVALID_APP_TYPE) {
        if (pid != INVALID_VALUE) {
            pidToAppTypeMap_[pid] = focusAppType;
        }
        AppKeyMessage appMsg(focusAppType, bundleName);
        uidToAppMsgMap_[uid] = appMsg;
    }
    isFocusAppsGameType_ = UpdatesFocusAppsType(focusAppType);
    return true;
}

bool SocPerfPlugin::IsFocusAppsAllGame()
{
    if (focusAppUids_.empty() || uidToAppMsgMap_.empty()) {
        unsigned long focusSize = static_cast<unsigned long>(focusAppUids_.size());
        unsigned long mapSize = static_cast<unsigned long>(uidToAppMsgMap_.size());
        SOC_PERF_LOGD("SocPerfPlugin: IsFoucsAppsAllGame data is empty, %{public}lu, %{public}lu",
            focusSize, mapSize);
        return false;
    }
    bool isAllGame = true;
    for (const int32_t& uid : focusAppUids_) {
        if (uidToAppMsgMap_.find(uid) == uidToAppMsgMap_.end() ||
            uidToAppMsgMap_[uid].GetAppType() != APP_TYPE_GAME) {
            isAllGame = false;
            break;
        }
    }
    SOC_PERF_LOGI("SocPerfPlugin: IsFoucsAppsAllGame is %{public}d", isAllGame);
    return isAllGame;
}

bool SocPerfPlugin::UpdatesFocusAppsType(int32_t appType)
{
    if (focusAppUids_.size() == 1) {
        return appType == APP_TYPE_GAME;
    }
    return isFocusAppsGameType_ && (appType == APP_TYPE_GAME);
}

std::string SocPerfPlugin::GetBundleNameByUid(const int32_t uid)
{
    std::string bundleName = "";
    OHOS::sptr<OHOS::ISystemAbilityManager> systemAbilityManager =
        OHOS::SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (systemAbilityManager == nullptr) {
        return bundleName;
    }
    OHOS::sptr<OHOS::IRemoteObject> object =
        systemAbilityManager->GetSystemAbility(OHOS::BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
    sptr<AppExecFwk::IBundleMgr> iBundleMgr = OHOS::iface_cast<OHOS::AppExecFwk::IBundleMgr>(object);
    if (!iBundleMgr) {
        SOC_PERF_LOGD("%{public}s null bundle manager.", __func__);
        return bundleName;
    }

    ErrCode ret = iBundleMgr->GetNameForUid(uid, bundleName);
    if (ret != ERR_OK) {
        SOC_PERF_LOGE("%{public}s get bundle name failed for %{public}d, err_code:%{public}d.", __func__, uid, ret);
    }
    return bundleName;
}

void SocPerfPlugin::HandleEventClick(const std::shared_ptr<ResData>& data)
{
    if (socperfGameBoostSwitch_ && (isFocusAppsGameType_ || IsGameEvent(data))) {
        SOC_PERF_LOGD("SocPerfPlugin: socperf->EVENT_CLICK game can not get click");
        return;
    }
    if (custGameState_) {
        SOC_PERF_LOGD("SocPerfPlugin: socperf->EVENT_CLICK cust game can not get click");
        return;
    }
    SOC_PERF_LOGD("SocPerfPlugin: socperf->EVENT_CLICK: %{public}lld", (long long)data->value);
    // touch down event
    if (data->value == ClickEventType::TOUCH_EVENT_DOWN_MMI) {
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequest(PERF_REQUEST_CMD_ID_EVENT_TOUCH_DOWN, "");
    } else if (data->value == ClickEventType::TOUCH_EVENT_DOWN) {
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequest(PERF_REQUEST_CMD_ID_EVENT_TOUCH_DOWN, "");
    } else if (data->value == ClickEventType::TOUCH_EVENT_UP) {
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequest(PERF_REQUEST_CMD_ID_EVENT_TOUCH_UP, "");
        HandleMoveEventBoost(data, false);
    } else if (data->value == ClickEventType::CLICK_EVENT) {
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequest(PERF_REQUEST_CMD_ID_EVENT_CLICK, "");
    }
}

bool SocPerfPlugin::HandleMoveEventBoost(const std::shared_ptr<ResData>& data, bool isSet)
{
    if (data->payload == nullptr || !data->payload.contains(CALLING_UID_NAME) ||
        !data->payload[CALLING_UID_NAME].is_string()) {
        SOC_PERF_LOGE("SocPerfPlugin: socperf->MOVE_EVENT param invalid");
        return false;
    }
    bool ret = false;
    int32_t uid = atoi(data->payload[CALLING_UID_NAME].get<std::string>().c_str());
    if (uidToAppMsgMap_.find(uid) != uidToAppMsgMap_.end() &&
        appNameMoveEvent_.find(uidToAppMsgMap_[uid].GetBundleName()) != appNameMoveEvent_.end()) {
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(PERF_REQUEST_CMD_ID_MOVE_EVENT_BOOST, isSet, "");
        ret = true;
    }
    return ret;
}

void SocPerfPlugin::HandleEventKey(const std::shared_ptr<ResData>& data)
{
    if (socperfGameBoostSwitch_ && isFocusAppsGameType_) {
        SOC_PERF_LOGD("SocPerfPlugin: socperf->EVENT_KEY game can not get key_event");
        return;
    }
    SOC_PERF_LOGD("SocPerfPlugin: socperf->EVENT_KEY: %{public}lld", (long long)data->value);
    // key down event
    if (data->value == KeyEventType::KEY_EVENT_DOWN) {
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequest(PERF_REQUEST_CMD_ID_EVENT_KEY_DOWN, "");
    } else if (data->value == KeyEventType::KEY_EVENT_UP) {
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequest(PERF_REQUEST_CMD_ID_EVENT_KEY_DOWN, "");
    }
}

#ifdef RESSCHED_RESOURCESCHEDULE_CROWN_POWER_KEY_ENABLE
void SocPerfPlugin::HandlePowerEventKey(const std::shared_ptr<ResData>& data)
{
    if (data == nullptr) {
        SOC_PERF_LOGD("SocPerfPlugin: socperf->POWER_KEY null data");
        return;
    }
    SOC_PERF_LOGD("SocPerfPlugin:socperf->POWER_KEY: %{public}lld", (long long)data->value);
    OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequest(PERF_REQUEST_CMD_ID_POWER_KEY, "powerkey");
}

void SocPerfPlugin::HandleCrownRotation(const std::shared_ptr<ResData>& data)
{
    if (data == nullptr) {
        SOC_PERF_LOGD("SocPerfPlugin: socperf->CROWN_ROTATION_STATUS null data");
        return;
    }
    SOC_PERF_LOGD("SocPerfPlugin:socperf->CROWN_ROTATION_STATUS: %{public}lld", (long long)data->value);
    if (data->value == CrownRotationStatus::CROWN_ROTATION_START) {
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(PERF_REQUEST_CMD_ID_CROWN_ROTATION, true, "");
    } else if (data->value == CrownRotationStatus::CROWN_ROTATION_END) {
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(PERF_REQUEST_CMD_ID_CROWN_ROTATION, false, "");
    }
}
#endif // RESSCHED_RESOURCESCHEDULE_CROWN_POWER_KEY_ENABLE

bool SocPerfPlugin::HandleGameBoost(const std::shared_ptr<ResData>& data)
{
    if (!socperfGameBoostSwitch_ || data == nullptr) {
        SOC_PERF_LOGD("SocPerfPlugin: socperf->GAME_BOOST null data");
        return false;
    }
    SOC_PERF_LOGD("SocPerfPlugin:socperf->GAME_BOOST: %{public}lld", (long long)data->value);
    if (data->value == GameBoostState::GAME_BOOST_LEVEL0) {
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(PERF_REQUEST_CMD_ID_GAME_BOOST, true, "");
    } else if (data->value == GameBoostState::GAME_BOOST_LEVEL1) {
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(PERF_REQUEST_CMD_ID_GAME_BOOST, true, "");
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(PERF_REQUEST_CMD_ID_GAME_BOOST_LEVEL1, true, "");
    } else if (data->value == GameBoostState::GAME_BOOST_LEVEL2) {
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(PERF_REQUEST_CMD_ID_GAME_BOOST, true, "");
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(PERF_REQUEST_CMD_ID_GAME_BOOST_LEVEL2, true, "");
    } else if (data->value == GameBoostState::GAME_BOOST_LEVEL3) {
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(PERF_REQUEST_CMD_ID_GAME_BOOST, true, "");
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(PERF_REQUEST_CMD_ID_GAME_BOOST_LEVEL3, true, "");
    }
    return true;
}

bool SocPerfPlugin::IsGameEvent(const std::shared_ptr<ResData>& data)
{
    int32_t pid = GetPidByData(data, CLIENT_PID_NAME);
    if (pid == INVALID_VALUE) {
        SOC_PERF_LOGD("SocPerfPlugin:socperf->IsGameEvent: %{public}d", pid);
        return false;
    }
    return pidToAppTypeMap_[pid] == APP_TYPE_GAME;
}

bool SocPerfPlugin::HandleUninstallEvent(const std::shared_ptr<ResData>& data)
{
    if (data == nullptr) {
        SOC_PERF_LOGD("SocPerfPlugin: socperf->UNINSTALL null data");
        return false;
    }
    if (data->value != AppInstallStatus::APP_UNINSTALL) {
        return false;
    }
    int32_t uid = INVALID_VALUE;
    if (data->payload == nullptr || !data->payload.contains(UID_NAME) ||
        !data->payload.at(UID_NAME).is_number_integer()) {
        return false;
    }
    uid = data->payload[UID_NAME].get<std::int32_t>();
    if (uid == INVALID_VALUE) {
        return false;
    }
    SOC_PERF_LOGI("SocPerfPlugin: socperf->UNINSTALL:%{public}d", uid);
    uidToAppMsgMap_.erase(uid);
    return true;
}

int32_t SocPerfPlugin::GetUidByData(const std::shared_ptr<ResData>& data)
{
    int32_t uid = INVALID_VALUE;
    if (data->payload == nullptr || !data->payload.contains(UID_NAME) || !data->payload.at(UID_NAME).is_string()) {
        return uid;
    }
    uid = atoi(data->payload[UID_NAME].get<std::string>().c_str());
    return uid;
}

void SocPerfPlugin::HandleLoadPage(const std::shared_ptr<ResData>& data)
{
    if (data->value == LOAD_PAGE_START) {
        SOC_PERF_LOGI("SocPerfPlugin: socperf->PUSH_PAGE_START");
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(PERF_REQUEST_CMD_ID_LOAD_PAGE_START, true, "");
    } else if (data->value == LOAD_PAGE_COMPLETE) {
        SOC_PERF_LOGI("SocPerfPlugin: socperf->PUSH_PAGE_COMPLETE");
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(PERF_REQUEST_CMD_ID_LOAD_PAGE_START, false, "");
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequest(PERF_REQUEST_CMD_ID_LOAD_PAGE_COMPLETE, "");
    }
}

void SocPerfPlugin::HandlePopPage(const std::shared_ptr<ResData>& data)
{
    SOC_PERF_LOGI("SocPerfPlugin: socperf->POP_PAGE: %{public}lld", (long long)data->value);
    OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequest(PERF_REQUEST_CMD_ID_POP_PAGE, "");
}

void SocPerfPlugin::HandleEventSlide(const std::shared_ptr<ResData>& data)
{
    if (socperfGameBoostSwitch_ && (isFocusAppsGameType_ || IsGameEvent(data))) {
        SOC_PERF_LOGD("SocPerfPlugin: socperf->EVENT_SLIDE game can not get slide");
        return;
    }
    if (custGameState_) {
        SOC_PERF_LOGD("SocPerfPlugin: socperf->EVENT_SLIDE cust game can not get slide");
        return;
    }
    SOC_PERF_LOGD("SocPerfPlugin: socperf->SLIDE_NORMAL: %{public}lld", (long long)data->value);
    if (data->value == SlideEventStatus::SLIDE_EVENT_ON) {
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(PERF_REQUEST_CMD_ID_EVENT_FLING, true, "");
    } else if (data->value == SlideEventStatus::SLIDE_EVENT_OFF) {
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(PERF_REQUEST_CMD_ID_EVENT_FLING, false, "");
    } else if (data->value == SlideEventStatus::SLIDE_NORMAL_BEGIN) {
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(PERF_REQUEST_CMD_ID_EVENT_DRAG, true, "");
    } else if (data->value == SlideEventStatus::SLIDE_NORMAL_END) {
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(PERF_REQUEST_CMD_ID_EVENT_DRAG, false, "");
    } else if (data->value == SlideEventStatus::MOVE_EVENT_ON) {
        HandleMoveEventBoost(data, true);
    }
}

void SocPerfPlugin::HandleEventWebGesture(const std::shared_ptr<ResData>& data)
{
    if (data == nullptr) {
        return;
    }
    SOC_PERF_LOGD("SocPerfPlugin: socperf->WEB_GESTURE: %{public}lld", (long long)data->value);
    if (data->value == WebGesture::WEB_GESTURE_START) {
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(PERF_REQUEST_CMD_ID_EVENT_WEB_GESTURE, true, "");
    } else if (data->value == WebGesture::WEB_GESTURE_END) {
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(PERF_REQUEST_CMD_ID_EVENT_WEB_GESTURE, false, "");
    }
}

void SocPerfPlugin::HandleResizeWindow(const std::shared_ptr<ResData>& data)
{
    if (data == nullptr) {
        return;
    }
    SOC_PERF_LOGI("SocPerfPlugin: socperf->RESIZE_WINDOW: %{public}lld", (long long)data->value);
    if (data->value == WindowResizeType::WINDOW_RESIZING) {
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequest(PERF_REQUEST_CMD_ID_RESIZE_WINDOW, "");
    } else if (data->value == WindowResizeType::WINDOW_RESIZE_STOP) {
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(PERF_REQUEST_CMD_ID_RESIZE_WINDOW, false, "");
    }
}

void SocPerfPlugin::HandleMoveWindow(const std::shared_ptr<ResData>& data)
{
    if (data == nullptr) {
        return;
    }
    SOC_PERF_LOGI("SocPerfPlugin: socperf->MOVE_WINDOW: %{public}lld", (long long)data->value);
    if (data->value == WindowMoveType::WINDOW_MOVING) {
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequest(PERF_REQUEST_CMD_ID_MOVE_WINDOW, "");
    } else if (data->value == WindowMoveType::WINDOW_MOVE_STOP) {
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(PERF_REQUEST_CMD_ID_MOVE_WINDOW, false, "");
    }
}

void SocPerfPlugin::HandleRemoteAnimation(const std::shared_ptr<ResData>& data)
{
    SOC_PERF_LOGI("SocPerfPlugin: socperf->REMOTE_ANIMATION: %{public}lld", (long long)data->value);
    if (data->value == ShowRemoteAnimationStatus::ANIMATION_BEGIN) {
        SOC_PERF_LOGI("SocPerfPlugin: socperf->REMOTE_ANIMATION: %{public}lld", (long long)data->value);
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(PERF_REQUEST_CMD_ID_REMOTE_ANIMATION, true, "");
    } else if (data->value == ShowRemoteAnimationStatus::ANIMATION_END) {
        SOC_PERF_LOGI("SocPerfPlugin: socperf->REMOTE_ANIMATION: %{public}lld", (long long)data->value);
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(PERF_REQUEST_CMD_ID_REMOTE_ANIMATION, false, "");
    } else if (data->value == ShowRemoteAnimationStatus::ANIMATION_UNLOCK_BEGIN) {
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(PERF_REQUEST_CMD_ID_REMOTE_UNLOCK, true, "");
    } else if (data->value == ShowRemoteAnimationStatus::ANIMATION_UNLOCK_END) {
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(PERF_REQUEST_CMD_ID_REMOTE_UNLOCK, false, "");
    }
}

void SocPerfPlugin::HandleDragStatusBar(const std::shared_ptr<ResData>& data)
{
    if (data == nullptr) {
        return;
    }
    SOC_PERF_LOGI("SocPerfPlugin: socperf->DRAG_STATUS_BAR: %{public}lld", (long long)data->value);
    if (data->value == StatusBarDragStatus::DRAG_START) {
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequest(PERF_REQUEST_CMD_ID_DRAG_STATUS_BAR, "");
    } else if (data->value == StatusBarDragStatus::DRAG_END) {
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(PERF_REQUEST_CMD_ID_DRAG_STATUS_BAR, false, "");
    }
}

void SocPerfPlugin::HandleWebGestureMove(const std::shared_ptr<ResData>& data)
{
    if (data == nullptr) {
        return;
    }
    SOC_PERF_LOGI("SocPerfPlugin: socperf->WEB_GESTURE_MOVE: %{public}lld", (long long)data->value);
    if (data->value == WebGestureMove::WEB_GESTURE_MOVE_START) {
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(PERF_REQUEST_CMD_ID_WEB_GESTURE_MOVE, true, "");
    } else if (data->value == WebGestureMove::WEB_GESTURE_MOVE_END) {
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(PERF_REQUEST_CMD_ID_WEB_GESTURE_MOVE, false, "");
    }
}

void SocPerfPlugin::HandleWebSlideNormal(const std::shared_ptr<ResData>& data)
{
    if (data == nullptr) {
        return;
    }
    SOC_PERF_LOGD("SocPerfPlugin: socperf->WEB_SLIDE_NORMAL: %{public}lld", (long long)data->value);
    if (data->value == WebSlideNormal::WEB_SLIDE_NORMAL_START) {
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(PERF_REQUEST_CMD_ID_WEB_SLIDE_NORMAL, true, "");
    } else if (data->value == WebSlideNormal::WEB_SLIDE_NORMAL_END) {
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(PERF_REQUEST_CMD_ID_WEB_SLIDE_NORMAL, false, "");
    }
}

void SocPerfPlugin::HandleLoadUrl(const std::shared_ptr<ResData>& data)
{
    SOC_PERF_LOGI("SocPerfPlugin: socperf->LOAD_URL");
    OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequest(PERF_REQUEST_CMD_ID_LOAD_URL, "");
}

void SocPerfPlugin::HandleMousewheel(const std::shared_ptr<ResData>& data)
{
    SOC_PERF_LOGI("SocPerfPlugin: socperf->MOUSEWHEEL");
    OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequest(PERF_REQUEST_CMD_ID_MOUSEWHEEL, "");
}

bool SocPerfPlugin::HandleAppStateChange(const std::shared_ptr<ResData>& data)
{
    if (data->value != ResType::ProcessStatus::PROCESS_CREATED) {
        return false;
    }
    int extensionType = ParsePayload(data, EXTENSION_TYPE_KEY);
    if (std::find(ResType::UI_SENSITIVE_EXTENSION.begin(), ResType::UI_SENSITIVE_EXTENSION.end(), extensionType) !=
        ResType::UI_SENSITIVE_EXTENSION.end()) {
        if (data->payload != nullptr && data->payload.contains(PRELOAD_MODE) &&
            atoi(data->payload[PRELOAD_MODE].get<std::string>().c_str()) == 1) {
            SOC_PERF_LOGI("SocPerfPlugin: socperf->APPSTATECHANGE is invalid as preload");
            return false;
        }
        SOC_PERF_LOGI("SocPerfPlugin: socperf->APPSTATECHANGE");
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequest(PERF_REQUEST_CMD_ID_APP_START, "");
        UpdateUidToAppMsgMap(data);
        return true;
    }
    return false;
}

bool SocPerfPlugin::UpdateUidToAppMsgMap(const std::shared_ptr<ResData>& data)
{
    int32_t uid = GetUidByData(data);
    if (uid == INVALID_VALUE) {
        return false;
    }
    if (uidToAppMsgMap_.find(uid) != uidToAppMsgMap_.end()) {
        return true;
    }
    if (reqAppTypeFunc_ == nullptr || !data->payload.contains(BUNDLE_NAME)) {
        return false;
    }
    std::string bundleName = data->payload[BUNDLE_NAME].get<std::string>().c_str();
    int32_t appType = reqAppTypeFunc_(bundleName);
    if (appType != INVALID_VALUE && appType != INVALID_APP_TYPE) {
        AppKeyMessage appMsg(appType, bundleName);
        uidToAppMsgMap_[uid] = appMsg;
    }
    return true;
}

void SocPerfPlugin::HandleScreenOn()
{
    OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(PERF_REQUEST_CMD_ID_SCREEN_OFF, false, "");
    OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(PERF_REQUEST_CMD_ID_SCREEN_ON, true, "");
    if (deviceMode_ == DISPLAY_MODE_FULL) {
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(PERF_REQUEST_CMD_ID_DISPLAY_MODE_FULL, true, "");
    } else if (deviceMode_ == DISPLAY_MODE_MAIN) {
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(PERF_REQUEST_CMD_ID_DISPLAY_MODE_MAIN, true, "");
    }  else if (deviceMode_ == DISPLAY_MODE_GLOBAL_FULL && deviceOrientation_ == DISPLAY_ORIENTAYION_LANDSCAPE) {
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(PERF_REQUEST_CMD_ID_DISPLAY_MODE_MAIN, false, "");
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(PERF_REQUEST_CMD_ID_DISPLAY_MODE_FULL, false, "");
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(PERF_REQUEST_CMD_ID_DISPLAY_GLOBAL_P, false, "");
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(PERF_REQUEST_CMD_ID_DISPLAY_GLOBAL_L, true, "");
    } else if (deviceMode_ == DISPLAY_MODE_GLOBAL_FULL && deviceOrientation_ == DISPLAY_ORIENTAYION_PORTRAIT) {
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(PERF_REQUEST_CMD_ID_DISPLAY_MODE_MAIN, false, "");
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(PERF_REQUEST_CMD_ID_DISPLAY_MODE_FULL, false, "");
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(PERF_REQUEST_CMD_ID_DISPLAY_GLOBAL_L, false, "");
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(PERF_REQUEST_CMD_ID_DISPLAY_GLOBAL_P, true, "");
    }
    const std::string screenModeOnStr = SCREEN_MODE_KEY + ":" + SCREEN_MODE_ON;
    OHOS::SOCPERF::SocPerfClient::GetInstance().RequestDeviceMode(screenModeOnStr, true);
}

void SocPerfPlugin::HandleScreenOff()
{
    std::lock_guard<ffrt::mutex> xmlLock(screenMutex_);
    if (screenStatus_ == SCREEN_OFF) {
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(PERF_REQUEST_CMD_ID_SCREEN_ON, false, "");
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(PERF_REQUEST_CMD_ID_SCREEN_OFF, true, "");
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(PERF_REQUEST_CMD_ID_DISPLAY_MODE_FULL, false, "");
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(PERF_REQUEST_CMD_ID_DISPLAY_MODE_MAIN, false, "");
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(PERF_REQUEST_CMD_ID_DISPLAY_GLOBAL_P, false, "");
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(PERF_REQUEST_CMD_ID_DISPLAY_GLOBAL_L, false, "");
    }
    const std::string screenModeOffStr = SCREEN_MODE_KEY + ":" + SCREEN_MODE_OFF;
    OHOS::SOCPERF::SocPerfClient::GetInstance().RequestDeviceMode(screenModeOffStr, true);
}

bool SocPerfPlugin::HandleScreenStatusAnalysis(const std::shared_ptr<ResData>& data)
{
    if (data == nullptr) {
        return false;
    }
    std::lock_guard<ffrt::mutex> xmlLock(screenMutex_);
    screenStatus_ = data->value;
    SOC_PERF_LOGI("SocPerfPlugin: socperf->HandleScreenStatusAnalysis: %{public}lld", (long long)screenStatus_);
    if (screenStatus_ == SCREEN_ON) {
        HandleScreenOn();
    } else if (screenStatus_ == SCREEN_OFF) {
        //post screen off task with 5000 milliseconds delay, to avoid frequent screen status change.
        std::function<void()> screenOffFunc = [this]() {
            HandleScreenOff();
        };
        ffrt::submit(screenOffFunc, {}, {}, ffrt::task_attr().delay(SCREEN_OFF_TIME_DELAY));
    }
    return true;
}

void SocPerfPlugin::HandleDeviceModeStatusChange(const std::shared_ptr<ResData>& data)
{
    if ((data->value != DeviceModeStatus::MODE_ENTER) && (data->value != DeviceModeStatus::MODE_QUIT)) {
        SOC_PERF_LOGW("SocPerfPlugin: device mode status value is error");
        return;
    }

    if (!data->payload.contains(DEVICE_MODE_TYPE_KEY) || !data->payload[DEVICE_MODE_TYPE_KEY].is_string() ||
        !data->payload.contains(DEVICE_MODE_PAYMODE_NAME) || !data->payload[DEVICE_MODE_PAYMODE_NAME].is_string()) {
        SOC_PERF_LOGW("SocPerfPlugin: device mode status payload is error");
        return;
    }
    deviceMode_ = data->payload[DEVICE_MODE_PAYMODE_NAME];
    bool status = (data->value == DeviceModeStatus::MODE_ENTER);
    const std::string deviceModeType = data->payload[DEVICE_MODE_TYPE_KEY];
    const std::string deviceModeStr = deviceModeType + ":" + deviceMode_;
    OHOS::SOCPERF::SocPerfClient::GetInstance().RequestDeviceMode(deviceModeStr, status);
    SOC_PERF_LOGI("SocPerfPlugin: device mode %{public}s  status%{public}d", deviceModeStr.c_str(), status);
    HandleSceenModeBoost(deviceModeType);
}

bool SocPerfPlugin::HandleSceenModeBoost(const std::string& deviceModeType)
{
    if (deviceModeType != DISPLAY_MODE_KEY) {
        return false;
    }

    std::lock_guard<ffrt::mutex> xmlLock(screenMutex_);
    if (deviceMode_ == DISPLAY_MODE_FULL && screenStatus_ == SCREEN_ON) {
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(PERF_REQUEST_CMD_ID_DISPLAY_MODE_MAIN, false, "");
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(PERF_REQUEST_CMD_ID_DISPLAY_GLOBAL_L, false, "");
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(PERF_REQUEST_CMD_ID_DISPLAY_GLOBAL_P, false, "");
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(PERF_REQUEST_CMD_ID_DISPLAY_MODE_FULL, true, "");
    } else if (deviceMode_ == DISPLAY_MODE_MAIN && screenStatus_ == SCREEN_ON) {
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(PERF_REQUEST_CMD_ID_DISPLAY_MODE_FULL, false, "");
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(PERF_REQUEST_CMD_ID_DISPLAY_GLOBAL_L, false, "");
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(PERF_REQUEST_CMD_ID_DISPLAY_GLOBAL_P, false, "");
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(PERF_REQUEST_CMD_ID_DISPLAY_MODE_MAIN, true, "");
    } else if (deviceMode_ == DISPLAY_MODE_GLOBAL_FULL && screenStatus_ == SCREEN_ON &&
        deviceOrientation_ == DISPLAY_ORIENTAYION_LANDSCAPE) {
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(PERF_REQUEST_CMD_ID_DISPLAY_MODE_MAIN, false, "");
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(PERF_REQUEST_CMD_ID_DISPLAY_MODE_FULL, false, "");
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(PERF_REQUEST_CMD_ID_DISPLAY_GLOBAL_P, false, "");
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(PERF_REQUEST_CMD_ID_DISPLAY_GLOBAL_L, true, "");
    } else if (deviceMode_ == DISPLAY_MODE_GLOBAL_FULL && screenStatus_ == SCREEN_ON &&
        deviceOrientation_ == DISPLAY_ORIENTAYION_PORTRAIT) {
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(PERF_REQUEST_CMD_ID_DISPLAY_MODE_MAIN, false, "");
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(PERF_REQUEST_CMD_ID_DISPLAY_MODE_FULL, false, "");
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(PERF_REQUEST_CMD_ID_DISPLAY_GLOBAL_L, false, "");
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(PERF_REQUEST_CMD_ID_DISPLAY_GLOBAL_P, true, "");
    }

    if (deviceMode_ == DISPLAY_MODE_FULL) {
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(PERF_REQUEST_CMD_ID_SCREEN_SWITCHED, true, "");
    }
    return true;
}

void SocPerfPlugin::HandleDeviceOrientationStatusChange(const std::shared_ptr<ResData>& data)
{
    if ((data->value != DeviceModeStatus::MODE_ENTER) && (data->value != DeviceModeStatus::MODE_QUIT)) {
        SOC_PERF_LOGW("SocPerfPlugin: device mode status value is error");
        return;
    }

    if (!data->payload.contains(DEVICE_MODE_TYPE_KEY) || !data->payload[DEVICE_MODE_TYPE_KEY].is_string() ||
        !data->payload.contains(DEVICE_MODE_PAYMODE_NAME) || !data->payload[DEVICE_MODE_PAYMODE_NAME].is_string()) {
        SOC_PERF_LOGW("SocPerfPlugin: device mode status payload is error");
        return;
    }
    deviceOrientation_ = data->payload[DEVICE_MODE_PAYMODE_NAME];
    const std::string deviceOrientationType = data->payload[DEVICE_MODE_TYPE_KEY];
    HandleSceenOrientationBoost(deviceOrientationType);
}

bool SocPerfPlugin::HandleSceenOrientationBoost(const std::string& deviceOrientationType)
{
    if (deviceOrientationType != DEVICE_ORIENTATION_TYPE_KEY) {
        return false;
    }

    if (deviceMode_ != DISPLAY_MODE_GLOBAL_FULL) {
        return false;
    }
    std::lock_guard<ffrt::mutex> xmlLock(screenMutex_);
    if (screenStatus_ == SCREEN_ON && deviceOrientation_ == DISPLAY_ORIENTAYION_LANDSCAPE) {
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(PERF_REQUEST_CMD_ID_DISPLAY_MODE_MAIN, false, "");
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(PERF_REQUEST_CMD_ID_DISPLAY_MODE_FULL, false, "");
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(PERF_REQUEST_CMD_ID_DISPLAY_GLOBAL_P, false, "");
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(PERF_REQUEST_CMD_ID_DISPLAY_GLOBAL_L, true, "");
    } else if (screenStatus_ == SCREEN_ON && deviceOrientation_ == DISPLAY_ORIENTAYION_PORTRAIT) {
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(PERF_REQUEST_CMD_ID_DISPLAY_MODE_FULL, false, "");
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(PERF_REQUEST_CMD_ID_DISPLAY_MODE_MAIN, false, "");
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(PERF_REQUEST_CMD_ID_DISPLAY_GLOBAL_L, false, "");
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(PERF_REQUEST_CMD_ID_DISPLAY_GLOBAL_P, true, "");
    }

    return true;
}

bool SocPerfPlugin::HandlePowerModeChanged(const std::shared_ptr<ResData> &data)
{
    if (data == nullptr) {
        return false;
    }
    SOC_PERF_LOGI("SocPerfPlugin: socperf->HandlePowerModeChanged: %{public}lld", (long long)data->value);
    const std::string powerModeStr = POWER_MODE_KEY + ":" + POWER_MODE;
    if (data->value == POWERMODE_ON) {
        OHOS::SOCPERF::SocPerfClient::GetInstance().RequestDeviceMode(powerModeStr, true);
    } else {
        OHOS::SOCPERF::SocPerfClient::GetInstance().RequestDeviceMode(powerModeStr, false);
    }
    return true;
}

void SocPerfPlugin::HandleWebDragResize(const std::shared_ptr<ResData>& data)
{
    if (data == nullptr) {
        return;
    }
    SOC_PERF_LOGI("SocPerfPlugin: socperf->WEB_DRAG_RESIZE: %{public}lld", (long long)data->value);
    if (data->value == WebDragResizeStatus::WEB_DRAG_START) {
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequest(PERF_REQUEST_CMD_ID_WEB_DRAG_RESIZE, "");
    } else if (data->value == WebDragResizeStatus::WEB_DRAG_END) {
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(PERF_REQUEST_CMD_ID_WEB_DRAG_RESIZE, false, "");
    }
}

bool SocPerfPlugin::HandleCameraStateChange(const std::shared_ptr<ResData>& data)
{
    if (data == nullptr) {
        return false;
    }
    SOC_PERF_LOGI("SocPerfPlugin: socperf->CAMERA_STATE_CHANGE: %{public}lld", (long long)data->value);
    if (data->value == CameraState::CAMERA_CONNECT) {
        if (IsAllowBoostScene()) {
            OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(PERF_REQUEST_CMD_ID_APP_USE_CAMERA, true, "");
        }
    } else if (data->value == CameraState::CAMERA_DISCONNECT) {
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(PERF_REQUEST_CMD_ID_APP_USE_CAMERA, false, "");
    }
    return true;
}

bool SocPerfPlugin::HandleGameStateChange(const std::shared_ptr<ResData>& data)
{
    bool ret = false;
    if (data == nullptr || data->payload == nullptr) {
        return ret;
    }
    if (!data->payload.contains(UID_NAME) || !data->payload.at(UID_NAME).is_number_integer() ||
        !data->payload.contains(GAME_ENV) || !data->payload.at(GAME_ENV).is_number_integer()) {
        SOC_PERF_LOGD("SocPerfPlugin: socperf->HandleGameStateChange invalid data");
        return ret;
    }
    if (data->payload[GAME_ENV] == GameEnvType::GAME_CUST) {
        SOC_PERF_LOGI("SocPerfPlugin: socperf->HandleGameStateChange: %{public}lld", (long long)data->value);
        custGameState_ = UpdateCustGameState(data);
    }
    return true;
}

bool SocPerfPlugin::UpdateCustGameState(const std::shared_ptr<ResData>& data)
{
    int32_t uid = data->payload[UID_NAME].get<std::int32_t>();
    SOC_PERF_LOGI("SocPerfPlugin: socperf->UpdateCustGameState: %{public}d", uid);
    if (data->value == GameState::GAME_GEE_FOCUS_STATE) {
        focusCustGameUids_.insert(uid);
        custGameState_ = true;
    } else if (data->value == GameState::GAME_LOST_FOCUS_STATE) {
        focusCustGameUids_.erase(uid);
        if (focusCustGameUids_.size() == 0) {
            custGameState_ = false;
        }
    }
    return custGameState_;
}

bool SocPerfPlugin::IsAllowBoostScene()
{
    bool ret = false;
    if (focusAppUids_.empty()) {
        return ret;
    }

    for (const int32_t& uid : focusAppUids_) {
        if (uidToAppMsgMap_.find(uid) != uidToAppMsgMap_.end() &&
            appNameUseCamera_.find(uidToAppMsgMap_[uid].GetBundleName()) != appNameUseCamera_.end()) {
            ret = true;
            break;
        }
    }
    return ret;
}

bool SocPerfPlugin::HandleSocperfSceneBoard(const std::shared_ptr<ResData> &data)
{
    if (data == nullptr) {
        return false;
    }
    SOC_PERF_LOGD("SocPerfPlugin: socperf->ANIMATION: %{public}lld", (long long)data->value);
    if (data->value == ShowRemoteAnimationStatus::ANIMATION_BEGIN) {
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(PERF_REQUEST_CMD_ID_REMOTE_ANIMATION, true, "");
    } else if (data->value == ShowRemoteAnimationStatus::ANIMATION_END) {
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(PERF_REQUEST_CMD_ID_REMOTE_ANIMATION, false, "");
    }
    return true;
}

bool SocPerfPlugin::HandleProcessStateChange(const std::shared_ptr<ResData> &data)
{
    if (data != nullptr && data->value == ResType::ProcessStatus::PROCESS_DIED) {
        HandleDeadProcess(data);
    }
    return true;
}

bool SocPerfPlugin::HandleDeadProcess(const std::shared_ptr<ResData>& data)
{
    int32_t pid = GetPidByData(data, PID_NAME);
    if (pid == INVALID_VALUE) {
        return false;
    }
    pidToAppTypeMap_.erase(pid);
    return true;
}
 
int32_t SocPerfPlugin::GetPidByData(const std::shared_ptr<ResData>& data, const std::string& key)
{
    if (data->payload == nullptr || !data->payload.contains(key) ||
        !data->payload.at(key).is_string()) {
        SOC_PERF_LOGD("SocPerfPlugin: socperf->HandleDeadProcess invalid data");
        return INVALID_VALUE;
    }
    return atoi(data->payload[key].get<std::string>().c_str());
}

bool SocPerfPlugin::HandleSocperfAccountActivating(const std::shared_ptr<ResData> &data)
{
    if (data == nullptr) {
        return false;
    }
    SOC_PERF_LOGD("SocPerfPlugin: socperf->AccountActivating: %{public}lld", (long long)data->value);
    if (data->value == AccountActivatingStatus::ACCOUNT_ACTIVATING_START) {
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(PERF_REQUEST_CMD_ID_ACCOUNT_ACTIVATING, true, "");
    }
    return true;
}

#ifdef RESSCHED_RESOURCESCHEDULE_CUST_SOC_PERF_ENABLE
bool SocPerfPlugin::HandleCustEvent(const std::shared_ptr<ResData> &data)
{
    if (data == nullptr || data->value <= 0) {
        return false;
    }
    SOC_PERF_LOGD("SocPerfPlugin: socperf->Anco: %{public}lld", (long long)data->value);
    OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequest(data->value, "");
    return true;
}

bool SocPerfPlugin::HandleCustEventBegin(const std::shared_ptr<ResData> &data)
{
    if (data == nullptr || data->value <= 0) {
        return false;
    }
    SOC_PERF_LOGD("SocPerfPlugin: socperf->SOCPERF_CUST_EVENT_BEGIN: %{public}lld", (long long)data->value);
    OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(data->value, true, "");
    return true;
}

bool SocPerfPlugin::HandleCustEventEnd(const std::shared_ptr<ResData> &data)
{
    if (data == nullptr || data->value <= 0) {
        return false;
    }
    SOC_PERF_LOGD("SocPerfPlugin: socperf->SOCPERF_CUST_EVENT_END: %{public}lld", (long long)data->value);
    OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(data->value, false, "");
    return true;
}

bool SocPerfPlugin::HandleCustAction(const std::shared_ptr<ResData> &data)
{
    if (data == nullptr || data->value <= 0 || custGameState_) {
        SOC_PERF_LOGD("SocPerfPlugin: socperf->HandleCustAction error, %{public}d", custGameState_);
        return false;
    }
    SOC_PERF_LOGD("SocPerfPlugin: socperf->HandleCustAction: %{public}lld", (long long)data->value);
    OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequest(data->value, "");
    return true;
}
#endif // RESSCHED_RESOURCESCHEDULE_CUST_SOC_PERF_ENABLE

bool SocPerfPlugin::HandleRgmBootingStatus(const std::shared_ptr<ResData> &data)
{
    if (data == nullptr) {
        return false;
    }
    SOC_PERF_LOGD("SocPerfPlugin: socperf->RGM_BOOTING_STATUS: %{public}lld", (long long)data->value);
    if (data->value == 0) {
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(PERF_REQUEST_CMD_ID_RGM_BOOTING_START, true, "");
    }
    return true;
}

bool SocPerfPlugin::HandleAppColdStartEx(const std::shared_ptr<ResData> &data)
{
    if (data == nullptr) {
        return false;
    }
    SOC_PERF_LOGD("SocPerfPlugin: socperf->APP_START_ONLY_PERF: %{public}lld", (long long)data->value);
    if (data->value == 0) {
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(PERF_REQUEST_CMD_ID_APP_START, true, "");
    } else if (data->value == 1) {
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(PERF_REQUEST_CMD_ID_APP_START, false, "");
    }
    return true;
}

bool SocPerfPlugin::HandleSceneRotation(const std::shared_ptr<ResData> &data)
{
    if (data == nullptr) {
        return false;
    }
    SOC_PERF_LOGD("SocPerfPlugin: socperf->PERF_REQUEST_CMD_ID_ROTATION: %{public}lld", (long long)data->value);
    if (data->value == 0) {
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(PERF_REQUEST_CMD_ID_ROTATION, true, "");
    } else if (data->value == 1) {
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(PERF_REQUEST_CMD_ID_ROTATION, false, "");
    }
    return true;
}

bool SocPerfPlugin::HandleBmmMoniterStatus(const std::shared_ptr<ResData> &data)
{
    if (data == nullptr) {
        return false;
    }
    SOC_PERF_LOGD("SocPerfPlugin: socperf->PERF_REQUEST_CMD_ID_BMM_MONITER_CHANGE: %{public}lld",
        (long long)data->value);
    if (data->value == BmmMoniterStatus::BMM_BACKGROUND) {
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(PERF_REQUEST_CMD_ID_BMM_MONITER_START, true, "");
        return true;
    }
    if (data->value == BmmMoniterStatus::BMM_CLOSE) {
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(PERF_REQUEST_CMD_ID_BMM_MONITER_START, false, "");
        return true;
    }
    return false;
}

#ifdef RESSCHED_RESOURCESCHEDULE_FILE_COPY_SOC_PERF_ENABLE
bool SocPerfPlugin::HandleFileCopyStatus(const std::shared_ptr<ResData> &data)
{
    if (data == nullptr) {
        return false;
    }
    SOC_PERF_LOGD("SocPerfPlugin: socperf->COPY_STATUS: %{public}lld", (long long)data->value);
    if (data->value == CopyStatus::COPY_START) {
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(PERF_REQUEST_CMD_ID_FILE_COPY, true, "");
        return true;
    }
    if (data->value == CopyStatus::COPY_STOP) {
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(PERF_REQUEST_CMD_ID_FILE_COPY, false, "");
        return true;
    }
    return false;
}
#endif

void SocPerfPlugin::HandleWebSildeScroll(const std::shared_ptr<ResData>& data)
{
    if (data == nullptr) {
        return;
    }
    SOC_PERF_LOGD("SocPerfPlugin: socperf->WEB_SLIDE_SCROLL: %{public}lld", (long long)data->value);
    if (data->value == WebDragResizeStatus::WEB_DRAG_START) {
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(PERF_REQUEST_CMD_ID_WEB_SLIDE_SCROLL, true, "");
    }
    if (data->value == WebDragResizeStatus::WEB_DRAG_END) {
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(PERF_REQUEST_CMD_ID_WEB_SLIDE_SCROLL, false, "");
    }
}

bool SocPerfPlugin::HandleRecentBuild(const std::shared_ptr<ResData>& data)
{
    if (data == nullptr) {
        return false;
    }
    SOC_PERF_LOGD("SocPerfPlugin: socperf->HandleRecentBuild: %{public}lld", (long long)data->value);
    if (data->value == RecentBuildStatus::RECENT_BUILD_START) {
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(PERF_REQUEST_CMD_ID_RECENT_BUILD, true, "");
    } else if (data->value == RecentBuildStatus::RECENT_BUILD_STOP) {
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(PERF_REQUEST_CMD_ID_RECENT_BUILD, false, "");
    }
    return true;
}

extern "C" bool OnPluginInit(std::string& libName)
{
    if (libName != LIB_NAME) {
        SOC_PERF_LOGE("SocPerfPlugin::OnPluginInit lib name is not match");
        return false;
    }
    SocPerfPlugin::GetInstance().Init();
    return true;
}

extern "C" void OnPluginDisable()
{
    SocPerfPlugin::GetInstance().Disable();
}

extern "C" void OnDispatchResource(const std::shared_ptr<ResData>& data)
{
    SocPerfPlugin::GetInstance().DispatchResource(data);
}
} // namespace ResourceSchedule
} // namespace OHOS
#endif // RESSCHED_RESOURCESCHEDULE_SOC_PERF_ENABLE
