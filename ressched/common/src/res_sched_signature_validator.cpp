/*
 * Copyright (c) 2025-2026 Huawei Device Co., Ltd.
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
#include "hisysevent.h"
#include <optional>

namespace OHOS {
namespace ResourceSchedule {

namespace {
constexpr const char *PROP_KEY = "key";
constexpr const char *PROP_VALUE = "value";
constexpr int32_t MAX_SIGNATURE_CACHE = 200;
}  // namespace

void ResSchedSignatureValidator::InitSignatureDependencyInterface(
    std::shared_ptr<BundleMgrHelper> bundleMgr)
{
    bundleMgr_ = bundleMgr;
}

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
    if (bundleMgr_ == nullptr) {
        ReportCheckError(bundleName, "ERR_NOT_SUPPORT");
        return SignatureCheckResult::ERR_NOT_SUPPORT;
    }
    if (bundleName.empty()) {
        RESSCHED_LOGE("%{public}s: bundleName is empty", __func__);
        ReportCheckError(bundleName, "ERR_PARAM_INVALID");
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
        // activatedOsAccountIds[0] is current active user. If current user no install, try another user.
        for (const auto &userId : activatedOsAccountIds) {
            uid = bundleMgr_->GetUidByBundleName(bundleName, userId);
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
        ReportCheckError(bundleName, "ERR_INTERNAL_ERROR/ConvertUid");
        return SignatureCheckResult::ERR_INTERNAL_ERROR;
    }
    return CheckSignature(uid, bundleName);
}

SignatureCheckResult ResSchedSignatureValidator::CheckSignatureByUid(const int32_t uid)
{
    if (bundleMgr_ == nullptr) {
        ReportCheckError("uid/" + std::to_string(uid), "ERR_NOT_SUPPORT");
        return SignatureCheckResult::ERR_NOT_SUPPORT;
    }
    std::string bundleName = bundleMgr_->GetBundleNameByUid(uid);
    if (bundleName.empty()) {
        RESSCHED_LOGE("%{public}s: convert uid %{public}d to name error", __func__, uid);
        ReportCheckError("uid/" + std::to_string(uid), "ERR_INTERNAL_ERROR/ConvertBundleName");
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

bool ResSchedSignatureValidator::GetSignatureFromConfig(const std::string &bundleName, std::string &signature)
{
    std::lock_guard<ffrt::mutex> autoLock(mutex_);
    auto config = signatureConfig_.find(bundleName);
    if (config == signatureConfig_.end()) {
        RESSCHED_LOGE("%{public}s: check %{public}s but no config, size %{public}u",
            __func__, bundleName.c_str(), static_cast<unsigned int>(signatureConfig_.size()));
        ReportCheckError(bundleName, "ERR_NO_SIGNATURE_CONFIG");
        return false;
    }
    signature = config->second;
    return true;
}

std::optional<bool> ResSchedSignatureValidator::CheckSignatureCache(const std::string &bundleName, int32_t uid)
{
    std::lock_guard<ffrt::mutex> autoLock(mutex_);
    auto cache = validCache_.find(bundleName);
    if (cache == validCache_.end()) {
        return std::nullopt;
    }
    auto cacheUid = cache->second.first;
    auto cacheIsValid = cache->second.second;
    if (cacheUid == uid) {
        RESSCHED_LOGD("%{public}s: %{public}s use cache is valid %{public}d", __func__, bundleName.c_str(),
            cacheIsValid);
        return cacheIsValid;
    } else {
        validCache_.erase(bundleName);
        return std::nullopt;
    }
}

bool ResSchedSignatureValidator::GetSignatureInfo(int32_t uid, std::string &signatureInfo)
{
    auto ret = bundleMgr_->GetSignatureInfoByUid(uid, signatureInfo);
    if (ERR_OK != ret) {
        RESSCHED_LOGE("%{public}s: check uid %{public}d error %{public}d", __func__, uid, ret);
        return false;
    }
    return true;
}

void ResSchedSignatureValidator::UpdateSignatureCache(const std::string &bundleName, int32_t uid, bool isValid)
{
    std::lock_guard<ffrt::mutex> autoLock(mutex_);
    if (validCache_.size() >= MAX_SIGNATURE_CACHE) {
        validCache_.clear();
    }
    validCache_[bundleName] = {uid, isValid};
}

SignatureCheckResult ResSchedSignatureValidator::CheckSignature(const int32_t uid, const std::string &bundleName)
{
    if (bundleMgr_ == nullptr) {
        ReportCheckError(bundleName, "ERR_NOT_SUPPORT");
        return SignatureCheckResult::ERR_NOT_SUPPORT;
    }

    std::string signature;
    if (!GetSignatureFromConfig(bundleName, signature)) {
        return SignatureCheckResult::ERR_NO_SIGNATURE_CONFIG;
    }

    auto cachedResult = CheckSignatureCache(bundleName, uid);
    if (cachedResult.has_value()) {
        return cachedResult.value() ? SignatureCheckResult::CHECK_OK : SignatureCheckResult::ERR_SIGNATURE_NO_MATCH;
    }

    std::string signatureInfo;
    if (!GetSignatureInfo(uid, signatureInfo)) {
        ReportCheckError(bundleName, "ERR_INTERNAL_ERROR/GetSignatureError");
        return SignatureCheckResult::ERR_INTERNAL_ERROR;
    }

    bool isValid = (signatureInfo == signature);
    UpdateSignatureCache(bundleName, uid, isValid);

    if (!isValid) {
        ReportCheckError(bundleName, "ERR_SIGNATURE_NO_MATCH");
        RESSCHED_LOGE("%{public}s: %{public}s illegal signature", __func__, bundleName.c_str());
    }
    RESSCHED_LOGD("%{public}s: %{public}s immediately check is valid %{public}d", __func__, bundleName.c_str(),
        isValid);
    return isValid ? SignatureCheckResult::CHECK_OK : SignatureCheckResult::ERR_SIGNATURE_NO_MATCH;
}

void ResSchedSignatureValidator::GetCurrentUserId(std::vector<int> &activatedOsAccountIds)
{
    if (bundleMgr_ == nullptr) {
        return;
    }
    bundleMgr_->GetCurrentUserId(activatedOsAccountIds);
}

void ResSchedSignatureValidator::AddSignatureConfig(std::unordered_map<std::string, std::string> &config)
{
    std::lock_guard<ffrt::mutex> autoLock(mutex_);
    for (const auto &[key, value] : config) {
        signatureConfig_[key] = value;
        validCache_.erase(key);
    }
}

void ResSchedSignatureValidator::ReportCheckError(const std::string& bundleName, const std::string& reason)
{
    HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::RSS, "ABNORMAL_ERR", HiviewDFX::HiSysEvent::EventType::STATISTIC,
        "MODULE_NAME", "Common",
        "FUNC_NAME", "ResSchedSignatureValidator::ReportCheckError",
        "ERR_INFO", "bundle:" + bundleName + "|reason:" + reason);
}
}  // namespace ResourceSchedule
}  // namespace OHOS