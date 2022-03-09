/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "socperf_client.h"

const int PARAMETERS_NUM_MIN        = 2;
const int PARAMETERS_NUM_WITHOUT_EX = 3;
const int PARAMETERS_NUM_WITH_EX    = 4;

static void PerfRequest(int argc, char *argv[])
{
    if (argc == PARAMETERS_NUM_WITHOUT_EX) {
        char* cmdId = argv[2];
        if (cmdId) {
            OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequest(atoi(cmdId), "");
        }
    }
}

static void PerfRequestEx(int argc, char *argv[])
{
    if (argc == PARAMETERS_NUM_WITH_EX) {
        char* cmdId = argv[2];
        char* onOffTag = argv[3];
        if (cmdId && onOffTag) {
            if (strcmp(onOffTag, "true") == 0) {
                OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(atoi(cmdId), true, "");
            } else if (strcmp(onOffTag, "false") == 0) {
                OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(atoi(cmdId), false, "");
            }
        }
    }
}

static void PowerRequest(int argc, char *argv[])
{
    if (argc == PARAMETERS_NUM_WITHOUT_EX) {
        char* cmdId = argv[2];
        if (cmdId) {
            OHOS::SOCPERF::SocPerfClient::GetInstance().PowerRequest(atoi(cmdId), "");
        }
    }
}

static void PowerRequestEx(int argc, char *argv[])
{
    if (argc == PARAMETERS_NUM_WITH_EX) {
        char* cmdId = argv[2];
        char* onOffTag = argv[3];
        if (cmdId && onOffTag) {
            if (strcmp(onOffTag, "true") == 0) {
                OHOS::SOCPERF::SocPerfClient::GetInstance().PowerRequestEx(atoi(cmdId), true, "");
            } else if (strcmp(onOffTag, "false") == 0) {
                OHOS::SOCPERF::SocPerfClient::GetInstance().PowerRequestEx(atoi(cmdId), false, "");
            }
        }
    }
}

static void PowerLimitBoost(int argc, char *argv[])
{
    if (argc == PARAMETERS_NUM_WITHOUT_EX) {
        char* onOffTag = argv[2];
        if (onOffTag) {
            if (strcmp(onOffTag, "true") == 0) {
                OHOS::SOCPERF::SocPerfClient::GetInstance().PowerLimitBoost(true, "");
            } else if (strcmp(onOffTag, "false") == 0) {
                OHOS::SOCPERF::SocPerfClient::GetInstance().PowerLimitBoost(false, "");
            }
        }
    }
}

static void ThermalRequest(int argc, char *argv[])
{
    if (argc == PARAMETERS_NUM_WITHOUT_EX) {
        char* cmdId = argv[2];
        if (cmdId) {
            OHOS::SOCPERF::SocPerfClient::GetInstance().ThermalRequest(atoi(cmdId), "");
        }
    }
}

static void ThermalRequestEx(int argc, char *argv[])
{
    if (argc == PARAMETERS_NUM_WITH_EX) {
        char* cmdId = argv[2];
        char* onOffTag = argv[3];
        if (cmdId && onOffTag) {
            if (strcmp(onOffTag, "true") == 0) {
                OHOS::SOCPERF::SocPerfClient::GetInstance().ThermalRequestEx(atoi(cmdId), true, "");
            } else if (strcmp(onOffTag, "false") == 0) {
                OHOS::SOCPERF::SocPerfClient::GetInstance().ThermalRequestEx(atoi(cmdId), false, "");
            }
        }
    }
}

static void ThermalLimitBoost(int argc, char *argv[])
{
    if (argc == PARAMETERS_NUM_WITHOUT_EX) {
        char* onOffTag = argv[2];
        if (onOffTag) {
            if (strcmp(onOffTag, "true") == 0) {
                OHOS::SOCPERF::SocPerfClient::GetInstance().ThermalLimitBoost(true, "");
            } else if (strcmp(onOffTag, "false") == 0) {
                OHOS::SOCPERF::SocPerfClient::GetInstance().ThermalLimitBoost(false, "");
            }
        }
    }
}

int main(int argc, char *argv[])
{
    if (argc >= PARAMETERS_NUM_MIN && argv) {
        char* function = argv[1];
        if (strcmp(function, "PerfRequest") == 0) {
            PerfRequest(argc, argv);
        } else if (strcmp(function, "PerfRequestEx") == 0) {
            PerfRequestEx(argc, argv);
        } else if (strcmp(function, "PowerRequest") == 0) {
            PowerRequest(argc, argv);
        } else if (strcmp(function, "PowerRequestEx") == 0) {
            PowerRequestEx(argc, argv);
        } else if (strcmp(function, "PowerLimitBoost") == 0) {
            PowerLimitBoost(argc, argv);
        } else if (strcmp(function, "ThermalRequest") == 0) {
            ThermalRequest(argc, argv);
        } else if (strcmp(function, "ThermalRequestEx") == 0) {
            ThermalRequestEx(argc, argv);
        } else if (strcmp(function, "ThermalLimitBoost") == 0) {
            ThermalLimitBoost(argc, argv);
        }
    }
    return 0;
}