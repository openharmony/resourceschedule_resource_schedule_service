# 资源调度服务 - 接口模块

---

## 快速参考

### 编译命令

```bash
# C++ 客户端库
hb build ressched/interfaces/innerkits/ressched_client:ressched_client
hb build ressched/interfaces/innerkits/suspend_manager_base_client:suspend_manager_base_client

# C API
hb build ressched/interfaces/kits/c/background_process_manager:background_process_manager

# ArkTS/ETS 接口
hb build ressched/interfaces/kits/ets/taihe/background_process_manager:backgroundprocessmanager_ani
hb build ressched/interfaces/kits/ets/taihe/systemload:systemload_taihe_native

# 接口测试
hb build ressched/interfaces/kits/test/unittest/systemload_jsunittest:js_unittest
```

### 接口输出

- **C++ 客户端**: `libressched_client.a`
- **C API**: `libbackground_process_manager.z.so`
- **ArkTS ANI**: `libbackgroundprocessmanager_ani.so`

---

## 概述

本目录包含资源调度服务（RSS）的接口定义和实现，包括内部客户端库（innerkits）和多语言绑定（kits），为上层应用和系统服务提供访问 RSS 能力的统一接口。

## 目录结构

```
interfaces/
├── innerkits/                    # 内部 C++ 客户端库
│   ├── ressched_client/          # RSS 客户端
│   │   ├── src/                  # 实现
│   │   └── include/              # 头文件
│   └── suspend_manager_base_client/  # 暂停管理器客户端
│       ├── src/                  # 实现
│       └── include/              # 头文件
└── kits/                         # 多语言绑定
    ├── c/                        # C API
    │   └── background_process_manager/
    ├── js/                       # JavaScript/Node-API
    │   └── napi/
    │       ├── background_process_manager/
    │       └── systemload/
    ├── ets/                      # ArkTS/ETS (taihe)
    │   └── taihe/
    │       ├── background_process_manager/
    │       └── systemload/
    └── test/                     # 接口测试
        └── unittest/
```

## 内部客户端 (innerkits)

### RSS 客户端 (ressched_client)

提供 C++ 客户端接口，供系统服务调用 RSS 功能。

#### 核心类

| 类名 | 文件 | 功能 |
|------|------|------|
| ResSchedClient | `res_sched_client.cpp` | 客户端主类 |
| ResIpcInit | `res_ipc_init.cpp` | IPC 初始化 |
| ResSaInit | `res_sa_init.cpp` | SA 初始化 |
| ResSchedCommonDeathRecipient | `res_sched_common_death_recipient.cpp` | 死亡监听 |
| KillEventListener | `kill_event_listener.cpp` | 进程终止监听 |

#### ResSchedClient

**单例模式**：
```cpp
class ResSchedClient {
public:
    static ResSchedClient& GetInstance();
    
    // 上报资源事件（异步）
    void ReportData(uint32_t resType, int64_t value, const std::string& payload);
    
    // 上报资源事件（同步）
    int32_t ReportSyncEvent(const std::shared_ptr<ResData>& data, std::string& reply);
    
    // 注册事件监听
    int32_t RegisterEventListener(const sptr<IRemoteObject>& listener, uint32_t eventType);
    
    // 注销事件监听
    int32_t UnRegisterEventListener(const sptr<IRemoteObject>& listener, uint32_t eventType);
    
    // 获取系统负载
    int32_t GetSystemLoadStatus(const std::string& payload, std::string& reply);
    
    // 判断服务是否可用
    bool IsAvailable();
};
```

**使用示例**：
```cpp
#include "res_sched_client.h"

// 上报应用状态变化
nlohmann::json payload;
payload["pid"] = pid;
payload["uid"] = uid;
payload["bundleName"] = bundleName;

ResSchedClient::GetInstance().ReportData(
    ResType::RES_TYPE_APP_STATE_CHANGE,
    static_cast<int64_t>(AppState::FOREGROUND),
    payload.dump());
```

#### KillEventListener

监听进程终止事件，用于清理资源。

