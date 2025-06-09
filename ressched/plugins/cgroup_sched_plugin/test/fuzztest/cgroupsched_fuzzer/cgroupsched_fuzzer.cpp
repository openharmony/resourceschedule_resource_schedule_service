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
#include "cgroup_adjuster.h"
#include "sched_controller.h"
#include "cgroup_controller.h"

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
#define TASK_KEEPING                    9
#define DATA_TRANSFER                   1

namespace OHOS {
namespace ResourceSchedule {
    const uint8_t* G_DATA = nullptr;
    size_t g_size = 0;
    size_t g_pos;
    std::shared_ptr<Supervisor> g_supervisor = std::make_shared<Supervisor>();
    const size_t STR_LEN = 10;

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

        // initialize
        G_DATA = data;
        g_size = size;

        // getdata
        int32_t uid = GetData<int32_t>();
        int32_t pid = GetData<int32_t>();
        std::string packageName = GetStringFromData(STR_LEN);
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

        // initialize
        G_DATA = data;
        g_size = size;

        // getdata
        int32_t uid = GetData<int32_t>();
        int32_t pid = GetData<int32_t>();
        std::string packageName = GetStringFromData(STR_LEN);
        auto transientTaskAppInfo =
        std::make_shared<TransientTaskAppInfo>(packageName, uid, pid);
        auto backgroundTaskObserver = std::make_unique<BackgroundTaskObserver>();
        backgroundTaskObserver->OnTransientTaskEnd(transientTaskAppInfo);

        return true;
    }

    bool MarshallingContinuousTaskCallbackInfoFuzzTest(const uint8_t* data, size_t size)
    {
        if (data == nullptr) {
            return false;
        }

        // initialize
        G_DATA = data;
        g_size = size;

        // getdata
        uint32_t typeId = GetData<int32_t>();
        std::vector<uint32_t> typeIds;
        typeIds.push_back(typeId);
        int32_t creatorUid = GetData<int32_t>();
        int32_t abilityId = GetData<int32_t>();
        pid_t creatorPid = GetData<pid_t>();
        std::string abilityName = GetStringFromData(STR_LEN);
        bool isBatchApi = GetData<bool>();
        bool isFromWebview = GetData<bool>();
        nlohmann::json payload;
        auto continuousTaskCallbackInfo =
            std::make_shared<ContinuousTaskCallbackInfo>(typeId, creatorUid, creatorPid, abilityName,
            isFromWebview, isBatchApi, typeIds, abilityId);
        auto backgroundTaskObserver = std::make_unique<BackgroundTaskObserver>();
        backgroundTaskObserver->MarshallingContinuousTaskCallbackInfo(continuousTaskCallbackInfo, payload);

        return true;
    }

    bool ContinuousTaskStartFuzzTest(const uint8_t* data, size_t size)
    {
        if (data == nullptr) {
            return false;
        }

        // initialize
        G_DATA = data;
        g_size = size;

        // getdata
        int32_t typeId = DATA_TRANSFER;
        int32_t creatorUid = GetData<int32_t>();
        pid_t creatorPid = GetData<pid_t>();
        std::string abilityName = GetStringFromData(STR_LEN);
        auto continuousTaskCallbackInfo =
            std::make_shared<ContinuousTaskCallbackInfo>(typeId, creatorUid, creatorPid, abilityName);
        auto backgroundTaskObserver = std::make_unique<BackgroundTaskObserver>();
        backgroundTaskObserver->OnContinuousTaskStart(continuousTaskCallbackInfo);
        typeId = TASK_KEEPING;
        continuousTaskCallbackInfo =
            std::make_shared<ContinuousTaskCallbackInfo>(typeId, creatorUid, creatorPid, abilityName);
        backgroundTaskObserver->OnContinuousTaskStart(continuousTaskCallbackInfo);

        return true;
    }

    bool ContinuousTaskStopFuzzTest(const uint8_t* data, size_t size)
    {
        if (data == nullptr) {
            return false;
        }

        // initialize
        G_DATA = data;
        g_size = size;

        // getdata
        int32_t typeId = DATA_TRANSFER;
        int32_t creatorUid = GetData<int32_t>();
        pid_t creatorPid = GetData<pid_t>();
        std::string abilityName = GetStringFromData(STR_LEN);
        auto continuousTaskCallbackInfo =
            std::make_shared<ContinuousTaskCallbackInfo>(typeId, creatorUid, creatorPid, abilityName);
        auto backgroundTaskObserver = std::make_unique<BackgroundTaskObserver>();
        backgroundTaskObserver->OnContinuousTaskStop(continuousTaskCallbackInfo);
        typeId = TASK_KEEPING;
        continuousTaskCallbackInfo =
            std::make_shared<ContinuousTaskCallbackInfo>(typeId, creatorUid, creatorPid, abilityName);
        backgroundTaskObserver->OnContinuousTaskStop(continuousTaskCallbackInfo);

        return true;
    }

