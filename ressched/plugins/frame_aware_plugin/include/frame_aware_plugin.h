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

#ifndef FRAME_AWARE_PLUGIN_H
#define FRAME_AWARE_PLUGIN_H

#include "single_instance.h"
#include "plugin.h"

#include <set>
#include <vector>
#include <string>

namespace OHOS {
namespace ResourceSchedule {
class FrameAwarePlugin : public Plugin {
    DECLARE_SINGLE_INSTANCE(FrameAwarePlugin)

public:
    void Init() override;
    void Disable() override;
    void DispatchResource(const std::shared_ptr<ResData>& resData) override;
private:
    std::set<uint32_t> resTypes;
    std::vector<std::string> ParsePayload(const std::string payload);
};
} // namespace ResourceSchedule
} // namespace OHOS

#endif // RME_PLUGIN_H
