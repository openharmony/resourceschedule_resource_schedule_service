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

#include "list_fling_recognizer.h"
#include "ffrt_inner.h"
#include "plugin_mgr.h"
#include "res_sched_log.h"
#include "res_sched_mgr.h"
#include "res_type.h"
#include "event_listener_mgr.h"


namespace OHOS {
namespace ResourceSchedule {
namespace {
    static constexpr int64_t LIST_FLINT_TIME_OUT_TIME = 3 * 1000 * 1000;
    static constexpr int64_t LIST_FLINT_END_TIME = 100 * 1000;
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
    }
}

ListFlingRecognizer::~ListFlingRecognizer()
{
    RESSCHED_LOGI("~UpdatingSceneRecognizer");
}

void ListFlingRecognizer::OnDispatchResource(uint32_t resType, int64_t value, const nlohmann::json& payload)
{
    switch (resType) {
        case ResType::RES_TYPE_SLIDE_RECOGNIZE:
            HandleSlideEvent(value, payload);
            break;
        case ResType::RES_TYPE_SEND_FRAME_EVENT:
            HandleSendFrameEvent(value, payload);
            break;
        default:
            break;
    }
}

void ListFlingRecognizer::HandleSlideEvent(int64_t value, const nlohmann::json& payload) 
{
    if (value == ResType::SlideEventStatus::SLIDE_NORMAL_BEGIN) {
        if (isInListFlingMode = false) {
            return;
        }
        if(listFlingEndTask_) {
            ffrt::skip(listFlingEndTask_);
        }
        listFlingEndTask_ = nullptr;
        listFlingTimeOutTask_ = nullptr;
        reportListFlingLockedEnd(payload);
        return;
    }
    if (value == ResType::SlideEventStatus::SLIDE_NORMAL_END) {
        listFlingMutex.lock();
        EventListenerMgr::GetInstance().SendEvent(ResType::EventType::EVENT_DRAW_FRAME_REPORT,
            ResType::EventValue::EVENT_VALUE_DRAW_FRAME_REPORT_START,extInfo);
        ResSchedMgr::GetInstance().ReportData(ResType::RES_TYPE_SLIDE_RECOGNIZE,
            ResType::SlideEventStatus::SLIDE_EVENT_ON, payload);
        isInListFlingMode = true;
        listFlingMutex.unlock();
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
}

void ListFlingRecognizer::HandleSendFrameEvent() 
{
    if (listFlingEndTask_) {
        ffrt::skip(listFlingEndTask_);
    }
    listFlingEndTask_ = ffrt::submit_h([payload]() {
        reportListFlingLockedEnd(payload);
        }, {}, {}, ffrt::task_attr().delay(LIST_FLINT_END_TIME));
}
} // namespace ResourceSchedule
} // namespace OHOS