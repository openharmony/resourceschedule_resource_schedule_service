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

#include "res_json_type.h"

#include "res_sched_log.h"

namespace OHOS {
namespace ResourceSchedule {
bool ResJsonType::ReadFromParcel(Parcel& parcel)
{
    std::string jsonStr = parcel.ReadString();
    return ResCommonUtil::LoadContentToJsonObj(jsonStr, jsonContext);
}

bool ResJsonType::Marshalling(Parcel& parcel) const
{
    std::string jsonStr = "";
    ResCommonUtil::DumpJsonToString(jsonContext, jsonStr);
    if (jsonStr.empty()) {
        RESSCHED_LOGE("ResJsonType Marshalling failed, context string is empty");
        return false;
    }

    if (!parcel.WriteString(jsonStr)) {
        RESSCHED_LOGE("ResJsonType Marshalling failed, content: %{public}s", jsonStr.c_str());
        return false;
    }
    return true;
}

ResJsonType *ResJsonType::Unmarshalling(Parcel& parcel)
{
    ResJsonType *info = new (std::nothrow) ResJsonType();

    if (info == nullptr) {
        RESSCHED_LOGE("ResJsonType Unmarshalling failed as create null ResJsonType");
        return nullptr;
    }

    if (!info->ReadFromParcel(parcel)) {
        RESSCHED_LOGE("ResJsonType Unmarshalling failed, Read from parcel faield");
        delete info;
        return nullptr;
    }
    return info;
}
} // namespace ResourceSchedule
} // namespace OHOS