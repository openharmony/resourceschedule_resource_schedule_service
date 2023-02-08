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
#include <string>
#include "config_policy_utils.h"
#include "event_runner.h"
#include "hisysevent.h"
#include "res_sched_log.h"
#include "hitrace_meter.h"

using namespace std;

namespace OHOS {
namespace ResourceSchedule {
using namespace AppExecFwk;
using namespace HiviewDFX;
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
    if (pluginSwitch_) {
        RESSCHED_LOGW("%{public}s, PluginMgr has Initialized!", __func__);
        return;
    }

    if (!pluginSwitch_) {
        pluginSwitch_ = make_unique<PluginSwitch>();
        std::string realPath = GetRealConfigPath(PLUGIN_SWITCH_FILE_NAME);
        if (realPath.empty() || !pluginSwitch_->LoadFromConfigFile(realPath)) {
            RESSCHED_LOGW("%{public}s, PluginMgr load switch config file failed!", __func__);
        }
    }

    if (!configReader_) {
        configReader_ = make_unique<ConfigReader>();
        std::string realPath = GetRealConfigPath(CONFIG_FILE_NAME);
        if (realPath.empty() || !configReader_->LoadFromCustConfigFile(realPath)) {
            RESSCHED_LOGW("%{public}s, PluginMgr load config file failed!", __func__);
        }
    }

    LoadPlugin();
    RESSCHED_LOGI("PluginMgr::Init success!");
}

void PluginMgr::LoadPlugin()
{
    if (!pluginSwitch_) {
        RESSCHED_LOGW("%{public}s, configReader null!", __func__);
        return;
    }

    std::list<PluginInfo> pluginInfoList = pluginSwitch_->GetPluginSwitch();
    for (const auto& info : pluginInfoList) {
        if (!info.switchOn) {
            continue;
        }
        shared_ptr<PluginLib> libInfoPtr = LoadOnePlugin(info);
        if (libInfoPtr == nullptr) {
            continue;
        }
        std::lock_guard<std::mutex> autoLock(pluginMutex_);
        pluginLibMap_.emplace(info.libPath, *libInfoPtr);

        RESSCHED_LOGI("%{public}s, init %{public}s success!", __func__, info.libPath.c_str());
    }
}

shared_ptr<PluginLib> PluginMgr::LoadOnePlugin(const PluginInfo& info)
{
    auto pluginHandle = dlopen(info.libPath.c_str(), RTLD_NOW);
    if (!pluginHandle) {
        RESSCHED_LOGE("%{public}s, not find plugin lib !", __func__);
        return nullptr;
    }

    auto onPluginInitFunc = reinterpret_cast<OnPluginInitFunc>(dlsym(pluginHandle, "OnPluginInit"));
    if (!onPluginInitFunc) {
        RESSCHED_LOGE("%{public}s, dlsym OnPluginInit failed!", __func__);
        dlclose(pluginHandle);
        return nullptr;
    }

    auto onPluginDisableFunc = reinterpret_cast<OnPluginDisableFunc>(dlsym(pluginHandle, "OnPluginDisable"));
    if (!onPluginDisableFunc) {
        RESSCHED_LOGE("%{public}s, dlsym OnPluginDisable failed!", __func__);
        dlclose(pluginHandle);
        return nullptr;
    }

    if (!onPluginInitFunc(info.libPath)) {
        RESSCHED_LOGE("%{public}s, %{public}s init failed!", __func__, info.libPath.c_str());
        dlclose(pluginHandle);
        return nullptr;
    }

    dispatcherHandlerMap_[info.libPath] =
            std::make_shared<EventHandler>(EventRunner::Create(RUNNER_NAME + to_string(handlerNum_++)));

    // OnDispatchResource is not necessary for plugin
    auto onDispatchResourceFunc = reinterpret_cast<OnDispatchResourceFunc>(dlsym(pluginHandle,
        "OnDispatchResource"));

    // OnDispatchResource is not necessary for plugin
    auto onDumpFunc = reinterpret_cast<OnDumpFunc>(dlsym(pluginHandle, "OnDump"));

    PluginLib libInfo;
    libInfo.handle = nullptr;
    libInfo.onPluginInitFunc_ = onPluginInitFunc;
    libInfo.onDispatchResourceFunc_ = onDispatchResourceFunc;
    libInfo.onDumpFunc_ = onDumpFunc;
    libInfo.onPluginDisableFunc_ = onPluginDisableFunc;

    return make_shared<PluginLib>(libInfo);
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
        RESSCHED_LOGE("%{public}s, failed, null res data.", __func__);
        return;
    }
    std::lock_guard<std::mutex> autoLock(resTypeMutex_);
    auto iter = resTypeLibMap_.find(resData->resType);
    if (iter == resTypeLibMap_.end()) {
        RESSCHED_LOGD("%{public}s, PluginMgr resType no lib register!", __func__);
        return;
    }
    std::string libNameAll = "[";
    for (const auto& libName : iter->second) {
        libNameAll.append(libName);
        libNameAll.append(",");
    }
    libNameAll.append("]");
    string trace_str(__func__);
    string resTypeString =
        ResType::resTypeToStr.count(resData->resType) ? ResType::resTypeToStr.at(resData->resType) : "UNKNOWN";
    trace_str.append(" PluginMgr ,resType[").append(std::to_string(resData->resType)).append("]");
    trace_str.append(",resTypeStr[").append(resTypeString).append("]");
    trace_str.append(",value[").append(std::to_string(resData->value)).append("]");
    trace_str.append(",pluginlist:").append(libNameAll);
    StartTrace(HITRACE_TAG_OHOS, trace_str, -1);
    RESSCHED_LOGD("%{public}s, PluginMgr, resType = %{public}d, "
                  "value = %{public}lld, pluginlist is %{public}s.", __func__,
                  resData->resType, (long long)resData->value, libNameAll.c_str());
    FinishTrace(HITRACE_TAG_OHOS);
    std::lock_guard<std::mutex> autoLock2(dispatcherHandlerMutex_);
    for (const auto& libPath : iter->second) {
        if (!dispatcherHandlerMap_[libPath]) {
            RESSCHED_LOGE("%{public}s, failed, %{public}s dispatcher handler is stopped.", __func__,
                libPath.c_str());
            continue;
        }
        dispatcherHandlerMap_[libPath]->PostTask(
            [libPath, resData, this] { deliverResourceToPlugin(libPath, resData); });
    }
}

