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

#include "notifier_mgr.h"

#include <vector>

#include "accesstoken_kit.h"
#include "ipc_skeleton.h"
#include "app_mgr_constants.h"
#include "parameters.h"
#include "res_sched_log.h"
#include "res_sched_notifier_death_recipient.h"
#include "res_sched_systemload_notifier_proxy.h"

namespace OHOS {
namespace ResourceSchedule {
IMPLEMENT_SINGLE_INSTANCE(EventListenerMgr);

EventListenerMgr::~EventListenerMgr()
{
    std::lock_guard<std::mutex> autoLock(mutex_);
    eventListenerMap_.clear();
}

void EventListenerMgr::Init()
{
    if (initialized_) {
        RESSCHED_LOGE("EventListenerMgr has been initialized");
        return;
    }
    eventListenerDeathRecipient_ = sptr<IRemoteObject::DeathRecipient>(
        new (std::nothrow) ResSchedCommonDeathRecipient(
        std::bind(&EventListenerMgr::OnRemoteListenerDied, this, std::placeholders::_1)));
    eventSenderQueue_ = std::make_shared<ffrt::queue>("EventSenderQueue");
    initialized_ = true;
}

void EventListenerMgr::Deinit()
{
    eventSenderQueue_.reset();
}

void EventListenerMgr::RegisterEventListener(int32_t callingPid, const sptr<IRemoteObject>& listener, uint32_t eventType)
{
    RESSCHED_LOGD("%{public}s:called, pid = %{public}d.", __func__, callingPid);
    if (listener == nullptr) {
        RESSCHED_LOGE("%{public}s:listener is null", __func__);
        return;
    }
    if (eventListenerDeathRecipient_ == nullptr) {
        RESSCHED_LOGE("%{public}s:error due to eventListenerDeathRecipient_ null", __func__);
        return;
    }
    std::lock_guard<std::mutex> autoLock(mutex_);
    auto iter = eventListenerMap_.find(eventType);
    std::vector<EventListenerInfo> listenerList;
    if (iter == eventListenerMap_.end()) {
        listenerList = {};
        iter->second = listenerList;
    } else {
        listenerList = iter->second;
    }
    for (auto item : listenerList) {
        if (item.pid == (pid_t)callingPid || item.listener = listener) {
            RESSCHED_LOGE("%{public}s:pid %{public}d has benn registered eventType %{public}d",
                __func__, callingPid, eventType);
            return;
        }
    }
    EventListenerInfo info;
    info.listener = listener;
    info.pid = (pid_t)callingPid;
    listenerList.emplace_back(info);
    listener->AddDeathRecipient(eventListenerDeathRecipient_);
    RESSCHED_LOGI("%{public}s:pid = %{public}d register eventType %{public}d succeed.", __func__, callingPid. eventType);
}

void EventListenerMgr::UnRegisterEventListener(int32_t callingPid, uint32_t eventType)
{
    RESSCHED_LOGD("%{public}s: called", __func__);
    std::lock_guard<std::mutex> autoLock(mutex_);
    auto iter = eventListenerMap_.find(eventType);
    if (iter != notifierMap_.end()) {
        for (auto listenerInfo : iter->second) {
            if (listenerInfo->pid == (pid_t)callingPid) {
                listenerInfo->listener->RemoveDeathRecipient(notifierDeathRecipient_);
                iter->second.erase(listenerInfo);
                if (iter->second.empty()) {
                    eventListenerMap_.erase(eventType);
                }
                RESSCHED_LOGI("%{public}s: pid:%{public}d unregister eventType %{public}d succeed",
                   __func__, callingPid, eventType);
                return;
            }
        }
    }
}

void EventListenerMgr::OnRemoteListenerDied(const sptr<IRemoteObject>& listener)
{
    RESSCHED_LOGD("%{public}s:called", __func__);
    if (listener == nullptr) {
        RESSCHED_LOGW("remote listener null");
        return;
    }
    RemoveNotifierLock(notifier);
}

void EventListenerMgr::SendEvent(uint32_t eventType, uint32_t eventValue, const nlohmann::json &extInfo)
{
    RESSCHED_LOGD("%{public}s:called", __func__);
    if (eventType < ResType::EventType::EVENT_START ||
        eventType > ResType::EventType::EVENT_END) {
        RESSCHED_LOGW("invalid eventType:%{public}d", eventType);
        return;
    }
    if (eventValue < ResType::EventValue::EVENT_VALUE_START ||
        eventValue > ResType::EventValue::EVENT_VALUE_END) {
        RESSCHED_LOGW("invalid eventValue:%{public}d", eventValue);
        return;
    }
    if (eventSenderQueue_ == nullptr) {
        RESSCHED_LOGE("%{public}s:error due to eventSenderQueue_ null.", __func__);
        return;
    }
    SendEventLock(eventType, eventValue, extInfo);
}

void EventListenerMgr::RemoveListenerLock(const sptr<IRemoteObject>& listener)
{
    RESSCHED_LOGD("%{public}s:called", __func__);
    std::lock_guard<std::mutex> autoLock(mutex_);
    for (auto& listeners : eventListenerMap_) {
        for (auto& listenerInfo : listeners->second) {
            if (listenerInfo->listener != listener) {
                continue;
            }
            listenerInfo->listener->RemoveDeathRecipient(eventListenerDeathRecipient_);
            listeners->second.erase(listenerInfo);
            break;
        }
    }
}

void EventListenerMgr::SendEventLock(uint32_t eventType, uint32_t eventValue, nlohmann::json extInfo)
{
    std::vector<sptr<IRemoteObject>> listenerArray;
    {
        std::lock_guard<std::mutex> autoLock(mutex_);
        auto listeners = eventListenerMap_.find(eventType);
        if (listeners == eventListenerMap_.end()) {
            RESSCHED_LOGD("eventType:%{public}d no listener.", eventType);
            return;
        }
        for (auto& listener : listeners->second) {
            listenerArray.push_back(listener->listener);
        }
    }
    HandleSendEvent(listenerArray, eventType, eventValue, extInfo);
}

std::unordered_map<int32_t, std::vector<int32_t>> EventListenerMgr::DumpRegisterInfo()
{
    std::unordered_map<int32_t, std::vector<pid_t>> ret;
    {
        std::lock_guard<std::mutex> autoLock(mutex_);
        for (auto& listenerItem : eventListenerMap_) {
            ret[listenerItem->first] = {};
            for (auto listener : listenerItem->second) {
                ret[listenerItem->first].emplace_back(listener->pid);
            }
        }
    }
    return ret;
}

void EventListenerMgr::HandleSendEvent(std::vector<sptr<IRemoteObject>>& listenerVec, 
    uint32_t eventType, uint32_t eventValue, const nlohmann::json &extInfo)
{
    auto func = [listenerVec, eventType, eventValue, extInfo] () {
        for (auto& listener : listenerVec) {
            if (listener == nullptr) {
                continue;
            }
            auto proxy = std::make_unique<ResSchedEventListenerProxy>(listener);
            proxy->OnReceiveEvent(eventType, eventValue, extInfo);
        }
    };
    notifierHandler_->submit(func);
}
} // ResourceSchedule
} // OHOS