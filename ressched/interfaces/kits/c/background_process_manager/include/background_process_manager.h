/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

/**
 * @addtogroup BackgroundProcessManager
 * @{
 *
 * @brief BackgroundProcessManager provides APIs.
 *
 * @since 17
 */

/**
 * @file background_process_manager.h
 *
 * @brief Declares the BackgroundProcessManager interfaces in C.
 *
 * BackgroundProcessManager refers to set or reset priority of process
 *
 * @library libbackground_process_manager.z.so
 * @kit BackgroundTasksKit
 * @syscap SystemCapability.Resourceschedule.BackgroundProcessManager
 * @since 17
 */

#ifndef RESOURCESCHEDULE_BACKGROUND_PROCESS_MANAGER_H
#define RESOURCESCHEDULE_BACKGROUND_PROCESS_MANAGER_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Describes the level of BackgroundProcessManager priority.
 *
 * @since 17
 */
typedef enum BackgroundProcessManager_ProcessPriority {
    /**
     * @brief Means the process has stopped working and in the background
     */
    PROCESS_BACKGROUND = 1,

    /**
     * @brief Means the process is working in the background
     */
    PROCESS_INACTIVE = 2,
} BackgroundProcessManager_ProcessPriority;

/**
 * @brief Description the status of the power saving mode.
 *
 * @since 20
 */
typedef enum BackgroundProcessManager_PowerSaveMode {
    /**
     * @brief Means the process request not to entry power saving mode
     * This setting may be overridden by settings in Task Manager
     */
    EFFICIENCY_MODE = 1,

    /**
     * @brief Means the process operating mode follows the system and may entry power saving mode
     */
    DEFAULT_MODE = 2,
} BackgroundProcessManager_PowerSaveMode;

/**
 * @brief Enum for BackgroundProcessManager error code.
 *
 * @since 17
 */
typedef enum BackgroundProcessManager_ErrorCode {
    /**
     * @error result is OK.
     */
    ERR_BACKGROUND_PROCESS_MANAGER_SUCCESS = 0,

    /**
     * @error Permission denied.
     */
    ERR_BACKGROUND_PROCESS_MANAGER_PERMISSION_DENIED = 201,

    /**
     * @error invalid parameter. Possible causes:
     * 1. priority is out of range.
     */
    ERR_BACKGROUND_PROCESS_MANAGER_INVALID_PARAM = 401,

    /**
     * @error Capability not supported.
     */
    ERR_BACKGROUND_PROCESS_MANAGER_CAPABILITY_NOT_SUPPORTED = 801,

    /**
     * @error remote error. Possible causes:
     * 1. remote is not work.
     */
    ERR_BACKGROUND_PROCESS_MANAGER_REMOTE_ERROR = 31800001,

    /**
     * @error Parameter error. Possible causes:
     * 1. Mandatory parameters are left unspecified;
     * 2. Incorrect parameter types;
     * 3. PowerSaveMode status is out of range.
     */
    ERR_BACKGROUND_PROCESS_MANAGER_PARAMETER_ERROR = 31800002,

    /**
     * @error Setup erro. This setting is overridden by setting in Task Manager.
     */
    ERR_BACKGROUND_PROCESS_MANAGER_SETUP_ERROR = 31800003,

    /**
     * @error The setting failed due to system scheduling reasons.
     */
    ERR_BACKGROUND_PROCESS_MANAGER_SYSTEM_SCHEDULING = 31800004,
} BackgroundProcessManager_ErrorCode;

/**
 * @brief Set the priority of process.
 *
 * @param pid Indicates the pid of the process to be set.
 * @param priority Indicates the priority to be set.
          Specific priority can be referenced {@link BackgroundProcessManager_ProcessPriority}.
 * @return {@link ERR_BACKGROUND_PROCESS_MANAGER_SUCCESS} 0 - Success.
 *         {@link ERR_BACKGROUND_PROCESS_MANAGER_INVALID_PARAM} 401 - Parameter error.
 * @since 17
 */
int OH_BackgroundProcessManager_SetProcessPriority(int pid, BackgroundProcessManager_ProcessPriority priority);

/**
 * @brief Reset the priority of process.
 *
 * @param pid Indicates the pid of the process to be reset.
 * @return {@link ERR_BACKGROUND_PROCESS_MANAGER_SUCCESS} 0 - Success.
 * @since 17
 */
int OH_BackgroundProcessManager_ResetProcessPriority(int pid);

/**
 * @brief Set the power saving mode of process. The setting may fail due to user setting
 * reasons or system scheduling reasons.
 * @param pid Indicates the pid of the power saving mode to be set.
 * @param powerSaveMode Indicates the power saving mode that needs to be set.
 * For details, please refer to BackgroundProcessManager_PowerSaveMode
 * @return {@link ERR_BACKGROUND_PROCESS_MANAGER_SUCCESS} 0 - Success.
 * @return {@link ERR_BACKGROUND_PROCESS_MANAGER_PERMISSION_DENIED} 201 - Permission denied.
 * @return {@link ERR_BACKGROUND_PROCESS_MANAGER_CAPABILITY_NOT_SUPPORTED} 801 - Capability not supported.
 * @return {@link ERR_BACKGROUND_PROCESS_MANAGER_PARAMETER_ERROR} 31800002 - Parameter error.
 * @return {@link ERR_BACKGROUND_PROCESS_MANAGER_SETUP_ERROR} 31800003 - Setup error.
 * @since 20
 */
int OH_BackgroundProcessManager_SetPowerSaveMode(int pid, BackgroundProcessManager_PowerSaveMode powerSaveMode);

/**
 * @brief Check is the process is in power saving mode.
 *
 * @param pid Indicates the process to be checked is the pid of the power saving mode.
 * @return {@link ERR_BACKGROUND_PROCESS_MANAGER_SUCCESS} 0 - Success.
 * @return {@link ERR_BACKGROUND_PROCESS_MANAGER_PERMISSION_DENIED} 201 - Permission denied.
 * @return {@link ERR_BACKGROUND_PROCESS_MANAGER_CAPABILITY_NOT_SUPPORTED} 801 - Capability not supported.
 * @return {@link ERR_BACKGROUND_PROCESS_MANAGER_PARAMETER_ERROR} 31800002 - Parameter error.
 * @since 20
 */
int OH_BackgroundProcessManager_IsPowerSaveMode(int pid);
#ifdef __cplusplus
};
#endif
#endif // RESOURCESCHEDULE_BACKGROUND_PROCESS_MANAGER_H
/** @} */
