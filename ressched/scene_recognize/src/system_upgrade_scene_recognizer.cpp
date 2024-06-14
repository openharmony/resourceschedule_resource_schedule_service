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

#include <file_ex.h>

#include "system_upgrade_scene_recognizer.h"
#include "ffrt_inner.h"
#include "parameters.h"
#include "plugin_mgr.h"
#include "res_sched_log.h"
#include "res_sched_mgr.h"
#include "res_type.h"


namespace OHOS {
namespace ResourceSchedule {
namespace {
    static const std::string OLD_SYSTEM_FINGERPRINT_PATH = "/data/service/el1/public/ressched/device_version";
    static const std::string SEPARATOR = "|";
    const std::vector<std::string> FINGERPRINTS = {
        "const.product.software.version",
        "const.product.build.type",
        "const.product.brand",
        "const.product.name",
        "const.product.devicetype",
        "const.product.incremental.version",
        "const.comp.hl.product_base_version.real"
    };
}

SystemUpgradeSceneRecognizer::~SystemUpgradeSceneRecognizer()
{
    RESSCHED_LOGI("~UpdatingSceneRecognizer");
}

SystemUpgradeSceneRecognizer::SystemUpgradeSceneRecognizer()
{
    Init();
}

void SystemUpgradeSceneRecognizer::Init()
{
    std::string oldSystemFingerprint;
    OHOS::LoadStringFromFile(OLD_SYSTEM_FINGERPRINT_PATH, oldSystemFingerprint);
    std::string curSystemFingerprint = GetCurSystemFingerprint();
    isSystemUpgraded_ = oldSystemFingerprint.empty() || (oldSystemFingerprint != curSystemFingerprint);
    if (isSystemUpgraded_) {
        OHOS::SaveStringToFile(OLD_SYSTEM_FINGERPRINT_PATH, curSystemFingerprint, true);
    }
}

std::string SystemUpgradeSceneRecognizer::GetCurSystemFingerprint()
{
    std::string curSystemFingerprint;
    for (const auto &item : FINGERPRINTS) {
        std::string itemFingerprint = OHOS::system::GetParameter(item, "");
        if (itemFingerprint.empty()) {
            continue;
        }
        if (!curSystemFingerprint.empty()) {
            curSystemFingerprint.append(SEPARATOR);
        }
        curSystemFingerprint.append(itemFingerprint);
    }
    return curSystemFingerprint;
}
 
void SystemUpgradeSceneRecognizer::OnDispatchResource(uint32_t resType, int64_t value, const nlohmann::json& payload)
{
    if (resType != ResType::RES_TYPE_BOOT_COMPLETED) {
        return;
    }
    if (isSystemUpgraded_) {
        RESSCHED_LOGI("enter updating scene");
        nlohmann::json payload;
        ResSchedMgr::GetInstance().ReportData(ResType::RES_TYPE_FIRST_BOOT_AFTER_SYSTEM_UPGRADE, 0, payload);
    }
}
} // namespace ResourceSchedule
} // namespace OHOS