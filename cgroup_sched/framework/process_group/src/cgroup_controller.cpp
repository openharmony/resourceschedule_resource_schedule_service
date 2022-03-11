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
#include "cgroup_controller.h"

#include <cerrno>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <unistd.h>
#include "process_group_macro.h"
#include "process_group_log.h"
#include "process_group_util.h"

namespace OHOS {
namespace ResourceSchedule {
namespace CgroupSetting {
CgroupController::CgroupController(const std::string& name, const std::string& path)
    : name_(name), path_(path), policyToTaskFd_(SP_CNT, -1), policyToProcFd_(SP_CNT, -1) {}

CgroupController::~CgroupController()
{
    for (auto& fd : policyToTaskFd_) {
        if (fd != -1) {
            close(fd);
            fd = -1;
        }
    }
    for (auto& fd : policyToProcFd_) {
        if (fd != -1) {
            close(fd);
            fd = -1;
        }
    }
}

CgroupController::CgroupController(CgroupController&& controller)
    : name_(std::move(controller.name_)), path_(std::move(controller.path_)),
    policyToTaskFd_(std::move(controller.policyToTaskFd_)), policyToProcFd_(std::move(controller.policyToProcFd_)) {}

CgroupController& CgroupController::operator=(CgroupController&& controller)
{
    name_ = std::move(controller.name_);
    path_ = std::move(controller.path_);
    policyToTaskFd_ = std::move(controller.policyToTaskFd_);
    policyToProcFd_ = std::move(controller.policyToProcFd_);
    return *this;
}

bool CgroupController::IsEnabled()
{
    std::string filePath(path_ + "/tasks");
    static bool enabled = !access(filePath.c_str(), F_OK);
    return enabled;
}

bool CgroupController::SetThreadSchedPolicy(int tid, SchedPolicy policy, bool isSetThreadGroup)
{
    int fd = (isSetThreadGroup ? policyToProcFd_[policy] : policyToTaskFd_[policy]);
    if (fd < 0) {
        PGCGS_LOGI("SetThreadSchedPolicy failed; fd = %{public}d", fd);
        errno = EINVAL;
        return false;
    }
    if (!AddTidToCgroup(tid, fd)) {
        return false;
    }
    return true;
}

bool CgroupController::AddTidToCgroup(int tid, int fd)
{
    std::string value = std::to_string(tid);
    if (TEMP_FAILURE_RETRY(write(fd, value.c_str(), value.length())) == value.length()) {
        return true;
    }
    /* If the thread is in the process of exiting, don't flag an error. */
    if (errno == ESRCH) {
        return true;
    }
    PGCGS_LOGE("AddTidToCgroup failed to write; fd = %{public}d, errno = %{public}d", fd, errno);
    return false;
}

bool CgroupController::AddSchedPolicy(SchedPolicy policy, const std::string& subgroup)
{
    return AddThreadSchedPolicy(policy, subgroup) && AddThreadGroupSchedPolicy(policy, subgroup);
}

bool CgroupController::GetTaskGroup(int tid, std::string& subgroup)
{
    std::string content;
    std::string filePath = StringPrintf("/proc/%d/cgroup", tid);
    if (!ReadFileToStringForVFS(filePath, content)) {
        PGCGS_LOGE("GetTaskGroup: fail to read  = %{public}s", filePath.c_str());
        return -1;
    }
    std::string cgTag = StringPrintf(":%s:", name_.c_str());
    size_t startPos = content.find(cgTag);
    if (startPos == std::string::npos) {
        return false;
    }
    startPos += cgTag.length() + 1;
    size_t endPos = content.find('\n', startPos);
    if (endPos == std::string::npos) {
        subgroup = content.substr(startPos, std::string::npos);
    } else {
        subgroup = content.substr(startPos, endPos - startPos);
    }
    return true;
}

bool CgroupController::AddThreadSchedPolicy(SchedPolicy policy, const std::string& subgroup)
{
    std::string filePath;
    if (subgroup.empty()) {
        filePath = StringPrintf("%s/tasks", path_.c_str());
    } else {
        filePath = StringPrintf("%s/%s/tasks", path_.c_str(), subgroup.c_str());
    }
    std::string realPath;
    if (!GetRealPath(filePath, realPath)) {
        return false;
    }
    int fd = TEMP_FAILURE_RETRY(open(realPath.c_str(), O_WRONLY | O_CLOEXEC));
    if (fd < 0) {
        PGCGS_LOGE("AddThreadSchedPolicy open file failed; file = %{public}s, fd = %{public}d ",
            realPath.c_str(), fd);
        return false;
    }
    policyToTaskFd_[policy] = fd;
    return true;
}

bool CgroupController::AddThreadGroupSchedPolicy(SchedPolicy policy, const std::string& subgroup)
{
    std::string filePath;
    if (subgroup.empty()) {
        filePath = StringPrintf("%s/cgroup.procs", path_.c_str());
    } else {
        filePath = StringPrintf("%s/%s/cgroup.procs", path_.c_str(), subgroup.c_str());
    }
    std::string realPath;
    if (!GetRealPath(filePath, realPath)) {
        return false;
    }
    int fd = TEMP_FAILURE_RETRY(open(realPath.c_str(), O_WRONLY | O_CLOEXEC));
    if (fd < 0) {
        PGCGS_LOGE("AddThreadGroupSchedPolicy open file failed; file = %{public}s'; fd = %{public}d ",
            realPath.c_str(), fd);
        return false;
    }
    policyToProcFd_[policy] = fd;
    return true;
}
} // namespace CgroupSetting
} // namespace ResourceSchedule
} // namespace OHOS
