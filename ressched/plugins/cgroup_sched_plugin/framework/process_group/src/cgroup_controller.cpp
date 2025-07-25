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
#include <fcntl.h>               // for open, O_WRONLY
#include <cstddef>              // for size_t
#include <cstdint>               // for int32_t
#include <unistd.h>              // for close, TEMP_FAILURE_RETRY, access
#include <cerrno>                // for errno
#include <map>                   // for map
#include <string>                // for basic_string, operator+, to_string
#include <type_traits>           // for move
#include <utility>               // for pair
#include <vector>                // for vector
#include "cgroup_action.h"       // for CgroupAction
#include "cgroup_sched_log.h"
#include "process_group_util.h"  // for StringPrintf, GetRealPath, ReadFileT...
#include "res_common_util.h"

namespace OHOS {
namespace ResourceSchedule {
namespace CgroupSetting {
constexpr uint64_t SCHEDULE_CGROUP_FDSAN_TAG = 0xd001702;
constexpr uint64_t COMMON_CGROUP_FDSAN_TAG = 0;

CgroupController::CgroupController(const std::string& name, const std::string& path)
{
    name_ = name;
    path_ = path;
    auto policyList = CgroupAction::GetInstance().GetSchedPolicyList();
    for (SchedPolicy policy : policyList) {
        policyToTaskFd_[policy] = -1;
        policyToProcFd_[policy] = -1;
    }
}

CgroupController::~CgroupController()
{
    for (auto& kv : policyToTaskFd_) {
        if (kv.second != -1) {
            fdsan_close_with_tag(kv.second, SCHEDULE_CGROUP_FDSAN_TAG);
            kv.second = -1;
        }
    }
    policyToTaskFd_.clear();
    for (auto& kv : policyToProcFd_) {
        if (kv.second != -1) {
            fdsan_close_with_tag(kv.second, SCHEDULE_CGROUP_FDSAN_TAG);
            kv.second = -1;
        }
    }
    policyToProcFd_.clear();
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
        CGS_LOGE("%{public}s failed; fd = %{public}d", __func__, fd);
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
    int32_t len = static_cast<int32_t>(value.length());
    if (TEMP_FAILURE_RETRY(write(fd, value.c_str(), value.length())) == len) {
        return true;
    }
    /* If the thread is in the process of exiting, don't flag an error. */
    if (errno == ESRCH) {
        return true;
    }
    CGS_LOGE("%{public}s failed to write; fd = %{public}d, errno = %{public}d", __func__, fd, errno);
    return false;
}

bool CgroupController::AddSchedPolicy(SchedPolicy policy, const std::string& subgroup)
{
    return AddThreadSchedPolicy(policy, subgroup) && AddThreadGroupSchedPolicy(policy, subgroup);
}

bool CgroupController::GetTaskGroup(int tid, std::string& subgroup)
{
    std::string content;
    if (!ReadFileToStringForVFSFromExecutor(tid, content)) {
        CGS_LOGE("%{public}s: fail to read pid %{public}d cgroup proc", __func__, tid);
        return false;
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
    if (!ResCommonUtil::GetRealPath(filePath, realPath)) {
        return false;
    }
    int fd = TEMP_FAILURE_RETRY(open(realPath.c_str(), O_WRONLY | O_CLOEXEC));
    if (fd < 0) {
        CGS_LOGE("%{public}s open tasks file failed, name is %{public}s, subgroup is %{public}s",
            __func__, name_.c_str(), subgroup.c_str());
        return false;
    }
    fdsan_exchange_owner_tag(fd, COMMON_CGROUP_FDSAN_TAG, SCHEDULE_CGROUP_FDSAN_TAG);
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
    if (!ResCommonUtil::GetRealPath(filePath, realPath)) {
        return false;
    }
    int fd = TEMP_FAILURE_RETRY(open(realPath.c_str(), O_WRONLY | O_CLOEXEC));
    if (fd < 0) {
        CGS_LOGE("%{public}s open cgroup.procs file failed, name is %{public}s, subgroup is %{public}s",
            __func__, name_.c_str(), subgroup.c_str());
        return false;
    }
    fdsan_exchange_owner_tag(fd, COMMON_CGROUP_FDSAN_TAG, SCHEDULE_CGROUP_FDSAN_TAG);
    policyToProcFd_[policy] = fd;
    return true;
}
} // namespace CgroupSetting
} // namespace ResourceSchedule
} // namespace OHOS
