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

namespace OHOS {
namespace SOCPERF {
SocPerfThreadWrap::SocPerfThreadWrap()
{
}

SocPerfThreadWrap::~SocPerfThreadWrap()
{
    if (handle) {
        dlclose(reportFunc);
    }
}

void SocPerfThreadWrap::InitQueue(const std::string& queueName)
{
    queue = std::make_shared<ffrt::queue>(queueName.c_str());
    if (queue == nullptr) {
        SOC_PERF_LOGE("create ffrt queue failed");
    }
}

void SocPerfThreadWrap::InitResNodeInfo(std::shared_ptr<ResNode> resNode)
{
    if (resNode == nullptr || queue == nullptr) {
        return;
    }
    std::function<void()>&& initResNodeInfoFunc = [this, resNode]() {
        resNodeInfo.insert(std::pair<int32_t, std::shared_ptr<ResNode>>(resNode->id, resNode));
        WriteNode(resNode->id, resNode->path, std::to_string(resNode->def));
        auto resStatus = std::make_shared<ResStatus>(resNode->def);
        resStatusInfo.insert(std::pair<int32_t, std::shared_ptr<ResStatus>>(resNode->id, resStatus));
    };
    queue->submit(initResNodeInfoFunc);
}

void SocPerfThreadWrap::InitPerfFunc(char* perfSoPath, char* perfSoFunc)
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
        dlclose(reportFunc);
    }
}

void SocPerfThreadWrap::InitGovResNodeInfo(std::shared_ptr<GovResNode> govResNode)
{
    if (govResNode == nullptr || queue == nullptr) {
        return;
    }
    std::function<void()>&& initGovResNodeInfoFunc = [this, govResNode]() {
        govResNodeInfo.insert(std::pair<int32_t, std::shared_ptr<GovResNode>>(govResNode->id, govResNode));
        for (int32_t i = 0; i < (int32_t)govResNode->paths.size(); i++) {
            WriteNode(govResNode->id, govResNode->paths[i], govResNode->levelToStr[govResNode->def][i]);
        }
        auto resStatus = std::make_shared<ResStatus>(govResNode->def);
        resStatusInfo.insert(std::pair<int32_t, std::shared_ptr<ResStatus>>(govResNode->id, resStatus));
    };
    queue->submit(initGovResNodeInfoFunc);
}

void SocPerfThreadWrap::DoFreqActionPack(std::shared_ptr<ResActionItem> head)
{
    if (head == nullptr || queue == nullptr) {
        return;
    }
    std::function<void()>&& doFreqActionPackFunc = [this, head]() {
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
    };
    queue->submit(doFreqActionPackFunc);
}

void SocPerfThreadWrap::UpdatePowerLimitBoostFreq(bool powerLimitBoost)
{
    if (queue == nullptr) {
        return;
    }
    std::function<void()>&& updatePowerLimitBoostFreqFunc = [this, powerLimitBoost]() {
        this->powerLimitBoost = powerLimitBoost;
        for (auto iter = resStatusInfo.begin(); iter != resStatusInfo.end(); ++iter) {
            ArbitrateCandidate(iter->first);
        }
        SendResStatusToPerfSo();
    };
    queue->submit(updatePowerLimitBoostFreqFunc);
}

void SocPerfThreadWrap::UpdateThermalLimitBoostFreq(bool thermalLimitBoost)
{
    if (queue == nullptr) {
        return;
    }
    std::function<void()>&& updateThermalLimitBoostFreqFunc = [this, thermalLimitBoost]() {
        this->thermalLimitBoost = thermalLimitBoost;
        for (auto iter = resStatusInfo.begin(); iter != resStatusInfo.end(); ++iter) {
            ArbitrateCandidate(iter->first);
        }
        SendResStatusToPerfSo();
    };
    queue->submit(updateThermalLimitBoostFreqFunc);
}

