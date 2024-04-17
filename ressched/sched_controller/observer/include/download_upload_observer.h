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
#ifndef RESSCHED_SCHED_CONTROLLER_OBSERVER_INCLUDE_DOWNLOAD_UPLOAD_OBSERVER_H
#define RESSCHED_SCHED_CONTROLLER_OBSERVER_INCLUDE_DOWNLOAD_UPLOAD_OBSERVER_H
#ifndef RESOURCE_REQUEST_REQUEST
#include "running_task_count.h"
namespace OHOS {
namespace ResourceSchedule {
class DownLoadUploadObserver : public Request::IRunningTaskObserver {
public:
    DownLoadUploadObserver() = default;
    ~DownLoadUploadObserver() = default;
    virtual void OnRunningTaskCountUpdate(int count) override;
private:
    bool isReportScene = false;
};
} // namespace ResourceSchedule
} // namespace OHOS
#endif
#endif // RESSCHED_SCHED_CONTROLLER_OBSERVER_INCLUDE_DOWNLOAD_UPLOAD_OBSERVER_H