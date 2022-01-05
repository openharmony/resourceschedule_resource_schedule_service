/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: demo plugin.
 */

#ifndef FOUNDATION_RESOURCESCHEDULE_PLUGINS_DEMO_INCLUDE_PLUGIN_H
#define FOUNDATION_RESOURCESCHEDULE_PLUGINS_DEMO_INCLUDE_PLUGIN_H

#include "single_instance.h"
#include "plugin.h"

namespace OHOS {
namespace ResourceSchedule {
class DemoPlugin : public Plugin {
    DECLARE_SINGLE_INSTANCE(DemoPlugin)

public:
    void Init() override;

    void Disable() override;

    void DispatchResource(const std::shared_ptr<ResData>& resData) override;
};
} // namespace ResourceSchedule
} // namespace OHOS

#endif //FOUNDATION_RESOURCESCHEDULE_PLUGINS_DEMO_INCLUDE_PLUGIN_H
