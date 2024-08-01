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

#ifndef RESSCHED_EXECUTOR_PLUGINS_FRAMEWORK_SOCPERF_EXECUTOR_PLUGIN_INCLUDE_SOCPERF_EXECUTOR_WIRTE_NODE_H
#define RESSCHED_EXECUTOR_PLUGINS_FRAMEWORK_SOCPERF_EXECUTOR_PLUGIN_INCLUDE_SOCPERF_EXECUTOR_WIRTE_NODE_H

#include <set>
#include <unordered_map>
#include "plugin.h"
#include "single_instance.h"
#include "socperf_executor_config.h"

namespace OHOS {
namespace ResourceSchedule {
enum RssExeEventType {
    SOCPERF_EVENT_IVALID = 0,
    SOCPERF_EVENT_WIRTE_NODE = 1,
};
class SocPerfExecutorWirteNode {
public:
    static SocPerfExecutorWirteNode& GetInstance();
    void InitThreadWraps();
    void WriteNodeThreadWraps(const std::vector<int32_t>& resIdVec, const std::vector<int64_t>& valueVec);
private:
    SocPerfConfig &socPerfConfig_ = SocPerfConfig::GetInstance();
    std::unordered_map<std::string, int32_t> fdInfo_;
private:
    SocPerfExecutorWirteNode();
    ~SocPerfExecutorWirteNode();
    void InitResourceNodeInfo(std::shared_ptr<ResourceNode> resourceNode);
    void UpdateResIdCurrentValue(int32_t resId, int64_t currValue);
    void UpdateCurrentValue(int32_t resId, int64_t currValue);
    void WriteNode(int32_t resId, const std::string& filePath, const std::string& value);
    int32_t GetFdForFilePath(const std::string& filePath);
};
} // namespace OHOS
} // namespace ResourceSchedule
#endif // RESSCHED_EXECUTOR_PLUGINS_FRAMEWORK_SOCPERF_EXECUTOR_PLUGIN_INCLUDE_SOCPERF_EXECUTOR_WIRTE_NODE_H