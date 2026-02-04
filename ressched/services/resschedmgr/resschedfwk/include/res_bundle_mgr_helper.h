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

#ifndef RESSCHED_BUNDLE_MGR_HELPER_H
#define RESSCHED_BUNDLE_MGR_HELPER_H

#include "bundle_mgr_interface.h"
#include "bundle_mgr_proxy.h"
#include "ipc_skeleton.h"
#include "iremote_object.h"
#include "remote_death_recipient.h"
#include "singleton.h"
#include "bundle_mgr_helper.h"
/*
#ifdef RES_SCHED_UNIT_TEST
#define WEAK_FUNC __attribute__((weak))
#else
#define WEAK_FUNC
#endif
*/

namespace OHOS {
namespace ResourceSchedule {

namespace {
    constexpr int32_t INVALID_UID = -1;
}
class ResBundleMgrHelper : public BundleMgrHelper {
public:
    std::string GetBundleNameByUid(const int32_t uid) override;
    int32_t GetUidByBundleName(const std::string &bundleName, const int32_t userId) override;
    ErrCode GetSignatureInfoByUid(const int32_t uid, std::string &signatureInfo) override;
    void GetCurrentUserId(std::vector<int> &activatedOsAccountIds) override;
    ResBundleMgrHelper();
    ~ResBundleMgrHelper();
private:
    bool Connect();
    void Disconnect();
    void OnRemoteDied(const wptr<IRemoteObject> &object);

private:
    sptr<AppExecFwk::IBundleMgr> bundleMgr_ = nullptr;
    std::mutex connectionMutex_;
    sptr<RemoteDeathRecipient> bundleMgrDeathRecipient_ = nullptr;
};
}  // namespace ResourceSchedule
}  // namespace OHOS
#endif  // RESSCHED_BUNDLE_MGR_HELPER_H