```cpp
class KillEventListener : public IRemoteObject::DeathRecipient {
public:
    void OnRemoteDied(const wptr<IRemoteObject>& remote) override;
};
```

### 暂停管理器客户端 (suspend_manager_base_client)

提供系统挂起/恢复功能的客户端接口。

#### 核心类

| 类名 | 功能 |
|------|------|
| SuspendManagerBaseClient | 暂停管理器客户端 |
| SuspendManagerBaseProxy | IPC 代理类 |

#### 接口

```cpp
class SuspendManagerBaseClient {
public:
    static SuspendManagerBaseClient& GetInstance();
    
    // 请求系统挂起
    int32_t RequestSuspend(bool immediately);
    
    // 获取挂起状态
    int32_t GetSuspendState(int32_t& state);
};
```

## 多语言绑定 (kits)

### C API (c)

提供 C 语言接口，供 Native 应用使用。

#### 后台进程管理 (background_process_manager)

**头文件**：`background_process_manager.h`

**接口**：
```c
// 申请后台运行
int RequestBackgroundRunning(const char* bundleName, int pid);

// 取消后台运行
int CancelBackgroundRunning(const char* bundleName);

// 检查后台运行状态
int IsBackgroundRunning(const char* bundleName, bool* isRunning);
```

### JavaScript API (js/napi)

通过 Node-API 提供 JavaScript/TypeScript 接口。

#### 后台进程管理

```typescript
namespace backgroundProcessManager {
    function requestBackgroundRunning(bundleName: string, pid: number): Promise<void>;
    function cancelBackgroundRunning(bundleName: string): Promise<void>;
}
```

#### 系统负载查询

```typescript
namespace systemLoad {
    function getSystemLoadStatus(): Promise<SystemLoadInfo>;
    
    interface SystemLoadInfo {
        cpuLoad: number;
        memoryLoad: number;
        ioLoad: number;
    }
}
```

### ArkTS/ETS API (ets/taihe)

通过 taihe 框架提供 ArkTS 接口。

#### 后台进程管理

```typescript
import { backgroundProcessManager } from '@kit.ResourceScheduleKit';

// 申请后台运行
await backgroundProcessManager.requestBackgroundRunning(bundleName, pid);

// 取消后台运行
await backgroundProcessManager.cancelBackgroundRunning(bundleName);
```

#### 系统负载

```typescript
import { systemload } from '@kit.ResourceScheduleKit';

// 获取系统负载
const loadStatus = await systemload.getSystemLoadStatus();
console.info(`CPU Load: ${loadStatus.cpuLoad}`);
```

## IPC 接口定义

### IResSchedService 接口

```cpp
class IResSchedService : public IRemoteBroker {
public:
    enum {
        REPORT_DATA = 1,
        REGISTER_EVENT_LISTENER,
        UNREGISTER_EVENT_LISTENER,
        GET_SYSTEM_LOAD_STATUS,
    };
    
    virtual int32_t ReportData(uint32_t resType, int64_t value, 
                               const std::string& payload) = 0;
    virtual int32_t RegisterEventListener(const sptr<IRemoteObject>& listener,
                                          uint32_t eventType) = 0;
    virtual int32_t UnRegisterEventListener(const sptr<IRemoteObject>& listener,
                                            uint32_t eventType) = 0;
    virtual int32_t GetSystemLoadStatus(const std::string& payload,
                                        std::string& reply) = 0;
};
```

### IResSchedEventListener 接口

```cpp
class IResSchedEventListener : public IRemoteBroker {
public:
    virtual void OnReceiveEvent(uint32_t eventType, int64_t value, 
                                const std::string& payload) = 0;
};
```

## 接口使用场景

### 系统服务使用

```cpp
// 窗口管理服务上报焦点变化
ResSchedClient::GetInstance().ReportData(
    ResType::RES_TYPE_WINDOW_FOCUS,
    pid,
    jsonPayload);
```

### 应用开发使用

