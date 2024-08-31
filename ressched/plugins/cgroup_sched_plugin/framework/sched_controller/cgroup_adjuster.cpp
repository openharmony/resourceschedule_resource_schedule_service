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
#include <fcntl.h>
#include <cerrno>
#include <string>
#include "app_mgr_constants.h"
#include "cgroup_event_handler.h"
#include "cgroup_sched_common.h"
#include "cgroup_sched_log.h"
#include "hitrace_meter.h"
#include "sched_controller.h"
#include "ressched_utils.h"
#include "res_type.h"
#include "wm_common.h"

#undef LOG_TAG
#define LOG_TAG "CgroupAdjuster"

namespace OHOS {
namespace ResourceSchedule {
using OHOS::AppExecFwk::ApplicationState;
using OHOS::AppExecFwk::AbilityState;
using OHOS::AppExecFwk::ExtensionState;
using OHOS::Rosen::WindowType;
constexpr uint32_t MAX_SIZE = 4096;

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

void CgroupAdjuster::AdjustForkProcessGroup(Application &app, ProcessRecord &pr)
{
    std::string filePath = ResSchedUtils::GetInstance().GetProcessFilePath(app.GetUid(), app.GetName(), pr.GetPid());
    int fd = open(filePath.c_str(), O_RDONLY);
    if (fd < 0) {
        CGS_LOGD("%{public}s File is not opened, error is %{public}s.",
            __func__, strerror(errno));
        return;
    }
    char fileContent[MAX_SIZE] = {0};
    int rd = read(fd, fileContent, sizeof(fileContent));
    if (rd < 0) {
        CGS_LOGE("%{public}s Read File Error, error is %{public}s.",
            __func__, strerror(errno));
    }
    const char *flag = "\n";
    char *line = strtok(fileContent, flag);
    while (line != NULL) {
        int32_t forkPid = std::stoi(line);
        line = strtok(NULL, flag);
        if (forkPid != pr.GetPid()) {
            int ret = CgroupSetting::SetThreadGroupSchedPolicy(forkPid, pr.curSchedGroup_);
            if (ret != 0) {
                CGS_LOGE("%{public}s set %{public}d to group %{public}d failed, ret = %{public}d!",
                    __func__, forkPid, (int)pr.curSchedGroup_, ret);
            }
        } else {
            continue;
        }
    }
    close(fd);
    return;
}

void CgroupAdjuster::AdjustProcessGroup(Application &app, ProcessRecord &pr, AdjustSource source)
{
    CGS_LOGI("%{public}s for %{public}d, source : %{public}d", __func__, pr.GetPid(), source);
    ComputeProcessGroup(app, pr, source);
    ResSchedUtils::GetInstance().ReportArbitrationResult(app, pr, source);
    ApplyProcessGroup(app, pr);

    if (!app.IsHostProcess(pr.GetPid())) {
        return;
    }

    /* Let the sched group of render process follow the sched group of host process */
    for (const auto &iter : app.GetPidsMap()) {
        const auto &procRecord = iter.second;
        if (procRecord && ((procRecord->processType_ == ProcRecordType::RENDER) ||
            (procRecord->processType_ == ProcRecordType::GPU))) {
            auto hostProcRecord = app.GetProcessRecord(procRecord->hostPid_);
            if (!hostProcRecord || (procRecord->hostPid_ != pr.GetPid())) {
                continue;
            }
            CGS_LOGD("%{public}s for %{public}d, source : %{public}d for render process",
                __func__, procRecord->GetPid(), source);
            procRecord->setSchedGroup_ = hostProcRecord->curSchedGroup_;
            if (procRecord->processType_ == ProcRecordType::RENDER ||
                ((procRecord->processType_ == ProcRecordType::GPU) && (hostProcRecord->curSchedGroup_ == SP_TOP_APP))) {
                CGS_LOGI("%{public}s for %{public}d, source : %{public}d for render process",
                    __func__, procRecord->GetPid(), AdjustSource::ADJS_SELF_RENDER_THREAD);
                ResSchedUtils::GetInstance().ReportArbitrationResult(app, *(procRecord.get()),
                    AdjustSource::ADJS_SELF_RENDER_THREAD);
            }
            ApplyProcessGroup(app, *procRecord);
        }
    }
}

void CgroupAdjuster::AdjustAllProcessGroup(Application &app, AdjustSource source)
{
    for (auto &iter : app.GetPidsMap()) {
        const auto &procRecord = iter.second;
        if (procRecord && (procRecord->processType_ != ProcRecordType::RENDER) &&
            (procRecord->processType_ != ProcRecordType::GPU) &&
            (procRecord->processType_ != ProcRecordType::LINUX)) {
            AdjustProcessGroup(app, *procRecord, source);
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
        if (pr.processType_ == ProcRecordType::RENDER) {
            auto hostProcRecord = app.GetProcessRecord(pr.hostPid_);
            group = hostProcRecord ? hostProcRecord->curSchedGroup_ : SP_DEFAULT;
        } else if (source == AdjustSource::ADJS_PROCESS_CREATE) {
            group = SP_DEFAULT;
        } else if (app.focusedProcess_ && (app.focusedProcess_->GetPid() == pr.GetPid())) {
            group = SP_TOP_APP;
        } else {
            if (pr.abilities_.size() == 0) {
                group = SP_DEFAULT;
                if (pr.processState_ == (int32_t)ApplicationState::APP_STATE_BACKGROUND) {
                    group = SP_BACKGROUND;
                }
            } else if (pr.IsVisible()) {
                group = SP_FOREGROUND;
            } else if (pr.HasServiceExtension()) {
                group = SP_DEFAULT;
                if (pr.processState_ == (int32_t)ApplicationState::APP_STATE_BACKGROUND) {
                    group = SP_BACKGROUND;
                }
            } else {
                if (pr.processState_ == (int32_t)ApplicationState::APP_STATE_BACKGROUND) {
                    group = SP_BACKGROUND;
                } else if (pr.processState_ == (int32_t)ApplicationState::APP_STATE_FOREGROUND) {
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
        payload["name"] = app.GetName();
        payload["oldGroup"] = std::to_string((int32_t)(pr.lastSchedGroup_));
        payload["newGroup"] = std::to_string((int32_t)(pr.curSchedGroup_));
        ResSchedUtils::GetInstance().ReportDataInProcess(ResType::RES_TYPE_CGROUP_ADJUSTER, 0, payload);
        AdjustForkProcessGroup(app, pr);
        FinishTrace(HITRACE_TAG_OHOS);
    }
}
} // namespace ResourceSchedule
} // namespace OHOS
