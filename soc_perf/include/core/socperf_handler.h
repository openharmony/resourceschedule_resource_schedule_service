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
    std::unordered_map<int, std::shared_ptr<ResNode>> resNodeInfo;
    std::unordered_map<int, std::shared_ptr<GovResNode>> govResNodeInfo;
    std::unordered_map<int, std::shared_ptr<ResStatus>> resStatusInfo;
    bool powerLimitBoost = false;
    bool thermalLimitBoost = false;

private:
    void UpdateResActionList(int resId, std::shared_ptr<ResAction> resAction, bool delayed);
    void UpdateCandidatesValue(int resId, int type);
    void ArbitrateCandidate(int resId);
    void ArbitratePairRes(int resId);
    void UpdatePairResValue(int minResId, int minResValue, int maxResId, int maxResValue);
    void UpdateCurrentValue(int resId, int value);
    void WriteNode(std::string path, std::string value);
    bool ExistNoCandidate(int resId, std::shared_ptr<ResStatus> resStatus, int perf, int power, int thermal);
    bool IsGovResId(int resId);
    bool IsValidResId(int resId);
};
} // namespace SOCPERF
} // namespace OHOS

#endif
