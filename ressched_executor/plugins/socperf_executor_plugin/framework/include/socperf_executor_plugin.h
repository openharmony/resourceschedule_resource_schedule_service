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

#ifndef RESSCHED_EXECUTOR_PLUGINS_FRAMEWORK_SOCPERF_EXECUTOR_PLUGIN_INCLUDE_SOCPERF_EXECUTOR_PLUGIN_H
#define RESSCHED_EXECUTOR_PLUGINS_FRAMEWORK_SOCPERF_EXECUTOR_PLUGIN_INCLUDE_SOCPERF_EXECUTOR_PLUGIN_H

#include <set>
#include <unordered_map>
#include "plugin.h"
#include "single_instance.h"

namespace OHOS {
namespace ResourceSchedule {
class SocPerfExecutorPlugin : public Plugin {
    DECLARE_SINGLE_INSTANCE(SocPerfExecutorPlugin)

public:
    void Init() override;
    void Disable() override;
    void DispatchResource(const std::shared_ptr<ResData>& resData) override;
private:
    void HandleSocperfWirteNode(const std::shared_ptr<ResData> &data);
    void SocPerfInitNode();
    void SocPerfWirteNode(const std::shared_ptr<ResData> &data);
private:
    std::set<uint32_t> resType_;
    std::unordered_map<uint32_t, std::function<void(const std::shared_ptr<ResData>& data)>> functionMap_;
};
} // namespace OHOS
} // namespace ResourceSchedule
#endif // RESSCHED_EXECUTOR_PLUGINS_FRAMEWORK_SOCPERF_EXECUTOR_PLUGIN_INCLUDE_SOCPERF_EXECUTOR_PLUGIN_H