    bool ContinuousTaskUpdateFuzzTest(const uint8_t* data, size_t size)
    {
        if (data == nullptr) {
            return false;
        }

        // initialize
        G_DATA = data;
        g_size = size;

        // getdata
        int32_t typeId = DATA_TRANSFER;
        int32_t creatorUid = GetData<int32_t>();
        pid_t creatorPid = GetData<pid_t>();
        std::string abilityName = GetStringFromData(STR_LEN);
        auto continuousTaskCallbackInfo =
            std::make_shared<ContinuousTaskCallbackInfo>(typeId, creatorUid, creatorPid, abilityName);
        auto backgroundTaskObserver = std::make_unique<BackgroundTaskObserver>();
        backgroundTaskObserver->OnContinuousTaskUpdate(continuousTaskCallbackInfo);
        typeId = TASK_KEEPING;
        continuousTaskCallbackInfo =
            std::make_shared<ContinuousTaskCallbackInfo>(typeId, creatorUid, creatorPid, abilityName);
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

        // getdata
        int32_t typeId = GetData<int32_t>();
        auto cgroupEventHandler =
            std::make_shared<CgroupEventHandler>("CgroupEventHandler_fuzz");

        int32_t eventId1 = EVENT_ID_REG_APP_STATE_OBSERVER;
        int64_t eventParam1 = typeId;
        cgroupEventHandler->ProcessEvent(eventId1, eventParam1);
        int32_t eventId2 = EVENT_ID_REG_BGTASK_OBSERVER;
        int64_t eventParam2 = typeId;
        cgroupEventHandler->ProcessEvent(eventId2, eventParam2);
        int32_t eventId3 = EVENT_ID_DEFAULT;
        int64_t eventParam3 = typeId;
        cgroupEventHandler->ProcessEvent(eventId3, eventParam3);

        return true;
    }

