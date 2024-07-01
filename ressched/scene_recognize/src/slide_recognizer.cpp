/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
 #include <atomic>

#include "slide_recognizer.h"
#include "ffrt_inner.h"
#include "plugin_mgr.h"
#include "res_sched_log.h"
#include "res_sched_mgr.h"
#include "res_type.h"
#include "res_common_utils.h"
#include "event_listener_mgr.h"
#include "res_sched_service_utils.h"


namespace OHOS {
namespace ResourceSchedule {
namespace {
    static constexpr int64_t LIST_FLINT_TIME_OUT_TIME = 3 * 1000 * 1000;
    static constexpr int64_t LIST_FLINT_END_TIME = 300 * 1000;
    static constexpr float LIST_FLING_SPEED_LIMIT = 10.0;
    static constexpr int64_t SLIDE_NORMAL_DETECTING_TIME = 15;
    static const std::string UP_SPEED_KEY = "up_speed";
    static bool isInListFlingMode = false;
    static int64_t lastTouchUpTime = 0;
    static ffrt::mutex listFlingMutex;
    static auto reportListFlingLockedEnd = [](const nlohmann::json payload){
        listFlingMutex.lock();
        if (!isInListFlingMode) {
            listFlingMutex.unlock();
            return;
        }
        ResSchedMgr::GetInstance().ReportData(ResType::RES_TYPE_SLIDE_RECOGNIZE,
            ResType::SlideEventStatus::SLIDE_EVENT_OFF, payload);
        nlohmann::json extInfo;
        EventListenerMgr::GetInstance().SendEvent(ResType::EventType::EVENT_DRAW_FRAME_REPORT,
            ResType::EventValue::EVENT_VALUE_DRAW_FRAME_REPORT_STOP,extInfo);
        isInListFlingMode = false;
        listFlingMutex.unlock();
    };
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
            HandleSendFrameEvent(payload);
            break;
        case ResType::RES_TYPE_CLICK_RECOGNIZE:
            HandleClickEvent(value, payload);
        default:
            break;
    }
}

void SlideRecognizer::HandleSlideEvent(int64_t value, const nlohmann::json& payload)
{
    if (value == ResType::SlideEventStatus::SLIDE_EVENT_DETECTING) {
        HandleSlideDetecting(payload);
    } else if (value == ResType::SlideEventStatus::SLIDE_EVENT_ON) {
        HandleListFlingStart(payload);
    }
}

void SlideRecognizer::HandleSlideDetecting(const nlohmann::json& payload)
{
    nlohmann::json extInfo;
    EventListenerMgr::GetInstance().SendEvent(ResType::EventType::EVENT_DRAW_FRAME_REPORT,
        ResType::EventValue::EVENT_VALUE_DRAW_FRAME_REPORT_START, extInfo);
    slideDetectingTime_ = ResSchedUtils::GetNowMillTime();
    if (state_ == SlideRecognizeStat::LIST_FLING) {
        if(listFlingEndTask_) {
        ffrt::skip(listFlingEndTask_);
        }
        if (listFlingTimeOutTask_) {
            ffrt::skip(listFlingTimeOutTask_);
        }
        listFlingEndTask_ = nullptr;
        listFlingTimeOutTask_ = nullptr;
        reportListFlingLockedEnd(payload);
    }
    state_ = SlideRecognizeStat::SLIDE_NORMAL_DETECTING;
}

void SlideRecognizer::HandleListFlingStart(const nlohmann::json& payload)
{
    listFlingMutex.lock();
    if (isInListFlingMode) {
        listFlingMutex.unlock();
        return;
    }
    nlohmann::json extInfo;
    EventListenerMgr::GetInstance().SendEvent(ResType::EventType::EVENT_DRAW_FRAME_REPORT,
        ResType::EventValue::EVENT_VALUE_DRAW_FRAME_REPORT_START, extInfo);
    ResSchedMgr::GetInstance().ReportData(ResType::RES_TYPE_SLIDE_RECOGNIZE,
        ResType::SlideEventStatus::SLIDE_EVENT_ON, payload);
    isInListFlingMode = true;
    listFlingMutex.unlock();
    if (listFlingEndTask_) {
        ffrt::skip(listFlingEndTask_);
    }
    listFlingEndTask_ = ffrt::submit_h([payload]() {
        reportListFlingLockedEnd(payload);
        }, {}, {}, ffrt::task_attr().delay(LIST_FLINT_END_TIME));
    if (listFlingTimeOutTask_) {
        ffrt::skip(listFlingTimeOutTask_);
    }
    listFlingTimeOutTask_ = ffrt::submit_h([payload]() {
        reportListFlingLockedEnd(payload);
        }, {}, {}, ffrt::task_attr().delay(LIST_FLINT_TIME_OUT_TIME));
}

void SlideRecognizer::HandleSendFrameEvent(const nlohmann::json& payload) 
{
    if (state_ == SlideRecognizeStat::SLIDE_NORMAL_DETECTING) {
        int64_t nowTime = ResSchedUtils::GetNowMillTime();
        if (nowTime - slideDetectingTime_ < SLIDE_NORMAL_DETECTING_TIME) {
            nlohmann::json extInfo;
            EventListenerMgr::GetInstance().SendEvent(ResType::EventType::EVENT_DRAW_FRAME_REPORT,
                ResType::EventValue::EVENT_VALUE_DRAW_FRAME_REPORT_STOP, extInfo);
            ResSchedMgr::GetInstance().ReportData(ResType::RES_TYPE_SLIDE_RECOGNIZE,
                ResType::SlideEventStatus::SLIDE_NORMAL_BEGIN, payload);
            state_ = SlideRecognizeStat::SLIDE_NORMAL;
        }
    } else if (state_ == SlideRecognizeStat::LIST_FLING) {
        if (listFlingEndTask_) {
            ffrt::skip(listFlingEndTask_);
        }
        listFlingEndTask_ = ffrt::submit_h([payload]() {
            reportListFlingLockedEnd(payload);
            }, {}, {}, ffrt::task_attr().delay(LIST_FLINT_END_TIME));
    }
}

void SlideRecognizer::HandleClickEvent(int64_t value, const nlohmann::json& payload)
{
    //not in slide stat
    if (state_ != SlideRecognizeStat::SLIDE_NORMAL) {
        return;
    }
    // receive up event, silde normal end
    if (value == ResType::ClickEventType::TOUCH_EVENT_UP ||
        value == ResType::ClickEventType::TOUCH_EVENT_PULL_UP) {
        ResSchedMgr::GetInstance().ReportData(ResType::RES_TYPE_SLIDE_RECOGNIZE,
            ResType::SlideEventStatus::SLIDE_NORMAL_END, payload);
        float upSpeed = 0.0;
        if (!payload.contains(UP_SPEED_KEY) || !payload[UP_SPEED_KEY].is_string()){
            return;
        }
        // if up speed large than LIST_FLING_SPEED_LIMIT,start recognize list fling.
        if(ResCommonUtil::StrToFloat(payload[UP_SPEED_KEY], upSpeed) && upSpeed > LIST_FLING_SPEED_LIMIT)
        {
            nlohmann::json extInfo;
            EventListenerMgr::GetInstance().SendEvent(ResType::EventType::EVENT_DRAW_FRAME_REPORT,
                ResType::EventValue::EVENT_VALUE_DRAW_FRAME_REPORT_START, extInfo);
            ResSchedMgr::GetInstance().ReportData(ResType::RES_TYPE_SLIDE_RECOGNIZE,
                ResType::SlideEventStatus::SLIDE_EVENT_ON, payload);
            state_ = SlideRecognizeStat::LIST_FLING;
        }
    }
}
} // namespace ResourceSchedule
} // namespace OHOS