/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "cgroupsched_fuzzer.h"
#include "background_task_observer.h"

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
    const uint8_t* G_DATA = nullptr;
    size_t g_size = 0;
    size_t g_pos;

/*
* describe: get data from outside untrusted data(G_DATA) which size is according to sizeof(T)
* tips: only support basic type
*/
    template<class T>
    T GetData()
    {
        T object {};
        size_t objectSize = sizeof(object);
        if (G_DATA == nullptr || objectSize > g_size - g_pos) {
            return object;
        }
        errno_t ret = memcpy_s(&object, objectSize, G_DATA + g_pos, objectSize);
        if (ret != EOK) {
            return {};
        }
        g_pos += objectSize;
        return object;
    }

/*
* get a string from G_DATA
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

    bool TransientTaskStartFuzzTest(const uint8_t* data, size_t size)
    {
        if (data == nullptr) {
            return false;
        }

        if (size <= sizeof(int32_t) + sizeof(int32_t)) {
            return false;
        }

        // initialize
        G_DATA = data;
        g_size = size;
        g_pos = 0;

        // getdata
        int32_t uid = GetData<int32_t>();
        int32_t pid = GetData<int32_t>();
        std::string packageName = GetStringFromData(int(size) - sizeof(int32_t) - sizeof(int32_t));
        auto transientTaskAppInfo =
            std::make_shared<TransientTaskAppInfo>(packageName, uid, pid);
        auto backgroundTaskObserver = std::make_unique<BackgroundTaskObserver>();
        backgroundTaskObserver->OnTransientTaskStart(transientTaskAppInfo);

        return true;
    }

    bool TransientTaskEndFuzzTest(const uint8_t* data, size_t size)
    {
        if (data == nullptr) {
            return false;
        }

        if (size <= sizeof(int32_t) + sizeof(int32_t)) {
            return false;
        }

        // initialize
        G_DATA = data;
        g_size = size;
        g_pos = 0;

        // getdata
        int32_t uid = GetData<int32_t>();
        int32_t pid = GetData<int32_t>();
        std::string packageName = GetStringFromData(int(size) - sizeof(int32_t) - sizeof(int32_t));
        auto transientTaskAppInfo =
        std::make_shared<TransientTaskAppInfo>(packageName, uid, pid);
        auto backgroundTaskObserver = std::make_unique<BackgroundTaskObserver>();
        backgroundTaskObserver->OnTransientTaskEnd(transientTaskAppInfo);

        return true;
    }

    bool ContinuousTaskStartFuzzTest(const uint8_t* data, size_t size)
    {
        if (data == nullptr) {
            return false;
        }

        if (size <= sizeof(int32_t) + sizeof(int32_t) + sizeof(pid_t)) {
            return false;
        }

        // initialize
        G_DATA = data;
        g_size = size;
        g_pos = 0;

        // getdata
        int32_t typeId = GetData<int32_t>();
        int32_t creatorUid = GetData<int32_t>();
        pid_t creatorPid = GetData<pid_t>();
        std::string abilityName = GetStringFromData(int(size) - sizeof(int32_t) - sizeof(int32_t) - sizeof(pid_t));
        auto continuousTaskCallbackInfo =
            std::make_shared<ContinuousTaskCallbackInfo>(typeId, creatorUid, creatorPid, abilityName);
        auto backgroundTaskObserver = std::make_unique<BackgroundTaskObserver>();
        backgroundTaskObserver->OnContinuousTaskStart(continuousTaskCallbackInfo);

        return true;
    }

    bool ContinuousTaskStopFuzzTest(const uint8_t* data, size_t size)
    {
        if (data == nullptr) {
            return false;
        }

        if (size <= sizeof(int32_t) + sizeof(int32_t) + sizeof(pid_t)) {
            return false;
        }

        // initialize
        G_DATA = data;
        g_size = size;
        g_pos = 0;

        // getdata
        int32_t typeId = GetData<int32_t>();
        int32_t creatorUid = GetData<int32_t>();
        pid_t creatorPid = GetData<pid_t>();
        std::string abilityName = GetStringFromData(int(size) - sizeof(int32_t) - sizeof(int32_t) - sizeof(pid_t));
        auto continuousTaskCallbackInfo =
            std::make_shared<ContinuousTaskCallbackInfo>(typeId, creatorUid, creatorPid, abilityName);
        auto backgroundTaskObserver = std::make_unique<BackgroundTaskObserver>();
        backgroundTaskObserver->OnContinuousTaskStop(continuousTaskCallbackInfo);

        return true;
    }

    bool RemoteDiedFuzzTest(const uint8_t* data, size_t size)
    {
        if (data == nullptr) {
            return false;
        }

        // initialize
        G_DATA = data;
        g_size = size;
        g_pos = 0;

        // getdata
        wptr<IRemoteObject> iRemoteObject = nullptr;
        auto backgroundTaskObserver = std::make_unique<BackgroundTaskObserver>();
        backgroundTaskObserver->OnRemoteDied(iRemoteObject);

        return true;
    }
} // namespace ResourceSchedule
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::ResourceSchedule::TransientTaskStartFuzzTest(data, size);
    OHOS::ResourceSchedule::TransientTaskEndFuzzTest(data, size);
    OHOS::ResourceSchedule::ContinuousTaskStartFuzzTest(data, size);
    OHOS::ResourceSchedule::ContinuousTaskStopFuzzTest(data, size);
    OHOS::ResourceSchedule::RemoteDiedFuzzTest(data, size);
    return 0;
}