    bool HandleProcessDiedFuzzTest(const uint8_t* data, size_t size)
    {
        if (data == nullptr) {
            return false;
        }

        // initialize
        G_DATA = data;
        g_size = size;

        // getdata
        uint32_t resType = GetData<uint32_t>();
        int64_t value = GetData<int64_t>();
        uid_t uid = GetData<uid_t>();
        pid_t pid = GetData<pid_t>();
        std::string packageName(std::to_string(*data));
        nlohmann::json payload;
        payload["uid"] = std::to_string(uid);
        payload["pid"] = std::to_string(pid);
        payload["bundleName"] = packageName;
        auto cgroupEventHandler =
            std::make_shared<CgroupEventHandler>("CgroupEventHandler_fuzz");

        cgroupEventHandler->SetSupervisor(g_supervisor);
        cgroupEventHandler->HandleProcessDied(resType, value, payload);

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

        // getdata
        uid_t uid = GetData<uid_t>();
        pid_t pid = GetData<pid_t>();
        std::string packageName(std::to_string(*data));
        auto cgroupEventHandler =
            std::make_shared<CgroupEventHandler>("CgroupEventHandler_fuzz");

        cgroupEventHandler->HandleTransientTaskStart(uid, pid, packageName);
        cgroupEventHandler->SetSupervisor(g_supervisor);
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

        // getdata
        uid_t uid = GetData<uid_t>();
        pid_t pid = GetData<pid_t>();
        std::string packageName(std::to_string(*data));
        auto cgroupEventHandler =
            std::make_shared<CgroupEventHandler>("CgroupEventHandler_fuzz");

        cgroupEventHandler->HandleTransientTaskEnd(uid, pid, packageName);
        cgroupEventHandler->SetSupervisor(g_supervisor);
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

        // getdata
        uid_t uid = GetData<uid_t>();
        pid_t pid = GetData<pid_t>();
        uint32_t typeIds = GetData<uint32_t>();
        int32_t abilityId = GetData<int32_t>();
        auto cgroupEventHandler =
            std::make_shared<CgroupEventHandler>("CgroupEventHandler_fuzz");

        cgroupEventHandler->HandleContinuousTaskUpdate(uid, pid, {typeIds}, abilityId);
        cgroupEventHandler->SetSupervisor(g_supervisor);
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

        // getdata
        uid_t uid = GetData<uid_t>();
        pid_t pid = GetData<pid_t>();
        uint32_t typeId = GetData<uint32_t>();
        int32_t abilityId = GetData<int32_t>();
        auto cgroupEventHandler =
            std::make_shared<CgroupEventHandler>("CgroupEventHandler_fuzz");

        cgroupEventHandler->HandleContinuousTaskCancel(uid, pid, typeId, abilityId);
        cgroupEventHandler->SetSupervisor(g_supervisor);
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

        // getdata
        uint32_t resType = GetData<uint32_t>();
        int64_t value = GetData<int64_t>();
        nlohmann::json payload;
        auto cgroupEventHandler =
            std::make_shared<CgroupEventHandler>("CgroupEventHandler_fuzz");

        cgroupEventHandler->HandleReportMMIProcess(resType, value, payload);
        cgroupEventHandler->SetSupervisor(g_supervisor);
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

        // getdata
        uint32_t resType = GetData<uint32_t>();
        int64_t value = GetData<int64_t>();
        nlohmann::json payload;
        auto cgroupEventHandler =
            std::make_shared<CgroupEventHandler>("CgroupEventHandler_fuzz");

        cgroupEventHandler->HandleReportRenderThread(resType, value, payload);
        cgroupEventHandler->SetSupervisor(g_supervisor);
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

        // getdata
        uint32_t resType = GetData<uint32_t>();
        int64_t value = GetData<int64_t>();
        int32_t keyTid = GetData<int32_t>();
        int32_t role = GetData<int32_t>();
        nlohmann::json payload;
        payload["keyTid"] = std::to_string(keyTid);
        payload["role"] = std::to_string(role);
        auto cgroupEventHandler =
            std::make_shared<CgroupEventHandler>("CgroupEventHandler_fuzz");

        cgroupEventHandler->HandleReportKeyThread(resType, value, payload);
        cgroupEventHandler->SetSupervisor(g_supervisor);
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

        // getdata
        uint32_t resType = GetData<uint32_t>();
        int64_t value = GetData<int64_t>();
        int32_t windowId = GetData<int32_t>();
        int32_t state = GetData<int32_t>();
        int32_t nowSerialNum = GetData<int32_t>();
        nlohmann::json payload;
        payload["windowId"] = std::to_string(windowId);
        payload["state"] = std::to_string(state);
        payload["serialNum"] = std::to_string(nowSerialNum);
        auto cgroupEventHandler =
            std::make_shared<CgroupEventHandler>("CgroupEventHandler_fuzz");

        cgroupEventHandler->HandleReportWindowState(resType, value, payload);
        cgroupEventHandler->SetSupervisor(g_supervisor);
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

        // getdata
        uint32_t resType = GetData<uint32_t>();
        int64_t value = GetData<int64_t>();
        nlohmann::json payload;
        auto cgroupEventHandler =
            std::make_shared<CgroupEventHandler>("CgroupEventHandler_fuzz");

        cgroupEventHandler->HandleReportWebviewAudioState(resType, value, payload);
        cgroupEventHandler->SetSupervisor(g_supervisor);
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

        // getdata
        uint32_t resType = GetData<uint32_t>();
        int64_t value = GetData<int64_t>();
        nlohmann::json payload;
        auto cgroupEventHandler =
            std::make_shared<CgroupEventHandler>("CgroupEventHandler_fuzz");

        cgroupEventHandler->HandleReportRunningLockEvent(resType, value, payload);
        cgroupEventHandler->SetSupervisor(g_supervisor);
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

        // getdata
        uid_t uid = GetData<uid_t>();
        pid_t pid = GetData<pid_t>();
        uint32_t resType = GetData<uint32_t>();
        int64_t value = GetData<int64_t>();
        nlohmann::json payload;
        payload["uid"] = std::to_string(uid);
        payload["pid"] = std::to_string(pid);
        auto cgroupEventHandler =
            std::make_shared<CgroupEventHandler>("CgroupEventHandler_fuzz");

        cgroupEventHandler->HandleReportHisysEvent(resType, value, payload);
        cgroupEventHandler->SetSupervisor(g_supervisor);
        cgroupEventHandler->HandleReportHisysEvent(resType, value, payload);

        return true;
    }

    bool HandleReportBluetoothConnectStateFuzzTest(const uint8_t* data, size_t size)
    {
        if (data == nullptr) {
            return false;
        }

        // initialize
        G_DATA = data;
        g_size = size;

        // getdata
        uid_t uid = GetData<uid_t>();
        pid_t pid = GetData<pid_t>();
        uint32_t resType = GetData<uint32_t>();
        int64_t value = GetData<int64_t>();
        nlohmann::json payload;
        payload["uid"] = std::to_string(uid);
        payload["pid"] = std::to_string(pid);
        auto cgroupEventHandler =
            std::make_shared<CgroupEventHandler>("CgroupEventHandler_fuzz");

        cgroupEventHandler->HandleReportBluetoothConnectState(resType, value, payload);
        cgroupEventHandler->SetSupervisor(g_supervisor);
        cgroupEventHandler->HandleReportBluetoothConnectState(resType, value, payload);

        return true;
    }

