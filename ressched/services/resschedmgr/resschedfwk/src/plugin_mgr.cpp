/*
 * Copyright (c) 2022-2025 Huawei Device Co., Ltd.
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
#include <cstdint>
#include <dlfcn.h>
#include <iostream>
#include <string>
#include <string_ex.h>
#include <fstream>
#include "config_policy_utils.h"
#include "file_ex.h"
#ifdef RESOURCE_SCHEDULE_SERVICE_WITH_FFRT_ENABLE
#include "ffrt_inner.h"
#else
#include "event_runner.h"
#endif
#include "hisysevent.h"
#include "refbase.h"
#include "res_sched_log.h"
#include "hitrace_meter.h"
#include "batch_log_printer.h"
#include "res_sched_file_util.h"
#include "res_sched_system_util.h"

using namespace std;

namespace OHOS {
namespace ResourceSchedule {
using namespace AppExecFwk;
using namespace HiviewDFX;

namespace {
    static const int32_t DISPATCH_WARNING_TIME = 10; // ms
    static const int32_t PLUGIN_SWITCH_FILE_IDX = 0;
    static const int32_t CONFIG_FILE_IDX = 1;
    static const int32_t SIMPLIFY_LIB_INDEX = 3;
    static const int32_t SIMPLIFY_LIB_LENGTH = 2;
    static const int32_t PLUGIN_REQUEST_ERROR = -1;
    static const char* PLUGIN_SWITCH_FILE_NAME = "etc/ressched/res_sched_plugin_switch.xml";
    static const char* CONFIG_FILE_NAME = "etc/ressched/res_sched_config.xml";
    static const char* EXT_CONFIG_LIB = "libsuspend_manager_service.z.so";
    static const char* RSS_SUBSCRIPTION_ACCURATLY_ENABLE = "rss.framework.subscription.accurately.enable";
#ifdef RESOURCE_SCHEDULE_SERVICE_WITH_EXT_RES_ENABLE
    static const int32_t DEFAULT_VALUE = -1;
    static const char* EXT_RES_KEY = "extType";
#endif
#ifndef RESOURCE_SCHEDULE_SERVICE_WITH_FFRT_ENABLE
    static const char* RUNNER_NAME = "rssDispatcher";
#endif
}

IMPLEMENT_SINGLE_INSTANCE(PluginMgr);

PluginMgr::~PluginMgr()
{
    OnDestroy();
}

void PluginMgr::Init(bool isRssExe)
{
    if (pluginSwitch_) {
        RESSCHED_LOGW("%{public}s, PluginMgr has Initialized!", __func__);
        return;
    }

    if (!pluginSwitch_) {
        pluginSwitch_ = make_unique<PluginSwitch>();
    }
    if (!configReader_) {
        configReader_ = make_unique<ConfigReader>();
    }

    if (!isRssExe) {
        LoadGetExtConfigFunc();
    }
    ReadSubscriptionAccuractlyEnableProperties();
    RESSCHED_LOGI("PluginMgr::Init success!");
}

std::vector<std::string> PluginMgr::GetConfigReaderStr()
{
    std::vector<std::string> configStrs;
    GetConfigContent(CONFIG_FILE_IDX, CONFIG_FILE_NAME, configStrs);
    return configStrs;
}

std::vector<std::string> PluginMgr::GetPluginSwitchStr()
{
    std::vector<std::string> switchStrs;
    GetConfigContent(PLUGIN_SWITCH_FILE_IDX, PLUGIN_SWITCH_FILE_NAME, switchStrs);
    return switchStrs;
}

void PluginMgr::ParseConfigReader(const std::vector<std::string>& configStrs)
{
    if (!configReader_) {
        RESSCHED_LOGW("%{public}s, configReader null!", __func__);
        return;
    }

    int32_t configStrsSize = static_cast<int32_t>(configStrs.size());
    RESSCHED_LOGI("plugin configStrs size %{public}d", configStrsSize);
    for (int index = 0; index < configStrsSize; index++) {
        if (configStrs[index].empty()) {
            continue;
        }
        if (!configReader_->LoadFromConfigContent(configStrs[index])) {
            RESSCHED_LOGW("%{public}s, PluginMgr load config file index:%{public}d failed!", __func__, index);
            HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::RSS, "INIT_FAULT",
                HiviewDFX::HiSysEvent::EventType::FAULT,
                "COMPONENT_NAME", "MAIN", "ERR_TYPE", "configure error",
                "ERR_MSG", "PluginMgr load parameter config file failed");
            continue;
        }
        RESSCHED_LOGI("PluginMgr load config file index:%{public}d success!", index);
    }
}

void PluginMgr::CompletePluginInitialization()
{
    // Set initialization flag to true
    isInit = true;
    // Call all registered initialization finish callbacks
    CallOnInitFinishCallbacks();
    // Log initialization success
    RESSCHED_LOGI("PluginMgr load plugin success!");
}

void PluginMgr::ParsePluginSwitch(const std::vector<std::string>& switchStrs, bool isRssExe)
{
    if (!pluginSwitch_) {
        RESSCHED_LOGW("%{public}s, pluginSwitch null!", __func__);
        return;
    }

    int32_t switchStrsSize = static_cast<int32_t>(switchStrs.size());
    RESSCHED_LOGI("plugin switchStrs size %{public}d", switchStrsSize);
    for (int32_t index = 0; index < switchStrsSize; index++) {
        if (switchStrs[index].empty()) {
            continue;
        }
        if (!pluginSwitch_->LoadFromConfigContent(switchStrs[index], isRssExe)) {
            RESSCHED_LOGW("%{public}s, PluginMgr load switch config file index:%{public}d failed!", __func__, index);
            HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::RSS, "INIT_FAULT",
                HiviewDFX::HiSysEvent::EventType::FAULT,
                "COMPONENT_NAME", "MAIN", "ERR_TYPE", "configure error",
                "ERR_MSG", "PluginMgr load switch config file failed!");
            continue;
        }
        RESSCHED_LOGI("PluginMgr load switch config file index:%{public}d success!", index);
    }
    LoadPlugin();
#ifdef RESOURCE_SCHEDULE_SERVICE_WITH_FFRT_ENABLE
    std::lock_guard<ffrt::mutex> autoLock(dispatcherHandlerMutex_);
#else
    std::lock_guard<std::mutex> autoLock(dispatcherHandlerMutex_);
#endif
#ifdef RESOURCE_SCHEDULE_SERVICE_WITH_FFRT_ENABLE
    dispatchers_.clear();
    for (const auto& [libPath, libInfo] : pluginLibMap_) {
        auto callback = [pluginName = libPath, time = pluginBlockTime]() {
            PluginMgr::GetInstance().HandlePluginTimeout(pluginName);
            ffrt::submit([pluginName]() {
                PluginMgr::GetInstance().EnablePluginIfResume(pluginName);
                }, {}, {}, ffrt::task_attr().delay(time));
        };
        dispatchers_.emplace(libPath, std::make_shared<ffrt::queue>(libPath.c_str(),
            ffrt::queue_attr().qos(ffrt::qos_user_interactive)));
    }
#else
    if (!dispatcher_) {
        dispatcher_ = std::make_shared<EventHandler>(EventRunner::Create(RUNNER_NAME));
    }
    if (!dispatcher_) {
        RESSCHED_LOGI("create dispatcher failed");
    }
#endif
    CompletePluginInitialization();
}

void PluginMgr::LoadGetExtConfigFunc()
{
    auto handle = dlopen(EXT_CONFIG_LIB, RTLD_NOW);
    if (!handle) {
        RESSCHED_LOGE("not find lib,errno: %{public}d", errno);
        return;
    }
    getExtMultiConfigFunc_ = reinterpret_cast<GetExtMultiConfigFunc>(dlsym(handle, "GetExtMultiConfig"));
    if (!getExtMultiConfigFunc_) {
        RESSCHED_LOGE("dlsym getExtConfig func failed!");
        dlclose(handle);
    }
}

void PluginMgr::GetConfigContent(int32_t configIdx, const std::string& configPath, std::vector<std::string>& contents)
{
    if (configIdx != -1 && getExtMultiConfigFunc_) {
        getExtMultiConfigFunc_(configIdx, contents);
        return;
    }
    auto configFilePaths = GetAllRealConfigPath(configPath);
    if (configFilePaths.size() <= 0) {
        return;
    }
    for (auto configFilePath : configFilePaths) {
        if (configFilePath.empty()) {
            continue;
        }
        std::string content;
        LoadStringFromFile(configFilePath, content);
        contents.emplace_back(content);
    }
}

std::vector<std::string> PluginMgr::GetAllRealConfigPath(const std::string& configName)
{
    std::vector<std::string> configFilePaths;
    auto cfgDirList = GetCfgDirList();
    if (cfgDirList == nullptr) {
        return configFilePaths;
    }
    std::string baseRealPath;
    for (const auto& cfgDir : cfgDirList->paths) {
        if (cfgDir == nullptr) {
            continue;
        }
        if (!CheckRealPath(std::string(cfgDir) + "/" + configName, baseRealPath)) {
            continue;
        }
        configFilePaths.emplace_back(baseRealPath);
    }
    FreeCfgDirList(cfgDirList);
    return configFilePaths;
}

bool PluginMgr::CheckRealPath(const std::string& partialPath, std::string& fullPath)
{
    char tmpPath[PATH_MAX] = {0};
    if (partialPath.size() > PATH_MAX || !realpath(partialPath.c_str(), tmpPath)) {
        return false;
    }
    fullPath = tmpPath;
    return true;
}

void PluginMgr::LoadPlugin()
{
    if (!pluginSwitch_) {
        RESSCHED_LOGW("%{public}s, pluginSwitch null!", __func__);
        return;
    }

    std::list<PluginInfo> pluginInfoList = pluginSwitch_->GetPluginSwitch();
    for (const auto& info : pluginInfoList) {
        if (!info.switchOn) {
            continue;
        }
        if (pluginLibMap_.find(info.libPath) != pluginLibMap_.end()) {
            continue;
        }
        shared_ptr<PluginLib> libInfoPtr = LoadOnePlugin(info);
        if (libInfoPtr == nullptr) {
            continue;
        }
        std::lock_guard<std::mutex> autoLock(pluginMutex_);
        pluginLibMap_.emplace(info.libPath, *libInfoPtr);

        RESSCHED_LOGD("%{public}s, init %{private}s success!", __func__, info.libPath.c_str());
    }
}

shared_ptr<PluginLib> PluginMgr::LoadOnePlugin(const PluginInfo& info)
{
    if (!ResCommonUtil::IsValidPath(info.libPath)) {
        RESSCHED_LOGE("%{public}s, libPath error!", __func__);
        HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::RSS, "INIT_FAULT", HiviewDFX::HiSysEvent::EventType::FAULT,
            "COMPONENT_NAME", info.libPath, "ERR_TYPE", "plugin failure",
            "ERR_MSG", info.libPath + "is error path!");
        return nullptr;
    }
    auto pluginHandle = dlopen(info.libPath.c_str(), RTLD_NOW);
    if (!pluginHandle) {
        RESSCHED_LOGE("%{public}s, not find plugin lib !", __func__);
        HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::RSS, "INIT_FAULT", HiviewDFX::HiSysEvent::EventType::FAULT,
                        "COMPONENT_NAME", "MAIN", "ERR_TYPE", "plugin failure",
                        "ERR_MSG", "PluginMgr dlopen " + info.libPath + " failed!");
        return nullptr;
    }
    std::string errorMsg = "";
    OnPluginInitFunc onPluginInitFunc;
    OnPluginDisableFunc onPluginDisableFunc;
    if (!CheckValidPlugin(info, pluginHandle, errorMsg, onPluginInitFunc, onPluginDisableFunc)) {
        RESSCHED_LOGE("%{public}s, dlsym OnPluginInit failed!", __func__);
        HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::RSS, "INIT_FAULT", HiviewDFX::HiSysEvent::EventType::FAULT,
                        "COMPONENT_NAME", "MAIN", "ERR_TYPE", "plugin failure",
                        "ERR_MSG", errorMsg);
        dlclose(pluginHandle);
        return nullptr;
    }

    // OnDispatchResource is not necessary for plugin
    auto onDispatchResourceFunc = reinterpret_cast<OnDispatchResourceFunc>(dlsym(pluginHandle,
        "OnDispatchResource"));

    // OnDeliverResource is not necessary for plugin
    auto onDeliverResourceFunc = reinterpret_cast<OnDeliverResourceFunc>(dlsym(pluginHandle,
        "OnDeliverResource"));

    // OnDispatchResource is not necessary for plugin
    auto onDumpFunc = reinterpret_cast<OnDumpFunc>(dlsym(pluginHandle, "OnDump"));

    PluginLib libInfo;
    libInfo.handle = std::shared_ptr<void>(pluginHandle, dlclose);
    libInfo.onPluginInitFunc_ = onPluginInitFunc;
    libInfo.onDispatchResourceFunc_ = onDispatchResourceFunc;
    libInfo.onDeliverResourceFunc_ = onDeliverResourceFunc;
    libInfo.onDumpFunc_ = onDumpFunc;
    libInfo.onPluginDisableFunc_ = onPluginDisableFunc;
    return make_shared<PluginLib>(libInfo);
}

bool PluginMgr::CheckValidPlugin(const PluginInfo& info, void* pluginHandle, std::string& errorMsg,
    OnPluginInitFunc& onPluginInitFunc, OnPluginDisableFunc& onPluginDisableFunc)
{
    onPluginInitFunc = reinterpret_cast<OnPluginInitFunc>(dlsym(pluginHandle, "OnPluginInit"));
    if (!onPluginInitFunc) {
        RESSCHED_LOGE("%{public}s, dlsym OnPluginInit failed!", __func__);
        errorMsg = "PluginMgr dlsym OnPluginInit " + info.libPath + "failed!";
        return false;
    }

    onPluginDisableFunc = reinterpret_cast<OnPluginDisableFunc>(dlsym(pluginHandle, "OnPluginDisable"));
    if (!onPluginDisableFunc) {
        RESSCHED_LOGE("%{public}s, dlsym OnPluginDisable failed!", __func__);
        errorMsg = "PluginMgr don't found dlsym " + info.libPath + "!";
        return false;
    }

    if (!onPluginInitFunc(const_cast<std::string&>(info.libPath))) {
        RESSCHED_LOGE("%{public}s, %{private}s init failed!", __func__, info.libPath.c_str());
        errorMsg = "Plugin " + info.libPath + " init failed!";
        return false;
    }
    return true;
}

PluginConfig PluginMgr::GetConfig(const std::string& pluginName, const std::string& configName)
{
    PluginConfig config;
    if (!configReader_) {
        return config;
    }
    return configReader_->GetConfig(pluginName, configName);
}

void PluginMgr::RemoveConfig(const std::string& pluginName, const std::string& configName)
{
    PluginConfig config;
    if (!configReader_) {
        return;
    }
    configReader_->RemoveConfig(pluginName, configName);
}

void PluginMgr::RemovePluginConfig(const std::string& pluginName)
{
    PluginConfig config;
    if (!configReader_) {
        return;
    }
    configReader_->RemovePluginConfig(pluginName);
}

void PluginMgr::Stop()
{
    OnDestroy();
}

#ifdef RESOURCE_SCHEDULE_SERVICE_WITH_EXT_RES_ENABLE
int32_t PluginMgr::GetExtTypeByResPayload(const std::shared_ptr<ResData>& resData)
{
    if (!resData || resData->resType != ResType::RES_TYPE_KEY_PERF_SCENE) {
        return DEFAULT_VALUE;
    }
    auto payload = resData->payload;
    if (!payload.contains(EXT_RES_KEY) || !payload[EXT_RES_KEY].is_string()) {
        return DEFAULT_VALUE;
    }
    int type = DEFAULT_VALUE;
    if (StrToInt(payload[EXT_RES_KEY], type)) {
        return type;
    } else {
        return DEFAULT_VALUE;
    }
}
#endif

bool PluginMgr::GetPluginListByResType(uint32_t resType, std::list<std::string>& pluginList)
{
    std::lock_guard<std::mutex> autoLock(resTypeMutex_);
    auto iter = resTypeLibMap_.find(resType);
    if (iter == resTypeLibMap_.end()) {
        RESSCHED_LOGD("%{public}s, PluginMgr resType no lib register!", __func__);
        return false;
    }
    pluginList.insert(pluginList.end(), iter->second.begin(), iter->second.end());
    return true;
}

inline void BuildSimplifyLibAll(const std::list<std::string>& pluginList, std::string& simplifyLibAll)
{
    for (const auto& libName : pluginList) {
        if (libName.length() < (SIMPLIFY_LIB_INDEX + SIMPLIFY_LIB_LENGTH)) {
            return;
        }
        simplifyLibAll.append(libName.substr(SIMPLIFY_LIB_INDEX, SIMPLIFY_LIB_LENGTH));
    }
}

void PluginMgr::GetResTypeList(std::set<uint32_t>& resTypeList)
{
    {
        std::lock_guard<std::mutex> autoLock(resTypeMutex_);
        for (const auto& [type, pluginList] : resTypeLibMap_) {
            resTypeList.insert(type);
        }
    }
    {
        std::lock_guard<std::mutex> autoLock(resTypeResValueMutex_);
        for (const auto& [typeValuePair, pluginList] : resTyperesValueLibMap_) {
            resTypeList.insert(typeValuePair.first);
        }
    }
}
std::shared_ptr<PluginLib> PluginMgr::GetPluginLib(const std::string& libPath)
{
    std::lock_guard<std::mutex> autoLock(libPathMutex_);
    auto iter = pluginLibMap_.find(libPath);
    if (iter == pluginLibMap_.end()) {
        RESSCHED_LOGE("%{public}s, PluginMgr libPath no lib register!", __func__);
        return nullptr;
    }
    return make_shared<PluginLib>(iter->second);
}

std::string PluginMgr::BuildDispatchTrace(const std::shared_ptr<ResData>& resData, std::string& libNameAll,
    const std::string& funcName, std::list<std::string>& pluginList)
{
    libNameAll.append("[");
    for (const auto& libName : pluginList) {
#ifdef RESOURCE_SCHEDULE_SERVICE_WITH_FFRT_ENABLE
        std::lock_guard<ffrt::mutex> autoLock(dispatcherHandlerMutex_);
        if (disablePlugins_.find(libName) != disablePlugins_.end()) {
            continue;
        }
#endif
        libNameAll.append(libName);
        libNameAll.append(",");
    }
    libNameAll.append("]");
    string trace_str(funcName);
    string resTypeString = GetStrFromResTypeStrMap(resData->resType);
    trace_str.append(" PluginMgr ,resType[").append(std::to_string(resData->resType)).append("]");
    trace_str.append(",resTypeStr[").append(resTypeString).append("]");
    trace_str.append(",value[").append(std::to_string(resData->value)).append("]");
    trace_str.append(",pluginlist:").append(libNameAll);
    return trace_str;
}

void PluginMgr::DispatchResource(const std::shared_ptr<ResData>& resData)
{
    if (!isInit.load()) {
        RESSCHED_LOGE("%{public}s, not init.", __func__);
        return;
    }
    if (!resData) {
        RESSCHED_LOGE("%{public}s, failed, null res data.", __func__);
        return;
    }
    std::list<std::string> pluginList;
#ifdef RESOURCE_SCHEDULE_SERVICE_WITH_EXT_RES_ENABLE
    int32_t extType = GetExtTypeByResPayload(resData);
    if (extType != DEFAULT_VALUE) {
        resData->resType = (uint32_t)extType;
    }
#endif
    GetPluginListByResType(resData->resType, pluginList);
    GetPluginListByResTypeAndValue(resData->resType, resData->value, pluginList);
    if (pluginList.empty()) {
        return;
    } else {
        std::unordered_set<std::string> uniqueSet(pluginList.begin(), pluginList.end());
        pluginList.assign(uniqueSet.begin(), uniqueSet.end());
    }
    std::string libNameAll = "";
    string trace_str = BuildDispatchTrace(resData, libNameAll, __func__, pluginList);
    StartTrace(HITRACE_TAG_APP, trace_str, -1);
    RESSCHED_LOGD("%{public}s, PluginMgr, resType = %{public}d, value = %{public}lld, pluginlist is %{public}s.",
        __func__, resData->resType, (long long)resData->value, libNameAll.c_str());
    FinishTrace(HITRACE_TAG_APP);
    std::string simplifyLibAll = "";
    BuildSimplifyLibAll(pluginList, simplifyLibAll);
    BatchLogPrinter::GetInstance().SubmitLog(std::to_string(resData->resType).
        append(",").append(std::to_string(resData->value)).append(",").append(simplifyLibAll));
#ifdef RESOURCE_SCHEDULE_SERVICE_WITH_FFRT_ENABLE
    DispatchResourceToPluginAsync(pluginList, resData);
#else
    std::lock_guard<std::mutex> autoLock(dispatcherHandlerMutex_);
    if (dispatcher_) {
        dispatcher_->PostTask(
            [pluginList, resData, this] {
                DispatchResourceToPluginSync(pluginList, resData);
            });
    }
#endif
}

int32_t PluginMgr::DeliverResource(const std::shared_ptr<ResData>& resData)
{
    if (!isInit.load()) {
        RESSCHED_LOGE("%{public}s, not init.", __func__);
        return PLUGIN_REQUEST_ERROR;
    }
    if (!resData) {
        RESSCHED_LOGE("%{public}s, failed, null res data.", __func__);
        return PLUGIN_REQUEST_ERROR;
    }

    std::string pluginLib;
    {
        std::lock_guard<std::mutex> autoLock(resTypeSyncMutex_);
        auto iter = resTypeLibSyncMap_.find(resData->resType);
        if (iter == resTypeLibSyncMap_.end()) {
            RESSCHED_LOGE("%{public}s, PluginMgr resType %{public}d no lib register!", __func__, resData->resType);
            return PLUGIN_REQUEST_ERROR;
        }
        pluginLib = iter->second;
    }

    PluginLib libInfo;
    {
        std::lock_guard<std::mutex> autoLock(pluginMutex_);
        auto itMap = pluginLibMap_.find(pluginLib);
        if (itMap == pluginLibMap_.end()) {
            RESSCHED_LOGE("%{public}s, no plugin %{public}s!", __func__, pluginLib.c_str());
            return PLUGIN_REQUEST_ERROR;
        }
        libInfo = itMap->second;
    }

    OnDeliverResourceFunc pluginDeliverFunc = libInfo.onDeliverResourceFunc_;
    if (!pluginDeliverFunc) {
        RESSCHED_LOGE("%{public}s, %{public}s no DeliverResourceFunc!", __func__, pluginLib.c_str());
        return PLUGIN_REQUEST_ERROR;
    }
    RESSCHED_LOGD("%{public}s, PluginMgr, resType = %{public}d, value = %{public}lld, plugin is %{public}s.",
        __func__, resData->resType, (long long)resData->value, pluginLib.c_str());

    int32_t ret;
    {
        std::string libName = "";
        std::list<std::string> pluginList = { pluginLib };
        std::string traceStr = BuildDispatchTrace(resData, libName, __func__, pluginList);
        HitraceScoped hitrace(HITRACE_TAG_OHOS, traceStr);
        InnerTimeUtil timeUtil(__func__, pluginLib);
        ret = pluginDeliverFunc(resData);
    }
    return ret;
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

void PluginMgr::UnSubscribeAllResources(const std::string& pluginLib)
{
    if (pluginLib.size() == 0) {
        RESSCHED_LOGE("%{public}s, PluginMgr failed, pluginLib is null.", __func__);
        return;
    }
    {
        std::lock_guard<std::mutex> autoLock(resTypeMutex_);
        std::unordered_set<uint32_t> keysToDelete;
        for (auto iter : resTypeLibMap_) {
            auto it = std::find(iter.second.begin(), iter.second.end(), pluginLib);
            if (it != iter.second.end()) {
                iter.second.erase(it);
            }
            if (iter.second.empty()) {
                keysToDelete.insert(iter.first);
            }
        }
        for (auto key : keysToDelete) {
            resTypeLibMap_.erase(key);
        }
    }
    {
        std::lock_guard<std::mutex> autoLock(resTypeResValueMutex_);
        std::unordered_set<ResPair, std::hash<ResPair>> keysToDelete;
        for (auto iter : resTyperesValueLibMap_) {
            auto it = std::find(iter.second.begin(), iter.second.end(), pluginLib);
            if (it != iter.second.end()) {
                iter.second.erase(it);
            }
            if (iter.second.empty()) {
                keysToDelete.insert(iter.first);
            }
        }
        for (auto key : keysToDelete) {
            resTyperesValueLibMap_.erase(key);
        }
    }
}

void PluginMgr::SubscribeSyncResource(const std::string& pluginLib, uint32_t resType)
{
    if (pluginLib.empty()) {
        RESSCHED_LOGE("%{public}s, PluginMgr failed, pluginLib is null.", __func__);
        return;
    }
    std::lock_guard<std::mutex> autoLock(resTypeSyncMutex_);
    auto iter = resTypeLibSyncMap_.find(resType);
    if (iter != resTypeLibSyncMap_.end()) {
        RESSCHED_LOGW("%{public}s, resType[%{public}d] subcribed by [%{public}s], replace by [%{public}s].",
            __func__, resType, iter->second.c_str(), pluginLib.c_str());
    }
    resTypeLibSyncMap_[resType] = pluginLib;
}

void PluginMgr::UnSubscribeSyncResource(const std::string& pluginLib, uint32_t resType)
{
    if (pluginLib.empty()) {
        RESSCHED_LOGE("%{public}s, PluginMgr failed, pluginLib is null.", __func__);
        return;
    }
    std::lock_guard<std::mutex> autoLock(resTypeSyncMutex_);
    auto iter = resTypeLibSyncMap_.find(resType);
    if (iter == resTypeLibSyncMap_.end()) {
        RESSCHED_LOGE("%{public}s, PluginMgr failed, res type has no plugin subscribe.", __func__);
        return;
    }
    resTypeLibSyncMap_.erase(iter);
}

void PluginMgr::DumpAllPlugin(std::string &result)
{
    std::list<PluginInfo> pluginInfoList = pluginSwitch_->GetPluginSwitch();
    for (const auto& info : pluginInfoList) {
        result.append(info.libPath).append(" ");
        DumpPluginInfoAppend(result, info);
    }
}

void PluginMgr::DumpAllPluginConfig(std::string &result)
{
    std::list<PluginInfo> pluginInfoList = pluginSwitch_->GetPluginSwitch();
    result.append("================Resource Schedule Plugin Switch================\n");
    for (const auto& info : pluginInfoList) {
        result.append(info.libPath).append(" ").append(std::to_string(info.switchOn)).append("\n");
    }
    configReader_->Dump(result);
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
        result.append(pluginName).append(" ");
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

void PluginMgr::ClearResource()
{
    {
        std::lock_guard<std::mutex> autoLock(resTypeMutex_);
        resTypeLibMap_.clear();
    }
    {
        std::lock_guard<std::mutex> autoLock(resTypeSyncMutex_);
        resTypeLibSyncMap_.clear();
    }
    {
        std::lock_guard<std::mutex> autoLock(resTypeStrMutex_);
        resTypeStrMap_.clear();
    }
    {
        std::lock_guard<std::mutex> autoLock(linkJumpOptMutex_);
        linkJumpOptSet_.clear();
    }
    {
        std::lock_guard<std::mutex> autoLock(resTypeResValueMutex_);
        resTyperesValueLibMap_.clear();
    }
}

void PluginMgr::RepairPlugin(TimePoint endTime, const std::string& pluginLib, PluginLib libInfo)
{
    auto& timeOutTime = pluginStat_[pluginLib].timeOutTime;
    int32_t crash_time = (int32_t)((endTime - timeOutTime.front()) / std::chrono::milliseconds(1));
    timeOutTime.emplace_back(endTime);
    RESSCHED_LOGW("%{public}s %{public}s crash %{public}d times in %{public}d ms!", __func__,
        pluginLib.c_str(), (int32_t)timeOutTime.size(), crash_time);
    if ((int32_t)timeOutTime.size() >= MAX_PLUGIN_TIMEOUT_TIMES) {
        if (crash_time < DISABLE_PLUGIN_TIME) {
            // disable plugin forever
            RESSCHED_LOGE("%{public}s, %{public}s disable it forever.", __func__, pluginLib.c_str());
            if (libInfo.onPluginDisableFunc_) {
                libInfo.onPluginDisableFunc_();
            }
            HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::RSS, "PLUGIN_DISABLE",
                HiSysEvent::EventType::FAULT, "plugin_name", pluginLib);
            timeOutTime.clear();
            pluginMutex_.lock();
            pluginLibMap_.erase(pluginLib);
            pluginMutex_.unlock();
            return;
        }

        timeOutTime.pop_front();
    }

    if (libInfo.onPluginDisableFunc_ && libInfo.onPluginInitFunc_) {
        RESSCHED_LOGW("%{public}s, %{public}s disable and enable it.", __func__, pluginLib.c_str());
        libInfo.onPluginDisableFunc_();
        libInfo.onPluginInitFunc_(const_cast<std::string&>(pluginLib));
    }
}

void PluginMgr::DispatchResourceToPluginSync(const std::list<std::string>& pluginList,
    const std::shared_ptr<ResData>& resData)
{
    auto sortPluginList = SortPluginList(pluginList);
    for (auto& pluginLib : sortPluginList) {
        PluginLib libInfo;
        {
            std::lock_guard<std::mutex> autoLock(pluginMutex_);
            auto itMap = pluginLibMap_.find(pluginLib);
            if (itMap == pluginLibMap_.end()) {
                RESSCHED_LOGE("%{public}s, no plugin %{public}s !", __func__, pluginLib.c_str());
                continue;
            }
            libInfo = itMap->second;
        }
        OnDispatchResourceFunc pluginDispatchFunc = libInfo.onDispatchResourceFunc_;
        if (!pluginDispatchFunc) {
            RESSCHED_LOGE("%{public}s, no DispatchResourceFun !", __func__);
            continue;
        }

        StartTrace(HITRACE_TAG_APP, pluginLib);
        auto beginTime = Clock::now();
        pluginDispatchFunc(std::make_shared<ResData>(resData->resType, resData->value, resData->payload));
        auto endTime = Clock::now();
        FinishTrace(HITRACE_TAG_APP);
        int32_t costTimeUs = (endTime - beginTime) / std::chrono::microseconds(1);
        int32_t costTime = costTimeUs / 1000;
        pluginStat_[pluginLib].Update(costTimeUs);

        if (costTime > DISPATCH_TIME_OUT) {
            // dispatch resource use too long time, unload it
            RESSCHED_LOGE("%{public}s, ERROR :%{public}s cost time(%{public}dms) over %{public}d ms! disable it.",
                __func__, pluginLib.c_str(), costTime, DISPATCH_TIME_OUT);
            RepairPlugin(endTime, pluginLib, libInfo);
        } else if (costTime > DISPATCH_WARNING_TIME) {
            RESSCHED_LOGW("%{public}s, WARNING :%{public}s plugin cost time(%{public}dms) over %{public}d ms!",
                __func__, pluginLib.c_str(), costTime, DISPATCH_WARNING_TIME);
        }
    }
}

#ifdef RESOURCE_SCHEDULE_SERVICE_WITH_FFRT_ENABLE
void PluginMgr::DispatchResourceToPluginAsync(const std::list<std::string>& pluginList,
    const std::shared_ptr<ResData>& resData)
{
    std::lock_guard<ffrt::mutex> autoLock(dispatcherHandlerMutex_);
    for (auto& pluginLib : pluginList) {
        if (disablePlugins_.find(pluginLib) != disablePlugins_.end()) {
            continue;
        }
        PluginLib libInfo;
        {
            std::lock_guard<std::mutex> autoLock(pluginMutex_);
            auto itMap = pluginLibMap_.find(pluginLib);
            if (itMap == pluginLibMap_.end()) {
                RESSCHED_LOGE("%{public}s, no plugin %{public}s !", __func__, pluginLib.c_str());
                continue;
            }
            libInfo = itMap->second;
        }
        OnDispatchResourceFunc pluginDispatchFunc = libInfo.onDispatchResourceFunc_;
        if (!pluginDispatchFunc) {
            RESSCHED_LOGE("%{public}s, no DispatchResourceFun !", __func__);
            continue;
        }
        if (dispatchers_[pluginLib] == nullptr) {
            continue;
        }
        dispatchers_[pluginLib]->submit(
            [pluginLib, resData, pluginDispatchFunc] {
                StartTrace(HITRACE_TAG_APP, pluginLib);
                pluginDispatchFunc(std::make_shared<ResData>(resData->resType, resData->value, resData->payload));
                FinishTrace(HITRACE_TAG_APP);
            });
    }
}

void PluginMgr::EnablePluginIfResume(const std::string& pluginLib)
{
    std::lock_guard<ffrt::mutex> autoLock(dispatcherHandlerMutex_);
    ffrt_queue_cancel_all(*reinterpret_cast<ffrt_queue_t*>(dispatchers_[pluginLib].get()));
    if (!IsPluginRunning(pluginLib)) {
        disablePlugins_.erase(pluginLib);
    } else {
        StartTrace(HITRACE_TAG_APP, "Blocked plugin:" + pluginLib, -1);
        RESSCHED_LOGI("plugin:%{public}s been locked!", pluginLib.c_str());
        FinishTrace(HITRACE_TAG_APP);
        ffrt::submit([lib = pluginLib]() {
            PluginMgr::GetInstance().EnablePluginIfResume(lib);
            }, {}, {}, ffrt::task_attr().delay(pluginBlockTime));
    }
}

void PluginMgr::HandlePluginTimeout(const std::string& pluginLib)
{
    std::lock_guard<ffrt::mutex> autoLock(dispatcherHandlerMutex_);
    ffrt_queue_cancel_all(*reinterpret_cast<ffrt_queue_t*>(dispatchers_[pluginLib].get()));
    if (IsPluginRunning(pluginLib)) {
        RESSCHED_LOGI("%{public}s, %{public}s has blocked task, stop dispatch resource to it!",
            __func__, pluginLib.c_str());
            disablePlugins_.insert(pluginLib);
    } else {
        RESSCHED_LOGI("%{public}s, %{public}s all tasks cancled!", __func__, pluginLib.c_str());
    }
}

void PluginMgr::RecordRinningStat(std::string pluginLib, bool isRunning)
{
    std::lock_guard<ffrt::mutex> autoLock(runningStatsMutex_);
    runningStats_[pluginLib] = isRunning;
}

bool PluginMgr::IsPluginRunning(const std::string& pluginLib)
{
    std::lock_guard<ffrt::mutex> autoLock(runningStatsMutex_);
    auto stateItem = runningStats_.find(pluginLib);
    if (stateItem == runningStats_.end()) {
        return false;
    }
    return stateItem->second;
}
#endif

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
    pluginSwitch_ = nullptr;
    ClearResource();
#ifdef RESOURCE_SCHEDULE_SERVICE_WITH_FFRT_ENABLE
    std::lock_guard<ffrt::mutex> autoLock(dispatcherHandlerMutex_);
    dispatchers_.clear();
#else
    std::lock_guard<std::mutex> autoLock(dispatcherHandlerMutex_);
    if (dispatcher_) {
        dispatcher_->RemoveAllEvents();
        dispatcher_ = nullptr;
    }
#endif
}

void PluginMgr::SetResTypeStrMap(const std::map<uint32_t, std::string>& resTypeStr)
{
    std::lock_guard<std::mutex> autoLock(resTypeStrMutex_);
    resTypeStrMap_ = resTypeStr;
}

void PluginMgr::SetBlockedTime(const int64_t time)
{
    pluginBlockTime = time;
}

void PluginMgr::SetLinkJumpOptSet(const std::unordered_set<std::string> linkJumpOptSet)
{
    std::lock_guard<std::mutex> autoLock(linkJumpOptMutex_);
    linkJumpOptSet_ = linkJumpOptSet;
}

bool PluginMgr::GetLinkJumpOptConfig(const std::string& bundleName, bool& isAllowedLinkJump)
{
    std::lock_guard<std::mutex> autoLock(linkJumpOptMutex_);
    auto iter = linkJumpOptSet_.find(bundleName);
    if (iter != linkJumpOptSet_.end()) {
        isAllowedLinkJump = true;
        return true;
    }
    return false;
}

std::string PluginMgr::GetStrFromResTypeStrMap(uint32_t resType)
{
    std::lock_guard<std::mutex> autoLock(resTypeStrMutex_);
    return resTypeStrMap_.count(resType) ? resTypeStrMap_.at(resType) : "UNKNOWN";
}

std::list<std::string> PluginMgr::SortPluginList(const std::list<std::string>& pluginList)
{
    auto sortPluginList = pluginList;
    sortPluginList.sort([&](const std::string& a, const std::string& b) -> bool {
        if (pluginStat_.find(a) == pluginStat_.end() || pluginStat_.find(b) == pluginStat_.end()) {
            return false;
        }
        return pluginStat_[a].AverageTime() < pluginStat_[b].AverageTime();
    });
    return sortPluginList;
}

PluginMgr::InnerTimeUtil::InnerTimeUtil(const std::string& func, const std::string& plugin)
{
    beginTime_ = Clock::now();
    functionName_ = func;
    pluginName_ = plugin;
}

PluginMgr::InnerTimeUtil::~InnerTimeUtil()
{
    auto endTime = Clock::now();
    int32_t costTime = (endTime - beginTime_) / std::chrono::milliseconds(1);
    RESSCHED_LOGD("%{public}s, %{public}s plugin cost time(%{public}d ms).",
        functionName_.c_str(), pluginName_.c_str(), costTime);
    if (costTime > DISPATCH_WARNING_TIME) {
        RESSCHED_LOGW("%{public}s, WARNING :%{public}s plugin cost time(%{public}d ms) over %{public}d ms!",
            functionName_.c_str(), pluginName_.c_str(), costTime, DISPATCH_WARNING_TIME);
    }
}
extern "C" {
    void SetBlockedTime(const int64_t time)
    {
        PluginMgr::GetInstance().SetBlockedTime(time);
    }
}

void PluginMgr::SubscribeResourceAccurately(const std::string& pluginLib, uint32_t resType, int64_t resValue)
{
    if (pluginLib.empty()) {
        RESSCHED_LOGE("%{public}s, PluginMgr failed, pluginLib is null.", __func__);
        return;
    }
    if (!subscriptionAccuractlyEnable_) {
        RESSCHED_LOGD("%{public}s disable", __func__);
        SubscribeResource(pluginLib, resType);
        return;
    }
    std::lock_guard<std::mutex> autoLock(resTypeResValueMutex_);
    ResPair resTypeValuePair = std::make_pair(resType, resValue);
    resTyperesValueLibMap_[resTypeValuePair].emplace_back(pluginLib);
}

void PluginMgr::UnSubscribeResourceAccurately(const std::string& pluginLib, uint32_t resType, int64_t resValue)
{
    if (pluginLib.empty()) {
        RESSCHED_LOGE("%{public}s, PluginMgr failed, pluginLib is null.", __func__);
        return;
    }
    if (!subscriptionAccuractlyEnable_) {
        RESSCHED_LOGD("%{public}s disable", __func__);
        UnSubscribeResource(pluginLib, resType);
        return;
    }
    std::lock_guard<std::mutex> autoLock(resTypeResValueMutex_);
    ResPair resTypeValuePair = std::make_pair(resType, resValue);
    auto iter = resTyperesValueLibMap_.find(resTypeValuePair);
    if (iter == resTyperesValueLibMap_.end()) {
        RESSCHED_LOGE("%{public}s, PluginMgr failed, res type and value has no plugin subscribe.", __func__);
        return;
    }

    iter->second.remove(pluginLib);
    if (iter->second.empty()) {
        resTyperesValueLibMap_.erase(iter);
    }
}

bool PluginMgr::GetPluginListByResTypeAndValue(uint32_t resType, int64_t resValue, std::list<std::string>& pluginList)
{
    std::lock_guard<std::mutex> autoLock(resTypeResValueMutex_);
    ResPair resTypeValuePair = std::make_pair(resType, resValue);
    auto iter = resTyperesValueLibMap_.find(resTypeValuePair);
    if (iter == resTyperesValueLibMap_.end()) {
        RESSCHED_LOGD("%{public}s, PluginMgr resType and resValue no lib register!", __func__);
        return false;
    }
    pluginList.insert(pluginList.end(), iter->second.begin(), iter->second.end());
    return true;
}

void PluginMgr::ReadSubscriptionAccuractlyEnableProperties()
{
    std::string enableString = ResCommonUtil::GetSystemProperties(RSS_SUBSCRIPTION_ACCURATLY_ENABLE, "1");
    int32_t enableInt;
    if (StrToInt(enableString, enableInt)) {
        subscriptionAccuractlyEnable_ = static_cast<bool>(enableInt);
    }
}

void PluginMgr::RegisterOnInitFinishCallback(const std::string& libName,
    const OnInitFinishCallbackPtr& callback)
{
    if (!callback || !*callback) {
        RESSCHED_LOGE("%{public}s, invalid callback of plugin %{public}s!", __func__, libName.c_str());
        return;
    }
    std::lock_guard<std::mutex> autoLock(onInitFinishCallbackMutex_);
    initFinishCallbacks_[libName] = callback;
    RESSCHED_LOGI("%{public}s, callback registered successfully for lib: %{public}s!", __func__, libName.c_str());
}

void PluginMgr::CallOnInitFinishCallbacks()
{
    std::map<std::string, OnInitFinishCallbackPtr> callbacks;
    {
        std::lock_guard<std::mutex> autoLock(onInitFinishCallbackMutex_);
        callbacks.swap(initFinishCallbacks_);
    }

    for (const auto& [libName, callback] : callbacks) {
        if (!callback || !*callback) {
            continue;
        }
        RESSCHED_LOGI("%{public}s, calling callback for lib: %{public}s!", __func__, libName.c_str());
        (*callback)();
    }
}
} // namespace ResourceSchedule
} // namespace OHOS
