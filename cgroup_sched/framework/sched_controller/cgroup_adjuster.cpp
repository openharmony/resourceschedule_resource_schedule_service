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

#include "cgroup_adjuster.h"

#include <unistd.h>
#include "app_mgr_constants.h"
#include "cgroup_event_handler.h"
#include "cgroup_sched_common.h"
#include "cgroup_sched_log.h"
#include "hitrace_meter.h"
#include "sched_controller.h"
#include "ressched_utils.h"
#include "res_type.h"
#include "wm_common.h"

namespace OHOS {
namespace ResourceSchedule {
namespace {
    constexpr HiviewDFX::HiLogLabel LOG_LABEL = {LOG_CORE, LOG_TAG_DOMAIN_ID_RMS, "CgroupAdjuster"};
}

using OHOS::AppExecFwk::ApplicationState;
using OHOS::AppExecFwk::AbilityState;
using OHOS::AppExecFwk::ExtensionState;
using OHOS::Rosen::WindowType;

CgroupAdjuster& CgroupAdjuster::GetInstance()
{
    static CgroupAdjuster instance;
    return instance;
}

void CgroupAdjuster::InitAdjuster()
{
    // Trigger load shared library
    (void)ResSchedUtils::GetInstance();
    auto handler = SchedController::GetInstance().GetCgroupEventHandler();
    if (handler) {
        handler->PostTask([this] {
            this->AdjustSelfProcessGroup();
        });
    }
}

void CgroupAdjuster::AdjustProcessGroup(Application &app, ProcessRecord &pr, AdjustSource source)
{
    CGS_LOGI("%{public}s for %{public}d, source : %{public}d", __func__, pr.GetPid(), source);
    ComputeProcessGroup(app, pr, source);
    ApplyProcessGroup(app, pr);
    ResSchedUtils::GetInstance().ReportArbitrationResult(app, pr, source);
}

void CgroupAdjuster::AdjustAllProcessGroup(Application &app, AdjustSource source)
{
    auto pidsMap = app.GetPidsMap();
    for (auto iter = pidsMap.begin(); iter != pidsMap.end(); iter++) {
        auto pr = iter->second;
        if (pr.get()) {
            AdjustProcessGroup(app, *(pr.get()), source);
        }
    }
}

inline void CgroupAdjuster::AdjustSelfProcessGroup()
{
    int pid = getpid();
    int group = SP_FOREGROUND;
    int ret = CgroupSetting::SetThreadGroupSchedPolicy(pid, group);
    if (ret != 0) {
        CGS_LOGE("%{public}s set %{public}d to group %{public}d failed, ret=%{public}d!", __func__, pid, group, ret);
    }
}

void CgroupAdjuster::ComputeProcessGroup(Application &app, ProcessRecord &pr, AdjustSource source)
{
    SchedPolicy group = SP_DEFAULT;

    {
        ChronoScope cs("ComputeProcessGroup");
        if (source == AdjustSource::ADJS_PROCESS_CREATE) {
            group = SP_DEFAULT;
        } else if (app.focusedProcess_) {
            group = SP_TOP_APP;
        } else {
            if (pr.abilities_.size() == 0) {
                group = SP_DEFAULT;
                if (app.state_ == (int32_t)ApplicationState::APP_STATE_BACKGROUND) {
                    group = SP_BACKGROUND;
                }
            } else if (pr.IsVisible()) {
                group = SP_FOREGROUND;
            } else if (pr.HasServiceExtension()) {
                group = SP_DEFAULT;
                if (app.state_ == (int32_t)ApplicationState::APP_STATE_BACKGROUND) {
                    group = SP_BACKGROUND;
                }
            } else {
                if (app.state_ == (int32_t)ApplicationState::APP_STATE_BACKGROUND) {
                    group = SP_BACKGROUND;
                } else if (app.state_ == (int32_t)ApplicationState::APP_STATE_FOREGROUND) {
                    group = SP_FOREGROUND;
                } else {
                    group = SP_DEFAULT;
                }
            }
        }
        pr.setSchedGroup_ = group;
    } // end ChronoScope
}

void CgroupAdjuster::ApplyProcessGroup(Application &app, ProcessRecord &pr)
{
    ChronoScope cs("ApplyProcessGroup");
    if (pr.curSchedGroup_ != pr.setSchedGroup_) {
        pid_t pid = pr.GetPid();
        int ret = CgroupSetting::SetThreadGroupSchedPolicy(pid, (int)pr.setSchedGroup_);
        if (ret != 0) {
            CGS_LOGE("%{public}s set %{public}d to group %{public}d failed, ret=%{public}d!",
                __func__, pid, pr.setSchedGroup_, ret);
            return;
        }

        pr.lastSchedGroup_ = pr.curSchedGroup_;
        pr.curSchedGroup_ = pr.setSchedGroup_;
        CGS_LOGI("%{public}s Set %{public}d's cgroup from %{public}d to %{public}d.",
            __func__, pr.GetPid(), pr.lastSchedGroup_, pr.curSchedGroup_);

        std::string traceStr(__func__);
        traceStr.append(" for ").append(std::to_string(pid)).append(", group change from ")
            .append(std::to_string((int32_t)(pr.lastSchedGroup_))).append(" to ")
            .append(std::to_string((int32_t)(pr.curSchedGroup_)));
        StartTrace(HITRACE_TAG_OHOS, traceStr);

        nlohmann::json payload;
        payload["pid"] = std::to_string(pr.GetPid());
        payload["uid"] = std::to_string(pr.GetUid());
        payload["name"] = app.name_;
        payload["oldGroup"] = std::to_string((int32_t)(pr.lastSchedGroup_));
        payload["newGroup"] = std::to_string((int32_t)(pr.curSchedGroup_));
        ResSchedUtils::GetInstance().ReportDataInProcess(ResType::RES_TYPE_CGROUP_ADJUSTER, 0, payload);

        FinishTrace(HITRACE_TAG_OHOS);
    }
}
} // namespace ResourceSchedule
} // namespace OHOS
