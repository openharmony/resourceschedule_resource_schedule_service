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

#include "executor_hitrace_chain.h"

namespace OHOS {
namespace ResourceSchedule {
namespace {
constexpr int32_t DEFAULT_FLAGS = HiTraceFlag::HITRACE_FLAG_INCLUDE_ASYNC |
                                  HiTraceFlag::HITRACE_FLAG_DONOT_CREATE_SPAN |
                                  HiTraceFlag::HITRACE_FLAG_NO_BE_INFO;
}

ExecutorHiTraceChain::ExecutorHiTraceChain(const bool isClearId, const char *name)
{
    if (isClearId) {
        HiTraceChainClearId();
        isBegin_ = true;
        traceId_ = HiTraceChainBegin(name, DEFAULT_FLAGS);
    }
}

ExecutorHiTraceChain::ExecutorHiTraceChain(const char *name, const int32_t flags)
{
    HiTraceIdStruct currentId = HiTraceChainGetId();
    isBegin_ = !HiTraceChainIsValid(&currentId);
    if (isBegin_) {
        traceId_ = HiTraceChainBegin(name, (flags >= 0) ? flags : DEFAULT_FLAGS);
    }
}

ExecutorHiTraceChain::~ExecutorHiTraceChain()
{
    if (isBegin_) {
        HiTraceChainEnd(&traceId_);
    }
}

} // namespace ResourceSchedule
} // namespace OHOS
