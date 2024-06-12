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

#include "resschedexecutor_fuzzer.h"
#include "res_sched_exe_common_untils.h"

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <securec.h>
#include <string>
#include <vector>
#include <memory>

#ifndef errno_t
typedef int errno_t;
#endif

#ifndef EOK
#define EOK 0
#endif

namespace OHOS {
namespace ResourceSchedule {
    const uint8_t* g_data = nullptr;
    constexpr errno_t NO_ERROR = 0;
    size_t g_size = 0;
    size_t g_pos;

    /**
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
        if (ret != NO_ERROR) {
            return {};
        }
        g_pos += objectSize;
        return object;
    }

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

    bool CheckPermissionFuzzTest(const uint8_t* data, size_t size)
    {
        if (data == nullptr) {
            return false;
        }

        if (size <= sizeof(int32_t)) {
            return false;
        }

        // initialize
        g_data = data;
        g_size = size;
        g_pos = 0;
        if (static_cast<int>(size) < 0){
            return false;
        }
        // getdata
        std::string permission = GetStringFromData(static_cast<int>(size));
        ResSchedExeCommonUtils::CheckPermission(permission);
        return true;
    }

    bool StringToJsonFuzzTest(const uint8_t* data, size_t size)
    {
        if (data == nullptr) {
            return false;
        }

        if (size <= sizeof(int32_t)) {
            return false;
        }

        // initialize
        g_data = data;
        g_size = size;
        g_pos = 0;
        if (static_cast<int>(size) < 0){
            return false;
        }
        // getdata
        std::string str = GetStringFromData(static_cast<int>(size));
        nlohmann::json payload;
        ResSchedExeCommonUtils::StringToJson(str, payload);
        return true;
    }
} // namespace ResourceSchedule
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::ResourceSchedule::CheckPermissionFuzzTest(data, size);
    OHOS::ResourceSchedule::StringToJsonFuzzTest(data, size);
    return 0;
}
