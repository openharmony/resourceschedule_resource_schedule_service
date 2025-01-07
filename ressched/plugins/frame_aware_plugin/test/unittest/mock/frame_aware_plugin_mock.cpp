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

#include "frame_aware_plugin.h"
#include "res_type.h"

#undef LOG_TAG
#define LOG_TAG "ueaServer-FrameAwarePlugin"

namespace OHOS {
namespace ResourceSchedule {
using namespace ResType;

void FrameAwarePlugin::DispatchResource(const std::shared_ptr<ResData>& data)
{
    auto funcIter = functionMap.find(data->resType);
    if (funcIter != functionMap.end()) {
        data->value = 1;
        return;
    }
    data->value = 0;
    return;
}
}
}
