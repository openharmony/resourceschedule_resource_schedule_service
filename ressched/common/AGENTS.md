# 资源调度服务 - 公共模块

本文件为 Claude Code 提供 RSS 公共模块的开发指导。

---

## 快速参考

### 编译命令

```bash
# 编译公共工具库
hb build ressched/common:ressched_common_utils

# 编译单元测试
hb build ressched/common/test/unittest:unittest

# 编译特定单元测试
hb build ressched/common/test/unittest:resschedstringutil_test
hb build ressched/common/test/unittest:resschedtimeutil_test
hb build ressched/common/test/unittest:resschedfileutil_test
hb build ressched/common/test/unittest:lrucache_test
```

### 常用工具头文件

```cpp
#include "res_sched_log.h"           // 日志
#include "res_sched_string_util.h"   // 字符串工具
#include "res_sched_time_util.h"     // 时间工具
#include "res_sched_lru_cache.h"     // LRU 缓存
#include "single_instance.h"         // 单例模式
```

---

## 概述

本目录包含资源调度服务（RSS）的公共工具类和通用功能，被各个模块共享使用。这些工具类提供了日志、缓存、文件操作、字符串处理等基础功能。

## 目录结构

```
common/
├── include/          # 公共头文件
├── src/              # 源文件实现
└── test/             # 测试
    ├── unittest/     # 单元测试
    └── resource/     # 测试资源
        └── json/     # JSON 测试数据
```

## 头文件列表

| 头文件 | 功能描述 |
|--------|----------|
| `res_sched_log.h` | 日志宏定义 |
| `single_instance.h` | 单例模式模板 |
| `res_common_util.h` | 通用工具函数 |
| `res_sched_string_util.h` | 字符串处理工具 |
| `res_sched_time_util.h` | 时间相关工具 |
| `res_sched_file_util.h` | 文件操作工具 |
| `res_sched_json_util.h` | JSON 处理工具 |
| `res_sched_system_util.h` | 系统信息工具 |
| `res_sched_lru_cache.h` | LRU 缓存模板 |
| `ipc_util.h` | IPC 工具函数 |
| `batch_log_printer.h` | 批量日志打印 |
| `res_sched_event_reporter.h` | 事件上报工具 |
| `res_sched_hisysevent_report_util.h` | HiSysEvent 上报工具 |
| `res_sched_hitrace_chain.h` | HiTrace 跟踪工具 |
| `bundle_mgr_helper.h` | Bundle 管理助手 |
| `oobe_manager.h` | OOBE 管理器 |
| `oobe_datashare_utils.h` | OOBE 数据共享工具 |
| `ioobe_task.h` | OOBE 任务接口 |
| `remote_death_recipient.h` | 远程死亡监听基类 |
| `res_json_type.h` | JSON 类型定义 |
| `res_sched_signature_validator.h` | 签名验证工具 |

## 核心工具详解

### 1. 日志系统 (res_sched_log.h)

提供统一的日志输出接口。

**日志级别**：
```cpp
RESSCHED_LOGD(fmt, ...)  // Debug
RESSCHED_LOGI(fmt, ...)  // Info
RESSCHED_LOGW(fmt, ...)  // Warning
RESSCHED_LOGE(fmt, ...)  // Error
RESSCHED_LOGF(fmt, ...)  // Fatal
```

**使用示例**：
```cpp
#include "res_sched_log.h"

RESSCHED_LOGI("Service initialized successfully");
RESSCHED_LOGE("Failed to load plugin: %{public}s", pluginName.c_str());
```

**日志标签**：
```cpp
#define LOG_TAG "RSS"
```

### 2. 单例模式 (single_instance.h)

提供线程安全的单例实现。

**宏定义**：
```cpp
#define DECLARE_SINGLE_INSTANCE(className) \
public: \
    static className& GetInstance(); \
private: \
    className() = default; \
    ~className() = default; \
    className(const className&) = delete; \
    className& operator=(const className&) = delete;

#define DEFINE_SINGLE_INSTANCE(className) \
    className& className::GetInstance() { \
        static className instance; \
        return instance; \
    }
```

**使用示例**：
```cpp
// 头文件
class ConfigReader {
    DECLARE_SINGLE_INSTANCE(ConfigReader)
public:
    void LoadConfig();
};

// 源文件
DEFINE_SINGLE_INSTANCE(ConfigReader)
```