    bool HandleMmiInputStateFuzzTest(const uint8_t* data, size_t size)
    {
        if (data == nullptr) {
            return false;
        }

        // initialize
        G_DATA = data;
        g_size = size;

        // getdata
        uid_t uid = GetData<uid_t>();
        pid_t pid = GetData<pid_t>();
        uint32_t resType = GetData<uint32_t>();
        int64_t value = GetData<int64_t>();
        nlohmann::json payload;
        payload["uid"] = std::to_string(uid);
        payload["pid"] = std::to_string(pid);
        auto cgroupEventHandler =
            std::make_shared<CgroupEventHandler>("CgroupEventHandler_fuzz");

        cgroupEventHandler->HandleMmiInputState(resType, value, payload);
        cgroupEventHandler->SetSupervisor(g_supervisor);
        cgroupEventHandler->HandleMmiInputState(resType, value, payload);

        return true;
    }

    bool HandleReportScreenCaptureEventFuzzTest(const uint8_t* data, size_t size)
    {
        if (data == nullptr) {
            return false;
        }

        // initialize
        G_DATA = data;
        g_size = size;

        // getdata
        uid_t uid = GetData<uid_t>();
        pid_t pid = GetData<pid_t>();
        uint32_t resType = GetData<uint32_t>();
        int64_t value = GetData<int64_t>();
        nlohmann::json payload;
        payload["uid"] = std::to_string(uid);
        payload["pid"] = std::to_string(pid);
        auto cgroupEventHandler =
            std::make_shared<CgroupEventHandler>("CgroupEventHandler_fuzz");

        cgroupEventHandler->HandleReportScreenCaptureEvent(resType, value, payload);
        cgroupEventHandler->SetSupervisor(g_supervisor);
        cgroupEventHandler->HandleReportScreenCaptureEvent(resType, value, payload);

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

        // getdata
        uid_t uid = GetData<uid_t>();
        pid_t pid = GetData<pid_t>();
        ProcessRecord pr(uid, pid);
        pr.processType_ = ProcRecordType::RENDER;
        pr.isActive_ = true;
        ProcessRecord mainProc(uid, pid);
        auto cgroupEventHandler =
            std::make_shared<CgroupEventHandler>("CgroupEventHandler_fuzz");

        cgroupEventHandler->CheckVisibilityForRenderProcess(pr, mainProc);
        cgroupEventHandler->SetSupervisor(g_supervisor);
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

        // getdata
        int32_t uid = GetData<int32_t>();
        int32_t pid = GetData<int32_t>();
        int32_t tid = GetData<int32_t>();
        int64_t value = GetData<int64_t>();
        nlohmann::json payload;
        auto cgroupEventHandler =
            std::make_shared<CgroupEventHandler>("CgroupEventHandler_fuzz");
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

        // getdata
        int32_t uid = GetData<int32_t>();
        int32_t pid = GetData<int32_t>();
        int32_t instanceId = GetData<int32_t>();
        uint32_t resType = GetData<uint32_t>();
        int64_t value = GetData<int64_t>();
        nlohmann::json payload;
        payload["uid"] = std::to_string(uid);
        payload["pid"] = std::to_string(pid);
        payload["instanceId"] = std::to_string(instanceId);
        auto cgroupEventHandler =
            std::make_shared<CgroupEventHandler>("CgroupEventHandler_fuzz");
        cgroupEventHandler->HandleReportAvCodecEvent(resType, value, payload);

        return true;
    }

