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

#include "res_sched_mgr.h"
#include "plugin_mgr.h"

namespace OHOS {
namespace ResourceSchedule {
using namespace AppExecFwk;

namespace {
    const std::string RMS_THREAD_NAME = "rmsMain";
}

IMPLEMENT_SINGLE_INSTANCE(ResSchedMgr);

void ResSchedMgr::Init()
{
    PluginMgr::GetInstance().Init();

    if (mainHandler_ == nullptr) {
        mainHandler_ = std::make_shared<EventHandler>(EventRunner::Create(RMS_THREAD_NAME));
    }
}

void ResSchedMgr::Stop()
{
    PluginMgr::GetInstance().Stop();

    if (mainHandler_ != nullptr) {
        mainHandler_->RemoveAllEvents();
        mainHandler_ = nullptr;
    }
}

void ResSchedMgr::ReportData(uint32_t resType, int64_t value, const std::string& payload)
{
    if (mainHandler_ == nullptr) {
        return;
    }
    // dispatch resource async
    mainHandler_->PostTask([resType, value, payload] {
        PluginMgr::GetInstance().DispatchResource(std::make_shared<ResData>(resType, value, payload));
    });
}

} // namespace ResourceSchedule
} // namespace OHOS