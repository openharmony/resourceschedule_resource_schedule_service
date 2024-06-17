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
#include "wm_common.h"

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
    std::shared_ptr<Supervisor> supervisor = std::make_shared<Supervisor>();

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

        auto event1 = AppExecFwk::InnerEvent::Get(EVENT_ID_REG_APP_STATE_OBSERVER, typeId);
        auto event2 = AppExecFwk::InnerEvent::Get(EVENT_ID_REG_BGTASK_OBSERVER, typeId);
        auto event3 = AppExecFwk::InnerEvent::Get(EVENT_ID_DEFAULT, typeId);
        cgroupEventHandler->ProcessEvent(event1);
        cgroupEventHandler->ProcessEvent(event2);
        cgroupEventHandler->ProcessEvent(event3);

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
        cgroupEventHandler->SetSupervisor(supervisor);
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
        cgroupEventHandler->SetSupervisor(supervisor);
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
        cgroupEventHandler->SetSupervisor(supervisor);
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
        cgroupEventHandler->SetSupervisor(supervisor);
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
        cgroupEventHandler->SetSupervisor(supervisor);
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
        cgroupEventHandler->SetSupervisor(supervisor);
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
        cgroupEventHandler->SetSupervisor(supervisor);
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
        cgroupEventHandler->SetSupervisor(supervisor);
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
        cgroupEventHandler->SetSupervisor(supervisor);
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
        cgroupEventHandler->SetSupervisor(supervisor);
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
        cgroupEventHandler->SetSupervisor(supervisor);
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
        cgroupEventHandler->SetSupervisor(supervisor);
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

    bool HandleReportAvCodecEventFuzzTest(const uint8_t* data, size_t size)
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
        cgroupEventHandler->HandleReportAvCodecEvent(resType, value, payload);

        return true;
    }

    bool HandleWindowVisibilityChangedFuzzTest(const uint8_t* data, size_t size)
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
        uint32_t windowId = GetData<int32_t>();
        uint32_t visibilityState = GetData<uint32_t>();
        auto cgroupEventHandler =
            std::make_shared<CgroupEventHandler>(OHOS::AppExecFwk::EventRunner::Create("CgroupEventHandler_fuzz"));

        cgroupEventHandler->HandleWindowVisibilityChanged(
            windowId, visibilityState, WindowType::APP_WINDOW_BASE, pid, uid);
        cgroupEventHandler->SetSupervisor(supervisor);
        cgroupEventHandler->HandleWindowVisibilityChanged(
            windowId, visibilityState, WindowType::APP_WINDOW_BASE, pid, uid);

        return true;
    }

    bool HandleDrawingContentChangeWindowFuzzTest(const uint8_t* data, size_t size)
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
        uint32_t windowId = GetData<int32_t>();
        auto cgroupEventHandler =
            std::make_shared<CgroupEventHandler>(OHOS::AppExecFwk::EventRunner::Create("CgroupEventHandler_fuzz"));

        cgroupEventHandler->HandleDrawingContentChangeWindow(windowId, WindowType::APP_WINDOW_BASE, false, pid, uid);
        cgroupEventHandler->SetSupervisor(supervisor);
        cgroupEventHandler->HandleDrawingContentChangeWindow(windowId, WindowType::APP_WINDOW_BASE, true, pid, uid);

        return true;
    }

    bool HandleUnfocusedWindowFuzzTest(const uint8_t* data, size_t size)
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
        uintptr_t token = GetData<uintptr_t>();
        uint32_t windowId = GetData<int32_t>();
        uint64_t displayId = GetData<uint64_t>();
        auto cgroupEventHandler =
            std::make_shared<CgroupEventHandler>(OHOS::AppExecFwk::EventRunner::Create("CgroupEventHandler_fuzz"));
        
        cgroupEventHandler->HandleUnfocusedWindow(
            windowId, token, WindowType::APP_WINDOW_BASE, displayId, pid, uid);
        cgroupEventHandler->SetSupervisor(supervisor);
        cgroupEventHandler->HandleUnfocusedWindow(
            windowId, token, WindowType::APP_WINDOW_BASE, displayId, pid, uid);

        return true;
    }

    bool HandleFocusedWindowFuzzTest(const uint8_t* data, size_t size)
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
        uintptr_t token = GetData<uintptr_t>();
        uint32_t windowId = GetData<int32_t>();
        uint64_t displayId = GetData<uint64_t>();
        auto cgroupEventHandler =
            std::make_shared<CgroupEventHandler>(OHOS::AppExecFwk::EventRunner::Create("CgroupEventHandler_fuzz"));
        cgroupEventHandler->SetSupervisor(supervisor);

        cgroupEventHandler->HandleFocusedWindow(
            windowId, token, WindowType::APP_WINDOW_BASE, displayId, pid, uid);

        return true;
    }

    bool HandleApplicationStateChangedFuzzTest(const uint8_t* data, size_t size)
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
        int32_t state = GetData<int32_t>();
        auto cgroupEventHandler =
            std::make_shared<CgroupEventHandler>(OHOS::AppExecFwk::EventRunner::Create("CgroupEventHandler_fuzz"));

        cgroupEventHandler->HandleApplicationStateChanged(uid, pid, packageName, state);
        cgroupEventHandler->SetSupervisor(supervisor);
        cgroupEventHandler->HandleApplicationStateChanged(uid, pid, packageName, state);

        return true;
    }

    bool HandleProcessStateChangedFuzzTest(const uint8_t* data, size_t size)
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
        int32_t state = GetData<int32_t>();
        auto cgroupEventHandler =
            std::make_shared<CgroupEventHandler>(OHOS::AppExecFwk::EventRunner::Create("CgroupEventHandler_fuzz"));

        cgroupEventHandler->HandleProcessStateChanged(uid, pid, packageName, state);
        cgroupEventHandler->SetSupervisor(supervisor);
        cgroupEventHandler->HandleProcessStateChanged(uid, pid, packageName, state);

        return true;
    }

    bool HandleAbilityStateChangedFuzzTest(const uint8_t* data, size_t size)
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
        std::string bundleName(std::to_string(*data));
        std::string abilityName(std::to_string(*data));
        uintptr_t token = GetData<uintptr_t>();
        int32_t abilityState = GetData<int32_t>();
        int32_t abilityType = GetData<int32_t>();
        auto cgroupEventHandler =
            std::make_shared<CgroupEventHandler>(OHOS::AppExecFwk::EventRunner::Create("CgroupEventHandler_fuzz"));

        cgroupEventHandler->HandleAbilityStateChanged(
            uid, pid, bundleName, abilityName, token, abilityState, abilityType);
        cgroupEventHandler->SetSupervisor(supervisor);
        cgroupEventHandler->HandleAbilityStateChanged(
            uid, pid, bundleName, abilityName, token, abilityState, abilityType);

        return true;
    }

    bool HandleExtensionStateChangedFuzzTest(const uint8_t* data, size_t size)
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
        std::string bundleName(std::to_string(*data));
        std::string abilityName(std::to_string(*data));
        uintptr_t token = GetData<uintptr_t>();
        int32_t extensionState = GetData<int32_t>();
        int32_t abilityType = GetData<int32_t>();
        auto cgroupEventHandler =
            std::make_shared<CgroupEventHandler>(OHOS::AppExecFwk::EventRunner::Create("CgroupEventHandler_fuzz"));

        cgroupEventHandler->HandleExtensionStateChanged(
            uid, pid, bundleName, abilityName, token, extensionState, abilityType);
        cgroupEventHandler->SetSupervisor(supervisor);
        cgroupEventHandler->HandleExtensionStateChanged(
            uid, pid, bundleName, abilityName, token, extensionState, abilityType);

        return true;
    }


    bool HandleProcessCreatedFuzzTest(const uint8_t* data, size_t size)
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
        std::string bundleName(std::to_string(*data));
        int32_t hostPid = GetData<int32_t>();
        int32_t processType = GetData<int32_t>();
        int32_t extensionType = GetData<int32_t>();
        auto cgroupEventHandler =
            std::make_shared<CgroupEventHandler>(OHOS::AppExecFwk::EventRunner::Create("CgroupEventHandler_fuzz"));

        cgroupEventHandler->HandleProcessCreated(uid, pid, hostPid, processType, bundleName, extensionType);
        cgroupEventHandler->SetSupervisor(supervisor);
        cgroupEventHandler->HandleProcessCreated(uid, pid, hostPid, processType, bundleName, extensionType);

        return true;
    }

    bool HandleReportAudioStateFuzzTest(const uint8_t* data, size_t size)
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

        cgroupEventHandler->HandleReportAudioState(resType, value, payload);
        cgroupEventHandler->SetSupervisor(supervisor);
        cgroupEventHandler->HandleReportAudioState(resType, value, payload);

        return true;
    }

    bool HandleWebviewScreenCaptureFuzzTest(const uint8_t* data, size_t size)
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

        cgroupEventHandler->HandleWebviewScreenCapture(resType, value, payload);

        return true;
    }

    bool HandleReportWebviewVideoStateFuzzTest(const uint8_t* data, size_t size)
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

        cgroupEventHandler->HandleReportWebviewVideoState(resType, value, payload);

        return true;
    }

    bool AdjustForkProcessGroupFuzzTest(const uint8_t* data, size_t size)
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
        Application app(uid);

        CgroupAdjuster::GetInstance().AdjustForkProcessGroup(app, pr);

        return true;
    }

    bool AdjustProcessGroupFuzzTest(const uint8_t* data, size_t size)
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
        Application app(uid);

        CgroupAdjuster::GetInstance().AdjustProcessGroup(app, pr, AdjustSource::ADJS_FG_APP_CHANGE);

        return true;
    }

    bool AdjustAllProcessGroupFuzzTest(const uint8_t* data, size_t size)
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
        Application app(uid);

        CgroupAdjuster::GetInstance().AdjustAllProcessGroup(app, AdjustSource::ADJS_FG_APP_CHANGE);

        return true;
    }

    bool ComputeProcessGroupFuzzTest(const uint8_t* data, size_t size)
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
        Application app(uid);

        CgroupAdjuster::GetInstance().ComputeProcessGroup(app, pr, AdjustSource::ADJS_FG_APP_CHANGE);

        return true;
    }

    bool ApplyProcessGroupFuzzTest(const uint8_t* data, size_t size)
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
        Application app(uid);

        CgroupAdjuster::GetInstance().ApplyProcessGroup(app, pr);

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
    OHOS::ResourceSchedule::HandleReportAvCodecEventFuzzTest(data, size);
    OHOS::ResourceSchedule::HandleWindowVisibilityChangedFuzzTest(data, size);
    OHOS::ResourceSchedule::HandleUnfocusedWindowFuzzTest(data, size);
    OHOS::ResourceSchedule::HandleFocusedWindowFuzzTest(data, size);
    OHOS::ResourceSchedule::HandleApplicationStateChangedFuzzTest(data, size);
    OHOS::ResourceSchedule::HandleProcessStateChangedFuzzTest(data, size);
    OHOS::ResourceSchedule::HandleAbilityStateChangedFuzzTest(data, size);
    OHOS::ResourceSchedule::HandleExtensionStateChangedFuzzTest(data, size);
    OHOS::ResourceSchedule::HandleReportAudioStateFuzzTest(data, size);
    OHOS::ResourceSchedule::HandleProcessCreatedFuzzTest(data, size);
    OHOS::ResourceSchedule::HandleWebviewScreenCaptureFuzzTest(data, size);
    OHOS::ResourceSchedule::HandleReportWebviewVideoStateFuzzTest(data, size);
    // cgroup_event_handler.cpp end

    // cgroup_adjuster.cpp
    OHOS::ResourceSchedule::AdjustForkProcessGroupFuzzTest(data, size);
    OHOS::ResourceSchedule::AdjustProcessGroupFuzzTest(data, size);
    OHOS::ResourceSchedule::AdjustAllProcessGroupFuzzTest(data, size);
    OHOS::ResourceSchedule::ComputeProcessGroupFuzzTest(data, size);
    OHOS::ResourceSchedule::ApplyProcessGroupFuzzTest(data, size);
    // cgroup_adjuster.cpp end

    return 0;
}