    bool HandleSceneBoardStateFuzzTest(const uint8_t* data, size_t size)
    {
        if (data == nullptr) {
            return false;
        }

        // initialize
        G_DATA = data;
        g_size = size;

        // getdata
        int32_t pid = GetData<int32_t>();
        uint32_t resType = GetData<uint32_t>();
        int64_t value = GetData<int64_t>();
        nlohmann::json payload;
        payload["pid"] = std::to_string(pid);
        auto cgroupEventHandler =
            std::make_shared<CgroupEventHandler>("CgroupEventHandler_fuzz");

        cgroupEventHandler->HandleSceneBoardState(resType, value, payload);
        cgroupEventHandler->SetSupervisor(g_supervisor);
        cgroupEventHandler->HandleSceneBoardState(resType, value, payload);

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

        // getdata
        uint32_t resType = GetData<uint32_t>();
        int64_t value = GetData<int64_t>();
        int32_t uid = GetData<int32_t>();
        int32_t pid = GetData<int32_t>();
        uint32_t windowId = GetData<int32_t>();
        uint32_t windowType = GetData<int32_t>();
        uint32_t visibilityState = GetData<uint32_t>();

        nlohmann::json payload;
        payload["uid"] = std::to_string(uid);
        payload["pid"] = std::to_string(pid);
        payload["windowId"] = std::to_string(windowId);
        payload["windowType"] = std::to_string(windowType);
        payload["visibilityState"] = std::to_string(visibilityState);
        auto cgroupEventHandler =
            std::make_shared<CgroupEventHandler>("CgroupEventHandler_fuzz");

        cgroupEventHandler->HandleWindowVisibilityChanged(resType, value, payload);
        cgroupEventHandler->SetSupervisor(g_supervisor);
        cgroupEventHandler->HandleWindowVisibilityChanged(resType, value, payload);

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

        // getdata
        uint32_t resType = GetData<uint32_t>();
        int64_t value = GetData<int64_t>();
        int32_t uid = GetData<int32_t>();
        int32_t pid = GetData<int32_t>();
        uint32_t windowId = GetData<int32_t>();
        uint32_t windowType = GetData<int32_t>();
        uint32_t drawingContentState = GetData<uint32_t>();

        nlohmann::json payload;
        payload["uid"] = std::to_string(uid);
        payload["pid"] = std::to_string(pid);
        payload["windowId"] = std::to_string(windowId);
        payload["windowType"] = std::to_string(windowType);
        payload["drawingContentState"] = std::to_string(drawingContentState);
        auto cgroupEventHandler =
            std::make_shared<CgroupEventHandler>("CgroupEventHandler_fuzz");

        cgroupEventHandler->HandleDrawingContentChangeWindow(resType, value, payload);
        cgroupEventHandler->SetSupervisor(g_supervisor);
        cgroupEventHandler->HandleDrawingContentChangeWindow(resType, value, payload);

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

        // getdata
        int32_t uid = GetData<int32_t>();
        int32_t pid = GetData<int32_t>();
        uint32_t windowId = GetData<int32_t>();
        uint64_t displayId = GetData<uint64_t>();
        auto cgroupEventHandler =
            std::make_shared<CgroupEventHandler>("CgroupEventHandler_fuzz");

        cgroupEventHandler->SetSupervisor(g_supervisor);
        cgroupEventHandler->HandleUnfocusedWindow(
            windowId, 1, displayId, pid, uid);

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

        // getdata
        int32_t uid = GetData<int32_t>();
        int32_t pid = GetData<int32_t>();
        uint32_t windowId = GetData<int32_t>();
        uint64_t displayId = GetData<uint64_t>();
        auto cgroupEventHandler =
            std::make_shared<CgroupEventHandler>("CgroupEventHandler_fuzz");
        cgroupEventHandler->SetSupervisor(g_supervisor);
        cgroupEventHandler->HandleFocusedWindow(
            windowId, 1, displayId, pid, uid);

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

        // getdata
        uint32_t resType = GetData<uint32_t>();
        int64_t value = GetData<int64_t>();
        uid_t uid = GetData<uid_t>();
        pid_t pid = GetData<pid_t>();
        std::string packageName(std::to_string(*data));
        int32_t state = GetData<int32_t>();
        nlohmann::json payload;
        payload["uid"] = std::to_string(uid);
        payload["pid"] = std::to_string(pid);
        payload["bundleName"] = packageName;
        payload["state"] = std::to_string(state);
        auto cgroupEventHandler =
            std::make_shared<CgroupEventHandler>("CgroupEventHandler_fuzz");

        cgroupEventHandler->HandleApplicationStateChanged(resType, value, payload);
        cgroupEventHandler->SetSupervisor(g_supervisor);
        cgroupEventHandler->HandleApplicationStateChanged(resType, value, payload);

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

        // getdata
        uint32_t resType = GetData<uint32_t>();
        int64_t value = GetData<int64_t>();
        uid_t uid = GetData<uid_t>();
        pid_t pid = GetData<pid_t>();
        std::string packageName(std::to_string(*data));
        int32_t state = GetData<int32_t>();
        nlohmann::json payload;
        payload["uid"] = std::to_string(uid);
        payload["pid"] = std::to_string(pid);
        payload["bundleName"] = packageName;
        payload["state"] = std::to_string(state);
        auto cgroupEventHandler =
            std::make_shared<CgroupEventHandler>("CgroupEventHandler_fuzz");

        cgroupEventHandler->SetSupervisor(g_supervisor);
        cgroupEventHandler->HandleProcessStateChanged(resType, value, payload);

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

        // getdata
        uint32_t resType = GetData<uint32_t>();
        int64_t value = GetData<int64_t>();
        uid_t uid = GetData<uid_t>();
        pid_t pid = GetData<pid_t>();
        std::string bundleName(std::to_string(*data));
        std::string abilityName(std::to_string(*data));
        int32_t recordId = GetData<int32_t>();
        int32_t abilityState = GetData<int32_t>();
        int32_t abilityType = GetData<int32_t>();
        nlohmann::json payload;
        payload["uid"] = std::to_string(uid);
        payload["pid"] = std::to_string(pid);
        payload["bundleName"] = bundleName;
        payload["abilityName"] = abilityName;
        payload["recordId"] = std::to_string(recordId);
        payload["abilityState"] = std::to_string(abilityState);
        payload["abilityType"] = std::to_string(abilityType);
        auto cgroupEventHandler =
            std::make_shared<CgroupEventHandler>("CgroupEventHandler_fuzz");

        cgroupEventHandler->HandleAbilityStateChanged(resType, value, payload);
        cgroupEventHandler->SetSupervisor(g_supervisor);
        cgroupEventHandler->HandleAbilityStateChanged(resType, value, payload);

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

        // getdata
        uint32_t resType = GetData<uint32_t>();
        int64_t value = GetData<int64_t>();
        uid_t uid = GetData<uid_t>();
        pid_t pid = GetData<pid_t>();
        std::string bundleName(std::to_string(*data));
        std::string abilityName(std::to_string(*data));
        int32_t recordId = GetData<int32_t>();
        int32_t extensionState = GetData<int32_t>();
        int32_t abilityType = GetData<int32_t>();
        nlohmann::json payload;
        payload["uid"] = std::to_string(uid);
        payload["pid"] = std::to_string(pid);
        payload["bundleName"] = bundleName;
        payload["abilityName"] = abilityName;
        payload["recordId"] = std::to_string(recordId);
        payload["extensionState"] = std::to_string(extensionState);
        payload["abilityType"] = std::to_string(abilityType);
        auto cgroupEventHandler =
            std::make_shared<CgroupEventHandler>("CgroupEventHandler_fuzz");

        cgroupEventHandler->HandleExtensionStateChanged(resType, value, payload);
        cgroupEventHandler->SetSupervisor(g_supervisor);
        cgroupEventHandler->HandleExtensionStateChanged(resType, value, payload);

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

        // getdata
        uint32_t resType = GetData<uint32_t>();
        int64_t value = GetData<int64_t>();
        uid_t uid = GetData<uid_t>();
        pid_t pid = GetData<pid_t>();
        std::string bundleName = std::to_string(*data);
        int32_t hostPid = GetData<int32_t>();
        int32_t processType = GetData<int32_t>();
        int32_t extensionType = GetData<int32_t>();
        nlohmann::json payload;
        payload["uid"] = std::to_string(uid);
        payload["pid"] = std::to_string(pid);
        payload["bundleName"] = bundleName;
        payload["hostPid"] = std::to_string(hostPid);
        payload["processType"] = std::to_string(processType);
        payload["extensionType"] = std::to_string(extensionType);
        auto cgroupEventHandler =
            std::make_shared<CgroupEventHandler>("CgroupEventHandler_fuzz");

        cgroupEventHandler->SetSupervisor(g_supervisor);
        cgroupEventHandler->HandleProcessCreated(resType, value, payload);

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

        // getdata
        uint32_t resType = GetData<uint32_t>();
        int64_t value = GetData<int64_t>();
        nlohmann::json payload;
        auto cgroupEventHandler =
            std::make_shared<CgroupEventHandler>("CgroupEventHandler_fuzz");

        cgroupEventHandler->HandleReportAudioState(resType, value, payload);
        cgroupEventHandler->SetSupervisor(g_supervisor);
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

        // getdata
        uint32_t resType = GetData<uint32_t>();
        int64_t value = GetData<int64_t>();
        nlohmann::json payload;
        auto cgroupEventHandler =
            std::make_shared<CgroupEventHandler>("CgroupEventHandler_fuzz");

        cgroupEventHandler->HandleWebviewScreenCapture(resType, value, payload);

        return true;
    }

