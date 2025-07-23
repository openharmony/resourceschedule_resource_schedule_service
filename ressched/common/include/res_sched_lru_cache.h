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

#ifndef RESSCHED_COMMON_INCLUDE_LRU_CACHE_H
#define RESSCHED_COMMON_INCLUDE_LRU_CACHE_H

#include <iostream>
#include <list>
#include <unordered_map>
#include <stdexcept>

namespace OHOS {
namespace ResourceSchedule {
template <typename K, typename V>
class ResschedLRUCache {
private:
    size_t capacity_;
    std::list<std::pair<K, V>> lruList;
    std::unordered_map<K, typename std::list<std::pair<K, V>>::iterator> relationMap;
public:
    ResschedLRUCache(size_t capacity = 256) : capacity_(capacity) {}
    bool get(const K& key, V& value)
    {
        if (relationMap.find(key) == relationMap.end()) {
            return false;
        }
        lruList.splice(lruList.begin(), lruList, relationMap[key]);
        value = relationMap[key]->second;
        return true;
    }
    void put(const K& key, const V& value)
    {
        if (relationMap.find(key) != relationMap.end()) {
            if (relationMap[key] != lruList.begin()) {
                lruList.erase(relationMap[key]);
                lruList.push_front(std::make_pair(key, value));
                relationMap[key] = lruList.begin();
            } else {
                relationMap[key]->second = value;
            }
        } else {
            if (lruList.size() >= capacity_) {
                relationMap.erase(lruList.back().first);
                lruList.pop_back();
            }
            lruList.push_front(std::make_pair(key, value));
            relationMap[key] = lruList.begin();
        }
    }
};
} // namespace ResourceSchedule
} // namespace OHOS
#endif // RESSCHED_COMMON_INCLUDE_RES_JSON_TYPE_H