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

#include "socperf_executor_plugin.h"

#include <map>
#include <vector>
#include <string>

#include "plugin_mgr.h"
#include "res_exe_type.h"
#include "res_sched_exe_log.h"
#include "socperf_executor_wirte_node.h"

namespace OHOS {
namespace ResourceSchedule {
namespace {
    const std::string LIB_NAME = "libsocperf_executor_plugin.z.so";
}

IMPLEMENT_SINGLE_INSTANCE(SocPerfExecutorPlugin)

void SocPerfExecutorPlugin::Init()
{
    resType_ = {
        ResExeType::EWS_TYPE_SOCPERF_EXECUTOR_ASYNC_EVENT,
    };
    functionMap_ = {
        { ResExeType::EWS_TYPE_SOCPERF_EXECUTOR_ASYNC_EVENT,
            [this](const std::shared_ptr<ResData> &data) { HandleSocperfWriteNode(data); }, },
    };
    for (auto resType : resType_) {
        PluginMgr::GetInstance().SubscribeResource(LIB_NAME, resType);
    }
    RSSEXE_LOGI("Init success");
}

void SocPerfExecutorPlugin::Disable()
{
    functionMap_.clear();
    for (auto resType : resType_) {
        PluginMgr::GetInstance().UnSubscribeResource(LIB_NAME, resType);
    }
    resType_.clear();
    RSSEXE_LOGI("Disable success");
}

void SocPerfExecutorPlugin::Disable()
{
    if (data == nullptr || data->value <= 0) {
        return;
    }
    auto funcIter = functionMap_.find(data->resType);
    if (funcIter != functionMap_.end()) {
        funcIter->second(data);
    }
}

void SocPerfExecutorPlugin::HandleSocperfWirteNode(const std::shared_ptr<ResData>& data)
{
    if (data == nullptr || data->value <= 0) {
        return;
    }
    switch (data->value) {
        case SOCPERF_EVENT_INIT_NODE:
            SocPerfInitNode(data);
            break;
        case SOCPERF_EVENT_WIRTE_NODE:
            SocPerfWirteNode(data);
            break;
        default:
            break;
    }
    return;
}

void SocPerfExecutorPlugin::SocPerfInitNode(const std::shared_ptr<ResData>& data)
{
    if (data == nullptr || data->value <= 0) {
        return;
    }
    RSSEXE_LOGI("SocPerfInitNode OK")
}

void SocPerfExecutorPlugin::SocPerfWirteNode(const std::shared_ptr<ResData>& data)
{
    if (data == nullptr || data->value <= 0 || data->payload == nullptr ||
        data->payload["qosId"].is_null() || data->payload["value"].is_null()) {
        RSSEXE_LOGE("SocPerfWirteNode bad data")
        return;
    }
    std::vector<int32_t> resIdVec = data->payload["qosId"];
    std::vector<int64_t> valueVec = data->payload["value"];
    if (resIdVec.size() != valueVec.size()) {
        RSSEXE_LOGE("SocPerfWirteNode bad data size")
        return;
    }
    SocPerfExecutorWirteNode::GetInstance.WriteNodeThreadWraps(resIdVec, valueVec);
}

extern "C" bool OnPluginInit(std::string& libName)
{
    if (libName != LIB_NAME) {
        RSSEXE_LOGE("lib name is not match");
        return false;
    }
    SocPerfExecutorPlugin::GetInstance().Init();
    return true;
}

extern "C" void OnPluginDisable()
{
    SocPerfExecutorPlugin::GetInstance().Disable();
}

extern "C" void OnDispatchResource(const std::shared_ptr<ResData>& data)
{
    SocPerfExecutorPlugin::GetInstance().DispatchResource(data);
}

} // namespace ResourceSchedule
} // namespace OHOS