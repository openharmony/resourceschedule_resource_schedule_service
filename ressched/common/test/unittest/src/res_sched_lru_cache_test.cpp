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

#include "res_sched_lru_cache_test.h"

using namespace std;
using namespace testing::ext;

namespace OHOS {
namespace ResourceSchedule {

void ResschedLRUCacheTest::SetUpTestCase() {}

void ResschedLRUCacheTest::TearDownTestCase() {}

void ResschedLRUCacheTest::SetUp()
{
    static const int maxSize = 2;
    cache = new ResschedLRUCache<int, std::string>(maxSize);
}

void ResschedLRUCacheTest::TearDown()
{
    delete cache;
}

/**
 * @tc.name: ResschedLRUCacheTest BasicPutGet
 * @tc.desc: test get
 * @tc.type: FUNC
 * @tc.require: issueICMIEN
 */
HWTEST_F(ResschedLRUCacheTest, BasicPutGet, Function | MediumTest | Level0)
{
    cache->put(1, "one");
    std::string val = "";
    EXPECT_TRUE(cache->get(1, val));
    EXPECT_EQ(val, "one");
}

/**
 * @tc.name: ResschedLRUCacheTest CacheEviction
 * @tc.desc: test CacheEviction
 * @tc.type: FUNC
 * @tc.require: issueICMIEN
 */
HWTEST_F(ResschedLRUCacheTest, CacheEviction, Function | MediumTest | Level0)
{
    cache->put(1, "one");
    cache->put(2, "two");
    cache->put(3, "three");
    std::string val = "";
    EXPECT_FALSE(cache->get(1, val));
    EXPECT_TRUE(cache->get(2, val));
    EXPECT_TRUE(cache->get(3, val));
}

/**
 * @tc.name: ResschedLRUCacheTest UpdateExistingKey
 * @tc.desc: test UpdateExistingKey
 * @tc.type: FUNC
 * @tc.require: issueICMIEN
 */
HWTEST_F(ResschedLRUCacheTest, UpdateExistingKey, Function | MediumTest | Level0)
{
    cache->put(1, "one");
    cache->put(1, "new_one");
    std::string val = "";
    EXPECT_TRUE(cache->get(1, val));
    EXPECT_EQ(val, "new_one");
}

/**
 * @tc.name: ResschedLRUCacheTest GetUpdatesLRU
 * @tc.desc: test GetUpdatesLRU
 * @tc.type: FUNC
 * @tc.require: issueICMIEN
 */
HWTEST_F(ResschedLRUCacheTest, GetUpdatesLRU, Function | MediumTest | Level0)
{
    cache->put(1, "one");
    cache->put(2, "two");
    std::string val = "";
    cache->get(1, val);
    cache->put(3, "three");
    EXPECT_TRUE(cache->get(1, val));
    EXPECT_FALSE(cache->get(2, val));
    EXPECT_TRUE(cache->get(3, val));
}
} // namespace ResourceSchedule
} // namespace OHOS
