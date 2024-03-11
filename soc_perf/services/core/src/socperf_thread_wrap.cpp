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
#include "socperf_thread_wrap.h"
#include <cstdio>            // for FILE
#include <cstdint>           // for int32_t
#include <climits>           // for PATH_MAX
#include <dlfcn.h>           // for dlopen
#include <list>              // for list, __list_iterator, operator!=
#include <new>               // for operator delete, operator new
#include <cstdlib>           // for realpath
#include <set>               // for set
#include <string>            // for basic_string, to_string
#include <unordered_map>     // for unordered_map, operator==, operator!=
#include <utility>           // for pair
#include <vector>            // for vector
#include <unistd.h>          // for open, write
#include <fcntl.h>           // for O_RDWR, O_CLOEXEC
#include "hisysevent.h"
#include "hitrace_meter.h"

namespace OHOS {
namespace SOCPERF {
namespace {
    const int32_t MAX_FREQUE_NODE = 1;
}
#ifdef SOCPERF_ADAPTOR_FFRT
SocPerfThreadWrap::SocPerfThreadWrap() : socperfQueue("socperf", ffrt::queue_attr().qos(ffrt::qos_user_interactive))
#else
SocPerfThreadWrap::SocPerfThreadWrap(
    const std::shared_ptr<AppExecFwk::EventRunner>& runner) : AppExecFwk::EventHandler(runner)
#endif
{
}

SocPerfThreadWrap::~SocPerfThreadWrap()
{
}

#ifndef SOCPERF_ADAPTOR_FFRT
void SocPerfThreadWrap::ProcessEvent(const AppExecFwk::InnerEvent::Pointer &event)
{
    if (event == nullptr) {
        return;
    }
    switch (event->GetInnerEventId()) {
        case INNER_EVENT_ID_INIT_RES_NODE_INFO: {
            InitResNodeInfo(event->GetSharedObject<ResNode>());
            break;
        }
        case INNER_EVENT_ID_INIT_GOV_RES_NODE_INFO: {
            InitGovResNodeInfo(event->GetSharedObject<GovResNode>());
            break;
        }
        case INNER_EVENT_ID_DO_FREQ_ACTION_PACK: {
            DoFreqActionPack(event->GetSharedObject<ResActionItem>());
            break;
        }
        case INNER_EVENT_ID_DO_FREQ_ACTION_DELAYED: {
            PostDelayTask(event->GetParam(), event->GetSharedObject<ResAction>());
            break;
        }
        case INNER_EVENT_ID_DO_FREQ_ACTION:
        case INNER_EVENT_ID_DO_FREQ_ACTION_LEVEL: {
            UpdateLimitStatus(event->GetInnerEventId(), event->GetSharedObject<ResAction>(), event->GetParam());
            break;
        }
        case INNER_EVENT_ID_POWER_LIMIT_BOOST_FREQ: {
            UpdatePowerLimitBoostFreq(event->GetParam());
            break;
        }
        case INNER_EVENT_ID_THERMAL_LIMIT_BOOST_FREQ: {
            UpdateThermalLimitBoostFreq(event->GetParam());
            break;
        }
        case INNER_EVENT_ID_CLEAR_ALL_ALIVE_REQUEST: {
            ClearAllAliveRequest();
            break;
        }
        default:
            break;
    }
}
#endif

void SocPerfThreadWrap::InitResNodeInfo(std::shared_ptr<ResNode> resNode)
{
    if (resNode == nullptr) {
        return;
    }
#ifdef SOCPERF_ADAPTOR_FFRT
    std::function<void()>&& initResNodeInfoFunc = [this, resNode]() {
#endif
        resNodeInfo.insert(std::pair<int32_t, std::shared_ptr<ResNode>>(resNode->id, resNode));
        WriteNode(resNode->id, resNode->path, std::to_string(resNode->def));
        auto resStatus = std::make_shared<ResStatus>(resNode->def);
        resStatusInfo.insert(std::pair<int32_t, std::shared_ptr<ResStatus>>(resNode->id, resStatus));
#ifdef SOCPERF_ADAPTOR_FFRT
    };
    socperfQueue.submit(initResNodeInfoFunc);
#endif
}

void SocPerfThreadWrap::InitPerfFunc(const char* perfSoPath, const char* perfSoFunc)
{
    if (!perfSoPath || !perfSoFunc) {
        return;
    }

    auto handle = dlopen(perfSoPath, RTLD_NOW);
    if (!handle) {
        SOC_PERF_LOGE("perf so doesn't exist");
        return;
    }

    reportFunc = reinterpret_cast<ReportDataFunc>(dlsym(handle, perfSoFunc));
    if (!reportFunc) {
        SOC_PERF_LOGE("perf func doesn't exist");
        dlclose(handle);
    }
}

void SocPerfThreadWrap::InitGovResNodeInfo(std::shared_ptr<GovResNode> govResNode)
{
    if (govResNode == nullptr) {
        return;
    }
#ifdef SOCPERF_ADAPTOR_FFRT
    std::function<void()>&& initGovResNodeInfoFunc = [this, govResNode]() {
#endif
        govResNodeInfo.insert(std::pair<int32_t, std::shared_ptr<GovResNode>>(govResNode->id, govResNode));
        for (int32_t i = 0; i < (int32_t)govResNode->paths.size(); i++) {
            WriteNode(govResNode->id, govResNode->paths[i], govResNode->levelToStr[govResNode->def][i]);
        }
        auto resStatus = std::make_shared<ResStatus>(govResNode->def);
        resStatusInfo.insert(std::pair<int32_t, std::shared_ptr<ResStatus>>(govResNode->id, resStatus));
#ifdef SOCPERF_ADAPTOR_FFRT
    };
    socperfQueue.submit(initGovResNodeInfoFunc);
#endif
}

void SocPerfThreadWrap::DoFreqActionPack(std::shared_ptr<ResActionItem> head)
{
    if (head == nullptr) {
        return;
    }
#ifdef SOCPERF_ADAPTOR_FFRT
    std::function<void()>&& doFreqActionPackFunc = [this, head]() {
#endif
        std::shared_ptr<ResActionItem> queueHead = head;
        while (queueHead) {
            if (IsValidResId(queueHead->resId)) {
                UpdateResActionList(queueHead->resId, queueHead->resAction, false);
            }
            auto temp = queueHead->next;
            queueHead->next = nullptr;
            queueHead = temp;
        }
        SendResStatusToPerfSo();
#ifdef SOCPERF_ADAPTOR_FFRT
    };
    socperfQueue.submit(doFreqActionPackFunc);
#endif
}

void SocPerfThreadWrap::UpdatePowerLimitBoostFreq(bool powerLimitBoost)
{
#ifdef SOCPERF_ADAPTOR_FFRT
    std::function<void()>&& updatePowerLimitBoostFreqFunc = [this, powerLimitBoost]() {
#endif
        this->powerLimitBoost = powerLimitBoost;
        for (auto iter = resStatusInfo.begin(); iter != resStatusInfo.end(); ++iter) {
            if (resStatusInfo[iter->first] == nullptr) {
                continue;
            }
            ArbitrateCandidate(iter->first);
        }
        SendResStatusToPerfSo();
#ifdef SOCPERF_ADAPTOR_FFRT
    };
    socperfQueue.submit(updatePowerLimitBoostFreqFunc);
#endif
}

void SocPerfThreadWrap::UpdateThermalLimitBoostFreq(bool thermalLimitBoost)
{
#ifdef SOCPERF_ADAPTOR_FFRT
    std::function<void()>&& updateThermalLimitBoostFreqFunc = [this, thermalLimitBoost]() {
#endif
        this->thermalLimitBoost = thermalLimitBoost;
        for (auto iter = resStatusInfo.begin(); iter != resStatusInfo.end(); ++iter) {
            if (resStatusInfo[iter->first] == nullptr) {
                continue;
            }
            ArbitrateCandidate(iter->first);
        }
        SendResStatusToPerfSo();
#ifdef SOCPERF_ADAPTOR_FFRT
    };
    socperfQueue.submit(updateThermalLimitBoostFreqFunc);
#endif
}

void SocPerfThreadWrap::UpdateLimitStatus(int32_t eventId, std::shared_ptr<ResAction> resAction, int32_t resId)
{
    if (resAction == nullptr) {
        return;
    }
#ifdef SOCPERF_ADAPTOR_FFRT
    std::function<void()>&& updateLimitStatusFunc = [this, eventId, resId, resAction]() {
#endif
        if (eventId == INNER_EVENT_ID_DO_FREQ_ACTION) {
            DoFreqAction(resId, resAction);
        } else if (eventId == INNER_EVENT_ID_DO_FREQ_ACTION_LEVEL) {
            DoFreqActionLevel(resId, resAction);
        }
        SendResStatusToPerfSo();
        if (resAction->onOff && resStatusInfo[resId] != nullptr) {
            HiSysEventWrite(OHOS::HiviewDFX::HiSysEvent::Domain::RSS, "LIMIT_REQUEST",
                            OHOS::HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
                            "CLIENT_ID", resAction->type,
                            "RES_ID", resId,
                            "CONFIG", resStatusInfo[resId]->candidate);
        }
#ifdef SOCPERF_ADAPTOR_FFRT
    };
    socperfQueue.submit(updateLimitStatusFunc);
#endif
}

void SocPerfThreadWrap::ClearAllAliveRequest()
{
#ifdef SOCPERF_ADAPTOR_FFRT
    std::function<void()>&& updateLimitStatusFunc = [this]() {
#endif
    for (const std::pair<int32_t, std::shared_ptr<ResStatus>>& item : this->resStatusInfo) {
        if (item.second == nullptr) {
            continue;
        }
        std::list<std::shared_ptr<ResAction>>& resActionList = item.second->resActionList[ACTION_TYPE_PERF];
        resActionList.clear();
        UpdateCandidatesValue(item.first, ACTION_TYPE_PERF);
    }
    SendResStatusToPerfSo();
#ifdef SOCPERF_ADAPTOR_FFRT
    };
    socperfQueue.submit(updateLimitStatusFunc);
#endif
}

void SocPerfThreadWrap::DoFreqAction(int32_t resId, std::shared_ptr<ResAction> resAction)
{
    if (!IsValidResId(resId) || resAction == nullptr) {
        return;
    }
    UpdateResActionList(resId, resAction, false);
}

void SocPerfThreadWrap::SendResStatusToPerfSo()
{
    if (!reportFunc) {
        return;
    }
    std::vector<int32_t> qosId;
    std::vector<int64_t> value;
    std::vector<int64_t> endTime;
    for (auto iter = resStatusInfo.begin(); iter != resStatusInfo.end(); ++iter) {
        int32_t resId = iter->first;
        std::shared_ptr<ResStatus> resStatus = iter->second;
        if ((resNodeInfo.find(resId) != resNodeInfo.end() && resNodeInfo[resId]->persistMode == REPORT_TO_PERFSO)
            || (govResNodeInfo.find(resId) != govResNodeInfo.end()
                && govResNodeInfo[resId]->persistMode == REPORT_TO_PERFSO)) {
            if (resStatus->previousValue != resStatus->currentValue
                || resStatus->previousEndTime != resStatus->currentEndTime) {
                qosId.push_back(resId);
                value.push_back(resStatus->currentValue);
                endTime.push_back(resStatus->currentEndTime);
                resStatus->previousValue = resStatus->currentValue;
                resStatus->previousEndTime = resStatus->currentEndTime;
            }
        }
    }
    if (qosId.size() > 0) {
        reportFunc(qosId, value, endTime, "");
        std::string log("send data to perf so");
        for (unsigned long i = 0; i < qosId.size(); i++) {
            log.append(",[id:").append(std::to_string(qosId[i]));
            log.append(", value:").append(std::to_string(value[i]));
            log.append(", endTime:").append(std::to_string(endTime[i])).append("]");
        }
        StartTrace(HITRACE_TAG_OHOS, log.c_str());
        FinishTrace(HITRACE_TAG_OHOS);
    }
}

void SocPerfThreadWrap::DoFreqActionLevel(int32_t resId, std::shared_ptr<ResAction> resAction)
{
    int32_t realResId = resId - RES_ID_ADDITION;
    if (!IsValidResId(realResId) || !resAction) {
        return;
    }
    int32_t level = (int32_t)resAction->value;
    if (!GetResValueByLevel(realResId, level, resAction->value)) {
        return;
    }
    UpdateResActionList(realResId, resAction, false);
}

void SocPerfThreadWrap::PostDelayTask(int32_t resId, std::shared_ptr<ResAction> resAction)
{
    if (!IsValidResId(resId) || resAction == nullptr) {
        return;
    }
#ifdef SOCPERF_ADAPTOR_FFRT
    ffrt::task_attr taskAttr;
    taskAttr.delay(resAction->duration * SCALES_OF_MILLISECONDS_TO_MICROSECONDS);
    std::function<void()>&& postDelayTaskFunc = [this, resId, resAction]() {
        UpdateResActionList(resId, resAction, true);
        SendResStatusToPerfSo();
    };
    socperfQueue.submit(postDelayTaskFunc, taskAttr);
#else
    UpdateResActionList(resId, resAction, true);
    SendResStatusToPerfSo();
#endif
}

bool SocPerfThreadWrap::GetResValueByLevel(int32_t resId, int32_t level, int64_t& resValue)
{
    if (resNodeInfo.find(resId) == resNodeInfo.end()
        || resNodeInfo[resId]->available.empty()) {
        SOC_PERF_LOGE("resId[%{public}d] is not valid.", resId);
        return false;
    }
    if (level < 0) {
        return false;
    }

    std::set<int64_t> available;
    for (auto a : resNodeInfo[resId]->available) {
        available.insert(a);
    }
    int32_t len = (int32_t)available.size();
    auto iter = available.begin();
    if (level < len) {
        std::advance(iter, len - 1 - level);
    }
    resValue = *iter;
    return true;
}

void SocPerfThreadWrap::UpdateResActionList(int32_t resId, std::shared_ptr<ResAction> resAction, bool delayed)
{
    std::shared_ptr<ResStatus> resStatus = resStatusInfo[resId];
    int32_t type = resAction->type;

    if (delayed) {
        UpdateResActionListByDelayedMsg(resId, type, resAction, resStatus);
    } else {
        UpdateResActionListByInstantMsg(resId, type, resAction, resStatus);
    }
}

void SocPerfThreadWrap::UpdateResActionListByDelayedMsg(int32_t resId, int32_t type,
    std::shared_ptr<ResAction> resAction, std::shared_ptr<ResStatus> resStatus)
{
    for (auto iter = resStatus->resActionList[type].begin();
        iter != resStatus->resActionList[type].end(); ++iter) {
        if (resAction == *iter) {
            resStatus->resActionList[type].erase(iter);
            UpdateCandidatesValue(resId, type);
            break;
        }
    }
}

void SocPerfThreadWrap::HandleShortTimeResAction(int32_t resId, int32_t type,
    std::shared_ptr<ResAction> resAction, std::shared_ptr<ResStatus> resStatus)
{
    resStatus->resActionList[type].push_back(resAction);
    UpdateCandidatesValue(resId, type);
#ifdef SOCPERF_ADAPTOR_FFRT
    PostDelayTask(resId, resAction);
#else
    auto event = AppExecFwk::InnerEvent::Get(
        INNER_EVENT_ID_DO_FREQ_ACTION_DELAYED, resAction, resId);
    this->SendEvent(event, resAction->duration);
#endif
}

void SocPerfThreadWrap::HandleLongTimeResAction(int32_t resId, int32_t type,
    std::shared_ptr<ResAction> resAction, std::shared_ptr<ResStatus> resStatus)
{
    for (auto iter = resStatus->resActionList[type].begin();
         iter != resStatus->resActionList[type].end(); ++iter) {
        if (resAction->TotalSame(*iter)) {
            resStatus->resActionList[type].erase(iter);
            break;
        }
    }
    resStatus->resActionList[type].push_back(resAction);
    UpdateCandidatesValue(resId, type);
}

void SocPerfThreadWrap::UpdateResActionListByInstantMsg(int32_t resId, int32_t type,
    std::shared_ptr<ResAction> resAction, std::shared_ptr<ResStatus> resStatus)
{
    switch (resAction->onOff) {
        case EVENT_INVALID: {
            HandleShortTimeResAction(resId, type, resAction, resStatus);
            break;
        }
        case EVENT_ON: {
            if (resAction->duration == 0) {
                HandleLongTimeResAction(resId, type, resAction, resStatus);
            } else {
                HandleShortTimeResAction(resId, type, resAction, resStatus);
            }
            break;
        }
        case EVENT_OFF: {
            for (auto iter = resStatus->resActionList[type].begin();
                iter != resStatus->resActionList[type].end(); ++iter) {
                if (resAction->PartSame(*iter) && (*iter)->onOff == EVENT_ON) {
                    resStatus->resActionList[type].erase(iter);
                    UpdateCandidatesValue(resId, type);
                    break;
                }
            }
            break;
        }
        default: {
            break;
        }
    }
}

void SocPerfThreadWrap::UpdateCandidatesValue(int32_t resId, int32_t type)
{
    std::shared_ptr<ResStatus> resStatus = resStatusInfo[resId];
    int64_t prevValue = resStatus->candidatesValue[type];
    int64_t prevEndTime = resStatus->candidatesEndTime[type];

    if (resStatus->resActionList[type].empty()) {
        resStatus->candidatesValue[type] = INVALID_VALUE;
        resStatus->candidatesEndTime[type] = MAX_INT_VALUE;
    } else {
        InnerArbitrateCandidatesValue(type, resStatus);
    }

    if (resStatus->candidatesValue[type] != prevValue || resStatus->candidatesEndTime[type] != prevEndTime) {
        ArbitrateCandidate(resId);
    }
}

void SocPerfThreadWrap::InnerArbitrateCandidatesValue(int32_t type, std::shared_ptr<ResStatus> resStatus)
{
    // perf first action type:  ACTION_TYPE_PERF\ACTION_TYPE_PERFLVL
    // power first action type: ACTION_TYPE_POWER\ACTION_TYPE_THERMAL
    bool isPerfFirst = (type == ACTION_TYPE_PERF || type == ACTION_TYPE_PERFLVL);

    int64_t res = isPerfFirst ? MIN_INT_VALUE : MAX_INT_VALUE;
    int64_t endTime = MIN_INT_VALUE;
    for (auto iter = resStatus->resActionList[type].begin();
        iter != resStatus->resActionList[type].end(); ++iter) {
        if (((*iter)->value > res && isPerfFirst)
            || ((*iter)->value < res && !isPerfFirst)) {
            res = (*iter)->value;
            endTime = (*iter)->endTime;
        } else if ((*iter)->value == res) {
            endTime = Max(endTime, (*iter)->endTime);
        }
    }
    resStatus->candidatesValue[type] = res;
    resStatus->candidatesEndTime[type] = endTime;
}

void SocPerfThreadWrap::ArbitrateCandidate(int32_t resId)
{
    std::shared_ptr<ResStatus> resStatus = resStatusInfo[resId];
    // if perf, power and thermal don't have valid value, send default value
    if (ExistNoCandidate(resId, resStatus)) {
        return;
    }
    // Arbitrate in perf, power and thermal
    ProcessLimitCase(resId);
    // perf request thermal level is highest priority in this freq adjuster
    if (ArbitratePairResInPerfLvl(resId)) {
        return;
    }
    // adjust resource value if it has 'max' config
    ArbitratePairRes(resId, false);
}

void SocPerfThreadWrap::ProcessLimitCase(int32_t resId)
{
    std::shared_ptr<ResStatus> resStatus = resStatusInfo[resId];
    int64_t candidatePerfValue = resStatus->candidatesValue[ACTION_TYPE_PERF];
    int64_t candidatePowerValue = resStatus->candidatesValue[ACTION_TYPE_POWER];
    int64_t candidateThermalValue = resStatus->candidatesValue[ACTION_TYPE_THERMAL];
    if (!powerLimitBoost && !thermalLimitBoost) {
        if (candidatePerfValue != INVALID_VALUE) {
            resStatus->candidate = Max(candidatePerfValue, candidatePowerValue, candidateThermalValue);
        } else {
            resStatus->candidate = (candidatePowerValue == INVALID_VALUE) ? candidateThermalValue :
                ((candidateThermalValue == INVALID_VALUE) ? candidatePowerValue :
                Min(candidatePowerValue, candidateThermalValue));
        }
    } else if (!powerLimitBoost && thermalLimitBoost) {
        resStatus->candidate = (candidateThermalValue != INVALID_VALUE) ? candidateThermalValue :
            Max(candidatePerfValue, candidatePowerValue);
    } else if (powerLimitBoost && !thermalLimitBoost) {
        resStatus->candidate = (candidatePowerValue != INVALID_VALUE) ? candidatePowerValue :
            Max(candidatePerfValue, candidateThermalValue);
    } else {
        if (candidatePowerValue == INVALID_VALUE && candidateThermalValue == INVALID_VALUE) {
            resStatus->candidate = candidatePerfValue;
        } else {
            resStatus->candidate = (candidatePowerValue == INVALID_VALUE) ? candidateThermalValue :
                ((candidateThermalValue == INVALID_VALUE) ? candidatePowerValue :
                Min(candidatePowerValue, candidateThermalValue));
        }
    }
    resStatus->currentEndTime = Min(resStatus->candidatesEndTime[ACTION_TYPE_PERF],
        resStatus->candidatesEndTime[ACTION_TYPE_POWER], resStatus->candidatesEndTime[ACTION_TYPE_THERMAL]);
}

bool SocPerfThreadWrap::ArbitratePairResInPerfLvl(int32_t resId)
{
    std::shared_ptr<ResStatus> resStatus = resStatusInfo[resId];
    int32_t pairResId = IsGovResId(resId) ? INVALID_VALUE : resNodeInfo[resId]->pair;
    // if resource self and resource's pair both not have perflvl value
    if (resStatus->candidatesValue[ACTION_TYPE_PERFLVL] == INVALID_VALUE && (pairResId != INVALID_VALUE &&
        resStatusInfo[pairResId]->candidatesValue[ACTION_TYPE_PERFLVL] == INVALID_VALUE)) {
        return false;
    }
    // if this resource has PerfRequestLvl value, the final arbitrate value change to PerfRequestLvl value
    if (resStatus->candidatesValue[ACTION_TYPE_PERFLVL] != INVALID_VALUE) {
        resStatus->candidate = resStatus->candidatesValue[ACTION_TYPE_PERFLVL];
    }
    // only limit max when PerfRequestLvl has max value
    bool limit = false;
    if (!IsGovResId(resId) && (resNodeInfo[resId]->mode == MAX_FREQUE_NODE ||
        (pairResId != INVALID_VALUE && resNodeInfo[pairResId]->mode == MAX_FREQUE_NODE))) {
        limit = true;
    }
    ArbitratePairRes(resId, limit);
    return true;
}

void SocPerfThreadWrap::ArbitratePairRes(int32_t resId, bool perfRequestLimit)
{
    bool limit = powerLimitBoost || thermalLimitBoost || perfRequestLimit;
    if (IsGovResId(resId)) {
        UpdateCurrentValue(resId, resStatusInfo[resId]->candidate);
        return;
    }

    int32_t pairResId = resNodeInfo[resId]->pair;
    if (pairResId == INVALID_VALUE) {
        UpdateCurrentValue(resId, resStatusInfo[resId]->candidate);
        return;
    }

    if (resNodeInfo[resId]->mode == MAX_FREQUE_NODE) {
        if (resStatusInfo[resId]->candidate < resStatusInfo[pairResId]->candidate) {
            if (limit) {
                UpdatePairResValue(pairResId,
                    resStatusInfo[resId]->candidate, resId, resStatusInfo[resId]->candidate);
            } else {
                UpdatePairResValue(pairResId,
                    resStatusInfo[pairResId]->candidate, resId, resStatusInfo[pairResId]->candidate);
            }
        } else {
            UpdatePairResValue(pairResId,
                resStatusInfo[pairResId]->candidate, resId, resStatusInfo[resId]->candidate);
        }
    } else {
        if (resStatusInfo[resId]->candidate > resStatusInfo[pairResId]->candidate) {
            if (limit) {
                UpdatePairResValue(resId,
                    resStatusInfo[pairResId]->candidate, pairResId, resStatusInfo[pairResId]->candidate);
            } else {
                UpdatePairResValue(resId,
                    resStatusInfo[resId]->candidate, pairResId, resStatusInfo[resId]->candidate);
            }
        } else {
            UpdatePairResValue(resId,
                resStatusInfo[resId]->candidate, pairResId, resStatusInfo[pairResId]->candidate);
        }
    }
}

void SocPerfThreadWrap::UpdatePairResValue(int32_t minResId, int64_t minResValue, int32_t maxResId,
    int64_t maxResValue)
{
    WriteNode(minResId, resNodeInfo[minResId]->path, std::to_string(resNodeInfo[minResId]->def));
    WriteNode(maxResId, resNodeInfo[maxResId]->path, std::to_string(resNodeInfo[maxResId]->def));
    UpdateCurrentValue(minResId, minResValue);
    UpdateCurrentValue(maxResId, maxResValue);
}

void SocPerfThreadWrap::UpdateCurrentValue(int32_t resId, int64_t currValue)
{
    resStatusInfo[resId]->currentValue = currValue;
    if (IsGovResId(resId)) {
        if (govResNodeInfo[resId]->levelToStr.find(currValue) != govResNodeInfo[resId]->levelToStr.end()) {
            std::vector<std::string> targetStrs = govResNodeInfo[resId]->levelToStr[resStatusInfo[resId]->currentValue];
            for (int32_t i = 0; i < (int32_t)govResNodeInfo[resId]->paths.size(); i++) {
                WriteNode(resId, govResNodeInfo[resId]->paths[i], targetStrs[i]);
            }
        }
    } else if (IsResId(resId)) {
        WriteNode(resId, resNodeInfo[resId]->path, std::to_string(resStatusInfo[resId]->currentValue));
    }
}

void SocPerfThreadWrap::WriteNode(int32_t resId, const std::string& filePath, const std::string& value)
{
    if ((IsGovResId(resId) && govResNodeInfo[resId]->persistMode == REPORT_TO_PERFSO)
        || (IsResId(resId) && resNodeInfo[resId]->persistMode == REPORT_TO_PERFSO)) {
        return;
    }
    int32_t fd = GetFdForFilePath(filePath);
    if (fd < 0) {
        return;
    }
    write(fd, value.c_str(), value.size());
}

int32_t SocPerfThreadWrap::GetFdForFilePath(const std::string& filePath)
{
    if (fdInfo.find(filePath) != fdInfo.end()) {
        return fdInfo[filePath];
    }
    char path[PATH_MAX + 1] = {0};
    if (filePath.size() == 0 || filePath.size() > PATH_MAX || !realpath(filePath.c_str(), path)) {
        return -1;
    }
    int32_t fd = open(path, O_RDWR | O_CLOEXEC);
    if (fd < 0) {
        return fd;
    }
    fdInfo.insert(std::pair<std::string, int32_t>(filePath, fd));
    return fdInfo[filePath];
}

bool SocPerfThreadWrap::ExistNoCandidate(int32_t resId, std::shared_ptr<ResStatus> resStatus)
{
    int64_t perfCandidate = resStatus->candidatesValue[ACTION_TYPE_PERF];
    int64_t powerCandidate = resStatus->candidatesValue[ACTION_TYPE_POWER];
    int64_t thermalCandidate = resStatus->candidatesValue[ACTION_TYPE_THERMAL];
    int64_t perfLvlCandidate = resStatus->candidatesValue[ACTION_TYPE_PERFLVL];
    if (perfCandidate == INVALID_VALUE && powerCandidate == INVALID_VALUE && thermalCandidate == INVALID_VALUE
        && perfLvlCandidate == INVALID_VALUE) {
        if (IsGovResId(resId)) {
            resStatus->candidate = govResNodeInfo[resId]->def;
        } else if (IsResId(resId)) {
            resStatus->candidate = resNodeInfo[resId]->def;
        }
        resStatus->currentEndTime = MAX_INT_VALUE;
        ArbitratePairRes(resId, false);
        return true;
    }
    return false;
}

bool SocPerfThreadWrap::IsGovResId(int32_t resId)
{
    if (resNodeInfo.find(resId) == resNodeInfo.end()
        && govResNodeInfo.find(resId) != govResNodeInfo.end()) {
        return true;
    }
    return false;
}

bool SocPerfThreadWrap::IsResId(int32_t resId)
{
    if (govResNodeInfo.find(resId) == govResNodeInfo.end()
        && resNodeInfo.find(resId) != resNodeInfo.end()) {
        return true;
    }
    return false;
}

bool SocPerfThreadWrap::IsValidResId(int32_t resId)
{
    if (resNodeInfo.find(resId) == resNodeInfo.end()
        && govResNodeInfo.find(resId) == govResNodeInfo.end()) {
        return false;
    }
    if (resStatusInfo.find(resId) == resStatusInfo.end()) {
        return false;
    }
    return true;
}
} // namespace SOCPERF
} // namespace OHOS
