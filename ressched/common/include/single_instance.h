/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: define of single instance. 
 */

#ifndef FOUNDATION_RESOURCESCHEDULE_COMMON_INCLUDE_SINGLE_INSTANCE_H
#define FOUNDATION_RESOURCESCHEDULE_COMMON_INCLUDE_SINGLE_INSTANCE_H

namespace OHOS {
namespace ResourceSchedule {
#define DECLARE_SINGLE_INSTANCE_BASE(className)       \
public:                                               \
    static className& GetInstance();                  \
private:                                              \
    className(const className&) = delete;             \
    className& operator=(const className &) = delete; \
    className(className&&) = delete;                  \
    className& operator=(className&&) = delete;       \


#define DECLARE_SINGLE_INSTANCE(className)   \
    DECLARE_SINGLE_INSTANCE_BASE(className)  \
private:                                     \
    className() = default;                   \
    ~className() = default;                  \

#define IMPLEMENT_SINGLE_INSTANCE(className) \
className& className::GetInstance()          \
{                                            \
    static auto instance = new className();  \
    return *instance;                        \
}
}; // namespace ResourceSchedule
}; // namespace OHOS

#endif // FOUNDATION_RESOURCESCHEDULE_COMMON_INCLUDE_SINGLE_INSTANCE_H