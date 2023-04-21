/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#include "perfrequest_fuzzer.h"
#include "socperf_client.h"

#include <cstdint>
#include <securec.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#ifndef errno_t
typedef int errno_t;
#endif

#ifndef EOK
#define EOK 0
#endif

namespace OHOS {
namespace SOCPERF {
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
        if (strlen <= 0){
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

    bool PerfRequestFuzzTest(const uint8_t* data, size_t size)
    {
        if (data == nullptr) {
            return false;
        }

        // initialize
        g_data = data;
        g_size = size;
        g_pos = 0;

        // getdata
        std::string msg;
        int32_t cmdId = GetData<int32_t>();
        msg = GetStringFromData(int(size) - sizeof(int32_t));
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequest(cmdId, msg);

        return true;
    }

    bool PerfRequestExFuzzTest(const uint8_t* data, size_t size)
    {
        if (data == nullptr) {
            return false;
        }

        // initialize
        g_data = data;
        g_size = size;
        g_pos = 0;

        // getdata
        std::string msg;
        int32_t cmdId = GetData<int32_t>();
        bool onOffTag = GetData<bool>();
        msg = GetStringFromData(int(size) - sizeof(int32_t) - sizeof(bool));
        OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(cmdId, onOffTag, msg);

        return true;
    }

    bool PowerLimitBoostFuzzTest(const uint8_t* data, size_t size)
    {
        if (data == nullptr) {
            return false;
        }

        // initialize
        g_data = data;
        g_size = size;
        g_pos = 0;

        // getdata
        std::string msg;
        bool onOffTag = GetData<bool>();
        msg = GetStringFromData(int(size) - sizeof(bool));
        OHOS::SOCPERF::SocPerfClient::GetInstance().PowerLimitBoost(onOffTag, msg);

        return true;
    }

    bool ThermalLimitBoostFuzzTest(const uint8_t* data, size_t size)
    {
        if (data == nullptr) {
            return false;
        }

        // initialize
        g_data = data;
        g_size = size;
        g_pos = 0;

        // getdata
        std::string msg;
        bool onOffTag = GetData<bool>();
        msg = GetStringFromData(int(size) - sizeof(bool));
        OHOS::SOCPERF::SocPerfClient::GetInstance().ThermalLimitBoost(onOffTag, msg);

        return true;
    }    

    bool LimitRequestFuzzTest(const uint8_t* data, size_t size)
    {
        if (data == nullptr) {
            return false;
        }

        // initialize
        g_data = data;
        g_size = size;
        g_pos = 0;

        // getdata
        std::vector<int32_t> tags;
        std::vector<int64_t> configs;
        std::string msg;
        int32_t tags_number = GetData<int32_t>();
        int64_t configs_number = GetData<int64_t>();
        int32_t clientId = GetData<int32_t>();
        tags.push_back(tags_number);
        configs.push_back(configs_number);
        msg = GetStringFromData(int(size) - 2 * sizeof(int32_t) - sizeof(int64_t));
        OHOS::SOCPERF::SocPerfClient::GetInstance().LimitRequest(clientId, tags, configs, msg);

        return true;
    }
} // namespace SOCPERF
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::SOCPERF::PerfRequestFuzzTest(data, size);
    OHOS::SOCPERF::PerfRequestExFuzzTest(data, size);
    OHOS::SOCPERF::PowerLimitBoostFuzzTest(data, size);
    OHOS::SOCPERF::ThermalLimitBoostFuzzTest(data, size);
    OHOS::SOCPERF::LimitRequestFuzzTest(data, size);
    return 0;
}