void SocPerfThreadWrap::UpdateLimitStatus(int32_t eventId, std::shared_ptr<ResAction> resAction, int32_t resId)
{
    if (resAction == nullptr || queue == nullptr) {
        return;
    }
    std::function<void()>&& updateLimitStatusFunc = [this, eventId, resId, resAction]() {
        if (eventId == INNER_EVENT_ID_DO_FREQ_ACTION) {
            DoFreqAction(resId, resAction);
        } else if (eventId == INNER_EVENT_ID_DO_FREQ_ACTION_LEVEL) {
            DoFreqActionLevel(resId, resAction);
        }
        SendResStatusToPerfSo();
    };
    queue->submit(updateLimitStatusFunc);
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
    if (!IsValidResId(resId) || queue == nullptr || resAction == nullptr) {
        return;
    }
    ffrt::task_attr taskAttr;
    taskAttr.delay(resAction->duration * SCALES_OF_MILLISECONDS_TO_MICROSECONDS);
    std::function<void()>&& postDelayTaskFunc = [this, resId, resAction]() {
        UpdateResActionList(resId, resAction, true);
        SendResStatusToPerfSo();
    };
    queue->submit(postDelayTaskFunc, taskAttr);
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
    PostDelayTask(resId, resAction);
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
    int64_t res = type == ACTION_TYPE_PERF ? MIN_INT_VALUE : MAX_INT_VALUE;
    int64_t endTime = MIN_INT_VALUE;
    for (auto iter = resStatus->resActionList[type].begin();
        iter != resStatus->resActionList[type].end(); ++iter) {
        if (((*iter)->value > res && type == ACTION_TYPE_PERF)
            || ((*iter)->value < res && type != ACTION_TYPE_PERF)) {
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
    int64_t candidatePerfValue = resStatus->candidatesValue[ACTION_TYPE_PERF];
    int64_t candidatePerfEndTime = resStatus->candidatesEndTime[ACTION_TYPE_PERF];
    int64_t candidatePowerValue = resStatus->candidatesValue[ACTION_TYPE_POWER];
    int64_t candidatePowerEndTime = resStatus->candidatesEndTime[ACTION_TYPE_POWER];
    int64_t candidateThermalValue = resStatus->candidatesValue[ACTION_TYPE_THERMAL];
    int64_t candidateThermalEndTime = resStatus->candidatesEndTime[ACTION_TYPE_THERMAL];

    if (ExistNoCandidate(resId, resStatus, candidatePerfValue, candidatePowerValue, candidateThermalValue)) {
        return;
    }

    if (!powerLimitBoost && !thermalLimitBoost) {
        if (candidatePerfValue != INVALID_VALUE) {
            resStatus->candidate = Max(candidatePerfValue, candidatePowerValue, candidateThermalValue);
        } else {
            resStatus->candidate =
                (candidatePowerValue == INVALID_VALUE) ? candidateThermalValue :
                    ((candidateThermalValue == INVALID_VALUE) ? candidatePowerValue :
                        Min(candidatePowerValue, candidateThermalValue));
        }
    } else if (!powerLimitBoost && thermalLimitBoost) {
        resStatus->candidate =
            (candidateThermalValue != INVALID_VALUE) ? candidateThermalValue :
                Max(candidatePerfValue, candidatePowerValue);
    } else if (powerLimitBoost && !thermalLimitBoost) {
        resStatus->candidate =
            (candidatePowerValue != INVALID_VALUE) ? candidatePowerValue :
                Max(candidatePerfValue, candidateThermalValue);
    } else {
        if (candidatePowerValue == INVALID_VALUE && candidateThermalValue == INVALID_VALUE) {
            resStatus->candidate = candidatePerfValue;
        } else {
            resStatus->candidate =
                (candidatePowerValue == INVALID_VALUE) ? candidateThermalValue :
                    ((candidateThermalValue == INVALID_VALUE) ? candidatePowerValue :
                        Min(candidatePowerValue, candidateThermalValue));
        }
    }
    resStatus->currentEndTime = Min(candidatePerfEndTime, candidatePowerEndTime, candidateThermalEndTime);
    ArbitratePairRes(resId);
}

void SocPerfThreadWrap::ArbitratePairRes(int32_t resId)
{
    if (IsGovResId(resId)) {
        UpdateCurrentValue(resId, resStatusInfo[resId]->candidate);
        return;
    }

    int32_t pairResId = resNodeInfo[resId]->pair;
    if (pairResId == INVALID_VALUE) {
        UpdateCurrentValue(resId, resStatusInfo[resId]->candidate);
        return;
    }

    if (resNodeInfo[resId]->mode == 1) {
        if (resStatusInfo[resId]->candidate < resStatusInfo[pairResId]->candidate) {
            if (powerLimitBoost || thermalLimitBoost) {
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
            if (powerLimitBoost || thermalLimitBoost) {
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

bool SocPerfThreadWrap::ExistNoCandidate(
    int32_t resId, std::shared_ptr<ResStatus> resStatus, int64_t perf, int64_t power, int64_t thermal)
{
    if (perf == INVALID_VALUE && power == INVALID_VALUE && thermal == INVALID_VALUE) {
        if (IsGovResId(resId)) {
            resStatus->candidate = govResNodeInfo[resId]->def;
        } else if (IsResId(resId)) {
            resStatus->candidate = resNodeInfo[resId]->def;
        }
        resStatus->currentEndTime = MAX_INT_VALUE;
        ArbitratePairRes(resId);
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
