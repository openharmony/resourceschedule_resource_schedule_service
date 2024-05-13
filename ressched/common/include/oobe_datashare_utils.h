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

#ifndef RESOURCE_SCHEDULE_SERVICE_RESSCHED_COMMON_OOBE_DATASHARE_UTILS_H
#define RESOURCE_SCHEDULE_SERVICE_RESSCHED_COMMON_OOBE_DATASHARE_UTILS_H

#include "datashare_helper.h"
#include "errors.h"
#include "ipc_skeleton.h"
#include "res_sched_log.h"
#include <map>
#include "mutex"

namespace OHOS {
namespace ResourceSchedule {
class DataShareUtils {
public:
    ~DataShareUtils ();
    static DataShareUtils& GetInstance();
    template <typename T>
    ErrCode GetValue(const std::string& key, T& value);
    Uri AssembleUri(const std::string& key);
    bool ReleaseDataShareHelper(std::shared_ptr<DataShare::DataShareHelper>& helper);
    std::shared_ptr<DataShare::DataShareHelper> CreateDataShareHelper();

private:
    DataShareUtils ();
    static constexpr int32_t PARAM_NUM_TEN = 10;
    static sptr<IRemoteObject> remoteObj_;
    static std::mutex mutex_;
    ErrCode GetStringValue(const std::string& key, std::string& value);
    void InitSystemAbilityManager();
};

template <typename T>
ErrCode DataShareUtils::GetValue(const std::string& key, T& value)
{
    std::string result;
    std::string callingIdentity = IPCSkeleton::ResetCallingIdentity();
    int32_t ret = GetStringValue(key, result);
    IPCSkeleton::SetCallingIdentity(callingIdentity);
    if (ret != ERR_OK) {
        RESSCHED_LOGW("resultSet->GetStringValue return not ok, ret=%{public}d", ret);
        return ret;
    }
    using ValueType = std::remove_cv_t<std::remove_reference_t<T>>;
    if constexpr (std::is_same_v<std::string, ValueType>) {
        value = result;
    } else if constexpr (std::is_same_v<int64_t, ValueType>) {
        value = static_cast<int64_t>(strtoll(result.c_str(), nullptr, PARAM_NUM_TEN));
    } else if constexpr (std::is_same_v<int32_t, ValueType>) {
        value = static_cast<int32_t>(strtoll(result.c_str(), nullptr, PARAM_NUM_TEN));
    } else {
        RESSCHED_LOGE("GetValue: invalid operation!");
        return ERR_INVALID_OPERATION;
    }
    return ERR_OK;
}
} // namespace ResourceSchedule
} // namespace OHOS
#endif // RESOURCE_SCHEDULE_SERVICE_RESSCHED_COMMON_OOBE_DATASHARE_UTILS_H