void PluginMgr::SubscribeResource(const std::string& pluginLib, uint32_t resType)
{
    if (pluginLib.size() == 0) {
        RESSCHED_LOGE("%{public}s, PluginMgr failed, pluginLib is null.", __func__);
        return;
    }
    std::lock_guard<std::mutex> autoLock(resTypeMutex_);
    resTypeLibMap_[resType].emplace_back(pluginLib);
}

void PluginMgr::UnSubscribeResource(const std::string& pluginLib, uint32_t resType)
{
    if (pluginLib.size() == 0) {
        RESSCHED_LOGE("%{public}s, PluginMgr failed, pluginLib is null.", __func__);
        return;
    }
    std::lock_guard<std::mutex> autoLock(resTypeMutex_);
    auto iter = resTypeLibMap_.find(resType);
    if (iter == resTypeLibMap_.end()) {
        RESSCHED_LOGE("%{public}s, PluginMgr failed, res type has no plugin subscribe.", __func__);
        return;
    }

    iter->second.remove(pluginLib);
    if (iter->second.empty()) {
        resTypeLibMap_.erase(iter);
    }
}

void PluginMgr::DumpAllPlugin(std::string &result)
{
    std::list<PluginInfo> pluginInfoList = pluginSwitch_->GetPluginSwitch();
    for (const auto& info : pluginInfoList) {
        result.append(info.libPath + std::string(DUMP_ONE_STRING_SIZE - info.libPath.size(), ' '));
        DumpPluginInfoAppend(result, info);
    }
}

void PluginMgr::DumpOnePlugin(std::string &result, std::string pluginName, std::vector<std::string>& args)
{
    std::list<PluginInfo> pluginInfoList = pluginSwitch_->GetPluginSwitch();
    auto pos = std::find_if(pluginInfoList.begin(),
        pluginInfoList.end(), [&pluginName](PluginInfo &info) { return pluginName == info.libPath; });
    if (pos == pluginInfoList.end()) {
        result.append(" Error params.\n");
        return;
    }
    if (args.size() == 0) {
        result.append(pluginName + std::string(DUMP_ONE_STRING_SIZE - pluginName.size(), ' '));
        DumpPluginInfoAppend(result, *pos);
    } else {
        result.append("\n");
        std::string errMsg = DumpInfoFromPlugin(result, pos->libPath, args);
        if (errMsg != "") {
            result.append(errMsg);
        }
    }
}

std::string PluginMgr::DumpInfoFromPlugin(std::string& result, std::string libPath, std::vector<std::string>& args)
{
    std::lock_guard<std::mutex> autoLock(pluginMutex_);
    auto pluginLib = pluginLibMap_.find(libPath);
    if (pluginLib == pluginLibMap_.end()) {
        return "Error params.";
    }

    if (pluginLib->second.onDumpFunc_) {
        pluginLib->second.onDumpFunc_(args, result);
        result.append("\n");
    }
    return "";
}

void PluginMgr::DumpHelpFromPlugin(std::string& result)
{
    std::vector<std::string> args;
    args.emplace_back("-h");
    std::string pluginHelpMsg = "";
    std::list<PluginInfo> pluginInfoList = pluginSwitch_->GetPluginSwitch();
    for (auto &pluginInfo : pluginInfoList) {
        DumpInfoFromPlugin(pluginHelpMsg, pluginInfo.libPath, args);
    }
    result.append(pluginHelpMsg);
}

