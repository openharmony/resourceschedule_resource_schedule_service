/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: base config info of plugin.
 */

#ifndef FOUNDATION_RESOURCESCHEDULE_SERVICES_RESSCHEDMGR_PLUGINBASE_INCLUDE_PLUGIN_H
#define FOUNDATION_RESOURCESCHEDULE_SERVICES_RESSCHEDMGR_PLUGINBASE_INCLUDE_PLUGIN_H

#include <cstdint>
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

#endif // FOUNDATION_RESOURCESCHEDULE_SERVICES_RESSCHEDMGR_PLUGINBASE_INCLUDE_PLUGIN_H