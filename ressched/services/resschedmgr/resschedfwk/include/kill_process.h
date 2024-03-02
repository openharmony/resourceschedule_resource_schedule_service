/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
#ifndef RESSCHED_SERVICES_RESSCHEDMGR_RESSCHEDFWK_INCLUDE_KILL_PROCESS_H
#define RESSCHED_SERVICES_RESSCHEDMGR_RESSCHEDFWK_INCLUDE_KILL_PROCESS_H

#include <string>
#include <vector>
#include "nlohmann/json.hpp"

namespace OHOS {
namespace ResourceSchedule {
class KillProcess {
public:
    int32_t KillProcessByPidWithClient(const nlohmann::json& payload);
private:
    time_t GetProcessStartTime(int pid);
    std::vector<std::string> GetStatInfo(const std::string &path);
    std::string ReadFileByChar(const std::string &path);
};
} // namespace ResourceSchedule
} // namespace OHOS

#endif // RESSCHED_SERVICES_RESSCHEDMGR_RESSCHEDFWK_INCLUDE_KILL_PROCESS_H