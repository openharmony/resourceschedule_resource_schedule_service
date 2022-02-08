/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef CONTINUOUS_TASK_DUMMY_H
#define CONTINUOUS_TASK_DUMMY_H

#include "parcel.h"

namespace OHOS {
namespace BackgroundTaskMgr {
class ContinuousTaskEventData : public Parcelable {
public:
    enum : int32_t {
            LONGTIME_TASK_START = 0,
            LONGTIME_TASK_CANCEL
    };

    ContinuousTaskEventData();
    ContinuousTaskEventData(int32_t typeId, uid_t creatorUid, pid_t creatorPid, std::string abilityName);
    virtual ~ContinuousTaskEventData();

    int32_t GetTypeId()
    {
        return typeId_;
    }
    uid_t GetCreatorUid()
    {
        return creatorUid_;
    }
    pid_t GetCreatorPid()
    {
        return creatorPid_;
    }
    std::string GetAbilityName()
    {
        return abilityName_;
    }
    bool Marshalling(Parcel &parcel) const;
    static ContinuousTaskEventData *Unmarshalling(Parcel &parcel);

private:
    bool ReadFromParcel(Parcel &parcel);

    int32_t typeId_;
    uid_t creatorUid_;
    pid_t creatorPid_;
    std::string abilityName_;
};

class ContinuousTaskConstant final {
public:
    enum class  SubscribeResult : int32_t {
        SUCCESS = 0,
        FAILED
    };
};

class ContinuousTaskEventSubscriber {
public:
    ContinuousTaskEventSubscriber() {};
    virtual ~ContinuousTaskEventSubscriber() {};

    virtual void OnSubscribeResult(ContinuousTaskConstant::SubscribeResult result);
    virtual void OnUnsubscribeResult(ContinuousTaskConstant::SubscribeResult result);
    virtual void OnContinuousTaskStart(const std::shared_ptr<ContinuousTaskEventData> &continuousTaskEventData);
    virtual void OnContinuousTaskCancel(const std::shared_ptr<ContinuousTaskEventData> &continuousTaskEventData);
    virtual void OnDied();
};

class BackgroundTaskMgrHelper_ {
public:
    static bool RequestSubscribe(const ContinuousTaskEventSubscriber &subscriber)
    {
        return true;
    }

    static bool RequestUnsubscribe(const ContinuousTaskEventSubscriber &subscriber)
    {
        return true;
    }
};
}
}
#endif // CONTINUOUS_TASK_DUMMY_H