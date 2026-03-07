# 资源调度服务 - 服务模块

---

## 快速参考

### 编译命令

```bash
# 编译 RSS 主服务
hb build ressched/services:resschedsvc

# 编译静态库版本（用于测试）
hb build ressched/services:resschedsvc_static

# 编译暂停管理器服务
hb build ressched/services:suspend_manager_base_service

# 编译整个子系统
hb build resource_schedule_service
```

### 服务输出

- **服务库**: `out/{product}/resourceschedule/resource_schedule_service/libresschedsvc.z.so`
- **静态库**: `out/{product}/obj/ressched/services/libresschedsvc_static/`
- **系统能力 ID**: SA 1901 (ressched), SA 1918 (ressched_executor)

### 服务启动

```bash
# 查看服务状态
ps -ef | grep ressched

# 查看日志
hilog | grep RSS
```

---

## 概述

本目录包含资源调度服务（RSS）的核心服务实现，包括资源调度管理器、插件框架和服务层接口。

## 目录结构

```
services/
├── resschedmgr/              # 资源调度管理器
│   ├── resschedfwk/          # 调度框架核心
│   │   ├── src/              # 实现文件
│   │   └── include/          # 头文件
│   └── pluginbase/           # 插件基类定义
│       └── include/          # 插件接口头文件
├── resschedservice/          # 服务层
│   ├── src/                  # 服务实现
│   └── include/              # 服务头文件
└── suspend_manager_base/     # 暂停管理器
    ├── src/                  # 实现文件
    └── include/              # 头文件
```

## 资源调度管理器 (resschedmgr)

### 调度框架 (resschedfwk)

框架核心组件，负责插件生命周期管理和事件分发。

#### 核心类

| 类名 | 文件 | 职责 |
|------|------|------|
| ResSchedMgr | `res_sched_mgr.cpp` | 资源调度管理器主类 |
| PluginMgr | `plugin_mgr.cpp` | 插件管理器 |
| ConfigReader | `config_reader.cpp` | 配置文件读取器 |
| PluginSwitch | `plugin_switch.cpp` | 插件开关管理 |
| EventListenerMgr | `event_listener_mgr.cpp` | 事件监听器管理 |
| NotifierMgr | `notifier_mgr.cpp` | 通知管理器 |
| KillProcess | `kill_process.cpp` | 进程终止工具 |
| ResBundleMgrHelper | `res_bundle_mgr_helper.cpp` | Bundle 管理助手 |

#### ResSchedMgr - 资源调度管理器

**职责**：
- 初始化/反初始化 RSS 服务
- 管理插件生命周期
- 协调各子系统

**关键方法**：
```cpp
class ResSchedMgr {
public:
    static ResSchedMgr& GetInstance();
    void Init();           // 初始化服务
    void Stop();           // 停止服务
    void ReportData();     // 上报数据
};
```

#### PluginMgr - 插件管理器

**职责**：
- 加载/卸载插件
- 分发事件到插件
- 插件生命周期管理

**插件加载流程**：
```
读取配置 → 加载 SO → 创建实例 → 调用 Init() → 注册事件订阅
```

**关键方法**：
```cpp
class PluginMgr {
public:
    void LoadPlugin(const std::string& pluginName);
    void UnloadPlugin(const std::string& pluginName);
    void DispatchResource(const std::shared_ptr<ResData>& data);
    int32_t DeliverResource(const std::shared_ptr<ResData>& data);
};
```

#### ConfigReader - 配置读取器

**职责**：
- 解析 XML 配置文件
- 管理插件配置
- 支持自定义配置覆盖

**配置文件**：
- `res_sched_config.xml` - 主配置
- `res_sched_plugin_switch.xml` - 插件开关
- `/data/service/el1/public/ressched/` - 自定义配置目录

### 插件基类 (pluginbase)

定义插件接口和数据结构。

#### 核心头文件

| 文件 | 内容 |
|------|------|
| `plugin.h` | Plugin 基类定义 |
| `res_data.h` | ResData 数据结构 |
| `config_info.h` | 配置信息结构 |
| `res_type.h` | 资源事件类型枚举 |