### 3. 字符串工具 (res_sched_string_util.h)

提供常用字符串处理函数。

**接口**：
```cpp
namespace ResSchedStringUtil {
    // 字符串分割
    std::vector<std::string> SplitString(const std::string& str, char delim);
    
    // 去除空白字符
    std::string TrimString(const std::string& str);
    
    // 字符串转小写
    std::string ToLowerString(const std::string& str);
    
    // 检查前缀
    bool StartsWith(const std::string& str, const std::string& prefix);
    
    // 检查后缀
    bool EndsWith(const std::string& str, const std::string& suffix);
    
    // 安全的字符串拷贝
    void SafeCopyString(char* dest, size_t destSize, const char* src);
}
```

### 4. 时间工具 (res_sched_time_util.h)

提供时间相关操作。

**接口**：
```cpp
namespace ResSchedTimeUtil {
    // 获取当前时间戳（毫秒）
    int64_t GetCurrentTimestampMs();
    
    // 获取当前时间戳（微秒）
    int64_t GetCurrentTimestampUs();
    
    // 时间戳转字符串
    std::string FormatTimestamp(int64_t timestamp);
    
    // 获取单调递增时间
    int64_t GetSteadyClockMs();
}
```

### 5. 文件工具 (res_sched_file_util.h)

提供文件操作功能。

**接口**：
```cpp
namespace ResSchedFileUtil {
    // 检查文件是否存在
    bool IsFileExists(const std::string& path);
    
    // 读取文件内容
    bool ReadFile(const std::string& path, std::string& content);
    
    // 写入文件
    bool WriteFile(const std::string& path, const std::string& content);
    
    // 获取文件大小
    int64_t GetFileSize(const std::string& path);
    
    // 创建目录
    bool CreateDir(const std::string& path);
    
    // 获取目录下文件列表
    std::vector<std::string> GetDirFiles(const std::string& path);
}
```

### 6. JSON 工具 (res_sched_json_util.h)

封装 nlohmann/json 库，提供 JSON 处理功能。

**接口**：
```cpp
namespace ResSchedJsonUtil {
    // 解析 JSON 字符串
    bool ParseJsonString(const std::string& str, nlohmann::json& json);
    
    // 将 JSON 转为字符串
    std::string ToString(const nlohmann::json& json, bool pretty = false);
    
    // 安全获取 JSON 值
    template<typename T>
    T GetSafeValue(const nlohmann::json& json, const std::string& key, 
                   const T& defaultValue);
}
```

### 7. LRU 缓存 (res_sched_lru_cache.h)

提供固定容量的 LRU 缓存实现。

**接口**：
```cpp
template<typename Key, typename Value>
class LruCache {
public:
    explicit LruCache(size_t capacity);
    
    // 插入/更新缓存
    void Put(const Key& key, const Value& value);
    
    // 获取缓存值
    bool Get(const Key& key, Value& value);
    
    // 检查是否存在
    bool Contains(const Key& key);
    
    // 删除缓存
    void Remove(const Key& key);
    
    // 清空缓存
    void Clear();
    
    // 获取当前大小
    size_t Size() const;
};
```

**使用示例**：
```cpp
LruCache<std::string, std::string> cache(100);
cache.Put("key1", "value1");

std::string value;
if (cache.Get("key1", value)) {
    RESSCHED_LOGI("Cached value: %{public}s", value.c_str());
}
```

### 8. 批量日志打印 (batch_log_printer.h)

用于批量收集和打印日志，减少日志开销。

**接口**：
```cpp
class BatchLogPrinter {
public:
    void AddLog(const std::string& log);
    void Flush();
    void SetBatchSize(size_t size);
    void SetFlushInterval(int64_t intervalMs);
};
```

### 9. HiTrace 跟踪 (res_sched_hitrace_chain.h)

提供性能跟踪功能。

**接口**：
```cpp
class HitraceScoped {
public:
    explicit HitraceScoped(const std::string& name);
    ~HitraceScoped();
};

// 使用示例
void ProcessEvent() {
    HitraceScoped trace("ProcessEvent");
    // 处理逻辑
}
```

### 10. Bundle 管理助手 (bundle_mgr_helper.h)

封装 Bundle 管理服务接口。

