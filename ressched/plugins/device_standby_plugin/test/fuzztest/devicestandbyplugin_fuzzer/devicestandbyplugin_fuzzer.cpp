/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "devicestandbyplugin_fuzzer.h"
#ifdef RSS_DEVICE_STANDBY_ENABLE
#include "device_standby_plugin.h"
#endif
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <securec.h>
#include <string>
#include <vector>

#ifndef errno_t
typedef int errno_t;
#endif

#ifndef EOK
#define EOK 0
#endif

namespace OHOS {
namespace ResourceSchedule {
    const uint8_t* g_data = nullptr;
    size_t g_size = 0;
    size_t g_pos;

/*
* describe: get data from outside untrusted data(g_data) which size is according to sizeof(T)
* tips: only support basic type
*/
    template<class T>
    T GetData()
    {
        T object {};
        size_t objectSize = sizeof(object);
        if (g_data == nullptr || objectSize > g_size - g_pos) {
            return object;
        }
        errno_t ret = memcpy_s(&object, objectSize, g_data + g_pos, objectSize);
        if (ret != EOK) {
            return {};
        }
        g_pos += objectSize;
        return object;
    }

/*
* get a string from g_data
*/
    std::string GetStringFromData(int strlen)
    {
        if (strlen <= 0) {
            return "";
        }
        char cstr[strlen];
        cstr[strlen - 1] = '\0';
        for (int i = 0; i < strlen - 1; i++) {
            char tmp = GetData<char>();
            if (tmp == '\0') {
                tmp = '1';
            }
            cstr[i] = tmp;
        }
        std::string str(cstr);
        return str;
    }

#ifdef RSS_DEVICE_STANDBY_ENABLE
    bool DeviceStandbyPluginFuzzTest(const uint8_t* data, size_t size)
    {
        if (data == nullptr) {
            return false;
        }

        if (size <= sizeof(int32_t) + sizeof(std::string)) {
            return false;
        }

        // initialize
        g_data = data;
        g_size = size;
        g_pos = 0;

        int32_t resType = GetData<int32_t>();
        int32_t appStartType = GetData<int32_t>();
        std::shared_ptr<ResData> resData = std::make_shared<ResData>(resType, appStartType, nullptr);
        std::shared_ptr<DeviceStandbyPlugin> deviceStandbyPlugin = std::make_shared<DeviceStandbyPlugin>();

        deviceStandbyPlugin->Init();
        deviceStandbyPlugin->DispatchResource(resData);
        deviceStandbyPlugin->Disable();
        return true;
    }
#endif // RSS_DEVICE_STANDBY_ENABLE
} // namespace ResourceSchedule
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    OHOS::ResourceSchedule::DeviceStandbyPluginFuzzTest(data, size);
    return 0;
}