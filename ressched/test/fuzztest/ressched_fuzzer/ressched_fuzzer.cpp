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

#include "ressched_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <securec.h>
#include <vector>
#include <unordered_set>
#include <random>

#include "iremote_stub.h"
#include "ires_sched_service.h"
#include "iservice_registry.h"
#include "oobe_datashare_utils.h"
#include "oobe_manager.h"
#include "ioobe_task.h"
#include "plugin_mgr.h"
#include "res_sched_service.h"
#include "singleton.h"
#include "system_ability_definition.h"
#include "res_sched_client.h"
#include "res_sched_service_stub.h"
#include "res_sched_systemload_notifier_client.h"
#include "res_sched_systemload_notifier_proxy.h"
#include "notifier_mgr.h"
#include "res_type.h"
#include "accesstoken_kit.h"
#include "token_setproc.h"

#define private public
#define protected public

#ifndef errno_t
typedef int errno_t;
#endif

#ifndef EOK
#define EOK 0
#endif

namespace OHOS {
namespace ResourceSchedule {
namespace {
    static const int32_t TWO_PARAMETERS = 2;

    static const std::unordered_set<uint32_t> THIRDPARTY_RES = {
        ResType::RES_TYPE_CLICK_RECOGNIZE,
        ResType::RES_TYPE_PUSH_PAGE,
        ResType::RES_TYPE_SLIDE_RECOGNIZE,
        ResType::RES_TYPE_POP_PAGE,
        ResType::RES_TYPE_LOAD_PAGE,
        ResType::RES_TYPE_WEB_GESTURE,
        ResType::RES_TYPE_REPORT_KEY_THREAD,
        ResType::RES_TYPE_REPORT_WINDOW_STATE,
        ResType::RES_TYPE_REPORT_SCENE_SCHED,
        ResType::RES_TYPE_WEB_GESTURE_MOVE,
        ResType::RES_TYPE_WEB_SLIDE_NORMAL,
        ResType::RES_TYPE_LOAD_URL,
        ResType::RES_TYPE_MOUSEWHEEL,
        ResType::RES_TYPE_WEBVIEW_AUDIO_STATUS_CHANGE,
        ResType::RES_TYPE_REPORT_RENDER_THREAD,
        ResType::RES_TYPE_LONG_FRAME,
        ResType::RES_TYPE_AUDIO_SILENT_PLAYBACK,
        ResType::RES_TYPE_REPORT_DISTRIBUTE_TID,
        ResType::RES_TYPE_WEBVIEW_SCREEN_CAPTURE,
        ResType::RES_TYPE_WEBVIEW_VIDEO_STATUS_CHANGE,
        ResType::RES_TYPE_BT_SERVICE_EVENT
    };

    static const std::unordered_map<uint32_t, std::vector<std::string>> RESTYPE_TO_PARAMS = {
        {ResType::RES_TYPE_CLICK_RECOGNIZE, {"clientPid", "name"}},
        {ResType::RES_TYPE_PUSH_PAGE, {"pageUrl"}},
        {ResType::RES_TYPE_SLIDE_RECOGNIZE, {"clientPid"}},
        {ResType::RES_TYPE_POP_PAGE, {}},
        {ResType::RES_TYPE_LOAD_PAGE, {}},
        {ResType::RES_TYPE_WEB_GESTURE, {}},
        {ResType::RES_TYPE_REPORT_KEY_THREAD, {"uid", "pid", "tid", "role"}},
        {ResType::RES_TYPE_REPORT_WINDOW_STATE, {"uid", "pid", "windowId", "serialNum", "state"}},
        {ResType::RES_TYPE_REPORT_SCENE_SCHED, {"uid", "sceneId"}},
        {ResType::RES_TYPE_WEB_GESTURE_MOVE, {}},
        {ResType::RES_TYPE_WEB_SLIDE_NORMAL, {}},
        {ResType::RES_TYPE_LOAD_URL, {}},
        {ResType::RES_TYPE_MOUSEWHEEL, {}},
        {ResType::RES_TYPE_WEBVIEW_AUDIO_STATUS_CHANGE, {"uid", "pid", "tid"}},
        {ResType::RES_TYPE_REPORT_RENDER_THREAD, {"uid", "pid"}},
        {ResType::RES_TYPE_LONG_FRAME, {"pid", "uid", "bundleName", "abilityName"}},
        {ResType::RES_TYPE_AUDIO_SILENT_PLAYBACK, {"uid"}},
        {ResType::RES_TYPE_REPORT_DISTRIBUTE_TID, {"uid", "pid"}},
        {ResType::RES_TYPE_WEBVIEW_SCREEN_CAPTURE, {"uid", "pid"}},
        {ResType::RES_TYPE_WEBVIEW_VIDEO_STATUS_CHANGE, {"uid", "pid"}},
        {ResType::RES_TYPE_BT_SERVICE_EVENT, {"ADDRESS", "STATE", "ROLE", "CONNECTIF", "STATUS"}}
    };

