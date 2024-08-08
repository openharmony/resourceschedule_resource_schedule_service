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

#ifndef CGROUP_SCHED_FRAMEWORK_PROCESS_GROUP_INCLUDE_PROCESS_GROUP_MACRO_H_
#define CGROUP_SCHED_FRAMEWORK_PROCESS_GROUP_INCLUDE_PROCESS_GROUP_MACRO_H_

// Mac Os' libc doesn't have this macro
#ifndef TEMP_FAILURE_RETRY
#define TEMP_FAILURE_RETRY(exp)                          \
    (                                                    \
        {                                                \
            decltype(exp) __result;                      \
            do {                                         \
                __result = (exp);                        \
            } while (__result == -1 && errno == EINTR);  \
            __result;                                    \
        }
    )
#endif

#endif  // CGROUP_SCHED_FRAMEWORK_PROCESS_GROUP_INCLUDE_PROCESS_GROUP_MACRO_H_