    bool HandleReportCosmicCubeStateFuzzTest(const uint8_t* data, size_t size)
    {
        if (data == nullptr) {
            return false;
        }

        // initialize
        G_DATA = data;
        g_size = size;

        // getdata
        uint32_t resType = GetData<uint32_t>();
        int64_t value = GetData<int64_t>();
        nlohmann::json payload;
        auto cgroupEventHandler =
            std::make_shared<CgroupEventHandler>("CgroupEventHandler_fuzz");

        cgroupEventHandler->HandleReportCosmicCubeState(resType, value, payload);

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

        // getdata
        uint32_t resType = GetData<uint32_t>();
        int64_t value = GetData<int64_t>();
        nlohmann::json payload;
        auto cgroupEventHandler =
            std::make_shared<CgroupEventHandler>("CgroupEventHandler_fuzz");

        cgroupEventHandler->HandleReportWebviewVideoState(resType, value, payload);

        return true;
    }

    bool HandleOnAppStoppedFuzzTest(const uint8_t* data, size_t size)
    {
        if (data == nullptr) {
            return false;
        }

        // initialize
        G_DATA = data;
        g_size = size;

        // getdata
        uint32_t resType = GetData<uint32_t>();
        int64_t value = GetData<int64_t>();
        nlohmann::json payload;
        auto cgroupEventHandler =
            std::make_shared<CgroupEventHandler>("CgroupEventHandler_fuzz");
        cgroupEventHandler->HandleOnAppStopped(resType, value, payload);
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

        // getdata
        uid_t uid = GetData<uid_t>();
        pid_t pid = GetData<pid_t>();
        ProcessRecord pr(uid, pid);
        Application app(uid);

        CgroupAdjuster::GetInstance().ApplyProcessGroup(app, pr);

        return true;
    }

