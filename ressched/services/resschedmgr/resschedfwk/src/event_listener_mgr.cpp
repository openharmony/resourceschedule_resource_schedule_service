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

#include "event_listener_mgr.h"

#include "accesstoken_kit.h"
#include "ipc_skeleton.h"
#include "app_mgr_constants.h"
#include "parameters.h"
#include "res_sched_log.h"
#include "res_sched_common_death_recipient.h"
#include "res_sched_event_listener_proxy.h"

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

void EventListenerMgr::RegisterEventListener(int32_t callingPid,
    const sptr<IRemoteObject>& listener, uint32_t eventType, uint32_t listenerGroup)
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
    if (iter != eventListenerMap_.end() && iter->second.find((pid_t)callingPid) != iter->second.end()) {
        if (iter->second[(pid_t)callingPid].groups.count(listenerGroup) == 0) {
            iter->second[(pid_t)callingPid].groups.emplace(listenerGroup);
            RESSCHED_LOGI("%{public}s:pid = %{public}d register eventType %{public}d group %{public}d succeed.",
                __func__, callingPid, eventType, listenerGroup);
        } else {
            RESSCHED_LOGE("%{public}s:pid %{public}d has benn registered eventType %{public}d group %{public}d",
                __func__, callingPid, eventType, listenerGroup);
        }
        return;
    }
    EventListenerInfo info;
    info.listener = listener;
    info.pid = (pid_t)callingPid;
    info.groups.emplace(listenerGroup);
    eventListenerMap_[eventType][info.pid] = info;
    listener->AddDeathRecipient(eventListenerDeathRecipient_);
    RESSCHED_LOGI("%{public}s:pid = %{public}d register eventType %{public}d group %{public}d succeed.",
        __func__, callingPid, eventType, listenerGroup);
}

void EventListenerMgr::UnRegisterEventListener(int32_t callingPid, uint32_t eventType, uint32_t listenerGroup)
{
    RESSCHED_LOGD("%{public}s: called", __func__);
    std::lock_guard<std::mutex> autoLock(mutex_);
    auto iter = eventListenerMap_.find(eventType);
    if (iter != eventListenerMap_.end()) {
        auto listenerItem = iter->second.find(callingPid);
        if (listenerItem != iter->second.end()) {
            listenerItem->second.groups.erase(listenerGroup);
            if (listenerItem->second.groups.size() != 0) {
                return;
            }
            listenerItem->second.listener->RemoveDeathRecipient(eventListenerDeathRecipient_);
            iter->second.erase(callingPid);
            if (iter->second.empty()) {
                eventListenerMap_.erase(eventType);
            }
            RESSCHED_LOGI("%{public}s: pid:%{public}d unregister eventType %{public}d succeed",
                __func__, callingPid, eventType);
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
    RemoveListenerLock(listener);
}

void EventListenerMgr::SendEvent(uint32_t eventType, uint32_t eventValue, const nlohmann::json &extInfo,
    uint32_t listenerGroup)
{
    RESSCHED_LOGD("%{public}s:called", __func__);
    if (eventType < ResType::EventType::EVENT_START ||
        eventType > ResType::EventType::EVENT_END) {
        RESSCHED_LOGW("invalid eventType:%{public}d", eventType);
        return;
    }
    if (eventSenderQueue_ == nullptr) {
        RESSCHED_LOGE("%{public}s:error due to eventSenderQueue_ null.", __func__);
        return;
    }
    SendEventLock(eventType, eventValue, extInfo, listenerGroup);
}

void EventListenerMgr::RemoveListenerLock(const sptr<IRemoteObject>& listener)
{
    RESSCHED_LOGD("%{public}s:called", __func__);
    std::lock_guard<std::mutex> autoLock(mutex_);
    for (auto& listeners : eventListenerMap_) {
        for (auto& listenerInfoItem : listeners.second) {
            if (listenerInfoItem.second.listener != listener) {
                continue;
            }
            listenerInfoItem.second.listener->RemoveDeathRecipient(eventListenerDeathRecipient_);
            listeners.second.erase(listenerInfoItem.first);
            break;
        }
    }
}

void EventListenerMgr::SendEventLock(uint32_t eventType, uint32_t eventValue, const nlohmann::json& extInfo,
    uint32_t listenerGroup)
{
    std::vector<sptr<IRemoteObject>> listenerArray;
    {
        std::lock_guard<std::mutex> autoLock(mutex_);
        auto listeners = eventListenerMap_.find(eventType);
        if (listeners == eventListenerMap_.end()) {
            RESSCHED_LOGD("eventType:%{public}d no listener.", eventType);
            return;
        }
        for (auto& listenerItem : listeners->second) {
            if (listenerItem.second.groups.count(listenerGroup) == 1) {
                listenerArray.push_back(listenerItem.second.listener);
            }
        }
    }
    HandleSendEvent(listenerArray, eventType, eventValue, extInfo, listenerGroup);
}

std::unordered_map<int32_t, std::vector<pid_t>> EventListenerMgr::DumpRegisterInfo()
{
    std::unordered_map<int32_t, std::vector<pid_t>> ret;
    {
        std::lock_guard<std::mutex> autoLock(mutex_);
        for (auto& listenerItem : eventListenerMap_) {
            ret[listenerItem.first] = {};
            for (auto listener : listenerItem.second) {
                ret[listenerItem.first].emplace_back(listener.second.pid);
            }
        }
    }
    return ret;
}

void EventListenerMgr::HandleSendEvent(std::vector<sptr<IRemoteObject>>& listenerVec,
    uint32_t eventType, uint32_t eventValue, const nlohmann::json &extInfo, uint32_t listenerGroup)
{
    auto func = [listenerVec, eventType, eventValue, extInfo, listenerGroup] () {
        for (auto& listener : listenerVec) {
            if (listener == nullptr) {
                continue;
            }
            auto proxy = std::make_unique<ResSchedEventListenerProxy>(listener);
            proxy->OnReceiveEvent(eventType, eventValue, listenerGroup,
                extInfo.dump(-1, ' ', false, nlohmann::detail::error_handler_t::replace));
        }
    };
    eventSenderQueue_->submit(func);
}
} // ResourceSchedule
} // OHOS