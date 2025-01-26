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

#ifndef RESSCHED_COMMON_INCLUDE_RES_SCHED_TIME_UTIL
#define RESSCHED_COMMON_INCLUDE_RES_SCHED_TIME_UTIL
#include <iomanip>
#include <sstream>
#include <string>
#include <sys/time.h>

const char* const DATE_FORMAT = "%Y-%m-%d";

namespace OHOS {
namespace ResourceSchedule {
namespace ResCommonUtil {

/**
 * @brief Obtain current seconds timestamp
 *
 * @param steady true if time will not affected by the system clock.
 * @return Returns current second timestamp;
 */
int64_t GetNowSecTime(bool steady = false);

/**
 * @brief Obtain current milliseconds timestamp
 *
 * @param steady true if time will not affected by the system clock.
 * @return Returns current millsecond timestamp;
 */
int64_t GetNowMillTime(bool steady = false);

/**
 * @brief Obtain current microseconds timestamp.
 *
 * @param steady true if time will not affected by the system clock.
 * @return Returns current microsecond timestamp;
 */
int64_t GetNowMicroTime(bool steady = false);

/**
 * @brief Obtain current milliseconds system clock timestamp.
 *
 * @return Returns current millsecond timestamp;
 */
int64_t GetCurrentTimestamp();

/**
 * @brief Converts given timestamp to string.
 *
 * @param timestamp Indicates the string to be converted
 * @return Returns current millsecond timestamp;
 */
std::string ConvertTimestampToStr(int64_t timestamp);

/**
 * @brief Calculate the timestamp of the current time after the duration.
 *
 * @param duration the duration has passed.
 * @return Returns the next timestamp after duration;
 */
int64_t GetNextMillTimeStamp(const int64_t duration, bool steady = false);

/**
 * @brief format input seconds to date.
 *
 * @param second the input second time.
 * @param format the date format
 * @return Returns the date of sconds format;
 */
std::string FormatSeconds(const int64_t seconds, const std::string& format);

/**
 * @brief format input millis timestamp to %Y-%m-%H-%M-ms.
 *
 * @param millis the input millisecond timestamp.
 * @return Returns the string of date;
 */
std::string FormatTimeWithMillis(const int64_t millis);
}

} // namespace ResourceSchedule
} // namespace OHOS

#endif // RESSCHED_COMMON_INCLUDE_RES_SCHED_TIME_UTIL