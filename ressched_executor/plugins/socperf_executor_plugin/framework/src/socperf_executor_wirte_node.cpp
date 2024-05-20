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

#include "socperf_executor_wirte_node.h"

#include <vector>
#include <string>
#include <unistd.h>          // for open, write
#include <fcntl.h>           // for O_RDWR, O_CLOEXEC
#include "res_sched_exe_log.h"

namespace OHOS {
namespace ResourceSchedule {

SocPerfExecutorWirteNode& SocPerfExecutorWirteNode::GetInstance()
{
    static SocPerfExecutorWirteNode instance;
    return instance;
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

void SocPerfExecutorWirteNode::WriteNodeThreadWraps(std::vector<int32_t>& resIdVec, std::vector<int64_t>& valueVec)
{
    size len = qosId.size();
    for (size i = 0; i < len; i++) {
        UpdateCurrentValue(qosId[i], value[i]);
    }
}

void SocPerfExecutorWirteNode::UpdateCurrentValue(int32_t resId, int64_t currValue)
{
    if (socPerfConfig_.IsGovResId(resId)) {
        std::shared_ptr<GovResNode> govResNode =
            std::static_pointer_cast<GovResNode>(socPerfConfig_.resourceNodeInfo_[resId]);
        if (govResNode->persistMode != REPORT_TO_PERFSO &&
            govResNode->levelToStr.find(currValue) != govResNode->levelToStr.end()) {
            std::vector<std::string> targetStrs = govResNode->levelToStr[resStatusInfo_[resId]->currentValue];
            for (int32_t i = 0; i < (int32_t)govResNode->paths.size(); i++) {
                WriteNode(resId, govResNode->paths[i], targetStrs[i]);
            }
        }
    } else {
        std::shared_ptr<ResNode> resNode = std::static_pointer_cast<ResNode>(socPerfConfig_.resourceNodeInfo_[resId]);
        WriteNode(resId, resNode->path, std::to_string(resStatusInfo_[resId]->currentValue));
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
        return -1;
    }
    int32_t fd = open(path, O_RDWR | O_CLOEXEC);
    if (fd < 0) {
        return fd;
    }
    fdInfo_.insert(std::pair<std::string, int32_t>(filePath, fd));
    return fdInfo_[filePath];
}

} // namespace OHOS
} // namespace ResourceSchedule