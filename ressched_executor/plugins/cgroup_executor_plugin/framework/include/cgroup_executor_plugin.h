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

#ifndef CGROUP_EXECUTOR_PLUGIN_FRAMEWORK_INCLUDE_CGROUP_EXECUTOR_PLUGIN_H
#define CGROUP_EXECUTOR_PLUGIN_FRAMEWORK_INCLUDE_CGROUP_EXECUTOR_PLUGIN_H

#include <set>
#include <unordered_map>
#include "plugin.h"
#include "single_instance.h"

namespace OHOS {
namespace ResourceSchedule {
namespace CgroupSetting {
class CgroupExecutorPlugin : public Plugin {
    DECLARE_SINGLE_INSTANCE(CgroupExecutorPlugin)
public:
    void Init() override;
    void Disable() override;
    void DispatchResource(const std::shared_ptr<ResData>& resData) override;
    int32_t DeliverResource(const std::shared_ptr<ResData>& resData) override;
private:
    std::set<uint32_t> resTypes_;
    bool ParseValue(int32_t& value, const char* name, const nlohmann::json& payload);
};
} // namespace CgroupSetting
} // namespace OHOS
} // namespace ResourceSchedule
#endif // CGROUP_EXECUTOR_PLUGIN_FRAMEWORK_INCLUDE_CGROUP_EXECUTOR_PLUGIN_H