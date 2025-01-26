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

#include "res_sched_time_util.h"

namespace OHOS {
namespace ResourceSchedule {
namespace ResCommonUtil {
namespace {
    constexpr int32_t MAX_TIME_STR_LEN = 64;
    constexpr int32_t SEC_TO_MILLISEC = 1000;
    constexpr int32_t MILLISEC_LENGTH = 3;
    constexpr int32_t MAX_FORMAT_BUFF = 32;
}
int64_t GetNowSecTime(bool steady)
{
    std::chrono::seconds secs;
    if (steady) {
        // get steady time, it will not change whe system time change.
        secs = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::steady_clock::now().time_since_epoch());
    } else {
        // get system time, it can change.
        secs = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now().time_since_epoch());
    }
    return static_cast<int64_t>(secs.count());
}

int64_t GetNowMillTime(bool steady)
{
    std::chrono::milliseconds millSecs;
    if (steady) {
        // get steady time, it will not change whe system time change.
        millSecs = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch());
    } else {
        // get system time, it can change.
        millSecs = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch());
    }
    return static_cast<int64_t>(millSecs.count());
}

int64_t GetNowMicroTime(bool steady)
{
    std::chrono::microseconds microSecs;
    if (steady) {
        // get steady time, it will not change whe system time change.
        microSecs = std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::steady_clock::now().time_since_epoch());
    } else {
        // get system time, it can change.
        microSecs = std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::system_clock::now().time_since_epoch());
    }
    return static_cast<int64_t>(microSecs.count());
}

int64_t GetCurrentTimestamp()
{
    // get current time,precision is ms + us
    struct timeval currentTime;
    gettimeofday(&currentTime, nullptr);
    return static_cast<int64_t>(currentTime.tv_sec) * SEC_TO_MILLISEC + currentTime.tv_usec /SEC_TO_MILLISEC;
}

std::string ConvertTimestampToStr(int64_t timestamp)
{
    char timeStr[MAX_TIME_STR_LEN];
    //timestamp is in millsecond unit, divide 1000 to second
    auto t = static_cast<std::time_t>(timestamp / SEC_TO_MILLISEC);
    auto local = std::localtime(&t);
    if (!local) {
        return "";
    }
    std::strftime(timeStr, MAX_TIME_STR_LEN, "%Y-%m-%d %H-%M-%S", local);
    std::stringstream oss;
    //milliseconds in timeStr should be 3 characters length
    oss << timeStr << "." << std::setw(MILLISEC_LENGTH) << std::setfill('0') << (timestamp % SEC_TO_MILLISEC);
    return oss.str();
}

int64_t GetNextMillTimeStamp(const int64_t time, bool steady)
{
    return GetNowMillTime(steady) + time;
}

std::string FormatSeconds(const int64_t seconds, const std::string& format)
{
    struct tm* time = std::localtime(&seconds);
    if (time == nullptr) {
        return "";
    }

    char result[MAX_FORMAT_BUFF] = {0};
    if (strftime(result, MAX_FORMAT_BUFF, format.c_str(), time) == 0) {
        return "";
    }
    return std::string(result);
}

std::string FormatTimeWithMillis(const int64_t millis)
{
    if (millis <= 0) {
        return "";
    }
    auto tp = std::chrono::system_clock::from_time_t(0) + std::chrono::milliseconds(millis);
    std::ostringstream oss;
    auto time_t = std::chrono::system_clock::to_time_t(tp);
    auto tm = std::localtime(&time_t);
    if (tm == nullptr) {
        return "";
    }
    oss << std::put_time(tm, "%Y-%m-%d %H:%M:%S");
    auto duration = tp.time_since_epoch();
    auto subsec = std::chrono::duration_cast<std::chrono::milliseconds>(duration) % std::chrono::seconds(1);
    oss << "." << std::setfill('0') << std::setw(MILLISEC_LENGTH) << (subsec.count() / SEC_TO_MILLISEC);
    return oss.str();
}
}
} // namespace ResourceSchedule
} // namespace OHOS