    bool DispatchResourceFuzzTest(const uint8_t* data, size_t size)
    {
        if (data == nullptr) {
            return false;
        }

        // initialize
        G_DATA = data;
        g_size = size;

        // getdata
        uint32_t resType = GetData<uint32_t>();
        int64_t value = GetData<int64_t>();
        nlohmann::json payload;
        auto resData = std::make_shared<ResData>(resType, value, payload);
        auto sched_Controller = std::make_unique<SchedController>();
        sched_Controller->DispatchResource(resData);

        return true;
    }

    bool DispatchOtherResourceFuzzTest(const uint8_t* data, size_t size)
    {
        if (data == nullptr) {
            return false;
        }

        // initialize
        G_DATA = data;
        g_size = size;

        // getdata
        uint32_t resType = GetData<uint32_t>();
        int64_t value = GetData<int64_t>();
        nlohmann::json payload;
        auto sched_Controller = std::make_shared<SchedController>();
        sched_Controller->DispatchOtherResource(resType, value, payload);

        return true;
    }

    bool DumpProcessRunningLockFuzzTest(const uint8_t* data, size_t size)
    {
        if (data == nullptr) {
            return false;
        }

        // initialize
        G_DATA = data;
        g_size = size;

        // getdata
        std::string result(std::to_string(*data));
        auto sched_Controller = std::make_shared<SchedController>();
        sched_Controller->Init();
        sched_Controller->DumpProcessRunningLock(result);
        sched_Controller->Disable();
        return true;
    }

    bool DumpProcessEventStateFuzzTest(const uint8_t* data, size_t size)
    {
        if (data == nullptr) {
            return false;
        }

        // initialize
        G_DATA = data;
        g_size = size;

        // getdata
        std::string result(std::to_string(*data));
        auto sched_Controller = std::make_shared<SchedController>();
        sched_Controller->Init();
        sched_Controller->DumpProcessEventState(result);
        sched_Controller->Disable();
        return true;
    }

    bool DumpProcessWindowInfoFuzzTest(const uint8_t* data, size_t size)
    {
        if (data == nullptr) {
            return false;
        }

        // initialize
        G_DATA = data;
        g_size = size;

        // getdata
        std::string result(std::to_string(*data));
        auto sched_Controller = std::make_shared<SchedController>();
        sched_Controller->Init();
        sched_Controller->DumpProcessWindowInfo(result);
        sched_Controller->Disable();
        return true;
    }

    bool AddTidToCgroupFuzzTest(const uint8_t* data, size_t size)
    {
        if (data == nullptr) {
            return false;
        }

        // initialize
        G_DATA = data;
        g_size = size;

        // getdata
        int tid = GetData<int>();
        int fd = GetData<int>();
        auto cgroupController = std::make_shared<CgroupSetting::CgroupController>("cpuset", "/dev/cpuset");
        cgroupController->AddTidToCgroup(tid, fd);
        return true;
    }

    bool AddSchedPolicyFuzzTest(const uint8_t* data, size_t size)
    {
        if (data == nullptr) {
            return false;
        }

        // initialize
        G_DATA = data;
        g_size = size;

        // getdata
        std::string subGroup(std::to_string(*data));
        auto cgroupController = std::make_shared<CgroupSetting::CgroupController>("cpuset", "/dev/cpuset");
        cgroupController->AddSchedPolicy(CgroupSetting::SP_BACKGROUND, subGroup);
        return true;
    }
    
    bool AddThreadSchedPolicyFuzzTest(const uint8_t* data, size_t size)
    {
        if (data == nullptr) {
            return false;
        }

        // initialize
        G_DATA = data;
        g_size = size;

        // getdata
        std::string subGroup(std::to_string(*data));
        auto cgroupController = std::make_shared<CgroupSetting::CgroupController>("cpuset", "/dev/cpuset");
        cgroupController->AddThreadSchedPolicy(CgroupSetting::SP_BACKGROUND, subGroup);
        return true;
    }

    bool AddThreadGroupSchedPolicyFuzzTest(const uint8_t* data, size_t size)
    {
        if (data == nullptr) {
            return false;
        }

        // initialize
        G_DATA = data;
        g_size = size;

        // getdata
        std::string subGroup(std::to_string(*data));
        auto cgroupController = std::make_shared<CgroupSetting::CgroupController>("cpuset", "/dev/cpuset");
        cgroupController->AddThreadGroupSchedPolicy(CgroupSetting::SP_BACKGROUND, subGroup);
        return true;
    }

