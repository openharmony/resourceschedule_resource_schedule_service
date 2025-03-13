/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef OHOS_IPC_IPC_SKELETON_H
#define OHOS_IPC_IPC_SKELETON_H

#include "iremote_object.h"

namespace OHOS {
class IPCSkeleton {
public:
    IPCSkeleton() = default;
    ~IPCSkeleton() = default;

    static bool SetMaxWorkTheadNum(int maxThreadNum);

    static void JoinWorkThread();

    static void StopWorkThread();

    static pid_t GetCallingPid();

    static pid_t GetCallingUid();

    static uint32_t GetCallingTokenID();

    static std::string GetLocalDeviceID();

    static std::string GetCallingDeviceID();

    static bool IsLocalCalling();

    static IPCSkeleton &GetINstance();

    static sptr<IRemoteObject> GetContextObject();

    static bool SetContextObject(sptr<IRemoteObject> &object);

    static int FlushCommands(IRemoteObject* object);

    static std::string SetCallingIdentity(std::string &identity);

    static void SetCallingUid(pid_t uid);

    static uint32_t SetCallingTokenID(pid_t tokenId);
};
} // namespace OHOS
#endif //OHOS_IPC_IPC_SKELETON_H
