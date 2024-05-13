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

#define private public
#define protected public

#include "socperfplugin_fuzzer.h"
#include <dlfcn.h>
#include "res_type.h"
#include "res_data.h"
#include "socperf_plugin.h"

namespace OHOS {
namespace ResourceSchedule {
namespace {
    std::string SOCPERF_PLUBIN_LIB_NAME = "libsocperf_plugin.z.so";
    std::string ERROR_LIB_NAME = "";
    using OnPluginInitFunc = bool (*)(std::string& libName);
    using OnPluginDisableFunc = void (*)();
    using OnDispatchResourceFunc = void (*)(const std::shared_ptr<ResData>& data);
}

bool TestWindowFocus(const uint8_t *data, size_t size)
{
    int64_t value = *(reinterpret_cast<const int64_t *>(data));
    const std::shared_ptr<ResData> &resData = std::make_shared<ResData>(ResType::RES_TYPE_WINDOW_FOCUS, value,
        nullptr);
    SocPerfPlugin::GetInstance().DispatchResource(resData);
    return true;
}

bool TestClickRecognize(const uint8_t *data, size_t size)
{
    int64_t value = *(reinterpret_cast<const int64_t *>(data));
    const std::shared_ptr<ResData> &resData = std::make_shared<ResData>(ResType::RES_TYPE_CLICK_RECOGNIZE, value,
        nullptr);
    SocPerfPlugin::GetInstance().DispatchResource(resData);
    return true;
}

bool TestLoadPage(const uint8_t *data, size_t size)
{
    int64_t value = *(reinterpret_cast<const int64_t *>(data));
    const std::shared_ptr<ResData> &resData = std::make_shared<ResData>(ResType::RES_TYPE_LOAD_PAGE, value,
        nullptr);
    SocPerfPlugin::GetInstance().DispatchResource(resData);
    return true;
}

bool TestSlideRecognize(const uint8_t *data, size_t size)
{
    int64_t value = *(reinterpret_cast<const int64_t *>(data));
    const std::shared_ptr<ResData> &resData = std::make_shared<ResData>(ResType::RES_TYPE_SLIDE_RECOGNIZE, value,
        nullptr);
    SocPerfPlugin::GetInstance().DispatchResource(resData);
    return true;
}

bool TestWebGesture(const uint8_t *data, size_t size)
{
    int64_t value = *(reinterpret_cast<const int64_t *>(data));
    const std::shared_ptr<ResData> &resData = std::make_shared<ResData>(ResType::RES_TYPE_WEB_GESTURE, value,
        nullptr);
    SocPerfPlugin::GetInstance().DispatchResource(resData);
    return true;
}

bool TestPopPage(const uint8_t *data, size_t size)
{
    int64_t value = *(reinterpret_cast<const int64_t *>(data));
    const std::shared_ptr<ResData> &resData = std::make_shared<ResData>(ResType::RES_TYPE_POP_PAGE, value,
        nullptr);
    SocPerfPlugin::GetInstance().DispatchResource(resData);
    return true;
}

bool TestAppAbilityStart(const uint8_t *data, size_t size)
{
    int64_t value = *(reinterpret_cast<const int64_t *>(data));
    const std::shared_ptr<ResData> &resData = std::make_shared<ResData>(ResType::RES_TYPE_APP_ABILITY_START, value,
        nullptr);
    SocPerfPlugin::GetInstance().DispatchResource(resData);
    return true;
}

bool TestResizeWindow(const uint8_t *data, size_t size)
{
    int64_t value = *(reinterpret_cast<const int64_t *>(data));
    const std::shared_ptr<ResData> &resData = std::make_shared<ResData>(ResType::RES_TYPE_RESIZE_WINDOW, value,
        nullptr);
    SocPerfPlugin::GetInstance().DispatchResource(resData);
    return true;
}

bool TestMoveWindow(const uint8_t *data, size_t size)
{
    int64_t value = *(reinterpret_cast<const int64_t *>(data));
    const std::shared_ptr<ResData> &resData = std::make_shared<ResData>(ResType::RES_TYPE_MOVE_WINDOW, value,
        nullptr);
    SocPerfPlugin::GetInstance().DispatchResource(resData);
    return true;
}

bool TestShowRemoteAnimation(const uint8_t *data, size_t size)
{
    int64_t value = *(reinterpret_cast<const int64_t *>(data));
    const std::shared_ptr<ResData> &resData = std::make_shared<ResData>(ResType::RES_TYPE_SHOW_REMOTE_ANIMATION, value,
        nullptr);
    SocPerfPlugin::GetInstance().DispatchResource(resData);
    return true;
}

bool TestDragStatusBar(const uint8_t *data, size_t size)
{
    int64_t value = *(reinterpret_cast<const int64_t *>(data));
    const std::shared_ptr<ResData> &resData = std::make_shared<ResData>(ResType::RES_TYPE_DRAG_STATUS_BAR, value,
        nullptr);
    SocPerfPlugin::GetInstance().DispatchResource(resData);
    return true;
}

bool TestWebGestureMove(const uint8_t *data, size_t size)
{
    int64_t value = *(reinterpret_cast<const int64_t *>(data));
    const std::shared_ptr<ResData> &resData = std::make_shared<ResData>(ResType::RES_TYPE_WEB_GESTURE_MOVE, value,
        nullptr);
    SocPerfPlugin::GetInstance().DispatchResource(resData);
    return true;
}

bool TestWebSlideNormal(const uint8_t *data, size_t size)
{
    int64_t value = *(reinterpret_cast<const int64_t *>(data));
    const std::shared_ptr<ResData> &resData = std::make_shared<ResData>(ResType::RES_TYPE_WEB_SLIDE_NORMAL, value,
        nullptr);
    SocPerfPlugin::GetInstance().DispatchResource(resData);
    return true;
}

bool TestLoadUrl(const uint8_t *data, size_t size)
{
    int64_t value = *(reinterpret_cast<const int64_t *>(data));
    const std::shared_ptr<ResData> &resData = std::make_shared<ResData>(ResType::RES_TYPE_LOAD_URL, value,
        nullptr);
    SocPerfPlugin::GetInstance().DispatchResource(resData);
    return true;
}

bool TestMouseWheel(const uint8_t *data, size_t size)
{
    int64_t value = *(reinterpret_cast<const int64_t *>(data));
    const std::shared_ptr<ResData> &resData = std::make_shared<ResData>(ResType::RES_TYPE_MOUSEWHEEL, value,
        nullptr);
    SocPerfPlugin::GetInstance().DispatchResource(resData);
    return true;
}

bool TestAppStateChange(const uint8_t *data, size_t size)
{
    int64_t value = *(reinterpret_cast<const int64_t *>(data));
    const std::shared_ptr<ResData> &resData = std::make_shared<ResData>(ResType::RES_TYPE_APP_STATE_CHANGE, value,
        nullptr);
    SocPerfPlugin::GetInstance().DispatchResource(resData);
    return true;
}

bool TestUseBySo(const uint8_t *data, size_t size)
{
    auto handle = dlopen(SOCPERF_PLUBIN_LIB_NAME.c_str(), RTLD_NOW);
    OnPluginInitFunc onPluginInitFunc = reinterpret_cast<OnPluginInitFunc>(dlsym(handle, "OnPluginInit"));
    onPluginInitFunc(SOCPERF_PLUBIN_LIB_NAME);
    onPluginInitFunc(ERROR_LIB_NAME);
    OnDispatchResourceFunc onDispatchResourceFunc = reinterpret_cast<OnDispatchResourceFunc>(dlsym(handle,
        "OnDispatchResource"));
    int64_t resType = *(reinterpret_cast<const int64_t *>(data));
    int64_t value = *(reinterpret_cast<const int64_t *>(data + sizeof(int64_t)));
    const std::shared_ptr<ResData>& resData = std::make_shared<ResData>(resType, value, nullptr);
    onDispatchResourceFunc(resData);
    const std::shared_ptr<ResData>& resDataInvalid = std::make_shared<ResData>(-1, -1, nullptr);
    onDispatchResourceFunc(resDataInvalid);
    OnPluginDisableFunc onPluginDisableFunc = reinterpret_cast<OnPluginDisableFunc>(dlsym(handle,
        "OnPluginDisable"));
    onPluginDisableFunc();
    if (handle != nullptr) {
        dlclose(handle);
        handle = nullptr;
    }
    return true;
}

bool TestFeatureInit(const uint8_t *data, size_t size)
{
    std::string featureName(reinterpret_cast<const char *>(data), size);
    OHOS::ResourceSchedule::SocPerfPlugin::GetInstance().InitFeatureSwitch(featureName);
    return true;
}
}
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    OHOS::ResourceSchedule::SocPerfPlugin::GetInstance().Init();
    OHOS::ResourceSchedule::TestFeatureInit(data, size);
    OHOS::ResourceSchedule::TestWindowFocus(data, size);
    OHOS::ResourceSchedule::TestClickRecognize(data, size);
    OHOS::ResourceSchedule::TestLoadPage(data, size);
    OHOS::ResourceSchedule::TestSlideRecognize(data, size);
    OHOS::ResourceSchedule::TestWebGesture(data, size);
    OHOS::ResourceSchedule::TestPopPage(data, size);
    OHOS::ResourceSchedule::TestAppAbilityStart(data, size);
    OHOS::ResourceSchedule::TestResizeWindow(data, size);
    OHOS::ResourceSchedule::TestMoveWindow(data, size);
    OHOS::ResourceSchedule::TestShowRemoteAnimation(data, size);
    OHOS::ResourceSchedule::TestDragStatusBar(data, size);
    OHOS::ResourceSchedule::TestWebGestureMove(data, size);
    OHOS::ResourceSchedule::TestWebSlideNormal(data, size);
    OHOS::ResourceSchedule::TestLoadUrl(data, size);
    OHOS::ResourceSchedule::TestMouseWheel(data, size);
    OHOS::ResourceSchedule::TestAppStateChange(data, size);
    OHOS::ResourceSchedule::TestUseBySo(data, size);
    OHOS::ResourceSchedule::SocPerfPlugin::GetInstance().Disable();
    return 0;
}

