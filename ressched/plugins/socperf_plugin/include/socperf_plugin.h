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

#ifndef RESSCHED_PLUGINS_SOCPERF_PLUGIN_INCLUDE_SOCPERF_PLUGIN_H
#define RESSCHED_PLUGINS_SOCPERF_PLUGIN_INCLUDE_SOCPERF_PLUGIN_H

#include <set>
#include <unordered_map>
#include "plugin.h"
#include "single_instance.h"
#include "app_mgr_constants.h"
#include "socperf_client.h"

namespace OHOS {
namespace ResourceSchedule {
class SocPerfPlugin : public Plugin {
    DECLARE_SINGLE_INSTANCE(SocPerfPlugin)

public:
    void Init() override;

    void Disable() override;

    void DispatchResource(const std::shared_ptr<ResData>& resData) override;

private:
    std::set<uint32_t> resTypes;
    std::unordered_map<uint32_t, std::function<void(const std::shared_ptr<ResData>& data)>> functionMap;
    void HandleWindowFocus(const std::shared_ptr<ResData>& data);
    void HandleEventClick(const std::shared_ptr<ResData>& data);
    void HandlePushPage(const std::shared_ptr<ResData>& data);
    void HandlePopPage(const std::shared_ptr<ResData>& data);
    void HandleEventSlide(const std::shared_ptr<ResData>& data);
    void HandleEventWebGesture(const std::shared_ptr<ResData>& data);
    void HandleAppAbilityStart(const std::shared_ptr<ResData>& data);
    void HandleResizeWindow(const std::shared_ptr<ResData>& data);
    void HandleMoveWindow(const std::shared_ptr<ResData>& data);
    void HandleSlideNormal(const std::shared_ptr<ResData>& data);
};
} // namespace ResourceSchedule
} // namespace OHOS

#endif // RESSCHED_PLUGINS_SOCPERF_PLUGIN_INCLUDE_SOCPERF_PLUGIN_H
