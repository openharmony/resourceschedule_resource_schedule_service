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

#include "cgroupsched_fuzzer.h"
#include "background_task_observer.h"
#include "cgroup_event_handler.h"
#include "cgroup_sched.h"
#include "cgroup_adjuster.h"

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

#define EVENT_ID_REG_APP_STATE_OBSERVER 1
#define EVENT_ID_REG_BGTASK_OBSERVER    2
#define EVENT_ID_DEFAULT                3

#define APP_MGR_SERVICE_ID              501
#define WINDOW_MANAGER_SERVICE_ID       4606
#define DEFAULT                         1904

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

    bool ContinuousTaskUpdateFuzzTest(const uint8_t* data, size_t size)
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
        backgroundTaskObserver->OnContinuousTaskUpdate(continuousTaskCallbackInfo);

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

    bool ProcessEventFuzzTest(const uint8_t* data, size_t size)
    {
        if (data == nullptr) {
            return false;
        }

        // initialize
        G_DATA = data;
        g_size = size;
        g_pos = 0;

        // getdata
        int32_t typeId = GetData<int32_t>();
        auto cgroupEventHandler =
            std::make_shared<CgroupEventHandler>(OHOS::AppExecFwk::EventRunner::Create("CgroupEventHandler_fuzz"));

        auto event = AppExecFwk::InnerEvent::Get(EVENT_ID_REG_APP_STATE_OBSERVER, typeId);
        cgroupEventHandler->ProcessEvent(event);

        event = AppExecFwk::InnerEvent::Get(EVENT_ID_REG_BGTASK_OBSERVER, typeId);
        cgroupEventHandler->ProcessEvent(event);

        event = AppExecFwk::InnerEvent::Get(EVENT_ID_DEFAULT, typeId);
        cgroupEventHandler->ProcessEvent(event);

        return true;
    }

    bool HandleAbilityRemovedFuzzTest(const uint8_t* data, size_t size)
    {
        if (data == nullptr) {
            return false;
        }

        // initialize
        G_DATA = data;
        g_size = size;
        g_pos = 0;

        // getdata
        std::string deviceId(std::to_string(*data));
        auto cgroupEventHandler =
            std::make_shared<CgroupEventHandler>(OHOS::AppExecFwk::EventRunner::Create("CgroupEventHandler_fuzz"));

        int32_t saId = APP_MGR_SERVICE_ID;
        cgroupEventHandler->HandleAbilityRemoved(saId, deviceId);

        saId = WINDOW_MANAGER_SERVICE_ID;
        cgroupEventHandler->HandleAbilityRemoved(saId, deviceId);

        saId = APP_MGR_SERVICE_ID;
        cgroupEventHandler->HandleAbilityRemoved(saId, deviceId);

        saId = DEFAULT;
        cgroupEventHandler->HandleAbilityRemoved(saId, deviceId);

        return true;
    }

    bool HandleTransientTaskStartFuzzTest(const uint8_t* data, size_t size)
    {
        if (data == nullptr) {
            return false;
        }

        // initialize
        G_DATA = data;
        g_size = size;
        g_pos = 0;

        // getdata
        uid_t uid = GetData<uid_t>();
        pid_t pid = GetData<pid_t>();
        std::string packageName(std::to_string(*data));
        auto cgroupEventHandler =
            std::make_shared<CgroupEventHandler>(OHOS::AppExecFwk::EventRunner::Create("CgroupEventHandler_fuzz"));
        cgroupEventHandler->HandleTransientTaskStart(uid, pid, packageName);

        return true;
    }

    bool HandleTransientTaskEndFuzzTest(const uint8_t* data, size_t size)
    {
        if (data == nullptr) {
            return false;
        }

        // initialize
        G_DATA = data;
        g_size = size;
        g_pos = 0;

        // getdata
        uid_t uid = GetData<uid_t>();
        pid_t pid = GetData<pid_t>();
        std::string packageName(std::to_string(*data));
        auto cgroupEventHandler =
            std::make_shared<CgroupEventHandler>(OHOS::AppExecFwk::EventRunner::Create("CgroupEventHandler_fuzz"));
        cgroupEventHandler->HandleTransientTaskEnd(uid, pid, packageName);

        return true;
    }

    bool HandleContinuousTaskUpdateFuzzTest(const uint8_t* data, size_t size)
    {
        if (data == nullptr) {
            return false;
        }

        // initialize
        G_DATA = data;
        g_size = size;
        g_pos = 0;

        // getdata
        uid_t uid = GetData<uid_t>();
        pid_t pid = GetData<pid_t>();
        uint32_t typeIds = GetData<uint32_t>();
        int32_t abilityId = GetData<int32_t>();
        auto cgroupEventHandler =
            std::make_shared<CgroupEventHandler>(OHOS::AppExecFwk::EventRunner::Create("CgroupEventHandler_fuzz"));
        cgroupEventHandler->HandleContinuousTaskUpdate(uid, pid, {typeIds}, abilityId);

        return true;
    }

    bool HandleContinuousTaskCancelFuzzTest(const uint8_t* data, size_t size)
    {
        if (data == nullptr) {
            return false;
        }

        // initialize
        G_DATA = data;
        g_size = size;
        g_pos = 0;

        // getdata
        uid_t uid = GetData<uid_t>();
        pid_t pid = GetData<pid_t>();
        uint32_t typeId = GetData<uint32_t>();
        int32_t abilityId = GetData<int32_t>();
        auto cgroupEventHandler =
            std::make_shared<CgroupEventHandler>(OHOS::AppExecFwk::EventRunner::Create("CgroupEventHandler_fuzz"));
        cgroupEventHandler->HandleContinuousTaskCancel(uid, pid, typeId, abilityId);

        return true;
    }

    bool HandleReportMMIProcessFuzzTest(const uint8_t* data, size_t size)
    {
        if (data == nullptr) {
            return false;
        }

        // initialize
        G_DATA = data;
        g_size = size;
        g_pos = 0;

        // getdata
        uint32_t resType = GetData<uint32_t>();
        int64_t value = GetData<int64_t>();
        nlohmann::json payload;
        auto cgroupEventHandler =
            std::make_shared<CgroupEventHandler>(OHOS::AppExecFwk::EventRunner::Create("CgroupEventHandler_fuzz"));
        cgroupEventHandler->HandleReportMMIProcess(resType, value, payload);

        return true;
    }

    bool HandleReportRenderThreadFuzzTest(const uint8_t* data, size_t size)
    {
        if (data == nullptr) {
            return false;
        }

        // initialize
        G_DATA = data;
        g_size = size;
        g_pos = 0;

        // getdata
        uint32_t resType = GetData<uint32_t>();
        int64_t value = GetData<int64_t>();
        nlohmann::json payload;
        auto cgroupEventHandler =
            std::make_shared<CgroupEventHandler>(OHOS::AppExecFwk::EventRunner::Create("CgroupEventHandler_fuzz"));
        cgroupEventHandler->HandleReportRenderThread(resType, value, payload);

        return true;
    }

    bool HandleReportKeyThreadFuzzTest(const uint8_t* data, size_t size)
    {
        if (data == nullptr) {
            return false;
        }

        // initialize
        G_DATA = data;
        g_size = size;
        g_pos = 0;

        // getdata
        uint32_t resType = GetData<uint32_t>();
        int64_t value = GetData<int64_t>();
        nlohmann::json payload;
        auto cgroupEventHandler =
            std::make_shared<CgroupEventHandler>(OHOS::AppExecFwk::EventRunner::Create("CgroupEventHandler_fuzz"));
        cgroupEventHandler->HandleReportKeyThread(resType, value, payload);

        return true;
    }

    bool HandleReportWindowStateFuzzTest(const uint8_t* data, size_t size)
    {
        if (data == nullptr) {
            return false;
        }

        // initialize
        G_DATA = data;
        g_size = size;
        g_pos = 0;

        // getdata
        uint32_t resType = GetData<uint32_t>();
        int64_t value = GetData<int64_t>();
        nlohmann::json payload;
        auto cgroupEventHandler =
            std::make_shared<CgroupEventHandler>(OHOS::AppExecFwk::EventRunner::Create("CgroupEventHandler_fuzz"));
        cgroupEventHandler->HandleReportWindowState(resType, value, payload);

        return true;
    }

    bool HandleReportWebviewAudioStateFuzzTest(const uint8_t* data, size_t size)
    {
        if (data == nullptr) {
            return false;
        }

        // initialize
        G_DATA = data;
        g_size = size;
        g_pos = 0;

        // getdata
        uint32_t resType = GetData<uint32_t>();
        int64_t value = GetData<int64_t>();
        nlohmann::json payload;
        auto cgroupEventHandler =
            std::make_shared<CgroupEventHandler>(OHOS::AppExecFwk::EventRunner::Create("CgroupEventHandler_fuzz"));
        cgroupEventHandler->HandleReportWebviewAudioState(resType, value, payload);

        return true;
    }

    bool HandleReportRunningLockEventFuzzTest(const uint8_t* data, size_t size)
    {
        if (data == nullptr) {
            return false;
        }

        // initialize
        G_DATA = data;
        g_size = size;
        g_pos = 0;

        // getdata
        uint32_t resType = GetData<uint32_t>();
        int64_t value = GetData<int64_t>();
        nlohmann::json payload;
        auto cgroupEventHandler =
            std::make_shared<CgroupEventHandler>(OHOS::AppExecFwk::EventRunner::Create("CgroupEventHandler_fuzz"));
        cgroupEventHandler->HandleReportRunningLockEvent(resType, value, payload);

        return true;
    }

    bool HandleReportHisysEventFuzzTest(const uint8_t* data, size_t size)
    {
        if (data == nullptr) {
            return false;
        }

        // initialize
        G_DATA = data;
        g_size = size;
        g_pos = 0;

        // getdata
        uint32_t resType = GetData<uint32_t>();
        int64_t value = GetData<int64_t>();
        nlohmann::json payload;
        auto cgroupEventHandler =
            std::make_shared<CgroupEventHandler>(OHOS::AppExecFwk::EventRunner::Create("CgroupEventHandler_fuzz"));
        cgroupEventHandler->HandleReportHisysEvent(resType, value, payload);

        return true;
    }

    bool CheckVisibilityForRenderProcessFuzzTest(const uint8_t* data, size_t size)
    {
        if (data == nullptr) {
            return false;
        }

        // initialize
        G_DATA = data;
        g_size = size;
        g_pos = 0;

        // getdata
        uid_t uid = GetData<uid_t>();
        pid_t pid = GetData<pid_t>();
        ProcessRecord pr(uid, pid);
        pr.isRenderProcess_ = true;
        pr.isActive_ = true;
        ProcessRecord mainProc(uid, pid);
        auto cgroupEventHandler =
            std::make_shared<CgroupEventHandler>(OHOS::AppExecFwk::EventRunner::Create("CgroupEventHandler_fuzz"));
        cgroupEventHandler->CheckVisibilityForRenderProcess(pr, mainProc);

        return true;
    }

    bool ParsePayloadFuzzTest(const uint8_t* data, size_t size)
    {
        if (data == nullptr) {
            return false;
        }

        // initialize
        G_DATA = data;
        g_size = size;
        g_pos = 0;

        // getdata
        int32_t uid = GetData<int32_t>();
        int32_t pid = GetData<int32_t>();
        int32_t tid = GetData<int32_t>();
        int64_t value = GetData<int64_t>();
        nlohmann::json payload;
        auto cgroupEventHandler =
            std::make_shared<CgroupEventHandler>(OHOS::AppExecFwk::EventRunner::Create("CgroupEventHandler_fuzz"));
        cgroupEventHandler->ParsePayload(uid, pid, tid, value, payload);

        return true;
    }
} // namespace ResourceSchedule
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    // background_task_observer.cpp
    OHOS::ResourceSchedule::TransientTaskStartFuzzTest(data, size);
    OHOS::ResourceSchedule::TransientTaskEndFuzzTest(data, size);
    OHOS::ResourceSchedule::ContinuousTaskStartFuzzTest(data, size);
    OHOS::ResourceSchedule::ContinuousTaskStopFuzzTest(data, size);
    OHOS::ResourceSchedule::ContinuousTaskUpdateFuzzTest(data, size);
    OHOS::ResourceSchedule::RemoteDiedFuzzTest(data, size);
    // background_task_observer.cpp end

    // cgroup_event_handler.cpp
    OHOS::ResourceSchedule::ProcessEventFuzzTest(data, size);
    OHOS::ResourceSchedule::HandleAbilityRemovedFuzzTest(data, size);
    OHOS::ResourceSchedule::HandleTransientTaskStartFuzzTest(data, size);
    OHOS::ResourceSchedule::HandleTransientTaskEndFuzzTest(data, size);
    OHOS::ResourceSchedule::HandleContinuousTaskUpdateFuzzTest(data, size);
    OHOS::ResourceSchedule::HandleContinuousTaskCancelFuzzTest(data, size);
    OHOS::ResourceSchedule::HandleReportMMIProcessFuzzTest(data, size);
    OHOS::ResourceSchedule::HandleReportRenderThreadFuzzTest(data, size);
    OHOS::ResourceSchedule::HandleReportKeyThreadFuzzTest(data, size);
    OHOS::ResourceSchedule::HandleReportWindowStateFuzzTest(data, size);
    OHOS::ResourceSchedule::HandleReportWebviewAudioStateFuzzTest(data, size);
    OHOS::ResourceSchedule::HandleReportRunningLockEventFuzzTest(data, size);
    OHOS::ResourceSchedule::HandleReportHisysEventFuzzTest(data, size);
    OHOS::ResourceSchedule::CheckVisibilityForRenderProcessFuzzTest(data, size);
    OHOS::ResourceSchedule::ParsePayloadFuzzTest(data, size);
    // cgroup_event_handler.cpp end

    return 0;
}
