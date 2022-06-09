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

#include "plugin_mgr.h"
#include <cinttypes>
#include <algorithm>
#include <dlfcn.h>
#include <iostream>
#include "config_policy_utils.h"
#include "event_runner.h"
#include "res_sched_log.h"

using namespace std;

namespace OHOS {
namespace ResourceSchedule {
using namespace AppExecFwk;
using OnPluginInitFunc = bool (*)(std::string);

namespace {
    const int32_t DISPATCH_WARNING_TIME = 10; // ms
    const int32_t DISPATCH_TIME_OUT = 50; // ms
    const std::string RUNNER_NAME = "rssDispatcher#";
    const char* PLUGIN_SWITCH_FILE_NAME = "etc/ressched/res_sched_plugin_switch.xml";
    const char* CONFIG_FILE_NAME = "etc/ressched/res_sched_config.xml";
}

IMPLEMENT_SINGLE_INSTANCE(PluginMgr);

PluginMgr::~PluginMgr()
{
    OnDestroy();
}

void PluginMgr::Init()
{
    RESSCHED_LOGI("PluginMgr::Init enter!");
    if (pluginSwitch_) {
        RESSCHED_LOGW("PluginMgr::Init has Initialized!");
        return;
    }

    if (!pluginSwitch_) {
        pluginSwitch_ = make_unique<PluginSwitch>();
        std::string realPath = GetRealConfigPath(PLUGIN_SWITCH_FILE_NAME);
        if (realPath.size() <= 0 || !pluginSwitch_->LoadFromConfigFile(realPath)) {
            RESSCHED_LOGW("PluginMgr::Init load config file failed!");
        }
    }

    if (!configReader_) {
        configReader_ = make_unique<ConfigReader>();
        std::string realPath = GetRealConfigPath(CONFIG_FILE_NAME);
        if (realPath.size() <= 0 || !configReader_->LoadFromCustConfigFile(realPath)) {
            RESSCHED_LOGW("PluginMgr::Init load config file failed!");
        }
    }

    LoadPlugin();
    RESSCHED_LOGI("PluginMgr::Init success!");
}

void PluginMgr::LoadPlugin()
{
    if (!pluginSwitch_) {
        RESSCHED_LOGW("PluginMgr::LoadPlugin configReader null!");
        return;
    }

    std::list<PluginInfo> pluginInfoList = pluginSwitch_->GetPluginSwitch();
    for (const auto& info : pluginInfoList) {
        if (!info.switchOn) {
            continue;
        }

        auto pluginHandle = dlopen(info.libPath.c_str(), RTLD_NOW);
        if (!pluginHandle) {
            RESSCHED_LOGE("PluginMgr::LoadPlugin not find plugin lib !");
            continue;
        }

        auto onPluginInitFunc = reinterpret_cast<OnPluginInitFunc>(dlsym(pluginHandle, "OnPluginInit"));
        if (!onPluginInitFunc) {
            RESSCHED_LOGE("PluginMgr::LoadPlugin dlsym OnPluginInit failed!");
            dlclose(pluginHandle);
            continue;
        }

        auto onPluginDisableFunc = reinterpret_cast<OnPluginDisableFunc>(dlsym(pluginHandle, "OnPluginDisable"));
        if (!onPluginDisableFunc) {
            RESSCHED_LOGE("PluginMgr::LoadPlugin dlsym OnPluginDisable failed!");
            dlclose(pluginHandle);
            continue;
        }

        if (!onPluginInitFunc(info.libPath)) {
            RESSCHED_LOGE("PluginMgr::LoadPlugin %{public}s init failed!", info.libPath.c_str());
            dlclose(pluginHandle);
            continue;
        }
        
        dispatcherHandlerMap_[info.libPath] =
            std::make_shared<EventHandler>(EventRunner::Create(RUNNER_NAME + to_string(handlerNum_++)));

        // OnDispatchResource is not necessary for plugin
        auto onDispatchResourceFunc = reinterpret_cast<OnDispatchResourceFunc>(dlsym(pluginHandle,
            "OnDispatchResource"));

        // shared_ptr save handle pointer and ensure handle close correctly
        shared_ptr<void> sharedPluginHandle(pluginHandle, CloseHandle);

        PluginLib libInfo;
        libInfo.handle = sharedPluginHandle;
        libInfo.onPluginInitFunc_ = onPluginInitFunc;
        libInfo.onDispatchResourceFunc_ = onDispatchResourceFunc;
        libInfo.onPluginDisableFunc_ = onPluginDisableFunc;
        {
            std::lock_guard<std::mutex> autoLock(pluginMutex_);
            pluginLibMap_.emplace(info.libPath, libInfo);
        }
        RESSCHED_LOGE("PluginMgr::LoadPlugin init %{public}s success!", info.libPath.c_str());
    }
}

PluginConfig PluginMgr::GetConfig(const std::string& pluginName, const std::string& configName)
{
    PluginConfig config;
    if (!configReader_) {
        return config;
    }
    return configReader_->GetConfig(pluginName, configName);
}

void PluginMgr::Stop()
{
    OnDestroy();
}

void PluginMgr::RemoveDisablePluginHandler()
{
    // clear already disable plugin handler
    std::lock_guard<std::mutex> autoLock(disablePluginsMutex_);
    dispatcherHandlerMutex_.lock();
    for (auto plugin : disablePlugins_) {
        RESSCHED_LOGI("PluginMgr::RemoveDisablePluginHandler remove %{plugin}s handler.", plugin.c_str());
        auto iter = dispatcherHandlerMap_.find(plugin);
        if (iter != dispatcherHandlerMap_.end()) {
            auto runner = iter->second->GetEventRunner();
            iter->second->RemoveAllEvents();
            runner->Stop();
            runner = nullptr;
            iter->second = nullptr;
            dispatcherHandlerMap_.erase(iter);
        }
    }
    dispatcherHandlerMutex_.unlock();
    disablePlugins_.clear();
}

void PluginMgr::DispatchResource(const std::shared_ptr<ResData>& resData)
{
    RemoveDisablePluginHandler();
    if (!resData) {
        RESSCHED_LOGE("PluginMgr::DispatchResource failed, null res data.");
        return;
    }
    std::lock_guard<std::mutex> autoLock(resTypeMutex_);
    auto iter = resTypeLibMap_.find(resData->resType);
    if (iter == resTypeLibMap_.end()) {
        RESSCHED_LOGW("PluginMgr::DispatchResource resType no lib register!");
        return;
    }
    std::string libNameAll = "[";
    for (const auto& libName : iter->second) {
        libNameAll.append(libName);
        libNameAll.append(",");
    }
    libNameAll.append("]");
    RESSCHED_LOGI("PluginMgr::DispatchResource resType = %{public}d, "
        "value = %{public}lld, pluginlist is %{public}s.",
        resData->resType, (long long)resData->value, libNameAll.c_str());
    {
        std::lock_guard<std::mutex> autoLock(dispatcherHandlerMutex_);
        for (const auto& libPath : iter->second) {
            if (!dispatcherHandlerMap_[libPath]) {
                RESSCHED_LOGE("PluginMgr::DispatchResource failed, %{public}s dispatcher handler is stopped.",
                    libPath.c_str());
                continue;
            }
            dispatcherHandlerMap_[libPath]->PostTask(
                [libPath, resData, this] { deliverResourceToPlugin(libPath, resData); });
        }
    }
}

void PluginMgr::SubscribeResource(const std::string& pluginLib, uint32_t resType)
{
    if (pluginLib.size() == 0) {
        RESSCHED_LOGE("PluginMgr::SubscribeResource failed, pluginLib is null.");
        return;
    }
    std::lock_guard<std::mutex> autoLock(resTypeMutex_);
    resTypeLibMap_[resType].emplace_back(pluginLib);
}

void PluginMgr::UnSubscribeResource(const std::string& pluginLib, uint32_t resType)
{
    if (pluginLib.size() == 0) {
        RESSCHED_LOGE("PluginMgr::UnSubscribeResource failed, pluginLib is null.");
        return;
    }
    std::lock_guard<std::mutex> autoLock(resTypeMutex_);
    auto iter = resTypeLibMap_.find(resType);
    if (iter == resTypeLibMap_.end()) {
        RESSCHED_LOGE("PluginMgr::UnSubscribeResource failed, res type has no plugin subscribe.");
        return;
    }

    iter->second.remove(pluginLib);
    if (iter->second.empty()) {
        resTypeLibMap_.erase(iter);
    }
}

std::string PluginMgr::GetRealConfigPath(const char* configName)
{
    char buf[MAX_PATH_LEN];
    char* configFilePath = GetOneCfgFile(configName, buf, MAX_PATH_LEN);
    char tmpPath[PATH_MAX + 1] = {0};
    if (strlen(configFilePath) == 0 || strlen(configFilePath) > PATH_MAX ||
        !realpath(configFilePath, tmpPath)) {
        RESSCHED_LOGE("%{public}s load config file wrong !", __func__);
        return "";
    }
    return std::string(tmpPath);
}

void PluginMgr::ClearResource()
{
    std::lock_guard<std::mutex> autoLock(resTypeMutex_);
    resTypeLibMap_.clear();
}

void PluginMgr::RepairPluginLocked(TimePoint endTime, const std::string& pluginLib, PluginLib libInfo)
{
    int32_t crash_time = (int32_t)((endTime - pluginTimeoutTime_[pluginLib].front()) / std::chrono::milliseconds(1));
    pluginTimeoutTime_[pluginLib].emplace_back(endTime);
    RESSCHED_LOGW("PluginMgr::RepairPluginLocked %{public}s crash %{public}d times in %{public}d ms!",
        pluginLib.c_str(), (int32_t)pluginTimeoutTime_[pluginLib].size(), crash_time);
    if ((int32_t)pluginTimeoutTime_[pluginLib].size() >= MAX_PLUGIN_TIMEOUT_TIMES) {
        if (crash_time < DISABLE_PLUGIN_TIME) {
            // disable plugin forever
            RESSCHED_LOGE("PluginMgr::RepairPluginLocked %{public}s disable it forever", pluginLib.c_str());
            if (libInfo.onPluginDisableFunc_) {
                libInfo.onPluginDisableFunc_();
            }
            pluginTimeoutTime_[pluginLib].clear();
            // pluginLibMap_ already locked
            auto itMap = pluginLibMap_.find(pluginLib);
            pluginLibMap_.erase(itMap);
            std::lock_guard<std::mutex> autoLock(disablePluginsMutex_);
            disablePlugins_.emplace_back(pluginLib);
            return;
        }

        auto iter = pluginTimeoutTime_[pluginLib].begin();
        pluginTimeoutTime_[pluginLib].erase(iter);
    }

    if (libInfo.onPluginDisableFunc_ && libInfo.onPluginInitFunc_) {
        RESSCHED_LOGW("PluginMgr::RepairPluginLocked %{public}s disable and enable it", pluginLib.c_str());
        libInfo.onPluginDisableFunc_();
        libInfo.onPluginInitFunc_(pluginLib);
    }
}

void PluginMgr::deliverResourceToPlugin(const std::string& pluginLib, const std::shared_ptr<ResData>& resData)
{
    std::lock_guard<std::mutex> autoLock(pluginMutex_);
    auto itMap = pluginLibMap_.find(pluginLib);
    if (itMap == pluginLibMap_.end()) {
        RESSCHED_LOGE("PluginMgr::deliverResourceToPlugin no plugin %{public}s !", pluginLib.c_str());
        return;
    }
    OnDispatchResourceFunc fun = itMap->second.onDispatchResourceFunc_;
    if (!fun) {
        RESSCHED_LOGE("PluginMgr::deliverResourceToPlugin no DispatchResourceFun !");
        return;
    }

    auto beginTime = Clock::now();
    fun(resData);
    auto endTime = Clock::now();
    int32_t costTime = (endTime - beginTime) / std::chrono::milliseconds(1);
    if (costTime > DISPATCH_TIME_OUT) {
        // dispatch resource use too long time, unload it
        RESSCHED_LOGE("PluginMgr::deliverResourceToPlugin ERROR :"
                      "%{public}s plugin cost time(%{public}dms) over %{public}d ms!",
                      pluginLib.c_str(), costTime, DISPATCH_TIME_OUT);
        RepairPluginLocked(endTime, pluginLib, itMap->second);
    } else if (costTime > DISPATCH_WARNING_TIME) {
        RESSCHED_LOGW("PluginMgr::deliverResourceToPlugin WARNING :"
                      "%{public}s plugin cost time(%{public}dms) over %{public}d ms!",
                      pluginLib.c_str(), costTime, DISPATCH_WARNING_TIME);
    }
}

void PluginMgr::UnLoadPlugin()
{
    std::lock_guard<std::mutex> autoLock(pluginMutex_);
    // unload all plugin
    for (const auto& [libPath, libInfo] : pluginLibMap_) {
        if (!libInfo.onPluginDisableFunc_) {
            continue;
        }
        libInfo.onPluginDisableFunc_();
    }
    // close all plugin handle
    pluginLibMap_.clear();
}

void PluginMgr::OnDestroy()
{
    UnLoadPlugin();
    configReader_ = nullptr;
    ClearResource();
    std::lock_guard<std::mutex> autoLock(dispatcherHandlerMutex_);
    for (auto iter = dispatcherHandlerMap_.begin(); iter != dispatcherHandlerMap_.end();) {
        if (iter->second != nullptr) {
            iter->second->RemoveAllEvents();
            iter->second = nullptr;
        }
        dispatcherHandlerMap_.erase(iter++);
    }
}

void PluginMgr::CloseHandle(const DlHandle& handle)
{
    if (!handle) {
        RESSCHED_LOGW("PluginMgr::CloseHandle nullptr handle!");
        return;
    }

    int32_t ret = dlclose(handle);
    if (ret) {
        RESSCHED_LOGW("PluginMgr::CloseHandle handle close failed!");
    }
}
} // namespace ResourceSchedule
} // namespace OHOS
