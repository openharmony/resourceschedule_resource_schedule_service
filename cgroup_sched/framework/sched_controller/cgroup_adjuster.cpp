/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

void CgroupAdjuster::InitAdjuster()
{
    auto handler = SchedController::GetInstance().GetCgroupEventHandler();
    if (handler != nullptr) {
        handler->PostTask([this] {
            this->AdjustSelfProcessGroup();
        });
    }
}

void CgroupAdjuster::AdjustProcessGroup(Application &app, ProcessRecord &pr, AdjustSource source)
{
    CGS_LOGI("%{public}s for %{public}d, source : %{public}d", __func__, pr.GetPid(), source);
    ComputeProcessGroup(app, pr, source);
    ApplyProcessGroup(pr);
}

void CgroupAdjuster::AdjustAllProcessGroup(Application &app, AdjustSource source)
{
    auto pidsMap = app.GetPidsMap();
    for (auto iter = pidsMap.begin(); iter != pidsMap.end(); iter++) {
        auto pr = iter->second;
        AdjustProcessGroup(app, *(pr.get()), source);
    }
}

inline void CgroupAdjuster::AdjustSelfProcessGroup()
{
    int pid = getpid();
    int group = (int)(CgroupSetting::SchedPolicy::SP_FOREGROUND);
    int ret = CgroupSetting::SetThreadGroupSchedPolicy(pid, group);
    if (ret != 0) {
        CGS_LOGE("%{public}s set %{public}d to group %{public}d failed, ret=%{public}d!", __func__, pid, group, ret);
    }
}

void CgroupAdjuster::ComputeProcessGroup(Application &app, ProcessRecord &pr, AdjustSource source)
{
    int32_t appState = app.state_;
    SchedPolicy group = SchedPolicy::SP_DEFAULT;
    auto focusProcess = app.focusedProcess_;

    {
        ChronoScope cs("ComputeProcessGroup");
        if (source == AdjustSource::ADJS_PROCESS_CREATE) {
            group = SchedPolicy::SP_DEFAULT;
        } else if (app.focusedProcess_ != nullptr) {
            group = SchedPolicy::SP_TOP_APP;
            if (pr.windowType_ == VALUE_INT(WindowType::WINDOW_TYPE_FLOAT)) {
                group = SchedPolicy::SP_FOREGROUND; // float window process --> fg
            }
        } else if (appState == VALUE_INT(ApplicationState::APP_STATE_FOREGROUND)) {
            group = GetCgroupForAbilityState(pr); // foreground app state from ability
        } else if (appState == VALUE_INT(ApplicationState::APP_STATE_BACKGROUND)) {
            group = SchedPolicy::SP_BACKGROUND; // background app state -> bg
        } else {
            group = GetCgroupForAbilityState(pr); // others: decide by ability state and extension state
        }

        if (group == SchedPolicy::SP_BACKGROUND && pr.runningContinuousTask_) {
            group = SchedPolicy::SP_FOREGROUND; // move background key task to fg
        }

        pr.setSchedGroup_ = group;
    } // end ChronoScope
}

SchedPolicy CgroupAdjuster::GetCgroupForAbilityState(ProcessRecord &pr)
{
    auto abilities = pr.GetAbilities();
    int total = abilities.size();
    int countMixAbi = 0; // has both ability & extension state
    int countActiveAbi = 0; // fg ability, connected extension, visible ability etc.
    int countInactiveAbi = 0; // others
    if (total == 0) {
        return SchedPolicy::SP_DEFAULT;
    }

    // visible ability set to fg directly
    for (auto abi : abilities) {
        if (abi->state_ >= 0 && abi->estate_ >= 0) {
            if (abi->estate_ == VALUE_INT(ExtensionState::EXTENSION_STATE_CONNECTED)) {
                countActiveAbi++;
            } else {
                countMixAbi++;
            }
        } else if (abi->state_ == VALUE_INT(AbilityState::ABILITY_STATE_FOREGROUND)
            || abi->estate_ == VALUE_INT(ExtensionState::EXTENSION_STATE_CONNECTED)) {
            countActiveAbi++;
        } else {
            countInactiveAbi++;
        }
    }
    if (countMixAbi == total) {
        // api7 cause mix ability/extension state
        return SchedPolicy::SP_DEFAULT;
    }
    if (countInactiveAbi == 0) {
        // sp_foreground : all abilitites are FOREGROUND or has a extension connected
        return SchedPolicy::SP_FOREGROUND;
    }
    if (countActiveAbi == 0) {
        // sp_background : all abilities are BACKGROUND
        return SchedPolicy::SP_BACKGROUND;
    }
    return SchedPolicy::SP_DEFAULT;
}

void CgroupAdjuster::ApplyProcessGroup(ProcessRecord &pr)
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

        std::string payload = std::to_string(pr.GetPid()) + "," +
                std::to_string(pr.GetUid()) + "," +
                pr.GetName() + "," +
                std::to_string(VALUE_INT(pr.lastSchedGroup_)) + "," +
                std::to_string(VALUE_INT(pr.curSchedGroup_));
        ResSchedUtils::GetInstance().ReportDataInProcess(ResType::RES_TYPE_CGROUP_ADJUSTER, 0, payload);
    }
}
} // namespace ResourceSchedule
} // namespace OHOS
