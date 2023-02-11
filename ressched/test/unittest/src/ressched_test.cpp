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
#include <cstdint>
#include <cstring>
#include <iostream>
#include <unordered_map>
#include "nativetoken_kit.h"
#include "res_sched_client.h"
#include "token_setproc.h"

const static int32_t PARAMETERS_NUM_MIN                      = 2;
const static int32_t PARAMETERS_NUM_MIN_KILL_PROCESS         = 4;
const static int32_t PARAMETERS_NUM_KILL_PROCESS_PROCESSNAME = 5;
const static int32_t PARAMETERS_STABLE_TIMES                 = 100;

static void MockProcess(std::string processName)
{
    static const char *PERMS[] = {
        "ohos.permission.DISTRIBUTED_DATASYNC"
    };
    uint64_t tokenId;
    NativeTokenInfoParams infoInstance = {
        .dcapsNum = 0,
        .permsNum = 1,
        .aclsNum = 0,
        .dcaps = nullptr,
        .perms = PERMS,
        .acls = nullptr,
        .processName = processName.c_str(),
        .aplStr = "system_core",
    };
    tokenId = GetAccessTokenId(&infoInstance);
    SetSelfTokenID(tokenId);
}

static void KillProcess(int32_t argc, char *argv[])
{
    if (argc >= PARAMETERS_NUM_MIN_KILL_PROCESS) {
        std::string caller = argv[PARAMETERS_NUM_MIN];
        MockProcess(caller);
        std::unordered_map<std::string, std::string> mapPayload;
        mapPayload["pid"] = argv[PARAMETERS_NUM_MIN_KILL_PROCESS - 1];
        if (argc >= PARAMETERS_NUM_KILL_PROCESS_PROCESSNAME) {
            mapPayload["processName"] = argv[PARAMETERS_NUM_KILL_PROCESS_PROCESSNAME - 1];
        }
        OHOS::ResourceSchedule::ResSchedClient::GetInstance().KillProcess(mapPayload);
    }
}

int32_t main(int32_t argc, char *argv[])
{
    if (argc >= PARAMETERS_NUM_MIN && argv) {
        char* function = argv[1];
        if (strcmp(function, "KillProcess") == 0) {
            KillProcess(argc, argv);
        }
    }
    return 0;
}