**接口**：
```cpp
class BundleMgrHelper {
    DECLARE_SINGLE_INSTANCE(BundleMgrHelper)
public:
    // 获取应用信息
    bool GetApplicationInfo(const std::string& bundleName, 
                           ApplicationInfo& appInfo);
    
    // 获取包名
    std::string GetBundleNameForUid(int32_t uid);
    
    // 检查应用是否存在
    bool IsBundleExists(const std::string& bundleName);
};
```

### 11. OOBE 管理器 (oobe_manager.h)

管理开箱即用体验（Out-of-Box Experience）状态。

**接口**：
```cpp
class OOBEManager {
    DECLARE_SINGLE_INSTANCE(OOBEManager)
public:
    // 检查是否完成 OOBE
    bool IsOOBECompleted();
    
    // 注册 OOBE 完成回调
    void RegisterCallback(const std::shared_ptr<IOOBETask>& callback);
    
    // 注销回调
    void UnregisterCallback(const std::shared_ptr<IOOBETask>& callback);
};
```

**OOBE 任务接口** (`ioobe_task.h`)：
```cpp
class IOOBETask {
public:
    virtual void OnOOBECompleted() = 0;
};
```

### 12. 签名验证 (res_sched_signature_validator.h)

提供应用签名验证功能。

**接口**：
```cpp
class ResSchedSignatureValidator {
public:
    // 验证应用签名
    bool ValidateAppSignature(const std::string& bundleName);
    
    // 验证调用者权限
    bool ValidateCallerPermission(const std::string& permission);
};
```

### 13. 系统工具 (res_sched_system_util.h)

提供系统信息查询。

**接口**：
```cpp
namespace ResSchedSystemUtil {
    // 获取系统总内存
    int64_t GetTotalMemory();
    
    // 获取可用内存
    int64_t GetAvailableMemory();
    
    // 获取 CPU 核心数
    int32_t GetCpuCoreCount();
    
    // 获取当前进程 CPU 使用率
    float GetProcessCpuUsage(int32_t pid);
}
```

### 14. IPC 工具 (ipc_util.h)

提供 IPC 相关工具函数。

**接口**：
```cpp
namespace IpcUtil {
    // 获取调用者 PID
    pid_t GetCallingPid();
    
    // 获取调用者 UID
    uid_t GetCallingUid();
    
    // 获取当前 PID
    pid_t GetSelfPid();
    
    // 获取当前 UID
    uid_t GetSelfUid();
}
```

### 15. 远程死亡监听 (remote_death_recipient.h)

提供远程对象死亡监听基类。

```cpp
class RemoteDeathRecipient : public IRemoteObject::DeathRecipient {
public:
    void OnRemoteDied(const wptr<IRemoteObject>& remote) override;
    virtual void OnRemoteDiedHandle(const wptr<IRemoteObject>& remote) = 0;
};
```

## 测试

### 单元测试

位于 `common/test/unittest/`：

| 测试文件 | 测试内容 |
|----------|----------|
| `string_util_test.cpp` | 字符串工具测试 |
| `time_util_test.cpp` | 时间工具测试 |
| `file_util_test.cpp` | 文件工具测试 |
| `json_util_test.cpp` | JSON 工具测试 |
| `lru_cache_test.cpp` | LRU 缓存测试 |
| `batch_log_printer_test.cpp` | 批量日志测试 |

### 测试资源

`common/test/resource/json/` - 存放 JSON 测试数据文件。

## 使用指南

### 包含头文件

```cpp
#include "res_sched_log.h"
#include "single_instance.h"
#include "res_sched_string_util.h"
#include "res_sched_time_util.h"
#include "res_sched_lru_cache.h"
```

### 链接依赖

在 BUILD.gn 中：

```gn
deps = [
    "//ressched/common:ressched_common",
]
```

## 性能考虑

1. **日志级别控制**：发布版本使用 Info 及以上级别
2. **缓存大小限制**：LRU 缓存设置合理容量
3. **避免频繁 IPC**：批量获取系统信息
4. **延迟初始化**：按需初始化 Helper 类

## 线程安全

| 类/工具 | 线程安全 |
|---------|----------|
| 日志宏 | 是（底层保证） |
| 单例模式 | 是（C++11 static 保证） |
| LRU 缓存 | 否（需外部加锁） |
| BundleMgrHelper | 是（内部加锁） |
| 时间工具 | 是（只读） |

## 扩展指南

添加新的工具类：

