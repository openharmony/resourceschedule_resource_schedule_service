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

#include "frame_aware_plugin.h"
#ifdef RESSCHED_FRAME_AWARE_SCHED_ENABLE
#include "frame_msg_intf.h"
#endif
#include "res_type.h"
#include "frame_aware_log.h"
#include "plugin_mgr.h"
#include "config_info.h"
#include "parameters.h"

#undef LOG_TAG
#define LOG_TAG "ueaServer-FrameAwarePlugin"

namespace OHOS {
namespace ResourceSchedule {
using namespace ResType;
namespace {
    const std::string LIB_NAME = "libframe_aware_plugin.z.so";
    const int DECIMAL = 10;
}
IMPLEMENT_SINGLE_INSTANCE(FrameAwarePlugin)

void FrameAwarePlugin::Init()
{
    netLatCtrl.Init();
    functionMap = {
#ifdef RESSCHED_FRAME_AWARE_SCHED_ENABLE
        { RES_TYPE_APP_STATE_CHANGE,
            [this](const std::shared_ptr<ResData>& data) { HandleAppStateChange(data); } },
        { RES_TYPE_PROCESS_STATE_CHANGE,
            [this](const std::shared_ptr<ResData>& data) { HandleProcessStateChange(data); } },
        { RES_TYPE_CONTINUOUS_TASK,
            [this](const std::shared_ptr<ResData>& data) { HandleContinuousTask(data); } },
        { RES_TYPE_CGROUP_ADJUSTER,
            [this](const std::shared_ptr<ResData>& data) { HandleCgroupAdjuster(data); } },
        { RES_TYPE_WINDOW_FOCUS,
            [this](const std::shared_ptr<ResData>& data) { HandleWindowsFocus(data); } },
        { RES_TYPE_REPORT_RENDER_THREAD,
            [this](const std::shared_ptr<ResData>& data) { HandleReportRender(data); } },
        { RES_TYPE_SLIDE_RECOGNIZE,
            [this](const std::shared_ptr<ResData>& data) { HandleEventSlide(data); } },
        { RES_TYPE_SCREEN_LOCK,
            [this](const std::shared_ptr<ResData>& data) { HandleScreenLock(data); } },
        { RES_TYPE_SCREEN_STATUS,
            [this](const std::shared_ptr<ResData>& data) { HandleScreenStatus(data); } },
        { RES_TYPE_USER_INTERACTION_SCENE,
            [this](const std::shared_ptr<ResData>& data) { HandleInteractionScene(data); } },
#endif
        { RES_TYPE_NETWORK_LATENCY_REQUEST,
            [this](const std::shared_ptr<ResData>& data) { HandleNetworkLatencyRequest(data); } },
    };
    resTypes = {
        RES_TYPE_APP_STATE_CHANGE,
        RES_TYPE_PROCESS_STATE_CHANGE,
        RES_TYPE_CONTINUOUS_TASK,
        RES_TYPE_CGROUP_ADJUSTER,
        RES_TYPE_WINDOW_FOCUS,
        RES_TYPE_REPORT_RENDER_THREAD,
        RES_TYPE_NETWORK_LATENCY_REQUEST,
        RES_TYPE_SLIDE_RECOGNIZE,
        RES_TYPE_SCREEN_LOCK,
        RES_TYPE_SCREEN_STATUS,
        RES_TYPE_USER_INTERACTION_SCENE,
    };
    for (auto resType : resTypes) {
        PluginMgr::GetInstance().SubscribeResource(LIB_NAME, resType);
    }
#ifdef RESSCHED_FRAME_AWARE_SCHED_ENABLE
    RME::FrameMsgIntf::GetInstance().Init();
#endif
    RME_LOGI("FrameAwarePlugin::Init ueaServer success");
}

void FrameAwarePlugin::Disable()
{
    for (auto resType : resTypes) {
        PluginMgr::GetInstance().UnSubscribeResource(LIB_NAME, resType);
    }
#ifdef RESSCHED_FRAME_AWARE_SCHED_ENABLE
    RME::FrameMsgIntf::GetInstance().Stop();
#endif
    functionMap.clear();
    resTypes.clear();
    RME_LOGI("FrameAwarePlugin::Disable ueaServer success");
}

#ifdef RESSCHED_FRAME_AWARE_SCHED_ENABLE
void FrameAwarePlugin::HandleAppStateChange(const std::shared_ptr<ResData>& data)
{
    if (!data->payload.is_object()) {
        return;
    }

    if (!data->payload.contains("pid") || !data->payload.contains("uid") || !data->payload.contains("bundleName") ||
        !data->payload["pid"].is_string() || !data->payload["uid"].is_string() ||
        !data->payload["bundleName"].is_string()) {
        RME_LOGI("FrameAwarePlugin::HandleAppStateChange payload is not contains pid or uid or bundleName");
        return;
    }

    int pid = ConvertToInteger(data, "pid");
    int uid = ConvertToInteger(data, "uid");
    std::string bundleName = data->payload["bundleName"].get<std::string>().c_str();
    RME::ThreadState state = static_cast<RME::ThreadState>(data->value);
    RME::FrameMsgIntf::GetInstance().ReportAppInfo(pid, uid, bundleName, state);
}

void FrameAwarePlugin::HandleProcessStateChange(const std::shared_ptr<ResData>& data)
{
    if (!data->payload.is_object()) {
        return;
    }

    if (!data->payload.contains("pid") || !data->payload.contains("uid") || !data->payload.contains("bundleName") ||
        !data->payload["pid"].is_string() || !data->payload["uid"].is_string() ||
        !data->payload["bundleName"].is_string()) {
        RME_LOGI("FrameAwarePlugin::HandleProcessStateChange payload is not contains pid or uid or bundleName");
        return;
    }

    int pid = ConvertToInteger(data, "pid");
    int uid = ConvertToInteger(data, "uid");
    std::string bundleName = data->payload["bundleName"].get<std::string>().c_str();
    RME::ThreadState state = static_cast<RME::ThreadState>(data->value);
    RME::FrameMsgIntf::GetInstance().ReportProcessInfo(pid, uid, bundleName, state);
}

void FrameAwarePlugin::HandleContinuousTask(const std::shared_ptr<ResData>& data)
{
    if (!data->payload.is_object()) {
        return;
    }
    if (!data->payload.contains("pid") || !data->payload.contains("uid") ||
        !data->payload["pid"].is_string() || !data->payload["uid"].is_string()) {
        RME_LOGI("FrameAwarePlugin::HandleContinuousTask payload is not contains pid or uid");
        return;
    }

    int pid = ConvertToInteger(data, "pid");
    int uid = ConvertToInteger(data, "uid");
    RME::FrameMsgIntf::GetInstance().ReportContinuousTask(pid, uid, data->value);
}

void FrameAwarePlugin::HandleCgroupAdjuster(const std::shared_ptr<ResData>& data)
{
    if (!data->payload.is_object()) {
        return;
    }

    if (!data->payload.contains("pid") || !data->payload.contains("uid") ||
        !data->payload["pid"].is_string() || !data->payload["uid"].is_string()) {
        RME_LOGI("FrameAwarePlugin::HandleCgroupAdjuster payload is not contains pid or uid");
        return;
    }

    if (!data->payload.contains("oldGroup") || !data->payload.contains("newGroup") ||
        !data->payload["oldGroup"].is_string() || !data->payload["newGroup"].is_string()) {
        RME_LOGI("FrameAwarePlugin::HandleCgroupAdjuster payload is not contains oldGroup or newGroup");
        return;
    }
    int pid = ConvertToInteger(data, "pid");
    int uid = ConvertToInteger(data, "uid");
    int oldGroup = ConvertToInteger(data, "oldGroup");
    int newGroup = ConvertToInteger(data, "newGroup");
    if (!data->value) {
        RME::FrameMsgIntf::GetInstance().ReportCgroupChange(pid, uid, oldGroup, newGroup);
    }
}

void FrameAwarePlugin::HandleWindowsFocus(const std::shared_ptr<ResData>& data)
{
    if (!data->payload.is_object()) {
        return;
    }

    if (!data->payload.contains("pid") || !data->payload.contains("uid") ||
        !data->payload["pid"].is_string() || !data->payload["uid"].is_string()) {
        RME_LOGI("FrameAwarePlugin::HandleWindowsFocus payload is not contains pid or uid");
        return;
    }

    int pid = ConvertToInteger(data, "pid");
    int uid = ConvertToInteger(data, "uid");
    RME::FrameMsgIntf::GetInstance().ReportWindowFocus(pid, uid, data->value);
}

void FrameAwarePlugin::HandleReportRender(const std::shared_ptr<ResData>& data)
{
    if (!data->payload.is_object()) {
        return;
    }

    if (!data->payload.contains("pid") || !data->payload.contains("uid") ||
        !data->payload["pid"].is_string() || !data->payload["uid"].is_string()) {
        RME_LOGI("FrameAwarePlugin::HandleReportRender payload is not contains pid or uid");
        return;
    }

    int pid = ConvertToInteger(data, "pid");
    int uid = ConvertToInteger(data, "uid");
    RME::FrameMsgIntf::GetInstance().ReportRenderThread(pid, uid, data->value);
}
#endif

void FrameAwarePlugin::HandleNetworkLatencyRequest(const std::shared_ptr<ResData>& data)
{
    if (!data->payload.is_object()) {
        return;
    }

    if (!data->payload.contains("clientPid") || !data->payload["clientPid"].is_string()) {
        RME_LOGE("FrameAwarePlugin::HandleNetworkLatencyRequest payload does not contain clientPid");
        return;
    }

    if (!data->payload.contains("identity") || !data->payload["identity"].is_string()) {
        RME_LOGI("FrameAwarePlugin::HandleNetworkLatencyRequest payload does not contain identity");
        return;
    }

    long long value = data->value;
    const std::string pid = data->payload["clientPid"].get<std::string>();
    const std::string identity = data->payload["identity"].get<std::string>();

    // add the pid prefix to distinguish identities between different apps
    const std::string pid_identity = pid + ":" + identity;
    netLatCtrl.HandleRequest(value, pid_identity);
}

#ifdef RESSCHED_FRAME_AWARE_SCHED_ENABLE
void FrameAwarePlugin::HandleEventSlide(const std::shared_ptr<ResData>& data)
{
    if (!data->payload.is_object()) {
        return;
    }

    if (!data->payload.contains("clientPid") || !data->payload.contains("callingUid") ||
        !data->payload["clientPid"].is_string() || !data->payload["callingUid"].is_string()) {
        RME_LOGI("FrameAwarePlugin::HandleEventSlide payload is not contains pid or uid");
        return;
    }

    int pid = ConvertToInteger(data, "clientPid");
    int uid = ConvertToInteger(data, "callingUid");
    RME::FrameMsgIntf::GetInstance().ReportSlideEvent(pid, uid, data->value);
}
#endif

void FrameAwarePlugin::HandleScreenLock(const std::shared_ptr<ResData>& data)
{
    if (!data->payload.is_object()) {
        return;
    }
    if (!data->value) {
        RME_LOGI("Screen Lock Report");
    }
}

void FrameAwarePlugin::HandleScreenStatus(const std::shared_ptr<ResData>& data)
{
    if (!data->payload.is_object()) {
        return;
    }
    if (data->value) {
        RME_LOGI("Screen Bright Report");
    }
}

#ifdef RESSCHED_FRAME_AWARE_SCHED_ENABLE
void FrameAwarePlugin::HandleInteractionScene(const std::shared_ptr<ResData>& data)
{
    RME::FrameMsgIntf::GetInstance().ReportInteractionScene(data->value);
}
#endif

void FrameAwarePlugin::DispatchResource(const std::shared_ptr<ResData>& data)
{
    RME_LOGD("FrameAwarePlugin:DispatchResource type:%{public}u, value:%{public}lld",
             data->resType, (long long)data->value);
    auto funcIter = functionMap.find(data->resType);
    if (funcIter != functionMap.end()) {
        auto function = funcIter->second;
        if (function) {
            function(data);
        }
    }
}

int FrameAwarePlugin::ConvertToInteger(const std::shared_ptr<ResData>& data, const char* idtype)
{
    char* endPtr = nullptr;
    std::string temp = data->payload[idtype].get<std::string>();
    const char* target = temp.c_str();
    int result = static_cast<int>(strtol(target, &endPtr, DECIMAL));
    if (result < 0 ||result > INT_MAX || errno == ERANGE || endPtr == target || !endPtr || *endPtr != '\0') {
        RME_LOGE("FrameAwarePlugin:Failed to convert integer!");
        return -1;
    }
    return result;
}

extern "C" bool OnPluginInit(std::string& libName)
{
    if (libName != LIB_NAME) {
        RME_LOGE("FrameAwarePlugin::OnPluginInit lib name is not match");
        return false;
    }
    FrameAwarePlugin::GetInstance().Init();
    RME_LOGI("FrameAwarePlugin::OnPluginInit success.");
    return true;
}

extern "C" void OnPluginDisable()
{
    FrameAwarePlugin::GetInstance().Disable();
    RME_LOGI("FrameAwarePlugin::OnPluginDisable success.");
}

extern "C" void OnDispatchResource(const std::shared_ptr<ResData>& data)
{
    FrameAwarePlugin::GetInstance().DispatchResource(data);
}
}
}
