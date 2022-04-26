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

#ifndef SOC_PERF_INCLUDE_CORE_SOCPERF_HANDLER_H
#define SOC_PERF_INCLUDE_CORE_SOCPERF_HANDLER_H

#include <unistd.h>
#include <unordered_map>
#include <stdlib.h>
#include <algorithm>
#include <climits>
#include "event_handler.h"
#include "event_runner.h"
#include "socperf_common.h"

namespace OHOS {
namespace SOCPERF {
class SocPerfHandler : public AppExecFwk::EventHandler {
public:
    explicit SocPerfHandler(const std::shared_ptr<AppExecFwk::EventRunner>& runner);
    ~SocPerfHandler() override;
    void ProcessEvent(const AppExecFwk::InnerEvent::Pointer& event) override;

private:
    std::unordered_map<int32_t, std::shared_ptr<ResNode>> resNodeInfo;
    std::unordered_map<int32_t, std::shared_ptr<GovResNode>> govResNodeInfo;
    std::unordered_map<int32_t, std::shared_ptr<ResStatus>> resStatusInfo;
    bool powerLimitBoost = false;
    bool thermalLimitBoost = false;

private:
    void UpdateResActionList(int32_t resId, std::shared_ptr<ResAction> resAction, bool delayed);
    void UpdateCandidatesValue(int32_t resId, int32_t type);
    void ArbitrateCandidate(int32_t resId);
    void ArbitratePairRes(int32_t resId);
    void UpdatePairResValue(int32_t minResId, int32_t minResValue, int32_t maxResId, int32_t maxResValue);
    void UpdateCurrentValue(int32_t resId, int32_t value);
    void WriteNode(std::string path, std::string value);
    bool ExistNoCandidate(
        int32_t resId, std::shared_ptr<ResStatus> resStatus, int32_t perf, int32_t power, int32_t thermal);
    bool IsGovResId(int32_t resId);
    bool IsValidResId(int32_t resId);
};
} // namespace SOCPERF
} // namespace OHOS

#endif
