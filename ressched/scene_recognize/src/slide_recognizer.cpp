/*
 * Copyright (c) 2024-2025 Huawei Device Co., Ltd.
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
#include "slide_recognizer.h"

#include <mutex>

#include "event_listener_mgr.h"
#include "ffrt_inner.h"
#include "hitrace_meter.h"
#include "plugin_mgr.h"
#include "res_common_util.h"
#include "res_sched_log.h"
#include "res_sched_mgr.h"
#include "res_type.h"
#include "scene_recognizer_mgr.h"

namespace OHOS {
namespace ResourceSchedule {
namespace {
    static const uint32_t AXIS_EVENT_PAD = 0;
    static const uint32_t AXIS_EVENT_FACTOR = 10;
    static const char* AXIS_EVENT_TYPE = "axis_event_type";
    static const char* UP_SPEED_KEY = "up_speed";
    static uint32_t g_slideState = SlideRecognizeStat::IDLE;
    static ffrt::recursive_mutex stateMutex;
    static auto g_reportListFlingLockedEnd = [](const nlohmann::json payload) {
        std::lock_guard<ffrt::recursive_mutex> lock(stateMutex);
        if (g_slideState != SlideRecognizeStat::LIST_FLING) {
            return;
        }
        ResSchedMgr::GetInstance().ReportData(ResType::RES_TYPE_SLIDE_RECOGNIZE,
            ResType::SlideEventStatus::SLIDE_EVENT_OFF, payload);
        nlohmann::json extInfo;
        EventListenerMgr::GetInstance().SendEvent(ResType::EventType::EVENT_DRAW_FRAME_REPORT,
            ResType::EventValue::EVENT_VALUE_DRAW_FRAME_REPORT_STOP, extInfo);
        g_slideState = SlideRecognizeStat::IDLE;
    };
}

SlideRecognizer::SlideRecognizer()
{
    AddAcceptResTypes({
        ResType::RES_TYPE_SLIDE_RECOGNIZE,
        ResType::RES_TYPE_SEND_FRAME_EVENT,
        ResType::RES_TYPE_CLICK_RECOGNIZE,
        ResType::RES_TYPE_AXIS_EVENT,
    });
}

SlideRecognizer::~SlideRecognizer()
{
    RESSCHED_LOGI("~UpdatingSceneRecognizer");
}

void SlideRecognizer::OnDispatchResource(uint32_t resType, int64_t value, const nlohmann::json& payload)
{
    switch (resType) {
        case ResType::RES_TYPE_SLIDE_RECOGNIZE:
            HandleSlideEvent(value, payload);
            break;
        case ResType::RES_TYPE_SEND_FRAME_EVENT:
            HandleSendFrameEvent(FillRealPidAndUid(payload));
            break;
        case ResType::RES_TYPE_CLICK_RECOGNIZE:
            HandleClickEvent(value, payload);
            break;
        case ResType::RES_TYPE_AXIS_EVENT:
            if (value == ResType::AxisEventStatus::AXIS_EVENT_END) {
                HandleClickEvent(ResType::ClickEventType::TOUCH_EVENT_UP, payload);
            }
            break;
        default:
            RESSCHED_LOGD("unkonw resType");
            break;
    }
}

void SlideRecognizer::HandleSlideEvent(int64_t value, const nlohmann::json& payload)
{
    if (value == ResType::SlideEventStatus::SLIDE_EVENT_DETECTING) {
        HandleSlideDetecting(payload);
    } else if (value == ResType::SlideEventStatus::SLIDE_EVENT_ON) {
        HandleListFlingStart(payload);
    } else if (value == ResType::SlideEventStatus::SLIDE_EVENT_OFF) {
        HandleSlideOFFEvent();
    }
}

void SlideRecognizer::HandleSlideOFFEvent()
{
    std::lock_guard<ffrt::recursive_mutex> lock(stateMutex);
    if (listFlingEndTask_) {
        ffrt::skip(listFlingEndTask_);
    }
    if (listFlingTimeOutTask_) {
        ffrt::skip(listFlingTimeOutTask_);
    }
    nlohmann::json extInfo;
    EventListenerMgr::GetInstance().SendEvent(ResType::EventType::EVENT_DRAW_FRAME_REPORT,
        ResType::EventValue::EVENT_VALUE_DRAW_FRAME_REPORT_STOP, extInfo);
    g_slideState = SlideRecognizeStat::IDLE;
}

void SlideRecognizer::HandleSlideDetecting(const nlohmann::json& payload)
{
    if (g_slideState == SlideRecognizeStat::LIST_FLING) {
        if (listFlingEndTask_) {
        ffrt::skip(listFlingEndTask_);
        }
        if (listFlingTimeOutTask_) {
            ffrt::skip(listFlingTimeOutTask_);
        }
        listFlingEndTask_ = nullptr;
        listFlingTimeOutTask_ = nullptr;
        g_reportListFlingLockedEnd(FillRealPidAndUid(payload));
    }
    SceneRecognizerMgr::GetInstance().SubmitTask([this, payload]() {
        StartDetecting(payload);
    });
}

void SlideRecognizer::StartDetecting(const nlohmann::json& payload)
{
    nlohmann::json extInfo;
    EventListenerMgr::GetInstance().SendEvent(ResType::EventType::EVENT_DRAW_FRAME_REPORT,
        ResType::EventValue::EVENT_VALUE_DRAW_FRAME_REPORT_START, extInfo);
    slideDetectingTime_ = ResCommonUtil::GetNowMillTime(true);
    g_slideState = SlideRecognizeStat::SLIDE_NORMAL_DETECTING;
    if (!payload.contains("clientPid") || !payload["clientPid"].is_string()) {
        RESSCHED_LOGE("payload with no clientPid");
        return;
    }
    slidePid_ = payload["clientPid"];
    if (!payload.contains("callingUid") || !payload["callingUid"].is_string()) {
        RESSCHED_LOGE("payload with no callingUid");
        return;
    }
    slideUid_ = payload["callingUid"];
}

void SlideRecognizer::HandleListFlingStart(const nlohmann::json& payload)
{
    std::lock_guard<ffrt::recursive_mutex> lock(stateMutex);
    if (g_slideState == SlideRecognizeStat::LIST_FLING) {
        return;
    }
    nlohmann::json extInfo;
    EventListenerMgr::GetInstance().SendEvent(ResType::EventType::EVENT_DRAW_FRAME_REPORT,
        ResType::EventValue::EVENT_VALUE_DRAW_FRAME_REPORT_START, extInfo);
    g_slideState = SlideRecognizeStat::LIST_FLING;
    if (listFlingEndTask_) {
        ffrt::skip(listFlingEndTask_);
    }
    listFlingEndTask_ = ffrt::submit_h([payload]() {
        g_reportListFlingLockedEnd(payload);
        }, {}, {}, ffrt::task_attr().delay(listFlingEndTime_));
    if (listFlingTimeOutTask_) {
        ffrt::skip(listFlingTimeOutTask_);
    }
    listFlingTimeOutTask_ = ffrt::submit_h([payload]() {
        g_reportListFlingLockedEnd(payload);
        }, {}, {}, ffrt::task_attr().delay(listFlingTimeOutTime_));
}

void SlideRecognizer::HandleSendFrameEvent(const nlohmann::json& payload)
{
    std::lock_guard<ffrt::recursive_mutex> lock(stateMutex);
    if (g_slideState == SlideRecognizeStat::SLIDE_NORMAL_DETECTING) {
        if (isInTouching_) {
            ResSchedMgr::GetInstance().ReportData(ResType::RES_TYPE_SLIDE_RECOGNIZE,
                ResType::SlideEventStatus::SLIDE_NORMAL_BEGIN, payload);
            g_slideState = SlideRecognizeStat::SLIDE_NORMAL;
        } else {
            g_slideState = SlideRecognizeStat::IDLE;
        }
        nlohmann::json extInfo;
        EventListenerMgr::GetInstance().SendEvent(ResType::EventType::EVENT_DRAW_FRAME_REPORT,
            ResType::EventValue::EVENT_VALUE_DRAW_FRAME_REPORT_STOP, extInfo);
    } else if (g_slideState == SlideRecognizeStat::LIST_FLING) {
        if (listFlingEndTask_) {
            ffrt::skip(listFlingEndTask_);
        }
        listFlingEndTask_ = ffrt::submit_h([payload]() {
            g_reportListFlingLockedEnd(payload);
            }, {}, {}, ffrt::task_attr().delay(listFlingEndTime_));
    }
}

void SlideRecognizer::HandleClickEvent(int64_t value, const nlohmann::json& payload)
{
    std::lock_guard<ffrt::recursive_mutex> lock(stateMutex);
    if (value == ResType::ClickEventType::TOUCH_EVENT_DOWN) {
        g_slideState = SlideRecognizeStat::IDLE;
        isInTouching_ = true;
    } else if (value == ResType::ClickEventType::TOUCH_EVENT_UP ||
        value == ResType::ClickEventType::TOUCH_EVENT_PULL_UP) {
        isInTouching_ = false;
    }
    //not in slide stat or slide detecting stat
    if (g_slideState != SlideRecognizeStat::SLIDE_NORMAL &&
        g_slideState != SlideRecognizeStat::SLIDE_NORMAL_DETECTING) {
        return;
    }
    // receive up event, silde normal end
    if (value == ResType::ClickEventType::TOUCH_EVENT_UP ||
        value == ResType::ClickEventType::TOUCH_EVENT_PULL_UP) {
        if (g_slideState != SlideRecognizeStat::SLIDE_NORMAL_DETECTING) {
            ResSchedMgr::GetInstance().ReportData(ResType::RES_TYPE_SLIDE_RECOGNIZE,
                ResType::SlideEventStatus::SLIDE_NORMAL_END, payload);
        }
        float upSpeed = 0.0;
        if (!payload.contains("clientPid") || !payload["clientPid"].is_string()) {
            RESSCHED_LOGE("payload with no clientPid");
            return;
        }
        if (!payload.contains(UP_SPEED_KEY) || !payload[UP_SPEED_KEY].is_string()) {
            return;
        }
        if (!ResCommonUtil::StrToFloat(payload[UP_SPEED_KEY], upSpeed)) {
            return;
        }
        std::string trace_str("TOUCH EVENT UPSPEED: ");
        trace_str.append(std::to_string(upSpeed));
        StartTrace(HITRACE_TAG_APP, trace_str, -1);
        FinishTrace(HITRACE_TAG_APP);
        if (payload.contains(AXIS_EVENT_TYPE) && payload[AXIS_EVENT_TYPE].is_string() &&
            payload[AXIS_EVENT_TYPE] == AXIS_EVENT_TYPE) {
            upSpeed = upSpeed * AXIS_EVENT_FACTOR;
        }
        // if up speed large than LIST_FLING_SPEED_LIMIT,start recognize list fling.
        if (upSpeed > listFlingSpeedLimit_) {
            nlohmann::json extInfo;
            EventListenerMgr::GetInstance().SendEvent(ResType::EventType::EVENT_DRAW_FRAME_REPORT,
                ResType::EventValue::EVENT_VALUE_DRAW_FRAME_REPORT_START, extInfo);
            ResSchedMgr::GetInstance().ReportData(ResType::RES_TYPE_SLIDE_RECOGNIZE,
                ResType::SlideEventStatus::SLIDE_EVENT_ON, FillRealPidAndUid(payload));
            g_slideState = SlideRecognizeStat::LIST_FLING;
        }
    }
}

nlohmann::json SlideRecognizer::FillRealPidAndUid(const nlohmann::json& payload)
{
    nlohmann::json payloadM = payload;
    if (!slidePid_.empty()) {
        payloadM["clientPid"] = slidePid_;
    }
    if (!slideUid_.empty()) {
        payloadM["callingUid"] = slideUid_;
    }
    return payloadM;
}

void SlideRecognizer::SetListFlingTimeoutTime(int64_t value)
{
    listFlingTimeOutTime_ = value;
}

void SlideRecognizer::SetListFlingEndTime(int64_t value)
{
    listFlingEndTime_ = value;
}

void SlideRecognizer::SetListFlingSpeedLimit(float value)
{
    listFlingSpeedLimit_ = value;
}
} // namespace ResourceSchedule
} // namespace OHOS