#### Plugin 基类

```cpp
class Plugin {
public:
    virtual ~Plugin() = default;
    virtual void Init() = 0;                    // 初始化
    virtual void Disable() = 0;                 // 停用
    virtual void DispatchResource(const std::shared_ptr<ResData>& data) = 0;
    virtual int32_t DeliverResource(const std::shared_ptr<ResData>& data);
};
```

#### ResData 结构

```cpp
struct ResData {
    uint32_t resType;           // 事件类型
    int64_t value;              // 事件值
    nlohmann::json payload;     // 附加数据
    nlohmann::json* reply;      // 回复数据（同步事件）
};
```

## 服务层 (resschedservice)

### 服务接口实现

实现 System Ability 接口，对外提供 IPC 服务。

#### 核心类

| 类名 | 功能 |
|------|------|
| ResSchedService | RSS 服务主类 |
| ResSchedServiceStub | IPC 存根实现 |
| ResSchedDeathRecipient | 死亡通知处理 |

#### ResSchedService

**System Ability ID**: 1901

**提供接口**：
```cpp
class ResSchedService : public SystemAbility, public ResSchedServiceStub {
public:
    // 报告资源事件
    int32_t ReportData(uint32_t resType, int64_t value, const std::string& payload);
    
    // 注册事件监听器
    int32_t RegisterEventListener(const sptr<IRemoteObject>& listener, uint32_t eventType);
    
    // 注销事件监听器
    int32_t UnRegisterEventListener(const sptr<IRemoteObject>& listener, uint32_t eventType);
    
    // 获取系统负载
    int32_t GetSystemLoadStatus(const std::string& payload, std::string& reply);
};
```

**权限检查**：
```cpp
static const char* NEEDED_PERMISSION = "ohos.permission.REPORT_RESOURCE_SCHEDULE_EVENT";
```

## 暂停管理器 (suspend_manager_base)

### 功能

管理系统挂起/恢复相关功能。

#### 核心类

| 类名 | 功能 |
|------|------|
| SuspendManagerBase | 暂停管理器基类 |
| SuspendManagerBaseClient | 客户端接口 |

#### 使用场景

- 设备休眠管理
- 后台任务控制
- 省电模式切换

## 服务初始化流程

```
1. SystemAbility 加载
   ↓
2. OnStart() 调用
   ↓
3. ResSchedMgr::Init()
   - 初始化配置读取器
   - 加载插件
   - 初始化事件监听
   ↓
4. 服务就绪，接收事件
```

## 服务停止流程

```
1. OnStop() 调用
   ↓
2. ResSchedMgr::Stop()
   - 停止事件监听
   - 卸载插件
   - 清理资源
   ↓
3. 服务停止
```

## 事件处理流程

### 异步事件

```
外部事件 → IPC 调用 → ResSchedService → PluginMgr::DispatchResource() → 插件处理
```

### 同步事件

```
外部请求 → IPC 调用 → ResSchedService → PluginMgr::DeliverResource() → 插件处理 → 返回结果
```

## 配置管理

### 配置层次

1. **系统配置** - `/system/etc/res_sched/`
2. **产品配置** - `/vendor/etc/res_sched/`
3. **用户配置** - `/data/service/el1/public/ressched/`

### 配置优先级

用户配置 > 产品配置 > 系统配置

### 配置热更新

```cpp
// 重新加载配置
ConfigReader::GetInstance().ReloadConfig();
```

## 性能优化

### 事件分发优化

- 批量处理相似事件
- 异步执行耗时操作
- 事件优先级队列

### 内存管理

- 插件按需加载
- 配置缓存机制
- 资源及时释放

## 调试与诊断

### 日志级别

```cpp
RESSCHED_LOGD("Debug message");  // 调试
RESSCHED_LOGI("Info message");   // 信息
RESSCHED_LOGW("Warning");        // 警告
RESSCHED_LOGE("Error");          // 错误
RESSCHED_LOGF("Fatal");          // 致命
```

### HiSysEvent 事件

```cpp
HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::RSS, "EVENT_NAME",
    HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
    "KEY", value);
```

### HiTrace 跟踪

