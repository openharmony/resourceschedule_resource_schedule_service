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

#include "res_sched_hisysevent_report_util.h"
#include "hisysevent_c.h"

namespace OHOS {
namespace ResourceSchedule {
namespace ResCommonUtil {
void HiSysAbnormalErrReport(const std::string& moduleName, const std::string& funcName, const std::string errInfo)
{
    struct HiSysEventParam params[] = {
        {
            .name = "MODULE_NAME",
            .t = HISYSEVENT_STRING,
            .V = { .s = const_cast<char *>(moduleName) },
            .arraySize = 0
        },
        {
            .name = "FUNC_NAME",
            .t = HISYSEVENT_STRING,
            .V = { .s = const_cast<char *>(funcName) },
            .arraySize = 0
        },
        {
            .name = "ERR_INFO",
            .t = HISYSEVENT_STRING,
            .V = { .s = const_cast<char *>(errInfo) },
            .arraySize = 0
        },
    };

    OH_HiSysEvent_Write(
        "RSS",
        "ABNORMAL_ERR",
        HISYSEVENT_STATISTIC,
        params,
        sizeof(params)/sizeof(params[0])
    );
}
}
} // namespace ResourceSchedule
} // namespace OHOS
