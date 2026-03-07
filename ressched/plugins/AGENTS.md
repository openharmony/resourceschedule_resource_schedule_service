# 资源调度服务 - 插件模块

本文件为 Claude Code 提供 RSS 插件模块的开发指导。

---

## 快速参考

### 编译命令

```bash
# Cgroup 调度插件
hb build ressched/plugins/cgroup_sched_plugin/framework:cgroup_sched
hb build ressched/plugins/cgroup_sched_plugin/framework/process_group:libprocess_group

# 其他插件
hb build ressched/plugins/socperf_plugin:socperf_plugin
hb build ressched/plugins/frame_aware_plugin:frame_aware_plugin
hb build ressched/plugins/device_standby_plugin:device_standby_plugin

# 插件测试
hb build ressched/plugins/cgroup_sched_plugin/test/unittest:ProcessGroupSubTest
hb build ressched/plugins/cgroup_sched_plugin/test/fuzztest/cgroupsched_fuzzer:fuzztest
```

### 插件输出位置

- **插件目录**: `out/{product}/resourceschedule/resource_schedule_service/`
- **插件后缀**: `*.z.so`
- **Cgroup 插件**: `libcgroup_sched.z.so`
- **进程组库**: `libprocess_group.z.so`

---

## 概述

本目录包含资源调度服务（RSS）的内置插件。插件是 RSS 的核心扩展机制，通过动态共享库（`.z.so`）加载，用于实现各种资源调度策略。

## 插件架构

### 插件基类

所有插件必须继承自 `Plugin` 基类：

```cpp
class Plugin {
public:
    virtual ~Plugin() = default;
    virtual void Init() = 0;                    // 插件初始化
    virtual void Disable() = 0;                 // 插件停用
    virtual void DispatchResource(const std::shared_ptr<ResData>& data) = 0;  // 事件分发
    virtual int32_t DeliverResource(const std::shared_ptr<ResData>& data);    // 同步交付
};
```

### 资源数据结构

```cpp
struct ResData {
    uint32_t resType;        // 资源事件类型
    int64_t value;           // 事件值
    nlohmann::json payload;  // 附加数据
    nlohmann::json* reply;   // 回复数据（同步事件）
};
```

## 目录结构

```
plugins/
├── cgroup_sched_plugin/       # Cgroup 调度插件
│   ├── framework/             # 框架核心
│   │   ├── process_group/     # 进程组管理
│   │   ├── sched_controller/  # 调度控制器
│   │   └── utils/             # 工具函数
│   ├── common/                # 公共头文件
│   ├── profiles/              # 配置文件
│   └── test/                  # 插件测试
├── socperf_plugin/            # SoC 性能插件
│   ├── src/                   # 源文件
│   ├── include/               # 头文件
│   └── test/                  # 测试
├── frame_aware_plugin/        # 帧感知插件
│   ├── src/                   # 源文件
│   ├── include/               # 头文件（含 latency_control）
│   └── test/                  # 测试
└── device_standby_plugin/     # 设备待机插件
    ├── src/                   # 源文件
    ├── include/               # 头文件
    └── test/                  # 测试
```

## 插件详情

### 1. Cgroup 调度插件 (cgroup_sched_plugin)

**功能**：进程 Cgroup 管理和调度

**核心组件**：

| 组件 | 文件 | 功能 |
|------|------|------|
| CgroupAdjuster | `sched_controller/cgroup_adjuster.cpp` | Cgroup 调整器 |
| CgroupEventHandler | `sched_controller/cgroup_event_handler.cpp` | Cgroup 事件处理器 |
| SchedController | `sched_controller/sched_controller.cpp` | 调度控制器 |
| Supervisor | `sched_controller/supervisor.cpp` | 进程监控器 |
| ResSchedUtils | `utils/ressched_utils.cpp` | 调度工具函数 |

**配置文件**：
- `profiles/cgroup_config.xml` - Cgroup 配置

**依赖事件**：
- `RES_TYPE_APP_STATE_CHANGE` - 应用状态变化
- `RES_TYPE_WINDOW_FOCUS` - 窗口焦点变化
- `RES_TYPE_CGROUP_ADJUSTER` - Cgroup 调整事件

### 2. SoC 性能插件 (socperf_plugin)

**功能**：SoC 性能频率调制

**主要功能**：
- CPU/GPU 频率调整
- 性能模式管理
- 功耗优化

**关键配置**：
- 性能档位定义
- 频率限制策略
- 温度控制阈值

### 3. 帧感知插件 (frame_aware_plugin)

**功能**：帧率感知和延迟控制

**核心模块**：
- `latency_control/` - 延迟控制逻辑
- 帧率预测算法
- UI 流畅度优化

**适用场景**：
- 游戏性能优化
- 动画流畅度提升
- 交互响应优化

### 4. 设备待机插件 (device_standby_plugin)

**功能**：设备待机状态管理

**功能特性**：
- 待机模式检测
- 省电策略执行
- 后台任务管理
- 唤醒事件处理

## 插件开发指南

### 创建新插件

1. **创建目录结构**
```
my_plugin/
├── src/
├── include/
└── test/
```

2. **实现插件类**

```cpp
#include "plugin.h"

class MyPlugin : public OHOS::ResourceSchedule::Plugin {
public:
    void Init() override;
    void Disable() override;
    void DispatchResource(const std::shared_ptr<ResData>& data) override;
};
```

