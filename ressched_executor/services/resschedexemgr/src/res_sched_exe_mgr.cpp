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

#include "res_sched_exe_mgr.h"

#include <cinttypes>

#include "hitrace_meter.h"

#include "plugin_mgr.h"
#include "res_exe_type.h"
#include "res_sched_exe_constants.h"
#include "res_sched_exe_log.h"

namespace OHOS {
namespace ResourceSchedule {

IMPLEMENT_SINGLE_INSTANCE(ResSchedExeMgr);

void ResSchedExeMgr::Init()
{
    PluginMgr::GetInstance().Init(true);
    PluginMgr::GetInstance().SetResTypeStrMap(ResExeType::resTypeToStr);
}

void ResSchedExeMgr::Stop()
{
    PluginMgr::GetInstance().Stop();
    PluginMgr::GetInstance().ClearResTypeStrMap();
}

int32_t ResSchedExeMgr::SendRequestSync(uint32_t resType, int64_t value,
    nlohmann::json& reply, const nlohmann::json& payload)
{
    // plugin sync dispatch resource function is coding...
    RSSEXE_LOGD("receive resType = %{public}u, value = %{public}lld.", resType, (long long)value);
    if (payload != nullptr) {
        std::string context = payload.dump(-1, ' ', false, nlohmann::detail::error_handler_t::replace);
        RSSEXE_LOGD("receive payload = %{private}s.", context.c_str());
    }
    reply["result"] = std::to_string(ResErrCode::RSSEXE_NO_ERR);
    return ResErrCode::RSSEXE_NO_ERR;
}

void ResSchedExeMgr::SendRequestAsync(uint32_t resType, int64_t value, const nlohmann::json& payload)
{
    RSSEXE_LOGD("receive resType = %{public}u, value = %{public}lld.", resType, (long long)value);
    std::string trace_str(__func__);
    trace_str.append(",resType[").append(std::to_string(resType)).append("]");
    trace_str.append(",value[").append(std::to_string(value)).append("]");
    StartTrace(HITRACE_TAG_OHOS, trace_str, -1);
    PluginMgr::GetInstance().DispatchResource(std::make_shared<ResData>(resType, value, payload));
    FinishTrace(HITRACE_TAG_OHOS);
}
} // namespace ResourceSchedule
} // namespace OHOS