    static const int32_t DEFAULT_API_VERSION = 11;
    static const int32_t PAYLOAD_MAX_SIZE = 3500;

    Security::AccessToken::PermissionStateFull HapState = {
        .permissionName = "",
        .isGeneral = true,
        .resDeviceID = {"local"},
        .grantStatus = {Security::AccessToken::PermissionState::PERMISSION_GRANTED},
        .grantFlags = {1}
    };

    Security::AccessToken::HapPolicyParams HapPolicyParams = {
        .apl = Security::AccessToken::APL_SYSTEM_BASIC,
        .domain = "test.domain.ressched",
        .permList = {},
        .permStateList = {HapState}
    };

    Security::AccessToken::HapInfoParams info = {
        .userID = 100,
        .bundleName = "com.hos.ressched",
        .instIndex = 0,
        .appIDDesc = "thirdParty",
        .apiVersion = DEFAULT_API_VERSION,
        .isSystemApp = false
    };
}

    constexpr int32_t MAX_CODE = 5;
    constexpr int32_t MIN_LEN = 4;
    std::mutex mutexLock;
    sptr<IRemoteObject> remoteObj;
    const uint8_t* g_data = nullptr;
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
        if (ret != EOK) {
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

    bool DoInit()
    {
        std::lock_guard<std::mutex> lock(mutexLock);
        if (remoteObj) {
            return true;
        }
        auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        if (!samgr) {
            return false;
        }
        remoteObj = samgr->GetSystemAbility(RES_SCHED_SYS_ABILITY_ID);
        if (!remoteObj) {
            return false;
        }
        return true;
    }

    int32_t onRemoteRequest(uint32_t code, MessageParcel& data)
    {
        if (!DoInit()) {
            return -1;
        }
        MessageParcel reply;
        MessageOption option;
        return remoteObj->SendRequest(code, data, reply, option);
    }

    bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
    {
        if (size <= MIN_LEN) {
            return false;
        }

        MessageParcel dataMessageParcel;
        if (!dataMessageParcel.WriteInterfaceToken(IRemoteStub<IResSchedService>::GetDescriptor())) {
            return false;
        }

        uint32_t code = *(reinterpret_cast<const uint32_t*>(data));
        size -= sizeof(uint32_t);

        dataMessageParcel.WriteBuffer(data + sizeof(uint32_t), size);
        dataMessageParcel.RewindRead(0);

        onRemoteRequest(code, dataMessageParcel);
        return true;
    }

    bool OnRemoteRequest(const uint8_t* data, size_t size)
    {
        if (data == nullptr) {
            return false;
        }

        if (size <= MIN_LEN) {
            return false;
        }

        // initialize
        g_data = data;
        g_size = size;
        g_pos = 0;

        // getdata
        uint32_t fuzzCode = GetData<uint32_t>();
        MessageParcel fuzzData;
        fuzzData.WriteInterfaceToken(ResSchedServiceStub::GetDescriptor());
        fuzzData.WriteBuffer(g_data + g_pos, g_size - g_pos);
        fuzzData.RewindRead(0);
        MessageParcel fuzzReply;
        MessageOption fuzzOption;
        DelayedSingleton<ResSchedService>::GetInstance()->OnRemoteRequest(fuzzCode % MAX_CODE,
            fuzzData, fuzzReply, fuzzOption);
        return true;
    }

    bool SyncEventFuzzTest(const uint8_t* data, size_t size)
    {
        if (data == nullptr) {
            return false;
        }

        if (size <= MIN_LEN) {
            return false;
        }

        // initialize
        g_data = data;
        g_size = size;
        g_pos = 0;

        // getdata
        MessageParcel fuzzData;
        fuzzData.WriteInterfaceToken(ResSchedServiceStub::GetDescriptor());
        fuzzData.WriteBuffer(g_data + g_pos, g_size - g_pos);
        fuzzData.RewindRead(0);
        MessageParcel fuzzReply;
        DelayedSingleton<ResSchedService>::GetInstance()->ReportSyncEventInner(fuzzData, fuzzReply);
        return true;
    }

    bool ResSchedClientFuzzTest(const uint8_t* data, size_t size)
    {
        if (data == nullptr) {
            return false;
        }

        if (size <= sizeof(uint32_t) + sizeof(int64_t) + TWO_PARAMETERS * sizeof(std::string)) {
            return false;
        }

        // initialize
        g_data = data;
        g_size = size;
        g_pos = 0;

        uint32_t resType = GetData<uint32_t>();
        int64_t value = GetData<int64_t>();
        std::unordered_map<std::string, std::string> mapPayload;
        mapPayload["pid"] = GetStringFromData(int(size) - sizeof(uint32_t) - sizeof(int64_t));
        mapPayload["processName"] = GetStringFromData(int(size) - sizeof(std::string) -
        sizeof(uint32_t) - sizeof(int64_t));

        ResSchedClient::GetInstance().ReportData(resType, value, mapPayload);
        ResSchedClient::GetInstance().KillProcess(mapPayload);
        ResSchedClient::GetInstance().StopRemoteObject();

        sptr<ResSchedSystemloadNotifierClient> callbackObj;
        ResSchedClient::GetInstance().RegisterSystemloadNotifier(callbackObj);
        ResSchedClient::GetInstance().UnRegisterSystemloadNotifier(callbackObj);
        ResSchedClient::GetInstance().GetSystemloadLevel();
        return true;
    }

    bool OnSystemloadLevelFuzzTest(const uint8_t* data, size_t size)
    {
        if (data == nullptr) {
            return false;
        }

        // initialize
        g_data = data;
        g_size = size;
        g_pos = 0;

        int32_t level = GetData<int32_t>();
        if (!DoInit()) {
            return false;
        }
        auto resSchedSystemloadNotifierProxy = std::make_unique<ResSchedSystemloadNotifierProxy>(remoteObj);
        resSchedSystemloadNotifierProxy->OnSystemloadLevel(level);
        return true;
    }

    bool NotifierMgrFuzzTest(const uint8_t* data, size_t size)
    {
        if (data == nullptr) {
            return false;
        }

        // initialize
        g_data = data;
        g_size = size;
        g_pos = 0;

        int32_t pid = GetData<int32_t>();
        int32_t type = GetData<int32_t>();
        int32_t level = GetData<int32_t>();
        int32_t state = GetData<int32_t>();
        if (!DoInit()) {
            return false;
        }

        NotifierMgr::GetInstance().Init();
        NotifierMgr::GetInstance().RegisterNotifier(pid, remoteObj);
        NotifierMgr::GetInstance().UnRegisterNotifier(pid);
        NotifierMgr::GetInstance().OnRemoteNotifierDied(remoteObj);
        NotifierMgr::GetInstance().OnDeviceLevelChanged(type, level);
        NotifierMgr::GetInstance().OnApplicationStateChange(state, level);
        NotifierMgr::GetInstance().GetSystemloadLevel();
        NotifierMgr::GetInstance().DumpRegisterInfo();
        NotifierMgr::GetInstance().Deinit();
        return true;
    }

    bool OOBEManagerFuzzTest(const uint8_t* data, size_t size)
    {
        if (data == nullptr) {
            return false;
        }

        if (size <= sizeof(std::string)) {
            return false;
        }

        std::string key = GetStringFromData(int(size));
        OOBEManager::ResDataAbilityObserver::UpdateFunc updateFunc = [&]() {};
        if (!DoInit()) {
            return false;
        }

        OOBEManager::GetInstance().GetOOBValue();
        OOBEManager::GetInstance().RegisterObserver(key, updateFunc);
        OOBEManager::GetInstance().UnregisterObserver();
        sptr<OOBEManager::ResDataAbilityObserver> oobeObserver = new OOBEManager::ResDataAbilityObserver();
        oobeObserver->OnChange();
        oobeObserver->SetUpdateFunc(updateFunc);
        OOBEManager::GetInstance().Initialize();
        OOBEManager::GetInstance().StartListen();
        return true;
    }

    bool OOBEDatashareUtilsFuzzTest(const uint8_t* data, size_t size)
    {
        if (data == nullptr) {
            return false;
        }

        if (size <= TWO_PARAMETERS * sizeof(std::string)) {
            return false;
        }

        std::string key = GetStringFromData(int(size));
        std::string value = GetStringFromData(int(size));
        if (!DoInit()) {
            return false;
        }

        DataShareUtils::GetInstance().GetValue(key, value);
        DataShareUtils::GetInstance().GetStringValue(key, value);
        std::shared_ptr<DataShare::DataShareHelper> helper = DataShareUtils::GetInstance().CreateDataShareHelper();
        DataShareUtils::GetInstance().ReleaseDataShareHelper(helper);
        DataShareUtils::GetInstance().InitSystemAbilityManager();
        DataShareUtils::GetInstance().AssembleUri(key);
        DataShareUtils::GetInstance().GetDataShareReadyFlag();
        DataShareUtils::GetInstance().SetDataShareReadyFlag(true);
        return true;
    }

    void SetHapToken()
    {
        Security::AccessToken::AccessTokenIDEx tokenIdEx = {0};
        tokenIdEx = Security::AccessToken::AccessTokenKit::AllocHapToken(info, HapPolicyParams);
        SetSelfTokenID(tokenIdEx.tokenIDEx);
    }

    void DeleteHapToken()
    {
        Security::AccessToken::AccessTokenID tokenId =
            Security::AccessToken::AccessTokenKit::GetHapTokenID(info.userID, info.bundleName, info.instIndex);
        Security::AccessToken::AccessTokenKit::DeleteToken(tokenId);
    }

    std::unordered_map<std::string, std::string> GetPayload(uint32_t resType)
    {
        std::unordered_map<std::string, std::string> payload;
        const auto &params = RESTYPE_TO_PARAMS.at(resType);
        size_t paramSize = params.size();
        if (paramSize == 0) {
            return payload;
        }
        size_t maxLen = PAYLOAD_MAX_SIZE / paramSize;
        size_t minLen = sizeof(int32_t);
        if (minLen > maxLen) {
            minLen = maxLen;
        }
        std::mt19937_64 gen(std::random_device{}());
        std::uniform_int_distribution<size_t> dis(minLen, maxLen);
        for (const auto &param : params) {
            payload[param] = GetStringFromData(dis(gen));
        }
        return payload;
    }

    bool ResSchedThirdPartyFuzzTest(const uint8_t* data, size_t size)
    {
        if (data == nullptr || size < sizeof(uint32_t)) {
            return false;
        }
        uint32_t selfToken = GetSelfTokenID();
        SetHapToken();
        g_data = data;
        g_size = size;
        g_pos = 0;

        uint32_t resType = GetData<uint32_t>() % ResType::RES_TYPE_LAST;
        if (THIRDPARTY_RES.find(resType) == THIRDPARTY_RES.end()) {
            return false;
        }

        int64_t value = GetData<int64_t>();
        auto mapPayload = GetPayload(resType);
        ResSchedClient::GetInstance().ReportData(resType, value, mapPayload);

        DeleteHapToken();
        SetSelfTokenID(selfToken);
        return true;
    }
} // namespace ResourceSchedule
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::ResourceSchedule::DoSomethingInterestingWithMyAPI(data, size);
    OHOS::ResourceSchedule::OnRemoteRequest(data, size);
    OHOS::ResourceSchedule::SyncEventFuzzTest(data, size);
    OHOS::ResourceSchedule::ResSchedClientFuzzTest(data, size);
    OHOS::ResourceSchedule::ResSchedThirdPartyFuzzTest(data, size);
    OHOS::ResourceSchedule::OnSystemloadLevelFuzzTest(data, size);
    OHOS::ResourceSchedule::NotifierMgrFuzzTest(data, size);
    OHOS::ResourceSchedule::OOBEManagerFuzzTest(data, size);
    OHOS::ResourceSchedule::OOBEDatashareUtilsFuzzTest(data, size);
    return 0;
}
