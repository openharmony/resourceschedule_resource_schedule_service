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
    switch (event->GetInnerEventId()) {
        case INNER_EVENT_ID_INIT_RES_NODE_INFO: {
            auto resNode = event->GetSharedObject<ResNode>();
            resNodeInfo.insert(std::pair<int, std::shared_ptr<ResNode>>(resNode->id, resNode));
            WriteNode(resNode->path, std::to_string(resNode->def));
            auto resStatus = std::make_shared<ResStatus>(resNode->def);
            resStatusInfo.insert(std::pair<int, std::shared_ptr<ResStatus>>(resNode->id, resStatus));
            break;
        }
        case INNER_EVENT_ID_INIT_GOV_RES_NODE_INFO: {
            auto govResNode = event->GetSharedObject<GovResNode>();
            govResNodeInfo.insert(std::pair<int, std::shared_ptr<GovResNode>>(govResNode->id, govResNode));
            for (int i = 0; i < (int)govResNode->paths.size(); i++) {
                WriteNode(govResNode->paths[i], govResNode->levelToStr[govResNode->def][i]);
            }
            auto resStatus = std::make_shared<ResStatus>(govResNode->def);
            resStatusInfo.insert(std::pair<int, std::shared_ptr<ResStatus>>(govResNode->id, resStatus));
            break;
        }
        case INNER_EVENT_ID_DO_FREQ_ACTION: {
            int resId = event->GetParam();
            if (!IsValidResId(resId)) {
                return;
            }
            std::shared_ptr<ResAction> resAction = event->GetSharedObject<ResAction>();
            UpdateResActionList(resId, resAction, false);
            break;
        }
        case INNER_EVENT_ID_DO_FREQ_ACTION_DELAYED: {
            int resId = event->GetParam();
            if (!IsValidResId(resId)) {
                return;
            }
            std::shared_ptr<ResAction> resAction = event->GetSharedObject<ResAction>();
            UpdateResActionList(resId, resAction, true);
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

void SocPerfHandler::UpdateResActionList(int resId, std::shared_ptr<ResAction> resAction, bool delayed)
{
    std::shared_ptr<ResStatus> resStatus = resStatusInfo[resId];
    int type = resAction->type;

    if (delayed) {
        for (auto iter = resStatus->resActionList[type].begin();
            iter != resStatus->resActionList[type].end(); ++iter) {
            if (resAction->TotalSame(*iter)) {
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

void SocPerfHandler::UpdateCandidatesValue(int resId, int type)
{
    std::shared_ptr<ResStatus> resStatus = resStatusInfo[resId];
    int prev = resStatus->candidates[type];

    if (resStatus->resActionList[type].empty()) {
        resStatus->candidates[type] = INVALID_VALUE;
    } else {
        if (type == ACTION_TYPE_PERF) {
            int res = MIN_INT_VALUE;
            for (auto iter = resStatus->resActionList[type].begin();
                iter != resStatus->resActionList[type].end(); ++iter) {
                res = Max(res, (*iter)->value);
            }
            resStatus->candidates[type] = res;
        } else {
            int res = MAX_INT_VALUE;
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

void SocPerfHandler::ArbitrateCandidate(int resId)
{
    std::shared_ptr<ResStatus> resStatus = resStatusInfo[resId];
    int candidatePerf = resStatus->candidates[ACTION_TYPE_PERF];
    int candidatePower = resStatus->candidates[ACTION_TYPE_POWER];
    int candidateThermal = resStatus->candidates[ACTION_TYPE_THERMAL];

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

void SocPerfHandler::ArbitratePairRes(int resId)
{
    if (IsGovResId(resId)) {
        UpdateCurrentValue(resId, resStatusInfo[resId]->candidate);
        return;
    }

    int pairResId = resNodeInfo[resId]->pair;
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

void SocPerfHandler::UpdatePairResValue(int minResId, int minResValue, int maxResId, int maxResValue)
{
    if (resStatusInfo[minResId]->current != minResValue) {
        WriteNode(resNodeInfo[minResId]->path, std::to_string(resNodeInfo[minResId]->def));
    }
    if (resStatusInfo[maxResId]->current != maxResValue) {
        WriteNode(resNodeInfo[maxResId]->path, std::to_string(resNodeInfo[maxResId]->def));
    }
    UpdateCurrentValue(minResId, minResValue);
    UpdateCurrentValue(maxResId, maxResValue);
}

void SocPerfHandler::UpdateCurrentValue(int resId, int currValue)
{
    if (resStatusInfo[resId]->current != currValue) {
        resStatusInfo[resId]->current = currValue;
        if (IsGovResId(resId)) {
            SOC_PERF_LOGI("govResId->%{public}d, choose->%{public}d", resId, resStatusInfo[resId]->current);
            std::vector<std::string> targetStrs = govResNodeInfo[resId]->levelToStr[resStatusInfo[resId]->current];
            for (int i = 0; i < (int)govResNodeInfo[resId]->paths.size(); i++) {
                WriteNode(govResNodeInfo[resId]->paths[i], targetStrs[i]);
            }
        } else {
            SOC_PERF_LOGI("resId->%{public}d, choose->%{public}d", resId, resStatusInfo[resId]->current);
            WriteNode(resNodeInfo[resId]->path, std::to_string(resStatusInfo[resId]->current));
        }
    }
}

void SocPerfHandler::WriteNode(std::string filePath, std::string value)
{
    char path[PATH_MAX + 1] = {0};
    if (filePath.size() == 0 || filePath.size() > PATH_MAX || realpath(filePath.c_str(), path) == nullptr) {
        return;
    }
    FILE* fd = fopen(path, "w");
    if (fd == nullptr) {
        return;
    }
    fprintf(fd, "%s", value.c_str());
    fclose(fd);
    fd = nullptr;
}

bool SocPerfHandler::ExistNoCandidate(
    int resId, std::shared_ptr<ResStatus> resStatus, int perf, int power, int thermal)
{
    if (perf == INVALID_VALUE && power == INVALID_VALUE && thermal == INVALID_VALUE) {
        if (IsGovResId(resId)) {
            resStatus->candidate = govResNodeInfo[resId]->def;
        } else {
            resStatus->candidate = resNodeInfo[resId]->def;
        }
        ArbitratePairRes(resId);
        return true;
    }
    return false;
}

bool SocPerfHandler::IsGovResId(int resId)
{
    if (resNodeInfo.find(resId) == resNodeInfo.end()
        && govResNodeInfo.find(resId) != govResNodeInfo.end()) {
        return true;
    }
    return false;
}

bool SocPerfHandler::IsValidResId(int resId)
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