void PluginMgr::DumpPluginInfoAppend(std::string &result, PluginInfo info)
{
    if (info.switchOn) {
        result.append(" | switch on\t");
    } else {
        result.append(" | switch off\t");
    }
    std::lock_guard<std::mutex> autoLock(pluginMutex_);
    if (pluginLibMap_.find(info.libPath) != pluginLibMap_.end()) {
        result.append(" | running now\n");
    } else {
        result.append(" | disabled\n");
    }
}

std::string PluginMgr::GetRealConfigPath(const char* configName)
{
    char buf[PATH_MAX + 1];
    char* configFilePath = GetOneCfgFile(configName, buf, PATH_MAX + 1);
    char tmpPath[PATH_MAX + 1] = {0};
    if (!configFilePath || strlen(configFilePath) == 0 || strlen(configFilePath) > PATH_MAX ||
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

void PluginMgr::RepairPlugin(TimePoint endTime, const std::string& pluginLib, PluginLib libInfo)
{
    int32_t crash_time = (int32_t)((endTime - pluginTimeoutTime_[pluginLib].front()) / std::chrono::milliseconds(1));
    pluginTimeoutTime_[pluginLib].emplace_back(endTime);
    RESSCHED_LOGW("%{public}s %{public}s crash %{public}d times in %{public}d ms!", __func__,
        pluginLib.c_str(), (int32_t)pluginTimeoutTime_[pluginLib].size(), crash_time);
    if ((int32_t)pluginTimeoutTime_[pluginLib].size() >= MAX_PLUGIN_TIMEOUT_TIMES) {
        if (crash_time < DISABLE_PLUGIN_TIME) {
            // disable plugin forever
            RESSCHED_LOGE("%{public}s, %{public}s disable it forever", __func__, pluginLib.c_str());
            if (libInfo.onPluginDisableFunc_) {
                libInfo.onPluginDisableFunc_();
            }
            HiSysEvent::Write("RSS", "PLUGIN_DISABLE",
                HiSysEvent::EventType::FAULT, "plugin_name", pluginLib);
            pluginTimeoutTime_[pluginLib].clear();
            pluginMutex_.lock();
            auto itMap = pluginLibMap_.find(pluginLib);
            pluginLibMap_.erase(itMap);
            pluginMutex_.unlock();
            std::lock_guard<std::mutex> autoLock(disablePluginsMutex_);
            disablePlugins_.emplace_back(pluginLib);
            return;
        }

        auto iter = pluginTimeoutTime_[pluginLib].begin();
        pluginTimeoutTime_[pluginLib].erase(iter);
    }

    if (libInfo.onPluginDisableFunc_ && libInfo.onPluginInitFunc_) {
        RESSCHED_LOGW("%{public}s, %{public}s disable and enable it", __func__, pluginLib.c_str());
        libInfo.onPluginDisableFunc_();
        libInfo.onPluginInitFunc_(pluginLib);
    }
}

void PluginMgr::deliverResourceToPlugin(const std::string& pluginLib, const std::shared_ptr<ResData>& resData)
{
    PluginLib libInfo;
    {
        std::lock_guard<std::mutex> autoLock(pluginMutex_);
        auto itMap = pluginLibMap_.find(pluginLib);
        if (itMap == pluginLibMap_.end()) {
            RESSCHED_LOGE("%{public}s, no plugin %{public}s !", __func__, pluginLib.c_str());
            return;
        }
        libInfo = itMap->second;
    }

    OnDispatchResourceFunc pluginDispatchFunc = libInfo.onDispatchResourceFunc_;
    if (!pluginDispatchFunc) {
        RESSCHED_LOGE("%{public}s, no DispatchResourceFun !", __func__);
        return;
    }

    auto beginTime = Clock::now();
    pluginDispatchFunc(resData);
    auto endTime = Clock::now();
    int32_t costTime = (endTime - beginTime) / std::chrono::milliseconds(1);
    if (costTime > DISPATCH_TIME_OUT) {
        // dispatch resource use too long time, unload it
        RESSCHED_LOGE("%{public}s, ERROR :"
                      "%{public}s plugin cost time(%{public}dms) over %{public}d ms! disable it.",
                      __func__, pluginLib.c_str(), costTime, DISPATCH_TIME_OUT);
        RepairPlugin(endTime, pluginLib, libInfo);
    } else if (costTime > DISPATCH_WARNING_TIME) {
        RESSCHED_LOGW("%{public}s, WARNING :"
                      "%{public}s plugin cost time(%{public}dms) over %{public}d ms!",
                      __func__, pluginLib.c_str(), costTime, DISPATCH_WARNING_TIME);
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

} // namespace ResourceSchedule
} // namespace OHOS
