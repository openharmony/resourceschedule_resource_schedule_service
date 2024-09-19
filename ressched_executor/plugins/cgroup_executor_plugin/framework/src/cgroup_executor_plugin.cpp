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

#include "cgroup_executor_plugin.h"
#include "cgroup_action.h"
#include "cgroup_sched_log.h"
#include "plugin_mgr.h"
#include "res_exe_type.h"

namespace OHOS {
namespace ResourceSchedule {
namespace CgroupSetting {
namespace {
    const std::string LIB_NAME = "cgroup_executor_plugin.z.so";
}

IMPLEMENT_SINGLE_INSTANCE(CgroupExecutorPlugin)

void CgroupExecutorPlugin::Init()
{
    resTypes_ = {
        ResExeType::RES_TYPE_SET_THREAD_SCHED_POLICY_SYNC_EVENT,
        ResExeType::RES_TYPE_SET_THREAD_GROUP_SCHED_POLICY_SYNC_EVENT,
    };
    for (auto resType : resTypes_) {
        PluginMgr::GetInstance().SubscribeResource(LIB_NAME, resType);
    }
    CGS_LOGI("%{public}s : init success", __func__);
}

void CgroupExecutorPlugin::Disable()
{
    for (auto resType : resTypes_) {
        PluginMgr::GetInstance().UnSubscribeResource(LIB_NAME, resType);
    }
    resTypes_.clear();
    CGS_LOGI("%{public}s : Disable success", __func__);
}

int32_t CgroupExecutorPlugin::DeliverResource(const std::shared_ptr<ResData>& resData)
{
    int32_t ret = -1;
    if (!(resTypes_.count(resData->resType))) {
        CGS_LOGE("%{public}s : invalid resType %{public}d", __func__, resData->resType);
        return ret;
    }
    int tid;
    int policy;
    if (!(ParseValue(tid, "tid", resData->payload)) || !(ParseValue(policy, "policy", resData->payload))) {
        CGS_LOGE("%{public}s : ParseValue failed", __func__);
        return ret;
    }

    SchedPolicy schedPolicy = SchedPolicy(policy);
    switch (resData->resType) {
        case ResExeType::RES_TYPE_SET_THREAD_SCHED_POLICY_SYNC_EVENT:
            ret = CgroupAction::GetInstance().SetThreadSchedPolicy(tid, schedPolicy) ? 0 : -1;
            break;
        case ResExeType::RES_TYPE_SET_THREAD_GROUP_SCHED_POLICY_SYNC_EVENT:
            ret = CgroupAction::GetInstance().SetThreadGroupSchedPolicy(tid, schedPolicy) ? 0 : -1;
            break;
        default:
            break;
    }
    (*(resData->reply))["ret"] = std::to_string(ret);
    return ret;
}

void CgroupExecutorPlugin::DispatchResource(const std::shared_ptr<ResData>& data)
{}

bool CgroupExecutorPlugin::ParseValue(int32_t& value, const char* name, const nlohmann::json& payload)
{
    if (payload.contains(name) && payload.at(name).is_string()) {
        value = atoi(payload[name].get<std::string>().c_str());
        return true;
    }
    return false;
}

extern "C" bool OnPluginInit(std::string& libName)
{
    if (libName != LIB_NAME) {
        CGS_LOGE("%{public}s : lib name is not match", __func__);
        return false;
    }
    CgroupExecutorPlugin::GetInstance().Init();
    return true;
}

extern "C" void OnPluginDisable()
{
    CgroupExecutorPlugin::GetInstance().Disable();
}

extern "C" int32_t OnDeliverResource(const std::shared_ptr<ResData>& data)
{
    return CgroupExecutorPlugin::GetInstance().DeliverResource(data);
}

extern "C" void OnDispatchResource(const std::shared_ptr<ResData>& data)
{
    CgroupExecutorPlugin::GetInstance().DispatchResource(data);
}
} // namespace CgroupSetting
} // namespace ResourceSchedule
} // namespace OHOS