/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: ResSchedMgr class implement.
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