    bool SetPos()
    {
        g_pos = 0;
        return true;
    }

    void BackgroundTaskObserverFuzzExecute(const uint8_t* data, size_t size)
    {
        OHOS::ResourceSchedule::TransientTaskStartFuzzTest(data, size);
        OHOS::ResourceSchedule::TransientTaskEndFuzzTest(data, size);
        OHOS::ResourceSchedule::MarshallingContinuousTaskCallbackInfoFuzzTest(data, size);
        OHOS::ResourceSchedule::ContinuousTaskStartFuzzTest(data, size);
        OHOS::ResourceSchedule::ContinuousTaskStopFuzzTest(data, size);
        OHOS::ResourceSchedule::ContinuousTaskUpdateFuzzTest(data, size);
        OHOS::ResourceSchedule::RemoteDiedFuzzTest(data, size);
    }

    void CgroupEventHandlerFuzzExecute(const uint8_t* data, size_t size)
    {
        OHOS::ResourceSchedule::ProcessEventFuzzTest(data, size);
        OHOS::ResourceSchedule::HandleProcessDiedFuzzTest(data, size);
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
        OHOS::ResourceSchedule::HandleMmiInputStateFuzzTest(data, size);
        OHOS::ResourceSchedule::HandleReportBluetoothConnectStateFuzzTest(data, size);
        OHOS::ResourceSchedule::CheckVisibilityForRenderProcessFuzzTest(data, size);
        OHOS::ResourceSchedule::ParsePayloadFuzzTest(data, size);
        OHOS::ResourceSchedule::HandleReportAvCodecEventFuzzTest(data, size);
        OHOS::ResourceSchedule::HandleSceneBoardStateFuzzTest(data, size);
        OHOS::ResourceSchedule::HandleWindowVisibilityChangedFuzzTest(data, size);
        OHOS::ResourceSchedule::HandleDrawingContentChangeWindowFuzzTest(data, size);
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
        OHOS::ResourceSchedule::HandleReportCosmicCubeStateFuzzTest(data, size);
        OHOS::ResourceSchedule::HandleOnAppStoppedFuzzTest(data, size);
    }

    void CgroupAdjusterFuzzExecute(const uint8_t* data, size_t size)
    {
        OHOS::ResourceSchedule::AdjustForkProcessGroupFuzzTest(data, size);
        OHOS::ResourceSchedule::AdjustProcessGroupFuzzTest(data, size);
        OHOS::ResourceSchedule::AdjustAllProcessGroupFuzzTest(data, size);
        OHOS::ResourceSchedule::ComputeProcessGroupFuzzTest(data, size);
        OHOS::ResourceSchedule::ApplyProcessGroupFuzzTest(data, size);
    }

    void SchedControllerFuzzExecute(const uint8_t* data, size_t size)
    {
        OHOS::ResourceSchedule::DispatchResourceFuzzTest(data, size);
        OHOS::ResourceSchedule::DispatchOtherResourceFuzzTest(data, size);
        OHOS::ResourceSchedule::DumpProcessRunningLockFuzzTest(data, size);
        OHOS::ResourceSchedule::DumpProcessEventStateFuzzTest(data, size);
        OHOS::ResourceSchedule::DumpProcessWindowInfoFuzzTest(data, size);
    }

    void CgroupControllerFuzzExecute(const uint8_t* data, size_t size)
    {
        OHOS::ResourceSchedule::AddTidToCgroupFuzzTest(data, size);
        OHOS::ResourceSchedule::AddSchedPolicyFuzzTest(data, size);
        OHOS::ResourceSchedule::AddThreadSchedPolicyFuzzTest(data, size);
        OHOS::ResourceSchedule::AddThreadGroupSchedPolicyFuzzTest(data, size);
    }

} // namespace ResourceSchedule
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    OHOS::ResourceSchedule::SetPos();
    /* Run your code on data */
    // background_task_observer.cpp
    OHOS::ResourceSchedule::BackgroundTaskObserverFuzzExecute(data, size);
    // background_task_observer.cpp end

    // cgroup_event_handler.cpp
    OHOS::ResourceSchedule::CgroupEventHandlerFuzzExecute(data, size);
    // cgroup_event_handler.cpp end

    // cgroup_adjuster.cpp
    OHOS::ResourceSchedule::CgroupAdjusterFuzzExecute(data, size);
    // cgroup_adjuster.cpp end

    // sched_controller.cpp
    OHOS::ResourceSchedule::SchedControllerFuzzExecute(data, size);
    // sched_controller.cpp end

    // cgroup_controller.cpp
    OHOS::ResourceSchedule::CgroupControllerFuzzExecute(data, size);
    // cgroup_controller.cpp end

    return 0;
}