3. **实现关键方法**

```cpp
void MyPlugin::Init() {
    // 订阅感兴趣的事件
    // 初始化资源
}

void MyPlugin::DispatchResource(const std::shared_ptr<ResData>& data) {
    switch (data->resType) {
        case ResType::RES_TYPE_SCREEN_STATUS:
            HandleScreenStatus(data);
            break;
        // 处理其他事件
    }
}
```

4. **创建 BUILD.gn**

```gn
ohos_shared_library("my_plugin") {
    sources = [ "src/my_plugin.cpp" ]
    include_dirs = [ "include" ]
    deps = [ "//ressched/services/resschedmgr:resschedmgr" ]
}
```

### 事件订阅

在 `Init()` 中订阅事件：

```cpp
void MyPlugin::Init() {
    // 插件管理器会自动分发所有事件
    // 在 DispatchResource 中过滤处理
}
```

### 性能约束

- **时间限制**：
  - > 1ms：记录警告日志
  - > 10ms：框架报告错误
- **线程安全**：使用独立线程处理耗时任务
- **内存管理**：避免内存泄漏

## 插件配置

### 开关配置

`res_sched_plugin_switch.xml`：

```xml
<pluginSwitch>
    <plugin name="cgroup_sched_plugin" enable="true"/>
    <plugin name="socperf_plugin" enable="true"/>
    <plugin name="frame_aware_plugin" enable="true"/>
    <plugin name="device_standby_plugin" enable="true"/>
</pluginSwitch>
```

### 参数配置

`res_sched_config.xml`：

```xml
<plugin name="cgroup_sched_plugin">
    <param name="key" value="value"/>
</plugin>
```

## 测试

### 单元测试

每个插件都有自己的测试目录：
```
plugin_name/test/unittest/
```

### Fuzz 测试

部分插件包含 Fuzz 测试：
```
plugin_name/test/fuzztest/
```

## 编译方法

### BUILD.gn 配置

每个插件目录下有独立的 `BUILD.gn` 文件定义编译配置。

### Cgroup 调度插件编译

```bash
# 编译 cgroup 调度插件
hb build ressched/plugins/cgroup_sched_plugin/framework:cgroup_sched

# 编译进程组库
hb build ressched/plugins/cgroup_sched_plugin/framework/process_group:libprocess_group

# 编译单元测试
hb build ressched/plugins/cgroup_sched_plugin/test/unittest:ProcessGroupSubTest
hb build ressched/plugins/cgroup_sched_plugin/test/unittest:ResschedUtilsTest

# 编译 Fuzz 测试
hb build ressched/plugins/cgroup_sched_plugin/test/fuzztest/cgroupsched_fuzzer:fuzztest
```

### 其他插件编译

```bash
# SoC 性能插件
hb build ressched/plugins/socperf_plugin:socperf_plugin

# 帧感知插件
hb build ressched/plugins/frame_aware_plugin:frame_aware_plugin

# 设备待机插件
hb build ressched/plugins/device_standby_plugin:device_standby_plugin
```

### 编译目标说明

| 目标 | 类型 | 说明 |
|------|------|------|
| `cgroup_sched` | shared_library | Cgroup 调度插件动态库 |
| `libprocess_group` | shared_library | 进程组管理库 |
| `socperf_plugin` | shared_library | SoC 性能插件 |
| `frame_aware_plugin` | shared_library | 帧感知插件 |
| `device_standby_plugin` | shared_library | 设备待机插件 |

### 编译依赖

插件编译依赖的外部组件：
```gn
external_deps = [
  "ability_runtime:app_manager",        # 应用管理器
  "c_utils:utils",                       # 通用工具
  "ffrt:libffrt",                        # 快速函数运行时
  "hilog:libhilog",                      # 日志
  "hisysevent:libhisysevent",            # 系统事件
  "hitrace:hitrace_meter",               # 性能跟踪
  "ipc:ipc_single",                      # IPC
  "libxml2:libxml2",                     # XML 解析
  "samgr:samgr_proxy",                   # SA 管理器
]
```

### 安全编译选项

```gn
sanitize = {
  cfi = true              # 控制流完整性
  cfi_cross_dso = true    # 跨 DSO CFI
  debug = false
}
branch_protector_ret = "pac_ret"  # 分支保护
asmflags = [ "-Wl,-z,relro,-z,now" ]  # 重定位保护
cflags_cc = [ "-fstack-protector-strong" ]  # 栈保护
```

### 功能开关

在 `cgroup_sched.gni` 中定义的功能开关：
```gn
if (cgroup_ressched_power_manager_enable) {
  external_deps += [ "power_manager:powermgr_client" ]
  defines += [ "POWER_MANAGER_ENABLE" ]
}
```

## 调试技巧

### 日志输出

```cpp
RESSCHED_LOGI("[MyPlugin] Event received: %{public}d", resType);
```

### 性能分析

使用 HiTrace：
```cpp
#include "res_sched_hitrace_chain.h"
HitraceScoped trace("MyPlugin::DispatchResource");
```

## 常见问题

1. **插件加载失败**：检查 `.z.so` 文件路径和权限
2. **事件未接收**：确认插件开关已启用
3. **性能告警**：优化 DispatchResource 执行时间