1. 在 `include/` 创建头文件
2. 在 `src/` 创建实现文件（如需要）
3. 在 `test/unittest/` 添加单元测试
4. 更新 `BUILD.gn` 添加新源文件

## 编译方法

### BUILD.gn 配置

公共模块的编译配置位于 `ressched/common/BUILD.gn`。

### 编译目标

| 目标名称 | 类型 | 说明 |
|----------|------|------|
| `ressched_common_utils` | shared_library | 公共工具动态库 |

### 编译命令

```bash
# 编译公共工具库
hb build ressched/common:ressched_common_utils

# 编译单元测试
hb build ressched/common/test/unittest:unittest

# 编译特定单元测试
hb build ressched/common/test/unittest:resschedstringutil_test
hb build ressched/common/test/unittest:resschedtimeutil_test
hb build ressched/common/test/unittest:resschedfileutil_test
hb build ressched/common/test/unittest:resschedjsonutil_test
hb build ressched/common/test/unittest:resschedsystemutil_test
hb build ressched/common/test/unittest:resschedeventreporter_test
hb build ressched/common/test/unittest:resschedhitracechain_test
hb build ressched/common/test/unittest:lrucache_test
hb build ressched/common/test/unittest:resschedsignaturevalidator_test
```

### 作为依赖使用

在 BUILD.gn 中引用公共库：

```gn
deps = [
  "${ressched_common}:ressched_common_utils",
]

configs = [
  "${ressched_common}:common_public_config",
]
```

### 源文件配置

```gn
sources = [
  "../common/src/batch_log_printer.cpp",
  "../common/src/oobe_datashare_utils.cpp",
  "../common/src/oobe_manager.cpp",
  "../common/src/res_json_type.cpp",
  "../common/src/res_sched_event_reporter.cpp",
  "../common/src/res_sched_file_util.cpp",
  "../common/src/res_sched_hisysevent_report_util.cpp",
  "../common/src/res_sched_hitrace_chain.cpp",
  "../common/src/res_sched_json_util.cpp",
  "../common/src/res_sched_string_util.cpp",
  "../common/src/res_sched_system_util.cpp",
  "../common/src/res_sched_time_util.cpp",
  "../common/src/res_sched_signature_validator.cpp",
]
```

### 外部依赖

```gn
external_deps = [
  "ability_base:zuri",                     # Ability 基础
  "ability_runtime:dataobs_manager",       # 数据观察管理
  "bundle_framework:appexecfwk_core",      # Bundle 框架
  "bundle_framework:appexecfwk_base",      # Bundle 基础
  "c_utils:utils",                         # 通用工具
  "config_policy:configpolicy_util",       # 配置策略
  "data_share:datashare_consumer",         # 数据共享
  "ffrt:libffrt",                          # 快速函数运行时
  "hilog:libhilog",                        # 日志
  "hisysevent:libhisysevent",              # 系统事件
  "hitrace:libhitracechain",               # 性能跟踪链
  "init:libbegetutil",                     # 初始化工具
  "ipc:ipc_core",                          # IPC 核心
  "json:nlohmann_json_static",             # JSON 库
  "os_account:os_account_innerkits",       # OS 账户
  "safwk:system_ability_fwk",              # SA 框架
  "samgr:samgr_proxy",                     # SA 管理器
]
```

### 安全编译选项

```gn
cflags_cc = [ "-fstack-protector-strong" ]  # 栈保护

sanitize = {
  cfi = true              # 控制流完整性
  cfi_cross_dso = true    # 跨 DSO CFI
  debug = false
}

branch_protector_ret = "pac_ret"  # 返回地址保护
```

### 公共导出配置

```gn
config("common_public_config") {
  include_dirs = [ "include" ]
}

config("common_private_config") {
  include_dirs = [ "${ressched_common}/include" ]
}
```

### 编译选项说明

| 选项 | 说明 |
|------|------|
| `-fstack-protector-strong` | 强栈保护 |
| `cfi = true` | 控制流完整性检查 |
| `branch_protector_ret = "pac_ret"` | ARM 指针认证 |
| `public_external_deps` | 公共导出依赖 |

### 单元测试编译选项

```gn
cflags = [
  "-Dprivate=public",    # 测试时暴露私有成员
  "-Dprotected=public",  # 测试时暴露保护成员
]

cflags_cc = [ "-DRES_SCHED_UNIT_TEST" ]  # 单元测试宏
```
