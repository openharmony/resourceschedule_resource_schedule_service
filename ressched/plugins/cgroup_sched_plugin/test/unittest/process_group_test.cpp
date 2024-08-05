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

#include <iostream>
#include <ctime>
#include <cstring>
#include "sched_policy.h"

using namespace OHOS::ResourceSchedule::CgroupSetting;
constexpr int ARGUMENT_NUMBER_TWO     = 2;
constexpr int ARGUMENT_NUMBER_THREE   = 3;

static bool CovertSchedPolicyStringToEnumType(char* policyStr, SchedPolicy* policy)
{
    for (int i = SP_DEFAULT; i < SP_CNT; i++) {
        SchedPolicy policyItem = static_cast<SchedPolicy>(i);
        const char* stringItem = GetSchedPolicyFullName(policyItem);
        if (!strcmp(stringItem, "error")) {
            continue;
        }
        if (!strcmp(policyStr, stringItem)) {
            *policy = policyItem;
            return true;
        }
    }
    return false;
}

static void SetThreadSchedPolicyTest(int argc, char *argv[])
{
    if (argc == ARGUMENT_NUMBER_THREE) {
        int tid = std::stoi(argv[1]);
        char* policyStr = argv[2];
        SchedPolicy policy;
        if (!CovertSchedPolicyStringToEnumType(policyStr, &policy)) {
            return;
        }
        if (SetThreadSchedPolicy(tid, policy) == 0) {
            std::cout << "Set thread tid = << "<< tid << " SolicyPolicy = "<<  policy << "ok." << std::endl;
        }
    }
}

static void SetThreadGroupSchedPolicyTest(int argc, char *argv[])
{
    if (argc == ARGUMENT_NUMBER_THREE) {
        int pid = std::stoi(argv[1]);
        char* policyStr = argv[2];
        SchedPolicy policy;
        if (!CovertSchedPolicyStringToEnumType(policyStr, &policy)) {
            return;
        }
        if (SetThreadGroupSchedPolicy(pid, policy) == 0) {
            std::cout << "Set thread group pid = << "<< pid << " SolicyPolicy = "<<  policy << "ok." << std::endl;
        }
    }
}

static void GetThreadSchedPolicyTest(int argc, char *argv[])
{
    if (argc == ARGUMENT_NUMBER_TWO) {
        int tid = std::stoi(argv[1]);
        SchedPolicy policy;
        if (GetThreadSchedPolicy(tid, &policy) == 0) {
            std::cout << "Get the SolicyPolicy of thread tid = " << tid << " is " <<  policy << "\n";
        }
    }
}

static void GetSchedPolicyShortenedNameTest(int argc, char *argv[])
{
    if (argc == ARGUMENT_NUMBER_TWO) {
        int policyNum = std::stoi(argv[1]);
        if (policyNum < SP_DEFAULT || policyNum > SP_MAX) {
            return;
        }
        SchedPolicy policy = static_cast<SchedPolicy>(policyNum);
        const char* policyStr = GetSchedPolicyShortenedName(policy);
        std::cout << "Get SchedPolicy shortened name of policy num = " << policyNum << " is:" << policyStr << std::endl;
    }
}

static void GetSchedPolicyFullNameTest(int argc, char *argv[])
{
    if (argc == ARGUMENT_NUMBER_TWO) {
        int policyNum = std::stoi(argv[1]);
        if (policyNum < SP_DEFAULT || policyNum > SP_MAX) {
            return;
        }
        SchedPolicy policy = static_cast<SchedPolicy>(policyNum);
        const char* policyStr = GetSchedPolicyShortenedName(policy);
        std::cout << "Get SchedPolicy full name of policy num = " << policyNum << " is:" << policyStr << std::endl;
    }
}

int main(int argc, char *argv[])
{
    if (argc > ARGUMENT_NUMBER_TWO) {
        char* operation = argv[1];
        if (!strcmp(operation, "SetThreadSchedPolicyTest")) {
            SetThreadSchedPolicyTest(argc, argv);
        } else if (!strcmp(operation, "SetThreadGroupSchedPolicyTest")) {
            SetThreadGroupSchedPolicyTest(argc, argv);
        } else if (!strcmp(operation, "GetThreadSchedPolicyTest")) {
            GetThreadSchedPolicyTest(argc, argv);
        } else if (!strcmp(operation, "GetSchedPolicyShortenedNameTest")) {
            GetSchedPolicyShortenedNameTest(argc, argv);
        } else if (!strcmp(operation, "GetSchedPolicyFullNameTest")) {
            GetSchedPolicyFullNameTest(argc, argv);
        }
    }
    return 0;
}
