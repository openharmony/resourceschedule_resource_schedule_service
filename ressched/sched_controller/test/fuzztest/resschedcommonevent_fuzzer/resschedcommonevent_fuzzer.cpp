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


#include "resschedcommonevent_fuzzer.h"

#include "event_controller.h"
#include "common_event_support.h"

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

namespace {
    static const int32_t THREE_PARAMETERS = 3;
}
    const uint8_t* DATA = nullptr;
    size_t g_size = 0;
    size_t g_pos;

/*
* describe: get data from outside untrusted data(DATA) which size is according to sizeof(T)
* tips: only support basic type
*/
    template<class T>
    T GetData()
    {
        T object {};
        size_t objectSize = sizeof(object);
        if (DATA == nullptr || objectSize > g_size - g_pos) {
            return object;
        }
        errno_t ret = memcpy_s(&object, objectSize, DATA + g_pos, objectSize);
        if (ret != EOK) {
            return {};
        }
        g_pos += objectSize;
        return object;
    }

/*
* get a string from DATA
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

    bool ResSchedCommonEventFuzzTest(const uint8_t* data, size_t size)
    {
        if (data == nullptr) {
            return false;
        }

        if (size <= sizeof(int32_t) + sizeof(std::string)) {
            return false;
        }

        // initialize
        DATA = data;
        g_size = size;
        g_pos = 0;

        int32_t systemAbilityId = GetData<int32_t>();
        std::string deviceId = GetStringFromData(int(size));

        std::shared_ptr<EventController> eventController = std::make_shared<EventController>();
        eventController->Init();
        eventController->sysAbilityListener_->OnAddSystemAbility(systemAbilityId, deviceId);
        eventController->sysAbilityListener_->OnRemoveSystemAbility(systemAbilityId, deviceId);
        eventController->Stop();
        return true;
    }

    bool ResSchedCommonEventGetUidFuzzTest(const uint8_t* data, size_t size)
    {
        if (data == nullptr) {
            return false;
        }

        if (size <= sizeof(int32_t) + sizeof(std::string)) {
            return false;
        }

        // initialize
        DATA = data;
        g_size = size;
        g_pos = 0;

        int32_t userId = GetData<int32_t>();
        std::string bundleName = GetStringFromData(int(size));
        std::shared_ptr<EventController> eventController = std::make_shared<EventController>();
        eventController->GetUid(userId, bundleName);
        return true;
    }

    bool ResSchedCommonEventSwitchFuzzTest(const uint8_t* data, size_t size)
    {
        if (data == nullptr) {
            return false;
        }
        // initialize
        DATA = data;
        g_size = size;
        g_pos = 0;

        AAFwk::Want want;
        EventFwk::CommonEventData commonEventData;
        want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_SCREEN_ON);
        commonEventData.SetWant(want);

        AAFwk::Want eventWant;
        EventFwk::CommonEventData eventData;
        eventWant.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_SCREEN_OFF);
        commonEventData.SetWant(eventWant);

        std::shared_ptr<EventController> eventController = std::make_shared<EventController>();
        eventController->OnReceiveEvent(commonEventData);
        eventController->OnReceiveEvent(eventData);
        return true;
    }

    bool ResSchedCommonReceiveEventFuzzTest(const uint8_t* data, size_t size)
    {
        if (data == nullptr) {
            return false;
        }
        // initialize
        DATA = data;
        g_size = size;
        g_pos = 0;

        int32_t code = GetData<int32_t>();
        int32_t value = GetData<int32_t>();
        std::string eventData = GetStringFromData(int(size) -
        THREE_PARAMETERS * sizeof(int32_t));
        std::string action = GetStringFromData(int(size) -
        THREE_PARAMETERS * sizeof(int32_t) - sizeof(std::string));

        nlohmann::json payload;
        AAFwk::Want want;
        EventFwk::CommonEventData commonEventData;
        want.SetAction(action);
        want.SetParam("NetType", value);
        commonEventData.SetWant(want);
        commonEventData.SetData(eventData);
        commonEventData.SetCode(code);

        std::shared_ptr<EventController> eventController = std::make_shared<EventController>();
        eventController->OnReceiveEvent(commonEventData);
        eventController->HandleConnectivityChange(want, code, payload);
        eventController->HandlePkgAddRemove(want, payload);
        eventController->handleEvent(code, eventData, payload);
        return true;
    }

    bool ResSchedPkgCommonEventFuzzTest(const uint8_t* data, size_t size)
    {
        if (data == nullptr) {
            return false;
        }
        // initialize
        DATA = data;
        g_size = size;
        g_pos = 0;

        int32_t code = GetData<int32_t>();
        int32_t value = GetData<int32_t>();
        std::string eventData = GetStringFromData(int(size) -
        THREE_PARAMETERS * sizeof(int32_t));
        std::string action = GetStringFromData(int(size) -
        THREE_PARAMETERS * sizeof(int32_t) - sizeof(std::string));

        nlohmann::json payload;
        AAFwk::Want want;
        EventFwk::CommonEventData commonEventData;
        want.SetAction(action);
        want.SetParam("NetType", value);
        commonEventData.SetWant(want);
        commonEventData.SetData(eventData);
        commonEventData.SetCode(code);

        std::shared_ptr<EventController> eventController = std::make_shared<EventController>();
        eventController->HandlePkgCommonEvent(eventData, want, payload);
        return true;
    }

    bool ResSchedPkgOtherCommonEventFuzzTest(const uint8_t* data, size_t size)
    {
        if (data == nullptr) {
            return false;
        }
        // initialize
        DATA = data;
        g_size = size;
        g_pos = 0;

        int32_t code = GetData<int32_t>();
        int32_t value = GetData<int32_t>();
        std::string eventData = GetStringFromData(int(size) -
        THREE_PARAMETERS * sizeof(int32_t));
        std::string action = GetStringFromData(int(size) -
        THREE_PARAMETERS * sizeof(int32_t) - sizeof(std::string));

        nlohmann::json payload;
        AAFwk::Want want;
        EventFwk::CommonEventData commonEventData;
        want.SetAction(action);
        want.SetParam("NetType", value);
        commonEventData.SetWant(want);
        commonEventData.SetData(eventData);
        commonEventData.SetCode(code);

        std::shared_ptr<EventController> eventController = std::make_shared<EventController>();
        eventController->HandlePkgCommonEvent(
            EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_ADDED, want, payload);
        eventController->HandlePkgCommonEvent(
            EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_CHANGED, want, payload);
        eventController->HandlePkgCommonEvent(
            EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_REPLACED, want, payload);
        eventController->HandlePkgCommonEvent(
            EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_FULLY_REMOVED, want, payload);
        eventController->HandlePkgCommonEvent(
            EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_REMOVED, want, payload);
        return true;
    }
} // namespace ResourceSchedule
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    OHOS::ResourceSchedule::ResSchedCommonEventFuzzTest(data, size);
    OHOS::ResourceSchedule::ResSchedCommonEventGetUidFuzzTest(data, size);
    OHOS::ResourceSchedule::ResSchedCommonReceiveEventFuzzTest(data, size);
    OHOS::ResourceSchedule::ResSchedPkgCommonEventFuzzTest(data, size);
    OHOS::ResourceSchedule::ResSchedCommonEventSwitchFuzzTest(data, size);
    OHOS::ResourceSchedule::ResSchedPkgOtherCommonEventFuzzTest(data, size);
    return 0;
}