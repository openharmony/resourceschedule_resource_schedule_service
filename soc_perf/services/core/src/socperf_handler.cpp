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
#include "socperf_handler.h"
#include <cstdio>            // for FILE
#include <cstdint>           // for int32_t
#include <climits>           // for PATH_MAX
#include <list>              // for list, __list_iterator, operator!=
#include <new>               // for operator delete, operator new
#include <cstdlib>           // for realpath
#include <string>            // for basic_string, to_string
#include <unordered_map>     // for unordered_map, operator==, operator!=
#include <utility>           // for pair
#include <vector>            // for vector
#include <unistd.h>          // for open, write
#include <fcntl.h>           // for O_RDWR, O_CLOEXEC
#include "socperf_common.h"  // for ResStatus, ResAction, ResNode, INVALID_V...

namespace OHOS {
namespace SOCPERF {
SocPerfHandler::SocPerfHandler(
    const std::shared_ptr<AppExecFwk::EventRunner>& runner) : AppExecFwk::EventHandler(runner)
{
}

SocPerfHandler::~SocPerfHandler()
{
}

void SocPerfHandler::ProcessEvent(const AppExecFwk::InnerEvent::Pointer &event)
{
    if (event == nullptr) {
        return;
    }
    switch (event->GetInnerEventId()) {
        case INNER_EVENT_ID_INIT_RES_NODE_INFO: {
            auto resNode = event->GetSharedObject<ResNode>();
            if (resNode != nullptr) {
                resNodeInfo.insert(std::pair<int32_t, std::shared_ptr<ResNode>>(resNode->id, resNode));
                WriteNode(resNode->path, std::to_string(resNode->def));
                auto resStatus = std::make_shared<ResStatus>(resNode->def);
                resStatusInfo.insert(std::pair<int32_t, std::shared_ptr<ResStatus>>(resNode->id, resStatus));
            }
            break;
        }
        case INNER_EVENT_ID_INIT_GOV_RES_NODE_INFO: {
            auto govResNode = event->GetSharedObject<GovResNode>();
            if (govResNode != nullptr) {
                govResNodeInfo.insert(std::pair<int32_t, std::shared_ptr<GovResNode>>(govResNode->id, govResNode));
                for (int32_t i = 0; i < (int32_t)govResNode->paths.size(); i++) {
                    WriteNode(govResNode->paths[i], govResNode->levelToStr[govResNode->def][i]);
                }
                auto resStatus = std::make_shared<ResStatus>(govResNode->def);
                resStatusInfo.insert(std::pair<int32_t, std::shared_ptr<ResStatus>>(govResNode->id, resStatus));
            }
            break;
        }
        case INNER_EVENT_ID_DO_FREQ_ACTION: {
            int32_t resId = event->GetParam();
            if (!IsValidResId(resId)) {
                return;
            }
            std::shared_ptr<ResAction> resAction = event->GetSharedObject<ResAction>();
            if (resAction != nullptr) {
                UpdateResActionList(resId, resAction, false);
            }
            break;
        }
        case INNER_EVENT_ID_DO_FREQ_ACTION_DELAYED: {
            int32_t resId = event->GetParam();
            if (!IsValidResId(resId)) {
                return;
            }
            std::shared_ptr<ResAction> resAction = event->GetSharedObject<ResAction>();
            if (resAction != nullptr) {
                UpdateResActionList(resId, resAction, true);
            }
            break;
        }
        case INNER_EVENT_ID_POWER_LIMIT_BOOST_FREQ: {
            powerLimitBoost = event->GetParam() == 1;
            for (auto iter = resStatusInfo.begin(); iter != resStatusInfo.end(); ++iter) {
                ArbitrateCandidate(iter->first);
            }
            break;
        }
        case INNER_EVENT_ID_THERMAL_LIMIT_BOOST_FREQ: {
            thermalLimitBoost = event->GetParam() == 1;
            for (auto iter = resStatusInfo.begin(); iter != resStatusInfo.end(); ++iter) {
                ArbitrateCandidate(iter->first);
            }
            break;
        }
        default: {
            break;
        }
    }
}

void SocPerfHandler::UpdateResActionList(int32_t resId, std::shared_ptr<ResAction> resAction, bool delayed)
{
    std::shared_ptr<ResStatus> resStatus = resStatusInfo[resId];
    int32_t type = resAction->type;

    if (delayed) {
        for (auto iter = resStatus->resActionList[type].begin();
            iter != resStatus->resActionList[type].end(); ++iter) {
            if (resAction == *iter) {
                resStatus->resActionList[type].erase(iter);
                UpdateCandidatesValue(resId, type);
                break;
            }
        }
    } else {
        switch (resAction->onOff) {
            case EVENT_INVALID: {
                resStatus->resActionList[type].push_back(resAction);
                UpdateCandidatesValue(resId, type);
                auto event = AppExecFwk::InnerEvent::Get(
                    INNER_EVENT_ID_DO_FREQ_ACTION_DELAYED, resAction, resId);
                this->SendEvent(event, resAction->duration);
                break;
            }
            case EVENT_ON: {
                if (resAction->duration == 0) {
                    for (auto iter = resStatus->resActionList[type].begin();
                        iter != resStatus->resActionList[type].end(); ++iter) {
                        if (resAction->TotalSame(*iter)) {
                            resStatus->resActionList[type].erase(iter);
                            break;
                        }
                    }
                    resStatus->resActionList[type].push_back(resAction);
                    UpdateCandidatesValue(resId, type);
                } else {
                    resStatus->resActionList[type].push_back(resAction);
                    UpdateCandidatesValue(resId, type);
                    auto event = AppExecFwk::InnerEvent::Get(
                        INNER_EVENT_ID_DO_FREQ_ACTION_DELAYED, resAction, resId);
                    this->SendEvent(event, resAction->duration);
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
}

void SocPerfHandler::UpdateCandidatesValue(int32_t resId, int32_t type)
{
    std::shared_ptr<ResStatus> resStatus = resStatusInfo[resId];
    int64_t prev = resStatus->candidates[type];

    if (resStatus->resActionList[type].empty()) {
        resStatus->candidates[type] = INVALID_VALUE;
    } else {
        if (type == ACTION_TYPE_PERF) {
            int64_t res = MIN_INT_VALUE;
            for (auto iter = resStatus->resActionList[type].begin();
                iter != resStatus->resActionList[type].end(); ++iter) {
                res = Max(res, (*iter)->value);
            }
            resStatus->candidates[type] = res;
        } else {
            int64_t res = MAX_INT_VALUE;
            for (auto iter = resStatus->resActionList[type].begin();
                iter != resStatus->resActionList[type].end(); ++iter) {
                res = Min(res, (*iter)->value);
            }
            resStatus->candidates[type] = res;
        }
    }

    if (resStatus->candidates[type] != prev) {
        ArbitrateCandidate(resId);
    }
}

void SocPerfHandler::ArbitrateCandidate(int32_t resId)
{
    std::shared_ptr<ResStatus> resStatus = resStatusInfo[resId];
    int64_t candidatePerf = resStatus->candidates[ACTION_TYPE_PERF];
    int64_t candidatePower = resStatus->candidates[ACTION_TYPE_POWER];
    int64_t candidateThermal = resStatus->candidates[ACTION_TYPE_THERMAL];

    if (ExistNoCandidate(resId, resStatus, candidatePerf, candidatePower, candidateThermal)) {
        return;
    }

    if (!powerLimitBoost && !thermalLimitBoost) {
        if (candidatePerf != INVALID_VALUE) {
            resStatus->candidate = Max(candidatePerf, candidatePower, candidateThermal);
        } else {
            resStatus->candidate =
                (candidatePower == INVALID_VALUE) ? candidateThermal :
                    ((candidateThermal == INVALID_VALUE) ? candidatePower : Min(candidatePower, candidateThermal));
        }
    } else if (!powerLimitBoost && thermalLimitBoost) {
        resStatus->candidate =
            (candidateThermal != INVALID_VALUE) ? candidateThermal : Max(candidatePerf, candidatePower);
    } else if (powerLimitBoost && !thermalLimitBoost) {
        resStatus->candidate =
            (candidatePower != INVALID_VALUE) ? candidatePower : Max(candidatePerf, candidateThermal);
    } else {
        if (candidatePower == INVALID_VALUE && candidateThermal == INVALID_VALUE) {
            resStatus->candidate = candidatePerf;
        } else {
            resStatus->candidate =
                (candidatePower == INVALID_VALUE) ? candidateThermal :
                    ((candidateThermal == INVALID_VALUE) ? candidatePower : Min(candidatePower, candidateThermal));
        }
    }

    ArbitratePairRes(resId);
}

void SocPerfHandler::ArbitratePairRes(int32_t resId)
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

void SocPerfHandler::UpdatePairResValue(int32_t minResId, int64_t minResValue, int32_t maxResId, int64_t maxResValue)
{
    WriteNode(resNodeInfo[minResId]->path, std::to_string(resNodeInfo[minResId]->def));
    WriteNode(resNodeInfo[maxResId]->path, std::to_string(resNodeInfo[maxResId]->def));
    UpdateCurrentValue(minResId, minResValue);
    UpdateCurrentValue(maxResId, maxResValue);
}

void SocPerfHandler::UpdateCurrentValue(int32_t resId, int64_t currValue)
{
    resStatusInfo[resId]->current = currValue;
    if (IsGovResId(resId)) {
        if (govResNodeInfo[resId]->levelToStr.find(currValue) != govResNodeInfo[resId]->levelToStr.end()) {
            std::vector<std::string> targetStrs = govResNodeInfo[resId]->levelToStr[resStatusInfo[resId]->current];
            for (int32_t i = 0; i < (int32_t)govResNodeInfo[resId]->paths.size(); i++) {
                WriteNode(govResNodeInfo[resId]->paths[i], targetStrs[i]);
            }
        }
    } else if (IsResId(resId)) {
        WriteNode(resNodeInfo[resId]->path, std::to_string(resStatusInfo[resId]->current));
    }
}

void SocPerfHandler::WriteNode(std::string filePath, std::string value)
{
    int32_t fd = GetFdForFilePath(filePath);
    if (fd < 0) {
        return;
    }
    write(fd, value.c_str(), value.size());
}

int32_t SocPerfHandler::GetFdForFilePath(std::string filePath)
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

bool SocPerfHandler::ExistNoCandidate(
    int32_t resId, std::shared_ptr<ResStatus> resStatus, int64_t perf, int64_t power, int64_t thermal)
{
    if (perf == INVALID_VALUE && power == INVALID_VALUE && thermal == INVALID_VALUE) {
        if (IsGovResId(resId)) {
            resStatus->candidate = govResNodeInfo[resId]->def;
        } else if (IsResId(resId)) {
            resStatus->candidate = resNodeInfo[resId]->def;
        }
        ArbitratePairRes(resId);
        return true;
    }
    return false;
}

bool SocPerfHandler::IsGovResId(int32_t resId)
{
    if (resNodeInfo.find(resId) == resNodeInfo.end()
        && govResNodeInfo.find(resId) != govResNodeInfo.end()) {
        return true;
    }
    return false;
}

bool SocPerfHandler::IsResId(int32_t resId)
{
    if (govResNodeInfo.find(resId) == govResNodeInfo.end()
        && resNodeInfo.find(resId) != resNodeInfo.end()) {
        return true;
    }
    return false;
}

bool SocPerfHandler::IsValidResId(int32_t resId)
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
