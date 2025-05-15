/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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

#ifndef RESSCHED_INTERFACES_INNERKITS_RESSCHED_CLIENT_INCLUDE_RES_SCHED_CLIENT_H
#define RESSCHED_INTERFACES_INNERKITS_RESSCHED_CLIENT_INCLUDE_RES_SCHED_CLIENT_H

#include <cstdint>                                  // for int64_t, uint32_t
#include <unordered_map>                            // for unordered_map
#include <unordered_set>                              // for unordered_set
#include <mutex>                                    // for mutex
#include <iosfwd>                                   // for string
#include <list>                                     // for list
#include "errors.h"                                 // for ErrCode
#include "iremote_object.h"                         // for IRemoteObject, IRemoteObject::DeathR...
#include "ires_sched_service.h"                     // for IResSchedService
#include "nocopyable.h"                             // for DISALLOW_COPY_AND_MOVE
#include "refbase.h"                                // for sptr, wptr
#include "res_sched_systemload_notifier_client.h"   // for ResSchedSystemloadNotifierClient
#include "res_sched_systemload_notifier_stub.h"     // for ResSchedSystemloadNotifierStub
#include "system_ability_status_change_stub.h"      // for SystemAbilityStatusChangeStub
#include "res_sched_event_listener.h"               // for ResSchedEvenetListener
#include "res_sched_event_listener_stub.h"          // for ResSchedEvenetListenerStub
#include "res_type.h"                               // for ResType
#include "nlohmann/json.hpp"

namespace OHOS {
namespace ResourceSchedule {
class ResSchedSvcStatusChange : public SystemAbilityStatusChangeStub {
public:
    ResSchedSvcStatusChange() = default;
    ~ResSchedSvcStatusChange() = default;
    void OnAddSystemAbility(int32_t systemAbilityId, const std::string& deviceId) override;
    void OnRemoveSystemAbility(int32_t systemAbilityId, const std::string& deviceId) override;
};
/*
 * this class wraped the functions of IResSchedService,effect is the same.
 * but through ResSchedClient, you don't need to get IResSchedService from samgr,
 * just use the functions is ok.
 */
class ResSchedClient {
    friend ResSchedSvcStatusChange;
public:
    /**
     * @brief Get the Instance object.
     *
     * @return Returns ResSchedClient&.
     */
    static ResSchedClient& GetInstance();

    /**
     * @brief Report resource data to the resource schedule service through inter-process communication.
     *
     * @param resType Indicates the resource type, all of the type have listed in res_type.h.
     * @param value Indicates the value of the resource type, defined by the developers.
     * @param mapPayload Indicates the context info of the resource type event.
     */
    void ReportData(uint32_t resType, int64_t value, const std::unordered_map<std::string, std::string>& mapPayload);

    /**
     * @brief Report the synchronization event to the resource schedule service.
     *
     * @param resType Indicates the resource type, all of the type have listed in res_type.h.
     * @param value Indicates the value of the resource type, defined by the developers.
     * @param payload Indicates the context info of the resource type event.
     * @param reply Indicates the return value of service processing.
     */
    int32_t ReportSyncEvent(const uint32_t resType, const int64_t value, const nlohmann::json& payload,
        nlohmann::json& reply);

    /**
     * @brief Report the synchronization event to the resource schedule service.
     *
     * @param resType Indicates the resource type, all of the type have listed in res_type.h.
     * @param value Indicates the value of the resource type, defined by the developers.
     * @param payload Indicates the context info of the resource type event.
     * @param reply Indicates the return value of service processing.
     */
    int32_t ReportSyncEvent(const uint32_t resType, const int64_t value,
        const std::unordered_map<std::string, std::string>& payload,
        std::unordered_map<std::string, std::string>& reply);

    /**
     * @brief Kill process with pid.
     *
     * @param mapPayload Indicates the context info of the kill message.
     */
    int32_t KillProcess(const std::unordered_map<std::string, std::string>& mapPayload);

    /**
     * @brief Stop remote Object, reset ResSchedClient.
     */
    void StopRemoteObject();

    /**
     * @brief Register systemload level listener.
     *
     * @param callbackObj systemload level listener object.
     */
    void RegisterSystemloadNotifier(const sptr<ResSchedSystemloadNotifierClient>& callbackObj);

    /**
     * @brief UnRegister systemload level listener.
     *
     * @param callbackObj systemload level listener object
     */
    void UnRegisterSystemloadNotifier(const sptr<ResSchedSystemloadNotifierClient>& callbackObj);

        /**
     * @brief Register event listener.
     *
     * @param eventListener event listener object.
     * @param eventType event type.
     */
    void RegisterEventListener(const sptr<ResSchedEventListener>& eventListener, uint32_t eventType,
        uint32_t listenerGroup = ResType::EventListenerGroup::LISTENER_GROUP_COMMON);