```cpp
HitraceScoped trace("ResSchedMgr::DispatchResource");
```

## 安全机制

### 权限控制

- IPC 调用权限检查
- UID 白名单机制
- 敏感操作限制

### UID 限制

```cpp
static constexpr int32_t MEMMGR_UID = 1111;
static constexpr int32_t FOUNDATION_UID = 5523;
static constexpr int32_t ROOT_UID = 0;
```

## 测试

### 单元测试

- `services/resschedmgr` 组件测试
- Mock 插件测试
- 配置解析测试

### 集成测试

- 服务启动/停止测试
- 插件加载测试
- IPC 接口测试

## 编译方法

### BUILD.gn 配置

服务模块的编译配置位于 `ressched/services/BUILD.gn`。

### 编译目标

| 目标名称 | 类型 | 说明 |
|----------|------|------|
| `resschedsvc` | shared_library | RSS 服务动态库 |
| `resschedsvc_static` | source_set | RSS 服务静态库（用于测试） |
| `suspend_manager_base_service` | source_set | 暂停管理器服务 |

### 编译命令

```bash
# 编译 RSS 服务
hb build ressched/services:resschedsvc

# 编译静态库版本（用于测试依赖）
hb build ressched/services:resschedsvc_static

# 编译暂停管理器服务
hb build ressched/services:suspend_manager_base_service

# 编译整个子系统
hb build resource_schedule_service
```

### 作为依赖使用

在 BUILD.gn 中依赖 RSS 服务：

```gn
deps = [
  "${ressched_services}:resschedsvc",
  # 或静态库版本
  "${ressched_services}:resschedsvc_static",
]
```

### 编译配置详解

#### 公共配置

```gn
config("ressched_config") {
  include_dirs = [
    "${ressched_common}/include",
    "resschedmgr/resschedfwk/include",
    "resschedmgr/pluginbase/include",
  ]
}
```

#### 服务配置

```gn
config("resschedsvc_private_config") {
  include_dirs = [
    "resschedservice/include",
    "../scene_recognize/include",
    "../sched_controller/common_event/include",
    "../sched_controller/observer/include",
  ]
}
```

### 编译依赖

核心外部依赖：
```gn
external_deps = [
  "ability_runtime:ability_manager",      # 应用运行时
  "access_token:libaccesstoken_sdk",      # 访问令牌
  "c_utils:utils",                         # 通用工具
  "common_event_service:cesfwk_innerkits", # 公共事件
  "ffrt:libffrt",                          # 快速函数运行时
  "hilog:libhilog",                        # 日志
  "hisysevent:libhisysevent",              # 系统事件
  "hitrace:hitrace_meter",                 # 性能跟踪
  "ipc:ipc_single",                        # IPC
  "safwk:system_ability_fwk",              # SA 框架
  "samgr:samgr_proxy",                     # SA 管理器
  "libxml2:libxml2",                       # XML 解析
]
```

### 安全编译选项

```gn
sanitize = {
  cfi = true              # 控制流完整性
  cfi_cross_dso = true    # 跨 DSO CFI
  debug = false
}
branch_protector_ret = "pac_ret"  # 返回地址保护
```

### IDL 接口编译

服务接口使用 IDL 定义，自动生成的代码：
```gn
idl_gen_interface("res_sched_service_interface") {
  src_idl = rebase_path("IResSchedService.idl")
}

idl_gen_interface("res_sched_event_listener_interface") {
  src_idl = rebase_path("IResSchedEventListener.idl")
}
```

### 功能开关

编译时可通过功能开关启用/禁用特性：
```gn
if (ressched_frame_aware_sched_enable) {
  defines += [ "RESSCHED_FRAME_AWARE_SCHED_ENABLE" ]
}

if (ressched_with_telephony_state_registry_enable) {
  defines += [ "RESSCHED_TELEPHONY_STATE_REGISTRY_ENABLE" ]
}
```

## 常见问题

1. **服务启动失败**：检查配置文件是否存在
2. **插件加载失败**：检查 SO 文件权限和依赖
3. **事件丢失**：检查事件订阅配置
4. **内存泄漏**：使用 Valgrind 检测
