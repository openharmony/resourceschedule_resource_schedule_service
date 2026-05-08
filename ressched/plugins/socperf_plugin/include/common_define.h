/*
 * Copyright (c) 2026-2026 Huawei Device Co., Ltd.
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

#ifndef RESSCHED_PLUGINS_SOCPERF_PLUGIN_INCLUDE_COMMON_DEFINE_H
#define RESSCHED_PLUGINS_SOCPERF_PLUGIN_INCLUDE_COMMON_DEFINE_H

#include <string>
#include <unordered_set>

namespace OHOS {
namespace ResourceSchedule {
namespace {
    const std::string LIB_NAME = "libsocperf_plugin.z.so";
    const std::string PLUGIN_NAME = "SOCPERF";
    const std::string ITEM_KEY_CAPACITY = "capacity";
    const std::string CONFIG_NAME_SOCPERF_FEATURE_SWITCH = "socperfFeatureSwitch";
    const std::string CONFIG_NAME_SOCPERF_EVENT_ID = "socperfEventId";
    const std::string CONFIG_NAME_SOCPERF_POLICY_MODE = "socperfPolicyMode";
    const std::string CONFIG_NAME_SOCPERF_LIMIT_POLICY = "limitPolicy";
    const std::string SUB_ITEM_KEY_NAME_SOCPERF_ON_DEMAND = "socperf_on_demand";
    const std::string SUB_ITEM_KEY_NAME_SOCPERF_GAME_BOOST = "socperf_game_boost";
    const std::string CONFIG_NAME_SOCPERF_BUNDLE_NAME_BOOST_LIST = "socperfBundleNameBoostList";
    const std::string SUB_ITEM_KEY_NAME_APP_USE_CAMERA_BOOST = "app_use_camera_boost";
    const std::string SUB_ITEM_KEY_NAME_APP_MOVE_EVENT_BOOST = "app_move_event_boost";
    const std::string CNOFIG_NAME_SOCPERF_CRUCIAL_FUNC = "socperfCrucialFunc";
    const std::string SUB_ITEM_KEY_NAME_SOCPERF_RERQ_APPTYPE_PATH = "socperf_req_apptype_path";
    const std::string SUB_ITEM_KEY_NAME_SOCPERF_RERQ_APPTYPE_FUNC = "socperf_req_apptype_func";
    const std::string CONFIG_NAME_SOCPERF_BATTERY_CAPACITY_LIMIT_FREQ = "socperfBatteryCapacityLimitFreq";
    const std::string CONFIG_NAME_SOCPERF_GAME_COLD_START_BOOST_SWITCH = "socperfGameColdStartBoostSwitch";
    const std::string GAME_BOOST_SWITCH = "switch";
    const std::string BUNDLE_NAME = "bundleName";
    const std::string CALLER_BUNDLE_NAME = "callerBundleName";
    const std::string SPECIAL_EXTENSION_STRING = "specialExtension";
    const std::string INFO_STRING = "info";
    const std::string PID_NAME = "pid";
    const std::string CLIENT_PID_NAME = "clientPid";
    const std::string UID_NAME = "uid";
    const std::string CALLING_UID_NAME = "callingUid";
    const std::string SOCPERF_TYPE_ID = "socperf_type_id";
    const std::string SOCPERF_TYPE_RGM = "socperf_type_rgm";
    const std::string SOCPERF_POLICY_MODE = "socperf_mode";
    const std::string EXTENSION_TYPE_KEY = "extensionType";
    const std::string DEVICE_MODE_TYPE_KEY = "deviceModeType";
    const std::string DEVICE_MODE_PAYMODE_NAME = "deviceMode";
    const std::string DISPLAY_MODE_KEY = "display";
    const std::string DEVICE_ORIENTATION_TYPE_KEY = "deviceOrientation";
    const std::string DISPLAY_MODE_FULL = "displayFull";
    const std::string DISPLAY_MODE_MAIN = "displayMain";
    const std::string DISPLAY_MODE_SUB = "displaySub";
    const std::string DISPLAY_MODE_GLOBAL_FULL = "displayGlobalFull";
    const std::string DISPLAY_ORIENTAYION_LANDSCAPE = "displayLandscape";
    const std::string DISPLAY_ORIENTAYION_PORTRAIT = "displayPortrait";
    const std::string SCREEN_MODE_KEY = "screenStatus";
    const std::string SCREEN_MODE_ON = "screen_on";
    const std::string SCREEN_MODE_OFF = "screen_off";
    const std::string POWER_MODE_KEY = "power";
    const std::string POWER_MODE = "powerMode";
    const std::string POWER_STATUS_KEY = "powerStatus";
    const std::string PRELOAD_MODE = "preloadMode";
    const std::string PRELAUNCH = "isPrelaunch";
    const std::string WEAK_ACTION_STRING = "weakInterAction";
    const std::string WEAK_ACTION_MODE = "actionmode:weakaction";
    const std::string KEY_APP_TYPE = "key_app_type";
    const std::string GAME_ENV = "env";
    const std::string RES_ID = "resId";
    const std::string COMMON_EVENT_CHARGE_STATE = "chargeState";
    const std::string SCHED_MODE_KEY = "schedMode";
    const std::string CLOUD_PARAMS = "params";
    const std::string ENABLE_STRING = "enable";
    const int32_t SCENE_LIVE_BROADCAST                      = 5;
    const int32_t SOC_PERF_SA_ID                            = 1906;
    const int32_t INVALID_VALUE                             = -1;
    const int32_t APP_TYPE_GAME                             = 2;
    const int32_t INVALID_APP_TYPE                          = 0;
    const int32_t POWERMODE_ON                              = 601;
    const int64_t TIME_INTERVAL                             = 4500;
    const int64_t WAKE_UP_TIME_DELAY                        = 3000000L;
    const int64_t SCREEN_OFF_TIME_DELAY                     = 5000000L;
    const int32_t PERF_REQUEST_CMD_ID_RGM_BOOTING_START     = 1000;
    const int32_t PERF_REQUEST_CMD_ID_APP_START             = 10000;
    const int32_t PERF_REQUEST_CMD_ID_WARM_START            = 10001;
    const int32_t PERF_REQUEST_CMD_ID_WINDOW_SWITCH         = 10002;
    const int32_t PERF_REQUEST_CMD_ID_EVENT_CLICK           = 10006;
    const int32_t PERF_REQUEST_CMD_ID_LOAD_PAGE_START       = 10007;
    const int32_t PERF_REQUEST_CMD_ID_EVENT_FLING           = 10008;
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
    const int32_t PERF_REQUEST_CMD_ID_LIVE_BROADCAST        = 10224;
    const int32_t PERF_REQUEST_CMD_ID_OOBE_CLONE            = 10225;
    const int32_t PERF_REQUEST_CMD_ID_IMAGE_WORK_PROCESS_START = 10221;

    const int32_t PERF_REQUEST_CMD_ID_SCREEN_ON_PERFORMANCE_MODE = 30028;
    const int32_t PERF_REQUEST_CMD_ID_SCREEN_OFF_PERFORMANCE_MODE = 30029;
    // PREMAKE_MODE_STRING = 1, PRELOADMODULE_MODE_STRING = 2,
    // PRELAUNCH_MODE_STRING = 4
    const std::unordered_set<std::string> PRELOAD_SET = {"1", "2", "4"};
}

} // namespace ResourceSchedule
} // namespace OHOS

#endif // RESSCHED_PLUGINS_SOCPERF_PLUGIN_INCLUDE_COMMON_DEFINE_H