```typescript
// ArkTS 应用查询系统负载
import { systemload } from '@kit.ResourceScheduleKit';

async function checkSystemLoad() {
    const status = await systemload.getSystemLoadStatus();
    if (status.cpuLoad > 80) {
        // 降低动画质量
    }
}
```

### 后台任务管理

```typescript
// 申请后台运行
import { backgroundProcessManager } from '@kit.ResourceScheduleKit';

async function startBackgroundTask() {
    try {
        await backgroundProcessManager.requestBackgroundRunning(
            'com.example.myapp', 
            process.pid
        );
    } catch (error) {
        console.error('Failed to request background running');
    }
}
```

## 接口版本管理

### 版本号定义

```cpp
constexpr int32_t API_VERSION_MAJOR = 1;
constexpr int32_t API_VERSION_MINOR = 0;
```

### 兼容性处理

```cpp
int32_t ResSchedClient::ReportData(uint32_t resType, int64_t value, 
                                   const std::string& payload) {
    if (!IsAvailable()) {
        return ERR_SERVICE_NOT_AVAILABLE;
    }
    // 检查服务端版本兼容性
    // ...
}
```

## 权限要求

### 接口权限

| 接口 | 所需权限 |
|------|----------|
| ReportData | ohos.permission.REPORT_RESOURCE_SCHEDULE_EVENT |
| RegisterEventListener | ohos.permission.REPORT_RESOURCE_SCHEDULE_EVENT |
| GetSystemLoadStatus | 无需权限（部分信息） |
| RequestBackgroundRunning | ohos.permission.KEEP_BACKGROUND_RUNNING |

### 权限检查

```cpp
static const char* NEEDED_PERMISSION = 
    "ohos.permission.REPORT_RESOURCE_SCHEDULE_EVENT";

int32_t ResSchedService::ReportData(uint32_t resType, int64_t value,
                                    const std::string& payload) {
    if (!VerifyPermission(NEEDED_PERMISSION)) {
        return ERR_PERMISSION_DENIED;
    }
    // ...
}
```

## 错误码定义

```cpp
enum ResErrCode {
    ERR_OK = 0,
    ERR_SERVICE_NOT_AVAILABLE = -1,
    ERR_PERMISSION_DENIED = -2,
    ERR_INVALID_PARAM = -3,
    ERR_IPC_FAILED = -4,
    ERR_TIMEOUT = -5,
};
```

## 测试

### 单元测试

位于 `interfaces/kits/test/unittest/`：
- 接口功能测试
- 参数边界测试
- 并发安全测试

### 示例测试

```cpp
TEST(ResSchedClientTest, ReportData_Success) {
    auto& client = ResSchedClient::GetInstance();
    
    nlohmann::json payload;
    payload["test"] = "value";
    
    int32_t ret = client.ReportData(
        ResType::RES_TYPE_APP_STATE_CHANGE,
        1,
        payload.dump());
    
    EXPECT_EQ(ret, ERR_OK);
}
```

## 性能优化

### 连接复用

- 维护 IPC 连接池
- 延迟连接建立
- 断线自动重连

### 批量上报

```cpp
// 收集多个事件后批量上报
class BatchReporter {
public:
    void AddEvent(const EventData& data);
    void Flush();  // 批量发送
};
```

## 最佳实践

1. **异步优先**：优先使用异步接口（ReportData）
2. **错误处理**：始终检查返回值
3. **权限申请**：在应用配置中声明所需权限
4. **资源释放**：页面销毁时注销事件监听
5. **频率控制**：避免高频调用，使用节流机制

## 编译方法

### BUILD.gn 配置

接口模块的编译配置分布在：
- `innerkits/ressched_client/BUILD.gn` - C++ 客户端库
- `innerkits/suspend_manager_base_client/BUILD.gn` - 暂停管理器客户端
- `kits/c/background_process_manager/BUILD.gn` - C API
- `kits/js/napi/*/BUILD.gn` - Node-API 接口
- `kits/ets/taihe/*/BUILD.gn` - ArkTS/ETS 接口

### 编译目标

