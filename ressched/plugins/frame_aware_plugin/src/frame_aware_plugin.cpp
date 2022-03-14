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
    const int INIT_VAL = -1;
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
    RME::FrameMsgIntf::GetInstance().Init();
    RESSCHED_LOGI("FrameAwarePlugin::Init success");
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
    RESSCHED_LOGI("FrameAwarePlugin:DispatchResource type:%{public}u, value:%{public}lld", data->resType, data->value);
    Json::Value payload = data->payload;
    int pid = INIT_VAL;
    switch (data->resType) {
        case RES_TYPE_APP_STATE_CHANGE:
            {
                int uid = payload["uid"].asInt();
                RESSCHED_LOGD("FrameAwarePlugin::[DispatchResource]:app state! uid:%{public}d", uid);
            }
            break;
        case RES_TYPE_PROCESS_STATE_CHANGE:
            {
                pid = payload["pid"].asInt();
                int tid = INIT_VAL;
                RME::ThreadState state = static_cast<RME::ThreadState>(data->value);
                RME::FrameMsgIntf::GetInstance().ReportProcessInfo(pid, tid, state);
                RESSCHED_LOGD("FrameAwarePlugin::[DispatchResource]:process info! resType: %{public}u.", data->resType);
            }
            break;
        case RES_TYPE_WINDOW_FOCUS:
            {
                pid = payload["pid"].asInt();
                RME::FrameMsgIntf::GetInstance().ReportWindowFocus(pid, data->value);
                RESSCHED_LOGD("FrameAwarePlugin::[DispatchResource]:window focus! resType: %{public}u.", data->resType);
            }
            break;
        default:
            RESSCHED_LOGI("FrameAwarePlugin::[DispatchResource]:unknow msg, resType: %{public}u.", data->resType);
            break;
    }
}

extern "C" bool OnPluginInit(std::string& libName)
{
    if (libName != LIB_NAME) {
        RESSCHED_LOGE("FrameAwarePlugin::OnPluginInit lib name is not match");
        return false;
    }
    FrameAwarePlugin::GetInstance().Init();
    RESSCHED_LOGI("FrameAwarePlugin::OnPluginInit success.");
    return true;
}

extern "C" void OnPluginDisable()
{
    FrameAwarePlugin::GetInstance().Disable();
    RESSCHED_LOGI("FrameAwarePlugin::OnPluginDisable success.");
}

extern "C" void OnDispatchResource(const std::shared_ptr<ResData>& data)
{
    FrameAwarePlugin::GetInstance().DispatchResource(data);
    RESSCHED_LOGI("FrameAwarePlugin::OnDispatchResource success.");
}
}
}
