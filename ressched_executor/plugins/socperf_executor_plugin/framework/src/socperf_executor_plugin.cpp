/*
 * Copyright (c) 2024-2024 Huawei Device Co., Ltd.
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
#include "socperf_executor_wirte_node.h"
#include "socperf_log.h"

namespace OHOS {
namespace ResourceSchedule {
namespace {
    const std::string LIB_NAME = "libsocperf_executor_plugin.z.so";
    const std::string QOSID_STRING = "qosId";
    const std::string VALUE_STRING = "value";
}

IMPLEMENT_SINGLE_INSTANCE(SocPerfExecutorPlugin)

void SocPerfExecutorPlugin::Init()
{
    resType_ = {
        ResExeType::EWS_TYPE_SOCPERF_EXECUTOR_ASYNC_EVENT,
    };
    functionMap_ = {
        { ResExeType::EWS_TYPE_SOCPERF_EXECUTOR_ASYNC_EVENT,
            [this](const std::shared_ptr<ResData> &data) { HandleSocperfWirteNode(data); }, },
    };
    for (auto resType : resType_) {
        PluginMgr::GetInstance().SubscribeResource(LIB_NAME, resType);
    }
    SocPerfInitNode();
    SOC_PERF_LOGI("Init success");
}

void SocPerfExecutorPlugin::Disable()
{
    functionMap_.clear();
    for (auto resType : resType_) {
        PluginMgr::GetInstance().UnSubscribeResource(LIB_NAME, resType);
    }
    resType_.clear();
    SOC_PERF_LOGI("Disable success");
}

void SocPerfExecutorPlugin::DispatchResource(const std::shared_ptr<ResData>& data)
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
        case SOCPERF_EVENT_WIRTE_NODE:
            SocPerfWirteNode(data);
            break;
        default:
            break;
    }
    return;
}

void SocPerfExecutorPlugin::SocPerfInitNode()
{
    SocPerfExecutorWirteNode::GetInstance().InitThreadWraps();
    SOC_PERF_LOGI("SocPerfInitNode OK");
}

void SocPerfExecutorPlugin::SocPerfWirteNode(const std::shared_ptr<ResData>& data)
{
    if (data == nullptr || data->value <= 0 || data->payload == nullptr ||
        data->payload[QOSID_STRING].is_null() || data->payload[VALUE_STRING].is_null()) {
        SOC_PERF_LOGE("SocPerfWirteNode bad data");
        return;
    }
    const std::vector<int32_t> resIdVec = data->payload[QOSID_STRING];
    const std::vector<int64_t> valueVec = data->payload[VALUE_STRING];
    if (resIdVec.size() != valueVec.size()) {
        SOC_PERF_LOGE("SocPerfWirteNode bad data size");
        return;
    }
    SocPerfExecutorWirteNode::GetInstance().WriteNodeThreadWraps(resIdVec, valueVec);
}

extern "C" bool OnPluginInit(std::string& libName)
{
    if (libName != LIB_NAME) {
        SOC_PERF_LOGE("lib name is not match");
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