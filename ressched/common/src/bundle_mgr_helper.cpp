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

#include "res_sched_log.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"
#include "bundle_mgr_helper.h"

namespace OHOS {
namespace ResourceSchedule {

namespace {
constexpr int32_t BUNDLE_MGR_SERVICE_SYS_ABILITY_ID = 401;
constexpr int32_t ERR_CONNECT_BMS_FAILED = 100;
}  // namespace

BundleMgrHelper::BundleMgrHelper()
{
    bundleMgrDeathRecipient_ = new (std::nothrow)
        RemoteDeathRecipient([this](const wptr<IRemoteObject> &object) { this->OnRemoteDied(object); });
}

BundleMgrHelper::~BundleMgrHelper()
{
    std::lock_guard<std::mutex> lock(connectionMutex_);
    Disconnect();
}

std::string WEAK_FUNC BundleMgrHelper::GetBundleNameByUid(int32_t uid)
{
    std::string bundle{""};
    std::lock_guard<std::mutex> lock(connectionMutex_);
    if (!Connect()) {
        return bundle;
    }
    if (bundleMgr_ != nullptr) {
        int32_t ret = bundleMgr_->GetNameForUid(uid, bundle);
        if (ret != ERR_OK) {
            RESSCHED_LOGE("uid:%{public}d, fail:%{public}d", uid, ret);
        }
    }
    return bundle;
}

std::int32_t WEAK_FUNC BundleMgrHelper::GetUidByBundleName(const std::string &bundleName, const int32_t userId)
{
    std::lock_guard<std::mutex> lock(connectionMutex_);
    if (!Connect()) {
        return INVALID_UID;
    }
    int32_t uid = INVALID_UID;
    if (bundleMgr_ != nullptr) {
        uid = bundleMgr_->GetUidByBundleName(bundleName, userId);
        if (uid == INVALID_UID) {
            RESSCHED_LOGE("%{public}s: bundleName %{public}s to uid fail", __func__, bundleName.c_str());
        }
    }
    return uid;
}

ErrCode WEAK_FUNC BundleMgrHelper::GetSignatureInfoByUid(const int32_t uid, AppExecFwk::SignatureInfo &signatureInfo)
{
    std::lock_guard<std::mutex> lock(connectionMutex_);
    if (!Connect()) {
        return ERR_CONNECT_BMS_FAILED;
    }
    return bundleMgr_->GetSignatureInfoByUid(uid, signatureInfo);
}

bool BundleMgrHelper::Connect()
{
    if (bundleMgr_ != nullptr) {
        return true;
    }
    sptr<ISystemAbilityManager> systemAbilityManager =
        SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (systemAbilityManager == nullptr) {
        RESSCHED_LOGE("get SystemAbilityManager failed");
        return false;
    }

    sptr<IRemoteObject> remoteObject = systemAbilityManager->GetSystemAbility(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
    if (remoteObject == nullptr) {
        RESSCHED_LOGE("get Bundle Manager failed");
        return false;
    }

    bundleMgr_ = iface_cast<AppExecFwk::IBundleMgr>(remoteObject);
    if (bundleMgr_ != nullptr && bundleMgr_->AsObject() != nullptr && bundleMgrDeathRecipient_ != nullptr) {
        bundleMgr_->AsObject()->AddDeathRecipient(bundleMgrDeathRecipient_);
        return true;
    }
    RESSCHED_LOGE("get bundleMgr failed");
    return false;
}

void BundleMgrHelper::Disconnect()
{
    if (bundleMgr_ != nullptr && bundleMgr_->AsObject() != nullptr) {
        bundleMgr_->AsObject()->RemoveDeathRecipient(bundleMgrDeathRecipient_);
        bundleMgr_ = nullptr;
    }
}

void BundleMgrHelper::OnRemoteDied(const wptr<IRemoteObject> &object)
{
    std::lock_guard<std::mutex> lock(connectionMutex_);
    Disconnect();
}
}  // namespace ResourceSchedule
}  // namespace OHOS
