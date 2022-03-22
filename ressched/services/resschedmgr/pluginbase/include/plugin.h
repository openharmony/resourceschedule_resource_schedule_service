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

#ifndef RESSCHED_SERVICES_RESSCHEDMGR_PLUGINBASE_INCLUDE_PLUGIN_H
#define RESSCHED_SERVICES_RESSCHEDMGR_PLUGINBASE_INCLUDE_PLUGIN_H

#include "config_info.h"
#include "res_data.h"

namespace OHOS {
namespace ResourceSchedule {
class Plugin {
public:
    virtual ~Plugin() = default;

    /**
     * when load plugin, init it
     */
    virtual void Init() = 0;

    /**
     * Disable the plugin when resource schedule service stopped.
     */
    virtual void Disable() = 0;

    /**
     * Dispatch resource data to plugin has subscribed resid.
     *
     * @param data the resource interested.
     */
    virtual void DispatchResource(const std::shared_ptr<ResData>& data) = 0;
};
} // namespace ResourceSchedule
} // namespace OHOS

#endif // RESSCHED_SERVICES_RESSCHEDMGR_PLUGINBASE_INCLUDE_PLUGIN_H
