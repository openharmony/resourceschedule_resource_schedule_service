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

#ifndef EFFICIENCY_PLUGIN_H
#define EFFICIENCY_PLUGIN_H

#include <set>

#include "json/json.h"

#include "plugin.h"
#include "single_instance.h"

namespace OHOS {
namespace ResourceSchedule {
class EfficiencyPlugin : public Plugin {
    DECLARE_SINGLE_INSTANCE(EfficiencyPlugin)

public:
    void Init() override;
    void Disable() override;
    void DispatchResource(const std::shared_ptr<ResData>& resData) override;

private:
    std::set<uint32_t> resTypes;
};
} // namespace ResourceSchedule
} // namespace OHOS
#endif // EFFICIENCY_PLUGIN_H