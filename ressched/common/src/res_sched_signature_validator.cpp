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

#include "res_sched_signature_validator.h"
#include "res_sched_log.h"
#include "bundle_mgr_helper.h"
#include "os_account_manager.h"

namespace OHOS {
namespace ResourceSchedule {

namespace {
constexpr const char *PROP_KEY = "key";
constexpr const char *PROP_VALUE = "value";
constexpr int32_t DEFAULT_USER_ID = 100;
constexpr int32_t MAX_SIGNATURE_CACHE = 200;
}  // namespace

ResSchedSignatureValidator &ResSchedSignatureValidator::GetInstance()
{
    static auto instance = new ResSchedSignatureValidator();
    return *instance;
}

void ResSchedSignatureValidator::OnAppInstallChanged(const std::string &bundleName)
{
    std::lock_guard<ffrt::mutex> autoLock(mutex_);
    bundleNameToUid_.erase(bundleName);
    validCache_.erase(bundleName);
}

SignatureCheckResult ResSchedSignatureValidator::CheckSignatureByBundleName(const std::string &bundleName)
{
    if (bundleName.empty()) {
        RESSCHED_LOGE("%{public}s: bundleName is empty", __func__);
        return SignatureCheckResult::ERR_PARAM_INVALID;
    }
    int32_t uid = -1;
    {
        // try use cache
        std::lock_guard<ffrt::mutex> autoLock(mutex_);
        auto item = bundleNameToUid_.find(bundleName);
        if (item != bundleNameToUid_.end()) {
            uid = item->second;
        }
    }
    if (uid < 0) {
        // no cache, call GetUidByBundleName
        std::vector<int> activatedOsAccountIds;
        GetCurrentUserId(activatedOsAccountIds);
        auto bundleMgr = BundleMgrHelper::GetInstance();
        // activatedOsAccountIds[0] is current active user. If current user no install, try another user.
        for (const auto &userId : activatedOsAccountIds) {
            uid = bundleMgr->GetUidByBundleName(bundleName, userId);
            if (uid >= 0) {
                // write cache
                std::lock_guard<ffrt::mutex> autoLock(mutex_);
                bundleNameToUid_[bundleName] = uid;
                break;
            }
        }
    }
    if (uid < 0) {
        RESSCHED_LOGE("%{public}s: convert bundleName %{public}s to uid error", __func__, bundleName.c_str());
        return SignatureCheckResult::ERR_INTERNAL_ERROR;
    }
    return CheckSignature(uid, bundleName);
}

SignatureCheckResult ResSchedSignatureValidator::CheckSignatureByUid(const int32_t uid)
{
    auto bundleMgr = BundleMgrHelper::GetInstance();
    std::string bundleName = bundleMgr->GetBundleNameByUid(uid);
    if (bundleName.empty()) {
        RESSCHED_LOGE("%{public}s: convert uid %{public}d to name error", __func__, uid);
        return SignatureCheckResult::ERR_INTERNAL_ERROR;
    }
    return CheckSignature(uid, bundleName);
}

SignatureCheckResult ResSchedSignatureValidator::CheckBundleInList(
    const std::vector<std::string> &whiteList, const std::string &bundleName)
{
    if (whiteList.empty() || bundleName.empty()) {
        return SignatureCheckResult::ERR_NOT_IN_LIST;
    }
    if (std::find(whiteList.begin(), whiteList.end(), bundleName) != whiteList.end()) {
        return CheckSignatureByBundleName(bundleName);
    } else {
        return SignatureCheckResult::ERR_NOT_IN_LIST;
    }
}

SignatureCheckResult ResSchedSignatureValidator::CheckSignature(const int32_t uid, const std::string &bundleName)
{
    std::string signature;
    {
        std::lock_guard<ffrt::mutex> autoLock(mutex_);
        auto config = signatureConfig_.find(bundleName);
        if (config == signatureConfig_.end()) {
            RESSCHED_LOGE("%{public}s: check %{public}s but no config, size %{public}u",
                __func__, bundleName.c_str(), static_cast<unsigned int>(signatureConfig_.size()));
            return SignatureCheckResult::ERR_NO_SIGNATURE_CONFIG;
        }
        signature = config->second;
        auto cache = validCache_.find(bundleName);
        if (cache != validCache_.end()) {
            auto cacheUid = cache->second.first;
            auto cacheIsValid = cache->second.second;
            if (cacheUid == uid) {
                RESSCHED_LOGD("%{public}s: %{public}s use cache is valid %{public}d", __func__, bundleName.c_str(),
                    cacheIsValid);
                return cacheIsValid ? SignatureCheckResult::CHECK_OK : SignatureCheckResult::ERR_SIGNATURE_NO_MATCH;
            } else {
                validCache_.erase(bundleName);
            }
        }
    }
    auto bundleMgr = BundleMgrHelper::GetInstance();
    AppExecFwk::SignatureInfo signatureInfo;
    auto ret = bundleMgr->GetSignatureInfoByUid(uid, signatureInfo);
    if (ERR_OK != ret) {
        RESSCHED_LOGE("%{public}s: check %{public}s error %{public}d", __func__, bundleName.c_str(), ret);
        return SignatureCheckResult::ERR_INTERNAL_ERROR;
    }
    bool isValid = (signatureInfo.appIdentifier == signature);
    {
        std::lock_guard<ffrt::mutex> autoLock(mutex_);
        if (validCache_.size() >= MAX_SIGNATURE_CACHE) {
            validCache_.clear();
        }
        validCache_[bundleName] = {uid, isValid};
    }
    if (!isValid) {
        RESSCHED_LOGE("%{public}s: %{public}s illegal signature", __func__, bundleName.c_str());
    }
    RESSCHED_LOGD("%{public}s: %{public}s immediately check is valid %{public}d", __func__, bundleName.c_str(),
        isValid);
    return isValid ? SignatureCheckResult::CHECK_OK : SignatureCheckResult::ERR_SIGNATURE_NO_MATCH;
}

void ResSchedSignatureValidator::GetCurrentUserId(std::vector<int> &activatedOsAccountIds)
{
    auto ret = OHOS::AccountSA::OsAccountManager::QueryActiveOsAccountIds(activatedOsAccountIds);
    if (ret != ERR_OK) {
        RESSCHED_LOGE("%{public}s: query account error %{public}d", __func__, ret);
        activatedOsAccountIds.push_back(DEFAULT_USER_ID);
    }
    if (activatedOsAccountIds.size() == 0) {
        RESSCHED_LOGE("%{public}s: query account empty", __func__);
        activatedOsAccountIds.push_back(DEFAULT_USER_ID);
    }
}

void ResSchedSignatureValidator::SetSignatureConfig(std::unordered_map<std::string, std::string> &config)
{
    std::lock_guard<ffrt::mutex> autoLock(mutex_);
    signatureConfig_ = std::move(config);
    validCache_.clear();
}

void ResSchedSignatureValidator::AddSignatureConfig(std::unordered_map<std::string, std::string> &config)
{
    std::lock_guard<ffrt::mutex> autoLock(mutex_);
    for (const auto &[key, value] : config) {
        signatureConfig_[key] = value;
        validCache_.erase(key);
    }
}

}  // namespace ResourceSchedule
}  // namespace OHOS