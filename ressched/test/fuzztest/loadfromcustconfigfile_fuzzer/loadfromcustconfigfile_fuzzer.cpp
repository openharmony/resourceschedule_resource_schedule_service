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

#include "loadfromcustconfigfile_fuzzer.h"

#include "config_reader.h"

namespace OHOS {
namespace ResourceSchedule {
    const std::string fuzzedFile = "myConfigFuzzed.xml";
    const int32_t FILE_RETURN_SUCCESS = 1;

    bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
    {
        // generate fuzzed xml
        FILE *pFile = fopen(fuzzedFile.c_str(), "wb");
        if (!pFile) {
            return false;
        }

        int32_t retCode = fwrite(reinterpret_cast<const void*>(data), size, 1, pFile); // 1 means count=1
        if (retCode < FILE_RETURN_SUCCESS) {
            (void)fclose(pFile);
            pFile = nullptr;
            return false;
        }

        (void)fclose(pFile);
        pFile = nullptr;

        std::shared_ptr<ConfigReader> configReader = std::make_shared<ConfigReader>();
        bool ret = configReader->LoadFromCustConfigFile(fuzzedFile);
        return ret;
    }
} // namespace ResourceSchedule
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::ResourceSchedule::DoSomethingInterestingWithMyAPI(data, size);
    return 0;
}