    /**
     * @brief UnRegister event listener.
     *
     * @param eventListener event listener object.
     * @param eventType event type
     */
    void UnRegisterEventListener(const sptr<ResSchedEventListener>& eventListener, uint32_t eventType,
        uint32_t listenerGroup = ResType::EventListenerGroup::LISTENER_GROUP_COMMON);

    /**
     * @brief client get systemload level.
     */
    int32_t GetSystemloadLevel();

    /**
     * @brief is allowed appliacation preload through resource scheduling services.
     *
     * @param bundleName bundleName of the application.
     */
    bool IsAllowedAppPreload(const std::string& bundleName, int32_t preloadMode);

    /**
     * @brief Check whether the application is in the link jump white list.
     * Calling from application process.
     *
     * @param isAllowedLinkJump true if the application is in the link jump white list.
     */
    int32_t IsAllowedLinkJump(bool& isAllowedLinkJump);

protected:
    ResSchedClient() = default;
    virtual ~ResSchedClient();

private:
    void AddResSaListenerLocked();
    void OnAddSystemAbility(int32_t systemAbilityId, const std::string& deviceId);
    int32_t InitSystemloadListenersLocked();
    int32_t InitInnerEventListenerLocked();
    void UnRegisterSystemloadListenersLocked();
    void UnRegisterEventListenerLocked(uint32_t eventType, uint32_t listenerGroup);
    void RecoverEventListener();
    class SystemloadLevelListener : public ResSchedSystemloadNotifierStub {
    public:
        SystemloadLevelListener() = default;
        virtual ~SystemloadLevelListener();
        void RegisterSystemloadLevelCb(const sptr<ResSchedSystemloadNotifierClient>& callbackObj);
        void UnRegisterSystemloadLevelCb(const sptr<ResSchedSystemloadNotifierClient>& callbackObj);
        bool IsSystemloadCbArrayEmpty();
        ErrCode OnSystemloadLevel(int32_t level) override;
    private:
        std::mutex listMutex_;
        std::list<sptr<ResSchedSystemloadNotifierClient>> systemloadLevelCbs_;
    };
    class InnerEventListener : public ResSchedEventListenerStub {
    public:
        InnerEventListener() = default;
        virtual ~InnerEventListener();
        void RegisterEventListener(const sptr<ResSchedEventListener>& eventListener, uint32_t eventType,
            uint32_t listenerGroup = ResType::EventListenerGroup::LISTENER_GROUP_COMMON);
        void UnRegisterEventListener(const sptr<ResSchedEventListener>& eventListener, uint32_t eventType,
            uint32_t listenerGroup = ResType::EventListenerGroup::LISTENER_GROUP_COMMON);
        ErrCode OnReceiveEvent(uint32_t eventType, uint32_t eventValue, uint32_t listenerGroup,
            const std::string& extInfo) override;
        bool IsInnerEventMapEmpty(uint32_t eventType, uint32_t listenerGroup);
        std::unordered_map<uint32_t, std::list<uint32_t>> GetRegisteredTypesAndGroup();
    private:
        std::mutex eventMutex_;
        std::unordered_map<uint32_t, std::unordered_map<uint32_t,
            std::list<sptr<ResSchedEventListener>>>> eventListeners_;
    };
    class ResSchedDeathRecipient : public IRemoteObject::DeathRecipient {
    public:
        explicit ResSchedDeathRecipient(ResSchedClient &resSchedClient);

        ~ResSchedDeathRecipient();

        void OnRemoteDied(const wptr<IRemoteObject> &object) override;

    private:
        ResSchedClient &resSchedClient_;
    };
    sptr<IResSchedService> GetProxy();
    ErrCode TryConnect();
    std::mutex mutex_;
    sptr<ResSchedDeathRecipient> recipient_;
    sptr<IRemoteObject> remoteObject_;
    sptr<IResSchedService> rss_;
    sptr<SystemloadLevelListener> systemloadLevelListener_;
    sptr<InnerEventListener> innerEventListener_;
    sptr<ResSchedSvcStatusChange> resSchedSvcStatusListener_;
    bool systemloadCbRegistered_ = false;
    bool isGetResTypeList_ = false;
    std::set<uint32_t> resTypeList_;
    std::unordered_map<uint32_t, std::unordered_set<uint32_t>> registeredInnerEvents;
    DISALLOW_COPY_AND_MOVE(ResSchedClient);
};
} // namespace ResourceSchedule
} // namespace OHOS

#endif // RESSCHED_INTERFACES_INNERKITS_RESSCHED_CLIENT_INCLUDE_RES_SCHED_CLIENT_H
