/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#ifndef SOC_PERF_SERVICES_CORE_INCLUDE_SOCPERF_HANDLER_H
#define SOC_PERF_SERVICES_CORE_INCLUDE_SOCPERF_HANDLER_H


#include <memory>           // for shared_ptr
#include <string>           // for string
#include <unordered_map>    // for unordered_map
#include <stdint.h>         // for uint32_t
#include "event_handler.h"  // for EventHandler
#include "event_runner.h"
#include "inner_event.h"    // for InnerEvent, InnerEvent::Pointer
namespace OHOS { namespace SOCPERF { class GovResNode; } }
namespace OHOS { namespace SOCPERF { class ResAction; } }
namespace OHOS { namespace SOCPERF { class ResNode; } }
namespace OHOS { namespace SOCPERF { class ResStatus; } }

namespace OHOS {
namespace SOCPERF {
enum SocPerfInnerEvent : uint32_t {
    INNER_EVENT_ID_INIT_RES_NODE_INFO = 0,
    INNER_EVENT_ID_INIT_GOV_RES_NODE_INFO,
    INNER_EVENT_ID_DO_FREQ_ACTION,
    INNER_EVENT_ID_DO_FREQ_ACTION_PACK,
    INNER_EVENT_ID_DO_FREQ_ACTION_DELAYED,
    INNER_EVENT_ID_POWER_LIMIT_BOOST_FREQ,
    INNER_EVENT_ID_THERMAL_LIMIT_BOOST_FREQ,
};

class SocPerfHandler : public AppExecFwk::EventHandler {
public:
    explicit SocPerfHandler(const std::shared_ptr<AppExecFwk::EventRunner>& runner);
    ~SocPerfHandler() override;
    void ProcessEvent(const AppExecFwk::InnerEvent::Pointer& event) override;

private:
    std::unordered_map<int32_t, std::shared_ptr<ResNode>> resNodeInfo;
    std::unordered_map<int32_t, std::shared_ptr<GovResNode>> govResNodeInfo;
    std::unordered_map<int32_t, std::shared_ptr<ResStatus>> resStatusInfo;
    std::unordered_map<std::string, int32_t> fdInfo;
    bool powerLimitBoost = false;
    bool thermalLimitBoost = false;

private:
    void UpdateResActionList(int32_t resId, std::shared_ptr<ResAction> resAction, bool delayed);
    void UpdateCandidatesValue(int32_t resId, int32_t type);
    void ArbitrateCandidate(int32_t resId);
    void ArbitratePairRes(int32_t resId);
    void UpdatePairResValue(int32_t minResId, int64_t minResValue, int32_t maxResId, int64_t maxResValue);
    void UpdateCurrentValue(int32_t resId, int64_t value);
    void WriteNode(std::string path, std::string value);
    bool ExistNoCandidate(
        int32_t resId, std::shared_ptr<ResStatus> resStatus, int64_t perf, int64_t power, int64_t thermal);
    bool IsGovResId(int32_t resId);
    bool IsResId(int32_t resId);
    bool IsValidResId(int32_t resId);
    int32_t GetFdForFilePath(std::string filePath);
};
} // namespace SOCPERF
} // namespace OHOS

#endif // SOC_PERF_SERVICES_CORE_INCLUDE_SOCPERF_HANDLER_H