| 目标名称 | 类型 | 说明 |
|----------|------|------|
| `ressched_client` | source_set | RSS 客户端库 |
| `suspend_manager_base_client` | source_set | 暂停管理器客户端 |
| `res_sched_service_stub` | source_set | IPC Stub |
| `background_process_manager` | shared_library | C API |
| `backgroundprocessmanager_ani` | taihe_shared_library | ArkTS ANI 实现 |
| `systemload_taihe_native` | taihe_shared_library | 系统负载 ArkTS |

### 编译命令

```bash
# 编译 RSS 客户端库
hb build ressched/interfaces/innerkits/ressched_client:ressched_client

# 编译暂停管理器客户端
hb build ressched/interfaces/innerkits/suspend_manager_base_client:suspend_manager_base_client

# 编译 C API
hb build ressched/interfaces/kits/c/background_process_manager:background_process_manager

# 编译 ArkTS/ETS 接口
hb build ressched/interfaces/kits/ets/taihe/background_process_manager:backgroundprocessmanager_ani
hb build ressched/interfaces/kits/ets/taihe/systemload:systemload_taihe_native

# 编译 JS 测试
hb build ressched/interfaces/kits/test/unittest/systemload_jsunittest:js_unittest
```

### IDL 接口编译

```gn
idl_gen_interface("res_sched_service_interface") {
  src_idl = rebase_path("IResSchedService.idl")
}

idl_gen_interface("res_sched_event_listener_interface") {
  src_idl = rebase_path("IResSchedEventListener.idl")
}

idl_gen_interface("res_sched_systemload_notifier_interface") {
  src_idl = rebase_path("IResSchedSystemloadNotifier.idl")
}
```

### 作为依赖使用

在 BUILD.gn 中引用接口库：

```gn
deps = [
  "${ressched_interfaces}/innerkits/ressched_client:ressched_client",
]

public_configs = [
  "${ressched_interfaces}/innerkits/ressched_client:client_public_config",
]
```

### 外部依赖

```gn
external_deps = [
  "ability_runtime:ability_manager",    # 应用运行时
  "c_utils:utils",                       # 通用工具
  "hilog:libhilog",                      # 日志
  "ipc:ipc_core",                        # IPC 核心
  "ipc:ipc_single",                      # IPC
  "samgr:samgr_proxy",                   # SA 管理器
  "safwk:system_ability_fwk",            # SA 框架
]
```

### 安全编译选项

```gn
sanitize = {
  cfi = true
  cfi_cross_dso = true
  debug = false
}
branch_protector_ret = "pac_ret"
```

### taihe 配置（ArkTS/ETS）

```gn
taihe_generated_file_path = "$taihe_file_path/out/resourceschedule/resource_schedule_service"

copy_taihe_idl("copy_backgroundprocessmanager_taihe") {
  sources = [ "idl/ohos.resourceschedule.backgroundProcessManager.taihe" ]
}

ohos_taihe("run_taihe") {
  taihe_generated_file_path = "$taihe_generated_file_path"
  deps = [ ":copy_backgroundprocessmanager_taihe" ]
  outputs = [
    "$taihe_generated_file_path/src/ohos.resourceschedule.backgroundProcessManager.ani.cpp",
    "$taihe_generated_file_path/src/ohos.resourceschedule.backgroundProcessManager.abi.c",
  ]
}
```

### 生成静态 ABC

```gn
generate_static_abc("backgroundprocessmanager_abc") {
  base_url = "$taihe_generated_file_path"
  files = [ "$taihe_generated_file_path/@ohos.resourceschedule.backgroundProcessManager.ets" ]
  is_boot_abc = "True"
  device_dst_file = "/system/framework/backgroundprocessmanager_abc.abc"
}
```

### 头文件引用

```cpp
// C++ 客户端
#include "res_sched_client.h"
#include "res_sched_ipc_interface_code.h"

// C API
#include "background_process_manager.h"

// ArkTS/ETS 通过 import 使用
// import { backgroundProcessManager } from '@kit.ResourceScheduleKit';
```
