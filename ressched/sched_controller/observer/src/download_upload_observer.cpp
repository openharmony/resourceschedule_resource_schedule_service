/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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
#ifdef RESOURCE_REQUEST_REQUEST
#include "download_upload_observer.h"
#include "nlohmann/json.hpp"
#include "res_sched_log.h"
#include "res_sched_mgr.h"
#include "res_type.h"
namespace OHOS {
namespace ResourceSchedule {
void DownLoadUploadObserver::OnRunningTaskCountUpdate(int count)
{
    RESSCHED_LOGD("download upload on running task count %{public}d", count);
    if (IsReporEnterScene(count)) {
        HandleEnterScene();
        return;
    }
    if (IsReporExitScene(count)) {
        HandleExitScene();
    }
}

bool DownLoadUploadObserver::IsReporEnterScene(int count)
{
    if (count > 0 && !isReportedScene_) {
        return true;
    }
    return false;
}

bool DownLoadUploadObserver::IsReporExitScene(int count)
{
    if (count == 0 && isReportedScene_) {
        return true;
    }
    return false;
}

void DownLoadUploadObserver::HandleEnterScene()
{
    const nlohmann::json payload = nlohmann::json::object();
    ResSchedMgr::GetInstance().ReportData(ResType::RES_TYPE_UPLOAD_DOWNLOAD,
        ResType::KeyUploadOrDownloadStatus::ENTER_UPLOAD_DOWNLOAD_SCENE, payload);
    isReportedScene_ = true;
}

void DownLoadUploadObserver::HandleExitScene()
{
    const nlohmann::json payload = nlohmann::json::object();
    ResSchedMgr::GetInstance().ReportData(ResType::RES_TYPE_UPLOAD_DOWNLOAD,
        ResType::KeyUploadOrDownloadStatus::EXIT_UPLOAD_DOWNLOAD_SCENE, payload);
    isReportedScene_ = false;
}
} // namespace ResourceSchedule
} // namespace OHOS
#endif
