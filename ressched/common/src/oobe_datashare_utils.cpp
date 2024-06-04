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

#include "datashare_errno.h"
#include "datashare_predicates.h"
#include "datashare_result_set.h"
#include "datashare_values_bucket.h"
#include "iservice_registry.h"
#include "oobe_datashare_utils.h"
#include "res_sched_log.h"
#include "system_ability_definition.h"
#include "uri.h"

namespace OHOS {
namespace ResourceSchedule {
sptr<IRemoteObject> DataShareUtils::remoteObj_;
std::mutex DataShareUtils::mutex_;
namespace {
const std::string SETTING_COLUMN_KEYWORD = "KEYWORD";
const std::string SETTING_COLUMN_VALUE = "VALUE";
const std::string SETTING_URI_PROXY = "datashare:///com.ohos.settingsdata/entry/settingsdata/SETTINGSDATA?Proxy=true";
} // namespace

DataShareUtils::DataShareUtils() = default;
DataShareUtils::~DataShareUtils() = default;

DataShareUtils& DataShareUtils::GetInstance()
{
    static DataShareUtils instance;
    instance.InitSystemAbilityManager();
    return instance;
}

ErrCode DataShareUtils::GetStringValue(const std::string& key, std::string& value)
{
    auto helper = CreateDataShareHelper();
    if (helper == nullptr) {
        RESSCHED_LOGE("DataShareUtils: helper does not created!");
        return ERR_NO_INIT;
    }
    std::vector<std::string> columns = {SETTING_COLUMN_VALUE};
    DataShare::DataSharePredicates predicates;
    predicates.EqualTo(SETTING_COLUMN_KEYWORD, key);
    RESSCHED_LOGD("keyWord=%{public}s", key.c_str());
    Uri uri(AssembleUri(key));
    auto resultSet = helper->Query(uri, predicates, columns);
    ReleaseDataShareHelper(helper);
    if (resultSet == nullptr) {
        RESSCHED_LOGE("helper->Query return nullptr");
        return ERR_INVALID_OPERATION;
    }
    int32_t count;
    resultSet->GetRowCount(count);
    if (count == 0) {
        RESSCHED_LOGW("not found value, keyWord=%{public}s, count=%{public}d", key.c_str(), count);
        return ERR_NAME_NOT_FOUND;
    }
    const int32_t INDEX = 0;
    resultSet->GoToRow(INDEX);
    int32_t ret = resultSet->GetString(INDEX, value);
    if (ret != DataShare::E_OK) {
        RESSCHED_LOGW("resultSet->GetString return not ok, ret=%{public}d", ret);
        return ERR_INVALID_VALUE;
    }
    return ERR_OK;
}

std::shared_ptr<DataShare::DataShareHelper> DataShareUtils::CreateDataShareHelper()
{
    if (remoteObj_ == nullptr) {
        RESSCHED_LOGE("Get remoteObj return nullptr!");
        return nullptr;
    }
    auto helper = DataShare::DataShareHelper::Creator(remoteObj_, SETTING_URI_PROXY);
    if (helper == nullptr) {
        RESSCHED_LOGW("helper is nullptr, uri=%{public}s, remoteObj_=%{public}p", SETTING_URI_PROXY.c_str(),
            remoteObj_.GetRefPtr());
        return nullptr;
    }
    return helper;
}

bool DataShareUtils::ReleaseDataShareHelper(std::shared_ptr<DataShare::DataShareHelper>& helper)
{
    if (helper == nullptr) {
        RESSCHED_LOGW("release helper fail, helper is null!");
        return false;
    }
    if (!helper->Release()) {
        RESSCHED_LOGW("release helper fail, remoteObj_%{public}p", remoteObj_.GetRefPtr());
        return false;
    }
    return true;
}

void DataShareUtils::InitSystemAbilityManager()
{
    auto sam = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (sam == nullptr) {
        RESSCHED_LOGE("get sam return nullptr");
        return;
    }
    auto remoteObj = sam->GetSystemAbility(RES_SCHED_SYS_ABILITY_ID);
    if (remoteObj == nullptr) {
        RESSCHED_LOGE("Get remoteObj return nullptr, systemAbilityId=%{public}d", RES_SCHED_SYS_ABILITY_ID);
        return;
    }
    remoteObj_ = remoteObj;
}

Uri DataShareUtils::AssembleUri(const std::string& key)
{
    Uri uri(SETTING_URI_PROXY + "&key=" + key);
    return uri;
}
} // namespace ResourceSchedule
} // namespace OHOS