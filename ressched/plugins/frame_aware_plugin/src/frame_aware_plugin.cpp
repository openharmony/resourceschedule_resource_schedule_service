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
#include "frame_msg_intf.h"
#include "res_type.h"
#include "res_sched_log.h"
#include "plugin_mgr.h"
#include "config_info.h"

namespace OHOS {
namespace ResourceSchedule {
using namespace ResType;
namespace {
    const std::string LIB_NAME = "libframe_aware_plugin.z.so";
}
IMPLEMENT_SINGLE_INSTANCE(FrameAwarePlugin)

void FrameAwarePlugin::Init()
{
    resTypes.insert(RES_TYPE_WINDOW_FOCUS);
    resTypes.insert(RES_TYPE_PROCESS_STATE_CHANGE);
    resTypes.insert(RES_TYPE_APP_STATE_CHANGE);
    for (auto resType : resTypes) {
        PluginMgr::GetInstance().SubscribeResource(LIB_NAME, resType);
    }
    RESSCHED_LOGI("ueaServerFrameAwarePlugin::Init begin");
    RME::FrameMsgIntf::GetInstance().Init();
    RESSCHED_LOGI("ueaServerFrameAwarePlugin::Init success");
}

void FrameAwarePlugin::Disable()
{
    RME::FrameMsgIntf::GetInstance().Stop();
    for (auto resType : resTypes) {
        PluginMgr::GetInstance().UnSubscribeResource(LIB_NAME, resType);
    }
    resTypes.clear();
    RESSCHED_LOGI("ueaServerFrameAwarePlugin::Disable success");
}

void FrameAwarePlugin::DispatchResource(const std::shared_ptr<ResData>& data)
{
    RESSCHED_LOGI("ueaServerFrameAwarePlugin::[DispatchResource] status type %{public}u, value=%{public}lld", data->resType, data->value);
    std::vector<std::string> payload = ParsePayload(data->payload);
    int pid = -1;
    switch (data->resType) {
        case RES_TYPE_APP_STATE_CHANGE:
            {
                int uid = std::stoi(payload[0]);
                RESSCHED_LOGI("ueaServerFrameAwarePlugin::[DispatchResource]:send app state info success! uid:%{public}d.", uid);
            }
            break;
        case RES_TYPE_PROCESS_STATE_CHANGE: 
            {
                pid = std::stoi(payload[0]);
                int tid = -1;
                RME::FrameMsgIntf::GetInstance().ReportProcessInfo(pid, tid, static_cast<RME::ThreadState>(data->value));
                RESSCHED_LOGI("ueaServerFrameAwarePlugin::[DispatchResource]:send process info success! resType: %{public}u.", data->resType);
            }
            break;
        case RES_TYPE_WINDOW_FOCUS:
            {
                pid = std::stoi(payload[0]);
                RME::FrameMsgIntf::GetInstance().ReportWindowFocus(pid, data->value);
                RESSCHED_LOGI("ueaServerFrameAwarePlugin::[DispatchResource]:send window focus success! resType: %{public}u.", data->resType);
            }
            break;
        default:
            RESSCHED_LOGI("ueaServerFrameAwarePlugin::[DispatchResource]:get unknow resource msg, resType: %{public}u.", data->resType);
            break;
    }
}

std::vector<std::string> FrameAwarePlugin::ParsePayload(const std::string payload)
{
    std::vector<std::string> res;
    if ("" == payload) {
        return res;
    }

    std::string payloadP = payload + ",";
    size_t pos = payloadP.find(",");
    size_t size = payloadP.size();

    while (pos != std::string::npos) {
        std::string tmpStr = payloadP.substr(0, pos);
        res.push_back(tmpStr);
        payloadP = payloadP.substr(pos + 1, size);
        pos = payloadP.find(",");
    }
/*    for (auto s : payload) {
        if (s == ",") {
            res.push_back(tmpStr);
            tmpStr = "";
        } else {
            tmpStr = tmpStr + s;
        }
    }*/
    return res;
}

extern "C" bool OnPluginInit(std::string& libName)
{
    if (libName != LIB_NAME) {
        RESSCHED_LOGE("ueaServerFrameAwarePlugin::OnPluginInit lib name is not match");
        return false;
    }
    FrameAwarePlugin::GetInstance().Init();
    RESSCHED_LOGI("ueaServerFrameAwarePlugin::OnPluginInit success.");
    return true;
}

extern "C" void OnPluginDisable()
{
    FrameAwarePlugin::GetInstance().Disable();
    RESSCHED_LOGI("ueaServerFrameAwarePlugin::OnPluginDisable success.");
}

extern "C" void OnDispatchResource(const std::shared_ptr<ResData>& data)
{
    FrameAwarePlugin::GetInstance().DispatchResource(data);
    RESSCHED_LOGI("ueaServerFrameAwarePlugin::OnDispatchResource success.");
}
}
}
