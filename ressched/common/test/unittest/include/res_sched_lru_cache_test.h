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


#ifndef LRU_CACHE_TEST
#define LRU_CACHE_TEST

#include "gtest/gtest.h"
#include "res_sched_lru_cache.h"

namespace OHOS {
namespace ResourceSchedule {
class ResschedLRUCacheTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
private:
    ResschedLRUCache<int, std::string>* cache;
};
} // namespace ResourceSchedule
} // namespace OHOS

#endif // LRU_CACHE_TEST
