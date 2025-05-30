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

#include "socperf_executor_wirte_node.h"

#include <vector>
#include <string>
#include <unistd.h>          // for open, write
#include <fcntl.h>           // for O_RDWR, O_CLOEXEC
#include "socperf_log.h"

namespace OHOS {
namespace ResourceSchedule {
constexpr uint64_t SCHEDULE_CGROUP_FDSAN_TAG = 0xd001702;
constexpr uint64_t COMMON_CGROUP_FDSAN_TAG = 0;

SocPerfExecutorWirteNode& SocPerfExecutorWirteNode::GetInstance()
{
    static SocPerfExecutorWirteNode instance;
    return instance;
}

SocPerfExecutorWirteNode::SocPerfExecutorWirteNode() {}

SocPerfExecutorWirteNode::~SocPerfExecutorWirteNode()
{
    for (auto it = fdInfo_.begin(); it != fdInfo_.end(); ++it) {
        int fd = it->second;
        if (fd >= 0) {
            fdsan_close_with_tag(fd, SCHEDULE_CGROUP_FDSAN_TAG);
        }
    }
    fdInfo_.clear();
}

void SocPerfExecutorWirteNode::InitThreadWraps()
{
    socPerfConfig_.Init();
    for (auto iter = socPerfConfig_.resourceNodeInfo_.begin(); iter != socPerfConfig_.resourceNodeInfo_.end(); ++iter) {
        std::shared_ptr<ResourceNode> resourceNode = iter->second;
        InitResourceNodeInfo(resourceNode);
    }
}

void SocPerfExecutorWirteNode::InitResourceNodeInfo(std::shared_ptr<ResourceNode> resourceNode)
{
    if (resourceNode == nullptr) {
        return;
    }
    if (resourceNode->isGov) {
        std::shared_ptr<GovResNode> govResNode = std::static_pointer_cast<GovResNode>(resourceNode);
        for (int32_t i = 0; govResNode->persistMode != REPORT_TO_PERFSO &&
             i < (int32_t)govResNode->paths.size(); i++) {
            WriteNode(govResNode->id, govResNode->paths[i], govResNode->levelToStr[govResNode->def][i]);
            }
    } else {
        std::shared_ptr<ResNode> resNode = std::static_pointer_cast<ResNode>(resourceNode);
        if (resNode->persistMode != REPORT_TO_PERFSO) {
            WriteNode(resNode->id, resNode->path, std::to_string(resNode->def));
        }
    }
}

void SocPerfExecutorWirteNode::WriteNodeThreadWraps(const std::vector<int32_t>& resIdVec,
    const std::vector<int64_t>& valueVec)
{
    int32_t len = (int32_t)resIdVec.size();
    for (int32_t i = 0; i < len; i++) {
        UpdateResIdCurrentValue(resIdVec[i], valueVec[i]);
    }
}

void SocPerfExecutorWirteNode::UpdateResIdCurrentValue(int32_t resId, int64_t currValue)
{
    if (currValue == NODE_DEFAULT_VALUE) {
        UpdateCurrentValue(resId, socPerfConfig_.resourceNodeInfo_[resId]->def);
    } else {
        UpdateCurrentValue(resId, currValue);
    }
}

void SocPerfExecutorWirteNode::UpdateCurrentValue(int32_t resId, int64_t currValue)
{
    if (socPerfConfig_.IsGovResId(resId)) {
        std::shared_ptr<GovResNode> govResNode =
            std::static_pointer_cast<GovResNode>(socPerfConfig_.resourceNodeInfo_[resId]);
        if (govResNode->persistMode != REPORT_TO_PERFSO &&
            govResNode->levelToStr.find(currValue) != govResNode->levelToStr.end()) {
            std::vector<std::string> targetStrs = govResNode->levelToStr[currValue];
            for (int32_t i = 0; i < (int32_t)govResNode->paths.size(); i++) {
                WriteNode(resId, govResNode->paths[i], targetStrs[i]);
            }
        }
    } else {
        std::shared_ptr<ResNode> resNode = std::static_pointer_cast<ResNode>(socPerfConfig_.resourceNodeInfo_[resId]);
        WriteNode(resId, resNode->path, std::to_string(currValue));
    }
}

void SocPerfExecutorWirteNode::WriteNode(int32_t resId, const std::string& filePath, const std::string& value)
{
    if (socPerfConfig_.resourceNodeInfo_[resId]->persistMode == REPORT_TO_PERFSO) {
        return;
    }
    int32_t fd = GetFdForFilePath(filePath);
    if (fd < 0) {
        return;
    }
    write(fd, value.c_str(), value.size());
}

int32_t SocPerfExecutorWirteNode::GetFdForFilePath(const std::string& filePath)
{
    if (fdInfo_.find(filePath) != fdInfo_.end()) {
        return fdInfo_[filePath];
    }
    char path[PATH_MAX + 1] = {0};
    if (filePath.size() == 0 || filePath.size() > PATH_MAX || !realpath(filePath.c_str(), path)) {
        SOC_PERF_LOGE("GetFdForFilePath filePath failed %{public}s", filePath.c_str());
        return -1;
    }
    int32_t fd = open(path, O_RDWR | O_CLOEXEC);
    if (fd < 0) {
        SOC_PERF_LOGE("GetFdForFilePath fd failed %{public}s %{public}d", filePath.c_str(), fd);
        return fd;
    }
    fdsan_exchange_owner_tag(fd, COMMON_CGROUP_FDSAN_TAG, SCHEDULE_CGROUP_FDSAN_TAG);
    fdInfo_.insert(std::pair<std::string, int32_t>(filePath, fd));
    return fdInfo_[filePath];
}

} // namespace OHOS
} // namespace ResourceSchedule